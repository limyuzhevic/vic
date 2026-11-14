chmod +x "$0"

# NLM Configuration Validation Script
# Validates NLM configuration files and generates schema documentation

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default configuration
NLM_EXECUTABLE="./nlm"
CONFIG_DIR="configs"

# Help function
show_help() {
    cat << EOF
NLM Configuration Validation Script

Usage: $0 [OPTIONS] CONFIG_FILE

Options:
  --help, -h                    Show this help message
  --schema, -s                  Generate configuration schema documentation
  --validate, -v                Validate configuration file
  --warnings-only, -w           Show only warnings (not errors)
  --quiet, -q                   Suppress non-essential output
  --verbose, -V                 Enable verbose output

Examples:
  $0 --validate configs/default.cfg
  $0 --schema > nlm_config_schema.md
  $0 -v -w configs/my_config.cfg

EOF
}

# Generate schema documentation
generate_schema() {
    local output_file="${1:-nlm_config_schema.md}"
    
    echo -e "${GREEN}Generating NLM configuration schema documentation...${NC}"
    
    # Compile and run NLM with config schema generation
    if ! g++ -std=c++20 -I./src -I./src/core -I./src/brain -I./src/experiments -c -o /tmp/nlm_config_generator.o src/main.cpp; then
        echo -e "${RED}ERROR: Failed to compile NLM with schema generation${NC}"
        return 1
    fi
    
    if ! g++ -std=c++20 /tmp/nlm_config_generator.o -o /tmp/nlm_config_generator; then
        echo -e "${RED}ERROR: Failed to link NLM with schema generation${NC}"
        return 1
    fi
    
    # Run schema generation
    ./tmp/nlm_config_generator --config-schema > "$output_file"
    
    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}Schema documentation saved to: $output_file${NC}"
    else
        echo -e "${RED}ERROR: Failed to generate schema documentation${NC}"
        return 1
    fi
}

# Validate configuration file
validate_config() {
    local config_file="$1"
    local warnings_only="$2"
    
    if [[ ! -f "$config_file" ]]; then
        echo -e "${RED}ERROR: Configuration file not found: $config_file${NC}"
        return 1
    fi
    
    echo -e "${GREEN}Validating configuration file: $config_file${NC}"
    
    # Compile and run NLM with config validation
    if ! g++ -std=c++20 -I./src -I./src/core -I./src/brain -I./src/experiments -c -o /tmp/nlm_config_validator.o src/main.cpp; then
        echo -e "${RED}ERROR: Failed to compile NLM with config validation${NC}"
        return 1
    fi
    
    if ! g++ -std=c++20 /tmp/nlm_config_validator.o -o /tmp/nlm_config_validator; then
        echo -e "${RED}ERROR: Failed to link NLM with config validation${NC}"
        return 1
    fi
    
    # Run validation
    ./tmp/nlm_config_validator --validate "$config_file" "$warnings_only"
    
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        echo -e "${GREEN}Configuration validation completed${NC}"
    else
        echo -e "${RED}Configuration validation failed${NC}"
    fi
    
    return $exit_code
}

# Main script execution
main() {
    local validate_only=false
    local warnings_only=false
    local verbose=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help|-h)
                show_help
                exit 0
                ;;
            --schema|-s)
                if [[ $# -lt 2 ]]; then
                    echo -e "${RED}ERROR: Output file required for schema generation${NC}"
                    exit 1
                fi
                generate_schema "$2"
                shift 2
                exit $?
                ;;
            --validate|-v)
                validate_only=true
                shift
                ;;
            --warnings-only|-w)
                warnings_only=true
                shift
                ;;
            --verbose|-V)
                verbose=true
                shift
                ;;
            *)
                if [[ "$validate_only" == "true" ]]; then
                    validate_config "$1" "$warnings_only"
                    shift
                    exit $?
                else
                    echo -e "${RED}ERROR: No command specified. Use '--help' for usage.${NC}"
                    exit 1
                fi
                ;;
        esac
    done
    
    # No arguments provided
    show_help
}

# Execute main function
main "$@"
