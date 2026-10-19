#!/usr/bin/env bash

# NLM Development Setup Script
# This script helps users set up and run the improved NLM system

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== NLM (Neural Learning Machine) Setup Script ===${NC}"
echo "This script will help you set up and run the improved NLM system."

echo -e "${YELLOW}Checking system requirements...${NC}"

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: CMakeLists.txt not found. Please run this script from the NLM root directory.${NC}"
    exit 1
fi

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake not found. Please install CMake (version 3.16 or higher).${NC}"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo -e "${RED}Error: Make not found. Please install Make.${NC}"
    exit 1
fi

echo -e "${GREEN}✓ All system requirements met${NC}"

# Build options
echo -e "${YELLOW}Build Options:${NC}"
echo "1. Build everything (default)"
echo "2. Build only tests"
echo "3. Build Python bindings only"
echo "4. Build demo executable"
echo "5. Quick setup (build + run demo)"

echo -n "Enter your choice (1-5): "
read choice

case $choice in
    1)
        echo -e "${YELLOW}Building complete NLM system...${NC}"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
        echo -e "${GREEN}✓ Build complete${NC}"
        echo -e "${YELLOW}To run the demo, execute:${NC}"
        echo -e "  ./nlm_phase6_demo"
        ;;

    2)
        echo -e "${YELLOW}Building tests only...${NC}"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
        make -j4 nlm_test
        echo -e "${GREEN}✓ Tests built${NC}"
        echo -e "${YELLOW}To run tests:${NC}"
        echo -e "  ctest --output-on-failure"
        ;;

    3)
        echo -e "${YELLOW}Building Python bindings...${NC}"
        if [ -f "requirements.txt" ]; then
            echo -e "${YELLOW}Installing Python dependencies...${NC}"
            pip install -r requirements.txt
        else
            echo -e "${YELLOW}No requirements.txt found. Installing pynlm...${NC}"
            pip install -e .
        fi
        echo -e "${GREEN}✓ Python bindings installed${NC}"
        ;;

    4)
        echo -e "${YELLOW}Building demo executable...${NC}"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4 nlm_phase6_demo
        echo -e "${GREEN}✓ Demo executable built${NC}"
        ;;

    5)
        echo -e "${YELLOW}Quick setup: building and running demo...${NC}"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4 nlm_phase6_demo
        echo -e "${YELLOW}Running demo...${NC}"
        ./nlm_phase6_demo
        ;;

    *)
        echo -e "${RED}Invalid choice. Please run the script again and choose 1-5.${NC}"
        exit 1
        ;;
esac

echo -e "${GREEN}=== Setup Complete ===${NC}"
echo ""
echo -e "${YELLOW}Next Steps:${NC}"
echo "1. Run './nlm_cli.py --help' for CLI options"
echo "2. Check the documentation in docs/USER_GUIDE.md"
echo "3. Use the Python examples in the source code"
echo "4. Run tests with 'ctest --output-on-failure' in the build directory"
echo ""
echo -e "${YELLOW}For more information, see:${NC}"
echo "- docs/USER_GUIDE.md (Complete user guide)"
echo "- CODE_IMPROVEMENTS.md (Summary of improvements)"
echo "- examples/ (Additional usage examples)"
