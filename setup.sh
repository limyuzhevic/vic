#!/bin/bash
# NLM Phase 6 Setup Script
# This script provides easy setup and quick start commands for NLM

set -e

PROJECT_NAME="NLM Phase 6 - Neural Learning Machine"
VERSION="0.1.0 (Phase 6 - Final Integration)"

function print_header() {
    echo "===================================="
    echo "$PROJECT_NAME"
    echo "Version: $VERSION"
    echo "===================================="
    echo ""
}

function print_usage() {
    print_header
    echo "Available setup and quick start options:"
    echo ""
    echo "1. Quick Build & Run (Recommended for first-time users)"
    echo "2. Full Development Setup"
    echo "3. Project Information"
    echo "4. Configuration Setup"
    echo "5. Quick Tests"
    echo ""
    echo "Usage: ./setup.sh [option]"
    echo ""
    echo "Options:"
    echo "  1, quick     Run quick build and integration demo"
    echo "  2, full      Full development setup (with testing)"
    echo "  3, info      Show project information and requirements"
    echo "  4, config    Setup configuration files"
    echo "  5, test      Run quick unit tests"
    echo "  help         Show this help message"
}

function quick_build_and_run() {
    print_header
    echo "Running Quick Build & Integration Demo..."
    echo ""
    
    # Check if build directory exists
    if [ ! -d "build" ]; then
        echo "Creating build directory..."
        mkdir -p build
    fi
    
    # Configure with CMake
    echo "Configuring CMake..."
    cd build && cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
    
    # Build
    echo "Building NLM Phase 6..."
    cmake --build . -- -j$(nproc)
    
    # Check if build was successful
    if [ -f "nlm" ]; then
        echo ""
        echo "Build successful! Running integration demo..."
        echo ""
        # Run with default demo
        echo "============================================================"
        echo "Running: ./nlm --experiment demo"
        echo "============================================================"
        ./nlm --experiment demo
        
        # Ask if user wants to run more tests
        echo ""
        echo "Would you like to run additional tests? (y/n)"
        read -r answer
        case $answer in
            y|Y)
                run_additional_tests
                ;;
            *)
                echo "Thanks for trying NLM Phase 6!"
                ;;
        esac
    else
        echo "Build failed! Check the output above for errors."
        exit 1
    fi
}

function full_development_setup() {
    print_header
    echo "Running Full Development Setup..."
    echo ""
    
    # Create build directory
    mkdir -p build
    
    # Configure CMake
    echo "1. Configuring CMake with full features..."
    cd build && cmake -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_CXX_FLAGS="-O0 -g -DDEBUG -Wall -Wextra" \
        -DTESTS_ENABLED=ON \
        -DBENCHMARKS_ENABLED=ON ..
    
    # Build
    echo "2. Building NLM with all features..."
    cmake --build . -- -j$(nproc)
    
    # Run tests
    echo "3. Running unit tests..."
    if command -v ctest &> /dev/null; then
        ctest --output-on-failure
    else
        echo "CTest not available. Skipping tests."
    fi
    
    # Run benchmarks
    echo "4. Running performance benchmarks..."
    if [ -f "nlm" ]; then
        ./nlm --benchmark --verbose
    else
        echo "Executable not found. Skipping benchmarks."
    fi
    
    echo ""
    echo "Full development setup complete!"
    echo ""
    echo "Next steps:"
    echo "- To build in Release mode: make build in build/"
    echo "- To run demo: make run in build/"
    echo "- To run tests: make test in build/"
}

