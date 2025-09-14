#pragma once

#include <ios>
#include <iostream>

namespace config
{

constexpr bool nmp { true };
constexpr bool lmr { true };

inline std::ostream& print_json(std::ostream& os)
{
    return os << R"({)" << '\n'
              << R"(    "nmp": )"   << std::boolalpha << nmp << std::noboolalpha << ",\n"
              << R"(    "lmr": )"   << std::boolalpha << lmr << std::noboolalpha << '\n'
              << R"(})" << '\n';
}

inline std::ostream& print_tv(std::ostream& os)
{
    return os << "nmp="<< std::boolalpha << nmp << std::noboolalpha << ','
              << "lmr="<< std::boolalpha << lmr << std::noboolalpha;
}

}