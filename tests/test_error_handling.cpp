#include "core/TestFramework/TestFramework.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace test_error_handling {

class ErrorHandlingTestSuite : public TestSuite {
public:
    std::string getName() const override {
        return "ErrorHandling";
    }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test 1: Exception creation and handling
            testExceptionCreation();
            results.emplace_back("ExceptionCreation", true, "Exception creation test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("ExceptionCreation", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 2: Error code conversion
            testErrorCodeConversion();
            results.emplace_back("ErrorCodeConversion", true, "Error code conversion test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("ErrorCodeConversion", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        return results;
    }
    
private:
    double getDuration(std::chrono::high_resolution_clock::time_point start) {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    void testExceptionCreation() {
        // Test basic NLM exception creation
        nlm::NLMException ex("Test error", nlm::NLMErrors::ConfigParseError);
        ASSERT_TRUE(std::string(ex.what()) == "Test error");
        ASSERT_TRUE(ex.getErrorCode() == nlm::NLMErrors::ConfigParseError);
        
        // Test specific exception types
        nlm::ConfigException configEx("Config error", nlm::NLMErrors::ConfigInvalidValue);
        ASSERT_TRUE(configEx.getErrorCode() == nlm::NLMErrors::ConfigInvalidValue);
        
        nlm::MemoryException memEx("Memory error");
        ASSERT_TRUE(memEx.getErrorCode() == nlm::NLMErrors::MemoryAllocationFailed);
    }
    
    void testErrorCodeConversion() {
        // Test error code to std::error_code conversion
        nlm::NLMException ex("Test", nlm::NLMErrors::UnknownError);
        auto errorCode = ex.toErrorCode();
        
        // Error codes should be valid
        ASSERT_TRUE(static_cast<int>(errorCode.value()) == 999);
        
        // Test all error code ranges
        ASSERT_TRUE(static_cast<int>(nlm::NLMErrors::Success) == 0);
        ASSERT_TRUE(static_cast<int>(nlm::NLMErrors::ConfigFileNotFound) == 1);
        ASSERT_TRUE(static_cast<int>(nlm::NLMErrors::MemoryAllocationFailed) == 4);
        ASSERT_TRUE(static_cast<int>(nlm::NLMErrors::UnknownError) == 999);
    }
};

} // namespace test_error_handling

// Test runner
int main() {
    nlm::TestRunner runner;
    
    // Add test suite
    runner.addSuite(std::make_unique<test_error_handling::ErrorHandlingTestSuite>());
    
    // Run all tests
    auto results = runner.runAllTests();
    
    // Print results
    runner.printResults(results);
    
    // Exit with error code if any tests failed
    if (runner.getFailedTests() > 0) {
        return 1;
    }
    
    return 0;
}
