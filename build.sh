#!/usr/bin/env bash
# NLM Build Script

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_banner() {
    echo -e "${GREEN}================================================${NC}"
    echo -e "${GREEN}NLM Build Script${NC}"
    echo -e "${GREEN}================================================${NC}"
    echo ""
}

check_project_structure() {
    echo -e "${YELLOW}Checking project structure...${NC}"
    
    if [ ! -f "CMakeLists.txt" ]; then
        echo -e "${RED}ERROR: CMakeLists.txt not found. Are you in the NLM project root?${NC}"
        exit 1
    fi
    
    if [ ! -f "README.md" ]; then
        echo -e "${RED}ERROR: README.md not found. Are you in the NLM project root?${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Project structure is valid${NC}"
}

build_project() {
    echo -e "${YELLOW}Building NLM project...${NC}"
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with CMake
    echo "Configuring with CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}CMake configuration failed${NC}"
        echo "Try: cmake .. -DCMAKE_BUILD_TYPE=Release"
        exit 1
    fi
    
    # Build with make
    echo "Building with make..."
    make -j4
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Build failed${NC}"
        echo "Try: make"
        exit 1
    fi
    
    cd ..
    
    echo -e "${GREEN}✓ Project built successfully${NC}"
    
    # List built executables
    echo ""
    echo "Built executables:"
    for exe in build/nlm*; do
        if [ -f "$exe" ]; then
            echo "  $exe"
        fi
    done
    
    echo ""
    echo "To run the Phase 6 integration demo:"
    echo "  ./build/nlm_phase6_demo"
    echo ""
    echo "To run the basic neural computation demo:"
    echo "  ./build/nlm"
    echo ""
    echo "To run examples:"
    echo "  cd examples && ./hello"
}

main() {
    print_banner
    check_project_structure
    build_project
    
    echo -e "${GREEN}================================================${NC}"
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo -e "${GREEN}================================================${NC}"
}

main "$@"
