#!/usr/bin/env bash
#
# Visualization Demo Script
#
# This script demonstrates the advanced visualization capabilities of the NLM system.
#

# Usage
#    ./demo_visualization.sh [options]
#
# Options:
#    --network FILE              Load network configuration from file
#    --activity                    Visualize neural activity
#    --spikes                      Visualize spike raster plot
#    --weights                     Visualize weight matrix
#    --live                        Enable real-time visualization updates
#    --update-rate RATE            Set update rate (default: 30 Hz)
#    --output FORMAT               Output format (png, svg, pdf, web)
#    --port PORT                   Web server port (default: 8080)
#    --help                        Show help message
#

# This script requires a valid brain configuration file.
# If no network file is provided, a default test network will be generated.

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_help() {
    echo "Visualization Demo Script for NLM"
    echo "=================================="
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --network FILE              Load network configuration from file"
    echo "  --activity                    Visualize neural activity"
    echo "  --spikes                      Visualize spike raster plot"
    echo "  --weights                     Visualize weight matrix"
    echo "  --live                        Enable real-time visualization updates"
    echo "  --update-rate RATE            Set update rate (default: 30 Hz)"
    echo "  --output FORMAT               Output format (png, svg, pdf, web)"
    echo "  --port PORT                   Web server port (default: 8080)"
    echo "  --help                        Show this help message"
    echo ""
    echo "Example commands:"
    echo "  $0 --network brain.json --activity --live"
    echo "  $0 --spikes --output svg --port 8080"
    echo "  $0 --weights --output web --help"
    echo ""
}

# Check if cmake is available
if ! command -v cmake &> /dev/null; then
    echo "${RED}Error: CMake is not installed${NC}" >&2
    exit 1
fi

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "${YELLOW}Building NLM project...${NC}"
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    cd .. || exit 1
fi

# Parse command line arguments
NETWORK_FILE=""
VISUALIZE_ACTIVITY=false
VISUALIZE_SPIKES=false
VISUALIZE_WEIGHTS=false
LIVE_MODE=false
UPDATE_RATE=30
OUTPUT_FORMAT="png"
WEB_PORT=8080

while [[ $# -gt 0 ]]; do
    case $1 in
        --network)
            NETWORK_FILE="$2"
            shift 2
            ;;
        --activity)
            VISUALIZE_ACTIVITY=true
            shift
            ;;
        --spikes)
            VISUALIZE_SPIKES=true
            shift
            ;;
        --weights)
            VISUALIZE_WEIGHTS=true
            shift
            ;;
        --live)
            LIVE_MODE=true
            shift
            ;;
        --update-rate)
            UPDATE_RATE="$2"
            shift 2
            ;;
        --output)
            OUTPUT_FORMAT="$2"
            shift 2
            ;;
        --port)
            WEB_PORT="$2"
            shift 2
            ;;
        --help)
            print_help
            exit 0
            ;;
        *)
            echo "${RED}Unknown option: $1${NC}" >&2
            print_help
            exit 1
            ;;
    esac
done

# Build command
cd build

# Add visualization options
VISUALIZATION_ARGS=""

if [ "$VISUALIZE_ACTIVITY" = true ]; then
    VISUALIZATION_ARGS="$VISUALIZATION_ARGS --activity"
fi

if [ "$VISUALIZE_SPIKES" = true ]; then
    VISUALIZATION_ARGS="$VISUALIZATION_ARGS --spikes"
fi

if [ "$VISUALIZE_WEIGHTS" = true ]; then
    VISUALIZATION_ARGS="$VISUALIZATION_ARGS --weights"
fi

if [ "$LIVE_MODE" = true ]; then
    VISUALIZATION_ARGS="$VISUALIZATION_ARGS --live"
fi

VISUALIZATION_ARGS="$VISUALIZATION_ARGS --update-rate $UPDATE_RATE"

# Add network file if specified
if [ -n "$NETWORK_FILE" ]; then
    VISUALIZATION_ARGS="$VISUALIZATION_ARGS --network \"$NETWORK_FILE\""
fi

# Run visualization
if [ "$OUTPUT_FORMAT" = "web" ]; then
    VISUALIZATION_ARGS="$VISUALIZATION_ARGS --web --port $WEB_PORT"
    echo "${YELLOW}Starting web-based visualization server...${NC}"
    echo "You can access the visualization at: http://localhost:$WEB_PORT"
    echo "Press Ctrl+C to stop the server."
    ./nlm visualize $VISUALIZATION_ARGS
else
    echo "${YELLOW}Running visualization with output format: $OUTPUT_FORMAT${NC}"
    ./nlm visualize $VISUALIZATION_ARGS
fi

cd ..