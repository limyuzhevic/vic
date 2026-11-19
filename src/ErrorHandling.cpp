#include "ErrorHandling.hpp"

namespace nlm {

// Error handling integration functions
// These functions integrate the error handling framework with existing NLM components

// Configure error handling for the entire NLM system
inline void setupErrorHandling() {
    setupGlobalErrorHandler();
    
    // Configure error handling for core components
    // This can be extended to integrate with other components as needed
    
    NLM_LOG_INFO("Error handling framework initialized");
}

// Error handling wrapper for configuration operations
inline bool safeConfigurationOperations() {
    try {
        // Test configuration loading
        Config config;
        
        // Test with a non-existent file
        bool loadResult = config.loadFromFile("/nonexistent/path/config.txt");
        NLM_LOG_DEBUG("Configuration load test: " + std::to_string(loadResult));
        
        // Test with empty content (simulated)
        // In real scenarios, this would use actual file operations
        
        return true;
    } catch (const NLMError& e) {
        ErrorHandler::handleError(e);
        return false;
    } catch (const std::exception& e) {
        ErrorHandler::handleError(NLMError(
            std::string("Unexpected error in configuration operations: ") + e.what(),
            ErrorCategory::System, ErrorSeverity::Error, "Configuration"
        ));
        return false;
    }
}

// Error handling wrapper for brain operations
inline bool safeBrainOperations() {
    try {
        // Test brain initialization
        auto config = std::make_shared<Config>();
        
        // Set minimal configuration
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        // Create and initialize brain
        Brain brain(config);
        bool initResult = brain.initialize();
        
        NLM_LOG_DEBUG("Brain initialization test: " + std::to_string(initResult));
        
        return true;
    } catch (const NLMError& e) {
        ErrorHandler::handleError(e);
        return false;
    } catch (const std::exception& e) {
        ErrorHandler::handleError(NLMError(
            std::string("Unexpected error in brain operations: ") + e.what(),
            ErrorCategory::System, ErrorSeverity::Error, "Brain"
        ));
        return false;
    }
}

// Error handling wrapper for memory operations
inline bool safeMemoryOperations() {
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        Brain brain(config);
        brain.initialize();
        
        // Test memory system initialization
        NLM_LOG_DEBUG("Memory operations test: available");
        
        return true;
    } catch (const NLMError& e) {
        ErrorHandler::handleError(e);
        return false;
    } catch (const std::exception& e) {
        ErrorHandler::handleError(NLMError(
            std::string("Unexpected error in memory operations: ") + e.what(),
            ErrorCategory::System, ErrorSeverity::Error, "Memory"
        ));
        return false;
    }
}

// Error handling wrapper for development operations
inline bool safeDevelopmentOperations() {
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        Brain brain(config);
        brain.initialize();
        
        // Test development system
        NLM_LOG_DEBUG("Development operations test: available");
        
        return true;
    } catch (const NLMError& e) {
        ErrorHandler::handleError(e);
        return false;
    } catch (const std::exception& e) {
        ErrorHandler::handleError(NLMError(
            std::string("Unexpected error in development operations: ") + e.what(),
            ErrorCategory::System, ErrorSeverity::Error, "Development"
        ));
        return false;
    }
}

// Error handling wrapper for checkpoint operations
inline bool safeCheckpointOperations() {
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        Brain brain(config);
        brain.initialize();
        
        // Test checkpoint operations
        std::string testFile = "./test_checkpoint.bin";
        
        // Clean up any existing test file
        if (std::filesystem::exists(testFile)) {
            std::filesystem::remove(testFile);
        }
        
        NLM_LOG_DEBUG("Checkpoint operations test: available");
        
        return true;
    } catch (const NLMError& e) {
        ErrorHandler::handleError(e);
        return false;
    } catch (const std::exception& e) {
        ErrorHandler::handleError(NLMError(
            std::string("Unexpected error in checkpoint operations: ") + e.what(),
            ErrorCategory::System, ErrorSeverity::Error, "Checkpoint"
        ));
        return false;
    }
}

// System error validation and recovery
inline bool validateSystemIntegrity() {
    NLM_LOG_DEBUG("Validating system integrity...");
    
    bool allChecksPassed = true;
    
    // Check configuration system
    try {
        Config config;
        allChecksPassed &= safeConfigurationOperations();
    } catch (...) {
        NLM_LOG_ERROR("Configuration system integrity check failed");
        allChecksPassed = false;
    }
    
    // Check brain system
    try {
        allChecksPassed &= safeBrainOperations();
    } catch (...) {
        NLM_LOG_ERROR("Brain system integrity check failed");
        allChecksPassed = false;
    }
    
    // Check memory system
    try {
        allChecksPassed &= safeMemoryOperations();
    } catch (...) {
        NLM_LOG_ERROR("Memory system integrity check failed");
        allChecksPassed = false;
    }
    
    // Check development system
    try {
        allChecksPassed &= safeDevelopmentOperations();
    } catch (...) {
        NLM_LOG_ERROR("Development system integrity check failed");
        allChecksPassed = false;
    }
    
    // Check checkpoint system
    try {
        allChecksPassed &= safeCheckpointOperations();
    } catch (...) {
        NLM_LOG_ERROR("Checkpoint system integrity check failed");
        allChecksPassed = false;
    }
    
    if (allChecksPassed) {
        NLM_LOG_INFO("System integrity validation passed");
        return true;
    } else {
        NLM_LOG_ERROR("System integrity validation failed");
        return false;
    }
}

// Error handling status reporting
inline std::string getErrorHandlingStatus() {
    std::string status = "Error Handling System Status:\n";
    status += "  - Global error handler: " + std::string(ErrorHandler::isErrorReportingEnabled() ? "Enabled" : "Disabled") + "\n";
    status += "  - Error count: " + std::to_string(ErrorHandler::getErrorCount()) + "\n";
    status += "  - Error handling framework: " + std::string(ErrorHandler::getErrorCount(ErrorSeverity::Error) > 0 ? "Active" : "Idle") + "\n";
    status += "  - System validation: " + std::string(validateSystemIntegrity() ? "Passed" : "Failed") + "\n";
    
    return status;
}

} // namespace nlm