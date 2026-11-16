#!/bin/bash
# NLM Command Line Interface Utilities
# Advanced CLI tools for NLM (Neural Learning Machine)

# Set default environment variables
export NLM_VERBOSE=${NLM_VERBOSE:-false}
export NLM_CONFIG=${NLM_CONFIG:-configs/default.cfg}
export NLM_PROFILE_DIR=${NLM_PROFILE_DIR:-./profiles}

# This file contains shell functions to enhance the NLM command-line experience

# NLM Phase Selector - Unified interface for running different phases
nlm_phase() {
    local phase="$1"
    local config_file="${2:-configs/default.cfg}"
    local verbose="${3:-false}"
    local headless="${4:-false}"
    
    case "$phase" in
        "2"|"phase2"|"neural")
            echo "Running NLM Phase 2: Real Neural Computation"
            ./nlm --config "$config_file" --verbose "$verbose" --headless "$headless"
            ;;
        "3"|"phase3"|"environment")
            echo "Running NLM Phase 3: Environment Interaction"
            ./nlm_phase3_demo --config "$config_file" --verbose "$verbose" --headless "$headless"
            ;;
        "4"|"phase4"|"cognitive")
            echo "Running NLM Phase 4: Cognitive Behaviors"
            ./nlm_phase4_demo --config "$config_file" --verbose "$verbose" --headless "$headless"
            ;;
        "5"|"phase5"|"performance")
            echo "Running NLM Phase 5: Performance & Optimization"
            ./nlm_phase5_demo --config "$config_file" --verbose "$verbose" --headless "$headless"
            ;;
        "6"|"phase6"|"integration")
            echo "Running NLM Phase 6: Integration Test"
            ./nlm_phase6_demo --config "$config_file" --verbose "$verbose" --headless "$headless"
            ;;
        "benchmark"|"bench")
            echo "Running NLM Performance Benchmark"
            ./nlm_benchmark --config "$config_file" --verbose "$verbose" --headless "$headless"
            ;;
        *)
            echo "Unknown phase: $phase"
            echo "Available phases: 2,3,4,5,6,benchmark, help"
            exit 1
            ;;
    esac
}

# NLM Configuration Manager
nlm_config() {
    local action="$1"
    local param="$2"
    local value="$3"
    local config_file="${4:-configs/default.cfg}"
    
    case "$action" in
        "list"|"show")
            echo "=== NLM Configuration ==="
            if [ -f "$config_file" ]; then
                cat "$config_file"
            else
                echo "Config file not found: $config_file"
                echo "Using default configuration"
                cat configs/default.cfg
            fi
            ;;
        "get"|"query")
            local value=$(grep "^$param = " "$config_file" 2>/dev/null | head -1 | cut -d'=' -f2 | sed 's/^ *//')
            if [ -n "$value" ]; then
                echo "$param = $value"
            else
                echo "Parameter not found: $param"
                exit 1
            fi
            ;;
        "set"|"modify")
            if [ -z "$value" ]; then
                echo "Error: Value required for parameter '$param'"
                echo "Usage: nlm-config set <parameter> <value> [config_file]"
                exit 1
            fi
            if [ ! -f "$config_file" ]; then
                echo "Creating new config file: $config_file"
                cp configs/default.cfg "$config_file"
            fi
            # Update or add parameter
            if grep -q "^$param = " "$config_file"; then
                sed -i "s/^$param = .*/$param = $value/" "$config_file"
                echo "Updated: $param = $value"
            else
                echo "$param = $value" >> "$config_file"
                echo "Added: $param = $value"
            fi
            ;;
        "validate"|"check")
            echo "=== Validating Configuration ==="
            if [ ! -f "$config_file" ]; then
                echo "Config file not found: $config_file"
                exit 1
            fi
            local errors=0
            while IFS= read -r line; do
                if [[ "$line" =~ ^[[:space:]]*[a-zA-Z0-9_.]+ = .*$ ]]; then
                    continue
                elif [[ -n "$line" && ! "$line" =~ ^[[:space:]]*# ]]; then
                    echo "Invalid line in config: $line"
                    ((errors++))
                fi
            done < "$config_file"
            
            # Check for duplicate keys
            echo "Checking for duplicate keys..."
            while IFS= read -r line; do
                if [[ "$line" =~ ^[[:space:]]*([a-zA-Z0-9_.]+)[[:space:]]*=[[:space:]]*.*$ ]]; then
                    key=${BASH_REMATCH[1]}
                    count=$(grep -c "^$key = " "$config_file")
                    if [ "$count" -gt 1 ]; then
                        echo "WARNING: Duplicate key '$key' found ($count times)"
                    fi
                fi
            done < "$config_file"
            
            if [ "$errors" -eq 0 ]; then
                echo "Configuration validation passed!"
            else
                echo "$errors configuration errors found"
                exit 1
            fi
            ;;
        *)
            echo "Unknown config action: $action"
            echo "Available actions: list, show, get, query, set, modify, validate, check"
            exit 1
            ;;
    esac
}

