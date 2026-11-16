#!/usr/bin/env bash

# NLM Command Line Interface
# Provides easy access to NLM demos, experiments, and management tools

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="${BUILD_DIR:-build}"
CONFIG_FILE="${CONFIG_FILE:-configs/default.cfg}"
DEFAULT_NEURON_COUNT=500
DEFAULT_REGION_COUNT=2
DEFAULT_STEPS=1000

# Function to print usage
usage() {
    cat << EOF
Usage: nlm <command> [options]

Commands:
  help                    Show this help message
  run <phase> [options]   Run a specific phase demo
  config <command>        Manage configuration files
  checkpoint <command>    Manage brain checkpoints
  experiment <command>     Run experiments
  monitor                 Monitor running simulation
  visualize [output]      Generate visualizations

Options for 'run':
  --phase <N>             Phase number (2, 3, 4, 5, 6)
  --neurons <N>           Number of neurons (default: $DEFAULT_NEURON_COUNT)
  --regions <N>           Number of regions (default: $DEFAULT_REGION_COUNT)
  --steps <N>             Simulation steps (default: $DEFAULT_STEPS)
  --config <file>         Configuration file (default: $CONFIG_FILE)
  --checkpoint <file>     Load from checkpoint
  --save <file>           Save checkpoint after simulation
  --verbose               Enable verbose logging

Examples:
  nlm run 2                           Run Phase 2 demo
  nlm run 3 --neurons 1000 --steps 5000 Run Phase 3 with more neurons
  nlm run 6 --save brain.ckpt           Run Phase 6 and save state
  nlm config set neuron_count 2000       Set configuration value
  nlm checkpoint list                    List available checkpoints
  nlm experiment phase5_demo             Run Phase 5 demo

EOF
}

# Function to show loading message
loading() {
    echo -e "${BLUE}Loading...${NC}"
    sleep 1
    echo -e "${GREEN}✓ Loaded${NC}"
}

# Function to check if build directory exists
check_build() {
    if [[ ! -d "$BUILD_DIR" ]]; then
        echo -e "${RED}Build directory not found: $BUILD_DIR${NC}"
        echo "Please run './build.sh' first to build the project."
        exit 1
    fi
}

# Function to check if executable exists
check_executable() {
    local executable="$BUILD_DIR/$1"
    if [[ ! -f "$executable" ]]; then
        echo -e "${RED}Executable not found: $executable${NC}"
        echo "Please run './build.sh' to build the project."
        exit 1
    fi
}

# Function to run phase demos
run_phase() {
    local phase="$1"
    local neurons="$2"
    local regions="$3"
    local steps="$4"
    local config_file="$5"
    local checkpoint_file="$6"
    local save_file="$7"
    local verbose="$8"

    check_build

    local executable="$BUILD_DIR/nlm_phase${phase}_demo"
    check_executable "$executable"

    echo -e "${BLUE}Running NLM Phase ${phase} demo...${NC}"
    echo "  Neurons: $neurons"
    echo "  Regions: $regions"
    echo "  Steps: $steps"

    # Build command
    cmd="$executable --neuron_count $neurons --region_count $regions"
    cmd+=" --simulation_timestep 0.001"

    if [[ -n "$config_file" && -f "$config_file" ]]; then
        cmd+=" --config $config_file"
    fi

    if [[ -n "$checkpoint_file" && -f "$checkpoint_file" ]]; then
        # For now, checkpoint loading is limited - just show warning
        echo -e "${YELLOW}Warning: Checkpoint loading not fully implemented yet${NC}"
    fi

    # Run the demo
    if [[ "$verbose" == "true" ]]; then
        echo -e "${YELLOW}Verbose mode enabled${NC}"
        "$executable" "$cmd"
    else
        "$executable" "$cmd" 2>&1 | head -50
        echo "... (see full output above)"
    fi

    # Save checkpoint if requested
    if [[ -n "$save_file" ]]; then
        echo -e "${BLUE}Saving checkpoint to: $save_file${NC}"
        # Note: In the real implementation, the demo would save its own checkpoint
        echo -e "${YELLOW}Checkpoint saving would be implemented in full version${NC}"
    fi

    echo -e "${GREEN}Phase ${phase} demo completed!${NC}"
}

# Function to manage configuration
manage_config() {
    local command="$1"
    local key="$2"
    local value="$3"

    case "$command" in
        set)
            if [[ -z "$key" || -z "$value" ]]; then
                echo -e "${RED}Error: Both key and value required for 'set' command${NC}"
                usage
                exit 1
            fi
            echo -e "${YELLOW}Config setting would be saved to $CONFIG_FILE${NC}"
            echo "  key: $key"
            echo "  value: $value"
            echo "(Implementation in ${RED}configs/Config.hpp${NC})"
            ;;
        get)
            if [[ -z "$key" ]]; then
                echo -e "${RED}Error: Key required for 'get' command${NC}"
                usage
                exit 1
            fi
            echo -e "${YELLOW}Config get would return value for key $key${NC}"
            echo "(Implementation in ${RED}configs/Config.hpp${NC})"
            ;;
        list)
            echo -e "${YELLOW}Config list would show all configuration options${NC}"
            echo "(Implementation in ${RED}configs/Config.hpp${NC})"
            ;;
        *)
            echo -e "${RED}Unknown config command${NC}"
            usage
            exit 1
            ;;
    esac
}

# Function to list checkpoints
list_checkpoints() {
    echo -e "${YELLOW}Checkpoint listing would scan for *.ckpt files${NC}"
    echo "(Implementation in ${RED}src/performance/CheckpointSystem.cpp${NC})"
}

# Main command dispatcher
if [[ "${1}" == "run" ]]; then
    phase="${2}"
    neurons="${3:-$DEFAULT_NEURON_COUNT}"
    regions="${4:-$DEFAULT_REGION_COUNT}"
    steps="${5:-$DEFAULT_STEPS}"
    config_file="${6}"
    checkpoint_file="${7}"
    save_file="${8}"
    verbose="false"

    # Simple parsing of flags
    for arg in "${@}"; do
        if [[ "$arg" == "--neurons" ]]; then
            neurons="${4:-$DEFAULT_NEURON_COUNT}"
        fi
    done

    run_phase "$phase" "$neurons" "$regions" "$steps" "$config_file" "$checkpoint_file" "$save_file" "$verbose"
fi

# If help requested
if [[ "${1}" == "help" ]]; then
    usage
fi

# If no arguments
if [[ $# -eq 0 ]]; then
    usage
    exit 1
fi

# Show error if something went wrong
if [[ "${1}" == "run" && $# -lt 2 ]]; then
    echo -e "${RED}Error: Phase number required for 'run' command${NC}"
    usage
    exit 1
fi

exit 0
