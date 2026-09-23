#!/bin/bash
# NLM Phase 6 Demo Script
# =======================
#
# This script provides a comprehensive interface for running NLM Phase 6 demos
# with different configurations and options. It simplifies the command line
# interface for users and provides helpful information about what's running.
#
# Usage:
#   ./run_nlm_demo.sh [OPTIONS]
#
# Examples:
#   ./run_nlm_demo.sh quick      # Run quick integration test
#   ./run_nlm_demo.sh basic      # Run with basic configuration
#   ./run_nlm_demo.sh research    # Run with research configuration
#   ./run_nlm_demo.sh help        # Show help information

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
MODE="demo"
CONFIG_FILE="configs/default.cfg"
LOG_LEVEL="info"
STEPS=2000
NEURONS=500
REGIONS=1
CONN_PROB=0.15
CHECKPOINT=false
NO_REPLAY=false
NO_DEV=false
NO_CURIOSITY=false
NO_REWARD=false
QUICK=false
NO_INTEGRATION=false
NO_MEMORY=false
NO_NEUROMOD=false
NO_PREDICTION=false
NO_DEV_TEST=false
NO_CHECKPOINT=false
NO_REPLAY_TEST=false
NAME="Phase6Integration"

# Usage information
function show_help() {
    echo "=== NLM Phase 6 Demo Script ==="
    echo ""
    echo "This script provides a simplified interface for running NLM Phase 6 demos."
    echo ""
    echo "Available modes:"
    echo "  quick      Run quick integration test (fast, minimal testing)"
    echo "  basic      Run with basic configuration (standard demo)"
    echo "  research   Run with research configuration (detailed analysis)"
    echo "  memory     Run with memory-intensive configuration"
    echo "  neuromod    Run with neuromodulation-focused configuration"
    echo "  development Run with development-focused configuration"
    echo "  help       Show this help message"
    echo ""
    echo "Options (can be combined):"
    echo "  --checkpoint                    Enable checkpointing"
    echo "  --no-replay                     Disable replay system"
    echo "  --no-development                Disable developmental system"
    echo "  --no-curiosity                  Disable curiosity system"
    echo "  --no-reward                     Disable reward modulation"
    echo "  --steps <n>                     Number of simulation steps"
    echo "  --neurons <n>                   Number of neurons"
    echo "  --regions <n>                   Number of regions"
    echo "  --conn-prob <f>                 Connection probability"
    echo "  --name <name>                   Set experiment name"
    echo "  --output <file>                 Save results to file"
    echo ""
    echo "Examples:"
    echo "  ./run_nlm_demo.sh quick --checkpoint --no-replay"
    echo "  ./run_nlm_demo.sh research --name 'My Research Experiment'"
    echo "  ./run_nlm_demo.sh memory --steps 5000 --neurons 1000"
    echo ""
}

# Parse command line arguments
function parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            # Mode options
            quick)
                MODE="quick"
                shift
                ;;
            basic)
                MODE="basic"
                shift
                ;;
            research)
                MODE="research"
                shift
                ;;
            memory)
                MODE="memory"
                shift
                ;;
            neuromod)
                MODE="neuromod"
                shift
                ;;
            development)
                MODE="development"
                shift
                ;;
            help|--help|-h)
                show_help
                exit 0
                ;;
            
            # Configuration options
            --config)
                CONFIG_FILE="$2"
                shift 2
                ;;
            --steps)
                STEPS="$2"
                shift 2
                ;;
            --neurons)
                NEURONS="$2"
                shift 2
                ;;
            --regions)
                REGIONS="$2"
                shift 2
                ;;
            --conn-prob)
                CONN_PROB="$2"
                shift 2
                ;;
            --name)
                NAME="$2"
                shift 2
                ;;
            --output)
                OUTPUT_FILE="$2"
                shift 2
                ;;
            
            # Feature options
            --checkpoint)
                CHECKPOINT=true
                shift
                ;;
            --no-replay)
                NO_REPLAY=true
                shift
                ;;
            --no-development)
                NO_DEV=true
                shift
                ;;
            --no-curiosity)
                NO_CURIOSITY=true
                shift
                ;;
            --no-reward)
                NO_REWARD=true
                shift
                ;;
            
            # Test options
            --quick)
                QUICK=true
                shift
                ;;
            --no-integration)
                NO_INTEGRATION=true
                shift
                ;;
            --no-memory)
                NO_MEMORY=true
                shift
                ;;
            --no-neuromod)
                NO_NEUROMOD=true
                shift
                ;;
            --no-prediction)
                NO_PREDICTION=true
                shift
                ;;
            --no-dev)
                NO_DEV_TEST=true
                shift
                ;;
            --no-checkpoint)
                NO_CHECKPOINT=true
                shift
                ;;
            --no-replay-test)
                NO_REPLAY_TEST=true
                shift
                ;;
            
            *)
                echo "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Set configuration based on mode
