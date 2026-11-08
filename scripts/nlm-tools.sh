#!/usr/bin/env bash
# NLM Command-Line Tools Wrapper Script
# This script provides easy access to NLM's advanced command-line utilities

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default configuration
NLM_EXECUTABLE="${NLM_EXECUTABLE:-./nlm}"
NLM_CONFIG_FILE="${NLM_CONFIG_FILE:-configs/default.cfg}"
NLM_LOG_LEVEL="${NLM_LOG_LEVEL:-Info}"
NLM_OUTPUT_FILE=""
NLM_VALIDATION_ONLY=false
NLM_SAVE_CONFIG=false
NLM_SAVE_CONFIG_FILE=""
NLM_TEST_MODE=""

# Help function
show_help() {
    echo -e "${CYAN}=== NLM Command-Line Tools ===${NC}"
    echo -e "${BLUE}NLM is an experimental artificial developmental brain simulator.${NC}"
    echo ""
    echo -e "${YELLOW}USAGE:${NC}"
    echo "  nlm-run [OPTIONS] [KEY=VALUE]..."
    echo "  nlm-tools [COMMAND] [OPTIONS]"
    echo ""
    echo -e "${YELLOW}QUICK START:${NC}"
    echo "  nlm-run              Run the standard NLM demo (3 tests)"
    echo "  nlm-run --help       Show help information"
    echo "  nlm-run --version    Show version information"
    echo "  nlm-run --config brain_config.json   Load JSON configuration"
    echo ""
    echo -e "${YELLOW}AVAILABLE COMMANDS (nlm-tools):${NC}"
    echo "  run                   Run NLM simulation"
    echo "  validate [FILE]       Validate configuration file"
    echo "  benchmark [ITERATIONS] Run benchmark test"
    echo "  config [COMMAND]      Configuration management"
    echo "  analysis [FILE]       Analyze simulation results"
    echo "  export [DIR]          Export results"
    echo "  import [FILE]         Import results"
    echo "  stats                 Show system statistics"
    echo "  help                  Show help information"
    echo ""
    echo -e "${YELLOW}RUN OPTIONS:${NC}"
    echo "  --help, -h            Show help"
    echo "  --version, -v         Show version"
    echo "  --config FILE, -c FILE  Load configuration (JSON/YAML/.cfg)"
    echo "  --log-level LEVEL     Set logging level (Debug, Info, Warning, Error, Critical)"
    echo "  --output-file FILE     Write logs to file"
    echo "  --validate            Validate configuration and exit"
    echo "  --save-config FILE     Save current config to file"
    echo "  --num-steps STEPS     Override number of simulation steps"
    echo "  --neuron-count COUNT  Override neuron count"
    echo "  --region-count COUNT  Override region count"
    echo "  --random-seed SEED     Override random seed"
    echo "  --plasticity-only      Run plasticity test only"
    echo "  --stdp-only            Run STDP verification only"
    echo "  --connectivity-only    Run connectivity test only"
    echo "  --reset               Reset brain before each test"
    echo ""
    echo -e "${YELLOW}CONFIGURATION MANAGEMENT:${NC}"
    echo "  nlm-tools config create [TYPE]           Create new config"
    echo "  nlm-tools config validate [FILE]         Validate config file"
    echo "  nlm-tools config export [FILE]           Export config to file"
    echo "  nlm-tools config import [FILE]           Import config from file"
    echo "  nlm-tools config list                    List all config keys"
    echo "  nlm-tools config get KEY                 Get config value"
    echo "  nlm-tools config set KEY VALUE           Set config value"
    echo ""
    echo -e "${YELLOW}ANALYSIS TOOLS:${NC}"
    echo "  nlm-tools analysis FILE                  Analyze simulation results"
    echo "  nlm-tools stats                            Show system statistics"
    echo ""
    echo -e "${YELLOW}EXAMPLES:${NC}"
    echo "  nlm-run --help"
    echo "  nlm-run --version"
    echo "  nlm-run --config brain_config.json --log-level Info"
    echo "  nlm-tools validate brain_config.json"
    echo "  nlm-tools config create json --neuron-count 1000"
    echo "  nlm-tools analysis simulation_results.csv"
    echo ""
    echo -e "${YELLOW}CONFIGURATION FILE TYPES:${NC}"
    echo "  JSON: brain_config.json"
    echo "  YAML: brain_config.yaml"
    echo "  Plain: brain_config.cfg"
    echo ""
    echo -e "${YELLOW}LOGGING LEVELS:${NC}"
    echo "  Debug   - Detailed debugging information"
    echo "  Info    - General information about execution"
    echo "  Warning - Potential issues that don't prevent execution"
    echo "  Error   - Errors that prevent completion of specific operations"
    echo "  Critical - Severe errors that prevent execution"
    echo ""
    echo -e "${YELLOW}EXIT CODES:${NC}"
    echo "  0  - Success"
    echo "  1  - General error"
    echo "  2  - Configuration error"
    echo "  3  - Simulation error"
    echo "  4  - Argument parsing error"
    echo ""
    echo -e "${BLUE}NLM Phase 2 Neural Learning Machine v0.1.0${NC}"
}

