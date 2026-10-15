#!/bin/bash
# NLM Configuration Validator
# 
# This script validates NLM configuration files and checks for:
# - Required parameters
# - Parameter types and ranges
# - Section consistency
# - File syntax
# 
# Usage:
#   ./validate_config.sh config_file.cfg
#   ./validate_config.sh configs/default.cfg
#   ./validate_config.sh --help

#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default validation rules
declare -A REQUIRED_PARAMS
REQUIRED_PARAMS[
    "random_seed"
    "simulation_timestep"
    "neuron_count"
    "region_count"
    "connection_probability"
    "log_level"
]

declare -A PARAM_TYPES
PARAM_TYPES["random_seed"]="int"
PARAM_TYPES["simulation_timestep"]="float"
PARAM_TYPES["neuron_count"]="int"
PARAM_TYPES["region_count"]="int"
PARAM_TYPES["connection_probability"]="float"
PARAM_TYPES["log_level"]="string"
PARAM_TYPES["plasticity_learning_rate"]="float"
PARAM_TYPES["stdp_ltp_weight"]="float"
PARAM_TYPES["stdp_ltd_weight"]="float"
PARAM_TYPES["stdp_time_constant"]="float"
PARAM_TYPES["synaptogenesis_rate"]="float"
PARAM_TYPES["pruning_rate"]="float"
PARAM_TYPES["dopamine_scale"]="float"
PARAM_TYPES["reward_discount_factor"]="float"
PARAM_TYPES["environment_width"]="int"
PARAM_TYPES["environment_height"]="int"
PARAM_TYPES["max_simulation_steps"]="int"
PARAM_TYPES["visualization_update_rate"]="float"

declare -A PARAM_RANGES
PARAM_RANGES["random_seed"]="0-2147483647"
PARAM_RANGES["simulation_timestep"]="0.0001-1.0"
PARAM_RANGES["neuron_count"]="10-100000"
PARAM_RANGES["region_count"]="1-1000"
PARAM_RANGES["connection_probability"]="0.0-1.0"
PARAM_RANGES["plasticity_learning_rate"]="0.0-1.0"
PARAM_RANGES["stdp_ltp_weight"]="0.0-1.0"
PARAM_RANGES["stdp_ltd_weight"]="0.0-1.0"
PARAM_RANGES["stdp_time_constant"]="1.0-1000.0"
PARAM_RANGES["synaptogenesis_rate"]="0.000001-0.1"
PARAM_RANGES["pruning_rate"]="0.00000001-0.01"
PARAM_RANGES["dopamine_scale"]="0.0-10.0"
PARAM_RANGES["reward_discount_factor"]="0.0-1.0"
PARAM_RANGES["environment_width"]="1-1000"
PARAM_RANGES["environment_height"]="1-1000"
PARAM_RANGES["max_simulation_steps"]="1-1000000"
PARAM_RANGES["visualization_update_rate"]="1.0-1000.0"

LOG_LEVEL_VALID="DEBUG INFO WARNING ERROR FATAL"

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_help() {
    cat << EOF
NLM Configuration Validator
===========================

Validates NLM configuration files for syntax, required parameters, and parameter ranges.

Usage:
  $0 CONFIG_FILE
  $0 --help
  $0 --defaults  # Show default values for all parameters

Validation Checks:
  - File syntax (key=value format)
  - Required parameters presence
  - Parameter types (int, float, string, bool)
  - Parameter value ranges
  - Log level validation
  - Configuration section consistency

Example:
  ./validate_config.sh configs/default.cfg

Output:
  Reports:
  - Valid/invalid configuration status
  - Missing or invalid parameters
  - Suggested fixes
  - Summary statistics
EOF
    exit 0
}

print_defaults() {
    cat << EOF
NLM Configuration Defaults
===========================

Basic Settings:
  random_seed = 42
  simulation_timestep = 0.001
  neuron_count = 500
  region_count = 1
  connection_probability = 0.15

Plasticity:
  plasticity_learning_rate = 0.01
  stdp_ltp_weight = 0.02
  stdp_ltd_weight = 0.015
  stdp_time_constant = 20.0

Development:
  synaptogenesis_rate = 0.0001
  pruning_rate = 0.00001

Neuromodulation:
  dopamine_scale = 1.0
  reward_discount_factor = 0.99

Environment:
  environment_name = GridWorld
  environment_width = 10
  environment_height = 10

Logging:
  log_level = INFO
  log_to_file = false
  log_filename = nlm.log

Simulation:
  max_simulation_steps = 10000
  simulation_time_limit = 0.0

Visualization:
  visualization_enabled = false
  visualization_update_rate = 30.0
EOF
    exit 0
}

