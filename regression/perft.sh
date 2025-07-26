#!/bin/bash

set -euo pipefail

SCRIPT_PATH="${BASH_SOURCE:-$0}"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

# Runs the perft command - we always go from the starting position with depth 7.
PERFT_PATH="${SCRIPT_DIR}"/../build/apps/perft
PERFT_DEPTH=7
PERFT_STRATEGY=$1
PERFT_RESULT=$("${PERFT_PATH}" -y -d ${PERFT_DEPTH} -s ${PERFT_STRATEGY})

# Adds additional metafields to the result and minifies the JSON.
REGRESSION_HARDWARE=$(lscpu | grep 'Model name' | cut -f 2 -d ":" | awk '{$1=$1}1')
REGRESSION_TIMESTAMP=$(date +%s)
REGRESSION_GIT=$(git rev-parse HEAD)
REGRESSION_LABEL=$2
REGRESSION_RESULT=$(echo ${PERFT_RESULT} | jq -c \
    --arg regression_hardware  "${REGRESSION_HARDWARE}" \
    --arg regression_timestamp "${REGRESSION_TIMESTAMP}" \
    --arg regression_git       "${REGRESSION_GIT}"       \
    --arg regression_label     "${REGRESSION_LABEL}"     \
    '. += {"hardware": $regression_hardware, "timestamp": $regression_timestamp, "git": $regression_git, "label": $regression_label}')

# Saves the result to the tracking json-lines file.
REGRESSION_TRACKING_PATH="${SCRIPT_DIR}"/tracking/perft.ndjson
echo ${REGRESSION_RESULT} >> "${REGRESSION_TRACKING_PATH}"