function set_mode_config() {
    case $MODE in
        quick)
            CONFIG_FILE="configs/default.cfg"
            STEPS=500
            NEURONS=100
            REGIONS=1
            CONN_PROB=0.1
            QUICK=true
            NO_INTEGRATION=true
            NO_MEMORY=true
            NO_NEUROMOD=true
            NO_PREDICTION=true
            NO_DEV_TEST=true
            NO_CHECKPOINT=true
            NO_REPLAY_TEST=true
            CHECKPOINT=false
            NO_REPLAY=true
            NO_DEV=true
            NO_CURIOSITY=true
            NO_REWARD=true
            ;;
        basic)
            CONFIG_FILE="configs/default.cfg"
            STEPS=2000
            NEURONS=500
            REGIONS=1
            CONN_PROB=0.15
            QUICK=false
            NO_INTEGRATION=false
            ;;
        research)
            CONFIG_FILE="configs/default.cfg"
            STEPS=5000
            NEURONS=2000
            REGIONS=3
            CONN_PROB=0.08
            QUICK=false
            NO_INTEGRATION=false
            CHECKPOINT=true
            ;;
        memory)
            CONFIG_FILE="configs/default.cfg"
            STEPS=3000
            NEURONS=1000
            REGIONS=1
            CONN_PROB=0.2
            QUICK=false
            NO_INTEGRATION=false
            CHECKPOINT=true
            NO_CURIOSITY=true
            ;;
        neuromod)
            CONFIG_FILE="configs/default.cfg"
            STEPS=2000
            NEURONS=800
            REGIONS=2
            CONN_PROB=0.12
            QUICK=false
            NO_INTEGRATION=false
            CHECKPOINT=true
            NO_DEV=true
            ;;
        development)
            CONFIG_FILE="configs/default.cfg"
            STEPS=4000
            NEURONS=1200
            REGIONS=2
            CONN_PROB=0.1
            QUICK=false
            NO_INTEGRATION=false
            CHECKPOINT=true
            NO_REWARD=true
            ;;
    esac
}

# Build command line arguments
function build_command() {
    CMD="./nlm --config $CONFIG_FILE"
    
    # Add basic configuration
    CMD+=" --steps $STEPS"
    CMD+=" --neurons $NEURONS"
    CMD+=" --regions $REGIONS"
    CMD+=" --conn-prob $CONN_PROB"
    CMD+=" --name \"$NAME\""
    
    # Add feature flags
    if [ "$CHECKPOINT" = true ]; then
        CMD+=" --checkpoint"
    fi
    
    if [ "$NO_REPLAY" = true ]; then
        CMD+=" --no-replay"
    fi
    
    if [ "$NO_DEV" = true ]; then
        CMD+=" --no-development"
    fi
    
    if [ "$NO_CURIOSITY" = true ]; then
        CMD+=" --no-curiosity"
    fi
    
    if [ "$NO_REWARD" = true ]; then
        CMD+=" --no-reward"
    fi
    
    # Add test flags
    if [ "$QUICK" = true ]; then
        CMD+=" --quick"
    fi
    
    if [ "$NO_INTEGRATION" = true ]; then
        CMD+=" --no-integration"
    fi
    
    if [ "$NO_MEMORY" = true ]; then
        CMD+=" --no-memory"
    fi
    
    if [ "$NO_NEUROMOD" = true ]; then
        CMD+=" --no-neuromod"
    fi
    
    if [ "$NO_PREDICTION" = true ]; then
        CMD+=" --no-prediction"
    fi
    
    if [ "$NO_DEV_TEST" = true ]; then
        CMD+=" --no-dev"
    fi
    
    if [ "$NO_CHECKPOINT" = true ]; then
        CMD+=" --no-checkpoint"
    fi
    
    if [ "$NO_REPLAY_TEST" = true ]; then
        CMD+=" --no-replay-test"
    fi
    
    # Add output file if specified
    if [ ! -z "$OUTPUT_FILE" ]; then
        CMD+=" --output $OUTPUT_FILE"
    fi
    
    echo "$CMD"
}

# Main function
function main() {
    echo -e "${BLUE}=== NLM Phase 6 Demo Script ===${NC}"
    echo -e "${GREEN}Starting NLM Phase 6 integration demo...${NC}"
    echo ""
    
    # Parse command line arguments
    parse_arguments "$@"
    
    # Set mode configuration
    set_mode_config
    
    # Build command
    CMD=$(build_command)
    
    echo -e "${YELLOW}Configuration:${NC}"
    echo "  Mode: $MODE"
    echo "  Config file: $CONFIG_FILE"
    echo "  Steps: $STEPS"
    echo "  Neurons: $NEURONS"
    echo "  Regions: $REGIONS"
    echo "  Connection probability: $CONN_PROB"
    echo "  Experiment name: $NAME"
    echo ""
    
    if [ "$CHECKPOINT" = true ]; then
        echo -e "${GREEN}Checkpointing enabled:${NC} $CHECKPOINT"
    fi
    
    if [ "$NO_REPLAY" = true ]; then
        echo -e "${GREEN}Replay system disabled:${NC} $NO_REPLAY"
    fi
    
    if [ "$NO_DEV" = true ]; then
        echo -e "${GREEN}Development system disabled:${NC} $NO_DEV"
    fi
    
    if [ "$NO_CURIOSITY" = true ]; then
        echo -e "${GREEN}Curiosity system disabled:${NC} $NO_CURIOSITY"
    fi
    
    if [ "$NO_REWARD" = true ]; then
        echo -e "${GREEN}Reward modulation disabled:${NC} $NO_REWARD"
    fi
    
    echo ""
    echo -e "${YELLOW}Command to execute:${NC}"
    echo "  $CMD"
    echo ""
    
    # Check if executable exists
    if [ ! -f "./nlm" ]; then
        echo -e "${RED}Error: nlm executable not found!${NC}"
        echo "Please build the project first:"
        echo "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
        exit 1
    fi
    
    # Run the command
    echo -e "${GREEN}Running NLM Phase 6 demo...${NC}"
    echo ""
    eval $CMD
    
    # Check exit status
    if [ $? -eq 0 ]; then
        echo ""
        echo -e "${GREEN}✓ NLM Phase 6 demo completed successfully!${NC}"
    else
        echo ""
        echo -e "${RED}✗ NLM Phase 6 demo failed!${NC}"
        exit 1
    fi
}

# Run main function
main "$@"