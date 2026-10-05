#!/bin/bash
# NLM Command Line Interface (CLI) for Phase 2
# Provides advanced control over the Neural Learning Machine simulation

# CLI Version
VERSION="2.0.0"

# Default configuration path
DEFAULT_CONFIG="configs/default.cfg"
ADVANCED_CONFIG="configs/advanced.cfg"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to display help information
function show_help() {
    echo -e "${CYAN}NLM Command Line Interface (Version $VERSION)${NC}"
    echo -e "${YELLOW}Usage:${NC} nlm [OPTIONS] [COMMAND] [ARGS...]"
    echo ""
    echo -e "${YELLOW}Commands:${NC}"
    echo "  help                    Show this help message"
    echo "  version                 Show version information"
    echo ""
    echo -e "${YELLOW}Configuration:${NC}"
    echo "  --config FILE           Specify configuration file (default: configs/default.cfg)"
    echo "  --advanced-config       Use advanced configuration (configs/advanced.cfg)"
    echo "  --export-config FILE    Export current configuration to file"
    echo "  --import-config FILE    Import configuration from file"
    echo ""
    echo -e "${YELLOW}Simulation Control:${NC}"
    echo "  start [TIMESTEPS]       Start simulation (with optional steps)"
    echo "  pause                   Pause current simulation"
    echo "  resume                  Resume paused simulation"
    echo "  stop                    Stop simulation and save state"
    echo "  status                  Show current simulation status"
    echo ""
    echo -e "${YELLOW}Export/Import:${NC}"
    echo "  export STATE_FILE       Export current brain state to file"
    echo "  import STATE_FILE       Import brain state from file"
    echo ""
    echo -e "${YELLOW}Analysis:${NC}"
    echo "  analyze [STEPS]         Run analysis on simulation (with optional steps)"
    echo "  metrics                 Show current simulation metrics"
    echo "  profile                 Run performance profiling"
    echo ""
    echo -e "${YELLOW}Debugging:${NC}"
    echo "  debug                   Enable debug mode"
    echo "  test                    Run unit tests"
    echo "  log-level LEVEL         Set log level (debug, info, warning, error, critical)"
    echo "  show-log                Show recent log entries"
    echo ""
    echo -e "${YELLOW}Advanced:${NC}"
    echo "  checkpoint INTERVAL     Set checkpoint interval (in steps)"
    echo "  visualization ENABLE    Enable or disable visualization"
    echo "  export-format FORMAT    Set export format (binary, json)"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  nlm --config myconfig.cfg start 1000          # Start with custom config"
    echo "  nlm export brain_state.bin                   # Export current state"
    echo "  nlm import saved_state.bin                    # Import saved state"
    echo "  nlm analyze 500                              # Analyze 500 steps"
    echo "  nlm --advanced-config start                   # Use advanced configuration"
}

# Function to show version information
function show_version() {
    echo -e "${CYAN}NLM Neural Learning Machine${NC}"
    echo -e "Version: ${YELLOW}$VERSION${NC}"
    echo -e "Phase: ${YELLOW}2 - Real Neural Computation${NC}"
    echo -e "Description: ${CYAN}An experimental artificial developmental brain${NC}"
    echo ""
    echo -e "${YELLOW}Features:${NC}"
    echo "  • Real LIF neuron dynamics"
    echo "  • Event-driven spike propagation with delays"
    echo "  • STDP and Hebbian plasticity rules"
    echo "  » Structural plasticity (synaptogenesis/pruning)"
    echo ""
    echo -e "${YELLOW}Built with:${NC}"
    echo "  • C++20 with modern C++ patterns"
    echo "  • PIMPL pattern for encapsulation"
    echo "  • Comprehensive error handling"
    echo "  • Advanced logging and monitoring"
}

# Function to handle unknown commands
function handle_unknown() {
    echo -e "${RED}Error: Unknown command '$1'${NC}"
    echo -e "Type '${YELLOW}nlm help${NC}' for available commands."
    exit 1
}

# Function to handle simulation start
function start_simulation() {
    local steps=${1:-1000}  # Default to 1000 steps if not specified
    
    echo -e "${CYAN}Starting NLM simulation...${NC}"
    echo -e "Steps: ${YELLOW}$steps${NC}"
    echo -e "Config: ${YELLOW}$CONFIG_FILE${NC}"
    echo ""
    
    # Check if nlm executable exists
    if [[ ! -f "./nlm" ]]; then
        echo -e "${RED}Error: NLM executable not found. Please build the project first.${NC}"
        echo "Run: ${YELLOW}mkdir build && cd build && cmake .. && make -j4${NC}"
        exit 1
    fi
    
    # Run the simulation
    echo -e "${GREEN}Simulation running...${NC}"
    ./nlm --config "$CONFIG_FILE" "$steps"
}

