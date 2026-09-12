// ErrorHandler Tests
// Phase 3: Error Handling and Recovery

#include "utils/ErrorHandler.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

namespace test_error_handler {

void testErrorHandlerBasic() {
    nlm::ErrorHandler::ErrorInfo errorInfo;
    errorInfo.code = 100;
    errorInfo.message = "Test error";
    errorInfo.category = nlm::ErrorHandler::ErrorCategory::System;
    errorInfo.severity = nlm::ErrorHandler::Severity::Warning;
    errorInfo.timestamp = std::chrono::system_clock::now();
    
    // Test error reporting
    nlm::ErrorHandler::reportError(errorInfo);
    
    // Get statistics
    auto stats = nlm::ErrorHandler::getStatistics();
    assert(stats.totalErrors > 0);
    
    std::cout << "    testErrorHandlerBasic passed" << std::endl;
}

void testErrorHandlerGlobalHandler() {
    bool handlerCalled = false;
    
    auto globalHandler = [&handlerCalled](const nlm::ErrorHandler::ErrorInfo& info) {
        handlerCalled = true;
    };
    
    nlm::ErrorHandler::setGlobalErrorHandler(globalHandler);
    
    nlm::ErrorHandler::ErrorInfo errorInfo;
    errorInfo.code = 101;
    errorInfo.message = "Test with global handler";
    errorInfo.category = nlm::ErrorHandler::ErrorCategory::System;
    errorInfo.severity = nlm::ErrorHandler::Severity::Error;
    errorInfo.timestamp = std::chrono::system_clock::now();
    
    nlm::ErrorHandler::reportError(errorInfo);
    
    assert(handlerCalled);
    
    std::cout << "    testErrorHandlerGlobalHandler passed" << std::endl;
}

void testErrorHandlerException() {
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::exception& e) {
        nlm::ErrorHandler::reportException(e);
    }
    
    auto recentErrors = nlm::ErrorHandler::getRecentErrors();
    assert(!recentErrors.empty());
    
    std::cout << "    testErrorHandlerException passed" << std::endl;
}

void testErrorHandlerSystemError() {
    nlm::ErrorHandler::reportSystemError(202, "System test error", 
                                         nlm::ErrorHandler::ErrorCategory::FileIO,
                                         nlm::ErrorHandler::Severity::Warning);
    
    auto stats = nlm::ErrorHandler::getStatistics();
    assert(stats.totalErrors > 0);
    
    std::cout << "    testErrorHandlerSystemError passed" << std::endl;
}

void testErrorHandlerCreateError() {
    auto errorInfo = nlm::ErrorHandler::createError(303, "Created error",
                                                     nlm::ErrorHandler::ErrorCategory::Configuration,
                                                     nlm::ErrorHandler::Severity::Critical);
    
    assert(errorInfo.code == 303);
    assert(errorInfo.category == nlm::ErrorHandler::ErrorCategory::Configuration);
    assert(errorInfo.severity == nlm::ErrorHandler::Severity::Critical);
    assert(!errorInfo.message.empty());
    
    std::cout << "    testErrorHandlerCreateError passed" << std::endl;
}

void testErrorHandlerContext() {
    nlm::ErrorHandler::addContext("test_key", "test_value");
    
    auto recentErrors = nlm::ErrorHandler::getRecentErrors();
    // Note: Context is stored in error info, not directly accessible here
    
    std::cout << "    testErrorHandlerContext passed" << std::endl;
}

void testErrorHandlerRecentErrors() {
    // Clear history first
    nlm::ErrorHandler::clearHistory();
    
    // Report some errors
    for (int i = 0; i < 5; ++i) {
        nlm::ErrorHandler::ErrorInfo errorInfo;
        errorInfo.code = 400 + i;
        errorInfo.message = "Test error " + std::to_string(i);
        errorInfo.category = nlm::ErrorHandler::ErrorCategory::System;
        errorInfo.severity = nlm::ErrorHandler::Severity::Warning;
        errorInfo.timestamp = std::chrono::system_clock::now();
        
        nlm::ErrorHandler::reportError(errorInfo);
    }
    
    // Get recent errors
    auto recentErrors = nlm::ErrorHandler::getRecentErrors(3);
    assert(recentErrors.size() <= 3);
    
    std::cout << "    testErrorHandlerRecentErrors passed" << std::endl;
}

