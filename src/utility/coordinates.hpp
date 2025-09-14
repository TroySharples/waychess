#pragma once

#include <string>

std::string to_coordinates_str(std::size_t mb);
std::size_t from_coordinates_str(std::string_view str);