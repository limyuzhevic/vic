#!/usr/bin/env bash
# Make the advanced features script executable
chmod +x advanced_features.sh

# Create directories for advanced features
mkdir -p advanced_presets profiles visualizations analysis_results brain_exports brain_imports

# Source the advanced features script (if it's in the same directory)
if [ -f "advanced_features.sh" ]; then
    echo "Advanced features script found and executable."
else
    echo "Error: advanced_features.sh not found in current directory."
    exit 1
fi

# Display welcome message
echo "=== NLM Advanced Features ==="
echo "This script provides advanced configuration, profiling, and analysis tools for NLM users."
echo ""
echo "Usage: ./advanced_features.sh <command> [options]"
echo ""
echo "Available commands:"
echo "  presets              Create configuration presets"
echo "  profile <demo>       Profile performance of a demo"
echo "  visualize <demo>     Generate advanced visualizations"
echo "  analyze <checkpoint>  Analyze brain state from checkpoint"
echo "  custom <config> <demo> [params]  Run custom demo with config"
echo "  export <checkpoint>   Export brain state with metadata"
echo "  import <export>       Import brain state"
echo "  help                 Show this help message"
echo ""
echo "Example usage:"
echo "  ./advanced_features.sh presets"
echo "  ./advanced_features.sh profile phase6"
echo "  ./advanced_features.sh visualize phase3"
echo "  ./advanced_features.sh analyze my_checkpoint.bin"
echo ""
echo "For more information about a specific command, run:"
echo "  ./advanced_features.sh help"

# Check for commands and run them
if [ $# -eq 0 ]; pocos ]; then
    echo "Error: No command specified."
    echo "For help, run: ./advanced_features.sh help"
    exit 1
fi

# Process the first argument
case "$1" in
    "presets")
        source "./advanced_features.sh" 2>/dev/null && create_presets
        ;;
    "profile")
        source "./advanced_features.sh" 2>/dev/null && profile_performance "$2"
        ;;
    "visualize")
        source "./advanced_features.sh" 2>/dev/null && visualize_advanced "$2"
        ;;
    "analyze")
        source "./advanced_features.sh" 2>/dev/null && analyze_brain_state "$2"
        ;;
    "custom")
        source "./advanced_features.sh" 2>/dev/null && custom_demo "$2" "$3" "$4"
        ;;
    "export")
        source "./advanced_features.sh" 2>/dev/null && export_brain_state "$2"
        ;;
    "import")
        source "./advanced_features.sh" 2>/dev/null && import_brain_state "$2"
        ;;
    "help"|"--help"|"-h")
        source "./advanced_features.sh" 2>/dev/null && advanced_features_help
        ;;
    *)
        echo "Unknown command: $1"
        source "./advanced_features.sh" 2>/dev/null && advanced_features_help
        exit 1
        ;;
esac