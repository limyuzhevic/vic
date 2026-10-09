#include <iostream>
#include <nlm/core/ErrorHandling/ErrorHandling.hpp>
#include <nlm/core/Validation/Validation.hpp>
#include <nlm/core/Integration/Integration.hpp>

int main() {
    std::cout << "=== NLM Error Handling System Test ===" << std::endl;
    
    
    // Test 1: Error logging
    std::cout << "\n1. Testing error logging..." << std::endl;
    nlm::ErrorHandling::log("Test message", nlm::ErrorSeverity::Info, "test");
    
    // Test 2: Error creation
    std::cout << "\n2. Testing error creation..." << std::endl;
    auto configError = nlm::ErrorHandling::createConfigurationError(
        nlm::ErrorCode::ConfigMissingRequired,
        "Test configuration error",
        "test_config"
    );
    
    auto fileError = nlm::ErrorHandling::createFileSystemError(
        nlm::ErrorCode::FileNotFound,
        "Test file system error",
        "test_filesystem"
    );
    
    auto memError = nlm::ErrorHandling::createMemoryError(
        nlm::ErrorCode::MemoryAllocationFailed,
        "Test memory error",
        "test_memory"
    );
    
    auto simError = nlm::ErrorHandling::createSimulationError(
        nlm::ErrorCode::SimulationStepFailed,
        "Test simulation error",
        "test_simulation"
    );
    
    auto valError = nlm::ErrorHandling::createValidationError(
        nlm::ErrorCode::ParameterValidationFailed,
        "Test validation error",
        "test_validation"
    );
    
    auto resError = nlm::ErrorHandling::createResourceError(
        nlm::ErrorCode::ResourceLimitExceeded,
        "Test resource error",
        "test_resource"
    );
    
    // Test 3: Error statistics
    std::cout << "\n3. Testing error statistics..." << std::endl;
    auto stats = nlm::ErrorHandling::getErrorStats();
    std::cout << "Total Errors: " << stats.totalErrors << std::endl;
    std::cout << "Recent Errors: " << stats.recentErrors << std::endl;
    
    // Test 4: Error report
    std::cout << "\n4. Generating error report..." << std::endl;
    nlm::ErrorHandling::reportErrors();
    
    // Test 5: Validation
    std::cout << "\n5. Testing validation..." << std::endl;
    
    // Test parameter validation
    bool valid1 = nlm::ValidationHelper::validateParameter("test_param", 5.0, 0.0, 10.0, "validation_test");
    std::cout << "Parameter validation: " << (valid1 ? "PASS" : "FAIL") << std::endl;
    
    bool valid2 = nlm::ValidationHelper::validateParameter("test_param2", -5, 0, 10, "validation_test2");
    std::cout << "Range validation (should fail): " << (valid2 ? "PASS" : "FAIL") << std::endl;
    
    // Test file existence validation
    bool valid3 = nlm::ValidationHelper::validateFileExists("nonexistent.txt", "file_validation_test");
    std::cout << "File validation (should fail): " << (valid3 ? "PASS" : "FAIL") << std::endl;
    
    // Test memory allocation validation
    bool valid4 = nlm::ValidationHelper::validateMemoryAllocation(0, "memory_validation_test");
    std::cout << "Memory validation (should fail): " << (valid4 ? "PASS" : "FAIL") << std::endl;
    
    // Test 6: Error recovery simulation
    std::cout << "\n6. Testing error recovery simulation..." << std::endl;
    
    // Create some errors
    auto error1 = nlm::ErrorHandling::createError(nlm::ErrorCode::Timeout, "Timeout error", "recovery_test");
    auto error2 = nlm::ErrorHandling::createError(nlm::ErrorCode::NetworkUnreachable, "Network unreachable error", "recovery_test");
    
    // Test recovery
    bool recovered1 = nlm::ErrorHandling::attemptRecovery(error1);
    bool recovered2 = nlm::ErrorHandling::attemptRecovery(error2);
    std::cout << "Timeout recovery: " << (recovered1 ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Network unreachable recovery: " << (recovered2 ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test 7: Integration validation
    std::cout << "\n7. Testing integration validation..." << std::endl;
    bool integrationValid = nlm::validateIntegration();
    std::cout << "Integration validation: " << (integrationValid ? "PASS" : "FAIL") << std::endl;
    
    // Test 8: Error reporting
    std::cout << "\n8. Testing error reporting..." << std::endl;
    std::string report = nlm::generateIntegrationErrorReport();
    std::cout << "Integration error report:" << std::endl;
    std::cout << report << std::endl;
    
    // Test 9: Error history
    std::cout << "\n9. Testing error history..." << std::endl;
    auto history = nlm::getErrorHistory();
    std::cout << "Error history size: " << history.size() << std::endl;
    
    // Test 10: Error filtering
    std::cout << "\n10. Testing error filtering..." << std::endl;
    auto errorsByCode = nlm::getErrorsByCode(nlm::ErrorCode::ConfigMissingRequired);
    std::cout << "Errors by code (ConfigMissingRequired): " << errorsByCode.size() << std::endl;
    
    auto errorsBySeverity = nlm::getErrorsBySeverity(nlm::ErrorSeverity::Error);
    std::cout << "Errors by severity (Error): " << errorsBySeverity.size() << std::endl;
    
    std::cout << "\n=== Test completed ===" << std::endl;
    return 0;
}