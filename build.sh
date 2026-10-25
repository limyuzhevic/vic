#!/bin/bash

# NLM Build Script
# This script provides convenient access to common build operations for the NLM framework

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== NLM Build Script ===${NC}"

echo -e "${YELLOW}Available commands:${NC}"
echo -e "  build       - Build the project"
echo -e "  build-all   - Build all components including tests"
echo -e "  clean       - Clean build artifacts"
echo -e "  tests       - Run tests"
echo -e "  help        - Show this help message"
echo -e ""

case "$1" in
    build)
        echo -e "${YELLOW}Building NLM project...${NC}"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}NLM built successfully!${NC}"
            echo -e "Executables available in build/:"
            find . -maxdepth 1 -type f -executable -printf "  %f\n" | grep -v CMakeCache.txt
        else
            echo -e "${RED}Build failed!${NC}"
            exit 1
        fi
        ;;

    build-all)
        echo -e "${YELLOW}Building NLM project with all components...${NC}"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON
        make -j$(nproc)
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}NLM built successfully with all components!${NC}"
            echo -e "Executables available in build/:"
            find . -maxdepth 1 -type f -executable -printf "  %f\n" | grep -v CMakeCache.txt
        else
            echo -e "${RED}Build failed!${NC}"
            exit 1
        fi
        ;;

    clean)
        echo -e "${YELLOW}Cleaning build artifacts...${NC}"
        rm -rf build
        rm -rf *.o
        rm -rf *.so
        rm -rf *.dylib
        rm -rf *.dll
        echo -e "${GREEN}Clean completed!${NC}"
        ;;

    tests)
        echo -e "${YELLOW}Running NLM tests...${NC}"
        if [ ! -d build ]; then
            echo -e "${RED}Build directory not found. Running 'build' first.${NC}"
            ./build.sh build
        fi
        cd build
        if [ -f CTestTestfile.cmake ]; then
            ctest --output-on-failure
            if [ $? -eq 0 ]; then
                echo -e "${GREEN}All tests passed!${NC}"
            else
                echo -e "${RED}Some tests failed!${NC}"
                exit 1
            fi
        else
            echo -e "${YELLOW}No tests found. Running integration test...${NC}"
            if [ -f ./integration_test ]; then
                ./integration_test quick
            else
                echo -e "${RED}Integration test not found!${NC}"
                exit 1
            fi
        fi
        ;;

    help)
        echo -e "${YELLOW}NLM Build Script Help${NC}"
        echo -e ""
        echo -e "This script provides convenient access to common build operations for the NLM framework."
        echo -e ""
        echo -e "Available commands:"
        echo -e "  build       - Build the project with default settings"
echo -e "  build-all   - Build the project with all optional features enabled"
echo -e "  clean       - Remove all build artifacts"
echo -e "  tests       - Build and run tests"
echo -e "  help        - Show this help message"
        echo -e ""
        echo -e "Example usage:"
        echo -e "  $0 build         # Build the project"
echo -e "  $0 build-all     # Build with all features"
echo -e "  $0 clean        # Clean build artifacts"
echo -e "  $0 tests        # Build and run tests"
        ;;

    *)
        echo -e "${RED}Unknown command: $1${NC}"
        echo -e "Run $0 help for more information."
        exit 1
        ;;
esac
