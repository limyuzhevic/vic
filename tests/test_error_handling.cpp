// Error Handling Tests
#include "ErrorHandling/ErrorHandling.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace test_error_handling {

void testErrorHandlingBasic() {
    std::cout << "    testErrorHandlingBasic passed" << std::endl;
}

void testErrorCategory() {
    // Test error category functionality
    auto ec = nlm::nlm_error_category();
    assert(ec.name() == std::string("nlm"));
    
    // Test specific error messages
    std::string successMsg = ec.message(static_cast<int>(nlm::NLMErrorCategory::Success));
    assert(successMsg == "Success");
    
    std::string configErrorMsg = ec.message(static_cast<int>(nlm::NLMErrorCategory::ConfigError));
    assert(configErrorMsg == "Configuration error");
    
    std::string brainErrorMsg = ec.message(static_cast<int>(nlm::NLMErrorCategory::BrainError));
    assert(brainErrorMsg == "Brain error");
    
    std::cout << "    testErrorCategory passed" << std::endl;
}

void testNLMException() {
    // Test exception creation
    nlm::NLMException exc1(nlm::NLMErrorCategory::ConfigFileNotFound, "File not found", "test_context");
    assert(exc1.getErrorCode() == nlm::NLMErrorCategory::ConfigFileNotFound);
    assert(exc1.getMessage() == "File not found");
    assert(exc1.getContext() == "test_context");
    
    // Test exception message
    std::string what = exc1.what();
    assert(what.find("test_context") != std::string::npos);
    assert(what.find("File not found") != std::string::npos);
    
    // Test exception from error category
    nlm::NLMException exc2 = nlm::NLMException::fromErrorCategory(nlm::NLMErrorCategory::BrainInitFailed, "brain_init");
    assert(exc2.getErrorCode() == nlm::NLMErrorCategory::BrainInitFailed);
    
    std::cout << "    testNLMException passed" << std::endl;
}

void testErrorMacros() {
    // Test that error macros work (they're mainly for convenience)
    try {
        NLM_THROW_ERROR(ConfigError, "test_context");
        assert(false && "Should have thrown");
    } catch (const nlm::NLMException& e) {
        assert(e.getErrorCode() == nlm::NLMErrorCategory::ConfigError);
    }
    
    try {
        NLM_THROW_CONFIG_ERROR(ConfigInvalidValue, "test_context");
        assert(false && "Should have thrown");
    } catch (const nlm::NLMException& e) {
        assert(e.getErrorCode() == nlm::NLMErrorCategory::ConfigInvalidValue);
    }
    
    try {
        NLM_THROW_RUNTIME_ERROR("Runtime error", "test_context");
        assert(false && "Should have thrown");
    } catch (const nlm::NLMException& e) {
        assert(e.getErrorCode() == nlm::NLMErrorCategory::Unknown);
    }
    
    std::cout << "    testErrorMacros passed" << std::endl;
}

void testErrorHandler() {
    // Test error handler instance
    nlm::ErrorHandler& handler = nlm::ErrorHandler::getInstance();
    
    // Test setThrowErrors
    handler.setThrowErrors(false);
    
    // Test error callback
    int errorCount = 0;
    handler.setErrorCallback([&errorCount](const nlm::NLMException& e) {
        errorCount++;
    });
    
    // Test handleError (should log, not throw)
    nlm::NLMException testError(nlm::NLMErrorCategory::ConfigError, "Test error");
    handler.handleError(testError);
    
    // Check that callback was called
    assert(errorCount == 1);
    
    // Get error summary
    std::string summary = handler.getErrorSummary();
    assert(summary.find("NLM Error Handler") != std::string::npos);
    
    std::cout << "    testErrorHandler passed" << std::endl;
}

void testErrorValidation() {
    // Test parameter validation macro
    nlm_validate_param(true, InvalidParameter, "test");
    
    try {
        nlm_validate_param(false, InvalidParameter, "test");
        assert(false && "Should have thrown");
    } catch (const nlm::NLMException& e) {
        assert(e.getErrorCode() == nlm::NLMErrorCategory::InvalidParameter);
    }
    
    std::cout << "    testErrorValidation passed" << std::endl;
}

void testErrorIntegration() {
    // Test that errors can be thrown and caught properly
    try {
        // Create an error that will be thrown
        nlm::NLMException exc(nlm::NLMErrorCategory::OutOfMemory, "Memory allocation failed", "AllocationTest");
        
        // Verify the error has correct properties
        assert(exc.getErrorCode() == nlm::NLMErrorCategory::OutOfMemory);
        assert(exc.getMessage() == "Memory allocation failed");
        assert(exc.getContext() == "AllocationTest");
        
        // Verify the full error message includes context
        std::string fullMsg = exc.what();
        assert(fullMsg.find("AllocationTest") != std::string::npos);
        assert(fullMsg.find("Memory allocation failed") != std::string::npos);
        
    } catch (const std::exception& e) {
        assert(false && "Should not catch NLMException as std::exception");
    }
    
    std::cout << "    testErrorIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Error Handling tests..." << std::endl;
    testErrorHandlingBasic();
    testErrorCategory();
    testNLMException();
    testErrorMacros();
    testErrorHandler();
    testErrorValidation();
    testErrorIntegration();
    std::cout << "\nAll Error Handling tests passed!" << std::endl;
}

} // namespace test_error_handling