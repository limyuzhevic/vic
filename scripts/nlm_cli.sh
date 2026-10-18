#!/usr/bin/env bash

# NLM Command Line Interface
# Advanced interface for running and managing NLM experiments

set -e

# Default configuration
CONFIG_FILE="configs/default.cfg"
MODE="demo"
LOG_LEVEL="INFO"
VERBOSE=false

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Help function
function print_help() {
    echo "NLM - Neural Learning Machine"
    echo "===================================="
    echo ""
    echo "USAGE:"
    echo "  ./nlm_cli [OPTIONS] [ARGS...]"
    echo ""
    echo "MODES:"
    echo "  demo                    Run Phase 6 integration demo (default)"
    echo "  phase2                  Run Phase 2: Basic neural computation"
    echo "  experiment [name]       Run specific experiment"
    echo "  interactive             Launch interactive mode"
    echo "  benchmark               Run performance benchmark"
    echo ""
    echo "ADVANCED OPTIONS:"
    echo "  --config FILE           Use configuration file (default: configs/default.cfg)"
    echo "  --log-level LEVEL       Set logging level: DEBUG, INFO, WARN, ERROR, FATAL"
    echo "  --verbose               Enable verbose output"
    echo "  --help                  Show this help message"
    echo "  --version               Show version information"
    echo ""
    echo "EXPERIMENT OPTIONS:"
    echo "  --neurons N             Number of neurons (default: from config)"
    echo "  --steps N               Number of simulation steps (default: from config)"
    echo "  --checkpoint            Enable checkpointing"
    echo "  --no-checkpoint         Disable checkpointing"
    echo "  --replay                Enable memory replay"
    echo "  --no-replay             Disable memory replay"
    echo "  --development           Enable developmental stages"
    echo "  --no-development        Disable developmental stages"
    echo ""
    echo "DEBUGGING OPTIONS:"
    echo "  --trace-neurons         Trace specific neuron activity"
    echo "  --trace-synapses        Trace specific synapse activity"
    echo "  --memory-stats         Print memory system statistics"
    echo "  --brain-stats          Print brain system statistics"
    echo ""
    echo "EXAMPLES:"
    echo "  ./nlm_cli --config my_config.cfg --neurons 500 demo"
    echo "  ./nlm_cli --steps 5000 --checkpoint experiment learning"
    echo "  ./nlm_cli --verbose benchmark"
}

# Version function
function print_version() {
    echo "NLM - Neural Learning Machine"
    echo "Phase 6: Final Integration"
    echo "Version: 0.1.0 (Phase 6.1)"
    echo "Built with C++20 and SIMD optimizations"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        demo|phase2|experiment|interactive|benchmark) MODE="$1"; shift;;
        --config) CONFIG_FILE="$2"; shift 2;;
        --log-level) LOG_LEVEL="$2"; shift 2;;
        --verbose) VERBOSE=true; shift;;
        --neurons) NEURON_COUNT="$2"; shift 2;;
        --steps) STEP_COUNT="$2"; shift 2;;
        --checkpoint|--no-checkpoint) CHECKPOINT_OPTION="$1"; shift;;
        --replay|--no-replay) REPLAY_OPTION="$1"; shift;;
        --development|--no-development) DEVELOPMENT_OPTION="$1"; shift;;
        --trace-neurons|--trace-synapses|--memory-stats|--brain-stats) DEBUG_OPT="$1"; shift;;
        --help) print_help; exit 0;;
        --version) print_version; exit 0;;
        *) echo "Unknown option: $1"; print_help; exit 1;;
    esac
done

# Override mode if provided as positional argument
if [[ "$1" != "" ]]; then
    MODE="$1"
fi

# Build arguments array for the main program
BUILD_ARGS=""

# Add config argument
BUILD_ARGS="$BUILD_ARGS --config $CONFIG_FILE"

# Add log level
BUILD_ARGS="$BUILD_ARGS --log-level $LOG_LEVEL"

# Add experiment options if specified
if [[ -n "${NEURON_COUNT:-}" ]]; then
    BUILD_ARGS="$BUILD_ARGS --neuron-count $NEURON_COUNT"
fi

if [[ -n "${STEP_COUNT:-}" ]]; then
    BUILD_ARGS="$BUILD_ARGS --steps $STEP_COUNT"
fi

# Convert checkpoint, replay, development options to boolean flags
if [[ -n "${CHECKPOINT_OPTION:-}" ]]; then
    if [[ "$CHECKPOINT_OPTION" == "--checkpoint" ]]; then
        BUILD_ARGS="$BUILD_ARGS --enable-checkpointing"
    else
        BUILD_ARGS="$BUILD_ARGS --disable-checkpointing"
    fi
