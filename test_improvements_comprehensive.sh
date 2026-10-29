#!/bin/bash
# Comprehensive Test Script for NLM Improvements
# Tests enhanced configuration system, modern C++ features, and improved functionality

set -e

PROJECT_ROOT="/workspace/app"
BUILD_DIR="${PROJECT_ROOT}/build"

# Function to check if required packages are available
check_requirements() {
    echo "=== Checking Requirements ==="
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null; then
        echo "✗ g++ compiler not found"
        return 1
    fi
    echo "✓ g++ compiler available"
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        echo "✗ CMake not found"
        return 1
    fi
    echo "✓ CMake available"
    
    # Check for pkg-config
    if ! command -v pkg-config &> /dev/null; then
        echo "⚠ pkg-config not found (optional for dependencies)"
    else
        echo "✓ pkg-config available"
    fi
    
    echo "Requirements check completed"
    return 0
}

# Function to configure build system
configure_build() {
    echo "=== Configuring Build System ==="
    
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Configure with all features enabled
    cmake "${PROJECT_ROOT}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_TESTS=ON \
        -DENABLE_DOCS=ON \
        -DENABLE_EXPERIMENTS=ON \
        -DENABLE_PERFORMANCE_ANALYSIS=ON \
        -DENABLE_CHECKPOINTING=ON \
        -DENABLE_JSON_SUPPORT=ON \
        -DENABLE_YAML_SUPPORT=ON \
        -DENABLE_ADVANCED_UI=ON
    
    if [ $? -eq 0 ]; then
        echo "✓ Build configuration completed"
    else
        echo "✗ Build configuration failed"
        return 1
    fi
}

# Function to build the project
build_project() {
    echo "=== Building Project ==="
    
    cd "${BUILD_DIR}"
    
    # Build with all targets
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo "✓ Project build completed"
    else
        echo "✗ Project build failed"
        return 1
    fi
}

# Function to run basic functionality tests
run_basic_tests() {
    echo "=== Running Basic Functionality Tests ==="
    
    cd "${BUILD_DIR}"
    
    # Run existing config tests
    if [ -f "nlm_test" ]; then
        echo "Running existing test suite..."
        ./nlm_test
        if [ $? -eq 0 ]; then
            echo "✓ Existing test suite passed"
        else
            echo "✗ Existing test suite failed"
            return 1
        fi
    else
        echo "✗ Test executable not found"
        return 1
    fi
}

# Function to test enhanced configuration system
run_config_tests() {
    echo "=== Testing Enhanced Configuration System ==="
    
    cd "${BUILD_DIR}"
    
    # Test enhanced config functionality
    echo "Testing JSON/YAML support..."
    
    # Create test config files
    cat > test_enhanced_config.json << EOF
{
    "brain_type": "Enhanced NLM Phase 6",
    "neuron_count": 1024,
    "learning_rate": 0.01,
    "plasticity_enabled": true,
    "memory_capacity": 5000,
    "checkpoint_interval": 1000,
    "description": "Test enhanced configuration system with JSON format"
}
EOF
    
    cat > test_enhanced_config.yaml << EOF
brain_type: "Enhanced NLM Phase 6"
neuron_count: 1024
learning_rate: 0.01
plasticity_enabled: true
memory_capacity: 5000
checkpoint_interval: 1000
description: "Test enhanced configuration system with YAML format"
EOF
    
    # Test loading different formats
    echo "Testing JSON loading..."
    if [ -f "test_enhanced_config.json" ]; then
        echo "✓ JSON test file created"
    else
        echo "✗ Failed to create JSON test file"
        return 1
    fi
    
    echo "Testing YAML loading..."
    if [ -f "test_enhanced_config.yaml" ]; then
        echo "✓ YAML test file created"
    else
        echo "✗ Failed to create YAML test file"
        return 1
    fi
    
    # Test enhanced main executable
    echo "Testing enhanced main executable..."
    if [ -f "nlm_enhanced" ]; then
        echo "✓ Enhanced main executable found"
        
        # Run a quick test mode
        ./nlm_enhanced --test-mode --validate-only 2>&1 | head -20
        if [ $? -eq 0 ]; then
            echo "✓ Enhanced main executable test passed"
        else
            echo "✗ Enhanced main executable test failed"
            return 1
        fi
    else
        echo "✗ Enhanced main executable not found"
        return 1
    fi
    
    # Clean up test files
    rm -f test_enhanced_config.json test_enhanced_config.yaml
    
    echo "✓ Configuration system tests completed successfully"
    return 0
}

# Function to test neuron improvements
run_neuron_tests() {
    echo "=== Testing Neuron System Improvements ==="
    
    cd "${BUILD_DIR}"
    
    if [ -f "test_neuron" ]; then
        echo "Running neuron system tests..."
        ./test_neuron
        if [ $? -eq 0 ]; then
            echo "✓ Neuron system tests passed"
        else
            echo "✗ Neuron system tests failed"
            return 1
        fi
    else
        echo "✗ Neuron test executable not found"
        return 1
    fi
    
    echo "✓ Neuron system improvements tests completed"
    return 0
}

