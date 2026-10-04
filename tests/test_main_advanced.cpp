// Comprehensive NLM Test Suite
// Phase 6: Integrated Artificial Brain Tests
// 
// This test suite provides comprehensive coverage of all NLM systems:
// - Core neural computation (LIF neurons, synaptic dynamics)
// - Memory systems (working, episodic, associative)
// - Prediction systems
// - Cognition systems (planning, concept formation, attention)
// - Development systems
// - Neuromodulation systems (dopamine, curiosity, novelty)
// - Plasticity systems (STDP, Hebbian, structural)
// - Integration and cross-system coordination
// - Stress testing and performance benchmarks
// - Memory usage analysis
// - Configuration and edge case testing

#include <iostream>
#include <cassert>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

// Forward declarations
namespace test_integration {
    void runAll();
    void runMemoryTests();
    void runNeuromodulationTests();
    void runPredictionTests();
    void runCognitionTests();
    void runDevelopmentTests();
    void runPlasticityTests();
    void runStressTests();
    void runPerformanceBenchmarks();
}

namespace test_advanced {
    void runAll();
    void runPythonBindingTests();
    void runConfigurationTests();
    void runCheckpointTests();
    void runExperimentTests();
}

namespace test_stress {
    void runAll();
    void runLargeScaleTests();
    void runMemoryLeakTests();
    void runPerformanceRegressionTests();
}

namespace test_edge_cases {
    void runAll();
    void runBoundaryConditionTests();
    void runErrorConditionTests();
    void runConfigurationEdgeCases();
}

int main() {
    std::cout << "=== NLM Phase 6 Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing Integrated Artificial Brain Systems:" << std::endl;
    std::cout << "  - Core neural computation and dynamics" << std::endl;
    std::cout << "  - Memory system integration" << std::endl;
    std::cout << "  - Neuromodulation and learning" << std::endl;
    std::cout << "  - Prediction and cognition systems" << std::endl;
    std::cout << "  - Development and structural plasticity" << std::endl;
    std::cout << "  - Cross-system integration and coordination" << std::endl;
    std::cout << "  - Advanced Python bindings and CLI" << std::endl;
    std::cout << "  - Stress testing and performance benchmarks" << std::endl;
    std::cout << "  - Edge case and error condition testing" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Run Phase 6 integration tests
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    std::cout << "Running Memory System tests..." << std::endl;
    try {
        test_integration::runMemoryTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuromodulation tests..." << std::endl;
    try {
        test_integration::runNeuromodulationTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Prediction System tests..." << std::endl;
    try {
        test_integration::runPredictionTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Cognition System tests..." << std::endl;
    try {
        test_integration::runCognitionTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Development System tests..." << std::endl;
    try {
        test_integration::runDevelopmentTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Plasticity System tests..." << std::endl;
    try {
        test_integration::runPlasticityTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run advanced tests
    std::cout << "=== Advanced Functionality Tests ===" << std::endl;
    
    std::cout << "Running Python Binding tests..." << std::endl;
    try {
        test_advanced::runPythonBindingTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Configuration tests..." << std::endl;
    try {
        test_advanced::runConfigurationTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Checkpoint tests..." << std::endl;
    try {
        test_advanced::runCheckpointTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Experiment tests..." << std::endl;
    try {
        test_advanced::runExperimentTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run stress tests
    std::cout << "=== Stress Testing ===" << std::endl;
    
    std::cout << "Running Large Scale tests..." << std::endl;
    try {
        test_stress::runLargeScaleTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Memory Leak tests..." << std::endl;
    try {
        test_stress::runMemoryLeakTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Performance Regression tests..." << std::endl;
    try {
        test_stress::runPerformanceRegressionTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run edge case tests
    std::cout << "=== Edge Case Testing ===" << std::endl;
    
    std::cout << "Running Boundary Condition tests..." << std::endl;
    try {
        test_edge_cases::runBoundaryConditionTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Error Condition tests..." << std::endl;
    try {
        test_edge_cases::runErrorConditionTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Configuration Edge Cases..." << std::endl;
    try {
        test_edge_cases::runConfigurationEdgeCases();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run performance benchmarks
    std::cout << "=== Performance Benchmarks ===" << std::endl;
    
    std::cout << "Running Performance Benchmarks..." << std::endl;
    try {
        test_integration::runPerformanceBenchmarks();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        std::cout << "NLM Phase 6 integrated artificial brain is fully functional." << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        std::cout << "Please review failed tests and fix any issues." << std::endl;
        return 1;
    }
}