#!/bin/bash

# NLM Build and Installation Script
# Provides comprehensive build, install, and configuration options
#
# Usage:
#   ./install.sh [options]
#
# Options:
#   --help, -h                    Show help message
#   --version, -v                 Show version
#   --type <debug|release>        Build type (default: release)
#   --prefix <path>               Installation prefix (default: /usr/local)
#   --python-bindings, -p         Build Python bindings
#   --python-version <version>     Python version (default: 3.8+)
#   --install-only                Only install, don't build
#   --build-only, -b              Only build, don't install
#   --tests                       Build and run tests
#   --rebuild                     Clean and rebuild
#   --clean                       Clean build directory
#   --gpu                        Build with GPU acceleration (if available)
#   --omp                         Build with OpenMP support (default: enabled)
#   --mpi                         Build with MPI support (if available)
#
# Examples:
#   ./install.sh --type debug --prefix /opt/nlm
#   ./install.sh --python-bindings --python-version 3.10 --install-only
#   ./install.sh --rebuild --tests

set -e

# Default configuration
BUILD_TYPE="release"
INSTALL_PREFIX="/usr/local"
BUILD_PYTHON_BINDINGS="off"
PYTHON_VERSION="3.8+"
BUILD_TESTS="off"
BUILD_OMP="on"
BUILD_MPI="off"
BUILD_GPU="off"
BUILD_ONLY="off"
INSTALL_ONLY="off"
REBUILD="off"
CLEAN_ONLY="off"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --version|-v)
            show_version
            exit 0
            ;;
        --type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --python-bindings|-p)
            BUILD_PYTHON_BINDINGS="on"
            shift
            ;;
        --python-version)
            PYTHON_VERSION="$2"
            shift 2
            ;;
        --tests)
            BUILD_TESTS="on"
            shift
            ;;
        --rebuild)
            REBUILD="on"
            shift
            ;;
        --clean)
            CLEAN_ONLY="on"
            shift
            ;;
        --gpu)
            BUILD_GPU="on"
            shift
            ;;
        --omp)
            BUILD_OMP="on"
            shift
            ;;
        --mpi)
            BUILD_MPI="on"
            shift
            ;;
        --build-only|-b)
            BUILD_ONLY="on"
            shift
            ;;
        --install-only)
            INSTALL_ONLY="on"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Function to show help message
show_help() {
    cat << EOF
NLM Build and Installation Script
================================

This script provides comprehensive build and installation capabilities for
NLM (Neural Learning Machine), a spiking neural network simulation system.

Build Types:
  debug      Debug build with symbols and assertions (default: release)
  release    Optimized release build

Installation:
  --prefix <path>   Installation directory (default: /usr/local)

Features:
  --python-bindings, -p    Build Python bindings (default: disabled)
  --python-version <v>     Python version requirement (default: 3.8+)
  --tests                  Build and run tests (default: disabled)
  --gpu                    Build with GPU acceleration (if supported)
  --omp                     Build with OpenMP support (default: enabled)
  --mpi                     Build with MPI support (if available)

Build Controls:
  --build-only, -b          Only build, skip installation
  --install-only            Only install, skip building
  --rebuild                 Clean and rebuild completely
  --clean                   Clean build directory only

Examples:
  # Build and install in custom location
  ./install.sh --type debug --prefix /opt/nlm --python-bindings

  # Build Python bindings only
  ./install.sh --python-bindings --python-version 3.10 --install-only

  # Rebuild with all features
  ./install.sh --rebuild --python-bindings --tests --gpu --omp

  # Quick debug build
  ./install.sh --type debug --build-only
EOF
}

# Function to show version
show_version() {
    echo "NLM (Neural Learning Machine)"
    echo "Version: 2.0.0"
    echo "Phase: Phase 2: Real Neural Computation"
    echo "Build: $(date)"
}

# Function to clean build directory
clean_build() {
    echo "Cleaning build directory..."
    rm -rf build
    echo "Build directory cleaned."
}

# Function to configure build
cmake_configure() {
    local build_type="$1"
    local install_prefix="$2"
    local python_bindings="$3"
    local python_version="$4"
    local tests="$5"
    local omp="$6"
    local mpi="$7"
    local gpu="$8"

    echo "Configuring CMake build..."
    echo "Build type: $build_type"
    echo "Install prefix: $install_prefix"
    echo "Python bindings: $python_bindings"
    echo "Tests: $tests"
    echo "OpenMP: $omp"
    echo "MPI: $mpi"
    echo "GPU: $gpu"

    # Base CMake command
    local cmake_cmd="cmake .."
    cmake_cmd+=" -DCMAKE_BUILD_TYPE=${build_type}"
    cmake_cmd+=" -DCMAKE_INSTALL_PREFIX=${install_prefix}"
    cmake_cmd+=" -DNLM_BUILD_PYTHON_BINDINGS=${python_bindings}"
    cmake_cmd+=" -DPYTHON_EXECUTABLE=$(which python3)"

    # Optional features
    if [[ "$omp" == "on" ]]; then
        cmake_cmd+=" -DNLM_ENABLE_OPENMP=ON"
    else
        cmake_cmd+=" -DNLM_ENABLE_OPENMP=OFF"
    fi

    if [[ "$mpi" == "on" ]]; then
        cmake_cmd+=" -DNLM_ENABLE_MPI=ON"
    fi

    if [[ "$gpu" == "on" ]]; then
        cmake_cmd+=" -DNLM_ENABLE_GPU=ON"
    fi

    # Set Python version if specified
    if [[ "$python_version" != "3.8+" ]]; then
        cmake_cmd+=" -DPYTHON_VERSION=${python_version}"
    fi

    # Tests option
    if [[ "$tests" == "on" ]]; then
        cmake_cmd+=" -DBUILD_TESTING=ON"
    fi

    # Execute CMake
    mkdir -p build
    cd build
    eval $cmake_cmd
}

