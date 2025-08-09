#pragma once

#include "evaluate_raw_material.hpp"

namespace evaluation
{

// A generic terminal evaluation signature. The behaviour of the evaluation heuristic may be changed
// with the args pointer. This must return an absolute centipawn evaluation (i.e. not relative to the
// playing side).
using evaluation = int (*)(const bitboard& bb, const void* args);

constexpr evaluation raw_material { &evaluate_raw_material };

}