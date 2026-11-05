#!/usr/bin/env bash
# Configuration Management Tools and Utilities
# Advanced configuration management for NLM

# Configuration Management Script
# A comprehensive set of tools for managing NLM configuration files

cat > config_manager.sh << 'EOF'
#!/bin/bash

# Configuration Management Script for NLM
# Provides advanced configuration operations and utilities

CONFIG_DIR="${CONFIG_DIR:-configs}"
DEFAULT_CONFIG="${CONFIG_DIR}/default.cfg"
BACKUP_DIR="${CONFIG_DIR}/backups"

# Ensure backup directory exists
ensure_backup_dir() {
    mkdir -p "$BACKUP_DIR"
}

# Create backup of configuration file
backup_config() {
    local config_file="$1"
    if [[ -f "$config_file" ]]; then
        local timestamp=$(date +"%Y%m%d_%H%M%S")
        local backup_file="$BACKUP_DIR/config_${timestamp}.backup"
        cp "$config_file" "$backup_file"
        echo "Configuration backed up to: $backup_file"
        return 0
    fi
    echo "Error: Configuration file not found: $config_file"
    return 1
}

# Validate configuration file
validate_config() {
    local config_file="$1"
    local format="${2:-auto}"
    
    echo "Validating configuration file: $config_file"
    
    if [[ ! -f "$config_file" ]]; then
        echo "Error: Configuration file not found: $config_file"
        return 1
    fi
    
    # Use Config class for validation if available
    if command -v nlm_config_validator &> /dev/null; then
        nlm_config_validator "$config_file"
    else
        # Simple validation
        if grep -q '^[^#[:space:]].*=' "$config_file" 2>/dev/null; then
            echo "Configuration file appears to be in valid format"
            return 0
        else
            echo "Error: Configuration file format appears invalid"
            return 1
        fi
    fi
}

# Convert configuration between formats
convert_config() {
    local input_file="$1"
    local output_format="$2"
    local output_file="${3:-}"
    
    if [[ -z "$output_file" ]]; then
        output_file="${input_file%.*}.$output_format"
    fi
    
    echo "Converting $input_file to $output_format format..."
    
    case "$output_format" in
        json|yaml)
            if [[ ! -f "$input_file" ]]; then
                echo "Error: Input file not found: $input_file"
                return 1
            fi
            
            # Use Config class to convert formats
            if command -v nlm_config_tool &> /dev/null; then
                nlm_config_tool convert "$input_file" "$output_format" "$output_file"
            else
                echo "Warning: Conversion tool not available. Manual conversion required."
            fi
            ;;
        keyvalue)
            echo "Converting to key=value format..."
            # Create key=value format from JSON/YAML
            if [[ "$input_file" == *.json ]] || [[ "$input_file" == *.yaml ]] || [[ "$input_file" == *.yml ]]; then
                if command -v jq &> /dev/null && [[ "$input_file" == *.json ]]; then
                    jq -r 'to_entries | .[] | "\(.key) = \(.value | tostring)"' "$input_file" > "$output_file"
                elif command -v yq &> /dev/null && ([[ "$input_file" == *.yaml ]] || [[ "$input_file" == *.yml ]]); then
                    yq -r '. as $item | keys[] as $key | "\($key) = \($item.\$key)"' "$input_file" > "$output_file"
                else
                    echo "Error: jq or yq required for JSON/YAML to key=value conversion"
                    return 1
                fi
            fi
            ;;
        *)
            echo "Error: Unsupported output format: $output_format"
            return 1
            ;;
    esac
}

