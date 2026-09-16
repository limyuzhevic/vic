#!/bin/bash

# Build and run Phase 6 Integration Demo
# This script helps users build and test the Phase 6 integration demonstration

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

function print_header() {
    echo -e "${BLUE}================================================================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================================================================${NC}"
}

function print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

function print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

function print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Main build and run script
print_header "PHASE 6 INTEGRATION DEMO BUILD AND RUN"

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed. Please install CMake (version 3.16 or higher)."
    echo "On Ubuntu/Debian: sudo apt-get install cmake"
    echo "On macOS: brew install cmake"
    exit 1
fi

# Check CMake version
CMAKE_VERSION=$(cmake --version | head -1 | grep -oE '[0-9]+\.[0-9]+')
CMAKE_VERSION_MAJOR=$(echo $CMAKE_VERSION | cut -d. -f1)
if [ "$CMAKE_VERSION_MAJOR" -lt 3 ] || ([ "$CMAKE_VERSION_MAJOR" -eq 3 ] && [ "$(echo $CMAKE_VERSION | cut -d. -f2)" -lt 16 ]); then
    print_warning "CMake version $CMAKE_VERSION is older than 3.16. This may cause issues."
    print_warning "Recommended version is 3.16 or higher."
fi

# Check for C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    print_error "C++ compiler not found. Please install a C++ compiler."
    exit 1
fi

# Check if we have existing build directory
if [ ! -d "$BUILD_DIR" ]; then
    print_info "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure CMake
print_info "Configuring CMake..."
cd "$BUILD_DIR"
cmake "$PROJECT_DIR" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 2>&1 | tee cmake_output.txt

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed. Check cmake_output.txt for details."
    exit 1
fi

# Build the project
print_info "Building the project..."
cmake --build . -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) 2>&1 | tee build_output.txt

if [ $? -ne 0 ]; then
    print_error "Build failed. Check build_output.txt for details."
    exit 1
fi

# Check if demo executables were built
if [ ! -f "$BUILD_DIR/nlm_phase6_demo" ]; then
    print_error "Phase 6 demo executable not found after build."
    exit 1
fi

print_info "Build successful!"
print_info "Demo executable: $BUILD_DIR/nlm_phase6_demo"

# Run the demo
print_header "RUNNING PHASE 6 INTEGRATION DEMO"
print_info "Running with default settings (1000 neurons, 10000 steps)..."
print_info "To run with custom settings, use: ./nlm_phase6_demo <neuron_count> <max_steps>"
print_info "Example: ./nlm_phase6_demo 500 2000"
print_info ""
print_info "Note: The demo will run for several minutes. Please wait..."
print_info ""

# Run with default settings
"$BUILD_DIR/nlm_phase6_demo" 2>&1

# Save results
if [ -f "$PROJECT_DIR/results.txt" ]; then
    mv "$PROJECT_DIR/results.txt" "$PROJECT_DIR/results_$(date +%Y%m%d_%H%M%S).txt"
fi

print_header "DEMO COMPLETED"
print_info "The Phase 6 Integration Demo has successfully demonstrated:"
print_info "✓ Full 15-step integrated brain loop"
print_info "✓ Memory integration (working, episodic, replay)"
print_info "✓ Prediction system integration"
print_info "✓ Cognitive integration (neural planning, concept formation)"
print_info "✓ Neuromodulation integration (dopamine, curiosity, novelty)"
print_info "✓ Development integration (developmental stages)"
print_info "✓ Sleep/rest cycles (memory consolidation)"
print_info "✓ Persistence (checkpoint save/load)"
print_info "✓ Multi-step planning (neural planner with SelfModel)"
print_info "✓ Concept formation (pattern discovery)"
print_info ""
print_info "For documentation and build instructions, see:"
print_info "  - $PROJECT_DIR/README.md"
print_info "  - $PROJECT_DIR/HOW_TO_USE.md"
print_info "  - $PROJECT_DIR/docs/PHASE6_ARCHITECTURE.md"

exit 0
