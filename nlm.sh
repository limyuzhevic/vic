#!/usr/bin/env bash

# NLM Command Line Interface (nlm.sh)
# Enhanced CLI with subcommands for all operations

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
NLM_BINARY="${SCRIPT_DIR}/nlm"
PYTHON_SCRIPT="${SCRIPT_DIR}/pynlm.py"

# Default configuration
DEFAULT_CONFIG="configs/default.cfg"
DEFAULT_LOG_LEVEL="INFO"
DEFAULT_COLOR="true"

# Set executable permission
chmod +x "$NLM_BINARY" 2>/dev/null || true

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Check if colors should be disabled
if [[ "${NO_COLOR}" == "true" ]] || ! command -v tput >/dev/null 2>&1; then
    RED='' GREEN='' YELLOW='' BLUE='' CYAN='' NC=''
fi

# Logging function
log() {
    local level="$1"
    shift
    local message="$*"
    local timestamp="$(date '+%H:%M:%S')"
    
    case "${level}" in
        "INFO")
            echo -e "${BLUE}[${timestamp} INFO]${NC} ${message}" >&2
            ;;
        "WARN")
            echo -e "${YELLOW}[${timestamp} WARN]${NC} ${message}" >&2
            ;;
        "ERROR")
            echo -e "${RED}[${timestamp} ERROR]${NC} ${message}" >&2
            ;;
        "SUCCESS")
            echo -e "${GREEN}[${timestamp} SUCCESS]${NC} ${message}" >&2
            ;;
        *)
            echo -e "${CYAN}[${timestamp} ${level}]${NC} ${message}" >&2
            ;;
    esac
}

# Print help
print_help() {
    cat << EOF
$(printf "${GREEN}NLM — Neural Learning Machine${NC}")
$(printf "${CYAN}An experimental artificial developmental brain${NC}")

${BLUE}USAGE:${NC}
  nlm [COMMAND] [OPTIONS]

${BLUE}COMMANDS:${NC}
  run           Run demonstrations or experiments
  config        Configuration management
  export        Export brain state or data
  import        Import brain state or data
  script        Run script mode for automated execution
  debug         Debugging and profiling tools
  help          Show help information

${BLUE}ENVIRONMENT:${NC}
  NLM_CONFIG    Path to config file (default: configs/default.cfg)
  NLM_LOG_LEVEL Logging level (INFO, WARN, ERROR, DEBUG)
  NO_COLOR      Disable colored output
  NLM_VERBOSE   Enable verbose output

${BLUE}EXAMPLES:${NC}
  # Run Phase 6 integration demo
  nlm run phase6-demo --episodes 3 --steps 1000

  # Run custom experiment with specific config
  nlm run phase3 --config my_config.cfg --seed 123

  # Export brain state for analysis
  nlm export brain --file brain_state.json --format json

  # Run automated experiment series
  nlm script experiments.txt

  # Debug with neural visualization
  nlm debug profile --time 10.0 --output profile.txt

EOF
}

# Print command help
print_command_help() {
    local command="$1"
    case "${command}" in
        "run")
            cat << EOF
${BLUE}Run demonstrations or experiments${NC}

${CYAN}USAGE:${NC}
  nlm run [DEMO] [OPTIONS]

${CYAN}DEMOS:${NC}
  phase6-demo    Phase 6 integration test (default)
  phase4-demo    Phase 4 cognition experiments
  phase3-demo    Phase 3 world interaction
  basic         Basic neural computation test

${CYAN}COMMON OPTIONS:${NC}
  --config FILE     Configuration file path (default: configs/default.cfg)
  --seed NUM        Random seed for reproducibility
  --episodes N      Number of episodes (for demos using episodes)
  --steps N         Number of steps per episode
  --help            Show this help

${CYAN}EXAMPLES:${NC}
  nlm run phase6-demo --episodes 3 --steps 1000
  nlm run phase4-demo --trials 100
  nlm run basic --config custom.cfg
EOF
            ;;

        "config")
            cat << EOF
${BLUE}Configuration management${NC}

${CYAN}USAGE:${NC}
  nlm config [COMMAND] [OPTIONS]

${CYAN}COMMANDS:${NC}
  list            List all available configuration options
  show [KEY]      Show configuration values
  set KEY VALUE    Set configuration value
  validate        Validate current configuration
  create FILE      Create new configuration file from template
  example FILE     Generate example configuration

${CYAN}EXAMPLES:${NC}
  nlm config list
  nlm config show neuron_count region_count
  nlm config validate
  nlm config create myconfig.cfg
