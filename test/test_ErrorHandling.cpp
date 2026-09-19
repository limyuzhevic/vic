// Test file for Error Handling Framework
// This file tests the new NLM error handling classes and utilities

#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

namespace nlm {

class ErrorHandlingTest {
public:
    static void runAllTests() {
        std::cout << "=== NLM Error Handling Framework Tests ===\n";
        
        // Test error classes
        testErrorClass();
        
        // Test validation utilities
        testParameterValidator();
        
        // Test error handling utilities
        testErrorHandler();
        
        // Test macros and utilities
        testMacrosAndUtilities();
        
        // Test config with error handling
        testConfigErrorHandling();
        
        std::cout << "\n=== All tests passed! ===\n";
    }
    
private:
    static void testErrorClass() {
        std::cout << "Testing NLMError class...\n";
        
        try {
            throw NLMError("Test error message", ErrorSeverity::Error, ErrorCategory::Parameter, __FILE__, __LINE__, __func__);
        } catch (const NLMError& e) {
            assert(e.getMessage() == "Test error message");
            assert(e.getSeverity() == ErrorSeverity::Error);
            assert(e.getCategory() == ErrorCategory::Parameter);
            assert(e.getFile() == __FILE__);
            assert(e.getLine() == __LINE__ - 2); // Account for assert
        }
        
        // Test error report generation
        NLMError error("Test error", ErrorSeverity::Warning, ErrorCategory::System, __FILE__, __LINE__, __func__);
        error.setContext("Test context");
        error.addDetail("key1", "value1");
        
        std::string report = error.getReport();
        assert(report.find("Test error") != std::string::npos);
        assert(report.find("Test context") != std::string::npos);
        assert(report.find("key1") != std::string::npos);
        
        std::cout << "  NLMError class tests passed\n";
    }
    
    static void testParameterValidator() {
        std::cout << "Testing ParameterValidator...\n";
        
        // Test range validation
        auto result1 = ParameterValidator::validateRange(5.0, 0.0, 10.0, "value");
        assert(result1.isValid());
        
        auto result2 = ParameterValidator::validateRange(15.0, 0.0, 10.0, "value");
        assert(!result2.isValid());
        assert(result2.getMessage().find("out of range") != std::string::npos);
        
        // Test positive validation
        auto result3 = ParameterValidator::validatePositive(5.0, "value");
        assert(result3.isValid());
        
        auto result4 = ParameterValidator::validatePositive(-5.0, "value");
        assert(!result4.isValid());
        
        // Test string validation
        auto result5 = ParameterValidator::validateString("valid", 0, 10, "name");
        assert(result5.isValid());
        
        auto result6 = ParameterValidator::validateString("", 5, 10, "name");
        assert(!result6.isValid());
        
        // Test config key validation
        auto result7 = ParameterValidator::validateConfigKey("valid_key");
        assert(result7.isValid());
        
        auto result8 = ParameterValidator::validateConfigKey("invalid key");
        assert(!result8.isValid());
        
        // Test neuron ID validation
        auto result9 = ParameterValidator::validateNeuronId(1);
        assert(result9.isValid());
        
        auto result10 = ParameterValidator::validateNeuronId(0);
        assert(!result10.isValid());
        
        std::cout << "  ParameterValidator tests passed\n";
    }
    
    static void testErrorHandler() {
        std::cout << "Testing ErrorHandler...\n";
        
        // Test error reporting
        NLMError testError("Test error", ErrorSeverity::Error, ErrorCategory::Parameter);
        ErrorHandler::reportError(testError);
        
        // Test warning reporting
        ErrorHandler::reportWarning("Test warning", "test context");
        
        // Test info reporting
        ErrorHandler::reportInfo("Test info", "test context");
        
        // Test safe allocate/deallocate
        int* ptr = ErrorHandler::safeAllocate<int>(10, "test allocation");
        assert(ptr != nullptr);
        ErrorHandler::safeDeallocate(ptr, "test deallocation");
        
        // Test makeSafePtr
        int* rawPtr = new int(42);
        auto safePtr = ErrorHandler::makeSafePtr(rawPtr, "test safe ptr");
        assert(safePtr != nullptr);
        assert(*safePtr == 42);
        
        std::cout << "  ErrorHandler tests passed\n";
    }
    
    static void testMacrosAndUtilities() {
        std::cout << "Testing macros and utilities...\n";
        
        // Test VALIDATE_PARAM macro
        try {
            VALIDATE_PARAM(5.0, ParameterValidator::validatePositive);
        } catch (const NLMError&) {
            // Expected to fail with negative value
        }
        
        // Test SAFE_CALL macro
        try {
            SAFE_CALL(throw NLMError("Test", ErrorSeverity::Error, ErrorCategory::Internal));
        } catch (const NLMError&) {
            // Expected to catch wrapped exception
        }
        
        // Test LOG_THROW_ERROR macro
        try {
            LOG_THROW_ERROR(ErrorSeverity::Error, ErrorCategory::System, "Test error");
        } catch (const NLMError& e) {
            assert(e.getMessage() == "Test error");
        }
        
        // Test getGlobalErrorHandler
        auto& handler = getGlobalErrorHandler();
        // The handler should be accessible
        
        // Test common error instances
        auto& memError = getMemoryAllocationError("test");
        auto& paramError = getParameterError("param1", "expected", "actual");
        auto& configError = getConfigurationError("key", "reason");
        auto& nullError = getNullPointerError("type", "test");
        
        std::cout << "  Macros and utilities tests passed\n";
    }
    
    static void testConfigErrorHandling() {
        std::cout << "Testing Config with error handling...\n";
        
        auto config = std::make_shared<Config>();
        
        // Test validation during initialization
        config->set("test_key", "test_value");
        assert(config->get<std::string>("test_key").value() == "test_value");
        
        // Test validation during retrieval
        config->set("number", 42);
        assert(config->get<int>("number").value() == 42);
        
        // Test validation during set operations
        try {
            config->set("", "empty_key");
        } catch (const NLMError&) {
            // Expected to fail with empty key
        }
        
        // Test parameter validation in get methods
        config->set("float_val", 3.14);
        assert(config->get<double>("float_val").value() == 3.14);
        
        std::cout << "  Config error handling tests passed\n";
    }
};

} // namespace nlm

// Simple test runner
int main() {
    nlm::ErrorHandlingTest::runAllTests();
    return 0;
}
