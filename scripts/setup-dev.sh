#!/bin/bash
# setup-dev.sh - Development environment setup script for NLM

set -e

# Development Configuration
BUILD_TYPE="Debug"
BUILD_DIR="build-dev"
PYTHON_BINDINGS=ON
TESTS=ON
PERFORMANCE=ON
COVERAGE=ON
ASAN=ON
TSAN=OFF

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check system requirements
print_info "Checking system requirements..."

# Check for CMake
if ! command -v cmake >/dev/null 2>&1; then
    print_error "CMake is not installed. Please install CMake 3.19 or higher."
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -1 | awk '{print $3}')
CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d'.' -f1)
CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d'.' -f2)

if [ $CMAKE_MAJOR -lt 3 ] || ([ $CMAKE_MAJOR -eq 3 ] && [ $CMAKE_MINOR -lt 19 ]); then
    print_warning "CMake version $CMAKE_VERSION is less than 3.19. Some features may not work correctly."
fi

# Check for compiler
if ! command -v g++ >/dev/null 2>&1 && ! command -v clang++ >/dev/null 2>&1; then
    print_error "No C++ compiler found. Please install g++ or clang++."
    exit 1
fi

# Check for Python
if ! command -v python3 >/dev/null 2>&1; then
    print_error "Python3 is not installed. Please install Python 3.8 or higher."
    exit 1
fi

PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
PYTHON_MAJOR=$(echo $PYTHON_VERSION | cut -d'.' -f1)
PYTHON_MINOR=$(echo $PYTHON_VERSION | cut -d'.' -f2)

if [ $PYTHON_MAJOR -lt 3 ] || ([ $PYTHON_MAJOR -eq 3 ] && [ $PYTHON_MINOR -lt 8 ]); then
    print_warning "Python version $PYTHON_VERSION is less than 3.8. Some features may not work correctly."
fi

# Check for pip
if ! command -v pip3 >/dev/null 2>&1; then
    print_warning "pip3 is not installed. Python dependencies may not be available."
fi

# Check for pybind11
if ! python3 -c "import pybind11" 2>/dev/null; then
    print_warning "pybind11 Python package not found. Installing..."
    pip3 install pybind11
fi

# Check for Ninja
if ! command -v ninja >/dev/null 2>&1; then
    print_warning "Ninja not found. Installing..."
    # Install Ninja (platform-specific)
    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get update
        sudo apt-get install -y ninja-build
    elif command -v brew >/dev/null 2>&1; then
        brew install ninja
    else
        print_error "Please install Ninja manually."
        exit 1
    fi
fi

# Check for required system libraries
print_info "Checking for system libraries..."

if command -v dpkg >/dev/null 2>&1; then
    # Check for required packages on Debian/Ubuntu
    REQUIRED_PACKAGES=("build-essential" "libpython3-dev" "python3-pip")
    for pkg in "${REQUIRED_PACKAGES[@]}"; do
        if ! dpkg -l | grep -q "^ii  $pkg"; then
            print_warning "Package $pkg is not installed. Consider installing it."
        fi
    done
elif command -v rpm >/dev/null 2>&1; then
    # Check for required packages on Fedora/CentOS
    REQUIRED_PACKAGES=("gcc-c++" "python3-devel" "python3-pip")
    for pkg in "${REQUIRED_PACKAGES[@]}"; do
        if ! rpm -q $pkg >/dev/null 2>&1; then
            print_warning "Package $pkg is not installed. Consider installing it."
        fi
    done
fi

# Create development build directory
print_info "Creating development build directory..."
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure CMake with development settings
print_info "Configuring CMake for development..."
cat > CMakeDevSettings.txt << EOF
# Development configuration
-DCMAKE_BUILD_TYPE=$BUILD_TYPE
-DNLM_ENABLE_PYTHON_BINDINGS=$PYTHON_BINDINGS
-DNLM_ENABLE_TESTS=$TESTS
-DNLM_ENABLE_PERFORMANCE=$PERFORMANCE
-DNLM_ENABLE_COVERAGE=$COVERAGE
-DNLM_ENABLE_ASAN=$ASAN
-DNLM_ENABLE_TSAN=$TSAN
-GNinja
EOF

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DNLM_ENABLE_PYTHON_BINDINGS=$PYTHON_BINDINGS \
    -DNLM_ENABLE_TESTS=$TESTS \
    -DNLM_ENABLE_PERFORMANCE=$PERFORMANCE \
    -DNLM_ENABLE_COVERAGE=$COVERAGE \
    -DNLM_ENABLE_ASAN=$ASAN \
    -DNLM_ENABLE_TSAN=$TSAN \
    -GNinja \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON

# Build the project
print_info "Building project..."
cmake --build . --config $BUILD_TYPE -- -j$(nproc)

# Generate compile_commands.json for tooling
if [ -f compile_commands.json ]; then
    print_info "Compile commands generated for tooling."
fi

# Run tests
if [ "$TESTS" = "ON" ]; then
    print_info "Running tests..."
    ctest --output-on-failure --no-compress-output
fi

# Setup Python environment
if [ "$PYTHON_BINDINGS" = "ON" ]; then
    print_info "Setting up Python environment..."
    
    # Install Python dependencies
    if [ -f ../nlm_utilities/requirements.txt ]; then
        pip3 install -r ../nlm_utilities/requirements.txt
    fi
    
    # Install the Python package in development mode
    if [ -f ../setup.py ]; then
        pip3 install -e ../python/setup.py
    fi
fi

# Generate development report
print_info "Generating development report..."
cd ..
cat > dev_setup_report.txt << EOF
Development Setup Report
========================
Setup date: $(date)
Build directory: $BUILD_DIR
Build type: $BUILD_TYPE

System information:
- OS: $(uname -s)
- Architecture: $(uname -m)
- Compiler: $(c++ --version 2>/dev/null | head -1 || echo "Unknown")
- CMake version: $(cmake --version 2>/dev/null | head -1 | awk '{print $3}')

Python information:
- Version: $PYTHON_VERSION
- Path: $(which python3)
- Site-packages: $(python3 -c "import site; print(site.getsitepackages()[0])" 2>/dev/null || echo "Unknown")

NLM features enabled:
- Python bindings: $PYTHON_BINDINGS
- Tests: $TESTS
- Performance optimizations: $PERFORMANCE
- Code coverage: $COVERAGE
- AddressSanitizer: $ASAN

Build completed successfully at: $(date)

Next steps:
1. To run tests: cd $BUILD_DIR && ctest --output-on-failure
2. To build Python bindings: cd $BUILD_DIR && cmake --build . --target pynlm
3. To install NLM: cd $BUILD_DIR && cmake --build . --target install
4. To run examples: Check the documentation for usage examples
EOF

print_info "Development environment setup completed successfully!"
echo ""
print_info "Development report saved to: dev_setup_report.txt"
print_info "Build directory: $BUILD_DIR"
print_info "Next steps:"
print_info "  1. Run tests: cd $BUILD_DIR && ctest --output-on-failure"
print_info "  2. Build Python bindings: cd $BUILD_DIR && cmake --build . --target pynlm"
print_info "  3. Install NLM: cd $BUILD_DIR && cmake --build . --target install"
print_info "  4. Check documentation: See build_docs.md for comprehensive information"