function project_info() {
    print_header
    echo "Project Information:"
    echo ""
    echo "NLM (熙然) - Neural Learning Machine"
    echo "Phase 6: FINAL INTEGRATION"
    echo ""
    echo "Architecture:"
    echo "• Event-driven spiking neural network"
    echo "• Leaky Integrate-and-Fire (LIF) neurons"
    echo "• Working memory with persistent activity"
    echo "• Episodic memory with experience replay"
    echo "• Associative memory with Hebbian patterns"
    echo "• Neuromodulation (dopamine, curiosity, novelty)"
    echo "• Prediction systems with prediction error"
    echo "• Neural planning and concept formation"
    echo "• Developmental system with age-dependent plasticity"
    echo "• Structural plasticity (synaptogenesis/pruning)"
    echo ""
    echo "Key Features:"
    echo "✓ Closed-loop interaction with environment"
    echo "✓ Experience-driven synaptic plasticity"
    echo "✓ Multi-timescale processing"
    echo "✓ Memory consolidation during rest/sleep"
    echo "✓ Real-time learning capabilities"
    echo ""
    echo "System Requirements:"
    echo "• C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 19+)"
    echo "• CMake 3.16 or higher"
    echo "• Make or Ninja build system"
    echo "• Core libraries: stdc++/c++
"
    echo ""
    echo "Documentation:"
    echo "• README.md - User documentation and quick start"
    echo "• docs/ - Technical documentation"
    echo "• configs/ - Configuration files"
    echo "• Makefile - Simplified build commands"
    echo ""
    echo "Examples and Demos:"
    echo "• ./nlm --experiment demo - Full integration demo"
    echo "• ./nlm --experiment plasticity - Plasticity learning experiment"
    echo "• ./nlm --experiment stdp - STDP experiment"
    echo "• ./nlm --benchmark - Performance benchmark"
}

function setup_configuration() {
    print_header
    echo "Setting up configuration files..."
    echo ""
    
    # Create configs directory if it doesn't exist
    mkdir -p configs
    
    # Create default config if it doesn't exist
    if [ ! -f "configs/default.cfg" ]; then
        echo "Creating default configuration..."
        cat > configs/default.cfg << 'EOF'
# NLM Configuration File
# Phase 6: FINAL INTEGRATION Configuration
# This configuration enables all integrated systems for complete brain functionality

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of brain regions
region_count = 3

# Connection probability for random connectivity
connection_probability = 0.15

# Plasticity settings
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Neuromodulation settings
dopamine_baseline = 0.1
curiosity_rate = 0.3
novelty_threshold = 0.1

# Memory system settings
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
associative_memory_pattern_count = 500

# Environment settings
environment_name = ComplexWorld
environment_width = 20
environment_height = 20

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 100000
integration_experiment_enabled = true
EOF
        echo "✓ Created: configs/default.cfg"
    else
        echo "✓ Default config already exists: configs/default.cfg"
    fi
    
    # Create basic config if it doesn't exist
    if [ ! -f "configs/basic.cfg" ]; then
        echo "Creating basic configuration..."
        cat > configs/basic.cfg << 'EOF'
# NLM Configuration File
# Phase 6: BASIC Configuration
# Simplified configuration for quick testing

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 100

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Basic Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Basic Neuromodulation settings
dopamine_baseline = 0.1

# Basic Memory system settings
working_memory_capacity = 50
episodic_memory_max_episodes = 100

# Simple Environment
environment_name = SimpleGridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm_basic.log

# Basic Simulation settings
max_simulation_steps = 10000
integration_experiment_enabled = true
EOF
        echo "✓ Created: configs/basic.cfg"
    else
        echo "✓ Basic config already exists: configs/basic.cfg"
    fi
    
    # Create custom template
    if [ ! -f "configs/custom_template.cfg" ]; then
        echo "Creating custom configuration template..."
        cat > configs/custom_template.cfg << 'EOF'
# Custom NLM Configuration Template
# Copy this file to default.cfg and modify as needed

# ===== CORE SETTINGS =====
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 3
connection_probability = 0.15

# ===== PLASTICITY SETTINGS =====
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
hebbian_learning_rate = 0.01
structural_plasticity_rate = 0.0001

# ===== DEVELOPMENT SETTINGS =====
development_synaptogenesis_rate = 0.0005
development_pruning_rate = 0.00005
development_plasticity_boost = 2.0

# ===== NEUROMODULATION SETTINGS =====
dopamine_baseline = 0.1
dopamine_learning_rate = 0.05
dopamine_time_constant = 100.0
curiosity_rate = 0.3
novelty_threshold = 0.1
prediction_error_discount = 0.95

# ===== MEMORY SYSTEM SETTINGS =====
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
associative_memory_pattern_count = 500

# ===== COGNITIVE SYSTEM SETTINGS =====
neural_planner_depth = 5
concept_formation_threshold = 0.5
attention_inhibition_strength = 0.5
attention_excitation_strength = 1.5

# ===== PREDICTION SYSTEM SETTINGS =====
prediction_accuracy_threshold = 0.8
prediction_error_threshold = 0.1
prediction_update_rate = 0.1

# ===== ENVIRONMENT SETTINGS =====
environment_name = ComplexWorld
environment_width = 20
environment_height = 20
max_steps_per_episode = 100

# ===== LOGGING SETTINGS =====
log_level = INFO
log_to_file = true
log_filename = nlm.log
log_file_max_size = 100MB
log_rotation_count = 5

# ===== SIMULATION SETTINGS =====
max_simulation_steps = 100000
simulation_time_limit = 0.0
checkpoint_interval = 1000
replay_interval = 100
consolidation_interval = 1000

# ===== PERFORMANCE SETTINGS =====
use_simd = true
use_parallel_processing = true
use_memory_pools = true
max_concurrent_threads = 4

# ===== EXPERIMENTAL SETTINGS =====
integration_experiment_enabled = true
integration_test_steps = 5000
integration_benchmark_mode = false
integration_detailed_logging = true
EOF
        echo "✓ Created: configs/custom_template.cfg"
    else
        echo "✓ Custom template already exists: configs/custom_template.cfg"
    fi
    
    echo ""
    echo "Configuration setup complete!"
    echo ""
    echo "Configuration files created in: configs/"
    echo "- default.cfg - Complete Phase 6 configuration"
    echo "- basic.cfg   - Simplified configuration for quick testing"
    echo "- custom_template.cfg - Template for creating custom configurations"
}

