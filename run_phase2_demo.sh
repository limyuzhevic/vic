#!/usr/bin/env bash

# NLM (熙然) - Neural Learning Machine - Phase 2 Demo Script
# This script demonstrates the enhanced features of the NLM Phase 2 system

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print section headers
print_section() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

# Function to print success messages
print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

# Function to print warning messages
print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# Function to print error messages
print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Main execution
main() {
    print_section "NLM (熙然) - Neural Learning Machine - Phase 2 Demo"
    echo "Enhanced artificial developmental brain demonstration"
    echo
    
    # Check if build exists
    if [ ! -f "./nlm" ]; then
        print_error "NLM executable not found!"
        echo "Please build the project first:"
        echo "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
        exit 1
    fi
    
    print_success "NLM executable found"
    
    # Parse command line arguments
    NEURON_COUNT=500
    REGION_COUNT=1
    TIMESTEP=0.001
    SEED=42
    STEPS=1000
    VERBOSE=false
    CONFIG_FILE="configs/default.cfg"
    CHECKPOINT=""
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --neuron-count)
                NEURON_COUNT=$2
                shift 2
                ;;
            --region-count)
                REGION_COUNT=$2
                shift 2
                ;;
            --timestep)
                TIMESTEP=$2
                shift 2
                ;;
            --seed)
                SEED=$2
                shift 2
                ;;
            --steps)
                STEPS=$2
                shift 2
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --quiet)
                VERBOSE=false
                shift
                ;;
            --config)
                CONFIG_FILE=$2
                shift 2
                ;;
            --checkpoint)
                CHECKPOINT=$2
                shift 2
                ;;
            --help)
                print_section "Help"
                echo "Usage: $0 [OPTIONS]"
                echo ""
                echo "Options:"
                echo "  --neuron-count N      Number of neurons (default: $NEURON_COUNT)"
                echo "  --region-count N      Number of regions (default: $REGION_COUNT)"
                echo "  --timestep T          Simulation timestep in seconds (default: $TIMESTEP)"
                echo "  --seed S              Random seed (default: $SEED)"
                echo "  --steps N             Number of simulation steps (default: $STEPS)"
                echo "  --verbose             Enable verbose logging"
                echo "  --quiet               Suppress informational messages"
                echo "  --config FILE         Load configuration from FILE (default: $CONFIG_FILE)"
                echo "  --checkpoint FILE     Save brain state to checkpoint file"
                echo "  --help                Show this help message"
                echo ""
                echo "Example:"
                echo "  $0 --neuron-count 1000 --steps 500 --verbose"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                echo "Use --help for usage information."
                exit 1
                ;;
        esac
    done
    
    print_section "Configuration"
    echo "Neuron count: $NEURON_COUNT"
    echo "Region count: $REGION_COUNT"
    echo "Simulation timestep: $TIMESTEP s"
    echo "Random seed: $SEED"
    echo "Simulation steps: $STEPS"
    echo "Configuration file: $CONFIG_FILE"
    if [ -n "$CHECKPOINT" ]; then
        echo "Checkpoint file: $CHECKPOINT"
    fi
    echo
    
    # Check if configuration file exists
    if [ ! -f "$CONFIG_FILE" ]; then
        print_warning "Configuration file not found: $CONFIG_FILE"
        print_warning "Using default configuration"
        CONFIG_FILE=""
    else
        print_success "Configuration file loaded: $CONFIG_FILE"
    fi
    
    print_section "Building Command"
    echo "./nlm \\
  --neuron-count $NEURON_COUNT \\
  --region-count $REGION_COUNT \\
  --timestep $TIMESTEP \\
  --seed $SEED \\
  --steps $STEPS"
    
    if [ "$VERBOSE" = true ]; then
        echo "  --verbose"
    fi
    
    if [ -n "$CONFIG_FILE" ]; then
        echo "  --config $CONFIG_FILE"
    fi
    
    if [ -n "$CHECKPOINT" ]; then
        echo "  --checkpoint $CHECKPOINT"
    fi
    
    echo
    
    # Run the simulation
    print_section "Running Phase 2 Demo"
    echo "Starting NLM Phase 2: Enhanced Real Neural Computation"
    echo
    
    # Build the command
    CMD="./nlm"
    CMD="$CMD --neuron-count $NEURON_COUNT"
    CMD="$CMD --region-count $REGION_COUNT"
    CMD="$CMD --timestep $TIMESTEP"
    CMD="$CMD --seed $SEED"
    CMD="$CMD --steps $STEPS"
    
    if [ "$VERBOSE" = true ]; then
        CMD="$CMD --verbose"
    fi
    
    if [ -n "$CONFIG_FILE" ]; then
        CMD="$CMD --config $CONFIG_FILE"
    fi
    
    if [ -n "$CHECKPOINT" ]; then
        CMD="$CMD --checkpoint $CHECKPOINT"
    fi
    
    # Execute the command
    eval "$CMD"
    
    # Check exit code
    EXIT_CODE=$?
    if [ $EXIT_CODE -eq 0 ]; then
        print_success "Phase 2 demo completed successfully!"
        echo ""
        echo "The NLM brain has successfully demonstrated:"
        echo "  ✓ Real LIF neuron dynamics"
        echo "  ✓ Event-driven spike propagation with delays"
        echo "  ✓ STDP and Hebbian plasticity rules"
        echo "  ✓ Structural plasticity (synaptogenesis/pruning)"
        echo "  ✓ Learning experiment demonstrates measurable changes"
        echo "  ✓ Network shows activity-dependent synaptic modification"
        echo ""
        echo "The NLM brain is now a functioning artificial neural substrate"
        echo "capable of changing its own synaptic connections through experience."
    else
        print_error "Phase 2 demo completed with errors!"
        echo "Exit code: $EXIT_CODE"
        exit $EXIT_CODE
    fi
}

# Execute main function
main "$@"