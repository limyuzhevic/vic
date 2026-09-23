#!/bin/bash
# NLM Phase 6 Quick Setup Script
# =============================
#
# This script provides a quick setup for running the NLM Phase 6 demo.
# It's designed to help users get started quickly without needing to
# understand all the complex build and configuration options.
#
# Usage:
#   ./quick_setup.sh [OPTION]
#
# Options:
#   build      Build the project and run a basic demo
#   test       Run a quick test to verify the build
#   help       Show help information
#

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_DIR="build"
MAKE_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Usage information
function show_help() {
    echo "=== NLM Phase 6 Quick Setup Script ==="
    echo ""
    echo "This script provides a quick setup for running the NLM Phase 6 demo."
    echo ""
    echo "Available options:"
    echo "  build      Build the project and run a basic demo"
    echo "  test       Run a quick test to verify the build"
    echo "  help       Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./quick_setup.sh build      # Build and run basic demo"
    echo "  ./quick_setup.sh test       # Test if project is already built"
    echo ""
}

# Function to check if build directory exists and has the nlm executable
function check_build() {
    if [ -f "$BUILD_DIR/nlm" ]; then
        echo -e "${GREEN}✓ Found existing build at $BUILD_DIR${NC}"
        return 0
    else
        echo -e "${YELLOW}⚠ No build found at $BUILD_DIR${NC}"
        return 1
    fi
}

# Function to build the project
function build_project() {
    echo -e "${BLUE}Building NLM Phase 6 project...${NC}"
    echo ""
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    echo -e "${YELLOW}Configuring with CMake...${NC}"
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
        echo -e "${RED}✗ CMake configuration failed!${NC}"
        return 1
    fi
    
    # Build with make
    echo -e "${YELLOW}Building with make...${NC}"
    if ! make -j$MAKE_JOBS; then
        echo -e "${RED}✗ Build failed!${NC}"
        return 1
    fi
    
    cd ..
    
    echo -e "${GREEN}✓ Build completed successfully!${NC}"
    echo ""
    echo -e "${YELLOW}You can now run the demo with:${NC}"
    echo "  ./build/nlm              # Basic demo"
    echo "  ./build/nlm_phase6_demo   # Phase 6 integration demo"
    echo "  ./run_nlm_demo.sh basic   # Using the demo script"
    echo ""
}

# Function to run a quick test
function run_test() {
    echo -e "${BLUE}Running quick test...${NC}"
    echo ""
    
    if check_build; then
        echo -e "${GREEN}✓ Build found, running quick test...${NC}"
        echo ""
        
        # Run quick test with the existing build
        echo -e "${YELLOW}Running NLM Phase 6 quick test...${NC}"
        ./"$BUILD_DIR"/nlm --quick
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ Quick test passed!${NC}"
            return 0
        else
            echo -e "${RED}✗ Quick test failed!${NC}"
            return 1
        fi
    else
        echo -e "${YELLOW}⚠ No build found, building...${NC}"
        echo ""
        
        if build_project; then
            echo -e "${GREEN}✓ Running quick test after build...${NC}"
            echo ""
            ./"$BUILD_DIR"/nlm --quick
            
            if [ $? -eq 0 ]; then
                echo -e "${GREEN}✓ Quick test passed!${NC}"
                return 0
            else
                echo -e "${RED}✗ Quick test failed!${NC}"
                return 1
            fi
        else
            echo -e "${RED}✗ Build failed!${NC}"
            return 1
        fi
    fi
}

# Function to run basic demo
function run_basic_demo() {
    echo -e "${BLUE}Running NLM Phase 6 basic demo...${NC}"
    echo ""
    
    if check_build; then
        echo -e "${GREEN}✓ Running basic demo...${NC}"
        echo ""
        
        # Run basic demo
        ./"$BUILD_DIR"/nlm
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ Basic demo completed successfully!${NC}"
            return 0
        else
            echo -e "${RED}✗ Basic demo failed!${NC}"
            return 1
        fi
    else
        echo -e "${YELLOW}⚠ No build found, building...${NC}"
        echo ""
        
        if build_project; then
            echo -e "${GREEN}✓ Running basic demo after build...${NC}"
            echo ""
            ./"$BUILD_DIR"/nlm
            
            if [ $? -eq 0 ]; then
                echo -e "${GREEN}✓ Basic demo completed successfully!${NC}"
                return 0
            else
                echo -e "${RED}✗ Basic demo failed!${NC}"
                return 1
            fi
        else
            echo -e "${RED}✗ Build failed!${NC}"
            return 1
        fi
    fi
}

# Main function
function main() {
    echo "=== NLM Phase 6 Quick Setup Script ==="
    echo ""
    
    case "$1" in
        build)
            run_basic_demo
            ;;
        test)
            run_test
            ;;
        help|--help|-h)
            show_help
            ;;
        *)
            show_help
            echo -e "${YELLOW}Usage: $0 [build|test|help]${NC}"
            exit 1
            ;;
    esac
}

# Run main function
main "$@"