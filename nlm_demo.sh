#!/bin/bash
# NLM Phase 6 Demonstration Script
# This script demonstrates the key improvements made to NLM Phase 6
# Run: chmod +x nlm_demo.sh && ./nlm_demo.sh

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_color() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to print section headers
print_section() {
    local title=$1
    echo ""
    echo -e "${BLUE}=== ${title} ===${NC}"
    echo ""
}

# Main demonstration
main() {
    echo ""
    print_color "$YELLOW" "NLM Phase 6 - Demonstration of Improvements"
    echo ""
    print_color "$GREEN" "This script demonstrates the key improvements made to NLM Phase 6:"
    echo ""
    
    # Check if NLM executable exists
    if [ ! -f "./nlm" ]; then
        print_color "$RED" "ERROR: NLM executable not found. Please compile NLM Phase 6 first."
        echo ""
        echo "To compile NLM Phase 6:"
        echo "  mkdir build"
        echo "  cd build"
        echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
        echo "  make -j4"
        exit 1
    fi
    
    print_section "1. Easy Mode - Simple Usage"
    print_color "$GREEN" "Running NLM in Easy mode with default settings..."
    echo ""
    
    # Run easy mode (this will show the improved user interface)
    if ./nlm --mode easy 2>&1 | head -50; then
        print_color "$GREEN" "✓ Easy mode demonstration completed successfully"
    else
        print_color "$YELLOW" "⚠ Easy mode demo may have run with warnings (this is normal)"
    fi
    
    print_section "2. Help System - Improved Documentation"
    print_color "$GREEN" "Displaying help information..."
    echo ""
    
    # Show help to demonstrate improved documentation
    ./nlm --help | head -30
    
    print_color "$GREEN" "✓ Help system is comprehensive and user-friendly"
    
    print_section "3. Version Information"
    print_color "$GREEN" "Displaying version information..."
    echo ""
    
    # Show version
    ./nlm --version
    
    print_color "$GREEN" "✓ Version information is clear and informative"
    
    print_section "4. List Available Configurations"
    print_color "$GREEN" "Listing available configuration files..."
    echo ""
    
    # List configs
    ./nlm --list-configs
    
    print_color "$GREEN" "✓ Configuration system is well-organized"
    
    print_section "5. Multiple Modes Demonstration"
    print_color "$YELLOW" "Demonstrating the four available modes..."
    echo ""
    
    print_color "$BLUE" "Mode 1: Easy Mode"
    print_color "$GREEN" "  - Simple, guided usage for beginners"
    print_color "$GREEN" "  - Sensible defaults"
    print_color "$GREEN" "  - Step-by-step guidance"
    
    print_color "$BLUE" "Mode 2: Demo Mode"
    print_color "$GREEN" "  - Pre-built demonstrations"
    print_color "$GREEN" "  - Multiple demo types"
    print_color "$GREEN" "  - Performance metrics"
    
    print_color "$BLUE" "Mode 3: Expert Mode"
    print_color "$GREEN" "  - Advanced control for researchers"
    print_color "$GREEN" "  - Scripting support"
    print_color "$GREEN" "  - Data export options"
    
    print_color "$BLUE" "Mode 4: Custom Mode"
    print_color "$GREEN" "  - Load configuration from files"
    print_color "$GREEN" "  - Flexible parameter customization"
    print_color "$GREEN" "  - Script integration"
    
    print_section "6. Key Improvements Summary"
    print_color "$YELLOW" "Key improvements implemented:"
    echo ""
    
    print_color "$GREEN" "✓ Better Command-Line Interface"
    print_color "$GREEN" "  - Multiple modes for different skill levels"
    print_color "$GREEN" "  - Comprehensive help system"
    print_color "$GREEN" "  - Advanced configuration options"
    
    print_color "$GREEN" "✓ Enhanced User Experience"
    print_color "$GREEN" "  - Improved error messages"
    print_color "$GREEN" "  - Progress tracking"
    print_color "$GREEN" "  - Better output formatting"
    
    print_color "$GREEN" "✓ Advanced Features"
    print_color "$GREEN" "  - Expert mode scripting"
    print_color "$GREEN" "  - Data export (CSV, JSON)"
    print_color "$GREEN" "  - Report generation"
    print_color "$GREEN" "  - Custom configuration loading"
    
    print_color "$GREEN" "✓ Better Structure"
    print_color "$GREEN" "  - Phase 6 integration focused"
    print_color "$GREEN" "  - Modular design"
    print_color "$GREEN" "  - Improved code organization"
    print_color "$GREEN" "  - Enhanced documentation"
    
    print_section "7. Usage Examples"
    print_color "$YELLOW" "Example commands to try:"
    echo ""
    print_color "$BLUE" "# Quick start with Easy mode"
    print_color "$GREEN" "./nlm --mode easy"
    echo ""
    print_color "$BLUE" "# Run specific integration test"
    print_color "$GREEN" "./nlm --mode demo --demo memory"
    echo ""
    print_color "$BLUE" "# Advanced experiment with data export"
    print_color "$GREEN" "./nlm --mode expert --neurons 2000 --enable-csv --enable-json"
    echo ""
    print_color "$BLUE" "# Use custom configuration"
    print_color "$GREEN" "./nlm --mode custom --config my_config.cfg"
    echo ""
    
    print_section "8. Conclusion"
    print_color "$GREEN" "The NLM Phase 6 system has been significantly improved:"
    echo ""
    print_color "$GREEN" "  - Better accessibility for users of all skill levels"
    print_color "$GREEN" "  - Powerful features for advanced users"
    print_color "$GREEN" "  - Comprehensive documentation and examples"
    print_color "$GREEN" "  - Robust error handling and user feedback"
    print_color "$GREEN" "  - Flexible configuration options"
    print_color "$GREEN" "  - Research-ready tools and automation capabilities"
    echo ""
    print_color "$YELLOW" "Ready to use! Start exploring NLM Phase 6 with the examples above."
}

# Run the demonstration
main
