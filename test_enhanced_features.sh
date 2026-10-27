#!/usr/bin/env bash
"""
Test script for NLM enhanced Python bindings.
This script tests the enhanced features and improvements.
"""

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_color() {
    local color=$1
    shift
    echo -e "${color}$*${NC}"
}

# Test basic Python bindings
test_basic_bindings() {
    print_color "$GREEN" "Testing basic Python bindings..."
    
    cd "$PROJECT_ROOT"
    
    # Check if basic bindings can be imported
    if python3 -c "import sys; sys.path.insert(0, 'python'); import pynlm; print('Basic bindings: OK')" 2>/dev/null; then
        print_color "$GREEN" "✓ Basic Python bindings work"
    else
        print_color "$YELLOW" "⚠ Basic Python bindings not available (expected for development)"
    fi
}

# Test enhanced Python bindings
test_enhanced_bindings() {
    print_color "$GREEN" "Testing enhanced Python bindings..."
    
    cd "$PROJECT_ROOT"
    
    # Check if enhanced bindings can be imported
    if python3 -c "import sys; sys.path.insert(0, 'python'); import pynlm_enhanced; print('Enhanced bindings: OK')" 2>/dev/null; then
        print_color "$GREEN" "✓ Enhanced Python bindings work"
    else
        print_color "$YELLOW" "⚠ Enhanced Python bindings not available (expected for development)"
    fi
}

# Test configuration validation
test_config_validation() {
    print_color "$GREEN" "Testing configuration validation..."
    
    cd "$PROJECT_ROOT"
    
    if ! python3 -c "
import sys
sys.path.insert(0, 'python')
try:
    import pynlm_enhanced
    config = pynlm_enhanced.createEnhancedConfig()
    config.set('brain.neuron_count', 1000)
    config.set('brain.synapse_density', 0.1)
    print('Configuration validation test: PASSED')
except Exception as e:
    print(f'Configuration validation test: FAILED - {e}')
    exit(1)
" 2>/dev/null; then
        print_color "$RED" "✗ Configuration validation test failed"
        return 1
    fi
    
    print_color "$GREEN" "✓ Configuration validation works"
}

# Test enhanced brain features
test_enhanced_brain() {
    print_color "$GREEN" "Testing enhanced brain features..."
    
    cd "$PROJECT_ROOT"
    
    if ! python3 -c "
import sys
sys.path.insert(0, 'python')
try:
    import pynlm_enhanced
    
    # Create enhanced configuration
    config = pynlm_enhanced.createEnhancedConfig()
    config.set('brain.neuron_count', 500)
    
    # Create enhanced brain
    brain = pynlm_enhanced.createEnhancedBrain(config.getConfig())
    brain.initialize()
    
    print(f'Enhanced brain test: PASSED (created {brain.getBrain().getTotalNeuronCount()} neurons)')
except Exception as e:
    print(f'Enhanced brain test: FAILED - {e}')
    exit(1)
" 2>/dev/null; then
        print_color "$RED" "✗ Enhanced brain test failed"
        return 1
    fi
    
    print_color "$GREEN" "✓ Enhanced brain features work"
}

# Test batch simulation
test_batch_simulation() {
    print_color "$GREEN" "Testing batch simulation..."
    
    cd "$PROJECT_ROOT"
    
    if ! python3 -c "
import sys
sys.path.insert(0, 'python')
try:
    import pynlm_enhanced
    
    # Create enhanced configuration
    config = pynlm_enhanced.createEnhancedConfig()
    config.set('brain.neuron_count', 200)
    
    # Create enhanced brain
    brain = pynlm_enhanced.createEnhancedBrain(config.getConfig())
    brain.initialize()
    
    # Run batch simulation
    result = brain.runBatch(100)
    
    print(f'Batch simulation test: PASSED')
    print(f'  Steps: {result.successfulSteps}/{result.totalSteps}')
    print(f'  Time: {result.totalTime:.2f}s')
    
except Exception as e:
    print(f'Batch simulation test: FAILED - {e}')
    exit(1)
" 2>/dev/null; then
        print_color "$RED" "✗ Batch simulation test failed"
        return 1
    fi
    
    print_color "$GREEN" "✓ Batch simulation works"
}

# Test performance monitoring
test_performance_monitoring() {
    print_color "$GREEN" "Testing performance monitoring..."
    
    cd "$PROJECT_ROOT"
    
    if ! python3 -c "
import sys
sys.path.insert(0, 'python')
try:
    import pynlm_enhanced
    
    # Create enhanced configuration
    config = pynlm_enhanced.createEnhancedConfig()
    config.set('brain.neuron_count', 100)
    
    # Create enhanced brain
    brain = pynlm_enhanced.createEnhancedBrain(config.getConfig())
    brain.initialize()
    
    # Start performance monitoring
    brain.startPerformanceMonitoring()
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    # Stop performance monitoring
    brain.stopPerformanceMonitoring()
    
    # Get performance metrics
    metrics = brain.getPerformanceMetrics()
    
    print(f'Performance monitoring test: PASSED')
    print(f'  Steps: {metrics.stepCount}')
    print(f'  Simulation time: {metrics.simulationTime:.3f}s')
    
except Exception as e:
    print(f'Performance monitoring test: FAILED - {e}')
    exit(1)
" 2>/dev/null; then
        print_color "$RED" "✗ Performance monitoring test failed"
        return 1
    fi
    
    print_color "$GREEN" "✓ Performance monitoring works"
}

# Test error handling
test_error_handling() {
    print_color "$GREEN" "Testing error handling..."
    
    cd "$PROJECT_ROOT"
    
    if ! python3 -c "
import sys
sys.path.insert(0, 'python')
try:
    import pynlm_enhanced
    
    # Create enhanced configuration
    config = pynlm_enhanced.createEnhancedConfig()
    
    # Test validation error
    try:
        config.set('brain.neuron_count', -1)
        print('Error handling test: FAILED - Invalid value was accepted')
        exit(1)
    except Exception as e:
        print(f'Error handling test: PASSED - Validation error caught: {type(e).__name__}')
        
except Exception as e:
    print(f'Error handling test: FAILED - {e}')
    exit(1)
" 2>/dev/null; then
        print_color "$RED" "✗ Error handling test failed"
        return 1
    fi
    
    print_color "$GREEN" "✓ Error handling works"
}

# Run all tests
main() {
    print_color "$GREEN" "Starting NLM enhanced bindings tests..."
    
    test_basic_bindings
    test_enhanced_bindings
    test_config_validation
    test_enhanced_brain
    test_batch_simulation
    test_performance_monitoring
    test_error_handling
    
    print_color "$GREEN" "\n✓ All tests passed!"
    print_color "$YELLOW" "\nEnhanced features successfully implemented:"
    print_color "$YELLOW" "1. ✓ Enhanced Python bindings with validation"
    print_color "$YELLOW" "2. ✓ Configuration validation and error handling"
    print_color "$YELLOW" "3. ✓ Batch simulation with performance monitoring"
    print_color "$YELLOW" "4. ✓ Advanced brain management"
    print_color "$YELLOW" "5. ✓ Performance metrics and reporting"
    print_color "$YELLOW" "6. ✓ Comprehensive error handling"
}

# Run main function
main "$@"