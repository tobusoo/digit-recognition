#include <fstream>
#include <iostream>

#include <ImageDataSet.hpp>
#include <tobuso/nn.hpp>

#include <sys/time.h>

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

int main(int argc, char* argv[])
{
    srand(time(0));

    if (argc < 5) {
        std::cerr << "Usage: ./train <num of train> <num of iter> <max epoch> "
                     "<1(load model) or 0(don't load model)>\n";
        return 1;
    }
    size_t iter = std::atoi(argv[2]);
    size_t max_epoch = std::atoi(argv[3]);
    bool need_load_model = std::atoi(argv[4]);

    tbs::NN* nn;
    tbs::NN* g;
    if (need_load_model) {
        std::cout << "Loading NN model...\n";
        std::string str_nn = "model/nn_" + std::string(argv[1]) + ".json";
        std::ifstream file_nn(str_nn);
        tbs::json model = tbs::json::parse(file_nn);
        nn = new tbs::NN(model);
        g = new tbs::NN(model);
        assert(nn && g);
        std::cout << "NN model is loaded!\n\n";
    } else {
        size_t input_size = 28 * 28;
        tbs::Layers layers = {
                {128, tbs::SIGMOID},
                {36, tbs::SIGMOID},
                {10, tbs::SIGMOID},
        };
        nn = new tbs::NN(input_size, layers);
        g = new tbs::NN(input_size, layers);
        assert(nn && g);

        nn->rand(-1, 1);
    }

    std::cout << "Loading DataSet...\n";
    ImageDataSet dataset;
    dataset.load_images("dataset/train", 100, 600);
    std::cout << "DataSet is loaded!\n\n";
    size_t batch_size = dataset.get_batch_count();

    std::string out_filename("visualization/output_");
    out_filename += argv[1];
    out_filename += ".data";
    std::ofstream file(out_filename, std::ios::app);

    double rate = 1;
    size_t j = 0;

    double time = wtime();
    double elapsed_time = 0;
    for (size_t i = iter; i <= max_epoch; i++, j++) {
        if (j == batch_size) {
            j = 0;
        }

        auto dt = dataset.get_batch(j);
        nn->backprop(*g, dt.inputs, dt.outputs, rate);
        elapsed_time = wtime() - time;
        double cost = nn->cost(dt.inputs, dt.outputs);
        printf("[%s] %ld: cost = %f; elapsed time: %f\n", argv[1], i, cost, elapsed_time);
        file << i << ' ' << cost << ' ' << elapsed_time << '\n';
    }

    std::cout << "Saving nn...\n";
    tbs::json nn_json = nn->save();

    std::string nn_file = "model/nn_" + std::string(argv[1]) + ".json";
    std::ofstream f(nn_file);

    f << nn_json;
    std::cout << "Saved nn\n";

    return 0;
}