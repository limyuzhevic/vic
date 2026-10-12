#!/usr/bin/env bash

# Helper script to run NLM experiments with different configurations

usage() {
    echo "NLM (Neural Learning Machine) Experiment Runner"
    echo ""
    echo "Usage: ./run-experiment.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --config FILE           Load configuration from file (default: configs/default.cfg)"
    echo "  --neurons N             Number of neurons (overrides config)"
    echo "  --regions R             Number of regions (overrides config)"
    echo "  --timestep T            Simulation timestep (overrides config)"
    echo "  --mode MODE             Experiment mode: basic, connectivity, plasticity, stdp, all"
    echo "  --preset PRESET         Configuration preset: default, test, benchmark"
    echo "  --save-results FILE     Save experiment results to file"
    echo "  --help                  Show this help message"
    echo ""
    echo "Experiment Modes:"
    echo "  basic       - Run basic connectivity test only"
    echo "  connectivity - Test neural connectivity and spike propagation"
    echo "  plasticity  - Run plasticity learning experiment"
    echo "  stdp        - Verify STDP learning rule"
    echo "  all         - Run all experiments sequentially"
    echo ""
    echo "Configuration Presets:"
    echo "  default     - Standard configuration for research (default)"
    echo "  test        - Small configuration for quick testing"
    echo "  benchmark   - Large configuration for performance testing"
    echo ""
    echo "Examples:"
    echo "  ./run-experiment.sh --mode connectivity --config configs/default.cfg"
    echo "  ./run-experiment.sh --mode plasticity --preset test --save-results plasticity_results.csv"
    echo "  ./run-experiment.sh --mode all --config configs/example.cfg --neurons 2000"
}

# Default values
CONFIG_FILE="configs/default.cfg"
MODE="all"
PRESET="default"
SAVE_RESULTS=""

# Parse command line arguments
while [ $# -gt 0 ]; do
    case $1 in
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --mode)
            MODE="$2"
            shift 2
            ;;
        --preset)
            PRESET="$2"
            shift 2
            ;;
        --save-results)
            SAVE_RESULTS="$2"
            shift 2
            ;;
        --neurons)
            NEURON_OVERRIDE="$2"
            shift 2
            ;;
        --regions)
            REGION_OVERRIDE="$2"
            shift 2
            ;;
        --timestep)
            TIMESTEP_OVERRIDE="$2"
            shift 2
            ;;
        --help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Check if compiled executable exists
if [ ! -f "./nlm" ]; then
    echo "Error: Compiled executable './nlm' not found."
    echo "Please compile the project first: mkdir build && cd build && cmake .. && make"
    exit 1
fi

# Check if config file exists
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Config file '$CONFIG_FILE' not found."
    echo "Available configs:"
    ls -la configs/
    exit 1
fi

echo "NLM Experiment Runner"
echo "====================="
echo "Config file: $CONFIG_FILE"
echo "Mode: $MODE"
echo "Preset: $PRESET"
if [ -n "$NEURON_OVERRIDE" ]; then
    echo "Neuron override: $NEURON_OVERRIDE"
fi
if [ -n "$REGION_OVERRIDE" ]; then
    echo "Region override: $REGION_OVERRIDE"
fi
if [ -n "$TIMESTEP_OVERRIDE" ]; then
    echo "Timestep override: $TIMESTEP_OVERRIDE"
fi
echo ""

# Build command line arguments
CMD_ARGS="--config $CONFIG_FILE"
if [ -n "$NEURON_OVERRIDE" ]; then
    CMD_ARGS="$CMD_ARGS --neuron_count $NEURON_OVERRIDE"
fi
if [ -n "$REGION_OVERRIDE" ]; then
    CMD_ARGS="$CMD_ARGS --region_count $REGION_OVERRIDE"
fi
if [ -n "$TIMESTEP_OVERRIDE" ]; then
    CMD_ARGS="$CMD_ARGS --simulation_timestep $TIMESTEP_OVERRIDE"
fi

# Set preset if specified
if [ "$PRESET" != "default" ]; then
    CMD_ARGS="$CMD_ARGS --preset $PRESET"
fi

# Run the experiment
./nlm $CMD_ARGS

# If save results was specified, copy the output
if [ -n "$SAVE_RESULTS" ] && [ -f "nlm.log" ]; then
    cp nlm.log "$SAVE_RESULTS"
    echo ""
    echo "Results saved to: $SAVE_RESULTS"
fi

echo ""
echo "Experiment completed successfully!"
