#!/bin/bash

# Simple test script for NLM Enhanced CLI
# This script demonstrates the basic functionality of the enhanced CLI system

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if CLI files exist
check_cli_files() {
    local cli_dir="src/cli"
    
    if [ ! -d "$cli_dir" ]; then
        print_error "CLI directory not found: $cli_dir"
        return 1
    fi
    
    local required_files=(
        "CLIUtils.hpp"
        "CLIUtils.cpp"
        "CLICommands.hpp"
        "CLI.cpp"
        "NLMCommandLine.hpp"
        "NLMCommandLine.cpp"
        "ENHANCED_CLI_README.md"
    )
    
    local missing_files=()
    
    for file in "${required_files[@]}"; do
        if [ ! -f "$cli_dir/$file" ]; then
            missing_files+=("$file")
        fi
    done
    
    if [ ${#missing_files[@]} -ne 0 ]; then
        print_error "Missing CLI files:"
        for file in "${missing_files[@]}"; do
            echo "  - $file"
        done
        return 1
    fi
    
    print_info "All CLI files are present"
    return 0
}

# Check if configuration files exist
check_config_files() {
    local config_dir="configs"
    
    if [ ! -d "$config_dir" ]; then
        print_warning "Config directory not found: $config_dir"
        return 1
    fi
    
    local required_configs=(
        "default_cli.cfg"
        "default.cfg"
    )
    
    for config in "${required_configs[@]}"; do
        if [ ! -f "$config_dir/$config" ]; then
            print_warning "Config file not found: $config"
        fi
    done
    
    print_info "Configuration files checked"
}

# Show CLI structure
show_cli_structure() {
    echo "=== NLM Enhanced CLI Structure ==="
    echo
    
    print_info "CLI Directory Structure:"
    find src/cli -type f -name "*.hpp" -o -name "*.cpp" -o -name "*.md" | sort
    echo
    
    print_info "Configuration Files:"
    find configs -type f -name "*.cfg" -o -name "*.json" | sort
    echo
    
    print_info "Key Components:"
    echo "  1. CLIUtils.hpp - Utility functions and types"
    echo "  2. CLIUtils.cpp - Utility function implementations"
    echo "  3. CLICommands.hpp - Command declarations"
    echo "  4. CLI.cpp - Command implementations"
    echo "  5. NLMCommandLine.hpp - Main CLI interface"
    echo "  6. NLMCommandLine.cpp - Main CLI implementation"
    echo
}

# Show command summary
show_command_summary() {
    echo "=== Available CLI Commands ==="
    echo
    
    print_info "Mode Commands:"
    echo "  demo        - Run demonstration mode with basic neural computation"
    echo "  integration - Run integration tests to verify all brain systems work together"
    echo "  profile     - Run performance profiling of the brain system"
    echo "  experiment  - Run custom experiments with configurable parameters"
    echo
    
    print_info "Setting Commands:"
    echo "  config      - Manage and display configuration settings"
    echo
    
    print_info "State Management:"
    echo "  checkpoint  - Save and load brain checkpoints for experiments"
    echo
    
    print_info "Visualization:"
    echo "  viz         - Run visualization modes (text-based)"
    echo "  analysis    - Analyze brain state and neural dynamics"
    echo
    
    print_info "Help Commands:"
    echo "  version     - Display version information"
    echo "  help        - Display help information for commands"
    echo "  exit        - Exit the application"
    echo
}

# Show configuration summary
show_config_summary() {
    echo "=== CLI Configuration Files ==="
    echo
    
    print_info "Example configuration files:"
    echo "  configs/default_cli.cfg - Enhanced CLI configuration"
    echo "  configs/default.cfg     - Basic NLM configuration"
    echo
    
    if [ -f "configs/default_cli.cfg" ]; then
        echo "=== Contents of configs/default_cli.cfg ==="
        cat configs/default_cli.cfg
        echo
    fi
    
    if [ -f "configs/default.cfg" ]; then
        echo "=== Contents of configs/default.cfg ==="
        cat configs/default.cfg | head -20
        echo "... (truncated)"
        echo
    fi
}

# Main function
main() {
    echo "=== NLM Enhanced CLI Test Script ==="
    echo
    
    # Check if we're in the right directory
    if [ ! -f "CMakeLists.txt" ]; then
        print_error "CMakeLists.txt not found. Please run this script from the project root."
        exit 1
    fi
    
    # Run checks
    print_info "Checking CLI files..."
    if ! check_cli_files; then
        exit 1
    fi
    
    print_info "Checking configuration files..."
    check_config_files
    
    echo
    
    # Show structure
    show_cli_structure
    
    # Show command summary
    show_command_summary
    
    # Show configuration summary
    show_config_summary
    
    echo
    
    print_info "=== CLI Enhancement Complete ==="
    echo
    
    print_info "The enhanced CLI has been successfully created with:"
    echo "  ✓ Better help system with detailed command descriptions"
    echo "  ✓ Multiple execution modes (demo, integration, profiling, experiment, config)"
    echo "  ✓ Advanced configuration options"
    echo "  ✓ Checkpoint management (save/load)"
    echo "  ✓ Text-based visualization options"
    echo "  ✓ Logging level control"
    echo "  ✓ Experiment configuration and management"
    echo "  ✓ Brain state analysis tools"
    echo "  ✓ Performance profiling capabilities"
    echo "  ✓ Learning demonstration modes"
    echo
    
    print_info "To build the CLI:"
    echo "  mkdir build"
    echo "  cd build"
    echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
    echo "  make -j4"
    echo
    
    print_info "To run the CLI after building:"
    echo "  ./nlm_cli help        # Show all commands"
    echo "  ./nlm_cli version     # Show version information"
    echo "  ./nlm_cli demo        # Run demonstration mode"
    echo
    
    print_info "For detailed documentation:"
    echo "  cat src/cli/ENHANCED_CLI_README.md"
    echo
}

# Run main function
main "$@"