parse_config_file() {
    local config_file="$1"
    declare -A config_params
    local line_num=0
    
    if [[ ! -f "$config_file" ]]; then
        print_error "Configuration file not found: $config_file"
        return 1
    fi
    
    while IFS= read -r line || [[ -n "$line" ]]; do
        line_num=$((line_num + 1))
        
        # Skip empty lines and comments
        [[ -z "$line" || "$line" =~ ^[[:space:]]*# ]] && continue
        [[ -z "$line" || "$line" =~ ^[[:space:]]*// ]] && continue
        
        # Remove inline comments
        line="${line%%#*}"
        line="${line%%//*}"
        line="${line%%;*}"
        
        # Trim whitespace
        line="${line##*( )}"
        line="${line%%*( )}"
        
        if [[ -z "$line" ]]; then
            continue
        fi
        
        # Parse key=value pair
        if [[ "$line" == *=* ]]; then
            local key="${line%%=*}"
            local value="${line#*=}"
            
            # Remove quotes
            key="${key##*( )}"
            key="${key%%*( )}"
            value="${value##*( )}"
            value="${value%%*( )}"
            
            if [[ "$value" == *'"'* ]]; then
                value="${value:1:-1}"
                value="${value#*'"'}"
            fi
            
            if [[ "$value" == *"'"* ]]; then
                value="${value:1:-1}"
                value="${value#*"'"}"
            fi
            
            config_params["$key"]="$value"
        else
            print_warning "Invalid syntax in $config_file line $line_num: $line"
        fi
    done < "$config_file"
    
    echo "$#" "$@"
}

validate_config() {
    local config_file="$1"
    declare -A config_params=("$@")
    
    local is_valid=true
    local error_count=0
    local warning_count=0
    
    # Check file syntax
    print_info "Checking syntax of $config_file..."
    
    # Check required parameters
    print_info "Checking required parameters..."
    for param in "${REQUIRED_PARAMS[@]}"; do
        if [[ -z "${config_params[$param]}" ]]; then
            print_error "Missing required parameter: $param"
            is_valid=false
            error_count=$((error_count + 1))
        fi
    done
    
    # Validate parameter types and ranges
    print_info "Validating parameter types and ranges..."
    for param in "${!config_params[@]}"; do
        if [[ -n "${config_params[$param]}" ]]; then
            # Check type
            if [[ -n "${PARAM_TYPES[$param]}" ]]; then
                local param_type="${PARAM_TYPES[$param]}"
                local value="${config_params[$param]}"
                
                case "$param_type" in
                    int)
                        if ! [[ "$value" =~ ^-?[0-9]+$ ]]; then
                            print_error "Parameter '$param' must be an integer, got: $value"
                            is_valid=false
                            error_count=$((error_count + 1))
                        fi
                        ;;
                    float)
                        if ! [[ "$value" =~ ^-?[0-9]*\.?[0-9]+$ ]]; then
                            print_error "Parameter '$param' must be a float, got: $value"
                            is_valid=false
                            error_count=$((error_count + 1))
                        fi
                        ;;
                    string)
                        if [[ "$value" =~ ^[0-9]+$ ]]; then
                            print_warning "Parameter '$param' appears to be numeric but should be a string: $value"
                            warning_count=$((warning_count + 1))
                        fi
                        ;;
                    bool)
                        if [[ "$value" != "true" && "$value" != "false" ]]; then
                            print_error "Parameter '$param' must be 'true' or 'false', got: $value"
                            is_valid=false
                            error_count=$((error_count + 1))
                        fi
                        ;;
                esac
            fi
            
            # Check range if defined
            if [[ -n "${PARAM_RANGES[$param]}" ]]; then
                local range="${PARAM_RANGES[$param]}"
                local value="${config_params[$param]}"
                
                if [[ "$param_type" == "int" ]]; then
                    if ! [[ "$value" =~ ^[0-9]+$ ]]; then
                        continue
                    fi
                    
                    if [[ "$range" == *-* ]]; then
                        IFS='-' read -r min max <<< "$range"
                        if (( value < min || value > max )); then
                            print_error "Parameter '$param' value $value is outside range [$min, $max]"
                            is_valid=false
                            error_count=$((error_count + 1))
                        fi
                    fi
                elif [[ "$param_type" == "float" ]]; then
                    if ! [[ "$value" =~ ^-?[0-9]*\.?[0-9]+$ ]]; then
                        continue
                    fi
                    
                    if [[ "$range" == *-* ]]; then
                        IFS='-' read -r min max <<< "$range"
                        local val_float=$(echo "$value" | sed 's/ //g')
                        if (( $(echo "$val_float < $min" | bc -l) )) || (( $(echo "$val_float > $max" | bc -l) )); then
                            print_error "Parameter '$param' value $value is outside range [$min, $max]"
                            is_valid=false
                            error_count=$((error_count + 1))
                        fi
                    fi
                fi
            fi
            
            # Special validation for log_level
            if [[ "$param" == "log_level" ]]; then
                local value="${config_params[$param]}"
                local found=false
                for level in $LOG_LEVEL_VALID; do
                    if [[ "$level" == "$value" ]]; then
                        found=true
                        break
                    fi
                done
                if [[ "$found" == "false" ]]; then
                    print_warning "Parameter 'log_level' has unusual value: $value. Valid values: $LOG_LEVEL_VALID"
                    warning_count=$((warning_count + 1))
                fi
            fi
        fi
    done
    
    # Validate section consistency
    print_info "Validating configuration consistency..."
    
    # Check neuron_count >= region_count
    if [[ -n "${config_params[neuron_count]}" && -n "${config_params[region_count]}" ]]; then
        local neurons="${config_params[neuron_count]}"
        local regions="${config_params[region_count]}"
        if (( neurons < regions )); then
            print_warning "neuron_count ($neurons) is less than region_count ($regions). This may cause issues."
            warning_count=$((warning_count + 1))
        fi
    fi
    
    # Check connection_probability in valid range
    if [[ -n "${config_params[connection_probability]}" ]]; then
        local prob="${config_params[connection_probability]}"
        if (( $(echo "$prob < 0 || $prob > 1" | bc -l) )); then
            print_error "connection_probability ($prob) must be between 0.0 and 1.0"
            is_valid=false
            error_count=$((error_count + 1))
        fi
    fi
    
    # Summary
    echo
    if [[ "$is_valid" == "true" ]]; then
        print_success "Configuration validation PASSED"
        print_info "Total warnings: $warning_count"
        if [[ $warning_count -gt 0 ]]; then
            print_info "Recommendations:" >&2
            print_info "  - Review warnings above" >&2
        fi
        return 0
    else
        print_error "Configuration validation FAILED"
        print_error "Total errors: $error_count, Warnings: $warning_count"
        if [[ $error_count -gt 0 ]]; then
            print_error "Configuration is invalid and should not be used"
            return 1
        else
            print_warning "Configuration has warnings but may be usable"
            return 2
        fi
    fi
}

# Main script execution
main() {
    # Parse command line arguments
    if [[ $# -eq 0 ]]; then
        print_help
        exit 0
    fi
    
    local action="validate"
    local config_file=""
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help)
                print_help
                exit 0
                ;;
            --defaults)
                print_defaults
                exit 0
                ;;
            -*)
                print_error "Unknown option: $1"
                print_help
                exit 1
                ;;
            *)
                config_file="$1"
                shift
                break
                ;;
        esac
    done
    
    if [[ -z "$config_file" ]]; then
        print_error "No configuration file specified"
        print_help
        exit 1
    fi
    
    # Parse configuration file
    local parse_result
    parse_result=$(parse_config_file "$config_file")
    local line_count=$parse_result
    
    print_info "Parsed $line_count lines from $config_file"
    
    # Validate configuration
    validate_config "$config_file" "${parse_result[@]}"
    local validation_result=$?
    
    if [[ $validation_result -eq 0 ]]; then
        exit 0
    elif [[ $validation_result -eq 1 ]]; then
        exit 1
    else
        exit 2
    fi
}

# Run main function with all arguments
main "$@"