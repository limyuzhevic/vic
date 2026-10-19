#!/bin/bash

# NLM CLI Script Wrapper
# This script provides a convenient way to run the NLM CLI with common options

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
NLM_CLI="${SCRIPT_DIR}/build/bin/nlm-cli"

if [ ! -f "${NLM_CLI}" ]; then
    echo "Error: NLM CLI not built. Run 'cmake --build build' first."
    exit 1
fi

# Parse common options
FORMAT="pretty"
CONFIG_FILE="configs/default.cfg"
SCRIPT_MODE=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --format)
            FORMAT="$2"
            shift 2
            ;;
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --script|-s)
            SCRIPT_MODE="--script"
            shift
            ;;
        --help|-h)
            "${NLM_CLI}" "$@"
            exit $?
            ;;
        --version)
            "${NLM_CLI}" --version
            exit $?
            ;;
        *)
            break
            ;;
    esac
done

# Build final command
CMD="${NLM_CLI} ${SCRIPT_MODE}"
CMD+=" --format ${FORMAT}"
CMD+=" --config ${CONFIG_FILE}"

# Execute remaining arguments
if [ "$#" -gt 0 ]; then
    CMD+=" "
    CMD+="$@"
fi

echo "Executing: ${CMD}"
${CMD}