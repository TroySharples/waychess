#!/bin/bash

set -euo pipefail

SCRIPT_PATH="${BASH_SOURCE:-$0}"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

BIN_DIR="${SCRIPT_DIR}"/../build/apps
EVALUATE_PATH="${BIN_DIR}"/waychess-evaluate
PRINT_PATH="${BIN_DIR}"/waychess-print

fen=$1
depth=$2
side=$3

# Start by printing the board.
${PRINT_PATH} -f "${fen}" -t unicode -o ${side}

while :
do
    if [[ "${side}" == "$(${PRINT_PATH} -f "${fen}" -t side)" ]]; then
        # It's our turn to play.
        move=$(${EVALUATE_PATH} -f "${fen}" -d ${depth} | jq -r '.["recommendation"]' )
        echo "Move: ${move}"
        fen=$(${PRINT_PATH} -f "${fen}" -m ${move})
    else
        # It's the user's turn to play.
        echo -n "Enter move: "
        read move
        fen=$(${PRINT_PATH} -f "${fen}" -m ${move})
    fi

    # Prints the board and FEN after every ply.
    ${PRINT_PATH} -f "${fen}" -t unicode -o ${side}
    echo "${fen}"
done

