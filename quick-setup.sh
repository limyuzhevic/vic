#!/bin/bash

# Quick Setup Script for NLM Configuration Tools
# This script provides a quick setup for the new NLM configuration system

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_color() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to print error and exit
error_exit() {
    print_color $RED "Error: $1"
    exit 1
}

# Main setup function
main() {
    print_color $GREEN "NLM Configuration Tools Quick Setup"
    print_color $GREEN "=================================="

    # Check if we're in the right directory
    if [ ! -f "/workspace/app/CMakeLists.txt" ]; then
        error_exit "CMakeLists.txt not found. Please run this script from the NLM project root."
    fi

    # Check if nlm-config exists
    if [ ! -f "/workspace/app/nlm-config" ]; then
        print_color $YELLOW "Warning: nlm-config not found. Please ensure it exists in the project root."
        print_color $YELLOW "The nlm-config script should be in the project root."
    fi

    # Make nlm-config executable
    if [ -f "/workspace/app/nlm-config" ]; then
        chmod +x /workspace/app/nlm-config
        print_color $GREEN "Made nlm-config executable"
    fi

    # Check if build directory exists
    if [ ! -d "/workspace/app/build" ]; then
        print_color $YELLOW "Warning: Build directory not found. The NLM application needs to be built first."
        print_color $YELLOW "Run: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
        print_color $YELLOW "After building, the nlm-config tool will be available."
    fi

    # Create configuration directory
    mkdir -p ~/.nlm-config/presets
    mkdir -p ~/.nlm-config/backups

    print_color $GREEN "\nSetup completed!"
    print_color $GREEN "\nNext Steps:"
    print_color $YELLOW "1. Build the NLM application (if not already built)"
    print_color $YELLOW "2. Add /workspace/app to your PATH (or copy nlm-config to /usr/local/bin)"
    print_color $YELLOW "3. Create your configuration files"
    print_color $YELLOW "4. Test your configuration with nlm-config"
    print_color $YELLOW "5. Run your application: ./nlm --config=your_config.cfg"

    print_color $GREEN "\nConfiguration Examples:"
    print_color $YELLOW "- Basic: /workspace/app/examples/config/simple_example.cfg"
    print_color $YELLOW "- JSON: /workspace/app/examples/config/json_example.json"
    print_color $YELLOW "- YAML: /workspace/app/examples/config/yaml_example.yaml"
    print_color $YELLOW "- Presets: ~/.nlm-config/presets/ (development.cfg, testing.cfg, production.cfg)"

    print_color $GREEN "\nConfiguration Commands:"
    print_color $YELLOW "- Validate: nlm-config validate your_config.cfg"
    print_color $YELLOW "- Compare: nlm-config compare config1.cfg config2.cfg"
    print_color $YELLOW "- Merge: nlm-config merge config1.cfg config2.cfg merged.cfg"
    print_color $YELLOW "- Export: nlm-config export my_config.json my_config.json"
    print_color $YELLOW "- Import: nlm-config import settings.json my_config.cfg"
    print_color $YELLOW "- List presets: nlm-config list"
    print_color $YELLOW "- Create preset: nlm-config create my_preset"
    print_color $YELLOW "- Show info: nlm-config info my_config.cfg"
    print_color $YELLOW "- Backup: nlm-config backup"
    print_color $YELLOW "- Restore: nlm-config restore backup_file.tar.gz"

    print_color $GREEN "\nFor detailed documentation, see:"
    print_color $YELLOW "- /workspace/app/docs/CONFIGURATION.md"
    print_color $YELLOW "- /workspace/app/CONFIG_IMPROVEMENT_PLAN.md"
    print_color $YELLOW "- /workspace/app/scripts/install-config-tools.sh"
}

# Run main function
main "$@"
