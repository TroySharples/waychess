#pragma once

#include <string_view>
#include <iostream>

enum class log_level  { critical=0, error=1, warning=2, notice=3, informational=4, debug=5 };
enum class log_method { none, cout, cerr, uci };

constexpr const char* to_string(log_level level)
{
    switch (level)
    {
        case log_level::critical:      return "critical";
        case log_level::error:         return "error";
        case log_level::warning:       return "warning";
        case log_level::notice:        return "notice";
        case log_level::informational: return "informational";
        case log_level::debug:         return "debug";
        default: throw std::invalid_argument("Unknown log-level");
    }
}
inline std::ostream& operator<<(std::ostream& os, const log_level& v) { return os << to_string(v); }

namespace details
{

void log_info_impl(std::string_view str, log_level level);

}

// Sets our log method - default to none.
void set_log_method(log_method method);

// Just call the implementation after comparing against our compile-time log-level - this will
// hopefully give the compiler the opportunity to more easily optimise out the call to log in the
// first place.
inline void log(std::string_view str, log_level level)
{
    if (static_cast<int>(level) <= LOG_LEVEL)
        details::log_info_impl(str, level);
}