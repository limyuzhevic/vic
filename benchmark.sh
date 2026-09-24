#!/usr/bin/env bash
# Performance benchmarking script for NLM
# This script runs benchmarks to measure NLM performance

set -e

echo "NLM Performance Benchmarking"
echo "============================="

# Check if NLM is built
if [[ ! -f "build/bin/nlm" ]]; then
    echo "ERROR: NLM not found. Please build NLM first:"
    echo "  mkdir build && cd build"
    echo "  cmake .."
    echo "  cmake --build ."
    exit 1
fi

# Default benchmark options
BENCHMARKS=(
    "basic_connectivity"
    "plasticity_learning"
    "stdp_verification"
    "phase3_world_interaction"
)

# Default parameters
NUM_EPISODES=5
STEPS_PER_EPISODE=200

# Parse command line options
while [[ $# -gt 0 ]]; do
    case $1 in
        --benchmarks)
            BENCHMARKS=($2)
            shift 2
            ;;
        --episodes)
            NUM_EPISODES=$2
            shift 2
            ;;
        --steps)
            STEPS_PER_EPISODE=$2
            shift 2
            ;;
        --help)
            echo "Usage: ./benchmark.sh [options]"
            echo ""
            echo "Options:"
            echo "  --benchmarks [list]  Comma-separated list of benchmarks to run"
            echo "  --episodes N         Number of episodes for benchmarks (default: 5)"
            echo "  --steps N            Steps per episode (default: 200)"
            echo "  --help               Show this help"
            echo ""
            echo "Available benchmarks:"
            for bench in "${BENCHMARKS[@]}"; do
                echo "  - ${bench}"
            done
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Run './benchmark.sh --help' for usage"
            exit 1
            ;;
    esac
done

echo "Benchmark configuration:"
echo "  Benchmarks: ${BENCHMARKS[*]}"
echo "  Episodes: ${NUM_EPISODES}"
echo "  Steps per episode: ${STEPS_PER_EPISODE}"
echo ""

# Run benchmarks
for benchmark in "${BENCHMARKS[@]}"; do
    echo "============================================"
    echo "Running benchmark: ${benchmark}"
    echo "============================================"
    
    case "${benchmark}" in
        "basic_connectivity")
            echo "Testing basic neural connectivity..."
            ${BUILD_DIR}/bin/nlm --benchmark basic-connectivity --episodes ${NUM_EPISODES}
            ;;
        
        "plasticity_learning")
            echo "Testing plasticity learning experiment..."
            ${BUILD_DIR}/bin/nlm --benchmark plasticity-learning --episodes ${NUM_EPISODES} --steps ${STEPS_PER_EPISODE}
            ;;
        
        "stdp_verification")
            echo "Testing STDP verification..."
            ${BUILD_DIR}/bin/nlm --benchmark stdp-verification --episodes ${NUM_EPISODES}
            ;;
        
        "phase3_world_interaction")
            echo "Testing Phase 3 world interaction..."
            ${BUILD_DIR}/bin/nlm --benchmark phase3-world --episodes ${NUM_EPISODES} --steps ${STEPS_PER_EPISODE}
            ;;
        
        *)
            echo "Unknown benchmark: ${benchmark}"
            ;;
    esac
    
done

echo ""
echo "All benchmarks completed!"
echo "Results are saved to: ${PWD}/benchmark_results/"
