// Error Handling Tests
#include "core/Error/Error.hpp"
#include <cassert>
#include <iostream>

namespace test_error {

void testErrorCreation() {
    std::cout << "  Testing error creation..." << std::endl;
    
    // Test base NLMError
    nlm::NLMError baseError("Test error", nlm::ErrorCategory::Validation, "Test context");
    assert(baseError.getCategory() == nlm::ErrorCategory::Validation);
    assert(std::string(baseError.what()).find("Test error") != std::string::npos);
    
    // Test ConfigurationError
    nlm::ConfigurationError configError("Missing key", "database_url");
    assert(configError.getCategory() == nlm::ErrorCategory::Configuration);
    assert(configError.getKey() == "database_url");
    
    // Test MemoryError
    nlm::MemoryError memoryError("Allocation failed", 1024 * 1024);
    assert(memoryError.getCategory() == nlm::ErrorCategory::Memory);
    assert(memoryError.getAllocatedBytes() == 1024 * 1024);
    
    // Test NeuralError
    nlm::NeuralError neuralError("Forward pass failed", "forward");
    assert(neuralError.getCategory() == nlm::ErrorCategory::Neural);
    assert(neuralError.getOperation() == "forward");
    
    // Test FileError
    nlm::FileError fileError("Read failed", "/path/to/file.txt", nlm::FileOperation::Read);
    assert(fileError.getCategory() == nlm::ErrorCategory::File);
    assert(fileError.getFilepath() == "/path/to/file.txt");
    assert(fileError.getOperation() == nlm::FileOperation::Read);
    
    // Test SystemError
    nlm::SystemError systemError("CPU limit exceeded", nlm::SystemResource::CPU);
    assert(systemError.getCategory() == nlm::ErrorCategory::System);
    assert(systemError.getResource() == nlm::SystemResource::CPU);
    
    std::cout << "    Error creation tests passed" << std::endl;
}

void testErrorCatalog() {
    std::cout << "  Testing error catalog..." << std::endl;
    
    // Test message retrieval
    std::string msg = nlm::ErrorCatalog::getMessage("validation_null_pointer");
    assert(!msg.empty());
    
    // Test message addition
    nlm::ErrorCatalog::addMessage("test_custom", "Custom error: {param}");
    std::string custom = nlm::ErrorCatalog::getMessage("test_custom");
    assert(custom.find("Custom error:") != std::string::npos);
    
    // Test message existence
    assert(nlm::ErrorCatalog::hasMessage("validation_null_pointer"));
    assert(!nlm::ErrorCatalog::hasMessage("nonexistent_key"));
    
    std::cout << "    Error catalog tests passed" << std::endl;
}

void testErrorUtilsValidation() {
    std::cout << "  Testing error utilities validation..." << std::endl;
    
    // Test null pointer validation (should fail and log error)
    bool nullResult = nlm::ErrorUtils::validateNotNull(nullptr, "ptr", "Context");
    assert(nullResult == false);
    
    // Test range validation (should fail and log error)
    bool rangeResult = nlm::ErrorUtils::validateRange(150, 0, 100, "temperature");
    assert(rangeResult == false);
    
    // Test range validation (should pass)
    bool rangePass = nlm::ErrorUtils::validateRange(50, 0, 100, "temperature");
    assert(rangePass == true);
    
    // Test type validation (should fail)
    bool typeFail = nlm::ErrorUtils::validateType("not_a_number", "numeric");
    assert(typeFail == false);
    
    // Test type validation (should pass)
    bool typePass = nlm::ErrorUtils::validateType("123", "numeric");
    assert(typePass == true);
    
    std::cout << "    Error utilities validation tests passed" << std::endl;
}

void testErrorUtilsRecovery() {
    std::cout << "  Testing error recovery..." << std::endl;
    
    // Test memory error recovery suggestion
    nlm::MemoryError memError("Allocation failed", 1024 * 1024);
    std::string recovery = nlm::ErrorUtils::suggestRecovery(memError);
    assert(recovery.find("Memory recovery") != std::string::npos);
    
    // Test file error recovery suggestion
    nlm::FileError fileError("Read failed", "/test/file.txt", nlm::FileOperation::Read);
    std::string fileRecovery = nlm::ErrorUtils::suggestRecovery(fileError);
    assert(fileRecovery.find("File recovery") != std::string::npos);
    
    // Test configuration error recovery suggestion
    nlm::ConfigurationError configError("Missing key", "config_key");
    std::string configRecovery = nlm::ErrorUtils::suggestRecovery(configError);
    assert(configRecovery.find("Configuration recovery") != std::string::npos);
    
    // Test retry suggestion
    assert(nlm::ErrorUtils::shouldRetry(memError) == false);
    
    std::cout << "    Error recovery tests passed" << std::endl;
}

void testErrorTranslation() {
    std::cout << "  Testing error translation..." << std::endl;
    
    // Test standard exception translation
    try {
        throw std::runtime_error("Standard error");
    } catch (const std::exception& e) {
        nlm::NLMError translated = nlm::exception::translate(e, nlm::ErrorCategory::Neural);
        assert(translated.getCategory() == nlm::ErrorCategory::Neural);
        assert(std::string(translated.what()).find("Standard error") != std::string::npos);
    }
    
    // Test system error translation
    nlm::NLMError sysError = nlm::exception::translateSystemError(EACCES, "Permission denied");
    assert(sysError.getCategory() == nlm::ErrorCategory::System);
    
    std::cout << "    Error translation tests passed" << std::endl;
}

void testErrorFormatting() {
    std::cout << "  Testing error formatting..." << std::endl;
    
    nlm::ConfigurationError configError("Test config error", "test_key", "Test context", 42, "test.cpp", "testFunction");
    std::string formatted = nlm::ErrorUtils::formatError(configError);
    assert(formatted.find("Configuration") != std::string::npos);
    assert(formatted.find("test key") != std::string::npos);
    
    std::cout << "    Error formatting tests passed" << std::endl;
}

void testErrorHandlingSuite() {
    std::cout << "Error Handling Suite" << std::endl;
    std::cout << "=======================" << std::endl;
    
    testErrorCreation();
    testErrorCatalog();
    testErrorUtilsValidation();
    testErrorUtilsRecovery();
    testErrorTranslation();
    testErrorFormatting();
    
    std::cout << "=======================" << std::endl;
    std::cout << "All error handling tests passed!" << std::endl;
}

} // namespace test_error

// Test runner
int main() {
    test_error::testErrorHandlingSuite();
    return 0;
}