fi

if [[ -n "${REPLAY_OPTION:-}" ]]; then
    if [[ "$REPLAY_OPTION" == "--replay" ]]; then
        BUILD_ARGS="$BUILD_ARGS --enable-replay"
    else
        BUILD_ARGS="$BUILD_ARGS --disable-replay"
    fi
fi

if [[ -n "${DEVELOPMENT_OPTION:-}" ]]; then
    if [[ "$DEVELOPMENT_OPTION" == "--development" ]]; then
        BUILD_ARGS="$BUILD_ARGS --enable-development"
    else
        BUILD_ARGS="$BUILD_ARGS --disable-development"
    fi
fi

# Debug options
if [[ -n "${DEBUG_OPT:-}" ]]; then
    case "$DEBUG_OPT" in
        --trace-neurons) BUILD_ARGS="$BUILD_ARGS --trace-neurons";;
        --trace-synapses) BUILD_ARGS="$BUILD_ARGS --trace-synapses";;
        --memory-stats) BUILD_ARGS="$BUILD_ARGS --memory-stats";;
        --brain-stats) BUILD_ARGS="$BUILD_ARGS --brain-stats";;
    esac
fi

# Run the main program
function run_mode() {
    case "$MODE" in
        demo)
            echo -e "${GREEN}Running NLM Phase 6 Integration Demo...${NC}"
            if [[ "$VERBOSE" == true ]]; then
                echo -e "${BLUE}Verbose mode enabled${NC}"
            fi
            echo -e "${YELLOW}Configuration: $CONFIG_FILE${NC}"
            
            # Run the Phase 6 demo
            if [[ -f "./nlm_phase6_demo" ]]; then
                ./nlm_phase6_demo $BUILD_ARGS
                return $?
            else
                echo -e "${RED}Error: nlm_phase6_demo not found. Building...${NC}"
                if [[ -f "CMakeLists.txt" ]]; then
                    mkdir -p build
                    cd build
                    cmake .. -DCMAKE_BUILD_TYPE=Release
                    make -j$(nproc)
                    cd ..
                    ./build/nlm_phase6_demo $BUILD_ARGS
                    return $?
                else
                    echo -e "${RED}Error: Could not build project. CMakeLists.txt not found.${NC}"
                    return 1
                fi
            fi
            ;;
        
        phase2)
            echo -e "${GREEN}Running NLM Phase 2: Real Neural Computation...${NC}"
            if [[ -f "./nlm_phase2" ]]; then
                ./nlm_phase2 $BUILD_ARGS
                return $?
            else
                echo -e "${RED}Error: nlm_phase2 not found. Building...${NC}"
                if [[ -f "CMakeLists.txt" ]]; then
                    mkdir -p build
                    cd build
                    cmake .. -DCMAKE_BUILD_TYPE=Release
                    make -j$(nproc)
                    cd ..
                    ./build/nlm_phase2 $BUILD_ARGS
                    return $?
                else
                    echo -e "${RED}Error: Could not build project. CMakeLists.txt not found.${NC}"
                    return 1
                fi
            fi
            ;;
        
        experiment)
            echo -e "${GREEN}Running NLM experiment...${NC}"
            # For experiments, you'd need to pass the experiment name as an argument
            echo -e "${YELLOW}Usage: ./nlm_cli [EXPERIMENT_NAME] experiment${NC}"
            return 1
            ;;
        
        interactive)
            echo -e "${GREEN}Launching NLM Interactive Mode...${NC}"
            # Launch interactive mode
            if [[ -f "./nlm_interactive" ]]; then
                ./nlm_interactive $BUILD_ARGS
                return $?
            else
                echo -e "${YELLOW}Note: Interactive mode not implemented yet.${NC}"
                echo -e "${YELLOW}Running demo instead...${NC}"
                run_mode demo
                return $?
            fi
            ;;
        
        benchmark)
            echo -e "${GREEN}Running NLM Performance Benchmark...${NC}"
            # Run benchmarks
            if [[ -f "./nlm_benchmark" ]]; then
                ./nlm_benchmark $BUILD_ARGS
                return $?
            else
                echo -e "${YELLOW}Note: Benchmark mode not implemented yet.${NC}"
                echo -e "${YELLOW}Running demo instead...${NC}"
                run_mode demo
                return $?
            fi
            ;;
        
        *)
            echo -e "${RED}Error: Unknown mode '$MODE'${NC}"
            print_help
            return 1
            ;;
    esac
}

# Run the selected mode
run_mode

# Exit with the appropriate code
exit ${PIPESTATUS[0]}