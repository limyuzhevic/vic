#!/usr/bin/env bash
# Build and installation script for NLM (熙然)
# This script provides a complete build and setup guide for developers

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_cb80c7d4-18ed-42c6-8a69-a0bdce4feb6f

echo "Building NLM (熙然) - Neural Learning Machine"
echo "============================================="
echo ""
echo "This is a sophisticated spiking neural network simulation framework"
echo "written in C++20 with Python bindings via pybind11."
echo ""

echo "=== Prerequisites ==="
echo "Required tools:"
echo "- CMake 3.16 or higher"
echo "- C++ compiler supporting C++20 (GCC 9+, Clang 12+, MSVC 2019+)"
echo "- Python 3.8 or higher"
echo "- pip (Python package installer)"
echo "- Optional: scikit-build-core, pybind11"
echo ""

echo "=== Build Options ==="
echo "1. Standard C++ build (for developers)"
echo "2. Python bindings installation (for end users)"
echo "3. Full development environment setup"
echo ""

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found. Please install CMake 3.16 or higher."
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "ERROR: No C++ compiler found. Please install GCC 9+ or Clang 12+."
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 not found. Please install Python 3.8 or higher."
    exit 1
fi

echo "Prerequisites check passed."
echo ""

# Create build directory
mkdir -p build
cd build

echo "=== Configuring CMake ==="
echo "Configuring with CMake...

echo "Available build types:"
echo "- Release (default, optimized for performance)"
echo "- Debug (with symbols and assertions)"
echo "- RelWithDebInfo (optimized with debug info)"
echo ""

# Configure CMake
echo "Running: cmake .. -DCMAKE_BUILD_TYPE=Release"
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed."
    exit 1
fi

echo "CMake configuration successful."
echo ""

# Determine number of CPU cores for parallel build
if command -v nproc &> /dev/null; then
    CORES=$(nproc)
elif command -v sysctl &> /dev/null; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=4
fi

echo "=== Building NLM ==="
echo "Building with $CORES parallel jobs..."
echo ""

# Build the project
echo "Running: make -j$CORES"
make -j$CORES

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed."
    exit 1
fi

echo "Build successful!"
echo ""

# Check what was built
if [ -f "src/main" ]; then
    echo "=== Executables built ==="
    echo "Main executable: ./src/main"
    echo "Phase 3 demo: ./src/experiments/Phase3Demo"
    echo "Phase 4 demo: ./src/experiments/Phase4Demo"
    echo "Phase 6 demo: ./src/experiments/Phase6Demo"
    echo "Phase 6 integrated experiment: ./src/experiments/Phase6IntegratedExperiment"
    echo ""
fi

if [ -f "libnlm_core.a" ]; then
    echo "Core library: libnlm_core.a"
fi

if [ -f "libnlm_agent.a" ]; then
    echo "Agent library: libnlm_agent.a"
fi

if [ -f "libnlm_world.a" ]; then
    echo "World library: libnlm_world.a"
fi

echo ""
echo "=== Installation ==="
echo "To install NLM system-wide:"
echo "sudo make install"
echo ""
echo "Installation directory: /usr/local (can be changed with -DCMAKE_INSTALL_PREFIX)"
echo ""

echo "=== Running Tests ==="
echo "To run the test suite:"
echo "./nlm_test (or ctest)"
echo ""
echo "=== Python Bindings ==="
echo "To build and install Python bindings, run from project root:"
echo "pip install scikit-build-core pybind11"
echo "pip install .  # Install from current directory"
echo ""
echo "Or for development mode:"
echo "pip install -e ."
echo ""

echo "=== Running Examples ==="
echo "To run the main demonstration program:"
echo "./src/main --help"
echo ""
echo "For experiment examples:"
echo "./src/experiments/Phase3Demo"
echo "./src/experiments/Phase4Demo"
echo "./src/experiments/Phase6Demo"
echo ""

echo "=== Quick Start ==="
echo "1. Compile and run: make -j$(nproc) && ./src/main"
echo "2. Run experiments: ./src/experiments/Phase3Demo"
echo "3. Install Python bindings: pip install ."
echo "4. Use Python API: See easy_usage.md for examples"
echo ""

echo "=== Additional Resources ==="
echo "Documentation:"
echo "- easy_usage.md: Simple usage guide with Python examples"
echo "- HOW_TO_USE.md: Complete documentation with C++ and Python examples"
echo "- docs/ARCHITECTURE.md: System architecture"
echo "- docs/SCIENCE.md: Scientific background"
echo "- docs/EXPERIMENTS.md: Experiment descriptions"
echo ""
echo "Architecture Notes:"
echo "- Phase 2: Real neural computation with LIF neurons and event-driven dynamics"
echo "- Phase 3: Basic learning and adaptation"
echo "- Phase 4: Integrated perception-action loop"
echo "- Phase 5: Advanced cognitive functions"
echo "- Phase 6: Full integrated artificial brain"
echo ""

echo "=== Troubleshooting ==="
echo "Common issues and solutions:"
echo "1. CMake errors: Ensure CMake 3.16+ is installed"
echo "2. Compiler errors: Check C++20 support in your compiler"
echo "3. Python bindings: Install pybind11 first with 'pip install pybind11'"
echo "4. Performance: Use Release build type for best performance"
echo ""
echo "For more help, see the project documentation or GitHub issues."
echo ""
echo "=== Build Complete ==="
echo "NLM has been successfully built and is ready to use!"
echo ""
echo "For development and research, consider exploring the Python API for easier usage."
echo "For low-level neural simulation, use the C++ API directly."
echo ""
