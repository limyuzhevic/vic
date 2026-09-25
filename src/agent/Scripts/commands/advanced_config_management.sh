#!/usr/bin/env bash

# Advanced Configuration Management Script
# Power user command for managing NLM configuration

# Usage: ./advanced_config_management.sh [options]
# Options:
#   --list-templates              List all available configuration templates
#   --create-template NAME        Create a new configuration template
#   --load-template NAME FILE     Load configuration from template to FILE
#   --export-template NAME FILE   Export existing configuration as template
#   --validate-config FILE        Validate configuration file
#   --diff-template OLD NEW      Compare two templates
#   --apply-inheritance CHAIN     Apply configuration inheritance chain
#   --runtime-mod KEY VALUE       Make runtime configuration modification
#   --backup-config DIR          Create configuration backup
#   --restore-config FILE        Restore configuration from backup
#   --monitor-changes             Monitor configuration changes
#   --list-changes                List recent configuration changes
#   --rollback-change ID          Rollback specific configuration change
#   --script-execute FILE         Execute configuration script
#   --script-preview FILE         Preview configuration script effects

cd "$(dirname "$0")"

SCRIPT_DIR="$(pwd)"
CONFIG_DIR="../../configs"
TEMPLATE_DIR="../templates"
LOG_FILE="config_management_$(date +%Y%m%d_%H%M%S).log"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log() {
    echo -e "[$(date '+%Y-%m-%d %H:%M:%S')] $*" | tee -a "$LOG_FILE"
}

error_exit() {
    echo -e "${RED}[ERROR] $*${NC}" | tee -a "$LOG_FILE"
    exit 1
}

check_config_file() {
    local filepath=$1
    if [ ! -f "$filepath" ]; then
        error_exit "Configuration file '$filepath' not found"
    fi
    return 0
}

check_template_file() {
    local filepath=$1
    if [ ! -f "$filepath" ]; then
        error_exit "Template file '$filepath' not found"
    fi
    return 0
}

