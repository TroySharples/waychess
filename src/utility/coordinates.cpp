#include "coordinates.hpp"

std::string to_coordinates_str(std::uint8_t v)
{
    std::string ret(2, ' ');

    // Some hacky ASCII arithmetic.
    ret[0] = 'a' + (static_cast<char>(v) & 07);
    ret[1] = '1' + (static_cast<char>(v) >> 3);

    return ret;
}

std::uint8_t from_coordinates_str(std::string_view v)
{
    // Some hacky ASCII arithmetic.
    return (v[0] - 'a') + ((v[1] - '1') << 3);
}