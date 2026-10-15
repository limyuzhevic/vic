#!/bin/bash
# NLM Quick Start Guide
# 
# This script provides a quick start guide for NLM Phase 6 with:
# - Quick setup instructions
# - Example commands
# - Configuration validation
# - Basic experiment runs
# 
# Usage:
#   ./quickstart.sh [OPTIONS]
#   ./quickstart.sh --help
#

#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# NLM executable (adjust path as needed)
NLM_EXECUTABLE="./nlm"

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_help() {
    cat << EOF
NLM Phase 6 Quick Start Guide
=============================

A quick introduction to NLM Phase 6 - Complete Brain Integration

This guide helps you get started with NLM quickly and efficiently.

Prerequisites:
  - C++20 compiler
  - CMake 3.16 or higher
  - Build directory with nlm executable

Quick Setup:
  1. Build the project:
     mkdir build && cd build
     cmake .. -DCMAKE_BUILD_TYPE=Release
     make -j4
     
  2. Run basic tests:
     ./validate_config.sh configs/default.cfg
     ./run_batch.sh integration:steps=1000 neurons=500

Basic Usage Examples:

1. Run basic connectivity test:
   $ $NLM_EXECUTABLE --run basic --steps 1000 --neurons 500

2. Run plasticity learning experiment:
   $ $NLM_EXECUTABLE --run plasticity --steps 2000 --plasticity-config

3. Run STDP verification:
   $ $NLM_EXECUTABLE --run stdp --steps 500

4. Run full integration test:
   $ $NLM_EXECUTABLE --run integration --steps 1000

5. Run with custom configuration:
   $ $NLM_EXECUTABLE --config my_config.cfg --run integration

6. Debug mode with verbose output:
   $ $NLM_EXECUTABLE --run integration --debug --verbose

7. Save brain state during run:
   $ $NLM_EXECUTABLE --run integration --checkpoint brain_state.bin

Configuration Examples:

Create a custom config file (my_config.cfg):
  # Custom NLM configuration
  random_seed = 123
  simulation_timestep = 0.001
  neuron_count = 1000
  region_count = 2
  connection_probability = 0.2
  plasticity_learning_rate = 0.02
  stdp_ltp_weight = 0.025
  stdp_ltd_weight = 0.02

Batch Processing Examples:

1. Run multiple experiments in sequence:
   ./run_batch.sh plasticity:steps=1000 neurons=500 stdp:steps=500 stdp_ltp_weight=0.025

2. Dry run to see what would be executed:
   ./run_batch.sh --dry-run integration:steps=500 neurons=200

3. Validate configuration before running:
   ./validate_config.sh my_config.cfg
   ./run_batch.sh integration:steps=1000

Development Workflow:

1. Development and testing:
   $ cd build
   $ cmake --build . --target nlm_test
   $ ./nlm_test

2. Run with specific environment:
   $ ./nlm --config configs/test.cfg --run integration --debug

3. Create and test new experiments:
   Edit src/experiments/Phase6Experiment.cpp
   Rebuild and test

4. Performance optimization:
   ./run_batch.sh benchmark:mode=detailed neurons=2000
   Check results in experiment_results/

Troubleshooting:

1. "Command not found" error:
   - Build the project first: make -j4
   - Check executable path: ls -la nlm

2. Configuration file errors:
   - Use validation: ./validate_config.sh config_file.cfg
   - Fix errors and retry

3. Memory issues:
   - Reduce neuron_count in config
   - Use smaller steps for debugging

4. Performance issues:
   - Use benchmark mode: ./run_batch.sh benchmark:mode=quick
   - Check system resources

Advanced Features:

1. Checkpoint and resume:
   $ ./nlm --run integration --checkpoint save_point.bin
   # Later...
   $ ./nlm --load save_point.bin --run integration

2. Parameter sweeps:
   Create shell script with different parameters
   Use run_batch.sh for batch processing

3. Experiment comparison:
   Run multiple experiments with different seeds
   Compare results in experiment_results/

4. Performance profiling:
   Enable with --profile flag
   Use benchmark tools

Documentation:

- README.md - Complete project documentation
- HOW_TO_USE.md - User guide
- docs/ARCHITECTURE.md - System architecture
- docs/SCIENCE.md - Scientific background

Getting Help:

- Project repository: https://github.com/project/nlm
- Issues and discussions
- Community forums

Quick Commands:

Build:    mkdir build && cd build && cmake .. && make
Test:     ./validate_config.sh configs/default.cfg
Run:      ./run_batch.sh integration:steps=1000
Help:     ./quickstart.sh --help

Happy experimenting with NLM!
EOF
    exit 0
}

main() {
    # Parse command line arguments
    if [[ $# -eq 0 ]]; then
        print_help
        exit 0
    fi
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help)
                print_help
                exit 0
                ;;
            --validate)
                if [[ -n "$2" ]]; then
                    ./validate_config.sh "$2"
                    shift 2
                else
                    print_error "Missing config file for validation"
                    print_help
                    exit 1
                fi
                ;;
            --run)
                if [[ -n "$2" ]]; then
                    print_info "To run NLM, use: $NLM_EXECUTABLE --run $2 [options]"
                    print_info "Examples:"
                    print_info "  $NLM_EXECUTABLE --run $2 --steps 1000"
                    print_info "  $NLM_EXECUTABLE --run $2 --debug"
                    shift 2
                else
                    print_error "Missing run mode"
                    print_help
                    exit 1
                fi
                ;;
            --build-info)
                print_info "Build Information:"
                print_info "NLM Executable: $NLM_EXECUTABLE"
                if [[ -f "$NLM_EXECUTABLE" ]]; then
                    print_success "Executable found"
                    local size=$(stat -c%s "$NLM_EXECUTABLE" 2>/dev/null || stat -f%z "$NLM_EXECUTABLE" 2>/dev/null)
                    print_info "Size: $((size / 1024 / 1024)) MB"
                else
                    print_error "Executable not found. Build project first."
                fi
                shift
                ;;
            --check-deps)
                print_info "Checking NLM dependencies..."
                # Check for basic requirements
                if command -v cmake >/dev/null 2>&1; then
                    print_success "CMake found: $(cmake --version | head -1)"
                else
                    print_error "CMake not found"
                fi
                
                if command -v make >/dev/null 2>&1; then
                    print_success "Make found: $(make --version | head -1)"
                else
                    print_error "Make not found"
                fi
                
                if command -v g++ >/dev/null 2>&1; then
                    print_success "g++ found: $(g++ --version | head -1)"
                elif command -v clang++ >/dev/null 2>&1; then
                    print_success "clang++ found: $(clang++ --version | head -1)"
                else
                    print_error "No C++ compiler found"
                fi
                
                # Check for build directory
                if [[ -d "build" ]]; then
                    print_success "Build directory found"
                    if [[ -f "build/nlm" ]]; then
                        print_success "NLM executable found in build directory"
                    else
                        print_warning "NLM executable not found in build directory"
                    fi
                else
                    print_warning "Build directory not found"
                    print_info "To build: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
                fi
                shift
                ;;
            -*)
                print_error "Unknown option: $1"
                print_help
                exit 1
                ;;
            *)
                print_error "Unknown argument: $1"
                print_help
                exit 1
                ;;
        esac
    done
}

# Run main function with all arguments
main "$@"