EOF
            ;;

        "export")
            cat << EOF
${BLUE}Export brain state or data${NC}

${CYAN}USAGE:${NC}
  nlm export [TYPE] [OPTIONS]

${CYAN}EXPORT TYPES:${NC}
  brain           Export current brain state (network, synapses, weights)
  memories        Export memory systems state
  config          Export current configuration
  metrics         Export performance metrics
  visualization   Export data for visualization

${CYAN}COMMON OPTIONS:${NC}
  --file FILE      Output file path
  --format FORMAT  Export format (json, binary, csv, txt)
  --compress       Compress output
  --help           Show this help

${CYAN}EXAMPLES:${NC}
  nlm export brain --file brain_state.json --format json
  nlm export memories --file memories.bin --format binary
  nlm export config --file config_export.txt --format txt
EOF
            ;;

        "import")
            cat << EOF
${BLUE}Import brain state or data${NC}

${CYAN}USAGE:${NC}
  nlm import [TYPE] [OPTIONS]

${CYAN}IMPORT TYPES:${NC}
  brain           Import brain state (restores neural network)
  config          Import configuration
  checkpoints     Import from checkpoint directory

${CYAN}COMMON OPTIONS:${NC}
  --file FILE      Input file path
  --format FORMAT  Import format (json, binary, cfg)
  --backup         Create backup before import
  --help           Show this help

${CYAN}EXAMPLES:${NC}
  nlm import brain --file saved_brain.bin --format binary
  nlm import config --file settings.cfg --backup
EOF
            ;;

        "script")
            cat << EOF
${BLUE}Run script mode for automated execution${NC}

${CYAN}USAGE:${NC}
  nlm script [OPTIONS] FILE

${CYAN}SCRIPT FILE FORMAT:${NC}
  Lines starting with # are comments
  Empty lines are ignored
  Each line is a command: COMMAND [OPTIONS]
  Supported commands: run, export, import, debug

${CYAN}COMMON OPTIONS:${NC}
  --variable VAR=VALUE    Set script variable
  --verbose               Enable verbose output
  --dry-run               Show what would be executed without running
  --help                  Show this help

${CYAN}EXAMPLES:${NC}
  nlm script experiment1.txt
  nlm script --variable EPISODES=10 experiment.txt
  nlm script --dry-run analysis.txt
EOF
            ;;

        "debug")
            cat << EOF
${BLUE}Debugging and profiling tools${NC}

${CYAN}USAGE:${NC}
  nlm debug [COMMAND] [OPTIONS]

${CYAN}DEBUG COMMANDS:${NC}
  profile           Profile performance and resource usage
  trace            Enable detailed execution tracing
  validate         Validate internal consistency
  memory           Check memory usage and leaks
  neural           Debug neural network state
  simulate         Run simulation with debug output

${CYAN}COMMON OPTIONS:${NC}
  --time SECONDS    Duration of profiling
  --output FILE     Output file for results
  --level LEVEL     Debug level (low, medium, high)
  --help            Show this help

${CYAN}EXAMPLES:${NC}
  nlm debug profile --time 10.0 --output profile.txt
  nlm debug trace --level high --output trace.log
  nlm debug validate --output validation_report.txt
EOF
            ;;

        "help"|*)
            print_help
            ;;
    esac
}

# Main function
main() {
    local command="${1:-help}"
    shift
    
    case "${command}" in
        "help")
            print_command_help "${2:-}"
            ;;
        "run")
            run_command "${@}"
            ;;
        "config")
            config_command "${@}"
            ;;
        "export")
            export_command "${@}"
            ;;
        "import")
            import_command "${@}"
            ;;
        "script")
            script_command "${@}"
            ;;
        "debug")
            debug_command "${@}"
            ;;
        *)
            echo -e "${RED}Error: Unknown command '${command}'${NC}" >&2
            print_help
            return 1
            ;;
    esac
}

