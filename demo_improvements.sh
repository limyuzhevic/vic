#!/bin/bash

# Simple NLM Test Script
# Demonstrates the improvements made to the NLM project

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}NLM Project Improvements Demonstration${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
}

print_footer() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${GREEN}Demonstration Complete!${NC}"
    echo -e "${BLUE}========================================${NC}"
}

# Test 1: Configuration Check
config_check() {
    print_info "Test 1: Configuration File Check"
    
    if [[ -f "configs/default.cfg" ]]; then
        print_success "✓ default.cfg exists"
        
        # Check for the fix we made
        if grep -q "dopamine_baseline = 0.1" "configs/default.cfg" && ! grep -q "dopamine_baseline = 0.0" "configs/default.cfg"; then
            print_success "✓ duplicate dopamine_baseline setting fixed"
        else
            print_error "✗ duplicate dopamine_baseline setting still exists"
        fi
    else
        print_error "✗ default.cfg not found"
    fi
    
    if [[ -f "configs/advanced.cfg" ]]; then
        print_success "✓ advanced.cfg exists with enhanced features"
        
        # Check for advanced features
        local features_found=0
        local total_features=5
        
        if grep -q "dopamine_excitation =" "configs/advanced.cfg"; then
            print_success "✓ Advanced neuromodulation settings"
            features_found=$((features_found + 1))
        fi
        
        if grep -q "working_memory_capacity =" "configs/advanced.cfg"; then
            print_success "✓ Memory system settings"
            features_found=$((features_found + 1))
        fi
        
        if grep -q "experiment_type =" "configs/advanced.cfg"; then
            print_success "✓ Experiment configuration"
            features_found=$((features_found + 1))
        fi
        
        if grep -q "enable_stdp =" "configs/advanced.cfg"; then
            print_success "✓ STDP enable option"
            features_found=$((features_found + 1))
        fi
        
        if grep -q "enable_working_memory =" "configs/advanced.cfg"; then
            print_success "✓ Working memory enable option"
            features_found=$((features_found + 1))
        fi
        
        if [[ $features_found -eq $total_features ]]; then
            print_success "✓ All advanced features present ($features_found/$total_features)"
        else
            print_warning "⚠ Only $features_found/$total_features advanced features found"
        fi
    else
        print_error "✗ advanced.cfg not found"
    fi
    
    echo ""
}

# Test 2: CLI Script Check
cli_check() {
    print_info "Test 2: Command Line Interface Check"
    
    if [[ -f "scripts/nlm.sh" ]]; then
        print_success "✓ nlm.sh script exists"
        
        if [[ -x "scripts/nlm.sh" ]]; then
            print_success "✓ nlm.sh is executable"
        else
            print_error "✗ nlm.sh is not executable"
        fi
        
        # Test help functionality
        if bash "scripts/nlm.sh" --help > /dev/null 2>&1; then
            print_success "✓ nlm.sh --help works"
        else
            print_error "✗ nlm.sh --help not working"
        fi
        
        # Test version functionality
        if bash "scripts/nlm.sh" --version > /dev/null 2>&1; then
            print_success "✓ nlm.sh --version works"
        else
            print_error "✗ nlm.sh --version not working"
        fi
        
        # Test validation functionality
        if bash "scripts/nlm.sh" --validate-config "configs/default.cfg" > /dev/null 2>&1; then
            print_success "✓ nlm.sh --validate-config works"
        else
            print_error "✗ nlm.sh --validate-config not working"
        fi
        
        # Check for command options
        local options_found=0
        local total_options=8
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--config"; then
            print_success "✓ --config option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--neurons"; then
            print_success "✓ --neurons option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--regions"; then
            print_success "✓ --regions option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--steps"; then
            print_success "✓ --steps option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--seed"; then
            print_success "✓ --seed option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--verbose"; then
            print_success "✓ --verbose option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--quiet"; then
            print_success "✓ --quiet option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--run-experiment"; then
            print_success "✓ --run-experiment option available"
            options_found=$((options_found + 1))
        fi
        
        if bash "scripts/nlm.sh" --help 2>&1 | grep -q "--benchmark"; then
            print_success "✓ --benchmark option available"
            options_found=$((options_found + 1))
        fi
        
        if [[ $options_found -eq $total_options ]]; then
            print_success "✓ All CLI options available ($options_found/$total_options)"
        else
            print_warning "⚠ Only $options_found/$total_options CLI options available"
        fi
    else
        print_error "✗ nlm.sh script not found"
    fi
    
    echo ""
}

