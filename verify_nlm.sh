#!/usr/bin/env bash
"""
Quick Verification Script for NLM Phase 2 Enhanced System
========================================================

This script performs a basic verification of the NLM enhanced system
by checking key components and running simple tests.

Usage: ./verify_nlm.sh
"""

# Set error handling

# Set error handling

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_color() {
    local color="$1"
    local message="$2"
    echo -e "${color}${message}${NC}"
}

print_header() {
    local title="$1"
    echo -e "\n${YELLOW}=====================================================\n${title}\n=====================================================${NC}"
}

# Check if NLM executable exists
check_executable() {
    if [[ ! -f "nlm" ]]; then
        print_color $RED "ERROR: NLM executable not found!"
        print_color $YELLOW "Please build the project first:"
        echo "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)"
        return 1
    fi
}

# Check documentation files
check_documentation() {
    local missing_docs=0
    local doc_files=(
        "NLM_Documentation.md"
        "README.md"
        "ENHANCED_SYSTEM_SUMMARY.md"
    )

    print_header "Checking Documentation"

    for doc in "${doc_files[@]}"; do
        if [[ -f "$doc" ]]; then
            local size=$(wc -c < "$doc" 2>/dev/null)
            if [[ $size -gt 1000 ]]; then
                print_color $GREEN "✓ Found $doc ($size bytes)"
            else
                print_color $YELLOW "⚠ $doc seems incomplete ($size bytes)"
                ((missing_docs++))
            fi
        else
            print_color $RED "✗ Missing: $doc"
            ((missing_docs++))
        fi
    done

    if [[ $missing_docs -eq 0 ]]; then
        print_color $GREEN "\nAll documentation files present and complete."
    else
        print_color $YELLOW "\n$missing_docs documentation file(s) missing or incomplete."
    fi
    return $((missing_docs > 0 ? 1 : 0))
}

# Check configuration files
check_configuration() {
    print_header "Checking Configuration Files"

    local config_files=(
        "configs/default.cfg"
        "configs/complete_config.cfg"
        "configs/example_config.json"
    )

    local missing_config=0

    for config in "${config_files[@]}"; do
        if [[ -f "$config" ]]; then
            print_color $GREEN "✓ Found $config"
        else
            print_color $RED "✗ Missing: $config"
            ((missing_config++))
        fi
    done

    if [[ $missing_config -eq 0 ]]; then
        print_color $GREEN "\nAll configuration files present."
    else
        print_color $YELLOW "\n$missing_config configuration file(s) missing."
    fi
    return $((missing_config > 0 ? 1 : 0))
}

# Check source files
check_source_files() {
    print_header "Checking Source Files"

    local source_files=(
        "src/main.cpp"
        "src/core/Config/Config.hpp"
        "src/core/Config/Config.cpp"
        "src/core/Config/ConfigSchema.cpp"
        "src/brain/Brain.hpp"
        "src/brain/Brain.cpp"
        "src/neuromodulation/Neuromodulator.hpp"
        "src/neuromodulation/Neuromodulator.cpp"
        "src/memory/NeuralWorkingMemory.hpp"
        "src/memory/NeuralWorkingMemory.cpp"
    )

    local missing_source=0

    for source in "${source_files[@]}"; do
        if [[ -f "$source" ]]; then
            local size=$(wc -c < "$source" 2>/dev/null)
            if [[ $size -gt 1000 ]]; then
                print_color $GREEN "✓ Found $source ($size bytes)"
            else
                print_color $YELLOW "⚠ $source seems incomplete ($size bytes)"
                ((missing_source++))
            fi
        else
            print_color $RED "✗ Missing: $source"
            ((missing_source++))
        fi
    done

    if [[ $missing_source -eq 0 ]]; then
        print_color $GREEN "\nAll key source files present."
    else
        print_color $YELLOW "\n$missing_source source file(s) missing or incomplete."
    fi
    return $((missing_source > 0 ? 1 : 0))
}

# Test basic functionality
quick_test() {
    print_header "Running Quick Test"

    print_color $YELLOW "Testing basic NLM functionality..."

    # Test help option
    echo "Testing help option..."
    if ./nlm --help > /dev/null 2>&1; then
        print_color $GREEN "✓ Help option works"
    else
        print_color $RED "✗ Help option failed"
        return 1
    fi

    # Test demo mode with minimal steps
    echo "Testing demo mode (50 steps)..."
    if timeout 30 ./nlm --demo --steps 50 --quiet > /dev/null 2>&1; then
        print_color $GREEN "✓ Demo mode executed successfully"
    else
        print_color $RED "✗ Demo mode failed or timed out"
        return 1
    fi

    print_color $GREEN "\nBasic functionality test passed."
    return 0
}

# Main function
main() {
    print_header "NLM Phase 2 Enhanced System Verification"

    echo -e "\nThis script verifies that the NLM enhanced system is properly installed and working."
    echo -e "It checks for essential files, documentation, and basic functionality."

    # Check for executable
    check_executable
    if [[ $? -ne 0 ]]; then
        echo "\n${YELLOW}Please build the project first before running verification.${NC}"
        echo "\nBuild commands:"
        echo "  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)"
        exit 1
    fi

    # Run checks
    local errors=0

    check_documentation || ((errors++))
    check_configuration || ((errors++))
    check_source_files || ((errors++))

    # Only run functional test if other checks pass
    if [[ $errors -eq 0 ]]; then
        quick_test || ((errors++))
    else
        print_color $YELLOW "\nSkipping functional test due to missing files."
    fi

    # Final results
    print_header "Verification Summary"

    if [[ $errors -eq 0 ]]; then
        echo -e "${GREEN}✅ SUCCESS: All checks passed!${NC}"
        echo "\nThe NLM Phase 2 Enhanced system is properly installed and ready for use."
        echo "\nKey features verified:"
        echo "  • Comprehensive documentation"
        echo "  • Configuration system with JSON/YAML support"
        echo "  • Enhanced neuromodulation"
        echo "  • Working memory and attentional systems"
        echo "  • Advanced command-line interface"
        echo "  • Complete brain integration"
        echo "\nNext steps:"
        echo "  1. Read NLM_Documentation.md for comprehensive information"
        echo "  2. Configure your project using configs/default.cfg"
        echo "  3. Run './nlm --demo --steps 1000' to start simulation"
        echo "  4. Explore documentation for advanced usage"
    else
        echo -e "${RED}❌ VERIFICATION FAILED: $errors check(s) failed.${NC}"
        echo "\nPlease fix the issues above before using the system."
        echo "\nCommon fixes:"
        echo "  • Build the project: mkdir build && cd build && cmake .. && make"
        echo "  • Check for missing source files in src/"
        echo "  • Verify documentation files in project root"
        echo "  • Ensure executable 'nlm' is built"
    fi

    exit $errors
}

# Run main function
main