# Version function
show_version() {
    echo "NLM Phase 2 v0.1.0"
    echo "Neural Learning Machine - Real Neural Computation"
    echo "Built on $(date +%Y-%m-%d) at $(date +%H:%M:%S)"
    echo "License: MIT"
    echo "Documentation: https://nlm.readthedocs.io"
}

# Run NLM simulation
run_nlm() {
    local args="$@"
    
    # Build command
    local cmd="$NLM_EXECUTABLE"
    
    # Add options
    if [ -n "$NLM_CONFIG_FILE" ]; then
        cmd+=" --config $NLM_CONFIG_FILE"
    fi
    
    if [ -n "$NLM_LOG_LEVEL" ]; then
        cmd+=" --log-level $NLM_LOG_LEVEL"
    fi
    
    if [ -n "$NLM_OUTPUT_FILE" ]; then
        cmd+=" --output-file $NLM_OUTPUT_FILE"
    fi
    
    if [ "$NLM_VALIDATION_ONLY" = true ]; then
        cmd+=" --validate-config"
    fi
    
    if [ "$NLM_SAVE_CONFIG" = true ]; then
        cmd+=" --save-config $NLM_SAVE_CONFIG_FILE"
    fi
    
    # Add additional arguments
    for arg in $args; do
        if [[ "$arg" == *=* ]]; then
            cmd+=" $arg"
        else
            cmd+=" $arg"
        fi
    done
    
    echo -e "${GREEN}Running NLM simulation...${NC}"
    echo -e "${BLUE}Command: $cmd${NC}"
    echo ""
    
    # Run the simulation
    $cmd
    local exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}Simulation completed successfully!${NC}"
    else
        echo -e "${RED}Simulation failed with exit code: $exit_code${NC}"
    fi
    
    return $exit_code
}

# Validate configuration
validate_config() {
    local config_file="${1:-$NLM_CONFIG_FILE}"
    
    if [ ! -f "$config_file" ]; then
        echo -e "${RED}Error: Configuration file not found: $config_file${NC}"
        return 1
    fi
    
    echo -e "${YELLOW}Validating configuration file: $config_file${NC}"
    
    # Determine file type based on extension
    local ext="${config_file##*.}"
    ext="${ext,,}"  # Convert to lowercase
    
    case "$ext" in
        json)
            echo -e "  File type: JSON"
            if command -v python3 &> /dev/null; then
                python3 -c "import json; json.load(open('$config_file')); echo '  Validation: PASSED'"
            else
                echo -e "  Validation: SKIP (Python3 not available)"
            fi
            ;;
        yaml|yml)
            echo -e "  File type: YAML"
            if command -v python3 &> /dev/null; then
                python3 -c "import yaml; yaml.safe_load(open('$config_file')); echo '  Validation: PASSED'"
            else
                echo -e "  Validation: SKIP (Python3 not available)"
            fi
            ;;
        cfg)
            echo -e "  File type: Plain text (.cfg)"
            echo -e "  Validation: CHECKING format..."
            while IFS= read -r line; do
                # Skip comments and empty lines
                if [[ -z "$line" ]] || [[ "$line" =~ ^# ]]; then
                    continue
                fi
                
                # Check for key=value format
                if [[ "$line" =~ ^[^=]+=[^=]+$ ]]; then
                    echo -e "    OK: $line"
                else
                    echo -e "    ERROR: Invalid line format: $line"
                    return 1
                fi
            done < "$config_file"
            ;;
        *)
            echo -e "${RED}Error: Unsupported file type: .$ext${NC}"
            echo -e "Supported types: .json, .yaml, .yml, .cfg"
            return 1
            ;;
    esac
    
    echo -e "${GREEN}Configuration validation completed.${NC}"
}

# Create configuration
config_create() {
    local config_type="${1:-json}"
    local output_file="${2:-config.json}"
    
    case "$config_type" in
        json)
            cat > "$output_file" << EOF
{
    "nlm": {
        "version": "0.1.0",
        "description": "NLM Neural Learning Machine Configuration",
        "parameters": {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
            "plasticity_enabled": true,
            "structural_plasticity_enabled": false,
            "development_enabled": false,
            "reward_modulation_enabled": true,
            "curiosity_enabled": false,
            "novelty_detection_enabled": true
        }
    }
}
EOF
            ;;
        yaml)
            cat > "$output_file" << EOF
# NLM Configuration File
# Neural Learning Machine - Phase 2

