#!/usr/bin/env bash
"""
NLM Build and Run Script

This script provides a simple interface to build and run NLM experiments.
It can be used to:
- Build the NLM framework
- Run predefined experiments
- Generate visualizations
- Debug issues
"""

set -e

# Configuration
BUILD_DIR="build"
CONFIG_DIR="configs"
SCRIPTS_DIR="scripts"
RESULTS_DIR="results"
VISUALIZATIONS_DIR="visualizations"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
EXPERIMENT_TYPE="demo"
NEURON_COUNT=1000
SIMULATION_STEPS=1000
CONFIG_FILE="$CONFIG_DIR/default.cfg"
OUTPUT_DIR="$RESULTS_DIR"
VERBOSE=false
QUIET=false
CHECKPOINT_PATH=""
ENABLE_CHECKPOINT=false
ENABLE_REPLAY=false
ENABLE_DEVELOPMENT=false
GENERATE_VIZ=false
DEBUG=false

# Help function
function show_help {
    cat << EOF
NLM Build and Run Script

Usage: $0 [OPTIONS]

OPTIONS:
  --help                    Show this help message
  --version                 Show version information
  --build                   Build the NLM framework
  --clean                   Clean build directory

EXPERIMENT OPTIONS:
  --experiment <type>       Type of experiment (demo, phase3, phase4, phase5, phase6)
  --neurons <count>         Number of neurons (default: 1000)
  --steps <steps>           Number of simulation steps (default: 1000)
  --config <file>           Configuration file (default: configs/default.cfg)
  --output <dir>            Output directory (default: results)
  --checkpoint <file>       Enable checkpointing with file path
  --enable-replay           Enable memory replay
  --enable-development      Enable developmental stages

VISUALIZATION OPTIONS:
  --viz                     Generate visualizations after experiment
  --debug                   Enable debugging

OUTPUT OPTIONS:
  -v, --verbose             Enable verbose output
  -q, --quiet               Enable quiet output

EXAMPLES:
  $0 --build
  $0 --experiment phase6 --steps 5000
  $0 --experiment demo --neurons 500 --viz
  $0 --experiment phase4 --checkpoint checkpoint.bin --enable-replay
EOF
}

# Version function
function show_version {
    echo "NLM (Neural Learning Machine) 1.0.0 (Phase 6)"
    echo "Build date: $(date)"
    echo "Author: NLM Project"
}

# Build function
function build_nlm {
    echo -e "${BLUE}Building NLM framework...${NC}"
    
    if [ ! -f "CMakeLists.txt" ]; then
        echo -e "${RED}Error: CMakeLists.txt not found. Please run from project root.${NC}"
        exit 1
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}NLM framework built successfully!${NC}"
        cd ..
    else
        echo -e "${RED}Build failed!${NC}"
        exit 1
    fi
}

# Clean function
function clean_build {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        echo -e "${GREEN}Build directory cleaned!${NC}"
    else
        echo -e "${YELLOW}Build directory does not exist.${NC}"
    fi
}

# Run experiment function
function run_experiment {
    echo -e "${BLUE}Running NLM experiment: $EXPERIMENT_TYPE${NC}"
    
    # Check if build directory exists
    if [ ! -d "$BUILD_DIR" ]; then
        echo -e "${RED}Error: Build directory not found. Please run --build first.${NC}"
        exit 1
    fi
    
    # Check if nlm-cli exists
    if [ ! -f "$BUILD_DIR/nlm-cli" ]; then
        echo -e "${RED}Error: nlm-cli not found in build directory.${NC}"
        exit 1
    fi
    
    # Prepare command
    cmd="$BUILD_DIR/nlm-cli \"
    cmd+="--experiment $EXPERIMENT_TYPE \"
    cmd+="--neurons $NEURON_COUNT \"
    cmd+="--steps $SIMULATION_STEPS \"
    cmd+="--config $CONFIG_FILE \"
    cmd+="--output $OUTPUT_DIR"
    
    # Add optional flags
    if [ "$ENABLE_CHECKPOINT" = true ]; then
        cmd+=" \"--checkpoint $CHECKPOINT_PATH"
    fi
    
    if [ "$ENABLE_REPLAY" = true ]; then
        cmd+=" --enable-replay"
    fi
    
    if [ "$ENABLE_DEVELOPMENT" = true ]; then
        cmd+=" --enable-development"
    fi
    
    if [ "$VERBOSE" = true ]; then
        cmd+=" --verbose"
    fi
    
    if [ "$QUIET" = true ]; then
        cmd+=" --quiet"
    fi
    
    echo -e "${YELLOW}Command: $cmd${NC}"
    
    # Run the command
    eval $cmd
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Experiment completed successfully!${NC}"
        
        # Generate visualizations if requested
        if [ "$GENERATE_VIZ" = true ]; then
            echo -e "${BLUE}Generating visualizations...${NC}"
            "$SCRIPTS_DIR/nlm_viz.py" --experiment-type "$EXPERIMENT_TYPE" --output-dir "$VISUALIZATIONS_DIR"
        fi
        
        # Debug if requested
        if [ "$DEBUG" = true ]; then
            echo -e "${BLUE}Running debug analysis...${NC}"
            "$SCRIPTS_DIR/nlm.py" run --experiment-type "$EXPERIMENT_TYPE" --debug
        fi
        
    else
        echo -e "${RED}Experiment failed!${NC}"
        exit 1
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help)
            show_help
            exit 0
            ;;
        --version)
            show_version
            exit 0
            ;;
        --build)
            build_nlm
            shift
            ;;
        --clean)
            clean_build
            shift
            ;;
        --experiment)
            EXPERIMENT_TYPE="$2"
            shift 2
            ;;
        --neurons)
            NEURON_COUNT="$2"
            shift 2
            ;;
        --steps)
            SIMULATION_STEPS="$2"
            shift 2
            ;;
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --checkpoint)
            ENABLE_CHECKPOINT=true
            CHECKPOINT_PATH="$2"
            shift 2
            ;;
        --enable-replay)
            ENABLE_REPLAY=true
            shift
            ;;
        --enable-development)
            ENABLE_DEVELOPMENT=true
            shift
            ;;
        --viz)
            GENERATE_VIZ=true
            shift
            ;;
        --debug)
            DEBUG=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -q|--quiet)
            QUIET=true
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
    
    # Check if we need to create directories
    mkdir -p "$OUTPUT_DIR"
    mkdir -p "$VISUALIZATIONS_DIR"
    mkdir -p "$SCRIPTS_DIR"
    
    # Run experiment if no --build or --clean
    if [ "$EXPERIMENT_TYPE" != "" ] && [ "$1" != "--build" ] && [ "$1" != "--clean" ]; then
        run_experiment
        exit 0
    fi
    
done

# Show help if no arguments provided
show_help
