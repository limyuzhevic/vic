// NLM Test Main
// Comprehensive Test Suite for NLM Framework
// Phase 2: Real Neural Computation Tests and Phase 6 Integration Tests
// Phase 6: New Functionality Validation Tests

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

namespace phase6_tests {
    void runAllPhase6Tests();
}

// New Test Suites for Phase 6 Functionality

// Python Bindings Tests
namespace python_bindings_tests {
    void runAll();
}

// Python API Tests
namespace python_api_tests {
    void runAll();
}

// Python Performance Tests
namespace python_performance_tests {
    void runAll();
}

// Unified API Tests
namespace api_consistency_tests {
    void runAll();
}

namespace api_integration_tests {
    void runAll();
}

namespace api_backward_compat_tests {
    void runAll();
}

namespace api_parameter_validation_tests {
    void runAll();
}

// Module Refactoring Tests
namespace module_cohesion_tests {
    void runAll();
}

namespace code_duplication_tests {
    void runAll();
}

namespace module_integration_tests {
    void runAll();
}

namespace module_maintenance_tests {
    void runAll();
}

// Error Handling Tests
namespace error_hierarchy_tests {
    void runAll();
}

namespace error_recovery_tests {
    void runAll();
}

namespace error_performance_tests {
    void runAll();
}

namespace error_integration_tests {
    void runAll();
}

// Documentation Tests
namespace documentation_completeness_tests {
    void runAll();
}

namespace code_documentation_tests {
    void runAll();
}

namespace api_documentation_tests {
    void runAll();
}

namespace example_validation_tests {
    void runAll();
}

// Advanced Features Tests
namespace advanced_config_tests {
    void runAll();
}

namespace advanced_simulation_tests {
    void runAll();
}

namespace network_analysis_tests {
    void runAll();
}

namespace research_feature_tests {
    void runAll();
}

// Performance Optimization Tests
namespace simd_optimization_tests {
    void runAll();
}

namespace parallel_processing_tests {
    void runAll();
}

namespace memory_optimization_tests {
    void runAll();
}

namespace event_driven_tests {
    void runAll();
}

// Phase 6 Integration Tests
namespace phase6_demo_integration_tests {
    void runAll();
}

namespace phase6_integrated_experiment_tests {
    void runAll();
}

namespace phase6_backward_compatibility_tests {
    void runAll();
}

namespace phase6_performance_tests {
    void runAll();
}