# Run command implementation
run_command() {
    local demo="${1:-phase6-demo}"
    shift
    
    local config_file="${NLM_CONFIG:-${DEFAULT_CONFIG}}"
    local seed=""
    local episodes=""
    local steps=""
    
    # Parse options
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --config)
                config_file="$2"
                shift 2
                ;;
            --seed)
                seed="$2"
                shift 2
                ;;
            --episodes)
                episodes="$2"
                shift 2
                ;;
            --steps)
                steps="$2"
                shift 2
                ;;
            --help|-h)
                print_command_help "run"
                return 0
                ;;
            *)
                echo -e "${RED}Error: Unknown option '$1'${NC}" >&2
                print_command_help "run"
                return 1
                ;;
        esac
    done
    
    log "INFO" "Running ${demo} with config: ${config_file}"
    
    # Build command line
    local cmd_args="${demo}"
    [[ -n "${seed}" ]] && cmd_args+=" --seed ${seed}"
    [[ -n "${episodes}" ]] && cmd_args+=" --episodes ${episodes}"
    [[ -n "${steps}" ]] && cmd_args+=" --steps ${steps}"
    
    # Execute the demo
    if [[ -x "${NLM_BINARY}" ]]; then
        "${NLM_BINARY}" ${cmd_args}
    else
        echo -e "${RED}Error: NLM binary not found or not executable${NC}" >&2
        return 1
    fi
}

# Config command implementation
config_command() {
    local subcmd="${1:-list}"
    shift
    
    case "${subcmd}" in
        "list")
            echo "${CYAN}Available configuration options:${NC}"
            echo "  random_seed              Random seed for experiments"
            echo "  simulation_timestep      Time step duration"
            echo "  neuron_count             Number of neurons"
            echo "  region_count             Number of brain regions"
            echo "  connection_probability   Connection probability"
            echo "  stdp_ltp_weight          STDP LTP weight"
            echo "  stdp_ltd_weight          STDP LTD weight"
            echo "  synaptogenesis_rate      Synaptogenesis rate"
            echo "  pruning_rate             Pruning rate"
            echo "  dopamine_baseline        Dopamine baseline level"
            echo "  reward_discount_factor   Reward discount factor"
            echo "  log_level                Logging level"
            echo "  log_to_file              Log to file"
            echo "  max_simulation_steps     Maximum simulation steps"
            echo "  visualization_enabled    Enable visualization"
            ;;
        
        "show")
            if [[ $# -eq 0 ]]; then
                echo -e "${RED}Error: Please specify configuration keys${NC}" >&2
                return 1
            fi
            # Would normally read from config file
            log "INFO" "Would show configuration for: $*"
            ;;
        
        "set")
            if [[ $# -lt 2 ]]; then
                echo -e "${RED}Error: Please specify key and value${NC}" >&2
                return 1
            fi
            local key="$1"
            local value="$2"
            log "INFO" "Would set configuration: ${key}=${value}"
            ;;
        
        "validate")
            log "INFO" "Would validate configuration"
            ;;
        
        "create")
            if [[ $# -eq 0 ]]; then
                echo -e "${RED}Error: Please specify output file${NC}" >&2
                return 1
            fi
            log "INFO" "Would create configuration template: $1"
            ;;
        
        "example")
            if [[ $# -eq 0 ]]; then
                echo -e "${RED}Error: Please specify output file${NC}" >&2
                return 1
            fi
            log "INFO" "Would create example configuration: $1"
            ;;
        
        "--help|-h")
            print_command_help "config"
            ;;
        *)
            echo -e "${RED}Error: Unknown config subcommand '${subcmd}'${NC}" >&2
            print_command_help "config"
            ;;
    esac
}

# Export command implementation (stub)
export_command() {
    local type="${1:-brain}"
    shift
    
    log "INFO" "Exporting ${type}..."
    # Implementation would go here
    echo -e "${YELLOW}Export functionality would be implemented here${NC}" >&2
    return 0
}

# Import command implementation (stub)
import_command() {
    local type="${1:-brain}"
    shift
    
    log "INFO" "Importing ${type}..."
    # Implementation would go here
    echo -e "${YELLOW}Import functionality would be implemented here${NC}" >&2
    return 0
}

# Script command implementation (stub)
script_command() {
    if [[ $# -eq 0 ]]; then
        echo -e "${RED}Error: Please specify script file${NC}" >&2
        return 1
    fi
    
    local script_file="$1"
    shift
    
    if [[ ! -f "${script_file}" ]]; then
        echo -e "${RED}Error: Script file not found: ${script_file}${NC}" >&2
        return 1
    fi
    
    log "INFO" "Running script: ${script_file}"
    # Implementation would go here
    echo -e "${YELLOW}Script execution functionality would be implemented here${NC}" >&2
    return 0
}

# Debug command implementation (stub)
debug_command() {
    local subcommand="${1:-profile}"
    shift
    
    log "INFO" "Debug ${subcommand}..."
    # Implementation would go here
    echo -e "${YELLOW}Debug functionality would be implemented here${NC}" >&2
    return 0
}

# Execute main function with all arguments
main "${@}"
