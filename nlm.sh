#!/bin/bash
# nlm.sh - Quick start script for NLM (Neural Learning Machine)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function print_header() {
    echo -e "${GREEN}=== NLM (熙然) - Neural Learning Machine ===${NC}"
    echo -e "${YELLOW}Phase-based artificial developmental brain simulation${NC}"
    echo
}

function print_usage() {
    echo -e "${YELLOW}Usage:${NC}"
    echo "  $0 [options]"
    echo
    echo -e "${YELLOW}Options:${NC}"
    echo "  --phase 2    Run Phase 2: Basic neural computation"
    echo "  --phase 4    Run Phase 4: Advanced brain systems"
    echo "  --phase 5    Run Phase 5: Performance optimized"
    echo "  --phase 6    Run Phase 6: Full integration (default)"
    echo "  --config FILE   Load configuration from FILE"
    echo "  --help        Show this help message"
    echo "  --version     Show version information"
    echo
    echo -e "${YELLOW}Examples:${NC}"
    echo "  $0                    # Run Phase 6 demo (default)"
    echo "  $0 --phase 2          # Run Phase 2 demo"
    echo "  $0 --phase 6 --config myconfig.cfg"
    echo "  $0 --help             # Show help"
}

function print_version() {
    echo -e "${GREEN}NLM (熙然) - Neural Learning Machine${NC}"
    echo -e "Version: 0.1.0 (Phase 6.0)"
    echo -e "Built: $(date)"
    echo
    echo -e "Key Features:"
    echo -e "  ✓ Leaky Integrate-and-Fire (LIF) neuron dynamics"
    echo -e "  ✓ Event-driven spike propagation with delays"
    echo -e "  ✓ STDP and Hebbian plasticity rules"
    echo -e "  ✓ Structural plasticity (synaptogenesis/pruning)"
    echo -e "  ✓ Full memory system integration"
    echo -e "  ✓ Neuromodulation systems"
    echo -e "  ✓ Prediction and cognition"
    echo -e "  ✓ Development system"
    echo -e "  ✓ Checkpoint and replay"
}

# Parse command line arguments
PHASE="6"
CONFIG="configs/default.cfg"

while [[ $# -gt 0 ]]; do
    case $1 in
        --phase*)
            PHASE="${1#--phase}"
            shift
            ;;
        --config)
            CONFIG="$2"
            shift 2
            ;;
        --help)
            print_usage
            exit 0
            ;;
        --version)
            print_version
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Validate phase number
if [[ ! "$PHASE" =~ ^[2-6]$ ]]; then
    echo -e "${RED}Error: Invalid phase number '$PHASE'. Must be 2, 4, 5, or 6.${NC}"
    exit 1
fi

print_header

echo -e "${YELLOW}Running Phase $PHASE demo...${NC}"
echo "Configuration: $CONFIG"
echo

# Set execute permissions on the main binary if it exists
if [[ -f "build/nlm" ]]; then
    EXECUTABLE="build/nlm"
elif [[ -f "./nlm" ]]; then
    EXECUTABLE="./nlm"
else
    echo -e "${RED}Error: NLM executable not found. Building...${NC}"
    echo -e "${YELLOW}Note: Building may take several minutes...${NC}"
    
    # Check if we have CMake and build tools
    if command -v cmake &> /dev/null && command -v make &> /dev/null; then
        mkdir -p build
        cd build
        cmake ..
        make -j4
        cd ..
        EXECUTABLE="build/nlm"
    else
        echo -e "${RED}Error: CMake or make not found. Please install build tools.${NC}"
        echo -e "${YELLOW}You can install them with:${NC}"
        echo "  sudo apt update && sudo apt install build-essential cmake"
        exit 1
    fi
fi

# Run the NLM demo
if [[ -f "$EXECUTABLE" ]]; then
    echo -e "${GREEN}Running NLM...${NC}"
    echo "This may take a few minutes depending on the phase and configuration."
    echo
    
    # Set timeout for the executable (10 minutes)
    timeout 600 "$EXECUTABLE" --phase "$PHASE" --config "$CONFIG"
    EXIT_CODE=$?
    
    if [[ $EXIT_CODE -eq 0 ]]; then
        echo
        echo -e "${GREEN}=== NLM completed successfully ===${NC}"
    elif [[ $EXIT_CODE -eq 124 ]]; then
        echo
        echo -e "${YELLOW}=== NLM timed out (10 minutes). This is normal for Phase 6 with large configurations. ===${NC}"
        echo -e "Try reducing the configuration size or using Phase 2/4/5 for faster demos."
    else
        echo
        echo -e "${RED}=== NLM failed with exit code $EXIT_CODE ===${NC}"
        exit $EXIT_CODE
    fi
else
    echo -e "${RED}Error: NLM executable not found after building.${NC}"
    exit 1
fi
