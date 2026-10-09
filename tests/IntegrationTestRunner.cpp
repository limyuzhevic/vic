// Integration Tests Implementation
// 
// This file implements the integration test runners for all categories of integration tests.

#include <iostream>
#include <cassert>
#include "IntegrationTestFramework.hpp"
#include "EndToEndTests.hpp"
#include "CrossComponentTests.hpp"
#include "PerformanceTests.hpp"
#include "StressTests.hpp"

namespace nlm {
namespace end_to_end_tests {
    void EndToEndTestRunner::runAllTests() {
        std::cout << "  Running End-to-End Simulation tests..." << std::endl;
        
        // Test complete neural network simulation from initialization to completion
        CompleteSimulationTest simTest;
        simTest.run();
        
        // Test neural development over time
        NeuralDevelopmentTest devTest;
        devTest.run();
        
        // Test plasticity and learning capabilities
        PlasticityLearningTest plasticTest;
        plasticTest.run();
        
        std::cout << "  End-to-End Simulation tests PASSED" << std::endl;
    }
}

namespace cross_component_tests {
    void CrossComponentTestRunner::runAllTests() {
        std::cout << "  Running Cross-Component Integration tests..." << std::endl;
        
        // Test memory-cognition integration
        MemoryCognitionIntegrationTest memCogTest;
        memCogTest.run();
        
        // Test neuromodulation-memory integration
        NeuromodulationMemoryIntegrationTest neuromodTest;
        neuromodTest.run();
        
        // Test motor-sensory integration
        MotorSensoryIntegrationTest motorTest;
        motorTest.run();
        
        std::cout << "  Cross-Component Integration tests PASSED" << std::endl;
    }
}

namespace performance_tests {
    void PerformanceTestRunner::runAllTests() {
        std::cout << "  Running Performance Integration tests..." << std::endl;
        
        // Test basic system performance under load
        BasicPerformanceTest basicTest;
        basicTest.run();
        
        // Test scalability with increasing system size
        ScalabilityTest scaleTest;
        scaleTest.run();
        
        // Test memory efficiency and memory pool performance
        MemoryEfficiencyTest memoryTest;
        memoryTest.run();
        
        // Benchmark plasticity computation performance
        PlasticityPerformanceTest plasticTest;
        plasticTest.run();
        
        std::cout << "  Performance Integration tests PASSED" << std::endl;
    }
}

namespace stress_tests {
    void StressTestRunner::runAllTests() {
        std::cout << "  Running Stress Integration tests..." << std::endl;
        
        // Test system behavior under high load conditions
        HighLoadStressTest highLoadTest;
        highLoadTest.run();
        
        // Test system behavior with memory pressure
        MemoryPressureStressTest memoryTest;
        memoryTest.run();
        
        // Test error handling and recovery mechanisms
        ErrorRecoveryStressTest errorTest;
        errorTest.run();
        
        // Test system behavior during prolonged operation
        ProlongedOperationStressTest prolongedTest;
        prolongedTest.run();
        
        std::cout << "  Stress Integration tests PASSED" << std::endl;
    }
}

} // namespace nlm
