#!/bin/bash

# NLM Command Line Interface Wrapper Script
# 
# This script provides a convenient way to run the NLM CLI tool with various options.
# It sets up the environment and handles errors gracefully.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Add the project root to the build directory
BUILD_DIR="$PROJECT_ROOT/build"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory not found at $BUILD_DIR"
    echo "Please run 'mkdir build && cd build && cmake ..' first"
    exit 1
fi

# Check if nlm-cli executable exists
if [ ! -f "$BUILD_DIR/nlm-cli" ]; then
    echo "Error: nlm-cli executable not found at $BUILD_DIR/nlm-cli"
    echo "Please build the project first with 'make'"
    exit 1
fi

# Parse command line arguments
MODE="phase6"
NEURONS="500"
STEPS="2000"
SEED="42"
OUTPUT=""
QUIET=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --mode)
            MODE="$2"
            shift 2
            ;;
        --neurons)
            NEURONS="$2"
            shift 2
            ;;
        --steps)
            STEPS="$2"
            shift 2
            ;;
        --seed)
            SEED="$2"
            shift 2
            ;;
        --output)
            OUTPUT="$2"
            shift 2
            ;;
        --quiet)
            QUIET=true
            shift
            ;;
        --help|-h)
            echo "Usage: nlm-cli [options]"
            echo ""
            echo "Options:"
            echo "  --mode <mode>          Demo mode: phase2, phase6, verify (default: phase6)"
            echo "  --neurons <n>          Number of neurons (default: 500)"
            echo "  --steps <n>            Number of simulation steps (default: 2000)"
            echo "  --seed <n>             Random seed (default: 42)"
            echo "  --output <file>        Output results to file"
            echo "  --quiet                Quiet mode (minimal output)"
            echo "  --help                 Show this help message"
            echo ""
            echo "Examples:"
            echo "  nlm-cli --mode phase2 --neurons 1000"
            echo "  nlm-cli --mode phase6 --steps 5000 --output results.txt"
            echo "  nlm-cli --mode verify"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Build command arguments
CMD_ARGS=""
[ "$MODE" != "phase6" ] && CMD_ARGS="--mode $MODE"
[ "$NEURONS" != "500" ] && CMD_ARGS="$CMD_ARGS --neurons $NEURONS"
[ "$STEPS" != "2000" ] && CMD_ARGS="$CMD_ARGS --steps $STEPS"
[ "$SEED" != "42" ] && CMD_ARGS="$CMD_ARGS --seed $SEED"
[ -n "$OUTPUT" ] && CMD_ARGS="$CMD_ARGS --output $OUTPUT"

if [ "$QUIET" = true ]; then
    CMD_ARGS="$CMD_ARGS --quiet"
fi

# Execute nlm-cli

echo "Running NLM CLI with arguments: $CMD_ARGS"
echo "Executable: $BUILD_DIR/nlm-cli"
echo ""

"$BUILD_DIR/nlm-cli" $CMD_ARGS

EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
    echo ""
    echo "=== Execution completed successfully ==="
else
    echo ""
    echo "=== Execution failed with exit code: $EXIT_CODE ==="
fi

exit $EXIT_CODE
