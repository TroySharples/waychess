#pragma once

#include <cstdint>
#include <string>

std::string to_coordinates_str(std::uint8_t mb);
std::uint8_t from_coordinates_str(std::string_view str);