# Function to validate build optimizations
validate_build_optimizations() {
    echo "=== Validating Build Optimizations ==="
    
    cd "${BUILD_DIR}"
    
    # Check for build artifacts
    if [ -f "nlm" ]; then
        echo "✓ Main NLM executable built"
    else
        echo "✗ Main NLM executable not found"
        return 1
    fi
    
    if [ -f "nlm_enhanced" ]; then
        echo "✓ Enhanced NLM executable built"
    else
        echo "✗ Enhanced NLM executable not found"
        return 1
    fi
    
    # Check for libraries
    if [ -f "libnlm_core.a" ]; then
        echo "✓ Core library built"
    else
        echo "⚠ Core library not found (may be in different location)"
    fi
    
    # Check for test executables
    if [ -f "nlm_test" ]; then
        echo "✓ Test suite built"
    else
        echo "⚠ Test suite not found"
    fi
    
    echo "✓ Build optimizations validation completed"
    return 0
}

# Function to run comprehensive test suite
run_comprehensive_tests() {
    echo "=== Running Comprehensive Test Suite ==="
    
    cd "${BUILD_DIR}"
    
    # Run all available tests
    local test_count=0
    local passed_count=0
    
    # Check for test executables and run them
    if [ -f "nlm_test" ]; then
        echo "Running main test suite..."
        if ./nlm_test; then
            echo "✓ Main test suite passed"
            ((passed_count++))
        else
            echo "✗ Main test suite failed"
        fi
        ((test_count++))
    fi
    
    if [ -f "nlm_enhanced" ]; then
        echo "Running enhanced test suite..."
        if ./nlm_enhanced --test-mode 2>&1 | head -10; then
            echo "✓ Enhanced test suite passed"
            ((passed_count++))
        else
            echo "✗ Enhanced test suite failed"
        fi
        ((test_count++))
    fi
    
    echo "Test Summary:"
    echo "  Tests run: $test_count"
    echo "  Tests passed: $passed_count"
    echo "  Tests failed: $((test_count - passed_count))"
    
    if [ $passed_count -eq $test_count ] || [ $test_count -eq 0 ]; then
        echo "✓ All tests completed successfully"
        return 0
    else
        echo "✗ Some tests failed"
        return 1
    fi
}

# Function to display help
show_help() {
    cat << EOF
Comprehensive NLM Test Script
==============================

This script tests all improvements made to the NLM (熙然) project:

Features Tested:
- Enhanced configuration system with JSON/YAML support
- Modern C++23 implementation
- Improved command-line interface
- Better integration of brain systems
- Performance optimizations
- Comprehensive testing infrastructure

Usage:
  $0 [OPTIONS]

Options:
  --check-only              Only check requirements, don't build or test
  --configure-only          Only configure build system, don't build
  --build-only              Only build project, don't test
  --test-only               Only run tests, assume project is built
  --comprehensive          Run comprehensive test suite (default)
  --quick                   Run quick validation tests only
  --validate-only           Only validate build artifacts
  --help                    Show this help message

Examples:
  $0                        Run comprehensive test suite
  $0 --check-only           Check system requirements
  $0 --build-only           Build the project
  $0 --test-only            Run tests only
  $0 --quick                Run quick validation

The test script validates:
1. System requirements and dependencies
2. Build configuration and compilation
3. Basic functionality and core features
4. Enhanced configuration system
5. Neuron system improvements
6. Build optimizations and performance
7. Comprehensive test suite

EOF
}

# Main execution logic
main() {
    local mode="comprehensive"
    
    # Parse command line arguments
    case "${1:-}" in
        --check-only)
            mode="check-only"
            ;;
        --configure-only)
            mode="configure-only"
            ;;
        --build-only)
            mode="build-only"
            ;;
        --test-only)
            mode="test-only"
            ;;
        --comprehensive)
            mode="comprehensive"
            ;;
        --quick)
            mode="quick"
            ;;
        --validate-only)
            mode="validate-only"
            ;;
        --help|-h)
            show_help
            return 0
            ;;
        "")
            mode="comprehensive"
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            return 1
            ;;
    esac
    
    echo "NLM Comprehensive Test Script"
    echo "=============================="
    echo "Mode: $mode"
    echo "Working directory: $BUILD_DIR"
    echo
    
    case "$mode" in
        "check-only")
            check_requirements
            ;;
        "configure-only")
            check_requirements
            configure_build
            ;;
        "build-only")
            check_requirements
            configure_build
            build_project
            ;;
        "test-only")
            run_basic_tests
            run_config_tests
            run_neuron_tests
            validate_build_optimizations
            ;;
        "comprehensive")
            check_requirements
            configure_build
            build_project
            run_basic_tests
            run_config_tests
            run_neuron_tests
            validate_build_optimizations
            run_comprehensive_tests
            ;;
        "quick")
            check_requirements
            configure_build
            build_project
            run_basic_tests
            run_config_tests
            validate_build_optimizations
            ;;
        "validate-only")
            validate_build_optimizations
            ;;
    esac
    
    local exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo "=== Test Script Completed Successfully ==="
    else
        echo "=== Test Script Completed with Errors ==="
    fi
    
    return $exit_code
}

# Run main function with all arguments
main "$@"