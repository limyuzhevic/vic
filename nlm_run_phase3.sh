#!/usr/bin/env bash
# nlm_run_phase3.sh - Run Phase 3 demo with common parameters

# This script provides a convenient way to run the Phase 3 demo
# which demonstrates world interaction, sensory processing, and basic learning

# Default parameters
NEURON_COUNT=500
STEPS=2000
DEBUG=false
CHECKPOINT=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --neurons)
            NEURON_COUNT="$2"
            shift 2
            ;;
        --steps)
            STEPS="$2"
            shift 2
            ;;
        --debug)
            DEBUG=true
            shift
            ;;
        --checkpoint)
            CHECKPOINT=true
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --neurons N      Number of neurons (default: $NEURON_COUNT)"
            echo "  --steps S        Number of simulation steps (default: $STEPS)"
            echo "  --debug          Enable debug logging"
            echo "  --checkpoint     Save checkpoint after completion"
            echo "  --help           Show this help"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo "=== Running NLM Phase 3 Demo ==="
echo "Neurons: $NEURON_COUNT"
echo "Steps: $STEPS"
echo

# Build command
CMD="./nlm_phase3_demo"

# Add parameters
CMD="$CMD --neuron_count $NEURON_COUNT --max_steps $STEPS"

# Add optional flags
if [ "$DEBUG" = true ]; then
    CMD="$CMD --debug"
fi

if [ "$CHECKPOINT" = true ]; then
    CMD="$CMD --checkpoint phase3_checkpoint.bin"
fi

# Execute
$CMD

# Check result
if [ $? -eq 0 ]; then
    echo "=== Phase 3 demo completed successfully ==="
else
    echo "=== Phase 3 demo failed ==="
    exit 1
fi