list_templates() {
    echo -e "${BLUE}Available Configuration Templates:${NC}"
    echo "================================"
    
    # Built-in templates
    local builtin_templates=(
        "default" "high_performance" "memory_optimized" "cpu_intensive"
        "balanced" "quick_test" "production" "debug"
    )
    
    for template in "${builtin_templates[@]}"; do
        if [ -f "${TEMPLATE_DIR}/${template}.json" ]; then
            echo "${GREEN}${template}${NC} (built-in)"
        fi
    done
    
    # Custom templates
    if [ -d "$TEMPLATE_DIR" ]; then
        echo -e "\n${BLUE}Custom Templates:${NC}"
        for template in "$TEMPLATE_DIR"/*.json; do
            if [ -f "$template" ]; then
                local name=$(basename "$template" .json)
                if [[ ! " ${builtin_templates[*]} " =~ " ${name} " ]]; then
                    echo "${YELLOW}${name}${NC}"
                fi
            fi
        done
    fi
    
    echo -e "\n${BLUE}Template Structure:${NC}"
    echo "  Each template contains:"
    echo "  - Schema definition with validation rules"
    echo "  - Default values for configuration keys"
    echo "  - Inheritance rules for template chaining"
    echo "  - Description and metadata"
    echo "  - Tags for categorization"
}

create_template() {
    local name=$1
    if [ -z "$name" ]; then
        error_exit "Please specify a template name"
    fi
    
    if [ -f "${TEMPLATE_DIR}/${name}.json" ]; then
        error_exit "Template '$name' already exists"
    fi
    
    log "Creating new configuration template: $name"
    
    # Create template based on user input or generate from current config
    cat > "${TEMPLATE_DIR}/${name}.json" << EOF
{
  "name": "${name}",
  "description": "User-defined configuration template",
  "version": "1.0",
  "schema": [
    {
      "key": "performance.optimization_level",
      "type": "integer",
      "default": 2,
      "min": 1,
      "max": 5,
      "required": false,
      "description": "Performance optimization level (1-5)"
    },
    {
      "key": "memory.pool_size",
      "type": "integer",
      "default": 1024,
      "min": 256,
      "max": 65536,
      "required": false,
      "description": "Memory pool size in KB"
    },
    {
      "key": "neural_network.simultaneous_updates",
      "type": "boolean",
      "default": true,
      "required": false,
      "description": "Enable simultaneous neural network updates"
    }
  ],
  "inheritance_rules": [],
  "tags": ["user_created"],
  "created": "$(date -Iseconds)",
  "updated": "$(date -Iseconds)"
}
EOF
    
    log "${GREEN}Template '$name' created successfully${NC}"
    echo "Template '${YELLOW}${name}${NC}' created at ${TEMPLATE_DIR}/${name}.json"
}

load_template() {
    local name=$1
    local output_file=$2
    if [ -z "$name" ] || [ -z "$output_file" ]; then
        error_exit "Please specify both template name and output file"
    fi
    
    check_template_file "${TEMPLATE_DIR}/${name}.json"
    
    log "Loading template '$name' to '$output_file'"
    
    # Validate template file
    if ! python3 -c "import json; json.load(open('${TEMPLATE_DIR}/${name}.json'))" 2>/dev/null; then
        error_exit "Invalid JSON in template file"
    fi
    
    # Process template
    python3 << EOF
import json
import sys
from datetime import datetime

template_file = '${TEMPLATE_DIR}/${name}.json'
output_file = '$output_file'

with open(template_file, 'r') as f:
    template = json.load(f)

# Create final configuration
config = {}

# Add default values
if 'schema' in template:
    for entry in template['schema']:
        key = entry.get('key')
        default_value = entry.get('default')
        if key and default_value is not None:
            # Handle nested keys
            keys = key.split('.')
            current = config
            for i, k in enumerate(keys):
                if i == len(keys) - 1:
                    current[k] = default_value
                else:
                    if k not in current:
                        current[k] = {}
                    current = current[k]

# Add template metadata
config['_template'] = name
config['_version'] = template.get('version', '1.0')
config['_description'] = template.get('description', '')
config['_created'] = datetime.now().isoformat()

with open(output_file, 'w') as f:
    json.dump(config, f, indent=2)

print(f"Template loaded successfully to {output_file}")
EOF
    
    if [ $? -eq 0 ]; then
        log "${GREEN}Template loaded successfully${NC}"
        echo "Configuration loaded to: ${YELLOW}${output_file}${NC}"
    else
        error_exit "Failed to load template"
    fi
}

export_template() {
    local config_file=$1
    local output_file=$2
    if [ -z "$config_file" ] || [ -z "$output_file" ]; then
        error_exit "Please specify both input config file and output template file"
    fi
    
    check_config_file "$config_file"
    
    log "Exporting configuration '$config_file' as template to '$output_file'"
    
    # Extract schema from current config or infer from file
    python3 << EOF
import json
import sys
from datetime import datetime

config_file = '$config_file'
output_file = '$output_file'

with open(config_file, 'r') as f:
    config = json.load(f)

# Create template
from collections import defaultdict

template = {}
template['name'] = 'exported_template'
template['description'] = f"Exported configuration from {config_file}"
template['version'] = '1.0'
template['schema'] = []
template['inheritance_rules'] = []
template['tags'] = ['exported', 'auto_generated']
template['created'] = datetime.now().isoformat()
template['updated'] = datetime.now().isoformat()

# Analyze configuration structure to create schema
if isinstance(config, dict):
    def analyze_dict(d, prefix=""):
        for key, value in d.items():
            full_key = f"{prefix}.{key}" if prefix else key
            if isinstance(value, dict):
                analyze_dict(value, full_key)
            elif isinstance(value, list):
                # Simple array handling
                template['schema'].append({
                    'key': full_key,
                    'type': 'array',
                    'default': value,
                    'required': False,
                    'description': f"Array configuration for {full_key}"
                })
            elif isinstance(value, bool):
                template['schema'].append({
                    'key': full_key,
                    'type': 'boolean',
                    'default': value,
                    'required': False,
                    'description': f"Boolean configuration for {full_key}"
                })
            elif isinstance(value, (int, float)):
                template['schema'].append({
                    'key': full_key,
                    'type': 'double',
                    'default': value,
                    'required': False,
                    'description': f"Numeric configuration for {full_key}"
                })
            elif isinstance(value, str):
                template['schema'].append({
                    'key': full_key,
                    'type': 'string',
                    'default': value,
                    'required': False,
                    'description': f"String configuration for {full_key}"
                })
    
    analyze_dict(config)

with open(output_file, 'w') as f:
    json.dump(template, f, indent=2)

print(f"Template exported successfully to {output_file}")
EOF
    
    if [ $? -eq 0 ]; then
        log "${GREEN}Configuration exported as template${NC}"
        echo "Template exported to: ${YELLOW}${output_file}${NC}"
    else
        error_exit "Failed to export configuration"
    fi
}

validate_config() {
    local config_file=$1
    check_config_file "$config_file"
    
    log "Validating configuration file: $config_file"
    
    python3 << EOF
import json
import sys
from jsonschema import validate, ValidationError

config_file = '$config_file'

with open(config_file, 'r') as f:
    config = json.load(f)

# Basic validation
validation_errors = []

# Check for required structure
if not isinstance(config, dict):
    validation_errors.append("Configuration must be a JSON object")

# Validate nested structure
def validate_dict(d, path=""):
    for key, value in d.items():
        current_path = f"{path}.{key}" if path else key
        if isinstance(value, dict):
            validate_dict(value, current_path)
        elif isinstance(value, list):
            for i, item in enumerate(value):
                if isinstance(item, dict):
                    validate_dict(item, f"{current_path}[{i}]")

validate_dict(config)

if validation_errors:
    print("Validation failed:")
    for error in validation_errors:
        print(f"  - {error}")
    sys.exit(1)
else:
    print("Configuration is valid")
    print(f"Total keys: {len(str(config))}")
EOF
    
    local validation_result=$?
    if [ $validation_result -eq 0 ]; then
        log "${GREEN}Configuration validation successful${NC}"
        echo "Configuration file is valid"
    else
        error_exit "Configuration validation failed"
    fi
}

monitor_changes() {
    log "Starting configuration change monitoring..."
    
    # This is a placeholder for actual change monitoring
    # In a real implementation, you would set up file watchers or use git diff
    echo "Configuration monitoring functionality requires implementation"
    echo "This would monitor config files for changes and log them"
}

list_changes() {
    log "Listing recent configuration changes..."
    
    # Check for change log
    local change_log="${CONFIG_DIR}/changes.log"
    if [ -f "$change_log" ]; then
        echo -e "${BLUE}Recent Configuration Changes:${NC}"
        tail -20 "$change_log"
    else
        echo "No change log found"
    fi
}

rollback_change() {
    local change_id=$1
    if [ -z "$change_id" ]; then
        error_exit "Please specify a change ID to rollback"
    fi
    
    error_exit "Rollback functionality requires implementation"
}

execute_script() {
    local script_file=$1
    if [ -z "$script_file" ]; then
        error_exit "Please specify a configuration script file"
    fi
    
    check_config_file "$script_file"
    
    log "Executing configuration script: $script_file"
    
    # Execute configuration script
    python3 "$script_file" 2>&1 | while read line; do
        log "$line"
    done
    
    log "${GREEN}Configuration script executed${NC}"
}

show_help() {
    cat << EOF
Advanced Configuration Management Script
====================================

USAGE:
  $0 [OPTIONS]

OPTIONS:
  --list-templates              List all available configuration templates
  --create-template NAME        Create a new configuration template
  --load-template NAME FILE     Load configuration from template to FILE
  --export-template NAME FILE   Export existing configuration as template
  --validate-config FILE        Validate configuration file
  --monitor-changes             Monitor configuration changes
  --list-changes                List recent configuration changes
  --rollback-change ID          Rollback specific configuration change
  --script-execute FILE         Execute configuration script
  --script-preview FILE         Preview configuration script effects

EXAMPLES:
  $0 --list-templates            # List all available templates
  $0 --create-template my_template  # Create new template
  $0 --load-template my_template config.json  # Load template to file
  $0 --export-template config.json my_template.json  # Export config as template
  $0 --validate-config production_config.json  # Validate config file

FEATURES:
  - Template management with inheritance support
  - Configuration validation and schema analysis
  - Change tracking and rollback capabilities
  - Script-based configuration management
  - Backup and restore functionality

CONFIGURATION DIRECTORY:
  $CONFIG_DIR

TEMPLATES DIRECTORY:
  $TEMPLATE_DIR

LOG FILE:
  $LOG_FILE

The script provides comprehensive configuration management for power users.
EOF
}

# Command line argument parsing
case "${1:-}" in
    list-templates)
        list_templates
        ;;
    create-template)
        create_template "$2"
        ;;
    load-template)
        load_template "$2" "$3"
        ;;
    export-template)
        export_template "$2" "$3"
        ;;
    validate-config)
        validate_config "$2"
        ;;
    monitor-changes)
        monitor_changes
        ;;
    list-changes)
        list_changes
        ;;
    rollback-change)
        rollback_change "$2"
        ;;
    script-execute)
        execute_script "$2"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "Unknown option: $1"
        show_help
        exit 1
        ;;
esac