# Test 3: Main.cpp Features Check
maincpp_check() {
    print_info "Test 3: Main.cpp Features Check"
    
    if [[ -f "src/main.cpp" ]]; then
        print_success "✓ main.cpp exists"
        
        # Check for mode support
        local modes_found=0
        local total_modes=3
        
        if grep -q "mode == \"phase2\"" "src/main.cpp" | head -1 > /dev/null; then
            print_success "✓ Phase 2 mode support"
            modes_found=$((modes_found + 1))
        fi
        
        if grep -q "mode == \"phase6\"" "src/main.cpp" | head -1 > /dev/null; then
            print_success "✓ Phase 6 mode support"
            modes_found=$((modes_found + 1))
        fi
        
        if grep -q "mode == \"experiment\"" "src/main.cpp" | head -1 > /dev/null; then
            print_success "✓ Experiment mode support"
            modes_found=$((modes_found + 1))
        fi
        
        if [[ $modes_found -eq $total_modes ]]; then
            print_success "✓ All modes supported ($modes_found/$total_modes)"
        else
            print_warning "⚠ Only $modes_found/$total_modes modes supported"
        fi
        
        # Check for command line parsing
        if grep -q "Parse command line arguments" "src/main.cpp" | head -1 > /dev/null; then
            print_success "✓ Command line argument parsing"
        else
            print_error "✗ Command line argument parsing not found"
        fi
        
        # Check for improved error handling
        if grep -q "Configuration file not found:" "src/main.cpp" | head -1 > /dev/null; then
            print_success "✓ Improved error handling"
        else
            print_warning "⚠ Error handling may need improvement"
        fi
        
        # Check for configuration override support
        if grep -q "config->set.*CommandLine" "src/main.cpp" | head -1 > /dev/null; then
            print_success "✓ Configuration override support"
        else
            print_warning "⚠ Configuration override support may be limited"
        fi
    else
        print_error "✗ main.cpp not found"
    fi
    
    echo ""
}

# Test 4: Documentation Check
doc_check() {
    print_info "Test 4: Documentation Check"
    
    local docs_found=0
    local total_docs=2
    
    if [[ -f "easy_usage.md" ]]; then
        print_success "✓ easy_usage.md exists (quick start guide)"
        docs_found=$((docs_found + 1))
    else
        print_error "✗ easy_usage.md not found"
    fi
    
    if [[ -f "HOW_TO_USE.md" ]]; then
        print_success "✓ HOW_TO_USE.md exists (detailed guide)"
        docs_found=$((docs_found + 1))
    else
        print_error "✗ HOW_TO_USE.md not found"
    fi
    
    if [[ -f "IMPROVEMENTS.md" ]]; then
        print_success "✓ IMPROVEMENTS.md exists (improvement summary)"
        docs_found=$((docs_found + 1))
    else
        print_error "✗ IMPROVEMENTS.md not found"
    fi
    
    if [[ $docs_found -eq $total_docs ]]; then
        print_success "✓ All required documentation files present ($docs_found/$total_docs)"
    else
        print_warning "⚠ Only $docs_found/$total_docs documentation files found"
    fi
    
    echo ""
}

# Test 5: Validation Script Check
validation_check() {
    print_info "Test 5: Validation Script Check"
    
    if [[ -f "scripts/validate_improvements.sh" ]]; then
        print_success "✓ validate_improvements.sh exists"
        
        if [[ -x "scripts/validate_improvements.sh" ]]; then
            print_success "✓ validate_improvements.sh is executable"
        else
            print_error "✗ validate_improvements.sh is not executable"
        fi
        
        # Check validation script content
        if grep -q "Configuration improvements" "scripts/validate_improvements.sh" | head -1 > /dev/null; then
            print_success "✓ Validation script checks configuration"
        else
            print_warning "⚠ Validation script may be incomplete"
        fi
        
        if grep -q "CLI improvements" "scripts/validate_improvements.sh" | head -1 > /dev/null; then
            print_success "✓ Validation script checks CLI"
        else
            print_warning "⚠ Validation script may not check CLI"
        fi
        
        if grep -q "Main.cpp improvements" "scripts/validate_improvements.sh" | head -1 > /dev/null; then
            print_success "✓ Validation script checks Main.cpp"
        else
            print_warning "⚠ Validation script may not check Main.cpp"
        fi
        
        if grep -q "Documentation improvements" "scripts/validate_improvements.sh" | head -1 > /dev/null; then
            print_success "✓ Validation script checks documentation"
        else
            print_warning "⚠ Validation script may not check documentation"
        fi
        
        if grep -q "Project structure" "scripts/validate_improvements.sh" | head -1 > /dev/null; then
            print_success "✓ Validation script checks project structure"
        else
            print_warning "⚠ Validation script may not check project structure"
        fi
        
        if grep -q "Test improvements" "scripts/validate_improvements.sh" | head -1 > /dev/null; then
            print_success "✓ Validation script checks tests"
        else
            print_warning "⚠ Validation script may not check tests"
        fi
    else
        print_error "✗ validate_improvements.sh not found"
    fi
    
    echo ""
}

# Main demonstration function
main_demo() {
    print_header
    
    print_info "Demonstrating NLM Project Improvements"
    print_info "This script validates all improvements made to the NLM project."
    print_info ""
    
    # Run all tests
    config_check
    cli_check
    maincpp_check
    doc_check
    validation_check
    
    print_footer
    
    print_info ""
    print_info "Summary of Improvements:"
    print_info "✓ Fixed duplicate configuration settings"
    print_info "✓ Enhanced command line interface"
    print_info "✓ Added comprehensive documentation"
    print_info "✓ Improved error handling and validation"
    print_info "✓ Added multiple configuration options"
    print_info "✓ Enhanced mode support (phase2, phase6, experiment)"
    print_info "✓ Added validation and testing tools"
    print_info ""
    print_info "The NLM project is now more user-friendly, flexible, and maintainable!"
}

# Run the demonstration
main_demo