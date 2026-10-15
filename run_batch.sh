#!/bin/bash
# NLM Phase 6 Experiment Batch Runner
# 
# This script provides batch processing capabilities for NLM Phase 6 experiments.
# It allows running multiple experiments with different configurations and parameters.
# 
# Usage:
#   ./run_batch.sh experiment1_param1 experiment1_param2 ...
#   ./run_batch.sh --help
# 
# Supported experiments:
#   plasticity    - Plasticity learning experiments
#   stdp         - STDP synaptic learning experiments  
#   integration   - Full integration tests
#   benchmark     - Performance benchmarking
#   learning      - Learning curve analysis
#   development   - Developmental process simulation
# 
# Batch parameter format:
#   experiment_name:key=value
#   experiment_name:key=value;key2=value2
# 
# Examples:
#   ./run_batch.sh --neurons=1000 --plasticity-config
#   ./run_batch.sh plasticity:steps=2000 stdp:tau=15.0 development:stage=CriticalPeriod
#   ./run_batch.sh benchmark:mode=detailed integration:systime=60

#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
NLM_EXECUTABLE="./nlm"
DEFAULT_CONFIG="configs/default.cfg"
RESULTS_DIR="experiment_results"
LOG_DIR="experiment_logs"

# Create directories
mkdir -p "$RESULTS_DIR"
mkdir -p "$LOG_DIR"

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() "$1"

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_help() {
    cat << EOF
NLM Phase 6 Batch Experiment Runner
===================================

Usage:
  ./run_batch.sh [OPTIONS] [EXPERIMENTS]

Options:
  --help                    Show this help message
  --config FILE             Configuration file (default: $DEFAULT_CONFIG)
  --output DIR              Output directory for results (default: $RESULTS_DIR)
  --verbose                 Enable verbose output
  --dry-run                 Show what would be run without executing

Experiments:
  plasticity[:param1=value1[:param2=value2]]...
  stdp[:param1=value1[:param2=value2]]...
  integration[:param1=value1[:param2=value2]]...
  benchmark[:param1=value1[:param2=value2]]...
  learning[:param1=value1[:param2=value2]]...
  development[:param1=value1[:param2=value2]]...
  memory[:param1=value1[:param2=value2]]...

Example runs:
  # Run single experiment with custom parameters
  ./run_batch.sh plasticity:steps=2000 neurons=1500
  
  # Run multiple experiments
  ./run_batch.sh plasticity:steps=1000 neurons=1000 stdp:tau=15.0 integration:systime=30
  
  # Run with custom configuration
  ./run_batch.sh --config my_config.cfg integration benchmark
  
  # Dry run to see what would be executed
  ./run_batch.sh --dry-run integration:steps=500 stdp:tau=20.0

Experiment parameters (key=value):

Plasticity:
  steps      Number of simulation steps (default: 1000)
  neurons    Number of neurons (default: 500)
  regions    Number of brain regions (default: 1)
  plasticity_learning_rate Learning rate for plasticity (default: 0.01)

STDP:
  steps      Number of simulation steps (default: 1000)
  neurons    Number of neurons (default: 500)
  stdp_ltp_weight  LTP weight (default: 0.02)
  stdp_ltd_weight  LTD weight (default: 0.015)
  stdp_time_constant STDP time constant in ms (default: 20.0)

Integration:
  steps      Number of simulation steps (default: 1000)
  neurons    Number of neurons (default: 500)
  regions    Number of brain regions (default: 1)
  systime    System time in seconds (default: 0.0)

Benchmark:
  mode       Benchmark mode (detailed, quick, comprehensive) (default: detailed)
  duration   Benchmark duration in seconds (default: 300)
  steps      Number of steps to benchmark (default: 10000)
  neurons    Number of neurons to benchmark (default: 500)

Learning:
  steps      Number of simulation steps (default: 2000)
  neurons    Number of neurons (default: 500)
  analysis   Analysis type (synaptic_weights, firing_rates, connectivity) (default: synaptic_weights)

Development:
  steps      Number of simulation steps (default: 2000)
  neurons    Number of neurons (default: 500)
  stage      Developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging) (default: CriticalPeriod)
  plasticity_modulation Plasticity modulation factor (default: 1.0)