function run_additional_tests() {
    echo ""
    echo "Running additional tests..."
    echo ""
    
    # Check if executable exists
    if [ ! -f "build/nlm" ]; then
        echo "Executable not found. Skipping additional tests."
        return
    fi
    
    # Test 1: Basic connectivity
    echo "1. Running basic connectivity test..."
    build/nlm --experiment demo 2>&1 | tail -20
    
    # Test 2: Plasticity experiment
    echo "2. Running plasticity experiment..."
    build/nlm --experiment plasticity 2>&1 | tail -20
    
    # Test 3: STDP experiment
    echo "3. Running STDP experiment..."
    build/nlm --experiment stdp 2>&1 | tail -20
    
    # Test 4: Benchmark
    echo "4. Running performance benchmark..."
    build/nlm --benchmark --verbose 2>&1 | tail -30
}

function run_quick_tests() {
    print_header
    echo "Running Quick Unit Tests..."
    echo ""
    
    # Check if we can run CMake tests
    if [ ! -d "build" ]; then
        echo "Build directory not found. Running setup first..."
        quick_build_and_run
        return
    fi
    
    echo "Note: Full test suite requires CTest. For quick tests, we recommend:"
    echo ""
    echo "1. Running integration demo:"
    echo "   build/nlm --experiment demo"
    echo ""
    echo "2. Running plasticity experiment:"
    echo "   build/nlm --experiment plasticity"
    echo ""
    echo "3. Running STDP experiment:"
    echo "   build/nlm --experiment stdp"
    echo ""
    echo "4. Running performance benchmark:"
    echo "   build/nlm --benchmark --verbose"
}

# Main script logic
case "$1" in
    "quick"|"1")
        quick_build_and_run
        ;;
    "full"|"2")
        full_development_setup
        ;;
    "info"|"3")
        project_info
        ;;
    "config"|"4")
        setup_configuration
        ;;
    "test"|"5")
        run_quick_tests
        ;;
    "help"|"--help"|"-h")
        print_usage
        ;;
    *)
        print_usage
        echo ""
        echo "Error: Invalid option or no argument provided."
        echo "Run './setup.sh help' for usage information."
        exit 1
        ;;
esac
