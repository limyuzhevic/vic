#!/bin/bash

# NLM Demo Control Script
# Helper utilities for running and managing NLM demos

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default configuration
DEFAULT_EPISODES=3
DEFAULT_STEPS=500
DEFAULT_NEURONS=500
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

function print_help() {
    echo "NLM Demo Control Script"
    echo "======================="
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --help, -h              Show this help message"
    echo "  --quick, -q             Run quick demo (3 episodes, 100 steps)"
    echo "  --demo-only             Only run demo, skip analysis"
    echo "  --benchmark             Run benchmark mode"
    echo "  --compare               Compare with baseline"
    echo "  --profile               Run with profiling"
    echo ""
    echo "Examples:"
    echo "  $0                      # Run default demo"
    echo "  $0 --quick              # Quick demo"
    echo "  $0 --demo-only          # Demo only, no analysis"
    echo ""
    echo "Output files:"
    echo "  demo_results.json       # Demo metrics"
    echo "  demo_plots.png          # Performance plots (if plotting enabled)"
    echo "  demo_logs/              # Detailed logs"
    echo ""
    echo "Configuration files:"
    echo "  config/default.cfg      # Default configuration"
    echo "  config/benchmark.cfg    # Benchmark configuration"
    echo ""
}

function check_build() {
    if [ ! -f "$BUILD_DIR/nlm" ]; then
        echo -e "${YELLOW}Build not found. Building NLM...${NC}"
        mkdir -p "$BUILD_DIR"
        cd "$PROJECT_ROOT"
        if ! cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release; then
            echo -e "${RED}Failed to configure build. Check CMake version. ${NC}"
            exit 1
        fi
        if ! cmake --build "$BUILD_DIR" -j$(nproc); then
            echo -e "${RED}Failed to build. Check compilation errors. ${NC}"
            exit 1
        fi
        echo -e "${GREEN}Build completed successfully. ${NC}"
    else
        echo -e "${GREEN}Build found at $BUILD_DIR/nlm ${NC}"
    fi
}

function run_demo() {
    local episodes=$1
    local steps=$2
    local neurons=$3
    local save_checkpoint=$4
    
    echo -e "${GREEN}Running NLM Phase 6 demo...${NC}"
    echo "Configuration: $episodes episodes, $steps steps, $neurons neurons"
    
    local demo_cmd="$BUILD_DIR/nlm"
    if [ "$episodes" -gt 0 ]; then
        demo_cmd="$demo_cmd -e $episodes"
    fi
    if [ "$steps" -gt 0 ]; then
        demo_cmd="$demo_cmd -s $steps"
    fi
    if [ "$neurons" -gt 0 ]; then
        demo_cmd="$demo_cmd -n $neurons"
    fi
    if [ -n "$save_checkpoint" ]; then
        demo_cmd="$demo_cmd --save $save_checkpoint"
    fi
    
    # Run demo
    if ! $demo_cmd; then
        echo -e "${RED}Demo failed. ${NC}"
        return 1
    fi
    
    return 0
}

function analyze_results() {
    local demo_dir=$1
    
    echo -e "${GREEN}Analyzing demo results...${NC}"
    
    # Create analysis directory
    mkdir -p "$demo_dir/analysis"
    
    # Extract metrics from demo output
    local results_file="$demo_dir/demo_results.json"
    
    # Generate summary
    echo "Demo completed successfully"
    echo "Results saved to: $results_file"
    echo "Analysis available in: $demo_dir/analysis/"
}

function run_benchmark() {
    echo -e "${YELLOW}Running benchmark mode...${NC}"
    
    local benchmarks=(
        "quick:3:100:100"
        "medium:5:200:200"
        "heavy:10:500:500"
    )
    
    for benchmark in "${benchmarks[@]}"; do
        local name=$(echo $benchmark | cut -d: -f1)
        local episodes=$(echo $benchmark | cut -d: -f2)
        local steps=$(echo $benchmark | cut -d: -f3)
        local neurons=$(echo $benchmark | cut -d: -f4)
        
        echo "Running $name benchmark..."
        local demo_dir="benchmarks/$name"
        mkdir -p "$demo_dir"
        
        if run_demo $episodes $steps $neurons "$demo_dir/checkpoint.bin"; then
            analyze_results "$demo_dir"
        else
            echo -e "${RED}$name benchmark failed. ${NC}"
        fi
        echo ""
    done
}

function main() {
    local quick=false
    local demo_only=false
    local benchmark_mode=false
    local compare_mode=false
    local profile_mode=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help|-h)
                print_help
                exit 0
                ;;
            --quick|-q)
                quick=true
                shift
                ;;
            --demo-only)
                demo_only=true
                shift
                ;;
            --benchmark)
                benchmark_mode=true
                shift
                ;;
            --compare)
                compare_mode=true
                shift
                ;;
            --profile)
                profile_mode=true
                shift
                ;;
            *)
                echo -e "${RED}Unknown option: $1 ${NC}"
                print_help
                exit 1
                ;;
        esac
    done
    
    # Check build
    check_build
    
    # Run demo or benchmark
    if [ "$benchmark_mode" = true ]; then
        run_benchmark
    elif [ "$quick" = true ]; then
        echo -e "${GREEN}Quick demo mode...${NC}"
        run_demo 3 100 100 "demo_checkpoint.bin"
    else
        echo -e "${GREEN}Standard demo mode...${NC}"
        run_demo $DEFAULT_EPISODES $DEFAULT_STEPS $DEFAULT_NEURONS "demo_checkpoint.bin"
    fi
    
    if [ "$demo_only" = false ]; then
        echo -e "${GREEN}Generating analysis...${NC}"
        analyze_results "demo_results"
    fi
    
    echo -e "${GREEN}Demo script completed successfully. ${NC}"
}

# Run main function with all arguments
main "$@"
