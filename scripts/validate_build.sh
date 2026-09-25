#!/bin/bash
# scripts/validate_build.sh
# Build validation script for NLM

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse command line arguments
VERBOSE=false
BUILD_TYPE="Release"
GENERATE_DOCS=false
RUN_TESTS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -docs|--generate-docs)
            GENERATE_DOCS=true
            shift
            ;;
        -t|--run-tests)
            RUN_TESTS=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -v, --verbose           Enable verbose output"
            echo "  -d, --debug             Build in Debug mode"
            echo "  --generate-docs          Generate documentation"
            echo "  -t, --run-tests         Run tests after build"
            echo "  -h, --help              Show this help message"
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

log_info "Validating NLM build..."

# Create build directory
BUILD_DIR="${PROJECT_ROOT}/build_validation"
mkdir -p "$BUILD_DIR"

# Configure CMake
log_info "Configuring CMake..."
cd "$PROJECT_ROOT"

CMAKE_ARGS="-B \"$BUILD_DIR\" -S \"$PROJECT_ROOT\" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"

# Add additional CMake arguments based on build type
if [ "$BUILD_TYPE" = "Debug" ]; then
    CMAKE_ARGS="${CMAKE_ARGS} -DNLM_DEBUG=ON -DNLM_ENABLE_ASSERTIONS=ON"
fi

if ! eval "cmake $CMAKE_ARGS"; then
    log_error "CMake configuration failed"
    exit 1
fi

# Build the project
log_info "Building NLM (${BUILD_TYPE})..."
if ! cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel $(nproc); then
    log_error "Build failed"
    exit 1
fi

log_info "Build completed successfully"

# Generate documentation if requested
if [ "$GENERATE_DOCS" = true ]; then
    log_info "Generating documentation..."
    # This would typically run doxygen or other documentation generators
    # For now, we'll just log the action
    log_info "Documentation generation not implemented yet"
fi

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    log_info "Running tests..."
    cd "$BUILD_DIR"
    if [ -f "./nlm_tests" ]; then
        if [ "$VERBOSE" = true ]; then
            ./nlm_tests --verbose
        else
            ./nlm_tests
        fi
    else
        log_warn "Test executable not found, skipping tests"
    fi
fi

# Validate build artifacts
log_info "Validating build artifacts..."
cd "$BUILD_DIR"

# Check for expected binaries
EXPECTED_BINS=("nlm" "nlm_tests" "coverage_analyzer")

for bin in "${EXPECTED_BINS[@]}"; do
    if [ -f "$bin" ]; then
        log_info "✓ Found binary: $bin"
    else
        log_warn "Binary not found: $bin"
    fi
done

# Check for libraries
if [ -f "libnlm.so" ] || [ -f "libnlm.a" ]; then
    log_info "✓ Found NLM library"
else
    log_warn "NLM library not found"
fi

# Check for include files
if [ -d "../src/testing" ]; then
    log_info "✓ Found testing directory"
else
    log_warn "Testing directory not found"
fi

log_info "Build validation completed"

if [ "$VERBOSE" = true ]; then
    log_info "Validation details:"
    log_info "  Build type: $BUILD_TYPE"
    log_info "  Build directory: $BUILD_DIR"
    log_info "  Tests run: $([ "$RUN_TESTS" = true ] && echo "Yes" || echo "No")"
fi

exit 0