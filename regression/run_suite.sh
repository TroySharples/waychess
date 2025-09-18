#!/bin/bash

set -euo pipefail

SCRIPT_PATH="${BASH_SOURCE:-$0}"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

SUITE_LABEL=$1

# We run each command in this function for better logging - needed to monitor the
# progress through this potentially very long test.
run() {
    echo "[RUNNING] $*"
    "$@"
}

# ###############################################################################
# PERFT SUITE
# ###############################################################################

RUN_PERFT_PATH="${SCRIPT_DIR}"/perft.sh

# Basic depth-7 perft across various hash-sizes.
for hash_mb in 1 2 5 10 20 50 100 200 500 1000 2000 4000; do
    run "${RUN_PERFT_PATH}" 7 "${hash_mb}" "${SUITE_LABEL}"
done

# Deep depth-8 perft with fewer hash-sizes.
for hash_mb in 50 500 4000; do
    run "${RUN_PERFT_PATH}" 8 "${hash_mb}" "${SUITE_LABEL}"
done

# ###############################################################################
# EVALUATE SUITE
# ###############################################################################

RUN_EVALUATE_PATH="${SCRIPT_DIR}"/evaluate.sh

# High-depth across various hash-sizes.
for depth in 12 13; do
    for hash_mb in 50 500 4000; do
        run "${RUN_EVALUATE_PATH}" "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" "${depth}" "${hash_mb}" "${SUITE_LABEL}"
    done
done

# ###############################################################################
# PUZZLER SUITE
# ###############################################################################

RUN_PUZZLER_PATH="${SCRIPT_DIR}"/puzzler.sh
PUZZLE_DIR="${SCRIPT_DIR}"/../puzzles

# Medium-depth solve across all puzzle files and a variety of hash-sizes.
for file in mate500 endgame500; do
    for depth in 9 10; do
        for hash_mb in 50 500 4000; do
            run "${RUN_PUZZLER_PATH}" "${PUZZLE_DIR}/${file}.csv" "${depth}" "${hash_mb}" "${SUITE_LABEL}"
        done
    done
done
