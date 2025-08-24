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

# Basic depth-7 perft across all strategies and various hash-sizes.
for strategy in copy unmake; do
    for hash_mb in 0 20 100 500 2000; do
        run "${RUN_PERFT_PATH}" "${strategy}" 7 "${hash_mb}" "${SUITE_LABEL}"
    done
done

# Deep depth-8 perft with fewer hash-sizes. We omit the case of no hash table
# as it just takes too long...
for strategy in copy unmake; do
    for hash_mb in 50 500 4000; do
        run "${RUN_PERFT_PATH}" "${strategy}" 8 "${hash_mb}" "${SUITE_LABEL}"
    done
done

# ###############################################################################
# EVALUATE SUITE
# ###############################################################################

RUN_EVALUATE_PATH="${SCRIPT_DIR}"/evaluate.sh

# Basic depth-6 evaluate (going deeper would take too long for non alpha/beta)
# across all searches. We use no hash here, as only negamax alpha/beta implements
# it.
for search in minimax negamax negamax-prune; do
    run "${RUN_EVALUATE_PATH}" "${search}" 6 0 "${SUITE_LABEL}"
done

# Focus on alpha/beta and go deeper across a variety of hash-sizes.
for depth in 7 8; do
    for hash_mb in 0 50 500 4000; do
        run "${RUN_EVALUATE_PATH}" "negamax-prune" "${depth}" "${hash_mb}" "${SUITE_LABEL}"
    done
done

# ###############################################################################
# PUZZLER SUITE
# ###############################################################################

RUN_PUZZLER_PATH="${SCRIPT_DIR}"/puzzler.sh
PUZZLE_DIR="${SCRIPT_DIR}"/../puzzles

# Basic depth-4 solve (going deeper would take too long for non alpha/beta)
# across all searches and all puzzle files. We use no hash here, as only negamax
# alpha/beta implements it.
for file in mate500 endgame500; do
    for search in minimax negamax negamax-prune; do
        run "${RUN_PUZZLER_PATH}" "${PUZZLE_DIR}/${file}.csv" "${search}" 4 0 "${SUITE_LABEL}"
    done
done

# Focus on alpha/beta and go deeper across a variety of hash-sizes.
for file in mate500 endgame500; do
    for depth in 4 5; do
        for hash_mb in 0 50 500 4000; do
            run "${RUN_PUZZLER_PATH}" "${PUZZLE_DIR}/${file}.csv" "negamax-prune" "${depth}" "${hash_mb}" "${SUITE_LABEL}"
        done
    done
done
