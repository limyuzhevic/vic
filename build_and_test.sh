#!/bin/bash

# NLM Build and Test Script
# Unified build system for NLM (Neural Learning Machine) project

# Color output for better visibility
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="NLM"
DEFAULT_BUILD_TYPE="Release"
DEFAULT_INSTALL_PREFIX="/usr/local"
DEFAULT_PYTHON_VERSION="3.8"

# Default configuration values
DEFAULT_CONFIG="neuron_count=1000
region_count=1
connection_probability=0.1
stdp_ltp_weight=0.01
stdp_ltd_weight=0.012
stdp_tau=20.0
synaptogenesis_rate=0.0001
pruning_rate=0.00001
replay_interval=100
consolidation_interval=1000
simulation_timestep=0.001
random_seed=42"

print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Build and test options:"
    echo "  build [type]           Build the project (type: Debug|Release|RelWithDebInfo|MinSizeRel)"
    echo "  install [prefix]      Install to system (prefix: /usr/local)"
    echo "  test                   Run tests"
    echo "  demo [phase]           Run demo (phase: 3|4|6)"
    echo "  python-install         Install Python package"
    echo "  python-dev             Install development Python package (editable)"
    echo "  python-test            Test Python package"
    echo "  clean                  Clean build artifacts"
    echo "  help                   Show this help message"
    echo ""
    echo "Configuration options:"
    echo "  -c, --config FILE      Use custom configuration file"
    echo "  -d, --debug            Build with debug flags"
    echo "  -O, --optimization LEVEL  Optimization level (0-3)"
    echo "  --python-version VERSION  Python version (default: $DEFAULT_PYTHON_VERSION)"
    echo "  --cmake-args ARGS      Additional CMake arguments"
    echo ""
    echo "Phase 6 specific options:"
    echo "  --enable-checkpointing Enable checkpoint save/load"
    echo "  --checkpoint-path PATH  Path for checkpoint files"
    echo "  --max-steps NUM        Maximum simulation steps"
    echo "  --enable-neuromodulation Enable all neuromodulation systems"
    echo "  --enable-curiosity     Enable curiosity-driven exploration"
    echo "  --enable-prediction    Enable prediction system"
    echo "  --enable-concept-formación Enable concept formation"
}

build_project() {
    local build_type="${1:-$DEFAULT_BUILD_TYPE}"
    local install_prefix="$DEFAULT_INSTALL_PREFIX"
    local config_file=""
    local cmake_args=""
    local optimization_level=2
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -c|--config)
                config_file="$2"
                shift 2
                ;;
            -d|--debug)
                build_type="Debug"
                optimization_level=0
                shift
                ;;
            -O)
                optimization_level="$2"
                shift 2
                ;;
            --cmake-args)
                cmake_args="$2"
                shift 2
                ;;
            *)
                echo "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done
    
    echo -e "${YELLOW}Building NLM ($build_type)...${NC}"
    
    # Create build directory
    mkdir -p build
    cd build || {
        echo -e "${RED}Error: Cannot enter build directory${NC}"
        exit 1
    }
    
    # Configure CMake
    echo "Configuring CMake with optimization level $optimization_level..."
    cmake .. \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_CXX_FLAGS="-O$optimization_level" \
        -DCMAKE_INSTALL_PREFIX="$install_prefix" \
        $cmake_args
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}CMake configuration failed${NC}"
        exit 1
    fi
    
    # Build
    echo "Building project..."
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}NLM built successfully!${NC}"
        echo "Build outputs:"
        ls -la *.a *.so *.exe 2>/dev/null || echo "No build artifacts found"
    else
        echo -e "${RED}Build failed${NC}"
        exit 1
    fi
    
    cd .. || exit 1
}

install_project() {
    local install_prefix="${1:-$DEFAULT_INSTALL_PREFIX}"
    
    echo -e "${YELLOW}Installing NLM to $install_prefix${NC}"
    
    # Create install directory
    sudo mkdir -p "$install_prefix"
    sudo chown -R $(whoami):$(whoami) "$install_prefix"
    
    # Build and install
    if [ ! -d "build" ]; then
        echo "Building first..."
        build_project
    fi
    
    cd build || {
        echo -e "${RED}Error: Cannot enter build directory${NC}"
        exit 1
    }
    
    sudo make install
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}NLM installed successfully!${NC}"
        echo "Installation location: $install_prefix"
    else
        echo -e "${RED}Installation failed${NC}"
        exit 1
    fi
    
    cd .. || exit 1
}