# Function to handle export
function export_state() {
    local output_file="${1:-brain_state.bin}"
    
    echo -e "${CYAN}Exporting brain state to: ${YELLOW}$output_file${NC}"
    
    # Check if nlm executable exists
    if [[ ! -f "./nlm" ]]; then
        echo -e "${RED}Error: NLM executable not found.${NC}"
        exit 1
    fi
    
    # Run export
    ./nlm --export "$output_file"
}

# Function to handle import
function import_state() {
    local input_file="${1:-saved_state.bin}"
    
    if [[ ! -f "$input_file" ]]; then
        echo -e "${RED}Error: State file '$input_file' not found.${NC}"
        exit 1
    fi
    
    echo -e "${CYAN}Importing brain state from: ${YELLOW}$input_file${NC}"
    
    # Check if nlm executable exists
    if [[ ! -f "./nlm" ]]; then
        echo -e "${RED}Error: NLM executable not found.${NC}"
        exit 1
    fi
    
    # Run import
    ./nlm --import "$input_file"
}

# Function to handle analysis
function run_analysis() {
    local steps=${1:-500}
    
    echo -e "${CYAN}Running analysis for ${YELLOW}$steps${NC} steps..."
    
    # Check if nlm executable exists
    if [[ ! -f "./nlm" ]]; then
        echo -e "${RED}Error: NLM executable not found.${NC}"
        exit 1
    fi
    
    # Run analysis
    ./nlm --analyze "$steps"
}

# Function to handle debug mode
function run_debug() {
    echo -e "${YELLOW}Enabling debug mode...${NC}"
    
    # Check if nlm executable exists
    if [[ ! -f "./nlm" ]]; then
        echo -e "${RED}Error: NLM executable not found.${NC}"
        exit 1
    fi
    
    # Run with debug flag
    ./nlm --debug
}

# Function to handle tests
function run_tests() {
    echo -e "${CYAN}Running NLM unit tests...${NC}"
    
    if [[ ! -f "./nlm_test" ]]; then
        echo -e "${YELLOW}Warning: nlm_test not found. Running basic tests...${NC}"
        # Try to find test executables
        if [[ -f "./build/nlm_test" ]]; then
            ./build/nlm_test
        else
            echo -e "${RED}Error: No test executables found. Please build the project with tests.${NC}"
            exit 1
        fi
    else
        ./nlm_test
    fi
}

# Function to handle configuration export
function export_config() {
    local output_file="${1:-nlm_config.cfg}"
    
    echo -e "${CYAN}Exporting configuration to: ${YELLOW}$output_file${NC}"
    
    if [[ "$CONFIG_FILE" == "configs/advanced.cfg" ]]; then
        # Copy advanced config
        cp "$ADVANCED_CONFIG" "$output_file"
    else
        # Generate a basic config
        cat > "$output_file" << EOF
# NLM Configuration Export
# Generated on $(date)
# From: $CONFIG_FILE

environment = development
mode = interactive
log_level = info
enable_performance_metrics = true
simulation_timestep = 0.001
network_arch = standard
learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
hebbian_learning_rate = 0.01
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
working_memory_size = 1000
episodic_memory_capacity = 10000
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true
prediction_error_enabled = true
development_enabled = true
enable_visualization = true
enable_experimental = false
EOF
        echo -e "${GREEN}Configuration exported successfully.${NC}"
    fi
}

# Function to handle configuration import
function import_config() {
    local input_file="${1:-imported_config.cfg}"
    
    if [[ ! -f "$input_file" ]]; then
        echo -e "${RED}Error: Configuration file '$input_file' not found.${NC}"
        exit 1
    fi
    
    echo -e "${CYAN}Importing configuration from: ${YELLOW}$input_file${NC}"
    
    # Create backup of current config
    if [[ -f "$CONFIG_FILE" ]]; then
        cp "$CONFIG_FILE" "${CONFIG_FILE}.backup.$(date +%s)"
    fi
    
    # Copy the config
    cp "$input_file" "$CONFIG_FILE"
    echo -e "${GREEN}Configuration imported successfully.${NC}"
}

# Main script logic
case "$1" in
    help|--help|-h)
        show_help
        ;;
    version|--version|-v)
        show_version
        ;;
    start)
        CONFIG_FILE="${2:-$DEFAULT_CONFIG}"
        start_simulation "$3"
        ;;
    export)
        export_state "$2"
        ;;
    import)
        import_state "$2"
        ;;
    analyze)
        run_analysis "$2"
        ;;
    debug)
        run_debug
        ;;
    test)
        run_tests
        ;;
    export-config)
        export_config "$2"
        ;;
    import-config)
        import_config "$2"
        ;;
    *)
        if [[ "$1" == *"="* ]]; then
            # Handle --config=file option
            CONFIG_FILE="${1#*=}"
            shift
            start_simulation "$@"
        elif [[ "$1" == --config ]]; then
            CONFIG_FILE="$2"
            shift 2
            start_simulation "$@"
        elif [[ "$1" == --advanced-config ]]; then
            CONFIG_FILE="$ADVANCED_CONFIG"
            shift
            start_simulation "$@"
        else
            handle_unknown "$1"
        fi
        ;;
esac