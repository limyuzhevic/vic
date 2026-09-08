#!/bin/bash

# Shell utilities for NLM command line interface

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

log_debug() {
    echo -e "${BLUE}[DEBUG]${NC} $*"
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Safe file operations
ensure_directory() {
    local dir="$1"
    if [ ! -d "$dir" ]; then
        mkdir -p "$dir"
        if [ $? -ne 0 ]; then
            log_error "Failed to create directory: $dir"
            return 1
        fi
        log_info "Created directory: $dir"
    fi
    return 0
}

# Print elapsed time
print_elapsed() {
    local start_time="$1"
    local end_time="$2"
    local elapsed=$((end_time - start_time))
    echo "Elapsed: ${elapsed}s"
}

# Validate positive integer
validate_positive_int() {
    local value="$1"
    if ! [[ "$value" =~ ^[0-9]+$ ]] || [ "$value" -le 0 ]; then
        log_error "Value must be a positive integer: $value"
        return 1
    fi
    return 0
}

# Validate float within range
validate_float_range() {
    local value="$1"
    local min="$2"
    local max="$3"
    local num_val=$(echo "$value" | bc -l 2>/dev/null || echo "0")
    
    if [ $? -ne 0 ]; then
        log_error "Invalid number: $value"
        return 1
    fi
    
    if [ $(echo "$num_val < $min" | bc -l 2>/dev/null || echo "0") -eq 1 ] || [ $(echo "$num_val > $max" | bc -l 2>/dev/null || echo "0") -eq 1 ]; then
        log_error "Value must be between $min and $max: $value"
        return 1
    fi
    return 0
}

# Generate unique filename
generate_unique_filename() {
    local prefix="$1"
    local extension="$2"
    local timestamp=$(date +%Y%m%d_%H%M%S)
    local random_suffix=$RANDOM
    echo "${prefix}_${timestamp}_${random_suffix}.${extension}"
}

# Check file size
check_file_size() {
    local filepath="$1"
    local max_size="$2"
    
    if [ ! -f "$filepath" ]; then
        log_error "File not found: $filepath"
        return 1
    fi
    
    local file_size=$(stat -c%s "$filepath" 2>/dev/null || stat -f%z "$filepath" 2>/dev/null)
    
    if [ $? -ne 0 ]; then
        log_warn "Could not determine file size for $filepath"
        return 0
    fi
    
    if [ $file_size -gt $max_size ]; then
        log_error "File too large: $filepath ($file_size bytes)"
        return 1
    fi
    
    return 0
}

# Backup file
backup_file() {
    local source="$1"
    local backup_dir="$2"
    
    if [ ! -f "$source" ]; then
        log_warn "Source file not found for backup: $source"
        return 0
    fi
    
    ensure_directory "$backup_dir" || return 1
    
    local timestamp=$(date +%Y%m%d_%H%M%S)
    local backup_file="$backup_dir/$(basename "$source").backup.$timestamp"
    
    cp "$source" "$backup_file"
    if [ $? -eq 0 ]; then
        log_info "Created backup: $backup_file"
    else
        log_error "Failed to create backup: $source -> $backup_file"
        return 1
    fi
    
    return 0
}

# Memory check (approximate)
check_memory_usage() {
    local threshold_mb="$1"
    local current_mb=$(free -m | awk 'NR==2{printf "%.0f", $3}')
    
    if [ $current_mb -gt $threshold_mb ]; then
        log_warn "High memory usage detected: ${current_mb}MB"
        return 1
    fi
    
    return 0
}

# Log file rotation
rotate_log_file() {
    local logfile="$1"
    local max_size_mb="$2"
    
    if [ ! -f "$logfile" ]; then
        return 0
    fi
    
    local file_size_kb=$(($(stat -c%s "$logfile" 2>/dev/null || stat -f%z "$logfile" 2>/dev/null) / 1024))
    local max_size_kb=$((max_size_mb * 1024))
    
    if [ $file_size_kb -gt $max_size_kb ]; then
        local rotated_file="$logfile.$(date +%Y%m%d_%H%M%S)"
        mv "$logfile" "$rotated_file"
        touch "$logfile"
        log_info "Rotated log file: $logfile -> $rotated_file"
    fi
}

# Execute command with timeout
execute_with_timeout() {
    local timeout_seconds="$1"
    shift
    local command_name="$1"
    shift
    local command_args="$@"
    
    if ! command_exists "timeout"; then
        log_warn "timeout command not available, running without timeout"
        $command_name "$command_args"
        return $?
    fi
    
    timeout "$timeout_seconds" $command_name "$command_args"
    local exit_code=$?
    
    if [ $exit_code -eq 124 ]; then
        log_error "Command timed out after ${timeout_seconds} seconds: $command_name $command_args"
        return 1
    elif [ $exit_code -ne 0 ]; then
        log_error "Command failed with exit code $exit_code: $command_name $command_args"
        return $exit_code
    fi
    
    return 0
}

# Convert bytes to human readable format
bytes_to_human() {
    local bytes="$1"
    local units=(B KB MB GB TB)
    local i=0
    local size="$bytes"
    
    while [ $(echo "$size >= 1024 && $i < ${#units[@]}" | bc -l 2>/dev/null || echo "0") -eq 1 ]; do
        size=$(echo "$size / 1024" | bc -l 2>/dev/null || echo "0")
        i=$((i + 1))
    done
    
    if [ ${#units[@]} -gt $i ]; then
        echo "${size} ${units[$i]}"
    else
        echo "${size} ${units[-1]}"
    fi
}