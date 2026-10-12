#!/usr/bin/env bash
# NLM Experiment Runner Script

# This script provides convenient access to NLM experiments with customizable parameters

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default parameters
EXPERIMENT="plasticity-learning"
NEURONS=500
STEPS=1000
REGIONS=1
OUTPUT_DIR="results"
SEED=42
DEBUG=""
HELP=0

# Print help
print_help() {
    cat << EOF
NLM Experiment Runner
======================

Usage: nlm-experiment [OPTIONS]

Options:
  --experiment NAME         Run specific experiment (default: plasticity-learning)
  --neurons COUNT           Number of neurons (default: 500)
  --steps COUNT             Simulation steps (default: 1000)
  --regions COUNT           Number of brain regions (default: 1)
  --output DIR              Output directory for results (default: results)
  --seed NUM                Random seed (default: 42)
  --debug                   Enable debug mode
  --help                    Show this help

Available Experiments:
  basic-connectivity         Test neural connectivity
  plasticity-learning        Comprehensive plasticity learning
  stdp-verification          STDP rule verification
  memory-formation          Episodic memory formation
  development-study          Brain development progression
  activity-pattern          Neural activity pattern analysis
  adaptation-test           Synaptic adaptation mechanisms
  reward-learning           Reward-based learning with dopamine
  curiosity-driven          Intrinsic motivation and exploration
  novelty-detection         Pattern novelty detection

Examples:
  nlm-experiment --experiment plasticity-learning --steps 5000 --neurons 1000
  nlm-experiment --experiment memory-formation --output ./memory_results
  nlm-experiment --experiment development-study --debug --seed 123
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --experiment)
            EXPERIMENT="$2"
            shift 2
            ;;
        --neurons)
            NEURONS="$2"
            shift 2
            ;;
        --steps)
            STEPS="$2"
            shift 2
            ;;
        --regions)
            REGIONS="$2"
            shift 2
            ;;
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --seed)
            SEED="$2"
            shift 2
            ;;
        --debug)
            DEBUG="--debug"
            shift
            ;;
        --help)
            HELP=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            print_help
            exit 1
            ;;
    esac
done

if [[ $HELP -eq 1 ]]; then
    print_help
    exit 0
fi

# Check if NLM binary exists
if [[ ! -f "./nlm" ]]; then
    echo -e "${RED}Error: NLM binary not found. Please build the project first.${NC}"
    echo "Run: mkdir build && cd build && cmake .. && make"
    exit 1
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Build command
CMD="./nlm --experiment $EXPERIMENT --neurons $NEURONS --steps $STEPS --regions $REGIONS --seed $SEED --output $OUTPUT_DIR $DEBUG"

echo -e "${BLUE}Running NLM Experiment${NC}"
echo "========================="
echo "Experiment: $EXPERIMENT"
echo "Neurons: $NEURONS"
echo "Steps: $STEPS"
echo "Regions: $REGIONS"
echo "Seed: $SEED"
echo "Output: $OUTPUT_DIR"
echo
echo "Command: $CMD"
echo
echo -e "${YELLOW}Starting simulation...${NC}"

# Run the simulation
eval $CMD

# Check exit code
if [[ $? -eq 0 ]]; then
    echo
echo -e "${GREEN}Experiment completed successfully!${NC}"
    echo "Results saved to: $OUTPUT_DIR"
else
    echo
echo -e "${RED}Experiment failed!${NC}"
    exit 1
fi
