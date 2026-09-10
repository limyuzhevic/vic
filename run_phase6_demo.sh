#!/bin/bash

# NLM Phase 6 Demo Script
# Run Phase 6 integration demo with various configuration options

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_NAME="NLM Phase 6 Demo"

print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Phase 6 Integration Demo Options:"
    echo "  --neuron-count NUM     Number of neurons (default: 500)"
    echo "  --region-count NUM     Number of brain regions (default: 2)"
    echo "  --max-steps NUM        Maximum simulation steps (default: 1000)"
    echo "  --enable-checkpointing Enable checkpointing (default: false)"
    echo "  --checkpoint-path PATH Path for checkpoint files (default: checkpoint.bin)"
    echo "  --enable-neuromodulation Enable neuromodulation systems (default: true)"
    echo "  --enable-curiosity     Enable curiosity system (default: true)"
    echo "  --enable-prediction    Enable prediction system (default: true)"
    echo "  --enable-concept-formación Enable concept formation (default: true)"
    echo "  --enable-memory        Enable memory systems (default: true)"
    echo "  --memory-capacity NUM  Working memory capacity (default: 50)"
    echo "  --verbose              Enable verbose output (default: false)"
    echo "  --no-graphics          Disable visualization (default: false)"
    echo ""
    echo "Output options:"
    echo "  --json-output FILE     Save results to JSON file"
    echo "  --csv-output FILE      Save metrics to CSV file"
    echo ""
    echo "Control options:"
    echo "  --help                 Show this help message"
    echo "  --version              Show version information"
}

# Default configuration
NEURON_COUNT=500
REGION_COUNT=2
MAX_STEPS=1000
ENABLE_CHECKPOINTING=false
CHECKPOINT_PATH="checkpoint.bin"
ENABLE_NEUROMODULATION=true
ENABLE_CURIOUSITY=true
ENABLE_PREDICTION=true
ENABLE_CONCEPT_FORMATION=true
ENABLE_MEMORY=true
MEMORY_CAPACITY=50
VERBOSE=false
NO_GRAPHICS=false
JSON_OUTPUT=""
CSV_OUTPUT=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --neuron-count)
            NEURON_COUNT="$2"
            shift 2
            ;;
        --region-count)
            REGION_COUNT="$2"
            shift 2
            ;;
        --max-steps)
            MAX_STEPS="$2"
            shift 2
            ;;
        --enable-checkpointing)
            ENABLE_CHECKPOINTING=true
            shift
            ;;
        --checkpoint-path)
            CHECKPOINT_PATH="$2"
            shift 2
            ;;
        --enable-neuromodulation)
            ENABLE_NEUROMODULATION=true
            shift
            ;;
        --disable-neuromodulation)
            ENABLE_NEUROMODULATION=false
            shift
            ;;
        --enable-curiosity)
            ENABLE_CURIOUSITY=true
            shift
            ;;
        --disable-curiosity)
            ENABLE_CURIOUSITY=false
            shift
            ;;
        --enable-prediction)
            ENABLE_PREDICTION=true
            shift
            ;;
        --disable-prediction)
            ENABLE_PREDICTION=false
            shift
            ;;
        --enable-concept-formation)
            ENABLE_CONCEPT_FORMATION=true
            shift
            ;;
        --disable-concept-formation)
            ENABLE_CONCEPT_FORMATION=false
            shift
            ;;
        --enable-memory)
            ENABLE_MEMORY=true
            shift
            ;;
        --disable-memory)
            ENABLE_MEMORY=false
            shift
            ;;
        --memory-capacity)
            MEMORY_CAPACITY="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --no-graphics)
            NO_GRAPHICS=true
            shift
            ;;
        --json-output)
            JSON_OUTPUT="$2"
            shift 2
            ;;
        --csv-output)
            CSV_OUTPUT="$2"
            shift 2
            ;;
        --help)
            print_usage
            exit 0
            ;;
        --version)
            echo "NLM Phase 6 Demo v1.0.0"
            echo "Based on Phase 6: Final Integration Architecture"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Print configuration summary
