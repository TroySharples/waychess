#include "logging.hpp"

#include "utility/uci.hpp"

#include <iostream>
#include <iomanip>

namespace details
{

namespace
{

void log_none(std::string_view /*str*/, log_level /*level*/)
{
    // Do nothing.
}

// TODO: Make thread-safe. Some fancy colour-based formating + timestamps for cout and cerr logs.

void log_cout(std::string_view str, log_level level)
{
    std::cout << level << " - " << str << std::endl;
}

void log_cerr(std::string_view str, log_level level)
{
    std::cerr << level << " - " << str << std::endl;
}

void log_uci(std::string_view str, log_level /*level*/)
{
    uci::command_info c;
    c.info = str;
    c.print(std::cout);
}

void (*logger)(std::string_view, log_level) { &log_none };

}

void log_info_impl(std::string_view str, log_level level)
{
    logger(str, level);
}

}

void set_log_method(log_method method)
{
    switch (method)
    {
        case log_method::none: details::logger = &details::log_none; break;
        case log_method::cout: details::logger = &details::log_cout; break;
        case log_method::cerr: details::logger = &details::log_cerr; break;
        case log_method::uci:  details::logger = &details::log_uci;  break;
    }
}