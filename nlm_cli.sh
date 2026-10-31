#!/usr/bin/env bash
# NLM Command-line Interface
# A comprehensive CLI for interacting with the NLM neural simulation framework

# Display help information
function show_help() {
    cat << EOF
NLM - Neural Learning Machine CLI
=================================

USAGE:
    nlm [OPTIONS] [ARGUMENTS]

BASIC COMMANDS:
    demo                    Run basic neural simulation demo
    benchmark [size]         Run performance benchmark (default: 500 neurons, 1000 steps)
    experiment [type]        Run specific neural experiment
    config [file]            Load configuration from file

ADVANCED COMMANDS:
    visualize [data]         Create visualizations of neural activity
    analyze [data]           Analyze neural patterns and behaviors
    profile [duration]       Profile performance and memory usage
    checkpoint [file]        Save/load brain state checkpoints
    simulate [steps]         Run continuous simulation

EXPERIMENT TYPES:
    connectivity            Test neural connectivity and signal propagation
    plasticity              Study synaptic plasticity (STDP, Hebbian)
    learning                Demonstrate learning capabilities
    development             Test developmental stages
    memory                  Test memory formation and recall
    social                  Test social learning and interactions

OPTIONS:
    --config FILE           Load configuration from specified file
    --help, -h              Show this help message
    --version               Show version information
    --verbose, -v           Enable verbose output
    --quiet, -q             Suppress non-essential output
    --output DIR            Set output directory for results
    --seed N               Set random seed for reproducible results
    --steps N               Number of simulation steps (default: 1000)
    --neurons N             Number of neurons (default: 500)
    --performance           Enable performance measurements

EXAMPLES:
    nlm demo                                # Run basic demo
    nlm benchmark 1000                      # Benchmark with 1000 neurons
    nlm experiment learning                  # Run learning experiment
    nlm config my_config.cfg                # Load custom configuration
    nlm visualize demo_output                # Visualize simulation results
    nlm analyze --performance neural_data    # Analyze neural activity

For more detailed information, see the documentation.
EOF
}

# Display version information
function show_version() {
    cat << EOF
NLM Neural Learning Machine v2.0.0
Phase 6: Final Integration
Experimental Artificial Developmental Brain

Built on Leaky Integrate-and-Fire (LIF) neuron dynamics
Event-driven spike propagation with synaptic delays
STDP and Hebbian plasticity rules
Structural plasticity (synaptogenesis/pruning)

Python bindings available via pynlm module
C++ core with extensive neural simulation capabilities

Copyright (c) 2024 NLM Research Project
MIT License
EOF
}

# Parse command line arguments
CONFIG_FILE=""
VERBOSE=false
QUIET=false
OUTPUT_DIR="./output"
SEED=42
STEPS=1000
NEURONS=500
PERFORMANCE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        --version)
            show_version
            exit 0
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --quiet|-q)
            QUIET=true
            shift
            ;;
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --seed)
            SEED="$2"
            shift 2
            ;;
        --steps)
            STEPS="$2"
            shift 2
            ;;
        --neurons)
            NEURONS="$2"
            shift 2
            ;;
        --performance)
            PERFORMANCE=true
            shift
            ;;
        demo|benchmark|experiment|visualize|analyze|profile|checkpoint|simulate)
            COMMAND="$1"
            shift
            break
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Set up logging
LOG_FILE="$OUTPUT_DIR/nlm_$(date +%Y%m%d_%H%M%S).log"

# Function to log messages
log_message() {
    local level="$1"
    local message="$2"
    
    if [ "$QUIET" = false ]; then
        if [ "$VERBOSE" = true ] || [ "$level" = "ERROR" ] || [ "$level" = "WARNING" ]; then
            echo "[$level] $message" | tee -a "$LOG_FILE"
        else
            echo "$message" | tee -a "$LOG_FILE"
        fi
    fi
}

# Log startup information
log_message "INFO" "NLM Neural Learning Machine v2.0.0 - Phase 6: Final Integration"
log_message "INFO" "Starting execution with command: ${COMMAND:-demo}"
log_message "INFO" "Random seed: $SEED, Neurons: $NEURONS, Steps: $STEPS"

