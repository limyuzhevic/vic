#!/bin/bash

# NLM Phase 6 Demo Runner Script
# Simple script to build and run the improved Phase 6 integration demo

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function print_header() {
    echo -e "${GREEN}=== NLM Phase 6 Demo Runner ===${NC}"
    echo "Improved integration test with advanced features"
    echo "=============================================${NC}"
    echo
}

function print_help() {
    print_header
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  -h, --help                    Show this help message"
    echo "  --build-only                  Only build the demo, don't run it"
    echo "  --quick-run                   Run quick test with minimal settings"
    echo "  --demo-mode                   Run demonstration mode with examples"
    echo
    echo "Examples:"
    echo "  $0                           Build and run demo with default settings"
    echo "  $0 --build-only              Build only, no execution"
    echo "  $0 --quick-run               Run quick test (faster, less output)"
    echo
}

function build_project() {
    echo -e "${YELLOW}Building NLM project...${NC}"
    
    if [ ! -d "build" ]; then
        mkdir -p build
        echo "Created build directory"
    fi
    
    cd build
    
    # Configure with CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release
    echo "Configured project with Release build type"
    
    # Build all targets
    make -j4
    echo "Built all targets successfully"
    
    # Check if Phase 6 demo was built
    if [ -f "nlm_phase6_demo" ]; then
        echo -e "${GREEN}✓ Phase 6 demo executable built successfully${NC}"
    else
        echo -e "${RED}✗ Phase 6 demo executable not found${NC}"
        echo "Checking available targets..."
        make help || make targets
        exit 1
    fi
    
    cd ..
}

function run_quick_test() {
    echo -e "${YELLOW}Running quick Phase 6 test...${NC}"
    echo "This will run a fast integration test with minimal settings."
    echo
    
    ./build/nlm_phase6_demo -n 100 -s 500 --no-checkpointing --no-replay
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Quick test completed successfully${NC}"
    else
        echo -e "${RED}✗ Quick test failed${NC}"
        exit 1
    fi
}

function run_demo() {
    echo -e "${YELLOW}Running Phase 6 demo with default settings...${NC}"
    echo "This will run a comprehensive integration test."
    echo "You can see all the new features in action."
    echo
    
    ./build/nlm_phase6_demo
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Demo completed successfully${NC}"
    else
        echo -e "${RED}✗ Demo failed${NC}"
        exit 1
    fi
}

function run_demo_with_examples() {
    echo -e "${YELLOW}Running Phase 6 demo with examples...${NC}"
    echo "Showing different configuration examples."
    echo
    
    echo "=== Example 1: Basic Configuration ==="
    ./build/nlm_phase6_demo -n 200 -s 1000
    
    echo -e "${YELLOW}=== Example 2: Advanced Features ===${NC}"
    ./build/nlm_phase6_demo -n 300 -s 1500 --enable-curiosity --enable-reward --enable-structural
    
    echo -e "${YELLOW}=== Example 3: Fast Run without Checkpointing ===${NC}"
    ./build/nlm_phase6_demo -n 150 -s 800 --no-checkpointing --no-replay
}

# Parse command line arguments
BUILD_ONLY=false
QUICK_RUN=false
DEMO_MODE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            print_help
            exit 0
            ;;
        --build-only)
            BUILD_ONLY=true
            shift
            ;;
        --quick-run)
            QUICK_RUN=true
            shift
            ;;
        --demo-mode)
            DEMO_MODE=true
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_help
            exit 1
            ;;
    esac
done

# Main execution
print_header

if [ "$BUILD_ONLY" = true ]; then
    build_project
    echo
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo "You can run the demo with: ./build/nlm_phase6_demo"
    exit 0
fi

# Build the project
build_project

# Run the demo based on mode
if [ "$QUICK_RUN" = true ]; then
    run_quick_test
elif [ "$DEMO_MODE" = true ]; then
    run_demo_with_examples
else
    run_demo
fi

echo
print_header
echo -e "${GREEN}All tasks completed successfully!${NC}"
echo
echo "The Phase 6 integration demo has been successfully improved with:"
echo "  ✓ Fixed Phase inconsistencies"
echo "  ✓ Enhanced command-line interface"
echo "  ✓ Configuration validation"
echo "  ✓ Advanced features (curiosity, reward, structural plasticity)"
echo "  ✓ Better error handling"
echo "  ✓ Comprehensive documentation"
echo
echo "For more information, see IMPROVEMENTS_SUMMARY.md"
