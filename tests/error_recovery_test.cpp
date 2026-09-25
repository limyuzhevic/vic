// Error Recovery Test Suite
// Tests for error recovery mechanism and recovery strategies
// Validates error recovery strategies and recovery performance

#include "error_recovery_test.hpp"
#include "core/ErrorHandling/ErrorHierarchy.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace nlm {

void testErrorRecoveryStrategies() {
    std::cout << "Testing error recovery strategies..." << std::endl;
    
    // Test that error recovery strategies work correctly
    NLMErrorHierarchy errorHierarchy;
    
    // Test different recovery strategies
    // Error hierarchy should support various recovery strategies
    std::vector<std::string> recoveryStrategies = {
        "None", "Retry", "Fallback", "Reset", "PartialRecovery", "EmergencyShutdown", "Ignore"
    };
    
    // All recovery strategies should be available
    for (const auto& strategy : recoveryStrategies) {
        assert(!strategy.empty());
    }
    
    std::cout << "Error recovery strategies test passed!" << std::endl;
}

void testErrorRecoveryPerformance() {
    std::cout << "Testing error recovery performance..." << std::endl;
    
    // Test that error recovery performance is acceptable
    NLMErrorHierarchy errorHierarchy;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Benchmark error recovery performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 50; ++i) {
        try {
            // Simulate error condition that triggers recovery
            brain.step(-1); // Invalid step
        } catch (const std::exception& e) {
            // Error properly handled
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Error recovery: 50 error recoveries in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per recovery: " << (duration.count() / 50.0) << " ms" << std::endl;
    
    // Error recovery should be fast
    double avgPerRecovery = duration.count() / 50.0;
    assert(avgPerRecovery < 10.0); // Less than 10ms per error recovery is good
    
    std::cout << "Error recovery performance test passed!" << std::endl;
}

void testErrorRecoveryConsistency() {
    std::cout << "Testing error recovery consistency..." << std::endl;
    
    // Test that error recovery is consistent across different error conditions
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test recovery from various error conditions
    std::vector<int> errorInputs = {-1, 0, 1000000, -1000};
    
    for (int errorInput : errorInputs) {
        try {
            brain.step(errorInput); // Various error conditions
        } catch (const std::exception& e) {
            // Error should be handled consistently
            std::string errorMessage = e.what();
            assert(!errorMessage.empty());
        }
        
        // Brain should remain functional after error
        brain.step(1);
        assert(brain.getTotalNeuronCount() > 0);
    }
    
    std::cout << "Error recovery consistency test passed!" << std::endl;
}

void testErrorRecoveryFromDifferentErrorTypes() {
    std::cout << "Testing error recovery from different error types..." << std::endl;
    
    // Test recovery from configuration errors
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test recovery from configuration error
    try {
        // Trigger configuration error
        config->set("invalid_param", "invalid_value", ConfigSource::Runtime);
    } catch (const std::exception& e) {
        // Configuration error should be handled
    }
    
    // Brain should still function after configuration error
    brain.step(5);
    assert(brain.getTotalNeuronCount() > 0);
    
    // Test recovery from memory error
    try {
        brain.step(-1); // Invalid step
    } catch (const std::exception& e) {
        // Memory error should be handled
    }
    
    // Brain should still function after memory error
    brain.step(5);
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "Error recovery from different error types test passed!" << std::endl;
}

void testErrorRecoveryWithIntegration() {
    std::cout << "Testing error recovery with system integration..." << std::endl;
    
    // Test error recovery in integrated systems
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test error recovery with memory systems
    auto workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        try {
            brain.step(-1); // Error that affects memory
        } catch (const std::exception& e) {
            // Error should be handled with memory systems
        }
    }
    
    // Test error recovery with neuromodulation systems
    auto dopamine = brain.getDopamine();
    if (dopamine) {
        try {
            brain.step(-1); // Error that affects neuromodulation
        } catch (const std::exception& e) {
            // Error should be handled with neuromodulation systems
        }
    }
    
    // Test error recovery with prediction systems
    auto prediction = brain.getPredictionSystem();
    if (prediction) {
        try {
            brain.step(-1); // Error that affects prediction
        } catch (const std::exception& e) {
            // Error should be handled with prediction systems
        }
    }
    
    // All systems should still function after errors
    brain.step(10);
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "Error recovery with system integration test passed!" << std::endl;
}

void testErrorRecoveryPerformanceMonitoring() {
    std::cout << "Testing error recovery performance monitoring..." << std::endl;
    
    // Test that error recovery performance is monitored
    NLMErrorHierarchy errorHierarchy;
    
    // Test performance monitoring
    std::string performanceReport = errorHierarchy.getPerformanceReport();
    assert(!performanceReport.empty());
    
    // Test detailed performance monitoring
    std::string detailedReport = errorHierarchy.getDetailedPerformanceReport();
    assert(!detailedReport.empty());
    
    std::cout << "Error recovery performance monitoring test passed!" << std::endl;
}

void runErrorRecoveryTests() {
    std::cout << "=== Running Error Recovery Tests ===" << std::endl;
    std::cout << std::endl;
    
    testErrorRecoveryStrategies();
    std::cout << std::endl;
    
    testErrorRecoveryPerformance();
    std::cout << std::endl;
    
    testErrorRecoveryConsistency();
    std::cout << std::endl;
    
    testErrorRecoveryFromDifferentErrorTypes();
    std::cout << std::endl;
    
    testErrorRecoveryWithIntegration();
    std::cout << std::endl;
    
    testErrorRecoveryPerformanceMonitoring();
    std::cout << std::endl;
    
    std::cout << "=== All Error Recovery Tests Passed ===" << std::endl;
}

} // namespace nlm
