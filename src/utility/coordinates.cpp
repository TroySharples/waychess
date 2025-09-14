#include "coordinates.hpp"

std::string to_coordinates_str(std::size_t mb)
{
    std::string ret(2, ' ');

    // Some hacky ASCII arithmetic.
    ret[0] = 'a' + static_cast<char>(mb & 07);
    ret[1] = '1' + static_cast<char>(mb >> 3);

    return ret;
}

std::size_t from_coordinates_str(std::string_view str)
{
    // Some hacky ASCII arithmetic.
    return (str[0] - 'a') + ((str[1] - '1') << 3);
}