#!/bin/bash
# NLM Test Suite Runner
# Comprehensive test suite for NLM (熙然) neural learning machine

# This script runs all available tests for the NLM system

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/tests"
BUILD_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/build"
MAIN_EXECUTABLE="nlm"

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if build exists
check_build() {
    if [[ ! -f "$BUILD_DIR/$MAIN_EXECUTABLE" ]]; then
        log_error "Executable not found: $BUILD_DIR/$MAIN_EXECUTABLE"
        log_info "Building NLM..."
        cd "$BUILD_DIR"
        if ! cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4; then
            log_error "Build failed"
            return 1
        fi
        cd - > /dev/null
    fi
}

# Run all tests
run_all_tests() {
    log_info "=== NLM Test Suite ==="
    log_info "Running comprehensive tests for NLM neural learning machine"
    log_info "Test directory: $TEST_DIR"
    log_info "Build directory: $BUILD_DIR"
    log_info ""
    
    check_build
    
    # Run brain tests
    log_info "Running Brain tests..."
    if [[ -f "$BUILD_DIR/tests" ]]; then
        "$BUILD_DIR/tests"
        if [[ $? -eq 0 ]]; then
            log_info "Brain tests: PASSED"
        else
            log_error "Brain tests: FAILED"
            return 1
        fi
    else
        log_error "Brain tests executable not found"
        return 1
    fi
    
    log_info ""
    log_info "=== All Tests Complete ==="
}

# Run specific test category
run_category_tests() {
    local category="$1"
    
    case "$category" in
        brain)
            log_info "Running Brain tests..."
            "$BUILD_DIR/tests"
            ;;
        cli)
            log_info "Running CLI tests..."
            # TODO: Add CLI test runner
            log_warn "CLI tests not yet implemented"
            ;;
        integration)
            log_info "Running Integration tests..."
            # TODO: Add integration test runner
            log_warn "Integration tests not yet implemented"
            ;;
        all)
            run_all_tests
            ;;
        *)
            log_error "Unknown test category: $category"
            log_info "Available categories: brain, cli, integration, all"
            return 1
            ;;
    esac
}

# Run experiment tests
run_experiment_tests() {
    log_info "=== NLM Experiment Tests ==="
    log_info "Testing different experiment types..."
    log_info ""
    
    # Test connectivity experiment
    log_info "Running connectivity experiment..."
    "$BUILD_DIR/$MAIN_EXECUTABLE" run connectivity --help 2>&1 | grep -q "connectivity" && \
    log_info "Connectivity experiment: AVAILABLE" || \
    log_error "Connectivity experiment: FAILED"
    
    # Test plasticity experiment
    log_info "Running plasticity experiment..."
    "$BUILD_DIR/$MAIN_EXECUTABLE" run plasticity --help 2>&1 | grep -q "plasticity" && \
    log_info "Plasticity experiment: AVAILABLE" || \
    log_error "Plasticity experiment: FAILED"
    
    # Test STDP experiment
    log_info "Running STDP experiment..."
    "$BUILD_DIR/$MAIN_EXECUTABLE" run stdp --help 2>&1 | grep -q "stdp" && \
    log_info "STDP experiment: AVAILABLE" || \
    log_error "STDP experiment: FAILED"
    
    # Test learning experiment
    log_info "Running learning experiment..."
    "$BUILD_DIR/$MAIN_EXECUTABLE" run learning --help 2>&1 | grep -q "learning" && \
    log_info "Learning experiment: AVAILABLE" || \
    log_error "Learning experiment: FAILED"
    
    # Test demo experiment
    log_info "Running demo experiment..."
    "$BUILD_DIR/$MAIN_EXECUTABLE" run demo --help 2>&1 | grep -q "demo" && \
    log_info "Demo experiment: AVAILABLE" || \
    log_error "Demo experiment: FAILED"
    
    log_info ""
    log_info "=== Experiment Tests Complete ==="
}

# Run documentation tests
'test-documentation'
    echo "# NLM Test Documentation" > "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "## NLM Test Suite Documentation" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "This document describes the test suite for NLM (熙然) neural learning machine." >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "### Available Tests" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "1. **Brain Tests** (`./tests`):" >> "test_documentation.md"
    echo "   - Unit tests for core brain functionality" >> "test_documentation.md"
    echo "   - Test neuron dynamics" >> "test_documentation.md"
    echo "   - Test synaptic transmission" >> "test_documentation.md"
    echo "   - Test STDP plasticity" >> "test_documentation.md"
    echo "   - Test brain initialization and configuration" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "2. **CLI Tests** (`./tests/test_cli.cpp`):" >> "test_documentation.md"
    echo "   - Test command-line interface" >> "test_documentation.md"
    echo "   - Test help system" >> "test_documentation.md"
    echo "   - Test experiment commands" >> "test_documentation.md"
    echo "   - Test script execution" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "3. **Integration Tests** (planned):" >> "test_documentation.md"
    echo "   - Test multiple experiment combinations" >> "test_documentation.md"
    echo "   - Test cross-experiment consistency" >> "test_documentation.md"
    echo "   - Test real-world scenarios" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "### Test Execution" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "#### Run All Tests" >> "test_documentation.md"
    echo "```bash" >> "test_documentation.md"
    echo "# From project root" >> "test_documentation.md"
    echo "./run_tests.sh" >> "test_documentation.md"
    echo "```" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "#### Run Brain Tests" >> "test_documentation.md"
    echo "```bash" >> "test_documentation.md"
    echo "./build/tests" >> "test_documentation.md"
    echo "```" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "#### Run Experiment Tests" >> "test_documentation.md"
    echo "```bash" >> "test_documentation.md"
    echo "./run_tests.sh experiment" >> "test_documentation.md"
    echo "```" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "### Test Coverage" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "The current test suite covers:" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "- **Core Functionality**: Neuron dynamics, synaptic transmission, plasticity" >> "test_documentation.md"
    echo "- **Integration**: Brain initialization, configuration, multi-region systems" >> "test_documentation.md"
    echo "- **Interface**: Command-line interface, help system, experiment management" >> "test_documentation.md"
    echo "- **Performance**: Real-time simulation, event-driven processing" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "### Test Metrics" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "Tests measure:" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "- **Functional Correctness**: Brain initialization, simulation accuracy" >> "test_documentation.md"
    echo "- **Performance**: Processing speed, memory efficiency" >> "test_documentation.md"
    echo "- **Reliability**: Consistency across runs, error conditions" >> "test_documentation.md"
    echo "- **User Experience**: Interface usability, command help" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "### Test Limitations" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "Current limitations:" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "- **Test Coverage**: Some edge cases not fully covered" >> "test_documentation.md"
    echo "- **Performance Testing**: Limited real-world scenario testing" >> "test_documentation.md"
    echo "- **Integration Testing**: Cross-component interactions not fully tested" >> "test_documentation.md"
    echo "- **Regression Testing**: Automated regression testing not implemented" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    echo "### Test Best Practices" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "For developers and researchers:" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    echo "1. **Always include control groups** in experiment tests" >> "test_documentation.md"
    echo "2. **Run tests multiple times** to ensure consistency" >> "test_documentation.md"
    echo "3. **Document test parameters** and assumptions" >> "test_documentation.md"
    echo "4. **Use statistical analysis** for experimental results" >> "test_documentation.md"
    echo "5. **Maintain test isolation** to prevent interference" >> "test_documentation.md"
    echo "" >> "test_documentation.md"
    
    log_info "Documentation created: test_documentation.md"
}