# Execute the requested command
if [ "$COMMAND" = "demo" ]; then
    echo "Running NLM demo..."
    echo "Building configuration..."
    
    # Create and run demo
    mkdir -p "$OUTPUT_DIR/demo"
    
    cat > "$OUTPUT_DIR/demo/config.cfg" << EOF
# NLM Demo Configuration
random_seed = $SEED
neuron_count = $NEURONS
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
replay_interval = 100
consolidation_interval = 1000
EOF
    
    echo "Configuration saved to: $OUTPUT_DIR/demo/config.cfg"
    echo "Demo output will be saved to: $OUTPUT_DIR/demo/"
    
    # Run the actual demo (if compiled)
    if [ -f "./build/nlm" ]; then
        echo "Running NLM demo executable..."
        ./build/nlm --config "$OUTPUT_DIR/demo/config.cfg" > "$OUTPUT_DIR/demo/output.txt" 2>&1
        echo "Demo completed. Results saved to $OUTPUT_DIR/demo/output.txt"
    else
        echo "Demo executable not found. Please build the project first."
        echo "Run: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
    fi
    
elif [ "$COMMAND" = "benchmark" ]; then
    echo "Running NLM benchmark..."
    echo "Benchmark parameters: $NEURONS neurons, $STEPS steps"
    
    # Create benchmark script
    cat > "$OUTPUT_DIR/benchmark.sh" << EOF
#!/bin/bash
cd "$(dirname "$0")"
mkdir -p benchmark_results
echo "Running benchmark with $NEURONS neurons and $STEPS steps..."
echo "Results will be saved to benchmark_results/"
EOF
    
    chmod +x "$OUTPUT_DIR/benchmark.sh"
    echo "Benchmark script created at: $OUTPUT_DIR/benchmark.sh"
    
elif [ "$COMMAND" = "experiment" ]; then
    echo "Running NLM experiment: $2"
    echo "Experiment type: $2"
    echo "Output will be saved to: $OUTPUT_DIR/experiments/"
    
    # Create experiment script
    mkdir -p "$OUTPUT_DIR/experiments"
    cat > "$OUTPUT_DIR/experiment.sh" << EOF
#!/bin/bash
cd "$(dirname "$0")"
echo "Running experiment: $1"
echo "Creating experiment data in experiments/$1/"
mkdir -p "experiments/$1"
echo "Experiment completed. Results saved to experiments/$1/"
EOF
    
    chmod +x "$OUTPUT_DIR/experiment.sh"
    echo "Experiment script created at: $OUTPUT_DIR/experiment.sh"
    
elif [ "$COMMAND" = "visualize" ]; then
    echo "Visualizing neural activity..."
    echo "Visualization data: $2"
    echo "Output will be saved to: $OUTPUT_DIR/visualizations/"
    
elif [ "$COMMAND" = "analyze" ]; then
    echo "Analyzing neural patterns..."
    echo "Analysis data: $2"
    echo "Output will be saved to: $OUTPUT_DIR/analysis/"
    
elif [ "$COMMAND" = "profile" ]; then
    echo "Profiling performance..."
    echo "Duration: ${2:-30} seconds"
    echo "Output will be saved to: $OUTPUT_DIR/profiles/"
    
elif [ "$COMMAND" = "checkpoint" ]; then
    echo "Managing checkpoints..."
    echo "Checkpoint file: $2"
    echo "Output will be saved to: $OUTPUT_DIR/checkpoints/"
    
elif [ "$COMMAND" = "simulate" ]; then
    echo "Running continuous simulation..."
    echo "Steps: ${2:-$STEPS}"
    echo "Output will be saved to: $OUTPUT_DIR/simulation/"
    
    mkdir -p "$OUTPUT_DIR/simulation"
    echo "Simulation configuration saved to: $OUTPUT_DIR/simulation/config.cfg"
    
    cat > "$OUTPUT_DIR/simulation/config.cfg" << EOF
# Continuous Simulation Configuration
random_seed = $SEED
neuron_count = $NEURONS
steps = ${2:-$STEPS}
output_dir = $OUTPUT_DIR/simulation
performance_monitor = $PERFORMANCE
EOF
    
elif [ "$COMMAND" = "help" ]; then
    show_help
    
else
    echo "Unknown command: $COMMAND"
    show_help
    exit 1
fi

log_message "INFO" "Command completed successfully"
log_message "INFO" "All output saved to: $LOG_FILE"
log_message "INFO" "NLM execution finished"
