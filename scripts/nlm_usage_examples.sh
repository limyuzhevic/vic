#!/usr/bin/bash

# NLM Usage Examples and Demos
# This script demonstrates the new features and improvements in NLM

# Colors for output
GREEN="\033[0;32m"
BLUE="\033[0;34m"
YELLOW="\033[1;33m"
RED="\033[0;31m"
NC="\033[0m" # No Color

# Print header
function print_header() {
    echo -e "${BLUE}===========================================================${NC}"
    echo -e "${BLUE}           NLM - Neural Learning Machine - Usage Examples${NC}"
    echo -e "${BLUE}===========================================================${NC}"
    echo -e "${YELLOW}This script demonstrates the new features and improvements in NLM${NC}"
    echo -e "${YELLOW}===========================================================${NC}"
}

# Print section header
function print_section() {
    echo -e "\n${GREEN}>> $1${NC}"
    echo -e "${GREEN}$(printf '%*.0s' ${#1} | tr ' ' '=')${NC}"
}

# Print example
function print_example() {
    echo -e "${YELLOW}Example:${NC} $1"
    echo -e "${GREEN}Command:${NC} $2"
}

# Main function
function main() {
    print_header
    
    print_section "NEW CLI INTERFACE"
    echo -e "${YELLOW}Overview:${NC} NLM now has a unified command-line interface with multiple modes"
    echo -e "${YELLOW}for different use cases and experimentation needs.${NC}"
    
    print_example "Run Phase 6 integration demo with custom settings" "nlm demo --phase6 --steps 5000 --neurons 1000"
    print_example "Run custom experiment with specific configuration" "nlm experiment --steps 10000 --neurons 2000 --regions 2 --enable-development"
    print_example "Start interactive mode for real-time control" "nlm interactive --port 8080 --host 0.0.0.0"
    print_example "Run performance benchmarks" "nlm benchmark --profile --detailed"
    print_example "Monitor running simulation" "nlm monitor --real-time --update 1s"
    print_example "List configuration options" "nlm config --list"
    print_example "Set configuration parameter" "nlm config --set neuron_count=2000"
    
    print_section "EXPERIMENT SYSTEM"
    echo -e "${YELLOW}Overview:${NC} Run and manage experiments with comprehensive configuration and result tracking.${NC}"
    
    print_example "Create and run an experiment" "nlm experiment --create demo_exp --neurons 500 --steps 1000"
    print_example "Run Phase 6 integration experiment" "nlm experiment --phase6 --enable-checkpointing"
    print_example "Compare experiment results" "nlm experiment --compare exp1.json exp2.json"
    print_example "Export experiment results" "nlm experiment --export results.csv --format csv"
    
    print_section "ADVANCED FEATURES"
    echo -e "${YELLOW}Overview:${NC} Access advanced features for power users and researchers.${NC}"
    
    print_example "Enable performance optimizations" "nlm optimized --event-driven --multithreading --simd"
    print_example "Enable sparse connectivity for large networks" "nlm optimized --sparse --neurons 10000"
    print_example "Enable checkpointing for long-running experiments" "nlm experiment --checkpoint --interval 1000"
    print_example "Enable replay and consolidation features" "nlm experiment --replay --consolidation"
    
    print_section "DEVELOPMENT TOOLS"
    echo -e "${YELLOW}Overview:${NC} Development and testing tools for researchers and developers.${NC}"
    
    print_example "Run development demo" "nlm demo --phase4 --neurons 1500"
    print_example "Run world interaction demo" "nlm demo --phase3 --episodes 10 --steps 300"
    print_example "Run cognition demo" "nlm demo --phase4 --neurons 1500 --cognition"
    print_example "Run visualization demo" "nlm demo --visualization --output network.png"
    
    print_section "CONFIGURATION MANAGEMENT"
    echo -e "${YELLOW}Overview:${NC} Advanced configuration management with JSON support.${NC}"
    
    print_example "Load configuration from file" "nlm config --load config.json"
    print_example "Set multiple configuration values" "nlm config --set neuron_count=2000 region_count=3 connection_probability=0.15"
    print_example "Save current configuration" "nlm config --save current_config.json"
    print_example "Validate configuration" "nlm config --validate --file test_config.json"
    
    print_section "PERFORMANCE MONITORING"
    echo -e "${YELLOW}Overview:${NC} Monitor simulation performance and resource usage.${NC}"
    
    print_example "Monitor simulation with real-time updates" "nlm monitor --start --update 0.5s"
    print_example "Profile simulation performance" "nlm benchmark --profile --save profile.json"
    print_example "Analyze simulation performance" "nlm analyze --input results.bin --output analysis.txt"
    print_example "Compare performance between configurations" "nlm compare --config1 config.json --config2 config2.json"
    
    print_section "ERROR HANDLING AND RECOVERY"
    echo -e "${YELLOW}Overview:${NC} Robust error handling and recovery mechanisms.${NC}"
    
    print_example "Enable error recovery" "nlm config --set error_recovery=true"
    print_example "Enable detailed error reporting" "nlm config --set error_reporting=detailed"
    print_example "Enable automatic recovery" "nlm config --set auto_recovery=true"
    print_example "Enable validation" "nlm validate --strict --all"
    
    print_section "VISUALIZATION AND OUTPUT"
    echo -e "${YELLOW}Overview:${NC} Enhanced visualization and output capabilities.${NC}"
    
    print_example "Generate network visualization" "nlm visualization --network --output network.png --format png"
    print_example "Generate performance graphs" "nlm visualization --performance --output performance.png"
    print_example "Generate simulation timeline" "nlm visualization --timeline --output timeline.svg"
    print_example "Generate result reports" "nlm visualization --reports --output results.html"
    
    print_section "MIGRATION GUIDE"
    echo -e "${YELLOW}Overview:${NC} How to migrate from previous versions.${NC}"
    echo -e "${YELLOW}No migration needed - all existing functionality preserved!${NC}"
    print_example "Existing demos still work" "nlm_phase3_demo 5 200"
    print_example "Original Phase 6 demo" "nlm_phase6_demo"
    print_example "Original Phase 2 demo" "nlm --demo --phase2"
    
    echo -e "\n${GREEN}All examples demonstrate the improved NLM functionality!${NC}"
    echo -e "${YELLOW}For more detailed help, use:${NC}"
    echo -e "  nlm --help"
    echo -e "  nlm demo --help"
    echo -e "  nlm experiment --help"
    echo -e "  nlm config --help"
}

# Run main function
main