Memory:
  steps      Number of simulation steps (default: 1000)
  neurons    Number of neurons (default: 500)
  capacity   Memory capacity (default: 10000)
  forgetting_rate Forgetting rate (default: 0.001)

EOF
    exit 0
}

# Function to parse experiment parameters
parse_experiment_params() {
    local params_string="$1"
    declare -A result
    
    IFS=':' read -r experiment_name extra_params <<< "$params_string"
    
    # Parse key=value pairs
    while [[ -n "$extra_params" ]]; do
        # Extract next key=value pair
        local pair="${extra_params%%:*}"
        extra_params="${extra_params#*:}"
        
        IFS='=' read -r key value <<< "$pair"
        result["$key"]="$value"
    done
    
    echo "$experiment_name"
    for key in "${!result[@]}"; do
        echo "$key:${result[$key]}"
    done
}

# Function to run a single experiment
generate_experiment_config() {
    local experiment_name="$1"
    shift
    
    local config_file="$RESULTS_DIR/experiment_${experiment_name}_$(date +%Y%m%d_%H%M%S).cfg"
    
    # Create experiment-specific config
    cat > "$config_file" << EOF
# NLM Phase 6 Experiment Configuration
# Experiment: $experiment_name
# Generated on: $(date)

# Basic settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation settings
dopamine_scale = 1.0

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = true
log_filename = ${LOG_DIR}/experiment_${experiment_name}_$(date +%Y%m%d_%H%M%S).log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
EOF
    
    # Add experiment-specific parameters
    for param in "$@"; do
        IFS='=' read -r key value <<< "$param"
        if [[ -n "$key" && -n "$value" ]]; then
            # Convert to appropriate format
            if [[ "$key" == "neurons" || "$key" == "steps" || "$key" == "regions" ]]; then
                echo "${key} = $value" >> "$config_file"
            elif [[ "$key" =~ ^[0-9]+$ ]]; then
                echo "${key} = $value" >> "$config_file"
            elif [[ "$value" =~ ^[0-9]+$ ]]; then
                echo "${key} = $value" >> "$config_file"
            elif [[ "$value" =~ ^[0-9]+\.[0-9]+$ ]]; then
                echo "${key} = $value" >> "$config_file"
            elif [[ "$value" == "true" || "$value" == "false" ]]; then
                echo "${key} = $value" >> "$config_file"
            else
                echo "${key} = \"$value\"" >> "$config_file"
            fi
        fi
    done
    
    echo "$config_file"
}

run_experiment() {
    local experiment_config="$1"
    local experiment_name="$(basename "$experiment_config" .cfg)"
    
    print_info "Running experiment: $experiment_name"
    print_info "Config file: $experiment_config"
    
    # Build the command
    local cmd="$NLM_EXECUTABLE --config $experiment_config --run integration"
    
    if [[ "$VERBOSE" == "true" ]]; then
        print_info "Command: $cmd"
    fi
    
    # Run the experiment
    local output_file="$RESULTS_DIR/${experiment_name}_output_$(date +%Y%m%d_%H%M%S).txt"
    local error_file="$RESULTS_DIR/${experiment_name}_error_$(date +%Y%m%d_%H%M%S).txt"
    
    if [[ "$DRY_RUN" == "true" ]]; then
        print_info "[DRY RUN] Would execute: $cmd"
        print_info "[DRY RUN] Output would be saved to: $output_file"
        print_info "[DRY RUN] Error would be saved to: $error_file"
    else
        print_info "Executing experiment..."
        
        # Run with timeout
        timeout 3000s $cmd > "$output_file" 2> "$error_file"
        
        local exit_code=$?
        
        if [[ $exit_code -eq 0 ]]; then
            print_success "Experiment $experiment_name completed successfully"
            
            # Extract key metrics from output
            if [[ -f "$output_file" ]]; then
                local metrics_file="$RESULTS_DIR/${experiment_name}_metrics_$(date +%Y%m%d_%H%M%S).json"
                cat << EOF > "$metrics_file"
{
  "experiment": "$experiment_name",
  "timestamp": "$(date -Iseconds)",
  "output_file": "$output_file",
  "exit_code": $exit_code,
  "status": "completed"
}
EOF
                print_info "Experiment metrics saved to: $metrics_file"
            fi
        else
            print_error "Experiment $experiment_name failed with exit code: $exit_code"
            
            # Create error metrics file
            local metrics_file="$RESULTS_DIR/${experiment_name}_error_$(date +%Y%m%d_%H%M%S).json"
            cat << EOF > "$metrics_file"
{
  "experiment": "$experiment_name",
  "timestamp": "$(date -Iseconds)",
  "output_file": "$output_file",
  "error_file": "$error_file",
  "exit_code": $exit_code,
  "status": "failed"
}
EOF
            print_info "Error metrics saved to: $metrics_file"
            
            if [[ -f "$error_file" && -s "$error_file" ]]; then
                print_warning "Error output (first 500 chars):"
                head -c 500 "$error_file" | cat -v
                echo
            fi
        fi
    fi
}

