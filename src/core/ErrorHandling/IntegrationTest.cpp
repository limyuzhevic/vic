// Simple Integration Test for Error Handling System
// 
// This test demonstrates the integration of the error handling system
// with the existing NLM components and showcases key features.

#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void testBasicExceptionHierarchy() {
    std::cout << "Testing Exception Hierarchy..." << std::endl;
    
    // Test that all exception types can be instantiated
    try {
        throw nlm::ConfigFileNotFoundException("test.cfg");
    } catch (const nlm::ConfigFileNotFoundException& e) {
        assert(e.getErrorCode() == nlm::ErrorCode::ConfigFileNotFound);
    }
    
    try {
        throw nlm::BrainInitException("Brain init failed");
    } catch (const nlm::BrainInitException& e) {
        assert(e.getErrorCode() == nlm::ErrorCode::BrainInitFailure);
    }
    
    try {
        throw nlm::NeuronThresholdException("Threshold exceeded");
    } catch (const nlm::NeuronThresholdException& e) {
        assert(e.getErrorCode() == nlm::ErrorCode::NeuronThresholdError);
    }
    
    try {
        throw nlm::SynapseConnectionException("Synapse failed");
    } catch (const nlm::SynapseConnectionException& e) {
        assert(e.getErrorCode() == nlm::ErrorCode::SynapseConnectionError);
    }
    
    try {
        throw nlm::TimeStepException("Time step error");
    } catch (const nlm::TimeStepException& e) {
        assert(e.getErrorCode() == nlm::ErrorCode::TimeStepError);
    }
    
    std::cout << "  ✓ Exception hierarchy test passed" << std::endl;
}

void testErrorHandlingConfiguration() {
    std::cout << "Testing Error Handling Configuration..." << std::endl;
    
    // Initialize error handling
    nlm::ErrorHandler::initialize(nlm::ErrorReportLevel::Warning, 
                                  nlm::ExceptionStrategy::Catch);
    
    auto& handler = nlm::ErrorHandler::getInstance();
    
    // Test configuration
    assert(handler.getGlobalLevel() == nlm::ErrorReportLevel::Warning);
    assert(handler.getStrategy() == nlm::ExceptionStrategy::Catch);
    
    std::cout << "  ✓ Error handling configuration test passed" << std::endl;
}

void testConfigWithErrorHandling() {
    std::cout << "Testing Config with Error Handling..." << std::endl;
    
    nlm::Config config;
    
    // Enable error handling
    config.setErrorHandlingEnabled(true);
    assert(config.isErrorHandlingEnabled() == true);
    
    // Set configuration with error handling
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("simulation_timestep", 0.001);
    
    // Get configuration
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto timestep = config.get<double>("simulation_timestep");
    assert(timestep.has_value());
    assert(*timestep == 0.001);
    
    // Test error reporting
    config.logError("Test error", "ConfigOperation");
    
    std::cout << "  ✓ Config with error handling test passed" << std::endl;
}

void testErrorReporting() {
    std::cout << "Testing Error Reporting..." << std::endl;
    
    nlm::ErrorHandler::getInstance().report("Test warning", 
                                            nlm::ErrorReportLevel::Warning,
                                            "TestContext");
    
    nlm::ErrorHandler::getInstance().report("Test error", 
                                            nlm::ErrorReportLevel::Error,
                                            "TestContext");
    
    const auto& stats = nlm::ErrorHandler::getInstance().getStatistics();
    assert(stats.totalErrors > 0);
    
    std::cout << "  ✓ Error reporting test passed" << std::endl;
}

void testRecoveryActions() {
    std::cout << "Testing Recovery Actions..." << std::endl;
    
    // Register recovery action
    bool recoveryExecuted = false;
    nlm::ErrorHandler::getInstance().registerRecoveryAction(
        "test_recovery",
        [&]() -> bool {
            recoveryExecuted = true;
            return true;
        }
    );
    
    // Execute recovery
    bool result = nlm::ErrorHandler::getInstance().executeRecovery("test_recovery");
    assert(result == true);
    assert(recoveryExecuted == true);
    
    // Register cleanup action
    bool cleanupExecuted = false;
    nlm::ErrorHandler::getInstance().registerCleanupAction(
        "test_cleanup",
        [&]() {
            cleanupExecuted = true;
        }
    );
    
    // Execute cleanup
    nlm::ErrorHandler::getInstance().executeCleanup("test_cleanup");
    assert(cleanupExecuted == true);
    
    std::cout << "  ✓ Recovery actions test passed" << std::endl;
}

void testGracefulDegradation() {
    std::cout << "Testing Graceful Degradation..." << std::endl;
    
    // Test degraded value
    auto degraded = nlm::ErrorHandling::GracefulDegradation::DegradedValue<int>(5);
    assert(degraded.get() == 5);
    assert(!degraded.isDegradedValue());
    
    // Test degraded config
    auto degradedConfig = nlm::ErrorHandling::GracefulDegradation::degradedConfig("Config unavailable");
    assert(degradedConfig.isDegradedValue());
    
    std::cout << "  ✓ Graceful degradation test passed" << std::endl;
}

void testGlobalErrorRecovery() {
    std::cout << "Testing Global Error Recovery..." << std::endl;
    
    nlm::ErrorHandling::GlobalErrorRecovery& recovery = nlm::getGlobalErrorRecovery();
    
    // Test safe execution
    auto result = recovery.executeSafely("TestOperation", []() -> int {
        return 42;
    });
    
    assert(result.get() == 42);
    
    std::cout << "  ✓ Global error recovery test passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== NLM Error Handling Integration Test ===" << std::endl;
    std::cout << "Testing comprehensive error handling integration..." << std::endl << std::endl;
    
    testBasicExceptionHierarchy();
    testErrorHandlingConfiguration();
    testConfigWithErrorHandling();
    testErrorReporting();
    testRecoveryActions();
    testGracefulDegradation();
    testGlobalErrorRecovery();
    
    std::cout << std::endl;
    std::cout << "=== All Tests Passed! ===" << std::endl;
    std::cout << "The error handling and recovery system is working correctly." << std::endl;
}

int main() {
    // Initialize error handling
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning,
                                  nlm::ExceptionStrategy::Catch);
    
    // Run tests
    runAllTests();
    
    return 0;
}
