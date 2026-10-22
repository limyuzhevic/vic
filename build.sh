#!/bin/bash
# NLM Build Script
# Makes it easy to build and run NLM demos

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
NUM_STEPS=10000
CHECKPOINT="true"
REPLAY="true"
DEVELOPMENT="true"

# Print usage
print_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build and run NLM Phase 6 integration demo

Options:
    --build-type TYPE      Build type: Debug, Release, RelWithDebInfo (default: Release)
    --steps N              Number of simulation steps (default: 10000)
    --checkpoint BOOL      Enable checkpointing (true/false, default: true)
    --replay BOOL          Enable replay (true/false, default: true)
    --development BOOL     Enable development (true/false, default: true)
    --help                 Show this help message

Examples:
    $0                          # Build and run with default settings
    $0 --build-type Debug       # Build and run in debug mode
    $0 --steps 1000 --help      # Build and run with 1000 steps
    $0 --checkpoint false       # Run without checkpointing
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --steps)
            NUM_STEPS="$2"
            shift 2
            ;;
        --checkpoint)
            CHECKPOINT="$2"
            shift 2
            ;;
        --replay)
            REPLAY="$2"
            shift 2
            ;;
        --development)
            DEVELOPMENT="$2"
            shift 2
            ;;
        --help)
            print_usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Convert boolean strings to C++ bool equivalents
CHECKPOINT_BOOL=false
if [[ "$CHECKPOINT" == "true" || "$CHECKPOINT" == "true" || "$CHECKPOINT" == "1" ]]; then
    CHECKPOINT_BOOL=true
fi

REPLAY_BOOL=false
if [[ "$REPLAY" == "true" || "$REPLAY" == "true" || "$REPLAY" == "1" ]]; then
    REPLAY_BOOL=true
fi

DEVELOPMENT_BOOL=false
if [[ "$DEVELOPMENT" == "true" || "$DEVELOPMENT" == "true" || "$DEVELOPMENT" == "1" ]]; then
    DEVELOPMENT_BOOL=true
fi

# Check if build directory exists
if [ ! -d "build" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir -p build
fi

# Configure CMake
cd build

echo -e "${YELLOW}Configuring CMake with type: $BUILD_TYPE${NC}"
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build the project
echo -e "${YELLOW}Building NLM...${NC}"
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

# Run the Phase 6 demo
echo -e "${GREEN}Running NLM Phase 6 demo with $NUM_STEPS steps...${NC}"

# Convert boolean values for command line
CHECKPOINT_STR="$CHECKPOINT_BOOL"
REPLAY_STR="$REPLAY_BOOL"
DEVELOPMENT_STR="$DEVELOPMENT_BOOL"

./nlm_phase6_demo $NUM_STEPS $CHECKPOINT_STR $REPLAY_STR $DEVELOPMENT_STR

echo -e "${GREEN}Demo completed successfully!${NC}"
