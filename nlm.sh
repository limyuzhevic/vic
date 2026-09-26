#!/usr/bin/env bash
# NLM CLI Tool Launcher
# 
# This script provides a unified interface to NLM (Neural Learning Machine) tools
# and utilities for advanced brain simulation, analysis, and management.

# Set default values
NLM_HOME="${NLM_HOME:-$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")}"
NLM_BIN_DIR="$NLM_HOME/bin"
NLM_ETC_DIR="$NLM_HOME/etc"
NLM_LIB_DIR="$NLM_HOME/lib"

# Add NLM bin directory to PATH
export PATH="$NLM_BIN_DIR:$PATH"

# Function to display NLM banner
function show_banner() {
    cat << EOF
╔════════════════════════════════════════════════════════════╗
║                                                            ║
║      NLM — 熙然 (Advanced Version)                          ║
║      Neural Learning Machine                               ║
║                                                            ║
║      Advanced Brain Simulation & Analysis Tools              ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝

Available commands:
  nlm                    - Main NLM command (shows this help)
  nlm-cli                - Advanced NLM command-line interface
  nlm-config             - Configuration management
  nlm-analyze            - Analysis tools
  nlm-visualize          - Visualization tools
  nlm-debug              - Debugging tools
  nlm-performance        - Performance tools

For detailed help, run:
  nlm-cli --help
  nlm-config --help
  nlm-analyze --help
EOF
}

# Function to check if NLM is properly installed
function check_nlm_installation() {
    if [[ ! -f "$NLM_BIN_DIR/nlm-cli" ]]; then
        echo "ERROR: NLM CLI not found at $NLM_BIN_DIR/nlm-cli"
        echo "Please install NLM first or set NLM_HOME environment variable."
        return 1
    fi
    if [[ ! -f "$NLM_BIN_DIR/nlm-config" ]]; then
        echo "ERROR: NLM config tool not found at $NLM_BIN_DIR/nlm-config"
        echo "Please install NLM first or set NLM_HOME environment variable."
        return 1
    fi
    return 0
}

# Function to show general help
function show_general_help() {
    cat << EOF
NLM (Neural Learning Machine) - Advanced Version

Usage: nlm [command] [options]

Available commands:

1. nlm status           Show current system status and configuration
2. nlm run <config>     Run simulation with configuration file
3. nlm analyze <file>   Analyze brain state or simulation results
4. nlm help [command]   Show help for specific command
5. nlm version          Show version information
6. nlm install <tool>   Install additional tools
7. nlm update           Update NLM to latest version
8. nlm config <file>    Load configuration from file

For more detailed help, use the advanced CLI:
  nlm-cli --help

For configuration management:
  nlm-config --help

For analysis tools:
  nlm-analyze --help

For visualization tools:
  nlm-visualize --help

For debugging tools:
  nlm-debug --help

For performance tools:
  nlm-performance --help

Configuration files:
  configs/default.cfg   - Default configuration
  configs/advanced.cfg  - Advanced configuration
  configs/performance.cfg - Performance configuration
  configs/research.cfg  - Research configuration

Brain state files:
  brain_states/*.json     - JSON format brain states
  brain_states/*.bin      - Binary format brain states
  brain_states/*.gz       - Compressed brain states

Documentation:
  $NLM_HOME/docs/README.md          - User documentation
  $NLM_HOME/docs/HOW_TO_USE.md       - Usage instructions
  $NLM_HOME/docs/ADVANCED_TOOLS.md   - Advanced tool documentation
  $NLM_HOME/docs/EXPERIMENTS.md      - Experiment documentation
EOF
}

# Main command dispatcher
function nlm() {
    local command="${1:-help}"
    shift || true
    
    case "$command" in
        "help")
            show_general_help
            ;;
        "status")
            if check_nlm_installation; then
                "$NLM_BIN_DIR/nlm-status"
            fi
            ;;
        "run")
            local config="${1:-configs/default.cfg}"
            if check_nlm_installation; then
                "$NLM_BIN_DIR/nlm-run" "$config"
            fi
            ;;
        "analyze")
            local file="${1:-}"
            if [[ -z "$file" ]]; then
                echo "ERROR: Please specify a file to analyze"
                echo "Usage: nlm analyze <file>"
                return 1
            fi
            if check_nlm_installation; then
                "$NLM_BIN_DIR/nlm-analyze" "$file"
            fi
            ;;
        "version")
            if check_nlm_installation; then
                "$NLM_BIN_DIR/nlm-version"
            fi
            ;;
        "install")
            local tool="${1:-}"
            if [[ -z "$tool" ]]; then
                echo "ERROR: Please specify a tool to install"
                echo "Usage: nlm install <tool_name>"
                return 1
            fi
            if check_nlm_installation; then
                "$NLM_BIN_DIR/nlm-install" "$tool"
            fi
            ;;
        "update")
            if check_nlm_installation; then
                echo "Checking for updates..."
                "$NLM_BIN_DIR/nlm-update"
            fi
            ;;
        "config")
            local config_file="${1:-}"
            if [[ -z "$config_file" ]]; then
                echo "ERROR: Please specify a configuration file"
                echo "Usage: nlm config <config_file>"
                return 1
            fi
            if check_nlm_installation; then
                "$NLM_BIN_DIR/nlm-config-loader" "$config_file"
            fi
            ;;
        "cli")
            if check_nlm_installation; then
                "$NLM_BIN_DIR/nlm-cli" "$@"
            fi
            ;;
        "*")
            echo "Unknown command: $command"
            echo "Run 'nlm help' for available commands"
            return 1
            ;;
    esac
}

# Check if we're running the script directly (not being sourced)
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    # Script is being executed directly
    if [[ $# -eq 0 ]]; then
        show_banner
    else
        nlm "$@"
    fi
fi
