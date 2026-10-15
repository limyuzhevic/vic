#!/bin/bash

# NLM Build and Test Script
# This script builds and runs the NLM neural simulation framework with the new experiment framework

set -e  # Exit on any error

echo "=== NLM Neural Learning Machine Build and Test ==="

# Configuration
BUILD_TYPE="Release"
BUILD_DIR="build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Step 1: Check for required dependencies
print_info "Checking for required dependencies..."

# Check for CMake
if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed. Please install CMake 3.16 or higher."
    exit 1
fi

cmake_version=$(cmake --version | head -n1 | awk '{print $3}')
cmake_major=${cmake_version%%.*}
if [ "$cmake_major" -lt 3 ]; then
    print_error "CMake version $cmake_version is too old. Please install CMake 3.16 or higher."
    exit 1
fi

# Check for C++ compiler
if ! command -v g++ &> /dev/null; then
    print_error "g++ compiler is not installed. Please install g++."
    exit 1
fi

print_success "All dependencies found."

# Step 2: Create build directory
print_info "Creating build directory..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Step 3: Configure CMake
print_info "Configuring CMake..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_CXX_STANDARD=20 ..

# Step 4: Build the project
print_info "Building project..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    print_success "Build completed successfully!"
else
    print_error "Build failed!"
    exit 1
fi

# Step 5: Run tests (if any)
print_info "Running tests..."
if [ -f "CTestTestfile.cmake" ]; then
    ctest --output-on-failure
    if [ $? -eq 0 ]; then
        print_success "All tests passed!"
    else
        print_error "Some tests failed!"
        exit 1
    fi
else
    print_warning "No tests found. Skipping test phase."
fi

# Step 6: Create demo configuration files
print_info "Creating demo configuration files..."

cd ..
mkdir -p configs/demo

cat > configs/demo/integration.json << EOF
{
    "version": "2.0",
    "format": "json",
    "config_entries": [
        {
            "key": "neurons",
            "int_value": 1000,
            "source": 0,
            "description": "Number of neurons in the brain"
        },
        {
            "key": "steps",
            "int_value": 2000,
            "source": 0,
            "description": "Maximum simulation steps"
        },
        {
            "key": "plasticity",
            "bool_value": true,
            "source": 0,
            "description": "Enable plasticity"
        }
    ]
}
EOF

cat > configs/demo/learning.json << EOF
{
    "version": "2.0",
    "format": "json",
    "config_entries": [
        {
            "key": "iterations",
            "int_value": 5000,
            "source": 0,
            "description": "Number of learning iterations"
        },
        {
            "key": "learning_rate",
            "double_value": 0.01,
            "source": 0,
            "description": "Learning rate"
        },
        {
            "key": "neurons",
            "int_value": 500,
            "source": 0,
            "description": "Number of neurons"
        }
    ]
}
EOF

print_success "Demo configuration files created!"

# Step 7: Show build summary
print_info "Build Summary:"
echo "  - Build type: $BUILD_TYPE"
echo "  - Source directory: $(pwd)"
echo "  - Build directory: $(pwd)/$BUILD_DIR"
echo "  - Main executable: $BUILD_DIR/nlm"
echo "  - Demo executables: $BUILD_DIR/nlm_phase3_demo, $BUILD_DIR/nlm_phase4_demo"
echo "  - Configuration files: configs/default.cfg, configs/demo/"
echo "  - Experiment framework: src/experiments/ExperimentFramework.hpp"
echo "  - CLI interface: src/experiments/CLIInterface.cpp"

# Step 8: Show usage examples
print_info "Usage Examples:"
echo "  1. Build the project:"
echo "     mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make"
echo "  "
echo "  2. Run basic neural simulation:"
echo "     cd build && ./nlm --neurons=1000 --steps=5000"
echo "  "
echo "  3. Run Phase 3 demo:"
echo "     cd build && ./nlm_phase3_demo"
echo "  "
echo "  4. Run Phase 4 demo:"
echo "     cd build && ./nlm_phase4_demo"
echo "  "
echo "  5. Run Phase 6 integration demo (new experiment framework):"
echo "     cd build && ./nlm --run-experiment integration --config=configs/demo/integration.json"
echo "  "
echo "  6. Interactive experiment runner:"
echo "     cd build && ./nlm --cli"
echo "  "
echo "  7. Save experiment configuration:"
echo "     cd build && ./nlm --save-experiment my_experiment.json"

print_success "Build and setup completed successfully!"
echo ""
echo "The NLM neural simulation framework is now ready to use."
echo "You can start by running: cd build && ./nlm --help"
