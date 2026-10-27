#!/usr/bin/env bash

# NLM Command Line Interface
# Provides easy access to different phases and experiments

# Default configuration file
DEFAULT_CONFIG="configs/default.cfg"
ADVANCED_CONFIG="configs/advanced.cfg"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored help information
print_help() {
    echo -e "${BLUE}NLM - Neural Learning Machine${NC}"
    echo -e "${YELLOW}An experimental artificial developmental brain${NC}"
    echo ""
    echo "USAGE:"
    echo "  nlm [OPTIONS] [PHASE]"
    echo ""
    echo "PHASES:"
    echo "  phase2      Run Phase 2: Real Neural Computation"
    echo "  phase6      Run Phase 6: Integration Demo (default)"
    echo "  experiment  Run specific experiments"
    echo ""
    echo "OPTIONS:"
    echo "  --help              Show this help message"
    echo "  --version           Show version information"
    echo "  --config FILE        Specify configuration file (default: $DEFAULT_CONFIG)"
    echo "  --neurons N          Number of neurons (overrides config)"
    echo "  --regions R         Number of regions (overrides config)"
    echo "  --steps S           Maximum simulation steps (overrides config)"
    echo "  --seed SEED         Random seed (overrides config)"
    echo "  --verbose           Enable verbose output"
    echo "  --quiet             Suppress non-essential output"
    echo ""
    echo "EXPERIMENTS:"
    echo "  --run-experiment NAME    Run specific experiment (e.g., stdp_test, plasticity_test)"
    echo "  --benchmark              Run performance benchmarks"
    echo ""
    echo "ADVANCED OPTIONS:"
    echo "  --checkpoint PATH        Save brain state to checkpoint file"
    echo "  --load-checkpoint FILE   Load brain state from checkpoint"
    echo "  --validate-config        Validate configuration file"
    echo "  --analyze-architecture    Analyze brain architecture"
    echo ""
    echo "Examples:"
    echo "  nlm --config configs/advanced.cfg --neurons 500 --steps 2000"
    echo "  nlm --run-experiment stdp_test --verbose"
    echo "  nlm --benchmark --checkpoint test.bin"
    echo ""
}

# Print version information
print_version() {
    echo "NLM v2.0.0 (Phase 6.3 Integration)"
    echo "Experimental Artificial Developmental Brain"
    echo "Built with C++ and modern neural architectures"
    echo ""
    echo "Key features:"
    echo "- Real spiking neural computation (LIF neurons)"
    echo "- Event-driven spike propagation"
    echo "- STDP and Hebbian plasticity"
    echo "- Structural plasticity"
    echo "- Integrated memory systems"
    echo "- Neuromodulation (dopamine, curiosity)"
    echo "- Prediction and cognition"
    echo "- Development stages"
    echo "- Checkpoint and replay systems"
    echo ""
}

# Validate configuration file
validate_config() {
    local config_file="$1"
    if [[ ! -f "$config_file" ]]; then
        echo -e "${RED}Error: Configuration file not found: $config_file${NC}"
        return 1
    fi
    # Check if file has valid configuration structure
    if ! grep -q "random_seed =" "$config_file" 2>/dev/null; then
        echo -e "${RED}Error: Invalid configuration file format${NC}"
        return 1
    fi
    echo -e "${GREEN}Configuration file '$config_file' exists and is valid${NC}"
    return 0
}

# Run NLM with specified options
run_nlm() {
    local phase="$1"
    local config_file="$2"
    local neurons="$3"
    local regions="$4"
    local steps="$5"
    local seed="$6"
    local verbose="$7"
    local quiet="$8"
    local checkpoint="$9"
    local load_checkpoint="${10}"
    
    # Build the command
    local cmd="./nlm"
    
    # Add phase flag
    cmd+=" --phase $phase"
    
    # Add config if specified
    if [[ -n "$config_file" ]]; then
        cmd+=" --config $config_file"
    fi
    
    # Add overrides
    if [[ -n "$neurons" ]]; then
        cmd+=" --neurons $neurons"
    fi
    
    if [[ -n "$regions" ]]; then
        cmd+=" --regions $regions"
    fi
    
    if [[ -n "$steps" ]]; then
        cmd+=" --steps $steps"
    fi
    
    if [[ -n "$seed" ]]; then
        cmd+=" --seed $seed"
    fi
    
    # Add logging flags
    if [[ "$verbose" == "true" ]]; then
        cmd+=" --verbose"
    fi
    
    if [[ "$quiet" == "true" ]]; then
        cmd+=" --quiet"
    fi
    
    # Add checkpoint options
    if [[ -n "$checkpoint" ]]; then
        cmd+=" --checkpoint $checkpoint"
    fi
    
    if [[ -n "$load_checkpoint" ]]; then
        cmd+=" --load-checkpoint $load_checkpoint"
    fi
    
    echo -e "${BLUE}Running: $cmd${NC}"
    
    # Execute the command
    eval "$cmd"
    
    # Check exit status
    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}Command completed successfully${NC}"
    else
        echo -e "${RED}Command failed with exit code $?${NC}"
        return 1
    fi
}

