#include "piece.hpp"


std::ostream& operator<<(std::ostream& os, const sliding_attack_table& v)
{
    // This function is for serialising a number of C++ functions that are used for recreating the attack
    // table. The idea is for the caller of this serialise function to wrap this in a namespace to uniquely
    // determine the type of attack table. Note that the memory buffer for these attack tables is separately
    // serialised to allow for greater flexibility in memory layouts.

    os << "/* !!!AUTO-GENERATED CODE!!! */\n";

    std::vector<std::uint64_t> buf;
    for (const auto& i : v)
        buf.insert(buf.end(), i.table.get_table().begin(), i.table.get_table().end());
    
    os << "constexpr std::array<std::uint64_t, " << buf.size() << "> sliding_attack_table_buf { ";
    if (!buf.empty())
        os << buf[0];
    for (auto i : buf)
        os << ", " << i;
    os << " };\n";

    os << "constexpr sliding_attack_table sliding_attack_table {\n";
    std::size_t offset { 0 };
    for (const auto& i : v)
    {
        os << " { .table = { { sliding_attack_table_buf.data() + " << offset << ", " << i.table.get_table().size() << " }, " << i.table.get_magic() << ", " << i.table.get_rshift() << " }, " << i.notmask << " } }\n ,";
        offset += i.table.get_table().size();
    }
    os << "\r};\n";

    return os;
}

