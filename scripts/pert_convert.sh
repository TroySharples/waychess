#!/bin/bash

# ###############################################################################
# PERFT CONVERTER
#
# Simple utility script that converts the JSON output of our perft application
# into a commonly used format for better compatiblity with third-party tools.
#
# Usage:
#     perft-converter.sh [DEPTH] [FEN]
#
# Example:
#     $ perft-converter.sh 3 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#     a2a3 380
#     b2b3 420
#     c2c3 420
#     d2d3 539
#     e2e3 599
#     f2f3 380
#     g2g3 420
#     h2h3 380
#     a2a4 420
#     b2b4 421
#     c2c4 441
#     d2d4 560
#     e2e4 600
#     f2f4 401
#     g2g4 421
#     h2h4 420
#     b1c3 440
#     g1h3 400
#     b1a3 400
#     g1f3 440
#
#     8902
# ###############################################################################

set -euo pipefail

SCRIPT_PATH="${BASH_SOURCE:-$0}"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

PERFT_PATH="${SCRIPT_DIR}"/../build/apps/perft

# Runs our perft utility and stores the result.
depth=$1
fen=$2
result=$("${PERFT_PATH}" -d "${depth}" -f "${fen}" -t)

# Prints the tree results.
echo "${result}" | jq -r '.tree[] | "\(.move) \(.nodes)"'

# Prints a newline.
echo ""

# Print total nodes at the end.
echo "${result}" | jq -r '.["total-nodes"]'
