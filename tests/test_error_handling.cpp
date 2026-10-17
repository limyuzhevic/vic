// Error Handling Tests
// Test the new error handling framework

#include "core/Error/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

namespace test_error_handling {

void testNLMError() {
    try {
        throw nlm::NLMError("Test NLM error");
    } catch (const nlm::NLMError& e) {
        assert(std::string(e.what()).find("NLM Error") != std::string::npos);
        std::cout << "✓ NLMError test passed" << std::endl;
    }
}

void testConfigError() {
    try {
        throw nlm::ConfigError("Test config error", "test_key");
    } catch (const nlm::ConfigError& e) {
        assert(std::string(e.what()).find("Config Error") != std::string::npos);
        assert(e.getKey() == "test_key");
        std::cout << "✓ ConfigError test passed" << std::endl;
    }
}

void testFileError() {
    try {
        throw nlm::FileError("Test file error", "/test/path", 
                            nlm::FileError::ErrorType::NotFound);
    } catch (const nlm::FileError& e) {
        assert(std::string(e.what()).find("File Error") != std::string::npos);
        assert(e.getFilePath() == "/test/path");
        assert(e.getType() == nlm::FileError::ErrorType::NotFound);
        std::cout << "✓ FileError test passed" << std::endl;
    }
}

void testValidationError() {
    try {
        throw nlm::ValidationError("Test validation error", "test_field",
                                  nlm::ValidationError::ErrorType::InvalidValue);
    } catch (const nlm::ValidationError& e) {
        assert(std::string(e.what()).find("Validation Error") != std::string::npos);
        assert(e.getField() == "test_field");
        assert(e.getType() == nlm::ValidationError::ErrorType::InvalidValue);
        std::cout << "✓ ValidationError test passed" << std::endl;
    }
}

void testInitializationError() {
    try {
        throw nlm::InitializationError("Test initialization error", "test_component");
    } catch (const nlm::InitializationError& e) {
        assert(std::string(e.what()).find("Initialization Error") != std::string::npos);
        assert(e.getComponent() == "test_component");
        std::cout << "✓ InitializationError test passed" << std::endl;
    }
}

void testCheckNotNull() {
    // Test with null pointer
    try {
        nlm::ErrorHandling::checkNotNull(nullptr, "test_ptr", "test.cpp", 123, "testFunc");
        assert(false); // Should not reach here
    } catch (const nlm::ValidationError& e) {
        assert(e.getField() == "test_ptr");
        std::cout << "✓ checkNotNull with null pointer test passed" << std::endl;
    }
    
    // Test with valid pointer
    int value = 42;
    int* validPtr = &value;
    NLM_CHECK_CONDITION(validPtr, "Valid pointer check"); // Should not throw
    std::cout << "✓ checkNotNull with valid pointer test passed" << std::endl;
}

void testMacros() {
    try {
        NLM_THROW_VALIDATION("Test macro error", "test_field", 
                            nlm::ValidationError::ErrorType::InvalidValue);
        assert(false); // Should not reach here
    } catch (const nlm::ValidationError& e) {
        assert(e.getField() == "test_field");
        std::cout << "✓ NLM_THROW_VALIDATION macro test passed" << std::endl;
    }
    
    try {
        NLM_THROW_CONFIG("Test config macro error", "test_key");
        assert(false); // Should not reach here
    } catch (const nlm::ConfigError& e) {
        assert(e.getKey() == "test_key");
        std::cout << "✓ NLM_THROW_CONFIG macro test passed" << std::endl;
    }
    
    try {
        NLM_THROW_FILE("test_operation", "/test/file.txt", "Test file macro error",
                      nlm::FileError::ErrorType::NotFound);
        assert(false); // Should not reach here
    } catch (const nlm::FileError& e) {
        assert(e.getFilePath() == "/test/file.txt");
        std::cout << "✓ NLM_THROW_FILE macro test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Error Handling Tests..." << std::endl;
    
    // Initialize logger for error testing
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Error);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    testNLMError();
    testConfigError();
    testFileError();
    testValidationError();
    testInitializationError();
    testCheckNotNull();
    testMacros();
    
    std::cout << "All error handling tests passed!" << std::endl;
}

} // namespace test_error_handling

// Entry point for testing
int main() {
    test_error_handling::runAll();
    return 0;
}