# Function to build the project
make_build() {
    echo "Building NLM..."
    make -j$(nproc)
    echo "Build completed successfully."
}

# Function to install the project
make_install() {
    echo "Installing NLM..."
    make install
    echo "Installation completed successfully."
}

# Function to run tests
make_tests() {
    echo "Running tests..."
    if [[ -f ./nlm_test ]]; then
        ./nlm_test
    elif [[ -f ./test/nlm_test ]]; then
        ./test/nlm_test
    else
        echo "Warning: Test executable not found, skipping tests."
    fi
    echo "Tests completed."
}

# Function to install Python bindings
install_python_bindings() {
    echo "Installing Python bindings..."
    
    # Check for Python development headers
    if ! python3 -c "import sysconfig; print('Python headers available')" 2>/dev/null; then
        echo "Warning: Python development headers not found. Python bindings may fail to build."
    fi

    # Install Python module via pip
    if command -v pip3 >/dev/null 2>&1; then
        echo "Installing pynlm via pip..."
        cd ..  # Go to project root
        pip3 install --upgrade pip
        pip3 install -e .
        cd build
    else
        echo "Warning: pip3 not found. Python bindings may not be installed."
    fi
}

# Function to create installation scripts and documentation
create_install_scripts() {
    local install_prefix="$1"
    
    echo "Creating installation scripts..."
    
    # Create bin directory
    mkdir -p "${install_prefix}/bin"
    
    # Create shell wrapper scripts
    cat > "${install_prefix}/bin/nlm" << 'EOF'
#!/bin/bash
# NLM wrapper script
# Usage: nlm [OPTIONS]
#
# This script provides a convenient interface to the NLM neural simulator.
# It handles command-line parsing and execution.

cd "$(dirname "$0")/.."

# Use the build directory's nlm executable
if [[ -f "build/nlm" ]]; then
    exec "build/nlm" "$@"
elif [[ -f "./nlm" ]]; then
    exec "./nlm" "$@"
else
    echo "Error: NLM executable not found. Please build NLM first."
    exit 1
fi
EOF

    cat > "${install_prefix}/bin/nlm-benchmark" << 'EOF'
#!/bin/bash
# NLM Benchmark wrapper script
# Usage: nlm-benchmark [OPTIONS]

cd "$(dirname "$0")/.."

if [[ -f "build/nlm_benchmark" ]]; then
    exec "build/nlm_benchmark" "$@"
elif [[ -f "./nlm_benchmark" ]]; then
    exec "./nlm_benchmark" "$@"
else
    echo "Error: NLM benchmark executable not found. Please build NLM first."
    exit 1
fi
EOF

    cat > "${install_prefix}/bin/nlm-demo" << 'EOF'
#!/bin/bash
# NLM Demo wrapper script
# Usage: nlm-demo [OPTIONS]

cd "$(dirname "$0")/.."

if [[ -f "build/nlm_phase6_demo" ]]; then
    exec "build/nlm_phase6_demo" "$@"
elif [[ -f "./nlm_phase6_demo" ]]; then
    exec "./nlm_phase6_demo" "$@"
else
    echo "Error: NLM demo executable not found. Please build NLM first."
    exit 1
fi
EOF

    # Make scripts executable
    chmod +x "${install_prefix}/bin/nlm"
    chmod +x "${install_prefix}/bin/nlm-benchmark"
    chmod +x "${install_prefix}/bin/nlm-demo"
    
    # Create documentation
    mkdir -p "${install_prefix}/share/doc/nlm"
    
    cat > "${install_prefix}/share/doc/nlm/installation.txt" << 'EOF'
NLM Installation Instructions
=============================

NLM (Neural Learning Machine) has been successfully installed.

Available Commands:
  nlm              Run the main neural simulation demo
  nlm-benchmark    Run performance benchmarks
  nlm-demo         Run advanced integration demo

Configuration:
  Configuration files are located in: /usr/local/etc/nlm/
  Python bindings are installed to: /usr/local/lib/python3.8/dist-packages/pynlm
  Documentation is available in: /usr/local/share/doc/nlm/

Getting Started:
  1. Run 'nlm --help' for command-line options
  2. Configure via 'nlm --config <config_file>'
  3. Use Python: 'import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig())'

For more information, visit: https://github.com/nlm-project/nlm
EOF

    cat > "${install_prefix}/share/doc/nlm/python-api.txt" << 'EOF'
NLM Python API Reference
=======================

Basic Usage:
  import pynlm
  
  # Create and configure brain
  config = pynlm.createDefaultConfig()
  config.set("brain.neuron_count", 1000)
  brain = pynlm.createBrain(config)
  brain.initialize()
  
  # Create world and agent
  world = pynlm.createSimpleWorld()
  agent = pynlm.createAgentBrain(brain)
  agent.initialize(world)
  
  # Run simulation
  for step in range(1000):
      world.update(0.1)
      agent.processSensoryInput(world.getSensoryPercept())
      brain.step(step)
      world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

Advanced Features:
  - Memory systems (working memory, episodic memory)
  - Neuromodulation (reward, curiosity, novelty)
  - Prediction and planning
  - Development and structural plasticity
  - Checkpoint save/load
  - Text-as-a-sense processing

API Classes:
  - Config: Configuration management
  - Brain: Neural simulation engine
  - AgentBrain: Brain-world interface
  - SimpleWorld: 2D simulation environment
  - Vision: Visual sensory input
  - Audio: Auditory sensory input
  - InternalSignals: Internal signal processing
  - Action: Motor commands
  - WorldObject: Environmental objects
  - SensoryPercept: Complete sensory data

Constants:
  - INVALID_NEURON_ID: Invalid neuron identifier
  - INVALID_SYNAPSE_ID: Invalid synapse identifier
  - INVALID_REGION_ID: Invalid region identifier
  - INVALID_POPULATION_ID: Invalid population identifier
  - INVALID_WORLD_OBJECT_ID: Invalid world object identifier

Functions:
  - createDefaultConfig(): Create default configuration
  - createBrain(config): Create neural brain
  - createSimpleWorld(): Create simulation world
  - createAgentBrain(brain): Create agent interface
  - runTrainingEpisode(): Run complete training episode
  - evaluatePolicy(): Evaluate policy performance
  - saveBrainState(): Save brain to file
  - loadBrainState(): Load brain from file
  - getBrainStatistics(): Get brain statistics
  - benchmarkBrain(): Run benchmark and save results
EOF

    echo "Installation scripts and documentation created."
}

