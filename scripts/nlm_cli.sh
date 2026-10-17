#!/bin/bash
# NLM Phase 6 Demo Script
# Simple interface for running NLM Phase 6 integration tests and demos

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR/.."
BUILD_DIR="$PROJECT_ROOT/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_usage() {
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  demo           Run Phase 6 integration demo"
    echo "  test           Run integration test suite"
    echo "  build          Build NLM with Phase 6 integration"
    echo "  clean          Clean build artifacts"
    echo "  help           Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 demo        # Run the comprehensive Phase 6 demo"
    echo "  $0 test        # Run the integration test suite"
    echo ""
}

run_build() {
    echo -e "${YELLOW}Building NLM with Phase 6 integration...${NC}"
    
    if [ ! -d "$BUILD_DIR" ]; then
        mkdir -p "$BUILD_DIR"
    fi
    
    cd "$BUILD_DIR"
    cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}NLM build successful!${NC}"
    else
        echo -e "${RED}Build failed!${NC}"
        return 1
    fi
}

run_demo() {
    echo -e "${YELLOW}Running NLM Phase 6 Integration Demo...${NC}"
    
    if [ ! -f "$BUILD_DIR/nlm_phase6_demo" ]; then
        echo -e "${RED}Demo executable not found. Building first...${NC}"
        run_build
    fi
    
    "$BUILD_DIR/nlm_phase6_demo"
}

run_tests() {
    echo -e "${YELLOW}Running NLM Integration Test Suite...${NC}"
    
    if [ ! -f "$BUILD_DIR/nlm_test" ]; then
        echo -e "${RED}Test executable not found. Building first...${NC}"
        run_build
    fi
    
    "$BUILD_DIR/nlm_test"
}

run_clean() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        echo "Cleaned build directory: $BUILD_DIR"
    fi
    
    # Clean any other build artifacts
    find "$PROJECT_ROOT" -name "*.o" -delete
    find "$PROJECT_ROOT" -name "*.obj" -delete
    find "$PROJECT_ROOT" -name "*.so" -delete
    find "$PROJECT_ROOT" -name "*.dylib" -delete
    find "$PROJECT_ROOT" -name "*.exe" -delete
    find "$PROJECT_ROOT" -name "*.bin" -delete
    
    echo -e "${GREEN}Clean complete.${NC}"
}

# Main command handling
case "$1" in
    build)
        run_build
        ;;
    demo)
        run_demo
        ;;
    test)
        run_tests
        ;;
    clean)
        run_clean
        ;;
    help|--help|-h)
        print_usage
        ;;
    *)
        echo -e "${RED}Unknown command: $1${NC}"
        echo "Run '$0 help' for usage information."
        exit 1
        ;;
esac
