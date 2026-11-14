#!/bin/bash
# NLM Demo Launcher Script
# This script provides easy access to NLM demo modes and automates common tasks

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default configuration
NLM_EXECUTABLE="./nlm"
NLM_CONFIG="configs/default.cfg"
NLM_OUTPUT_DIR="./nlm_output"
NLM_LOG_LEVEL="INFO"
NLM_SEED=42
QUIET=false
VERBOSE=false

# Help function
show_help() {
    cat << EOF
NLM Demo Launcher Script

Usage: $0 [OPTIONS] DEMO [ARGS...]

Options:
  --help, -h                Show this help message
  --config FILE              Load configuration from FILE
  --output-dir DIR           Set output directory (default: ./nlm_output)
  --log-level LEVEL          Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
  --seed N                   Set random seed
  --quiet, -q                Suppress non-essential output
  --verbose, -V              Enable verbose output

Available Demos:
  phase2                     Phase 2: Real Neural Computation (LIF neurons, STDP, plasticity)
  phase3 [episodes steps]    Phase 3: World Interaction (experience -> action -> learning)
  phase4                     Phase 4: Cognitive Integration (memory, prediction, planning)
  phase5                     Phase 5: Personality & Sandbox (sandbox learning)
  phase6                     Phase 6: Final Integration (comprehensive system test)
  benchmark                  Performance benchmarks for different network sizes

Examples:
  $0 phase2                                  # Run Phase 2 demo with default settings
  $0 phase3 10 500                          # Run Phase 3 with 10 episodes, 500 steps each
  $0 --config myconfig.cfg phase5           # Run Phase 5 with custom config
  $0 --seed 123 benchmark                   # Run benchmark with reproducible seed
  $0 --verbose phase2                        # Run Phase 2 with verbose output
  $0 --output-dir ./my_results phase3 5 200  # Custom output directory

For more detailed information, use:
  $0 demo --help
  $0 --help
EOF
}

# Execute demo function
execute_demo() {
    local demo_mode="$1"
    shift
    
    # Build NLM command
    local cmd="$NLM_EXECUTABLE"
    
    # Add global options
    cmd+=" --config $NLM_CONFIG"
    cmd+=" --seed $NLM_SEED"
    cmd+=" --output-dir $NLM_OUTPUT_DIR"
    
    if [[ "$NLM_LOG_LEVEL" != "INFO" ]]; then
        cmd+=" --log-level $NLM_LOG_LEVEL"
    fi
    
    if [[ "$QUIET" == "true" ]]; then
        cmd+=" --quiet"
    fi
    
    if [[ "$VERBOSE" == "true" ]]; then
        cmd+=" --verbose"
    fi
    
    # Add demo command and arguments
    cmd+=" demo $demo_mode"
    
    # Add remaining arguments
    for arg in "$@"; do
        cmd+=" $arg"
    done
    
    echo -e "${GREEN}Executing: $cmd${NC}"
    
    # Create output directory
    mkdir -p "$NLM_OUTPUT_DIR"
    
    # Execute the command
    eval "$cmd"
}

# Parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help|-h)
                show_help
                exit 0
                ;;
            --config)
                NLM_CONFIG="$2"
                shift 2
                ;;
            --output-dir)
                NLM_OUTPUT_DIR="$2"
                shift 2
                ;;
            --log-level)
                NLM_LOG_LEVEL="$2"
                shift 2
                ;;
            --seed)
                NLM_SEED="$2"
                shift 2
                ;;
            --quiet|-q)
                QUIET=true
                shift
                ;;
            --verbose|-V)
                VERBOSE=true
                shift
                ;;
            *)
                # Pass through to demo execution
                execute_demo "$1" "$@"
                return 0
                ;;
        esac
    done
    
    # No arguments provided
    show_help
}

# Main script execution
parse_arguments "$@"