# Parse command line arguments
parse_args() {
    local phase="phase6"
    local config_file="$DEFAULT_CONFIG"
    local neurons=""
    local regions=""
    local steps=""
    local seed=""
    local verbose=false
    local quiet=false
    local checkpoint=""
    local load_checkpoint=""
    local experiment_name=""
    local run_benchmark=false
    local validate_only=false
    local analyze_arch=false
    
    # Parse positional arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            phase2|phase6|experiment)
                phase="$1"
                shift
                ;;
            --help)
                print_help
                exit 0
                ;;
            --version)
                print_version
                exit 0
                ;;
            --config)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    config_file="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --config requires a file argument${NC}"
                    exit 1
                fi
                ;;
            --neurons)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    neurons="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --neurons requires a number${NC}"
                    exit 1
                fi
                ;;
            --regions)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    regions="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --regions requires a number${NC}"
                    exit 1
                fi
                ;;
            --steps)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    steps="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --steps requires a number${NC}"
                    exit 1
                fi
                ;;
            --seed)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    seed="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --seed requires a number${NC}"
                    exit 1
                fi
                ;;
            --verbose)
                verbose=true
                shift
                ;;
            --quiet)
                quiet=true
                shift
                ;;
            --checkpoint)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    checkpoint="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --checkpoint requires a file path${NC}"
                    exit 1
                fi
                ;;
            --load-checkpoint)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    load_checkpoint="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --load-checkpoint requires a file path${NC}"
                    exit 1
                fi
                ;;
            --run-experiment)
                if [[ -n "$2" && ! "$2" =~ ^-- ]]; then
                    experiment_name="$2"
                    shift 2
                else
                    echo -e "${RED}Error: --run-experiment requires an experiment name${NC}"
                    exit 1
                fi
                ;;
            --benchmark)
                run_benchmark=true
                shift
                ;;
            --validate-config)
                validate_only=true
                shift
                ;;
            --analyze-architecture)
                analyze_arch=true
                shift
                ;;
            *)
                echo -e "${RED}Error: Unknown option: $1${NC}"
                print_help
                exit 1
                ;;
        esac
    done
    
    # Run validation if requested
    if [[ "$validate_only" == true ]]; then
        validate_config "$config_file"
        exit 0
    fi
    
    # Analyze architecture if requested
    if [[ "$analyze_arch" == true ]]; then
        echo -e "${BLUE}Analyzing NLM architecture...${NC}"
        if [[ -f "./analyze_arch" ]]; then
            ./analyze_arch "$config_file"
        else
            echo -e "${YELLOW}Architecture analyzer not found. Running basic analysis...${NC}"
            ./nlm --phase phase6 --config "$config_file" --verbose
        fi
        exit 0
    fi
    
    # Run experiment if requested
    if [[ -n "$experiment_name" ]]; then
        echo -e "${BLUE}Running experiment: $experiment_name${NC}"
        # Create experiment runner script
        cat > ./run_experiment.sh << EOF
#!/bin/bash
cd "$(dirname "$0")"
./nlm --phase experiment --experiment-name "$experiment_name" --config "$config_file"
EOF
        chmod +x ./run_experiment.sh
        ./run_experiment.sh
        return $?
    fi
    
    # Run benchmark if requested
    if [[ "$run_benchmark" == true ]]; then
        echo -e "${BLUE}Running performance benchmarks...${NC}"
        if [[ -f "./benchmark" ]]; then
            ./benchmark --config "$config_file"
        else
            echo -e "${YELLOW}Benchmark tool not found. Running Phase 6 demo as performance test...${NC}"
            ./nlm --phase phase6 --config "$config_file" --verbose
        fi
        return $?
    fi
    
    # Run main NLM program
    run_nlm "$phase" "$config_file" "$neurons" "$regions" "$steps" "$seed" "$verbose" "$quiet" "$checkpoint" "$load_checkpoint"
}

# Main execution
parse_args "$@"