nlm:
  version: "0.1.0"
  description: "NLM Neural Learning Machine Configuration"
  parameters:
    random_seed: 42
    simulation_timestep: 0.001
    neuron_count: 500
    region_count: 1
    connection_probability: 0.15
    plasticity_learning_rate: 0.01
    stdp_ltp_weight: 0.02
    stdp_ltd_weight: 0.015
    stdp_tau: 20.0
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001
    plasticity_enabled: true
    structural_plasticity_enabled: false
    development_enabled: false
    reward_modulation_enabled: true
    curiosity_enabled: false
    novelty_detection_enabled: true
EOF
            ;;
        cfg)
            cat > "$output_file" << EOF
# NLM Configuration File
# Neural Learning Machine - Phase 2

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of brain regions
region_count = 1

# Connection probability for random connectivity
connection_probability = 0.15

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Feature flags
plasticity_enabled = true
structural_plasticity_enabled = false
development_enabled = false
reward_modulation_enabled = true
curiosity_enabled = false
novelty_detection_enabled = true
EOF
            ;;
        *)
            echo -e "${RED}Error: Unsupported configuration type: $config_type${NC}"
            echo -e "Supported types: json, yaml, cfg"
            return 1
            ;;
    esac
    
    echo -e "${GREEN}Configuration created: $output_file${NC}"
}

# Configuration management
config_manage() {
    local command="$1"
    shift
    
    case "$command" in
        validate)
            validate_config "$1"
            ;;
        export)
            local output_file="${1:-config.json}"
            echo -e "${YELLOW}Exporting configuration to: $output_file${NC}"
            # This would require a config export function
            echo -e "${YELLOW}Config export not yet implemented.${NC}"
            ;;
        import)
            echo -e "${YELLOW}Importing configuration from: $1${NC}"
            # This would require a config import function
            echo -e "${YELLOW}Config import not yet implemented.${NC}"
            ;;
        *)
            echo -e "${RED}Unknown command: $command${NC}"
            ;;
    esac
}

# Analysis tool
run_analysis() {
    local analysis_file="${1:-simulation_results.csv}"
    
    if [ ! -f "$analysis_file" ]; then
        echo -e "${RED}Error: Analysis file not found: $analysis_file${NC}"
        return 1
    fi
    
    echo -e "${YELLOW}Analyzing simulation results: $analysis_file${NC}"
    
    # Basic analysis
    echo -e "  File type: ${YELLOW}$(file "$analysis_file" | cut -d: -f2)${NC}"
    
    if [[ "$analysis_file" == *.csv ]]; then
        local lines=$(wc -l < "$analysis_file")
        echo -e "  Lines: ${YELLOW}$lines${NC}"
        
        local fields=$(head -1 "$analysis_file" | sed 's/[^,]//g' | wc -c)
        echo -e "  Fields: ${YELLOW}$fields${NC}"
    fi
    
    echo -e "${GREEN}Analysis completed.${NC}"
}

# Statistics
show_stats() {
    echo -e "${CYAN}=== NLM System Statistics ===${NC}"
    echo -e "${YELLOW}NLM Version:${NC} $(show_version)"
    echo -e "${YELLOW}Configuration file:${NC} $NLM_CONFIG_FILE"
    echo -e "${YELLOW}Log level:${NC} $NLM_LOG_LEVEL"
    echo -e "${YELLOW}Output file:${NC} $NLM_OUTPUT_FILE"
    echo -e "${YELLOW}Current directory:${NC} $(pwd)"
    echo -e "${YELLOW}Available tools:${NC}"
    echo "  - nlm-run (basic NLM execution)"
    echo "  - nlm-tools (advanced utilities)"
    echo ""
    echo -e "${YELLOW}Build Information:${NC}"
    echo "  Platform: Linux"
    echo "  Architecture: x86_64"
   好的
def handle_error() {
    echo -e "${RED}Error: Missing required argument${NC}"
    return 1
}

# Main script logic
if [[ $# -eq 0 ]]; then
    show_help
    exit 0
fi

# Check for help/version before processing
if [[ "$1" == "--help" ]] || [[ "$1" == "-h" ]]; then
    show_help
    exit 0
fi

if [[ "$1" == "--version" ]] || [[ "$1" == "-v" ]]; then
    show_version
    exit 0
fi

# Handle subcommands
if [[ "$1" == "nlm-tools" ]]; then
    if [[ $# -lt 2 ]]; then
        echo -e "${RED}Error: Missing subcommand${NC}"
        exit 1
    fi
    
    command="$2"
    shift 2
    
    case "$command" in
        run)
            run_nlm "$@"
            ;;
        config)
            config_manage "$@"
            ;;
        validate)
            validate_config "$@"
            ;;
        analysis)
            run_analysis "$@"
            ;;
        stats)
            show_stats
            ;;
        *)
            echo -e "${RED}Unknown command: $command${NC}"
            exit 1
            ;;
    esac
    exit $?
fi

# Default execution (nlm-run)
run_nlm "$@"