# Merge multiple configuration files
merge_configs() {
    local output_file="$1"
    shift
    
    local temp_config="$(mktemp)"
    
    # Start with empty config
    > "$temp_config"
    
    for config_file in "$@"; do
        if [[ -f "$config_file" ]]; then
            echo "Merging: $config_file"
            
            # Simple merge - later files override earlier ones
            while IFS= read -r line; do
                # Skip comments and empty lines
                [[ "$line" =~ ^[[:space:]]*# ]] && continue
                [[ -z "$line" ]] && continue
                
                echo "$line" >> "$temp_config"
            done < "$config_file"
        else
            echo "Warning: Configuration file not found: $config_file"
        fi
    done
    
    # Use Config class for proper merging
    if command -v nlm_config_tool &> /dev/null; then
        nlm_config_tool merge "$temp_config" "$output_file"
    else
        mv "$temp_config" "$output_file"
    fi
    
    rm -f "$temp_config"
}

# Generate configuration statistics
generate_stats() {
    local config_file="$1"
    
    echo "Generating configuration statistics for: $config_file"
    
    if [[ ! -f "$config_file" ]]; then
        echo "Error: Configuration file not found: $config_file"
        return 1
    fi
    
    if command -v nlm_config_tool &> /dev/null; then
        nlm_config_tool stats "$config_file"
    else
        # Simple statistics
        local total_lines=$(wc -l < "$config_file")
        local non_comment_lines=$(grep -v '^#' "$config_file" | grep -v '^$' | wc -l)
        local key_value_lines=$(grep -E '^[[:space:]]*[^#[:space:]].*=' "$config_file" | wc -l)
        
        echo "Configuration Statistics:"
        echo "  Total lines: $total_lines"
        echo "  Non-comment lines: $non_comment_lines"
        echo "  Key=value pairs: $key_value_lines"
        
        # Extract sample keys
        echo "  Sample keys:"
        grep -E '^[[:space:]]*([^#[:space:]]+).*=' "$config_file" | head -5 | while IFS= read -r line; do
            local key=$(echo "$line" | cut -d'=' -f1 | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            echo "    - $key"
        done
    fi
}

# Apply configuration changes
apply_changes() {
    local config_file="$1"
    local changes_file="$2"
    
    echo "Applying changes from: $changes_file to $config_file"
    
    backup_config "$config_file"
    
    if command -v nlm_config_tool &> /dev/null; then
        nlm_config_tool apply "$config_file" "$changes_file"
    else
        # Simple sed-based application
        while IFS= read -r change; do
            # Parse change (format: key=value)
            if [[ "$change" =~ ^([^=]+)=(.*)$ ]]; then
                local key="${BASH_REMATCH[1]}"
                local value="${BASH_REMATCH[2]}"
                sed -i "s/^\(${key}\s*=\s*\).*/\1${value}/" "$config_file"
            fi
        done < "$changes_file"
    fi
}

# Show configuration diff
show_diff() {
    local config1="$1"
    local config2="$2"
    
    echo "Comparing configurations: $config1 vs $config2"
    
    if [[ ! -f "$config1" || ! -f "$config2" ]]; then
        echo "Error: One or both configuration files not found"
        return 1
    fi
    
    if command -v diff &> /dev/null; then
        diff -u "$config1" "$config2" | head -50
    else
        echo "diff command not available"
    fi
}

# Export configuration to different formats
export_config() {
    local config_file="$1"
    local output_format="$2"
    local output_file="${3:-}"
    
    if [[ -z "$output_file" ]]; then
        output_file="${config_file%.*}.$output_format"
    fi
    
    echo "Exporting $config_file to $output_format format..."
    
    convert_config "$config_file" "$output_format" "$output_file"
}

# Show usage
usage() {
    cat << EOF
Configuration Management Utilities

Usage: config_manager.sh <command> [options]

Commands:
    backup <config_file>           Create backup of configuration file
    validate <config_file> [format]  Validate configuration file
    convert <input> <format> [output] Convert between configuration formats
    merge <output> <config1> [config2...]  Merge multiple configuration files
    stats <config_file>            Generate configuration statistics
    apply <config_file> <changes>   Apply configuration changes
    diff <config1> <config2>        Show configuration differences
    export <config_file> <format> [output] Export configuration to another format
    help                           Show this help message

Formats: json, yaml, keyvalue, auto

Examples:
    config_manager.sh backup configs/default.cfg
    config_manager.sh validate configs/default.cfg json
    config_manager.sh convert configs/default.cfg yaml configs/default.yaml
    config_manager.sh merge configs/merged.cfg configs/base.cfg configs/override.cfg
    config_manager.sh stats configs/default.cfg
    config_manager.sh export configs/default.cfg json
EOF
}

# Main command handling
case "$1" in
    backup)
        ensure_backup_dir
        backup_config "$2"
        ;;
    validate)
        validate_config "$2" "$3"
        ;;
    convert)
        convert_config "$2" "$3" "$4"
        ;;
    merge)
        shift
        merge_configs "$1" "$@"
        ;;
    stats)
        generate_stats "$2"
        ;;
    apply)
        apply_changes "$2" "$3"
        ;;
    diff)
        show_diff "$2" "$3"
        ;;
    export)
        export_config "$2" "$3" "$4"
        ;;
    help|--help|-h)
        usage
        ;;
    *)
        echo "Error: Unknown command: $1"
        usage
        exit 1
        ;;
esac

EOF

chmod +x config_manager.sh