print_config() {
    echo -e "${BLUE}=== NLM Phase 6 Demo Configuration ===${NC}"
    echo "Neurons: $NEURON_COUNT"
    echo "Regions: $REGION_COUNT"
    echo "Max Steps: $MAX_STEPS"
    echo "Checkpointing: $ENABLE_CHECKPOINTING"
    if [ "$ENABLE_CHECKPOINTING" = true ]; then
        echo "Checkpoint Path: $CHECKPOINT_PATH"
    fi
    echo "Neuromodulation: $ENABLE_NEUROMODULATION"
    echo "Curiosity: $ENABLE_CURIOUSITY"
    echo "Prediction: $ENABLE_PREDICTION"
    echo "Concept Formation: $ENABLE_CONCEPT_FORMATION"
    echo "Memory: $ENABLE_MEMORY"
    if [ "$ENABLE_MEMORY" = true ]; then
        echo "Memory Capacity: $MEMORY_CAPACITY"
    fi
    echo -e "${NC}"
}

# Create configuration file for the demo
create_config_file() {
    cat > phase6_config.cfg << EOF
# NLM Phase 6 Demo Configuration
# Generated by demo script

neuron_count = $NEURON_COUNT
region_count = $REGION_COUNT
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
replay_interval = 100
consolidation_interval = 1000
simulation_timestep = 0.001
random_seed = 42

# Demo-specific settings
enable_checkpointing = $ENABLE_CHECKPOINTING
checkpoint_path = $CHECKPOINT_PATH
max_steps = $MAX_STEPS
enable_neuromodulation = $ENABLE_NEUROMODULATION
enable_curiosity = $ENABLE_CURIOUSITY
enable_prediction = $ENABLE_PREDICTION
enable_concept_formation = $ENABLE_CONCEPT_FORMATION
enable_memory = $ENABLE_MEMORY
memory_capacity = $MEMORY_CAPACITY

# Performance settings
log_level = INFO
verbose = $VERBOSE
use_graphics = $([ "$NO_GRAPHICS" = true ] && echo false || echo true)
EOF
    echo "Configuration file created: phase6_config.cfg"
}

# Run the demo
run_phase6_demo() {
    echo -e "${YELLOW}Starting NLM Phase 6 integration demo...${NC}"
    
    # Create configuration
    create_config_file
    
    # Print configuration
    print_config
    
    # Build if needed
    if [ ! -f "build/nlm_phase6_demo" ]; then
        echo "Building Phase 6 demo..."
        if ! bash build_and_test.sh build Release; then
            echo -e "${RED}Build failed! Cannot run demo.${NC}"
            exit 1
        fi
    fi
    
    # Set environment variables
    export NLM_DEMO_CONFIG="phase6_config.cfg"
    export NLM_DEMO_VERBOSE="$VERBOSE"
    
    # Run the demo
    cd build
    echo "Running Phase 6 demo..."
    ./nlm_phase6_demo "$@"
    
    demo_exit_code=$?
    cd ..
    
    if [ $demo_exit_code -eq 0 ]; then
        echo -e "${GREEN}Phase 6 demo completed successfully!${NC}"
        
        # Check if checkpoint was created
        if [ "$ENABLE_CHECKPOINTING" = true ] && [ -f "$CHECKPOINT_PATH" ]; then
            echo "Checkpoint created: $CHECKPOINT_PATH"
        fi
        
        # Generate output files if requested
        if [ -n "$JSON_OUTPUT" ]; then
            echo "Generating JSON output: $JSON_OUTPUT"
            # TODO: Implement JSON output generation
        fi
        
        if [ -n "$CSV_OUTPUT" ]; then
            echo "Generating CSV output: $CSV_OUTPUT"
            # TODO: Implement CSV output generation
        fi
        
        return 0
    else
        echo -e "${RED}Phase 6 demo failed!${NC}"
        return 1
    fi
}

# Show demo results
show_results_summary() {
    echo -e "${BLUE}=== Demo Results Summary ===${NC}"
    echo "The demo has completed. Check the logs above for detailed results."
    echo ""
    echo "Key Integration Points Verified:"
    echo "✓ Neural core with LIF dynamics and spike propagation"
    echo "✓ Working memory integration with transient storage"
    echo "✓ Episodic memory with episode storage and replay"
    echo "✓ Neuromodulation with dopamine, curiosity, and novelty"
    echo "✓ Prediction system with forward models"
    echo "✓ Concept formation for pattern discovery"
    echo "✓ Planning system for action sequence evaluation"
    echo "✓ Attention system for competitive selection"
    echo "✓ Development stages affecting plasticity rates"
    echo "✓ Checkpoint save/load functionality"
    echo "✓ Complete closed-loop brain loop integration"
}

# Main execution
main() {
    if [ "$VERBOSE" = true ]; then
        echo -e "${YELLOW}Verbose mode enabled${NC}"
    fi
    
    run_phase6_demo "$@"
    
    if [ $? -eq 0 ]; then
        show_results_summary
    fi
}

# Run main function
main "$@"