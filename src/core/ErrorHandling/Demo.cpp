// Simple Error Handling Demonstration
// 
// This program demonstrates the key features of the NLM Error Handling System.
// It shows exception handling, error recovery, and integration examples.

#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>
#include <string>

// Demonstration Functions

void demonstrateExceptionHierarchy() {
    std::cout << "=== Exception Hierarchy Demo ===" << std::endl;
    
    std::cout << "\n1. Config Exceptions:" << std::endl;
    try {
        throw nlm::ConfigFileNotFoundException("missing_config.cfg");
    } catch (const nlm::ConfigFileNotFoundException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
        std::cout << "   ✓ Error Code: " << static_cast<int>(e.getErrorCode()) << std::endl;
    }
    
    try {
        throw nlm::MalformedConfigException("Invalid syntax at line 42");
    } catch (const nlm::MalformedConfigException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n2. Brain Exceptions:" << std::endl;
    try {
        throw nlm::BrainInitException("Failed to initialize brain");
    } catch (const nlm::BrainInitException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::SimulationRuntimeException("Simulation crashed");
    } catch (const nlm::SimulationRuntimeException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n3. Neuron Exceptions:" << std::endl;
    try {
        throw nlm::NeuronThresholdException("Neuron threshold exceeded");
    } catch (const nlm::NeuronThresholdException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::RefractoryPeriodException("Neuron in refractory period");
    } catch (const nlm::RefractoryPeriodException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n4. Synapse Exceptions:" << std::endl;
    try {
        throw nlm::SynapseConnectionException("Synapse connection failed");
    } catch (const nlm::SynapseConnectionException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::WeightCalculationException("Invalid weight calculation");
    } catch (const nlm::WeightCalculationException& e) {
        std::cout << "   ✓ Caught: " << e.what() << std::endl;
    }
}

void demonstrateErrorReporting() {
    std::cout << "\n=== Error Reporting Demo ===" << std::endl;
    
    // Initialize error handling
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning);
    
    std::cout << "\n1. Warning Level Reporting:" << std::endl;
    nlm::ErrorHandler::getInstance().report(
        "Configuration file not found, using defaults",
        nlm::ErrorReportLevel::Warning,
        "ConfigLoad"
    );
    
    std::cout << "\n2. Error Level Reporting:" << std::endl;
    nlm::ErrorHandler::getInstance().report(
        "Failed to allocate memory for neuron population",
        nlm::ErrorReportLevel::Error,
        "BrainInit"
    );
    
    std::cout << "\n3. Critical Level Reporting:" << std::endl;
    nlm::ErrorHandler::getInstance().report(
        "System resources exhausted, cannot continue",
        nlm::ErrorReportLevel::Critical,
        "SystemError"
    );
    
    // Show error statistics
    std::cout << "\n4. Error Statistics:" << std::endl;
    const auto& stats = nlm::ErrorHandler::getInstance().getStatistics();
    std::cout << "   Total Errors: " << stats.totalErrors << std::endl;
    std::cout << "   Errors Handled: " << stats.errorsHandled << std::endl;
    std::cout << "   Errors Recovered: " << stats.errorsRecovered << std::endl;
}

void demonstrateRecoveryActions() {
    std::cout << "\n=== Recovery Actions Demo ===" << std::endl;
    
    std::cout << "\n1. Registering Recovery Actions:" << std::endl;
    
    // Register memory allocation recovery
    bool memoryRecoveryCalled = false;
    nlm::ErrorHandler::getInstance().registerRecoveryAction(
        "memory_allocation",
        [&]() -> bool {
            memoryRecoveryCalled = true;
            std::cout << "   - Memory allocation recovery executed" << std::endl;
            return true;
        }
    );
    
    // Register simulation recovery
    bool simulationRecoveryCalled = false;
    nlm::ErrorHandler::getInstance().registerRecoveryAction(
        "simulation_runtime",
        [&]() -> bool {
            simulationRecoveryCalled = true;
            std::cout << "   - Simulation runtime recovery executed" << std::endl;
            return true;
        }
    );
    
    // Register cleanup actions
    bool cleanupCalled = false;
    nlm::ErrorHandler::getInstance().registerCleanupAction(
        "config_operation",
        [&]() {
            cleanupCalled = true;
            std::cout << "   - Config operation cleanup executed" << std::endl;
        }
    );
    
    std::cout << "\n2. Executing Recovery Actions:" << std::endl;
    
    bool memoryResult = nlm::ErrorHandler::getInstance().executeRecovery("memory_allocation");
    std::cout << "   Memory recovery result: " << (memoryResult ? "Success" : "Failed") << std::endl;
    
    bool simulationResult = nlm::ErrorHandler::getInstance().executeRecovery("simulation_runtime");
    std::cout << "   Simulation recovery result: " << (simulationResult ? "Success" : "Failed") << std::endl;
    
    std::cout << "\n3. Executing Cleanup Actions:" << std::endl;
    nlm::ErrorHandler::getInstance().executeCleanup("config_operation");
    std::cout << "   Cleanup was called: " << (cleanupCalled ? "Yes" : "No") << std::endl;
}

void demonstrateGracefulDegradation() {
    std::cout << "\n=== Graceful Degradation Demo ===" << std::endl;
    
    std::cout << "\n1. Normal Values:" << std::endl;
    auto normalInt = nlm::ErrorHandling::GracefulDegradation::DegradedValue<int>(42);
    std::cout << "   Normal int: " << normalInt.get() << std::endl;
    std::cout << "   Is degraded: " << (normalInt.isDegradedValue() ? "Yes" : "No") << std::endl;
    
    std::cout << "\n2. Degraded Config:" << std::endl;
    auto degradedConfig = nlm::ErrorHandling::GracefulDegradation::degradedConfig("Configuration file corrupted");
    std::cout << "   Is degraded: " << (degradedConfig.isDegradedValue() ? "Yes" : "No") << std::endl;
    std::cout << "   Reason: " << degradedConfig.getDegradationReason() << std::endl;
    
    std::cout << "\n3. Degraded Brain:" << std::endl;
    auto degradedBrain = nlm::ErrorHandling::GracefulDegradation::degradedBrain("Brain initialization failed");
    std::cout << "   Is degraded: " << (degradedBrain.isDegradedValue() ? "Yes" : "No") << std::endl;
    std::cout << "   Reason: " << degradedBrain.getDegradationReason() << std::endl;
}

void demonstrateConfigErrorHandling() {
    std::cout << "\n=== Config Error Handling Demo ===" << std::endl;
    
    nlm::Config config;
    
    std::cout << "\n1. Creating Configuration with Error Handling:" << std::endl;
    config.setErrorHandlingEnabled(true);
    std::cout << "   Error handling enabled: " << (config.isErrorHandlingEnabled() ? "Yes" : "No") << std::endl;
    
    std::cout << "\n2. Setting Configuration Values:" << std::endl;
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("simulation_timestep", 0.001);
    config.set("region_count", static_cast<int64_t>(2));
    
    std::cout << "   Configuration values set successfully" << std::endl;
    
    std::cout << "\n3. Getting Configuration Values:" << std::endl;
    auto neuronCount = config.get<int64_t>("neuron_count");
    if (neuronCount) {
        std::cout << "   Neuron count: " << *neuronCount << std::endl;
    }
    
    auto timestep = config.get<double>("simulation_timestep");
    if (timestep) {
        std::cout << "   Simulation timestep: " << *timestep << std::endl;
    }
    
    auto learningRate = config.get<double>("learning_rate");
    if (learningRate) {
        std::cout << "   Learning rate: " << *learningRate << std::endl;
    } else {
        std::cout << "   Learning rate: Not set (expected)" << std::endl;
    }
    
    std::cout << "\n4. Error Statistics from Config Operations:" << std::endl;
    const auto& stats = nlm::ErrorHandler::getInstance().getStatistics();
    std::cout << "   Total errors: " << stats.totalErrors << std::endl;
}

void demonstrateErrorContext() {
    std::cout << "\n=== Error Context Demo ===" << std::endl;
    
    std::cout << "\n1. Creating Error Context:" << std::endl;
    nlm::ErrorHandling::ErrorContext context("ComplexOperation");
    
    context.addStep("Step 1: Initialization");
    context.addStep("Step 2: Loading Configuration");
    context.addStep("Step 3: Processing Data");
    context.addStep("Step 4: Finalization");
    
    context.addError("Configuration file not found");
    context.addError("Memory allocation failed");
    
    std::cout << "\n2. Error Context Information:" << std::endl;
    std::cout << "   " << context.getContext() << std::endl;
}

void demonstrateGlobalErrorRecovery() {
    std::cout << "\n=== Global Error Recovery Demo ===" << std::endl;
    
    nlm::ErrorHandling::GlobalErrorRecovery& recovery = nlm::getGlobalErrorRecovery();
    
    std::cout << "\n1. Configuration:" << std::endl;
    std::cout << "   Global level: " << static_cast<int>(recovery.getGlobalLevel()) << std::endl;
    std::cout << "   Exception strategy: " << static_cast<int>(recovery.getStrategy()) << std::endl;
    
    std::cout << "\n2. Safe Execution with Degraded Result:" << std::endl;
    
    auto result = recovery.executeSafely("RiskyOperation", []() -> int {
        return 42;
    });
    
    if (result.isDegradedValue()) {
        std::cout << "   Operation failed with reason: " << result.getDegradationReason() << std::endl;
    } else {
        std::cout << "   Operation succeeded with result: " << result.get() << std::endl;
    }
    
    std::cout << "\n3. Error Handler Statistics:" << std::endl;
    const auto& handlerStats = nlm::ErrorHandler::getInstance().getStatistics();
    std::cout << "   Total errors: " << handlerStats.totalErrors << std::endl;
    std::cout << "   Errors handled: " << handlerStats.errorsHandled << std::endl;
}

void demonstrateIntegrationExample() {
    std::cout << "\n=== Integration Example Demo ===" << std::endl;
    
    std::cout << "\n1. Complete Application Workflow:" << std::endl;
    
    // Initialize error handling
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning);
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(100));
    config.set("simulation_timestep", 0.001);
    
    // Create brain with error handling
    auto brainConfig = std::make_shared<nlm::Config>(config);
    std::cout << "   ✓ Configuration created and validated" << std::endl;
    
    // Simulate with error handling
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        // This would normally call brain.step(step), but for demo we just show the pattern
        std::cout << "   ✓ Simulated step " << step << std::endl;
        
        // Simulate error handling
        if (step == 5) {
            nlm::ErrorHandler::getInstance().report(
                "Simulated spike event",
                nlm::ErrorReportLevel::Warning,
                "SimulationStep" + std::to_string(step)
            );
        }
    }
    
    std::cout << "\n2. Recovery Mechanisms:" << std::endl;
    std::cout << "   ✓ Retry logic available for transient errors" << std::endl;
    std::cout << "   ✓ Checkpointing available for state recovery" << std::endl;
    std::cout << "   ✓ Graceful degradation available for non-critical failures" << std::endl;
    std::cout << "   ✓ Cleanup handlers available for resource management" << std::endl;
}

void runAllDemos() {
    std::cout << "NLM Error Handling and Recovery System - Demonstration" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "This demonstration showcases the key features of the NLM error handling system." << std::endl << std::endl;
    
    // Run all demonstrations
    demonstrateExceptionHierarchy();
    demonstrateErrorReporting();
    demonstrateRecoveryActions();
    demonstrateGracefulDegradation();
    demonstrateConfigErrorHandling();
    demonstrateErrorContext();
    demonstrateGlobalErrorRecovery();
    demonstrateIntegrationExample();
    
    std::cout << std::endl;
    std::cout << "=== Demonstration Complete ===" << std::endl;
    std::cout << "The NLM error handling system provides comprehensive error handling, recovery, and integration capabilities." << std::endl;
}

int main() {
    // Initialize the error handling system
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning);
    
    // Run the demonstration
    runAllDemos();
    
    return 0;
}
