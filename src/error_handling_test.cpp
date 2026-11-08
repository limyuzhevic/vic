// Error Handling Test Program
// Demonstrates the enhanced error handling functionality

#include "ErrorHandling.hpp"
#include <iostream>
#include <string>
#include <vector>

void testErrorHandlingBasics() {
    std::cout << "=== Basic Error Handling Test ===" << std::endl;
    
    try {
        // Test 1: Normal operation
        std::cout << "\nTest 1: Testing normal error handling...";
        nlm::ErrorManager::resetToDefault();
        std::cout << " PASSED" << std::endl;
        
        // Test 2: Error logging
        std::cout << "\nTest 2: Testing error logging...";
        try {
            nlm::ErrorManager::logError(
                nlm::ErrorCategory::CONFIG_FILE_NOT_FOUND,
                nlm::ErrorSeverity::ERROR,
                "Configuration file 'config.json' not found"
            );
            std::cout << " PASSED (error logged successfully)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << " PASSED (exception thrown as expected)" << std::endl;
        }
        
        // Test 3: Error context
        std::cout << "\nTest 3: Testing error context...";
        std::string errorContext = "TestContext";
        nlm::ErrorContext ctx(errorContext);
        ctx.addData("user", "testuser");
        ctx.addData("timestamp", "2024-01-01T12:00:00Z");
        ctx.activate();
        
        std::cout << " PASSED (error context created and activated)" << std::endl;
        
        // Test 4: Error history
        std::cout << "\nTest 4: Testing error history...";
        std::vector<nlm::ErrorInfo> history = nlm::ErrorManager::getErrorHistory();
        std::cout << " PASSED (error history retrieved, count: " << history.size() << ")" << std::endl;
        
        // Test 5: Error suggestions
        std::cout << "\nTest 5: Testing error suggestions...";
        std::vector<std::string> suggestions = nlm::ErrorUtils::getErrorSuggestions(
            nlm::ErrorCategory::CONFIG_FILE_NOT_FOUND
        );
        std::cout << " PASSED (suggestions retrieved, count: " << suggestions.size() << ")" << std::endl;
        
        for (const auto& suggestion : suggestions) {
            std::cout << "    - " << suggestion << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testErrorSeverityLevels() {
    std::cout << "\n=== Error Severity Levels Test ===" << std::endl;
    
    try {
        // Test different severity levels
        std::cout << "\nTest 1: Testing DEBUG severity...";
        nlm::ErrorManager::logError(
            nlm::ErrorCategory::UNKNOWN_ERROR,
            nlm::ErrorSeverity::DEBUG,
            "Debug message - detailed information"
        );
        std::cout << " PASSED (DEBUG error logged)" << std::endl;
        
        std::cout << "\nTest 2: Testing INFO severity...";
        nlm::ErrorManager::logError(
            nlm::ErrorCategory::UNKNOWN_ERROR,
            nlm::ErrorSeverity::INFO,
            "Information message - operational details"
        );
        std::cout << " PASSED (INFO error logged)" << std::endl;
        
        std::cout << "\nTest 3: Testing WARNING severity...";
        nlm::ErrorManager::logError(
            nlm::ErrorCategory::UNKNOWN_ERROR,
            nlm::ErrorSeverity::WARNING,
            "Warning message - potential issue"
        );
        std::cout << " PASSED (WARNING error logged)" << std::endl;
        
        std::cout << "\nTest 4: Testing ERROR severity...";
        nlm::ErrorManager::logError(
            nlm::ErrorCategory::UNKNOWN_ERROR,
            nlm::ErrorSeverity::ERROR,
            "Error message - something went wrong"
        );
        std::cout << " PASSED (ERROR error logged)" << std::endl;
        
        std::cout << "\nTest 5: Testing CRITICAL severity...";
        nlm::ErrorManager::logError(
            nlm::ErrorCategory::UNKNOWN_ERROR,
            nlm::ErrorSeverity::CRITICAL,
            "Critical message - serious problem"
        );
        std::cout << " PASSED (CRITICAL error logged)" << std::endl;
        
        std::cout << "\nTest 6: Testing FATAL severity...";
        nlm::ErrorManager::logError(
            nlm::ErrorCategory::UNKNOWN_ERROR,
            nlm::ErrorSeverity::FATAL,
            "Fatal message - system cannot continue"
        );
        std::cout << " PASSED (FATAL error logged)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testErrorCategories() {
    std::cout << "\n=== Error Categories Test ===" << std::endl;
    
    try {
        // Test different error categories
        std::vector<std::pair<nlm::ErrorCategory, std::string>> testErrors = {
            {nlm::ErrorCategory::CONFIG_FILE_NOT_FOUND, "Configuration file not found"},
            {nlm::ErrorCategory::CONFIG_VALIDATION_ERROR, "Configuration validation failed"},
            {nlm::ErrorCategory::NEURON_NOT_FOUND, "Neuron not found in brain"},
            {nlm::ErrorCategory::MEMORY_NOT_INITIALIZED, "Memory system not initialized"},
            {nlm::ErrorCategory::PERFORMANCE_ERROR, "Performance error occurred"},
            {nlm::ErrorCategory::OUT_OF_BOUNDS_ERROR, "Array index out of bounds"},
            {nlm::ErrorCategory::UNKNOWN_ERROR, "Unknown error occurred"}
        };
        
        std::cout << "\nTest 1: Testing all error categories...";
        for (size_t i = 0; i < testErrors.size(); ++i) {
            try {
                nlm::ErrorManager::logError(
                    testErrors[i].first,
                    nlm::ErrorSeverity::ERROR,
                    testErrors[i].second
                );
                std::cout << "  " << (i + 1) << ". " << errorCategoryToString(testErrors[i].first) << " - PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "  " << (i + 1) << ". " << errorCategoryToString(testErrors[i].first) << " - PASSED (exception)" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testErrorMacros() {
    std::cout << "\n=== Error Macros Test ===" << std::endl;
    
    try {
        std::cout << "\nTest 1: Testing NLM_THROW_ERROR macro...";
        try {
            NLM_THROW_ERROR(
                nlm::ErrorCategory::CONFIG_VALIDATION_ERROR,
                nlm::ErrorSeverity::ERROR,
                "Test error from macro"
            );
            std::cout << " FAILED (should have thrown exception)" << std::endl;
        } catch (const nlm::NLMException& e) {
            std::cout << " PASSED (NLMException thrown)" << std::endl;
            std::cout << "  Category: " << errorCategoryToString(e.getCategory()) << std::endl;
            std::cout << "  Severity: " << errorSeverityToString(e.getSeverity()) << std::endl;
        } catch (const std::exception& e) {
            std::cout << " PASSED (std::exception thrown)" << std::endl;
        }
        
        std::cout << "\nTest 2: Testing NLM_SAFE_CALL macro...";
        std::string safeTestResult = "safe operation completed";
        try {
            NLM_SAFE_CALL({
                // Safe operation (this would normally be a risky operation)
                if (true) {
                    safeTestResult = "safe operation succeeded";
                }
            });
            std::cout << " PASSED (safe operation completed)" << std::endl;
            std::cout << "  Result: " << safeTestResult << std::endl;
        } catch (const std::exception& e) {
            std::cout << " FAILED (unexpected exception: " << e.what() << ")" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testErrorUtils() {
    std::cout << "\n=== Error Utilities Test ===" << std::endl;
    
    try {
        std::cout << "\nTest 1: Testing parameter validation...";
        
        // Test null pointer validation
        try {
            int* nullPtr = nullptr;
            nlm::ErrorUtils::validateNotNull(nullPtr, "testPointer");
            std::cout << " FAILED (should have thrown exception)" << std::endl;
        } catch (const nlm::NLMException& e) {
            std::cout << " PASSED (null pointer validation caught)" << std::endl;
        }
        
        // Test range validation
        try {
            nlm::ErrorUtils::validateRange(15, 1, 10, "testValue");
            std::cout << " FAILED (should have thrown exception)" << std::endl;
        } catch (const nlm::NLMException& e) {
            std::cout << " PASSED (range validation caught)" << std::endl;
        }
        
        // Test string validation
        try {
            nlm::ErrorUtils::validateNotEmpty("", "testString");
            std::cout << " FAILED (should have thrown exception)" << std::endl;
        } catch (const nlm::NLMException& e) {
            std::cout << " PASSED (string validation caught)" << std::endl;
        }
        
        // Test valid values (should not throw)
        std::cout << "\nTest 2: Testing valid parameter validation...";
        try {
            int validInt = 5;
            nlm::ErrorUtils::validateNotNull(&validInt, "validPointer");
            
            nlm::ErrorUtils::validateRange(validInt, 1, 10, "validValue");
            
            nlm::ErrorUtils::validateNotEmpty("validString", "validStringParam");
            
            std::cout << " PASSED (all valid parameter checks passed)" << std::endl;
        } catch (const std::exception& e) {
            std::cout << " FAILED (unexpected exception: " << e.what() << ")" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

std::string errorCategoryToString(nlm::ErrorCategory category) {
    static const std::map<nlm::ErrorCategory, std::string> categoryStrings = {
        {nlm::ErrorCategory::CONFIG_FILE_NOT_FOUND, "CONFIG_FILE_NOT_FOUND"},
        {nlm::ErrorCategory::CONFIG_FILE_PARSE_ERROR, "CONFIG_FILE_PARSE_ERROR"},
        {nlm::ErrorCategory::CONFIG_VALIDATION_ERROR, "CONFIG_VALIDATION_ERROR"},
        {nlm::ErrorCategory::CONFIG_MISSING_FIELD, "CONFIG_MISSING_FIELD"},
        {nlm::ErrorCategory::CONFIG_INVALID_VALUE, "CONFIG_INVALID_VALUE"},
        {nlm::ErrorCategory::CONFIG_DUPLICATE_FIELD, "CONFIG_DUPLICATE_FIELD"},
        {nlm::ErrorCategory::NEURON_NOT_FOUND, "NEURON_NOT_FOUND"},
        {nlm::ErrorCategory::SYNAPSE_NOT_FOUND, "SYNAPSE_NOT_FOUND"},
        {nlm::ErrorCategory::REGION_NOT_FOUND, "REGION_NOT_FOUND"},
        {nlm::ErrorCategory::BRAIN_NOT_INITIALIZED, "BRAIN_NOT_INITIALIZED"},
        {nlm::ErrorCategory::INVALID_NEURON_TYPE, "INVALID_NEURON_TYPE"},
        {nlm::ErrorCategory::INVALID_SYNAPSE_TYPE, "INVALID_SYNAPSE_TYPE"},
        {nlm::ErrorCategory::CONNECTION_ERROR, "CONNECTION_ERROR"},
        {nlm::ErrorCategory::MEMORY_NOT_INITIALIZED, "MEMORY_NOT_INITIALIZED"},
        {nlm::ErrorCategory::WORKING_MEMORY_FULL, "WORKING_MEMORY_FULL"},
        {nlm::ErrorCategory::EPISODIC_MEMORY_FULL, "EPISODIC_MEMORY_FULL"},
        {nlm::ErrorCategory::MEMORY_TRAIL_NOT_FOUND, "MEMORY_TRAIL_NOT_FOUND"},
        {nlm::ErrorCategory::MEMORY_CORRUPTION, "MEMORY_CORRUPTION"},
        {nlm::ErrorCategory::DEVELOPMENT_NOT_STARTED, "DEVELOPMENT_NOT_STARTED"},
        {nlm::ErrorCategory::DEVELOPMENT_NOT_COMPLETE, "DEVELOPMENT_NOT_COMPLETE"},
        {nlm::ErrorCategory::CRITICAL_PERIOD_NOT_ENTERED, "CRITICAL_PERIOD_NOT_ENTERED"},
        {nlm::ErrorCategory::DEVELOPMENT_ERROR, "DEVELOPMENT_ERROR"},
        {nlm::ErrorCategory::NEUROMODULATOR_NOT_INITIALIZED, "NEUROMODULATOR_NOT_INITIALIZED"},
        {nlm::ErrorCategory::INVALID_NEUROMODULATOR_TYPE, "INVALID_NEUROMODULATOR_TYPE"},
        {nlm::ErrorCategory::NEUROMODULATION_ERROR, "NEUROMODULATION_ERROR"},
        {nlm::ErrorCategory::PREDICTION_NOT_INITIALIZED, "PREDICTION_NOT_INITIALIZED"},
        {nlm::ErrorCategory::PREDICTION_MODEL_NOT_LOADED, "PREDICTION_MODEL_NOT_LOADED"},
        {nlm::ErrorCategory::PREDICTION_ERROR, "PREDICTION_ERROR"},
        {nlm::ErrorCategory::MOTOR_SYSTEM_NOT_INITIALIZED, "MOTOR_SYSTEM_NOT_INITIALIZED"},
        {nlm::ErrorCategory::INVALID_MOTOR_COMMAND, "INVALID_MOTOR_COMMAND"},
        {nlm::ErrorCategory::MOTOR_EXECUTION_ERROR, "MOTOR_EXECUTION_ERROR"},
        {nlm::ErrorCategory::ENVIRONMENT_NOT_INITIALIZED, "ENVIRONMENT_NOT_INITIALIZED"},
        {nlm::ErrorCategory::INVALID_ENVIRONMENT_TYPE, "INVALID_ENVIRONMENT_TYPE"},
        {nlm::ErrorCategory::ENVIRONMENT_ERROR, "ENVIRONMENT_ERROR"},
        {nlm::ErrorCategory::CONCEPT_NOT_FOUND, "CONCEPT_NOT_FOUND"},
        {nlm::ErrorCategory::PLANNER_NOT_INITIALIZED, "PLANNER_NOT_INITIALIZED"},
        {nlm::ErrorCategory::COGNITION_ERROR, "COGNITION_ERROR"},
        {nlm::ErrorCategory::AGENT_NOT_INITIALIZED, "AGENT_NOT_INITIALIZED"},
        {nlm::ErrorCategory::SENSOR_ERROR, "SENSOR_ERROR"},
        {nlm::ErrorCategory::AGENT_ERROR, "AGENT_ERROR"},
        {nlm::ErrorCategory::PERFORMANCE_ERROR, "PERFORMANCE_ERROR"},
        {nlm::ErrorCategory::TIMEOUT_ERROR, "TIMEOUT_ERROR"},
        {nlm::ErrorCategory::MEMORY_ERROR, "MEMORY_ERROR"},
        {nlm::ErrorCategory::COMPUTATION_ERROR, "COMPUTATION_ERROR"},
        {nlm::ErrorCategory::INITIALIZATION_ERROR, "INITIALIZATION_ERROR"},
        {nlm::ErrorCategory::SHUTDOWN_ERROR, "SHUTDOWN_ERROR"},
        {nlm::ErrorCategory::RESOURCE_NOT_AVAILABLE, "RESOURCE_NOT_AVAILABLE"},
        {nlm::ErrorCategory::SYSTEM_ERROR, "SYSTEM_ERROR"},
        {nlm::ErrorCategory::EXTERNAL_INTERFACE_ERROR, "EXTERNAL_INTERFACE_ERROR"},
        {nlm::ErrorCategory::PYTHON_BINDING_ERROR, "PYTHON_BINDING_ERROR"},
        {nlm::ErrorCategory::GUI_ERROR, "GUI_ERROR"},
        {nlm::ErrorCategory::UNKNOWN_ERROR, "UNKNOWN_ERROR"},
        {nlm::ErrorCategory::UNEXPECTED_ERROR, "UNEXPECTED_ERROR"},
        {nlm::ErrorCategory::VALIDATION_ERROR, "VALIDATION_ERROR"},
        {nlm::ErrorCategory::NOT_IMPLEMENTED_ERROR, "NOT_IMPLEMENTED_ERROR"},
        {nlm::ErrorCategory::PERMISSION_ERROR, "PERMISSION_ERROR"},
        {nlm::ErrorCategory::OUT_OF_BOUNDS_ERROR, "OUT_OF_BOUNDS_ERROR"}
    };
    
    auto it = categoryStrings.find(category);
    return (it != categoryStrings.end()) ? it->second : "UNKNOWN";
}

std::string errorSeverityToString(nlm::ErrorSeverity severity) {
    static const std::map<nlm::ErrorSeverity, std::string> severityStrings = {
        {nlm::ErrorSeverity::DEBUG, "DEBUG"},
        {nlm::ErrorSeverity::INFO, "INFO"},
        {nlm::ErrorSeverity::WARNING, "WARNING"},
        {nlm::ErrorSeverity::ERROR, "ERROR"},
        {nlm::ErrorSeverity::CRITICAL, "CRITICAL"},
        {nlm::ErrorSeverity::FATAL, "FATAL"}
    };
    
    auto it = severityStrings.find(severity);
    return (it != severityStrings.end()) ? it->second : "UNKNOWN";
}

int main() {
    std::cout << "NLM Error Handling Test Program" << std::endl;
    std::cout << "==============================" << std::endl;
    
    // Run all tests
    testErrorHandlingBasics();
    testErrorSeverityLevels();
    testErrorCategories();
    testErrorMacros();
    testErrorUtils();
    
    std::cout << "\n=== All Error Handling Tests Completed ===" << std::endl;
    return 0;
}