# Main script execution
main() {
    # Parse command line arguments
    CONFIG_FILE="$DEFAULT_CONFIG"
    VERBOSE=false
    DRY_RUN=false
    
    # Process flags
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help)
                print_help
                exit 0
                ;;
            --config)
                CONFIG_FILE="$2"
                shift 2
                ;;
            --output)
                RESULTS_DIR="$2"
                shift 2
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --dry-run)
                DRY_RUN=true
                shift
                ;;
            --)
                shift
                break
                ;;
            -*)
                print_error "Unknown option: $1"
                print_help
                exit 1
                ;;
            *)
                break
                ;;
        esac
    done
    
    # Validate config file
    if [[ ! -f "$CONFIG_FILE" ]]; then
        print_error "Config file not found: $CONFIG_FILE"
        print_error "Available config files:"
        ls -la "configs/" 2>/dev/null || print_error "configs directory not found"
        exit 1
    fi
    
    # Create results directory
    mkdir -p "$RESULTS_DIR"
    
    # Check if NLM executable exists
    if [[ ! -f "$NLM_EXECUTABLE" ]]; then
        print_error "NLM executable not found: $NLM_EXECUTABLE"
        print_error "Please build the NLM project first:"
        print_error "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release"
        print_error "  make -j4"
        exit 1
    fi
    
    # Parse experiments
    EXPERIMENTS=()
    for arg in "$@"; do
        # Skip processed flags
        if [[ "$arg" == --* ]]; then
            continue
        fi
        
        # Parse experiment and parameters
        local temp_file=$(mktemp)
        parse_experiment_params "$arg" > "$temp_file"
        
        local experiment_name=$(head -n1 "$temp_file")
        local params=()
        while read -r line; do
            if [[ -n "$line" ]]; then
                params+=("$line")
            fi
        done < <(tail -n +2 "$temp_file")
        
        EXPERIMENTS+=("$experiment_name" "${params[@]}")
        rm -f "$temp_file"
    done
    
    # Run experiments
    if [[ ${#EXPERIMENTS[@]} -eq 0 ]]; then
        print_warning "No experiments specified, running default integration experiment"
        EXPERIMENTS+=("integration")
    fi
    
    print_info "Starting batch experiment run with ${#EXPERIMENTS[@]} experiment(s)"
    print_info "Results directory: $RESULTS_DIR"
    print_info "Log directory: $LOG_DIR"
    
    # Run each experiment
    for ((i=0; i<${#EXPERIMENTS[@]}; i+=2)); do
        local experiment_name="${EXPERIMENTS[$i]}"
        shift 2
        
        # Collect all parameters for this experiment
        local params_count=$(( (${#EXPERIMENTS[@]} - i - 2) / 2 ))
        local experiment_params=()
        
        for ((j=i+2; j<${#EXPERIMENTS[@]} && j-i-2 < params_count*2; j+=2)); do
            experiment_params+=("${EXPERIMENTS[$j]}=${EXPERIMENTS[$((j+1))]}")
        done
        
        print_info "=== Processing experiment: $experiment_name ==="
        
        # Generate experiment configuration
        local config_file=$(generate_experiment_config "$experiment_name" "${experiment_params[@]}")
        
        # Run the experiment
        run_experiment "$config_file"
        
        print_info ""
    done
    
    print_success "Batch experiment run completed"
}

# Run main function with all arguments
main "$@"