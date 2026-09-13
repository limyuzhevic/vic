// NLM Test Main
// Phase 6: Comprehensive Integration Tests

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

namespace test_advanced_behaviors {
    void runAllAdvancedAgentBehaviorTests();
}

namespace test_brain_loop_integration {
    void runAllBrainLoopIntegrationTests();
}

namespace test_plasticity_fixes {
    void runAllPlasticityFixesTests();
}

namespace test_checkpoint_system {
    void runAllCheckpointTests();
}

namespace test_performance_monitoring {
    void runAllPerformanceMonitoringTests();
}

namespace test_configuration_validation {
    void runAllConfigurationValidationTests();
}

int main() {
    std::cout << "=== NLM Phase 6 Tests ===" << std::endl;
    std::cout << "Testing Comprehensive System Integration:" << std::endl;
    std::cout << "  - Real Neural Computation (Phase 2 core tests)" << std::endl;
    std::cout << "  - Advanced Agent Behaviors (8 behaviors)" << std::endl;
    std::cout << "  - Brain Loop Integration (15 steps)" << std::endl;
    std::cout << "  - Plasticity System Fixes (3 bugs)" << std::endl;
    std::cout << "  - Checkpoint System (save/load)" << std::endl;
    std::cout << "  - Performance Monitoring" << std::endl;
    std::cout << "  - Configuration Validation" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Phase 2: Core Neural Computation Tests
    std::cout << "=== Phase 2: Core Neural Computation Tests ===" << std::endl;
    
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
    std::cout << "=== Advanced Agent Behavior Tests ===" << std::endl;
    
    std::cout << "Running Advanced Agent Behavior tests..." << std::endl;
    try {
        test_advanced_behaviors::runAllAdvancedAgentBehaviorTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Brain Loop Integration Tests ===" << std::endl;
    
    std::cout << "Running Brain Loop Integration tests..." << std::endl;
    try {
        test_brain_loop_integration::runAllBrainLoopIntegrationTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Plasticity System Fixes Tests ===" << std::endl;
    
    std::cout << "Running Plasticity System Fixes tests..." << std::endl;
    try {
        test_plasticity_fixes::runAllPlasticityFixesTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Checkpoint System Tests ===" << std::endl;
    
    std::cout << "Running Checkpoint System tests..." << std::endl;
    try {
        test_checkpoint_system::runAllCheckpointTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Performance Monitoring Tests ===" << std::endl;
    
    std::cout << "Running Performance Monitoring tests..." << std::endl;
    try {
        test_performance_monitoring::runAllPerformanceMonitoringTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Configuration Validation Tests ===" << std::endl;
    
    std::cout << "Running Configuration Validation tests..." << std::endl;
    try {
        test_configuration_validation::runAllConfigurationValidationTests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "Comprehensive test suite completed successfully!" << std::endl;
        std::cout << "✓ Advanced Agent Behaviors: 8 behaviors tested" << std::endl;
        std::cout << "✓ Brain Loop Integration: 15 brain loop steps tested" << std::endl;
        std::cout << "✓ Plasticity System Fixes: 3 bugs fixed" << std::endl;
        std::cout << "✓ Checkpoint System: 2 stubs fixed to full implementation" << std::endl;
        std::cout << "✓ Performance Monitoring: 6 performance improvements verified" << std::endl;
        std::cout << "✓ Configuration Validation: 5 validation aspects tested" << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        return 1;
    }
}