# NLM Brain Exporter/Importer
nlm_export() {
    local output_file="$1"
    local format="${2:-binary}"
    local config_file="${3:-configs/default.cfg}"
    
    echo "Exporting NLM brain to $output_file ($format format)"
    echo "This feature requires the actual NLM C++ library to be built"
    echo "Please compile with checkpoint system enabled for export functionality"
    
    # Placeholder for actual export implementation
    echo "Export functionality to be implemented in future version"
}

nlm_import() {
    local input_file="$1"
    local config_file="${2:-configs/default.cfg}"
    local output_dir="${3:-.}"
    
    echo "Importing NLM brain from $input_file"
    echo "This feature requires the actual NLM C++ library to be built"
    
    # Placeholder for actual import implementation
    echo "Import functionality to be implemented in future version"
}

# NLM Performance Profiler
nlm_profile() {
    local duration="${1:-60}"
    local output_file="${2:-profile_results.csv}"
    local phase="${3:-6}"
    local config_file="${4:-configs/default.cfg}"
    
    echo "Starting NLM performance profiling for $duration seconds (Phase $phase)"
    echo "Results will be saved to $output_file"
    
    # Start monitoring
    ./nlm_benchmark --profile "$duration" --config "$config_file" --output "$output_file" --phase "$phase"
}

# NLM Debug Console
nlm_debug() {
    local brain_state="$1"
    local analysis_type="${2:-basic}"
    
    echo "NLM Debug Console"
    echo "Brain state file: $brain_state"
    echo "Analysis type: $analysis_type"
    
    echo "Debug functionality requires NLM debug tools"
    echo "Debug features to be implemented in future version"
}

# NLM Help and Documentation
nlm_help() {
    cat << 'EOF'
=== NLM Command Line Interface (CLI) ===

=== Basic Usage ===

nlm_phase [phase] [config_file] [verbose] [headless]
  Run a specific NLM phase (2, 3, 4, 5, 6, benchmark)

nlm_config [action] [parameter] [value] [config_file]
  Manage NLM configuration files

nlm_export <output_file> [format] [config_file]
  Export brain state to file

nlm_import <input_file> [config_file] [output_dir]
  Import brain state from file

nlm_profile [duration] [output_file] [phase] [config_file]
  Profile NLM performance

nlm_debug [brain_state] [analysis_type]
  Debug NLM brain state

=== Available Phases ===

2 or phase2 or neural     - Real neural computation (Phase 2)
3 or phase3 or environment - Environment interaction (Phase 3)
4 or phase4 or cognitive   - Cognitive behaviors (Phase 4)
5 or phase5 or performance - Performance and optimization (Phase 5)
6 or phase6 or integration - Integration test (Phase 6)
benchmark or bench         - Performance benchmarking

=== Configuration Actions ===

list or show               - List configuration
 get or query <parameter>  - Get parameter value
  set or modify <param> <val> - Set parameter value
 validate or check         - Validate configuration

=== Formats ===

binary                     - Binary brain state
json                       - JSON format
xml                        - XML format
graphics                   - Visualization data

=== Analysis Types ===

basic                      - Basic brain analysis
spikes                      - Spike pattern analysis
plasticity                  - Plasticity analysis
memory                      - Memory analysis
performance                  - Performance analysis
integration                  - Integration analysis

=== Examples ===

# Run Phase 6 integration test with custom config
nlm_phase 6 my_config.cfg true false

# List current configuration
nlm_config list

# Get specific parameter
nlm_config get brain.neuron_count

# Set parameter in config
nlm_config set brain.neuron_count 2000 my_config.cfg

# Validate configuration
nlm_config validate my_config.cfg

# Profile performance for 2 minutes
nlm_profile 120 profile_results.csv 6

=== Advanced Options ===

# Environment variables
export NLM_VERBOSE=true      # Enable verbose output
export NLM_CONFIG=configs/my_config.cfg  # Default config file
export NLM_PROFILE_DIR=./profiles  # Profile output directory

# Interactive mode (if supported by your terminal)
nlm_interactive

=== Getting Started ===

1. Build NLM with CLI support:
   mkdir build && cd build
   cmake .. -DNLM_BUILD_CLI=ON
   make -j$(nproc)

2. Run your first experiment:
   nlm_phase 6

3. Explore configuration options:
   nlm_config list

=== Need Help? ===

- Check the NLM documentation in docs/
- Run 'nlm_help' for this help text
- Check examples in examples/
- Visit the NLM GitHub repository

=== Version ===

NLM CLI v2.0.0 (Advanced Edition)

EOF
}