run_tests() {
    echo -e "${YELLOW}Running NLM tests...${NC}"
    
    # Check if tests were built
    if [ ! -f "build/nlm_test" ]; then
        echo "Building tests first..."
        build_project
    fi
    
    cd build || {
        echo -e "${RED}Error: Cannot enter build directory${NC}"
        exit 1
    }
    
    # Run tests
    ctest --output-on-failure
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}All tests passed!${NC}"
    else
        echo -e "${RED}Tests failed${NC}"
        exit 1
    fi
    
    cd .. || exit 1
}

run_demo() {
    local phase="${1:-6}"
    
    echo -e "${YELLOW}Running Phase $phase demo...${NC}"
    
    # Check if demo exists
    local demo_exe="build/nlm_phase${phase}_demo"
    if [ ! -f "$demo_exe" ]; then
        echo "Building demo first..."
        build_project
    fi
    
    # Run demo
    "$demo_exe"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Demo completed successfully!${NC}"
    else
        echo -e "${RED}Demo failed${NC}"
        exit 1
    fi
}

install_python() {
    echo -e "${YELLOW}Installing Python package...${NC}"
    
    # Install Python package using pip
    pip install .
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Python package installed successfully!${NC}"
        echo "Test with: python -c \"import pynlm; print('NLM Python bindings loaded successfully')\""
    else
        echo -e "${RED}Python package installation failed${NC}"
        exit 1
    fi
}

install_python_dev() {
    echo -e "${YELLOW}Installing Python development package (editable)...${NC}"
    
    # Install development version
    pip install -e .
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Development Python package installed successfully!${NC}"
        echo "You can now modify Python bindings and see changes immediately."
    else
        echo -e "${RED}Development Python package installation failed${NC}"
        exit 1
    fi
}

test_python() {
    echo -e "${YELLOW}Testing Python package...${NC}"
    
    # Run Python tests if available
    if [ -f "tests/test_*.py" ]; then
        python -m pytest tests/ -v
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}Python tests passed!${NC}"
        else
            echo -e "${RED}Python tests failed${NC}"
            exit 1
        fi
    else
        # Basic functionality test
        python -c "
import pynlm

print('Testing NLM Python bindings...')

# Test basic configuration
config = pynlm.createDefaultConfig()
print('✓ Default config created')

# Test brain creation
brain = pynlm.createBrain(config)
brain.initialize()
print('✓ Brain created and initialized')

# Test basic statistics
total_neurons = brain.getTotalNeuronCount()
print(f'✓ Brain has {total_neurons} neurons')

# Test simulation
for i in range(10):
    brain.step(i)
print('✓ Brain simulation completed')

print('✓ All basic tests passed!')
        " || {
            echo -e "${RED}Python tests failed${NC}"
            exit 1
        }
    fi
}

clean_build() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    
    if [ -d "build" ]; then
        rm -rf build
        echo "Build directory removed"
    else
        echo "No build directory found"
    fi
    
    # Remove Python build artifacts
    if [ -d "dist" ]; then
        rm -rf dist
        echo "dist directory removed"
    fi
    
    if [ -d "*.egg-info" ]; then
        rm -rf *.egg-info
        echo "*.egg-info directories removed"
    fi
    
    echo -e "${GREEN}Clean completed!${NC}"
}

main() {
    local action="${1:-help}"
    
    case $action in
        build)
            build_project "${2:-}"
            ;;
        install)
            install_project "${2:-}"
            ;;
        test)
            run_tests
            ;;
        demo)
            run_demo "${2:-6}"
            ;;
        python-install)
            install_python
            ;;
        python-dev)
            install_python_dev
            ;;
        python-test)
            test_python
            ;;
        clean)
            clean_build
            ;;
        help|*)
            print_usage
            ;;
        *)
            echo -e "${RED}Unknown command: $action${NC}"
            print_usage
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"