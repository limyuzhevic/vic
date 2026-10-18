#!/usr/bin/env bash

# NLM Build and Test Script
# This script provides easy access to common build and test operations

set -e

cd "$(dirname "$0")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_usage() {
    echo "NLM Build and Test Script"
    echo "==========================="
    echo ""
    echo "Usage: $0 [command] [options]"
    echo ""
    echo "Commands:"
    echo "  build              Build the NLM project"
    echo "  test               Run the main test suite"
    echo "  quick-test         Run quick tests (no args)"
    echo "  docs               Generate documentation"
    echo "  install            Install Python bindings"
    echo "  clean              Clean build artifacts"
    echo "  help               Show this help message"
    echo ""
    echo "Options:"
    echo "  --verbose          Enable verbose output"
    echo "  --quick            Skip long-running tests"
    echo "  --config <file>    Use custom configuration file"
    echo ""
    echo "Examples:"
    echo "  $0 build --verbose"
    echo "  $0 quick-test"
    echo "  $0 test --config my_config.cfg"
}

build_project() {
    local verbose="$1"
    
    echo -e "${YELLOW}Building NLM project...${NC}"
    
    if [ ! -d "build" ]; then
        mkdir -p build
    fi
    
    cd build
    
    # Configure with CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build with parallel jobs
    make -j$(nproc)
    
    if [ "$verbose" = "--verbose" ]; then
        echo -e "${GREEN}Build completed successfully!${NC}"
        echo "Binaries are in: $(pwd)"
    fi
    
    cd ..
}

test_project() {
    local verbose="$1"
    local quick="$2"
    local config_file="$3"
    
    echo -e "${YELLOW}Running NLM tests...${NC}"
    
    if [ ! -d "build" ]; then
        echo -e "${RED}Build directory not found. Running build first...${NC}"
        build_project "$verbose"
    fi
    
    cd build
    
    # Run the main NLM executable
    echo -e "${YELLOW}Running NLM Phase 2 main test...${NC}"
    ./nlm "$@"
    
    if [ "$quick" = "--quick" ]; then
        echo -e "${GREEN}Quick test completed!${NC}"
    else
        echo -e "${YELLOW}Running comprehensive tests...${NC}"
        # Run additional tests if they exist
        if [ -f "nlm_phase3_demo" ]; then
            echo -e "${YELLOW}Running Phase 3 demo...${NC}"
            ./nlm_phase3_demo
        fi
        
        if [ -f "nlm_phase4_demo" ]; then
            echo -e "${YELLOW}Running Phase 4 demo...${NC}"
            ./nlm_phase4_demo
        fi
    fi
    
    cd ..
}

run_quick_tests() {
    echo -e "${YELLOW}Running quick NLM tests...${NC}"
    
    if [ ! -d "build" ]; then
        echo -e "${RED}Build directory not found. Running build first...${NC}"
        build_project
    fi
    
    cd build
    
    # Run a quick test with minimal settings
    echo -e "${YELLOW}Running quick demo...${NC}"
    ./nlm --config ../configs/default.cfg --steps 50 --neurons 100
    
    cd ..
}

generate_docs() {
    echo -e "${YELLOW}Generating documentation...${NC}"
    
    # Check for documentation tools
    if command -v doxygen &> /dev/null; then
        echo -e "${YELLOW}Generating API documentation with Doxygen...${NC}"
        cd docs
        doxygen Doxyfile
        cd ..
    else
        echo -e "${YELLOW}Doxygen not found, skipping API documentation${NC}"
    fi
    
    # Copy documentation to build directory
    if [ ! -d "build/docs" ]; then
        mkdir -p build/docs
    fi
    
    cp -r docs/ html/ build/docs/ 2>/dev/null || echo -e "${YELLOW}No docs to copy${NC}"
    
    echo -e "${GREEN}Documentation generated in build/docs/${NC}"
}

install_python_bindings() {
    echo -e "${YELLOW}Installing Python bindings...${NC}"
    
    if [ -f "pyproject.toml" ]; then
        pip install -e .
        echo -e "${GREEN}Python bindings installed successfully!${NC}"
    else
        echo -e "${RED}pyproject.toml not found${NC}"
        return 1
    fi
}

clean_build() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    
    if [ -d "build" ]; then
        rm -rf build
        echo -e "${GREEN}Build directory cleaned${NC}"
    fi
    
    # Remove generated documentation
    if [ -d "docs/html" ]; then
        rm -rf docs/html
        echo -e "${GREEN}Documentation cleaned${NC}"
    fi
    
    echo -e "${GREEN}Clean completed${NC}"
}

# Parse command line arguments
if [ $# -eq 0 ]; then
    print_usage
    exit 1
fi

command="$1"
shift

# Parse options for verbose/quick/config
verbose=""
quick=""
config_file=""

while [ $# -gt 0 ]; do
    case "$1" in
        --verbose)
            verbose="--verbose"
            shift
            ;;
        --quick)
            quick="--quick"
            shift
            ;;
        --config)
            config_file="--config $2"
            shift 2
            ;;
        *)
            shift
            ;;
    esac
done

# Execute command
 case "$command" in
    build)
        build_project "$verbose"
        ;;
    test)
        test_project "$verbose" "$quick" "$config_file"
        ;;
    quick-test)
        run_quick_tests
        ;;
    docs)
        generate_docs
        ;;
    install)
        install_python_bindings
        ;;
    clean)
        clean_build
        ;;
    help|*)
        print_usage
        ;;
    *)
        echo -e "${RED}Unknown command: $command${NC}"
        print_usage
        exit 1
        ;;
    esac