# Interactive mode
nlm_interactive() {
    echo "=== NLM Interactive Mode ==="
    echo "Welcome to the NLM Command Line Interface"
    echo "Type 'help' for commands, 'exit' to quit"
    
    while true; do
        echo -n ">>> "
        read -r command
        
        case "$command" in
            "exit"|"quit")
                echo "Goodbye!"
                exit 0
                ;;
            "help"|"?")
                nlm_help
                ;;
            "phase"*)
                # Parse phase command
                args=($command)
                nlm_phase "${args[1]}" "${args[2]}" "${args[3]}" "${args[4]}"
                ;;
            "config"*)
                args=($command)
                nlm_config "${args[1]}" "${args[2]}" "${args[3]}" "${args[4]}"
                ;;
            "profile"*)
                args=($command)
                nlm_profile "${args[1]}" "${args[2]}" "${args[3]}" "${args[4]}"
                ;;
            "debug"*)
                args=($command)
                nlm_debug "${args[1]}" "${args[2]}"
                ;;
            "export"*)
                args=($command)
                nlm_export "${args[1]}" "${args[2]}" "${args[3]}"
                ;;
            "import"*)
                args=($command)
                nlm_import "${args[1]}" "${args[2]}" "${args[3]}"
                ;;
            "*"*)
                echo "Unknown command: $command"
                echo "Type 'help' for available commands"
                ;;
        esac
    done
}

# Auto-complete (simplified)
_nlm_complete() {
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    
    case "${prev}" in
        "nlm_phase")
            COMPREPLY=( $(compgen -W "2 3 4 5 6 benchmark" -- "$cur") )
            ;;
        "nlm_config")
            if [ "${COMP_WORDS[1]}" = "list" ] || [ "${COMP_WORDS[1]}" = "show" ]; then
                COMPREPLY=()
            elif [ "${COMP_WORDS[1]}" = "get" ] || [ "${COMP_WORDS[1]}" = "query" ]; then
                COMPREPLY=( $(compgen -W "brain.neuron_count brain.synapse_density brain.connection_probability brain.v_thresh brain.v_rest brain.tau_mem brain.plasticity.stdp.enable" -- "$cur") )
            elif [ "${COMP_WORDS[1]}" = "set" ] || [ "${COMP_WORDS[1]}" = "modify" ]; then
                COMPREPLY=( $(compgen -W "brain.neuron_count brain.synapse_density brain.connection_probability brain.v_thresh brain.v_rest brain.tau_mem" -- "$cur") )
            fi
            ;;
        *)
            COMPREPLY=( $(compgen -W "phase config profile debug export import help exit interactive" -- "$cur") )
            ;;
    esac
}

# Initialize command
nlm_init() {
    echo "=== NLM CLI Initialization ==="
    
    # Create config directory if it doesn't exist
    if [ ! -d "configs" ]; then
        mkdir -p configs
        cp configs/default.cfg configs/default.cfg.backup 2>/dev/null || true
        echo "Created configs directory"
    fi
    
    # Create profiles directory if it doesn't exist
    if [ ! -d "profiles" ]; then
        mkdir -p profiles
        echo "Created profiles directory"
    fi
    
    # Create examples directory if it doesn't exist
    if [ ! -d "examples" ]; then
        mkdir -p examples
        echo "Created examples directory"
    fi
    
    # Create documentation
    if [ ! -f "NLM_CLI.md" ]; then
        nlm_help > NLM_CLI.md
        echo "Created NLM_CLI.md documentation"
    fi
    
    echo "NLM CLI initialization complete!"
    echo "Run 'nlm_help' for usage information"
}

# Main entry point
if [ "$1" = "--interactive" ] || [ "$1" = "-i" ]; then
    nlm_interactive
elif [ "$1" = "--help" ] || [ "$1" = "-h" ] || [ "$1" = "help" ]; then
    nlm_help
elif [ "$1" = "--init" ] || [ "$1" = "init" ]; then
    nlm_init
else
    echo "NLM Command Line Interface"
    echo "Run 'nlm_help' for usage information or 'nlm_interactive' for interactive mode"
    echo "Or use 'nlm_phase <phase> [options]' to run a specific phase"
fi