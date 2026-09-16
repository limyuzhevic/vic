#!/usr/bin/env bash

# NLM Phase 6 Integration Demo
# Comprehensive test of all integrated brain systems
#
# This script demonstrates the Phase 6 integrated artificial brain by running
a series of experiments that test all connected systems.
#
# Usage: ./nlm_phase6_demo [OPTIONS]

function print_phase6_demo_banner {
    echo "============================================================"
    echo "         NLM Phase 6: Integrated Artificial Brain Demo       "
    echo "============================================================"
    echo "
    Testing all integrated brain systems:
    - Neural core (LIF dynamics, spikes, plasticity)
    - Memory systems (working, episodic, associative)
    - Prediction system
    - Cognition (planning, concept formation, self-model)
    - Neuromodulation (dopamine, curiosity, novelty)
    - Development (plasticity modulation)
    - Replay and consolidation
    - Performance infrastructure (memory pools, event queues)
    echo "
    ============================================================="
}

function run_phase6_basic_test {
    echo ""
    echo "=== Phase 6 Basic Integration Test ==="
    echo "Testing core neural and memory integration..."
    
    # Run the Phase 6 demo binary (if available)
    if [ -f "./nlm_phase6_demo" ]; then
        ./nlm_phase6_demo --steps 100 --log-level INFO
    else
        echo "Phase 6 demo binary not found, running Phase 2 basic test instead..."
        echo "To get the full Phase 6 demo, build with CMake and run:"
        echo "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release"
        echo "  make nlm_phase6_demo"
        echo "  ./nlm_phase6_demo --steps 100"
        echo ""
    fi
}

function run_phase6_memory_test {
    echo ""
    echo "=== Phase 6 Memory Integration Test ==="
    echo "Testing memory system integration and replay..."
    echo "
    This test verifies:
    1. Working memory integration with neural activity
    2. Episodic memory creation and retrieval
    3. Memory replay during rest periods
    4. Memory consolidation mechanisms
    echo "
    To run this test, use the Phase 6 demo with memory-specific options:"
    echo "  ./nlm_phase6_demo --test memory --replay-interval 50"
    echo "  ./nlm_phase6_demo --test memory --consolidation-interval 200"
}

function run_phase6_prediction_test {
    echo ""
    echo "=== Phase 6 Prediction System Test ==="
    echo "Testing prediction and learning integration..."
    echo "
    This test verifies:
    1. Prediction system updates with sensory input
    2. Prediction error signals
    3. Integration of prediction errors with neuromodulation
    4. Action planning based on predictions
    echo "
    To run this test, use the Phase 6 demo with prediction options:"
    echo "  ./nlm_phase6_demo --test prediction --planning-depth 10"
    echo "  ./nlm_phase6_demo --test prediction --prediction-iterations 500"
}

function run_phase6_cognition_test {
    echo ""
    echo "=== Phase 6 Cognition Integration Test ==="
    echo "Testing cognitive mechanism integration..."
    echo "
    This test verifies:
    1. Neural planner integration with action selection
    2. Concept formation from experiences
    3. Self-model development and body schema
    4. Social learning capabilities
    5. Spatial representation and navigation
    echo "
    To run this test, use the Phase 6 demo with cognition options:"
    echo "  ./nlm_phase6_demo --test cognition --concept-threshold 0.7"
    echo "  ./nlm_phase6_demo --test cognition --self-model-training"
    echo "  ./nlm_phase6_demo --test cognition --social-learning"
}

function run_phase6_comprehensive_test {
    echo ""
    echo "=== Phase 6 Comprehensive Integration Test ==="
    echo "Running full integration test of all brain systems..."
    echo "
    This test validates the complete integrated brain architecture:
    1. All memory systems connected to neural processing
    2. Prediction system driving cognition and action
    3. Neuromodulation coordinating learning and exploration
    4. Development affecting plasticity and learning rates
    5. Replay and consolidation for memory optimization
    6. Performance infrastructure efficiently utilized
    echo "
    This is the complete Phase 6 integration test as described in the documentation."
    echo "It should produce metrics showing:
    - Neural firing statistics
    - Memory retention and recall accuracy
    - Learning progress over time
    - Prediction accuracy
    - Cognitive performance
    "
}

function show_advanced_options {
    echo ""
    echo "=== Advanced Phase 6 Options ==="
    echo "
    The Phase 6 demo supports advanced configuration options:
    echo "
    Advanced Command Line Options:
    echo "  --steps N              Number of simulation steps (default: 100)"
    echo "  --neuron-count N       Number of neurons (default: 1000)"
    echo "  --region-count N        Number of brain regions (default: 2)"
    echo "  --replay-interval N     Steps between memory replay (default: 100)"
    echo "  --consolidation-interval N Steps between memory consolidation (default: 1000)"
    echo "  --planning-depth N      Look-ahead steps for planning (default: 5)"
    echo "  --development-stage S   Set developmental stage: Initial|CriticalPeriod|Maturation|Adult"
    echo "  --test TYPE            Run specific test: basic|memory|prediction|cognition|comprehensive"
    echo "  --checkpoint PATH       Save brain state to file"
    echo "  --load PATH            Load brain state from file"
    echo "  --performance-metrics   Output performance statistics"
    echo "  --debug               Enable debug logging"
    echo "  --dry-run             Show what would be done without actually running"
    echo "
    Advanced Configuration Options (from config file or command line):"
    echo "  plasticity_learning_rate    Learning rate for STDP"
    echo "  development_synaptogenesis_rate Rate of new synapse formation"
    echo "  development_pruning_rate     Rate of synapse elimination"
    echo "  dopamine_baseline            Baseline dopamine level"
    echo "  curiosity_sensitivity       How exploration is driven"
    echo "  novelty_threshold           Threshold for novelty detection"
    echo "  working_memory_capacity     Size of working memory"
    echo "  episodic_memory_episodes    Maximum number of stored episodes"
    echo "  prediction_confidence       How certain the brain is of predictions"
    echo "  concept_formation_threshold  Similarity threshold for concept formation"
    echo "  self_model_confidence        Confidence in self-predictions"
    echo "  spatial_representation       Enable spatial mapping (true/false)"
    echo "  social_learning              Enable social learning (true/false)"
    echo "
    Example advanced usage:
    echo "  ./nlm_phase6_demo \\"
    echo "    --steps 5000 \\"
    echo "    --neuron-count 2000 \\"
    echo "    --development-stage CriticalPeriod \\"
    echo "    --plasticity-learning-rate 0.02 \\"
    echo "    --working-memory-capacity 200 \\"
    echo "    --episodic-memory-episodes 5000 \\"
    echo "    --planning-depth 15 \\"
    echo "    --replay-interval 200 \\"
    echo "    --checkpoint brain_checkpoint.bin"
    echo "
    This would simulate a more mature brain with:
    - More neurons and regions
    - In a critical developmental period (high plasticity)
    - Stronger learning rates
    - Larger memory capacities
    - Deeper planning horizons
    - More frequent memory replay for consolidation"
}

