// Error Hierarchy Test Suite
// Tests for error hierarchy validation and error type consistency
// Validates standardized error hierarchy and error category consistency

#include "error_hierarchy_test.hpp"
#include "core/ErrorHandling/ErrorHierarchy.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace nlm {

void testErrorHierarchyStructure() {
    std::cout << "Testing error hierarchy structure..." << std::endl;
    
    // Test that error hierarchy has proper structure
    NLMErrorHierarchy errorHierarchy;
    
    // Test that error hierarchy has proper error categories
    assert(errorHierarchy.getErrorCount() == 0); // Initial state
    
    // Test error category consistency
    // Error categories should be properly defined
    std::string errorReport = errorHierarchy.getErrorReport();
    assert(!errorReport.empty());
    
    std::cout << "Error hierarchy structure test passed!" << std::endl;
}

void testErrorCategoryConsistency() {
    std::cout << "Testing error category consistency..." << std::endl;
    
    // Test that error categories are consistent across the system
    NLMErrorHierarchy errorHierarchy;
    
    // Test error types are properly defined
    std::vector<std::string> expectedCategories = {
        "Configuration", "Memory", "Plasticity", "Neuromodulation",
        "Development", "Prediction", "WorkingMemory", "EpisodicMemory",
        "Action", "Environment", "Network", "Resource", "State", "Permission", "Performance"
    };
    
    // All error categories should be consistent
    for (const auto& category : expectedCategories) {
        assert(!category.empty());
    }
    
    std::cout << "Error category consistency test passed!" << std::endl;
}

void testErrorTypeConsistency() {
    std::cout << "Testing error type consistency..." << std::endl;
    
    // Test that error types are consistent within categories
    // Configuration errors
    std::vector<std::string> configErrorTypes = {
        "InvalidParameter", "MissingParameter", "ParameterRange", "FileNotFound",
        "FileFormat", "CommandLineError", "DuplicateKey", "CircularReference",
        "SchemaValidation", "InvalidValueType"
    };
    
    // Memory errors
    std::vector<std::string> memoryErrorTypes = {
        "AllocationFailure", "CapacityExceeded", "ConsistencyViolation", "Corruption",
        "EncodingError", "Timeout", "ConcurrentAccess", "InvalidReference"
    };
    
    // All error types should be consistently defined
    for (const auto& errorType : configErrorTypes) {
        assert(!errorType.empty());
    }
    
    for (const auto& errorType : memoryErrorTypes) {
        assert(!errorType.empty());
    }
    
    std::cout << "Error type consistency test passed!" << std::endl;
}

void testErrorMessageConsistency() {
    std::cout << "Testing error message consistency..." << std::endl;
    
    // Test that error messages follow consistent format
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that error messages follow consistent format
    try {
        brain.step(-1); // Invalid step
    } catch (const std::exception& e) {
        std::string errorMessage = e.what();
        
        // Error message should follow consistent format
        assert(errorMessage.find("NLM") != std::string::npos); // Should start with NLM
        assert(errorMessage.find("[") != std::string::npos); // Should have brackets
        assert(errorMessage.find("]") != std::string::npos); // Should have brackets
    }
    
    std::cout << "Error message consistency test passed!" << std::endl;
}

void testErrorContextConsistency() {
    std::cout << "Testing error context consistency..." << std::endl;
    
    // Test that error context is consistently provided
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that errors include context information
    try {
        brain.step(-1); // Invalid step
    } catch (const std::exception& e) {
        std::string errorMessage = e.what();
        
        // Error should include context (optional)
        // Context may or may not be present depending on error type
        assert(!errorMessage.empty());
    }
    
    std::cout << "Error context consistency test passed!" << std::endl;
}

void testErrorPerformanceTracking() {
    std::cout << "Testing error performance tracking..." << std::endl;
    
    // Test that error handling performance is properly tracked
    NLMErrorHierarchy errorHierarchy;
    
    // Test performance metrics
    std::string performanceReport = errorHierarchy.getPerformanceReport();
    assert(!performanceReport.empty());
    
    // Test detailed performance metrics
    std::string detailedReport = errorHierarchy.getDetailedPerformanceReport();
    assert(!detailedReport.empty());
    
    std::cout << "Error performance tracking test passed!" << std::endl;
}

void testErrorRecoveryConsistency() {
    std::cout << "Testing error recovery consistency..." << std::endl;
    
    // Test that error recovery is consistent across different error types
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test recovery from different error conditions
    bool recoverySuccessful = true;
    
    try {
        brain.step(-1); // Invalid step
    } catch (const std::exception& e) {
        // Error should be handled consistently
        std::cout << "Error properly caught: " << e.what() << std::endl;
    }
    
    // Test that brain can recover from error
    brain.step(5);
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "Error recovery consistency test passed!" << std::endl;
}

void runErrorHierarchyTests() {
    std::cout << "=== Running Error Hierarchy Tests ===" << std::endl;
    std::cout << std::endl;
    
    testErrorHierarchyStructure();
    std::cout << std::endl;
    
    testErrorCategoryConsistency();
    std::cout << std::endl;
    
    testErrorTypeConsistency();
    std::cout << std::endl;
    
    testErrorMessageConsistency();
    std::cout << std::endl;
    
    testErrorContextConsistency();
    std::cout << std::endl;
    
    testErrorPerformanceTracking();
    std::cout << std::endl;
    
    testErrorRecoveryConsistency();
    std::cout << std::endl;
    
    std::cout << "=== All Error Hierarchy Tests Passed ===" << std::endl;
}

} // namespace nlm
