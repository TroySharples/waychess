#pragma once

#include "evaluate_raw_material.hpp"

namespace evaluation
{

// A generic terminal evaluation signature. This must return an absolute centipawn
// evaluation (i.e. not relative to the playing side).
using evaluation = int (*)(const bitboard& bb);

constexpr evaluation raw_material { &evaluate_raw_material };

}