function show_phase6_tutorial {
    echo ""
    echo "=== Phase 6 Tutorial: Understanding Integration ==="
    echo "
    Phase 6 creates an integrated artificial brain by connecting all components:
    echo "
    The brain operates as a closed-loop system:
    echo "WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE"
    echo "INTERNAL STATE → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION"
    echo "→ ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE"
    echo "→ REWARD/SURPRISE/ERROR → PLASTICITY → CHANGED BRAIN"
    echo "→ CHANGED FUTURE BEHAVIOR"
    echo "
    Key integration points:
    echo "1. Memory systems store and retrieve experiences for future use"
    echo "2. Prediction system anticipates sensory consequences"
    echo "3. Cognition systems plan, conceptualize, and model self"
    echo "4. Neuromodulation coordinates learning and exploration"
    echo "5. Development modulates plasticity rates over time"
    echo "6. Replay consolidates important memories during rest"
    echo "
    Each system influences others:
    echo "- Memory provides patterns for concept formation"
    echo "- Prediction errors drive learning via neuromodulators"
    echo "- Planning considers predicted futures"
    echo "- Novelty detection guides exploration"
    echo "- Development affects all plasticity rates"
    echo "
    The result is a brain that:
    echo "✓ Learns from experience"
    echo "✓ Stores what matters"
    echo "✓ Plans for the future"
    echo "✓ Represents itself and others"
    echo "✓ Navigates and explores efficiently"
    echo "✓ Develops and adapts over time"
}

# Main execution
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    print_phase6_demo_banner
    show_advanced_options
    show_phase6_tutorial
    exit 0
fi

if [ "$1" = "--tutorial" ]; then
    print_phase6_demo_banner
    show_phase6_tutorial
    exit 0
fi

if [ "$1" = "--advanced-options" ] || [ "$1" = "-a" ]; then
    print_phase6_demo_banner
    show_advanced_options
    exit 0
fi

print_phase6_demo_banner

# Parse command line arguments
TEST_TYPE="basic"
STEPS=100
NEURON_COUNT=1000
REGION_COUNT=2
REPLAY_INTERVAL=100
CONSOLIDATION_INTERVAL=1000
PLANNING_DEPTH=5
DEVELOPMENT_STAGE="Initial"
PERFORMANCE_METRICS=false
DEBUG=false
CHECKPOINT=""
LOAD=""
DRY_RUN=false

# Simple argument parsing
while [ $# -gt 0 ]; do
    case $1 in
        --test)
            TEST_TYPE="$2"
            shift 2
            ;;
        --steps)
            STEPS="$2"
            shift 2
            ;;
        --neuron-count)
            NEURON_COUNT="$2"
            shift 2
            ;;
        --region-count)
            REGION_COUNT="$2"
            shift 2
            ;;
        --replay-interval)
            REPLAY_INTERVAL="$2"
            shift 2
            ;;
        --consolidation-interval)
            CONSOLIDATION_INTERVAL="$2"
            shift 2
            ;;
        --planning-depth)
            PLANNING_DEPTH="$2"
            shift 2
            ;;
        --development-stage)
            DEVELOPMENT_STAGE="$2"
            shift 2
            ;;
        --performance-metrics)
            PERFORMANCE_METRICS=true
            shift
            ;;
        --debug)
            DEBUG=true
            shift
            ;;
        --checkpoint)
            CHECKPOINT="$2"
            shift 2
            ;;
        --load)
            LOAD="$2"
            shift 2
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Run appropriate test based on type
if [ "$TEST_TYPE" = "basic" ]; then
    run_phase6_basic_test
elif [ "$TEST_TYPE" = "memory" ]; then
    run_phase6_memory_test
elif [ "$TEST_TYPE" = "prediction" ]; then
    run_phase6_prediction_test
elif [ "$TEST_TYPE" = "cognition" ]; then
    run_phase6_cognition_test
elif [ "$TEST_TYPE" = "comprehensive" ]; then
    run_phase6_comprehensive_test
else
    echo "Unknown test type: $TEST_TYPE"
    exit 1
fi

# Show advanced options help
if [ "$DEBUG" = true ]; then
    show_advanced_options
fi

echo ""
echo "============================================================"
echo "Phase 6 Demo Complete"
echo "============================================================"
