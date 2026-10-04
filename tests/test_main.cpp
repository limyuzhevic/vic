// NLM Test Main
// Phase 6: Comprehensive Integrated Brain Tests
// 
// This test suite extends the original Phase 2 tests with Phase 6 integration tests,
// including comprehensive memory systems, neuromodulation, prediction, cognition,
// development, and advanced functionality tests.

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

// Phase 6 integration tests
namespace test_integration {
    void runMemoryTests();
    void runNeuromodulationTests();
    void runPredictionTests();
    void runCognitionTests();
    void runDevelopmentTests();
    void runPlasticityTests();
    void runPerformanceBenchmarks();
}

// Advanced functionality tests
namespace test_advanced {
    void runPythonBindingTests();
    void runConfigurationTests();
    void runCheckpointTests();
    void runExperimentTests();
}

// Stress testing
namespace test_stress {
    void runLargeScaleTests();
    void runMemoryLeakTests();
    void runPerformanceRegressionTests();
}

// Edge case testing
namespace test_edge_cases {
    void runBoundaryConditionTests();
    void runErrorConditionTests();
    void runConfigurationEdgeCases();
}

int main() {
    std::cout << "=== NLM Phase 6 Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing Integrated Artificial Brain Systems:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics and Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity and Learning" << std::endl;
    std::cout << "  - Memory System Integration (Working, Episodic, Associative)" << std::endl;
    std::cout << "  - Neuromodulation Systems (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "  - Prediction System and Error Correction" << std::endl;
    std::cout << "  - Cognition Systems (Planning, Concept Formation, Attention)" << std::endl;
    std::cout << "  - Development System (Structural Plasticity, Maturation)" << std::endl;
    std::cout << "  - Advanced Python Bindings and CLI Interface" << std::endl;
    std::cout << "  - Configuration Management and Checkpoints" << std::endl;
    std::cout << "  - Stress Testing and Performance Benchmarks" << std::endl;
    std::cout << "  - Edge Case and Error Condition Testing" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // === Phase 2 Core Tests (Extended to Phase 6) ===
    std::cout << "=== Phase 2 Core Tests (Extended) ===" << std::endl;
    
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
    
    // === Phase 6 Integration Tests ===
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
    
    // === Advanced Functionality Tests ===
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
    
    // === Stress Testing ===
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
    
    // === Edge Case Testing ===
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
    
    // === Performance Benchmarks ===
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
