#pragma once

#include <ios>
#include <iostream>

namespace config
{

constexpr bool eval_ks { true };
constexpr bool eval_ps { true };

constexpr bool nmp   { true };
constexpr bool lmr   { true };
constexpr bool km    { true };
constexpr bool hh    { false };
constexpr bool see   { true };
constexpr bool scout { true };

// The initial delta of the aspiration window - 0 if we shouldn't use aspiration windows in the search.
constexpr int awd { 35 };

inline std::ostream& print_json(std::ostream& os)
{
    return os << R"({)"   << '\n'
    << R"(    "eval_ks": )" << std::boolalpha << eval_ks << std::noboolalpha << ",\n"
    << R"(    "eval_ps": )" << std::boolalpha << eval_ps << std::noboolalpha << ",\n"
    << R"(    "nmp": )"     << std::boolalpha << nmp     << std::noboolalpha << ",\n"
    << R"(    "lmr": )"     << std::boolalpha << lmr     << std::noboolalpha << ",\n"
    << R"(    "km": )"      << std::boolalpha << km      << std::noboolalpha << ",\n"
    << R"(    "hh": )"      << std::boolalpha << hh      << std::noboolalpha << ",\n"
    << R"(    "see": )"     << std::boolalpha << see     << std::noboolalpha << ",\n"
    << R"(    "scout": )"   << std::boolalpha << scout   << std::noboolalpha << ",\n"
    << R"(    "awd": )"     << awd << '\n'
    << R"(})" << '\n';
}

inline std::ostream& print_tv(std::ostream& os)
{
    return os << "eval_ks=" << std::boolalpha << eval_ks << std::noboolalpha << ','
              << "eval_ps=" << std::boolalpha << eval_ps << std::noboolalpha << ','
              << "nmp="     << std::boolalpha << nmp     << std::noboolalpha << ','
              << "lmr="     << std::boolalpha << lmr     << std::noboolalpha << ','
              << "km="      << std::boolalpha << km      << std::noboolalpha << ','
              << "hh="      << std::boolalpha << hh      << std::noboolalpha << ','
              << "see="     << std::boolalpha << see     << std::noboolalpha << ','
              << "scout="   << std::boolalpha << scout   << std::noboolalpha << ','
              << "awd="     << awd;
}

}