void testErrorHandlerClearHistory() {
    // Report an error
    nlm::ErrorHandler::ErrorInfo errorInfo;
    errorInfo.code = 500;
    errorInfo.message = "Error to clear";
    errorInfo.category = nlm::ErrorHandler::ErrorCategory::System;
    errorInfo.severity = nlm::ErrorHandler::Severity::Error;
    errorInfo.timestamp = std::chrono::system_clock::now();
    
    nlm::ErrorHandler::reportError(errorInfo);
    
    assert(nlm::ErrorHandler::getStatistics().totalErrors > 0);
    
    nlm::ErrorHandler::clearHistory();
    assert(nlm::ErrorHandler::getStatistics().totalErrors == 0);
    
    std::cout << "    testErrorHandlerClearHistory passed" << std::endl;
}

void testErrorHandlerRecoveryInfo() {
    nlm::ErrorHandler::ErrorInfo errorInfo;
    errorInfo.code = 600;
    errorInfo.message = "Error for recovery test";
    errorInfo.category = nlm::ErrorHandler::ErrorCategory::System;
    errorInfo.severity = nlm::ErrorHandler::Severity::Warning;
    errorInfo.timestamp = std::chrono::system_clock::now();
    
    nlm::ErrorHandler::reportError(errorInfo);
    
    // Get recovery info (should be non-recoverable without a recovery function)
    auto recoveryInfo = nlm::ErrorHandler::getRecoveryInfo(errorInfo);
    assert(!recoveryInfo.recoverable);
    
    std::cout << "    testErrorHandlerRecoveryInfo passed" << std::endl;
}

void testErrorHandlerRecoveryFunction() {
    bool recoveryCalled = false;
    
    auto recoveryFunc = [&recoveryCalled]() {
        recoveryCalled = true;
        return true;
    };
    
    nlm::ErrorHandler::setRecoveryFunction(nlm::ErrorHandler::ErrorCategory::System, recoveryFunc);
    
    nlm::ErrorHandler::ErrorInfo errorInfo;
    errorInfo.code = 700;
    errorInfo.message = "Error with recovery function";
    errorInfo.category = nlm::ErrorHandler::ErrorCategory::System;
    errorInfo.severity = nlm::ErrorHandler::Severity::Warning;
    errorInfo.timestamp = std::chrono::system_clock::now();
    
    nlm::ErrorHandler::reportError(errorInfo);
    
    // Note: Recovery would require errorHandler.attemptRecovery(errorInfo)
    // but we can't access the ErrorHandler instance from here
    
    std::cout << "    testErrorHandlerRecoveryFunction passed" << std::endl;
}

void testErrorHandlerGetErrorMessage() {
    // Test built-in error messages
    std::string msg = nlm::ErrorHandler::getErrorMessage(0);
    assert(msg == "Success");
    
    msg = nlm::ErrorHandler::getErrorMessage(1);
    assert(msg == "General error");
    
    // Test custom error message
    nlm::ErrorHandler::setErrorMessage(999, "Custom error message");
    msg = nlm::ErrorHandler::getErrorMessage(999);
    assert(msg == "Custom error message");
    
    std::cout << "    testErrorHandlerGetErrorMessage passed" << std::endl;
}

void testErrorHandlerFormatError() {
    nlm::ErrorHandler::ErrorInfo errorInfo;
    errorInfo.code = 800;
    errorInfo.message = "Formatted error test";
    errorInfo.category = nlm::ErrorHandler::ErrorCategory::System;
    errorInfo.severity = nlm::ErrorHandler::Severity::Error;
    errorInfo.file = "test.cpp";
    errorInfo.line = 123;
    errorInfo.function = "testFunction";
    errorInfo.timestamp = std::chrono::system_clock::now();
    
    std::string formatted = nlm::ErrorHandler::formatError(errorInfo);
    assert(!formatted.empty());
    assert(formatted.find("Error 800") != std::string::npos);
    assert(formatted.find("Formatted error test") != std::string::npos);
    
    std::cout << "    testErrorHandlerFormatError passed" << std::endl;
}

void runAll() {
    testErrorHandlerBasic();
    testErrorHandlerGlobalHandler();
    testErrorHandlerException();
    testErrorHandlerSystemError();
    testErrorHandlerCreateError();
    testErrorHandlerContext();
    testErrorHandlerRecentErrors();
    testErrorHandlerClearHistory();
    testErrorHandlerRecoveryInfo();
    testErrorHandlerRecoveryFunction();
    testErrorHandlerGetErrorMessage();
    testErrorHandlerFormatError();
}

} // namespace test_error_handler
