// NLM Error Handling and Robustness Examples
// Example usage of the enhanced error handling system

#include "BrainErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

using namespace nlm;

// Example 1: Basic Error Handling
void demonstrateBasicErrorHandling() {
    std::cout << "=== Example 1: Basic Error Handling ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    
    try {
        auto brain = EnhancedBrainFactory::createBrain(config);
        if (!brain) {
            std::cout << "Failed to create brain" << std::endl;
            return;
        }
        
        std::cout << "Brain created successfully" << std::endl;
        
        // Simulate an error scenario
        if (!brain->initialize()) {
            std::cout << "Brain initialization failed as expected" << std::endl;
        }
        
    } catch (const BrainError& e) {
        std::cout << "Caught BrainError: " << e.what() << std::endl;
        std::cout << "Error category: " << BrainError::errorCategoryToString(e.getCategory()) << std::endl;
        std::cout << "Context: " << e.getContext() << std::endl;
    }
}

// Example 2: Error Recovery Demonstration
void demonstrateErrorRecovery() {
    std::cout << "\n=== Example 2: Error Recovery Demonstration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    config->set("error.recovery_enabled", true);
    config->set("error.max_recovery_attempts", 3);
    
    auto brain = EnhancedBrainFactory::createBrain(config);
    if (!brain) {
        std::cout << "Failed to create brain" << std::endl;
        return;
    }
    
    std::cout << "Brain created with error recovery enabled" << std::endl;
    
    // Start a transaction
    brain->startTransaction();
    
    // Simulate a successful operation
    std::cout << "Performing operation with transaction..." << std::endl;
    
    // Commit transaction
    brain->commitTransaction();
    
    std::cout << "Transaction completed successfully" << std::endl;
}

// Example 3: Memory Management Example
void demonstrateMemoryManagement() {
    std::cout << "\n=== Example 3: Memory Management Example ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 2000);
    config->set("error.memory_limit_bytes", 50 * 1024 * 1024); // 50MB limit
    
    auto brain = EnhancedBrainFactory::createBrain(config);
    if (!brain) {
        std::cout << "Failed to create brain" << std::endl;
        return;
    }
    
    std::cout << "Brain created with memory limits" << std::endl;
    std::cout << "Current memory usage: " << brain->getCurrentMemoryUsage() << " bytes" << std::endl;
    std::cout << "Max memory usage: " << brain->getMaxMemoryUsage() << " bytes" << std::endl;
}

// Example 4: Configuration Validation Example
void demonstrateConfigurationValidation() {
    std::cout << "\n=== Example 4: Configuration Validation Example ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Set up invalid configuration to demonstrate validation
    config->set("brain.neuron_count", -100); // Invalid: negative neurons
    config->set("brain.v_thresh", 150.0f);  // Invalid: above 0mV
    config->set("brain.tau_mem", -5.0f);     // Invalid: negative time constant
    
    // Create brain with invalid configuration
    auto brain = EnhancedBrainFactory::createBrain(config);
    
    if (brain) {
        std::cout << "Brain created despite invalid configuration (safe defaults applied)" << std::endl;
    } else {
        std::cout << "Failed to create brain due to invalid configuration" << std::endl;
    }
}

// Example 5: Comprehensive Error Handling Example
void demonstrateComprehensiveErrorHandling() {
    std::cout << "\n=== Example 5: Comprehensive Error Handling Example ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1500);
    config->set("error.recovery_enabled", true);
    config->set("error.global_recovery_enabled", true);
    
    // Setup global error handling
    ErrorHandlingUtilities::setupGlobalErrorHandling(config);
    
    // Create brain with enhanced error handling
    auto brain = EnhancedBrainFactory::createBrainWithRecovery(config);
    if (!brain) {
        std::cout << "Failed to create brain" << std::endl;
        return;
    }
    
    std::cout << "Brain created with comprehensive error handling" << std::endl;
    
    // Check for any recent errors
    auto recentErrors = brain->getRecentErrors(10);
    if (!recentErrors.empty()) {
        std::cout << "Recent errors detected:" << std::endl;
        for (const auto& error : recentErrors) {
            std::cout << "  - " << error.what() << std::endl;
        }
    } else {
        std::cout << "No recent errors detected" << std::endl;
    }
}

// Example 6: State Management Example
void demonstrateStateManagement() {
    std::cout << "\n=== Example 6: State Management Example ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 800);
    
    auto brain = EnhancedBrainFactory::createBrain(config);
    if (!brain) {
        std::cout << "Failed to create brain" << std::endl;
        return;
    }
    
    std::cout << "Brain created for state management demonstration" << std::endl;
    
    // Start a transaction
    brain->startTransaction();
    std::cout << "Transaction started" << std::endl;
    
    // Perform some operations
    std::cout << "Performing operations..." << std::endl;
    
    // Commit transaction
    brain->commitTransaction();
    std::cout << "Transaction committed" << std::endl;
    
    // Check transaction state
    if (brain->isInTransaction()) {
        std::cout << "Currently in transaction" << std::endl;
    } else {
        std::cout << "Not in transaction" << std::endl;
    }
}

// Example 7: Error Logging Example
void demonstrateErrorLogging() {
    std::cout << "\n=== Example 7: Error Logging Example ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1200);
    config->set("error.log_file", "demo_error_log.txt");
    
    auto brain = EnhancedBrainFactory::createBrain(config);
    if (!brain) {
        std::cout << "Failed to create brain" << std::endl;
        return;
    }
    
    std::cout << "Brain created with error logging enabled" << std::endl;
    
    // Simulate some error conditions
    std::cout << "Simulating error conditions..." << std::endl;
    
    // Note: In a real scenario, errors would be logged automatically
    // This is just a demonstration of the error logging capability
}

// Main function demonstrating all error handling examples
int main() {
    std::cout << "NLM Error Handling and Robustness Examples" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        demonstrateBasicErrorHandling();
        demonstrateErrorRecovery();
        demonstrateMemoryManagement();
        demonstrateConfigurationValidation();
        demonstrateComprehensiveErrorHandling();
        demonstrateStateManagement();
        demonstrateErrorLogging();
        
        std::cout << "\n=== All Examples Completed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\nUnexpected error during execution: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
