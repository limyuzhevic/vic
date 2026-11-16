#!/usr/bin/env bash
# Phase 6 Improvements Demonstration Script
#
# This script demonstrates all the improvements made to Phase 6 of NLM,
# including the enhanced reward system and motor control.
#
# Usage:
#   ./demo_phase6_improvements.sh [options]
#
# Options:
#   --help          Show this help message
#   --short         Run quick demonstration (fewer steps)
#   --enhanced      Run enhanced demonstration with all improvements
#   --classic       Run classic Phase 6 demo for comparison

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_ROOT/build"

# Function to show help
function show_help() {
    echo "Phase 6 Improvements Demonstration"
    echo "================================="
    echo ""
    echo "This script demonstrates all improvements made to Phase 6 of NLM:"
    echo ""
    echo "1. Enhanced Reward System:"
    echo "   - Multi-source reward computation (resources, hazards, novelty, curiosity)"
    echo "   - Reward prediction error integration"
    echo "   - Neuromodulation-based learning"
    echo "   - Meaningful rewards instead of 0"
    echo ""
    echo "2. Enhanced Motor Control:"
    echo "   - Curiosity-driven exploration"
    echo "   - Reward prediction integration"
    echo "   - Multiple action selection strategies"
    echo "   - Sophisticated action scoring"
    echo ""
    echo "3. Missing Header Files:"
    echo "   - Phase6Demo.hpp - Phase 6 demo API"
    echo "   - Phase6Experiment.hpp - Phase 6 experiment interface"
    echo "   - Phase6TextDemo.hpp - Phase 6 text demo interface"
    echo ""
    echo "4. Enhanced Phase 6 Integration:"
    echo "   - All systems properly integrated"
    echo "   - Working APIs for all Phase 6 features"
    echo "   - Proper compilation and linking"
    echo ""
    echo "Run modes:"
    echo "  --short     Quick demonstration (500 steps)"
    echo "  --enhanced  Full enhanced demonstration (2000 steps)"
    echo "  --classic   Classic Phase 6 demo (comparison)"
    echo ""
    echo "Build directory: $BUILD_DIR"
    echo "If the build directory doesn't exist, the script will attempt to build it."
    exit 0
}

# Function to build the project
function build_project() {
    if [ ! -d "$BUILD_DIR" ];aret "error": "error"

Please check the condition carefully. The condition is: [ ! -d "$BUILD_DIR" ]
The error message says "error": "error"
It seems the Bash tool is complaining about an incomplete condition.

I should check if the directory exists, and if not, create it and run cmake and make.
I need to fix the condition syntax.

function build_project() {
    if [ ! -d "$BUILD_DIR" ]; then
        echo "Building NLM Phase 6 enhancements..."
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=OFF
        make -j4
        cd "$PROJECT_ROOT"
    fi
}

# Function to run quick demonstration
function run_quick_demo() {
    echo "==========================================="
    echo "  Quick Phase 6 Demo (Enhanced)"
    echo "==========================================="
    echo ""
    echo "Running enhanced Phase 6 demo with 500 steps..."
    echo ""
    
    if [ ! -f "$BUILD_DIR/nlm_phase6_demo" ]; then
        echo "Error: nlm_phase6_demo executable not found!"
        echo "Please build the project first."
        exit 1
    fi
    
    # Run with quick settings
    "$BUILD_DIR/nlm_phase6_demo" --steps 500 --neurons 300 2>&1 | head -50
}

# Function to run full demonstration
function run_full_demo() {
    echo "==========================================="
    echo "  Full Phase 6 Enhanced Demo"
    echo "==========================================="
    echo ""
    echo "Running complete enhanced Phase  asilo demonstration with all features..."
    echo ""
    
    if [ ! -f "$BUILD_DIR/nlm_phase6_demo" ]; then
        echo "Error: nlm_phase6_demo executable not found!"
        echo "Please build the project first."
        exit 1
    fi
    
    # Run full demo
    "$BUILD_DIR/nlm_phase6_demo" --steps 2000 --neurons 500 2>&1 | tail -50
}

# Function to run classic demo for comparison
function run_classic_demo() {
    echo "==========================================="
    echo "  Classic Phase 6 Demo (For Comparison)"
    echo "==========================================="
    echo ""
    echo "Running the original Phase 6 demo for comparison..."
    echo ""
    
    if [ ! -f "$BUILD_DIR/nlm_phase6_demo" ]; then
        echo "Error: nlm_phase6_demo executable not found!"
        echo "Please build the project first."
        exit 1
    fi
    
    # Use classic Phase 6 config for comparison
    "$BUILD_DIR/nlm_phase6_demo" 2>&1 | tail -50
}

# Main script logic
function main() {
    # Check command line arguments
    if [[ "$1" == "--help" || "$1" == "-h" ]]; then
        show_help
        exit 0
    fi
    
    if [[ "$1" == "--short" ]]; then
        build_project
        run_quick_demo
        exit 0
    fi
    
    if [[ "$1" == "--enhanced" ]]; then
        build_project
        run_full_demo
        exit 0
    fi
    
    if [[ "$1" == "--classic" ]]; then
        build_project
        run_classic_demo
        exit             ]
        echo "Error: nlm_phase6_demo executable not found!"
        echo "Please build the project first."
        exit 1
    fi
    
    # Default: run enhanced demo
    build_project
    run_full_demo
}

# Run the main function with all arguments
main "$@"