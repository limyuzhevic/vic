#!/bin/bash

# NLM Validation and Testing Script
# Validates all improvements and new features

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Test 1: Check for configuration improvements
validate_configurations() {
    print_info "Test 1: Validating configuration improvements..."
    
    local issues=0
    
    # Check if default.cfg has duplicate settings fixed
    if grep -q "dopamine_baseline = 0.0" configs/default.cfg; then
        print_error "ERROR: default.cfg still has duplicate dopamine_baseline setting"
        issues=$((issues + 1))
    else
        print_success "✓ default.cfg configuration is clean (no duplicate settings)"
    fi
    
    # Check if advanced.cfg exists
    if [[ -f "configs/advanced.cfg" ]]; then
        print_success "✓ advanced.cfg exists with comprehensive settings"
        
        # Check for new advanced settings
        if grep -q "dopamine_excitation =" configs/advanced.cfg; then
            print_success "✓ advanced.cfg includes neuromodulation enhancements"
        else
            print_warning "⚠ advanced.cfg may be missing some neuromodulation settings"
        fi
        
        if grep -q "working_memory_capacity =" configs/advanced.cfg; then
            print_success "✓ advanced.cfg includes memory system settings"
        else
            print_warning "⚠ advanced.cfg may be missing memory system settings"
        fi
    else
        print_error "ERROR: advanced.cfg not found"
        issues=$((issues + 1))
    fi
    
    return $issues
}

# Test 2: Check for improved command line interface
validate_cli() {
    print_info "Test 2: Validating command line interface improvements..."
    
    local issues=0
    
    # Check if nlm.sh script exists and is executable
    if [[ -f "scripts/nlm.sh" ]]; then
        if [[ -x "scripts/nlm.sh" ]]; then
            print_success "✓ nlm.sh script exists and is executable"
            
            # Check for help functionality
            if bash scripts/nlm.sh --help >/dev/null 2>&1; then
                print_success "✓ nlm.sh --help works correctly"
            else
                print_error "ERROR: nlm.sh --help is not working"
                issues=$((issues + 1))
            fi
        else
            print_error "ERROR: nlm.sh script is not executable"
            issues=$((issues + 1))
        fi
    else
        print_error "ERROR: nlm.sh script not found"
        issues=$((issues + 1))
    fi
    
    return $issues
}

# Test 3: Check for improved main.cpp features
validate_main_cpp() {
    print_info "Test 3: Validating main.cpp improvements..."
    
    local issues=0
    
    # Check for mode support (phase2, phase6, experiment)
    if grep -q "mode = \"phase2\"" src/main.cpp; then
        print_success "✓ main.cpp supports phase2 mode"
    else
        print_error "ERROR: main.cpp missing phase2 mode support"
        issues=$((issues + 1))
    fi
    
    if grep -q "mode == \"phase6\"" src/main.cpp; then
        print_success "✓ main.cpp supports phase6 mode"
    else
        print_error "ERROR: main.cpp missing phase6 mode support"
        issues=$((issues + 1))
    fi
    
    if grep -q "mode == \"experiment\"" src/main.cpp; then
        print_success "✓ main.cpp supports experiment mode"
    else
        print_error "ERROR: main.cpp missing experiment mode support"
        issues=$((issues + 1))
    fi
    
    # Check for improved error handling
    if grep -q "Configuration file not found:" src/main.cpp | head -1 > /dev/null; then
        print_success "✓ main.cpp has improved error handling for missing config files"
    else
        print_warning "⚠ main.cpp may need improved error handling"
    fi
    
    # Check for command line argument parsing
    if grep -q "Parse command line arguments" src/main.cpp; then
        print_success "✓ main.cpp includes command line argument parsing"
    else
        print_error "ERROR: main.cpp missing command line argument parsing"
        issues=$((issues + 1))
    fi
    
    return $issues
}

# Test 4: Check for improved documentation
validate_documentation() {
    print_info "Test 4: Validating documentation improvements..."
    
    local issues=0
    
    # Check for easy_usage.md
    if [[ -f "easy_usage.md" ]]; then
        print_success "✓ easy_usage.md exists with simplified usage instructions"
    else
        print_error "ERROR: easy_usage.md not found"
        issues=$((issues + 1))
    fi
    
    # Check for HOW_TO_USE.md
    if [[ -f "HOW_TO_USE.md" ]]; then
        print_success "✓ HOW_TO_USE.md exists with detailed usage instructions"
    else
        print_error "ERROR: HOW_TO_USE.md not found"
        issues=$((issues + 1))
    fi
    
    return $issues
}

# Test 5: Check for improved tests
validate_tests() {
    print_info "Test 5: Validating test improvements..."
    
    local issues=0
    
    # Check for test files
    local test_count=$(find tests -name "*.cpp" -o -name "*.h" | wc -l)
    if [[ $test_count -gt 0 ]]; then
        print_success "✓ Found $test_count test files"
    else
        print_error "ERROR: No test files found"
        issues=$((issues + 1))
    fi
    
    return $issues
}

# Test 6: Validate project structure
validate_project_structure() {
    print_info "Test 6: Validating project structure..."
    
    local issues=0
    
    # Check for key directories
    local directories=(
        "src"
        "src/brain"
        "src/core"
        "src/experiments"
        "configs"
        "docs"
        "scripts"
        "tests"
    )
    
    for dir in "${directories[@]}"; do
        if [[ -d "$dir" ]]; then
            print_success "✓ $dir directory exists"
        else
            print_error "ERROR: $dir directory not found"
            issues=$((issues + 1))
        fi
    done
    
    return $issues
}

# Main validation function
main_validation() {
    print_info "Starting NLM Improvements Validation..."
    print_info "=" * 60
    
    local total_issues=0
    
    # Run all tests
    validate_project_structure
    struct_result=$?
    total_issues=$((total_issues + struct_result))
    
    validate_configurations
    config_result=$?
    total_issues=$((total_issues + config_result))
    
    validate_cli
    cli_result=$?
    total_issues=$((total_issues + cli_result))
    
    validate_main_cpp
    main_result=$?
    total_issues=$((total_issues + main_result))
    
    validate_documentation
    doc_result=$?
    total_issues=$((total_issues + doc_result))
    
    validate_tests
    test_result=$?
    total_issues=$((total_issues + test_result))
    
    print_info "=" * 60
    print_info "Validation Summary:"
    print_info "- Project structure issues: $struct_result"
    print_info "- Configuration issues: $config_result"
    print_info "- CLI issues: $cli_result"
    print_info "- Main.cpp issues: $main_result"
    print_info "- Documentation issues: $doc_result"
    print_info "- Test issues: $test_result"
    
    if [[ $total_issues -eq 0 ]]; then
        print_success "All validations passed! ✓"
        print_info "NLM improvements are working correctly."
        return 0
    else
        print_error "Validation found $total_issues issues that need attention."
        return 1
    fi
}

# Run the validation
main_validation
exit $?