# Main execution logic
main() {
    # Show help if no arguments
    if [[ $# -eq 0 ]]; then
        show_help
        exit 0
    fi

    # Handle clean-only operation
    if [[ "$CLEAN_ONLY" == "on" ]]; then
        clean_build
        exit 0
    fi

    # Handle rebuild
    if [[ "$REBUILD" == "on" ]]; then
        echo "Rebuilding NLM..."
        clean_build
    fi

    # Determine build phase
    if [[ "$INSTALL_ONLY" == "on" ]]; then
        echo "Installing NLM (already built)..."
        cd build
        make_install
        install_python_bindings
        create_install_scripts "$INSTALL_PREFIX"
        exit 0
    fi

    if [[ "$BUILD_ONLY" == "on" ]]; then
        echo "Building NLM only..."
        cd build
        make_build
        exit 0
    fi

    # Full build and install
    echo "NLM Installation Script"
    echo "======================="
    echo

    # Configure
    cmake_configure "$BUILD_TYPE" "$INSTALL_PREFIX" "$BUILD_PYTHON_BINDINGS" "$PYTHON_VERSION" "$BUILD_TESTS" "$BUILD_OMP" "$BUILD_MPI" "$BUILD_GPU"

    # Build
    make_build

    # Run tests if requested
    if [[ "$BUILD_TESTS" == "on" ]]; then
        make_tests
    fi

    # Install
    make_install

    # Install Python bindings if requested
    if [[ "$BUILD_PYTHON_BINDINGS" == "on" ]]; then
        install_python_bindings
    fi

    # Create installation scripts and documentation
    create_install_scripts "$INSTALL_PREFIX"

    # Success message
    echo
    echo "========================================"
    echo "NLM installation completed successfully!"
    echo "========================================"
    echo
    echo "Installation Summary:"
    echo "  Build type: $BUILD_TYPE"
    echo "  Install prefix: $INSTALL_PREFIX"
    echo "  Python bindings: $([[ "$BUILD_PYTHON_BINDINGS" == "on" ]] && echo "Enabled" || echo "Disabled")"
    echo "  Tests: $([[ "$BUILD_TESTS" == "on" ]] && echo "Enabled" || echo "Disabled")"
    echo "  OpenMP: $([[ "$BUILD_OMP" == "on" ]] && echo "Enabled" || echo "Disabled")"
    echo "  MPI: $([[ "$BUILD_MPI" == "on" ]] && echo "Enabled" || echo "Disabled")"
    echo "  GPU: $([[ "$BUILD_GPU" == "on" ]] && echo "Enabled" || echo "Disabled")"
    echo
    echo "Usage:"
    echo "  nlm                    Run the main neural simulation"
    echo "  nlm-benchmark          Run performance benchmarks"
    echo "  nlm-demo               Run advanced integration demo"
    echo
    echo "Documentation: $INSTALL_PREFIX/share/doc/nlm/"
    echo "Binaries: $INSTALL_PREFIX/bin/"
}

# Execute main function
main "$@"