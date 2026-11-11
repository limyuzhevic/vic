# NLM Build System Helper Script

# This script provides convenient build commands for the NLM project

# Build all targets
cmake --build . --target all

# Build the Python module (requires pybind11 and Python)
cmake --build . --target pynlm

# Install Python module
cmake --build . --target install_python_bindings

# Build just the main demo executables
for exe in nlm nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_benchmark; do
    cmake --build . --target $exe
    if [ $? -eq 0 ]; then
        echo "Successfully built $exe"
    else
        echo "Failed to build $exe"
    fi
done

# Run tests
cmake --build . --target test

# Build with specific options
# Release build (default)
# Debug build:
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
make -j$(nproc)

# Build with Python bindings enabled
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)

# Installation
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Quick build script for developers
#!/bin/bash

PROJECT_ROOT="$(dirname "$0")"

if [ "$1" = "quick" ]; then
    # Quick build for development
    mkdir -p "$PROJECT_ROOT/build" && cd "$PROJECT_ROOT/build"
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make -j$(nproc)
    echo "Quick build completed. Run ./nlm, ./nlm_phase3_demo, etc. from build directory."
elif [ "$1" = "python" ]; then
    # Build Python module
    cd "$PROJECT_ROOT"
    pip install -e .
    echo "Python module installed. Test with: python -c 'import pynlm; print(pynlm.__version__)'"
elif [ "$1" = "all" ]; then
    # Build everything
    cd "$PROJECT_ROOT"
    ./build.sh quick
    echo "Full build completed. Run ./build/nlm for main demo."
else
    echo "Usage: $0 [quick|python|all]"
    echo "  quick  - Build for development (with debug symbols)"
    echo "  python - Build and install Python module"
    echo "  all    - Build everything"
    exit 1
fi