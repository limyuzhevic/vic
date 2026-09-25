#!/usr/bin/env bash

# Advanced Visualization Control Script
# Power user command for controlling NLM visualization

# Usage: ./advanced_visualization_control.sh [options]
# Options:
#   --list-components             List all registered visualization components
#   --create-component NAME       Create new visualization component
#   --enable-component NAME       Enable visualization component
#   --disable-component NAME      Disable visualization component
#   --show-network               Show neural network visualization
#   --show-performance           Show performance metrics visualization
#   --show-debug NAME            Show debug visualization
#   --export-visualization FILE   Export visualization to file
#   --export-all                 Export all visualizations
#   --interactive-mode           Enable interactive visualization mode
#   --batch-mode                 Enable batch visualization mode
#   --update-rate RATE            Set visualization update rate
#   --monitor-updates            Monitor visualization updates
#   --help                       Show help message

cd "$(dirname "$0")"

SCRIPT_DIR="$(pwd)"
LOG_FILE="visualization_control_$(date +%Y%m%d_%H%M%S).log"
VIZ_DATA_DIR="../visualization_data"
EXPORT_DIR="../visualization_exports"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

log() {
    echo -e "[$(date '+%Y-%m-%d %H:%M:%S')] $*" | tee -a "$LOG_FILE"
}

error_exit() {
    echo -e "${RED}[ERROR] $*${NC}" | tee -a "$LOG_FILE"
    exit 1
}

