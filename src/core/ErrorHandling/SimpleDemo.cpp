// **Simple Error Handling Demo**
//
// A simple demonstration of the NLM error handling system.
// This example shows basic usage patterns and integration.

#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

void demoBasicExceptionHandling() {
    std::cout << "=== Basic Exception Handling Demo ===" << std::endl;
    
    // Initialize error handling
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning);
    
    // Demonstrate exception hierarchy
    std::cout << "\n1. Testing Exception Hierarchy:" << std::endl;
    
    try {
        throw nlm::ConfigFileNotFoundException("my_config.cfg");
    } catch (const nlm::ConfigFileNotFoundException& e) {
        std::cout << "   Caught ConfigFileNotFoundException: " << e.what() << std::endl;
        std::cout << "   Error Code: " << static_cast<int>(e.getErrorCode()) << std::endl;
    }
    
    try {
        throw nlm::BrainInitException("Brain initialization failed");
    } catch (const nlm::BrainInitException& e) {
        std::cout << "   Caught BrainInitException: " << e.what() << std::endl;
        std::cout << "   Error Code: " << static_cast<int>(e.getErrorCode()) << std::endl;
    }
    
    try {
        throw nlm::NeuronThresholdException("Neuron threshold exceeded");
    } catch (const nlm::NeuronThresholdException& e) {
        std::cout << "   Caught NeuronThresholdException: " << e.what() << std::endl;
        std::cout << "   Error Code: " << static_cast<int>(e.getErrorCode()) << std::endl;
    }
}

void demoErrorReporting() {
    std::cout << "\n=== Error Reporting Demo ===" << std::endl;
    
    // Demonstrate error reporting
    std::cout << "\n2. Testing Error Reporting:" << std::endl;
    
    nlm::ErrorHandler::getInstance().report("This is a warning message", 
                                            nlm::ErrorReportLevel::Warning, 
                                            "DemoContext");
    
    nlm::ErrorHandler::getInstance().report("This is an error message", 
                                            nlm::ErrorReportLevel::Error, 
                                            "DemoContext");
    
    nlm::ErrorHandler::getInstance().report("This is a critical message", 
                                            nlm::ErrorReportLevel::Critical, 
                                            "DemoContext");
}

void demoErrorStatistics() {
    std::cout << "\n=== Error Statistics Demo ===" << std::endl;
    
    std::cout << "\n3. Testing Error Statistics:" << std::endl;
    
    auto& handler = nlm::ErrorHandler::getInstance();
    const auto& stats = handler.getStatistics();
    
    std::cout << "   Total Errors: " << stats.totalErrors << std::endl;
    std::cout << "   Errors Handled: " << stats.errorsHandled << std::endl;
    std::cout << "   Errors Recovered: " << stats.errorsRecovered << std::endl;
    std::cout << "   Last Error: " << stats.lastErrorMessage << std::endl;
    
    // Reset statistics
    handler.resetStatistics();
    std::cout << "   Statistics reset" << std::endl;
}

void demoRecoveryActions() {
    std::cout << "\n=== Recovery Actions Demo ===" << std::endl;
    
    std::cout << "\n4. Testing Recovery Actions:" << std::endl;
    
    // Register recovery action
    bool recoveryCalled = false;
    nlm::ErrorHandler::getInstance().registerRecoveryAction(
        "demo_error",
        [&]() -> bool {
            recoveryCalled = true;
            std::cout << "   Recovery action executed" << std::endl;
            return true;
        }
    );
    
    // Execute recovery action
    bool result = nlm::ErrorHandler::getInstance().executeRecovery("demo_error");
    std::cout << "   Recovery result: " << (result ? "Success" : "Failed") << std::endl;
    std::cout << "   Recovery was called: " << (recoveryCalled ? "Yes" : "No") << std::endl;
    
    // Register cleanup action
    bool cleanupCalled = false;
    nlm::ErrorHandler::getInstance().registerCleanupAction(
        "demo_cleanup",
        [&]() {
            cleanupCalled = true;
            std::cout << "   Cleanup action executed" << std::endl;
        }
    );
    
    // Execute cleanup action
    nlm::ErrorHandler::getInstance().executeCleanup("demo_cleanup");
    std::cout << "   Cleanup was called: " << (cleanupCalled ? "Yes" : "No") << std::endl;
}

void demoGracefulDegradation() {
    std::cout << "\n=== Graceful Degradation Demo ===" << std::endl;
    
    std::cout << "\n5. Testing Graceful Degradation:" << std::endl;
    
    // Create degraded values
    auto degradedInt = nlm::ErrorHandling::GracefulDegradation::DegradedValue<int>(42);
    std::cout << "   Normal value: " << degradedInt.get() << std::endl;
    std::cout << "   Is degraded: " << (degradedInt.isDegradedValue() ? "Yes" : "No") << std::endl;
    
    // Create degraded neuron
    auto degradedNeuron = nlm::ErrorHandling::GracefulDegradation::degradedNeuron("Neuron unavailable");
    std::cout << "   Degraded neuron: " << (degradedNeuron.isDegradedValue() ? "Yes" : "No") << std::endl;
    std::cout << "   Degradation reason: " << degradedNeuron.getDegradationReason() << std::endl;
}

void demoConfigErrorHandling() {
    std::cout << "\n=== Config Error Handling Demo ===" << std::endl;
    
    std::cout << "\n6. Testing Config Error Handling:" << std::endl;
    
    nlm::Config config;
    
    // Configure error handling for config
    config.setErrorHandlingEnabled(true);
    config.setErrorStrategy(nlm::ExceptionStrategy::Catch);
    
    std::cout << "   Error handling enabled: " << (config.isErrorHandlingEnabled() ? "Yes" : "No") << std::endl;
    
    // Set some configuration
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("simulation_timestep", 0.001);
    
    std::cout << "   Configuration set successfully" << std::endl;
    
    // Try to get configuration
    auto neuronCount = config.get<int64_t>("neuron_count");
    if (neuronCount) {
        std::cout << "   Neuron count: " << *neuronCount << std::endl;
    }
    
    auto learningRate = config.get<double>("learning_rate");
    if (learningRate) {
        std::cout << "   Learning rate: " << *learningRate << std::endl;
    } else {
        std::cout << "   Learning rate not set (expected)" << std::endl;
    }
}

void demoErrorContext() {
    std::cout << "\n=== Error Context Demo ===" << std::endl;
    
    std::cout << "\n7. Testing Error Context:" << std::endl;
    
    nlm::ErrorHandling::ErrorContext context("DemoOperation");
    context.addStep("Step 1: Initialization");
    context.addStep("Step 2: Processing");
    context.addStep("Step 3: Finalization");
    context.addError("Some error occurred");
    
    std::cout << "   Context: " << context.getContext() << std::endl;
}

void mainDemo() {
    std::cout << "NLM Error Handling and Recovery System - Simple Demo" << std::endl;
    std::cout << "========================================================" << std::endl;
    
    // Run all demos
    demoBasicExceptionHandling();
    demoErrorReporting();
    demoErrorStatistics();
    demoRecoveryActions();
    demoGracefulDegradation();
    demoConfigErrorHandling();
    demoErrorContext();
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
}

// Global initialization function
void initializeDemo() {
    // Configure error handling
    nlm::ErrorHandler::initialize(nlm::ErrorReportLevel::Warning, 
                                  nlm::ExceptionStrategy::Catch);
}

int main() {
    // Initialize demo
    initializeDemo();
    
    // Run demo
    mainDemo();
    
    return 0;
}