int main() {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing Real Neural Computation (Phase 2) and All Phase 6 New Functionality:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << "  - Phase 6 Integration and Agent Systems" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Phase 2: Core System Tests ===" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Types tests..." << std::endl;
    try {
        test_types::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Config tests..." << std::endl;
    try {
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Random tests..." << std::endl;
    try {
        test_random::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Clock tests..." << std::endl;
    try {
        test_clock::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuron tests..." << std::endl;
    try {
        test_neuron::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Synapse tests..." << std::endl;
    try {
        test_synapse::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running STDP tests..." << std::endl;
    try {
        test_stdp::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain tests..." << std::endl;
    try {
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Phase 6 Integration Tests ===" << std::endl;
    try {
        phase6_tests::runAllPhase6Tests();
        std::cout << "Phase 6 Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Phase 6 Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running New Python Bindings Tests ===" << std::endl;
    try {
        python_bindings_tests::runAll();
        std::cout << "Python Bindings Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Python Bindings Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running New Python API Tests ===" << std::endl;
    try {
        python_api_tests::runAll();
        std::cout << "Python API Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Python API Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running New Python Performance Tests ===" << std::endl;
    try {
        python_performance_tests::runAll();
        std::cout << "Python Performance Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Python Performance Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Unified API Tests ===" << std::endl;
    try {
        api_consistency_tests::runAll();
        std::cout << "API Consistency Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "API Consistency Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        api_integration_tests::runAll();
        std::cout << "API Integration Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "API Integration Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        api_backward_compat_tests::runAll();
        std::cout << "API Backward Compatibility Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "API Backward Compatibility Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        api_parameter_validation_tests::runAll();
        std::cout << "API Parameter Validation Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "API Parameter Validation Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Module Refactoring Tests ===" << std::endl;
    try {
        module_cohesion_tests::runAll();
        std::cout << "Module Cohesion Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Module Cohesion Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        code_duplication_tests::runAll();
        std::cout << "Code Duplication Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Code Duplication Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        module_integration_tests::runAll();
        std::cout << "Module Integration Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Module Integration Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        module_maintenance_tests::runAll();
        std::cout << "Module Maintenance Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Module Maintenance Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Error Handling Tests ===" << std::endl;
    try {
        error_hierarchy_tests::runAll();
        std::cout << "Error Hierarchy Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error Hierarchy Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        error_recovery_tests::runAll();
        std::cout << "Error Recovery Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error Recovery Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        error_performance_tests::runAll();
        std::cout << "Error Performance Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error Performance Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        error_integration_tests::runAll();
        std::cout << "Error Integration Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error Integration Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Documentation Tests ===" << std::endl;
    try {
        documentation_completeness_tests::runAll();
        std::cout << "Documentation Completeness Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Documentation Completeness Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        code_documentation_tests::runAll();
        std::cout << "Code Documentation Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Code Documentation Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        api_documentation_tests::runAll();
        std::cout << "API Documentation Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "API Documentation Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        example_validation_tests::runAll();
        std::cout << "Example Validation Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Example Validation Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Advanced Features Tests ===" << std::endl;
    try {
        advanced_config_tests::runAll();
        std::cout << "Advanced Config Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Advanced Config Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        advanced_simulation_tests::runAll();
        std::cout << "Advanced Simulation Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Advanced Simulation Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        network_analysis_tests::runAll();
        std::cout << "Network Analysis Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Network Analysis Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        research_feature_tests::runAll();
        std::cout << "Research Feature Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Research Feature Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Performance Optimization Tests ===" << std::endl;
    try {
        simd_optimization_tests::runAll();
        std::cout << "SIMD Optimization Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "SIMD Optimization Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        parallel_processing_tests::runAll();
        std::cout << "Parallel Processing Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Parallel Processing Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        memory_optimization_tests::runAll();
        std::cout << "Memory Optimization Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Memory Optimization Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        event_driven_tests::runAll();
        std::cout << "Event-Driven Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Event-Driven Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Running Phase 6 Integration Tests ===" << std::endl;
    try {
        phase6_demo_integration_tests::runAll();
        std::cout << "Phase 6Demo Integration Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Phase 6Demo Integration Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        phase6_integrated_experiment_tests::runAll();
        std::cout << "Phase6IntegratedExperiment Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Phase6IntegratedExperiment Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        phase6_backward_compatibility_tests::runAll();
        std::cout << "Phase 6 Backward Compatibility Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Phase 6 Backward Compatibility Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    try {
        phase6_performance_tests::runAll();
        std::cout << "Phase 6 Performance Tests: PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Phase 6 Performance Tests: FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "✓ Phase 2: Core System Tests" << std::endl;
        std::cout << "✓ Phase 6: Python Bindings Tests" << std::endl;
        std::cout << "✓ Phase 6: Python API Tests" << std::endl;
        std::cout << "✓ Phase 6: Python Performance Tests" << std::endl;
        std::cout << "✓ Phase 6: Unified API Tests" << std::endl;
        std::cout << "✓ Phase 6: Module Refactoring Tests" << std::endl;
        std::cout << "✓ Phase 6: Error Handling Tests" << std::endl;
        std::cout << "✓ Phase 6: Documentation Tests" << std::endl;
        std::cout << "✓ Phase 6: Advanced Features Tests" << std::endl;
        std::cout << "✓ Phase 6: Performance Optimization Tests" << std::endl;
        std::cout << "✓ Phase 6: Phase 6 Integration Tests" << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        return 1;
    }
}
