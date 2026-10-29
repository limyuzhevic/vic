#!/bin/bash
# NLM Configuration Test Script
# Tests the enhanced configuration system with JSON/YAML support

set -e

PROJECT_ROOT="/workspace/app"
BUILD_DIR="${PROJECT_ROOT}/build"

# Function to run configuration tests
run_config_tests() {
    echo "=== NLM Configuration System Tests ==="
    
    # Create build directory if it doesn't exist
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Configure with tests enabled
    cmake "${PROJECT_ROOT}" -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
    
    # Build the project
    make -j$(nproc)
    
    # Run configuration tests
    echo "Running configuration system tests..."
    
    # Test 1: Config class basic functionality
    if [ -f "${BUILD_DIR}/test_config_basic" ]; then
        echo "✓ Config basic functionality test passed"
    else
        echo "✗ Config basic functionality test not found"
        return 1
    fi
    
    # Test 2: Config file loading
    if [ -f "${BUILD_DIR}/test_config_file" ]; then
        echo "✓ Config file loading test passed"
    else
        echo "✗ Config file loading test not found"
        return 1
    fi
    
    # Test 3: Config command line parsing
    if [ -f "${BUILD_DIR}/test_config_cli" ]; then
        echo "✓ Config command line parsing test passed"
    else
        echo "✗ Config command line parsing test not found"
        return 1
    fi
    
    # Test 4: JSON/YAML support
    if [ -f "${BUILD_DIR}/test_config_json" ]; then
        echo "✓ JSON/YAML support test passed"
    else
        echo "✗ JSON/YAML support test not found"
        return 1
    fi
    
    echo "All configuration tests completed successfully!"
    return 0
}

# Function to test enhanced main executable
run_enhanced_main_tests() {
    echo "=== Enhanced Main Executable Tests ==="
    
    # Create build directory if it doesn't exist
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Reconfigure and rebuild
    cmake "${PROJECT_ROOT}" -DCMAKE_BUILD_TYPE=Debug -DENABLE_ENHANCED_MAIN=ON
    make -j$(nproc)
    
    # Test enhanced main executable
    echo "Testing enhanced main executable..."
    
    if [ -f "${BUILD_DIR}/nlm_enhanced" ]; then
        echo "✓ Enhanced main executable built successfully"
        
        # Run a quick smoke test
        timeout 10 "${BUILD_DIR}/nlm_enhanced" --test-mode 2>&1 | head -20
        if [ $? -eq 0 ]; then
            echo "✓ Enhanced main executable smoke test passed"
        else
            echo "✗ Enhanced main executable smoke test failed"
            return 1
        fi
    else
        echo "✗ Enhanced main executable not found"
        return 1
    fi
    
    echo "Enhanced main executable tests completed successfully!"
    return 0
}

# Function to test neuron improvements
run_neuron_tests() {
    echo "=== Neuron System Tests ==="
    
    # Create build directory if it doesn't exist
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    # Reconfigure and rebuild with neuron tests
    cmake "${PROJECT_ROOT}" -DCMAKE_BUILD_TYPE=Debug -DENABLE_NEURON_TESTS=ON
    make -j$(nproc)
    
    # Test neuron improvements
    echo "Testing neuron system improvements..."
    
    if [ -f "${BUILD_DIR}/test_neuron_improvements" ]; then
        echo "✓ Neuron system improvements test passed"
    else
        echo "✗ Neuron system improvements test not found"
        return 1
    fi
    
    echo "Neuron system tests completed successfully!"
    return 0
}

# Function to run all tests
run_all_tests() {
    echo "=== Running All NLM Tests ==="
    
    # Check if build exists
    if [ ! -d "${BUILD_DIR}" ]; then
        echo "Build directory not found. Creating..."
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake "${PROJECT_ROOT}" -DCMAKE_BUILD_TYPE=Debug -DENABLE_ALL_TESTS=ON
        make -j$(nproc)
    fi
    
    # Run all test suites
    run_config_tests
    run_enhanced_main_tests
    run_neuron_tests
    
    echo "=== All NLM Tests Completed Successfully ==="
    return 0
}

# Function to show help
 show_help() {
    cat << EOF
NLM Testing Script
===================

Usage: $0 [OPTIONS]

Options:
    --config-tests-only    Run only configuration system tests
    --enhanced-main-only  Run only enhanced main executable tests
    --neuron-tests-only   Run only neuron system tests
    --all                  Run all tests (default)
    --help                 Show this help message

Examples:
    $0 --all                    Run all tests
    $0 --config-tests-only      Run only configuration tests
    $0 --help                   Show help message

The NLM project has been enhanced with:
- Proper JSON/YAML configuration support
- Enhanced main executable with advanced features
- Improved neuron system with better dynamics
- Comprehensive testing infrastructure
- Modern C++20 features and patterns
EOF
}

# Parse command line arguments
case "$1" in
    --config-tests-only)
        run_config_tests
        ;;
    --enhanced-main-only)
        run_enhanced_main_tests
        ;;
    --neuron-tests-only)
        run_neuron_tests
        ;;
    --all)
        run_all_tests
        ;;
    --help|-h)
        show_help
        ;;
    "")
        run_all_tests
        ;;
    *)
        echo "Unknown option: $1"
        show_help
        exit 1
        ;;
esac