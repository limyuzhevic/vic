#include "core/Error/Error.hpp"
#include "core/Error/ErrorRecovery.hpp"
#include "core/Error/ErrorReporting.hpp"
#include <iostream>
#include <cassert>

void testErrorBasic() {
    std::cout << "Testing basic error functionality..." << std::endl;
    
    // Test error creation
    try {
        throw nlm::ConfigError("Test config error");
    } catch (const nlm::ConfigError& e) {
        assert(e.getCategory() == nlm::ErrorCategory::Configuration);
        assert(e.getSeverity() == nlm::ErrorSeverity::Error);
        std::cout << "  ConfigError creation: PASSED" << std::endl;
    }
    
    // Test error reporting
    nlm::ErrorHandling::reportError("Test error report",
                                   nlm::ErrorHandling::createContext("TestComponent"),
                                   nlm::ErrorCategory::System,
                                   nlm::ErrorSeverity::Warning);
    std::cout << "  Error reporting: PASSED" << std::endl;
    
    // Test error context creation
    auto context = nlm::ErrorHandling::createContext("UnitTest");
    context.addDetail("test_key", "test_value");
    assert(context.component == "UnitTest");
    assert(context.details["test_key"] == "test_value");
    std::cout << "  Error context: PASSED" << std::endl;
    
    std::cout << "Basic error tests: PASSED" << std::endl;
}

void testErrorRecovery() {
    std::cout << "Testing error recovery functionality..." << std::endl;
    
    // Test error recovery manager
    nlm::globalErrorRecoveryManager.registerRecoveryCallback(
        nlm::ErrorCategory::System,
        [](const nlm::NLMException& error) -> bool {
            std::cout << "  Recovery callback triggered for: " << error.what() << std::endl;
            return true;
        }
    );
    
    // Test recovery attempt
    nlm::ConfigError testError("Test error for recovery");
    bool recovered = nlm::globalErrorRecoveryManager.attemptRecovery(testError);
    assert(recovered);
    std::cout << "  Error recovery: PASSED" << std::endl;
    
    // Test hasRecovery
    assert(nlm::globalErrorRecoveryManager.hasRecovery(nlm::ErrorCategory::System));
    std::cout << "  Recovery availability: PASSED" << std::endl;
    
    std::cout << "Error recovery tests: PASSED" << std::endl;
}

void testErrorReporting() {
    std::cout << "Testing error reporting utilities..." << std::endl;
    
    // Test error report generation
    std::string report = nlm::getSystemErrorReport();
    assert(!report.empty());
    assert(report.find("NLM Error") != std::string::npos || report.find("NLM COMPREHENSIVE") != std::string::npos);
    std::cout << "  Error report generation: PASSED" << std::endl;
    
    // Test error statistics
    nlm::clearSystemErrorStats();
    auto stats = nlm::ErrorMonitor::getErrorStats();
    assert(stats.totalErrors == 0);
    std::cout << "  Error statistics: PASSED" << std::endl;
    
    std::cout << "Error reporting tests: PASSED" << std::endl;
}

void testErrorMacros() {
    std::cout << "Testing error reporting macros..." << std::endl;
    
    // Test NLM_THROW_ERROR macro
    try {
        nlm::NLM_THROW_ERROR("Macro test error", nlm::ConfigError, nlm::ErrorCategory::Configuration, nlm::ErrorSeverity::Error);
    } catch (const nlm::ConfigError& e) {
        assert(e.getCategory() == nlm::ErrorCategory::Configuration);
        std::cout << "  NLM_THROW_ERROR macro: PASSED" << std::endl;
    }
    
    // Test NLM_VALIDATE macro
    bool result = false;
    do {
        NLM_VALIDATE(true, "Validation test");
        result = true;
    } while (false);
    assert(result);
    std::cout << "  NLM_VALIDATE macro: PASSED" << std::endl;
    
    std::cout << "Error macro tests: PASSED" << std::endl;
}

int main() {
    std::cout << "=== NLM Error Handling System Test ===" << std::endl;
    
    try {
        testErrorBasic();
        testErrorRecovery();
        testErrorReporting();
        testErrorMacros();
        
        std::cout << std::endl;
        std::cout << "=== All Error Handling Tests PASSED ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Error Handling Tests FAILED ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
