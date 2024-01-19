#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <tobuso/matrix.hpp>

namespace fs = std::filesystem;

double color_to_double(sf::Color color);
double convert_pixel_to_double(const sf::Uint8* pixels);
tbs::Matrix process_image(fs::path image_path);
tbs::Matrix process_image_name(fs::path filename);