list_components() {
    echo -e "${CYAN}Registered Visualization Components:${NC}"
    echo "==============================="
    
    # Built-in components
    local builtin_components=(
        "neural_network" "neuron_activity" "synapse_weights" "spike_rasters"
        "performance_metrics" "memory_usage" "custom_visualization"
    )
    
    for component in "${builtin_components[@]}"; do
        echo "${GREEN}${component}${NC}"
    done
    
    # Check for custom components
    local custom_dir="../custom_components"
    if [ -d "$custom_dir" ]; then
        echo -e "\n${YELLOW}Custom Components:${NC}"
        for component in "$custom_dir"/*.json; do
            if [ -f "$component" ]; then
                local name=$(basename "$component" .json)
                echo "${YELLOW}${name}${NC}"
            fi
        done
    fi
    
    echo -e "\n${CYAN}Component Types:${NC}"
    echo "  - Neural Network: Brain structure and connectivity"
    echo "  - Neuron Activity: Individual neuron states and firing rates"
    echo "  - Synapse Weights: Connection strengths and plasticity"
    echo "  - Spike Rasters: Spike timing and frequency visualizations"
    echo "  - Performance Metrics: System resource utilization"
    echo "  - Memory Usage: Memory pool allocations and leaks"
    echo "  - Custom: User-defined visualization components"
}

create_component() {
    local name=$1
    if [ -z "$name" ]; then
        error_exit "Please specify a component name"
    fi
    
    mkdir -p "../custom_components"
    
    if [ -f "../custom_components/${name}.json" ]; then
        error_exit "Component '$name' already exists"
    fi
    
    log "Creating new visualization component: $name"
    
    # Create component definition
    cat > "../custom_components/${name}.json" << EOF
{
  "name": "${name}",
  "type": "custom",
  "description": "User-defined visualization component",
  "enabled": true,
  "parameters": {
    "update_rate": 10.0,
    "threshold": 0.5,
    "color_scheme": "viridis",
    "dimensions": 2,
    "interactive": true
  },
  "data_sources": [],
  "visualization_settings": {
    "width": 800,
    "height": 600,
    "title": "${name.replace('_', ' ')}",
    "axes": {"x": "Time", "y": "Value"},
    "legend": true
  },
  "created": "$(date -Iseconds)",
  "updated": "$(date -Iseconds)",
  "tags": ["custom", "user_defined"]
}
EOF
    
    log "${GREEN}Visualization component '$name' created successfully${NC}"
    echo "Component '${YELLOW}${name}${NC}' created at ${YELLOW}../custom_components/${name}.json${NC}"
}

show_network() {
    log "Displaying neural network visualization..."
    
    echo -e "${CYAN}Neural Network Visualization${NC}"
    echo "============================="
    
    # Check for network data
    if [ -f "$VIZ_DATA_DIR/network_data.json" ]; then
        echo "Network data available"
        echo "Generating visualization..."
        
        # Create network visualization
        python3 << 'EOF'
import json
import sys
import os
from datetime import datetime

viz_data_file = '$VIZ_DATA_DIR/network_data.json'
if not os.path.exists(viz_data_file):
    print("Network data not found")
    sys.exit(1)

with open(viz_data_file, 'r') as f:
    network_data = json.load(f)

print("Neural Network Structure:")
print(f"  Total Neurons: {network_data.get('neuronCount', 0)}")
print(f"  Total Synapses: {network_data.get('synapseCount', 0)}")
print(f"  Brain Regions: {network_data.get('regionCount', 0)}")

if 'populations' in network_data:
    print(f"\nPopulations:")
    for pop_name, pop_data in network_data['populations'].items():
        print(f"  - {pop_name}: {pop_data.get('neuronCount', 0)} neurons")

# Generate simple ASCII visualization
print(f"\nNetwork Topology Visualization:")
print("  [Brain]"),
for i in range(5):
    print(" -> [Region", i, "]", end="")
    for j in range(3):
        print(" -> [Population", i*3+j, "]", end="")
        for k in range(10):
            print(" o", end="")
        print(" <- [Neuron", k, "]", end="")
print(" <- [Output]")

EOF
    else
        echo "No network data available"
        echo "Run neural network simulation first to generate data"
    fi
    
    echo -e "\n${YELLOW}Visualization Options:${NC}"
    echo "  --show-network-structure    Show brain regions and populations"
    echo "  --show-neuron-activity      Show individual neuron firing"
    echo "    --threshold VALUE         Set activity threshold"
    echo "  --show-synapse-weights      Show connection strengths"
    echo "  --show-spike-rasters       Show spike timing visualization"
    echo "  --export-to FILE            Export visualization to file"
}

show_performance() {
    log "Displaying performance metrics visualization..."
    
    echo -e "${CYAN}Performance Metrics Visualization${NC}"
    echo "==============================="
    
    if [ -f "$VIZ_DATA_DIR/performance_data.json" ]; then
        echo "Performance data available"
        
        # Create performance dashboard
        python3 << 'EOF'
import json
import os
import statistics

perf_file = '$VIZ_DATA_DIR/performance_data.json'
if not os.path.exists(perf_file):
    print("Performance data not found")
    exit(1

with open(perf_file, 'r') as f:
    perf_data = json.load(f)

print("Performance Metrics Dashboard:")

# Get recent metrics
metrics = perf_data.get('recentMetrics', [])
if metrics:
    latest = metrics[-1]
    
    print(f"\nResource Utilization:")
    for resource, usage in latest.get('resources', {}).items():
        print(f"  {resource}: {usage:.2f}%")
    
    print(f"\nPerformance Counters:")
    counters = latest.get('counters', {})
    for counter_name, count in counters.items():
        print(f"  {counter_name}: {count:,}")
    
    print(f"\nMemory Usage:")
    memory = latest.get('memory', {})
    print(f"  Total: {memory.get('total', 0) / 1024 / 1024:.2f} MB")
    print(f"  Peak: {memory.get('peak', 0) / 1024 / 1024:.2f} MB")
    print(f"  Allocations: {memory.get('allocations', 0):,}")
else:
    print("No performance metrics available")

EOF
    else
        echo "No performance data available"
        echo "Run performance benchmark first to collect data"
    fi
    
    echo -e "\n${YELLOW}Performance Visualization Options:${NC}"
    echo "  --show-cpu-usage           Display CPU utilization"
    echo "  --show-memory-usage       Display memory usage"
    echo "  --show-throughput          Display operations per second"
    echo "  --show-latency            Display response times"
    echo "  --show-history            Display historical trends"
    echo "  --export-to FILE           Export to CSV or JSON"
}

enable_interactive_mode() {
    log "Enabling interactive visualization mode..."
    
    echo -e "${MAGENTA}Interactive Visualization Mode${NC}"
    echo "==============================="
    
    echo "Interactive features available:"
    echo "  - Mouse interaction: Click on neurons to inspect details"
    echo "  - Zoom/pan: Navigate through network structure"
    echo "  - Real-time updates: Watch metrics update live"
    echo "  - Data filtering: Filter data by time or metrics"
    echo "  - Annotation: Add notes and markers"
    
    echo -e "\n${YELLOW}Interactive Commands:${NC}"
    echo "  q - Quit interactive mode"
    echo "  r - Refresh display"
    echo "  f - Filter data"
    echo "  a - Add annotation"
    echo "  s - Save current view"
    echo "  h - Show help"
    
    echo "\nStarting interactive visualization..."
    echo "Note: This requires a GUI environment. For now, showing menu-based interface."
}

show_help() {
    cat << EOF
Advanced Visualization Control Script
====================================

USAGE:
  $0 [OPTIONS]

OPTIONS:
  --list-components             List all registered visualization components
  --create-component NAME       Create new visualization component
  --enable-component NAME       Enable visualization component
  --disable-component NAME      Disable visualization component
  --show-network               Show neural network visualization
  --show-performance           Show performance metrics visualization
  --show-debug NAME            Show debug visualization
  --export-visualization FILE   Export visualization to file
  --export-all                 Export all visualizations
  --interactive-mode           Enable interactive visualization mode
  --batch-mode                 Enable batch visualization mode
  --update-rate RATE            Set visualization update rate
  --monitor-updates            Monitor visualization updates
  --help                       Show help message

EXAMPLES:
  $0 --list-components          # List available components
  $0 --create-component my_viz   # Create new visualization component
  $0 --show-network             # Display neural network visualization
  $0 --show-performance         # Show performance metrics dashboard
  $0 --interactive-mode         # Start interactive visualization
  $0 --export-to visualization.png  # Export visualization

FEATURES:
  - Component management and registration
  - Multiple visualization types
  - Interactive and batch modes
  - Export functionality
  - Real-time monitoring
  - Custom component support

VISUALIZATION DATA DIRECTORY:
  $VIZ_DATA_DIR

EXPORT DIRECTORY:
  $EXPORT_DIR

LOG FILE:
  $LOG_FILE

The script provides comprehensive visualization control for power users.
EOF
}

# Command line argument parsing
case "${1:-}" in
    list-components)
        list_components
        ;;
    create-component)
        create_component "$2"
        ;;
    show-network)
        show_network
        ;;
    show-performance)
        show_performance
        ;;
    interactive-mode)
        enable_interactive_mode
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