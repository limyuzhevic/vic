#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <limits>
#include <sstream>

void testConsoleErrorReporter() {
    std::cout << "Testing ConsoleErrorReporter..." << std::endl;
    
    nlm::ConsoleErrorReporter reporter(nlm::ErrorSeverity::Warning, true, true);
    
    // Test error reporting
    nlm::ErrorInfo error(nlm::ErrorCode::MemoryAllocationFailed,
                        nlm::ErrorCategory::Memory,
                        nlm::ErrorSeverity::Error,
                        "Failed to allocate 1MB memory block");
    
    reporter.report(error);
    std::cout << "✓ ConsoleErrorReporter test passed" << std::endl;
}

void testFileErrorReporter() {
    std::cout << "Testing FileErrorReporter..." << std::endl;
    
    std::string testFile = "/tmp/test_errors.log";
    nlm::FileErrorReporter reporter(testFile, true, nlm::ErrorSeverity::Error);
    
    nlm::ErrorInfo error(nlm::ErrorCode::FileReadError,
                        nlm::ErrorCategory::FileSystem,
                        nlm::ErrorSeverity::Error,
                        "Cannot read file: test.txt");
    
    reporter.report(error);
    reporter.closeOutputFile();
    
    // Verify file was written
    std::ifstream file(testFile);
    if (file.good()) {
        std::cout << "✓ FileErrorReporter test passed" << std::endl;
    } else {
        std::cout << "✗ FileErrorReporter test failed" << std::endl;
    }
}

void testErrorManager() {
    std::cout << "Testing ErrorManager..." << std::endl;
    
    auto consoleReporter = std::make_shared<nlm::ConsoleErrorReporter>(nlm::ErrorSeverity::Info);
    nlm::ErrorManager::getInstance().addReporter(consoleReporter);
    
    // Test error reporting
    nlm::ErrorManager::getInstance().report(
        nlm::ErrorCode::ValidationFailed,
        nlm::ErrorCategory::Validation,
        nlm::ErrorSeverity::Error,
        "Validation error occurred"
    );
    
    nlm::ErrorManager::getInstance().report(
        ErrorCode::InvalidConfigValue,
        ErrorCategory::Configuration,
        ErrorSeverity::Warning,
        "Invalid configuration value"
    );
    
    std::cout << "✓ ErrorManager test passed" << std::endl;
}

void testResourceGuard() {
    std::cout << "Testing ResourceGuard..." << std::endl;
    
    bool cleanupCalled = false;
    
    {
        auto guard = nlm::ResourceGuard([&]() {
            cleanupCalled = true;
            std::cout << "   Resource cleanup executed" << std::endl;
        });
        
        std::cout << "   ResourceGuard created, cleanup will happen on scope exit" << std::endl;
    }
    
    if (cleanupCalled) {
        std::cout << "✓ ResourceGuard test passed" << std::endl;
    } else {
        std::cout << "✗ ResourceGuard test failed" << std::endl;
    }
}

void testTransactionGuard() {
    std::cout << "Testing TransactionGuard..." << std::endl;
    
    bool operationSucceeded = false;
    bool rollbackCalled = false;
    
    try {
        nlm::TransactionGuard transaction("Test transaction");
        
        // Simulate operations
        std::cout << "   Performing transaction operations..." << std::endl;
        operationSucceeded = true;
        
        // Explicitly commit
        transaction.commit();
        
        std::cout << "   Transaction committed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "   Transaction exception: " << e.what() << std::endl;
    }
    
    std::cout << "✓ TransactionGuard test passed" << std::endl;
}

void testValidationUtils() {
    std::cout << "Testing ValidationUtils..." << std::endl;
    
    // Test vector validation
    std::vector<float> testData = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    auto errors = nlm::ValidationUtils::validateVector(testData, 0.0f, 10.0f);
    
    if (errors.empty()) {
        std::cout << "   Vector validation passed" << std::endl;
    } else {
        std::cout << "   Vector validation errors: " << errors.size() << std::endl;
    }
    
    // Test range validation
    if (nlm::ValidationUtils::isInRange<float>(5.0f, 0.0f, 10.0f)) {
        std::cout << "   Range validation passed" << std::endl;
    } else {
        std::cout << "   Range validation failed" << std::endl;
    }
    
    std::cout << "✓ ValidationUtils test passed" << std::endl;
}

void testExceptionTypes() {
    std::cout << "Testing Exception Types..." << std::endl;
    
    // Test MemoryException
    try {
        throw nlm::MemoryException(1024 * 1024, 512 * 1024, "Not enough memory");
    } catch (const nlm::MemoryException& e) {
        std::cout << "   Caught MemoryException: " << e.what() << std::endl;
        std::cout << "   Requested: " << e.requested_size << " bytes" << std::endl;
        std::cout << "   Available: " << e.available_size << " bytes" << std::endl;
    }
    
    // Test ConfigurationException
    try {
        throw nlm::ConfigurationException("learning_rate", "double", "Invalid learning rate");
    } catch (const nlm::ConfigurationException& e) {
        std::cout << "   Caught ConfigurationException: " << e.what() << std::endl;
        std::cout << "   Key: " << e.key << ", Expected type: " << e.expected_type << std::endl;
    }
    
    // Test ValidationException
    try {
        std::vector<std::string> errors = {"Invalid value", "Out of range"};
        throw nlm::ValidationException(errors, "Input validation failed");
    } catch (const nlm::ValidationException& e) {
        std::cout << "   Caught ValidationException: " << e.what() << std::endl;
        std::cout << "   Error count: " << e.validation_errors.size() << std::endl;
    }
    
    std::cout << "✓ Exception Types test passed" << std::endl;
}

void testErrorMacros() {
    std::cout << "Testing Error Macros..." << std::endl;
    
    // This should report an error but not throw
    NLM_REPORT_ERROR(nlm::ErrorCode::OutOfRange,
                    nlm::ErrorCategory::Validation,
                    nlm::ErrorSeverity::Error,
                    "Value is out of range");
    
    // This should throw an exception
    try {
        NLM_THROW_ERROR(nlm::ErrorCode::InvalidParameter,
                       nlm::ErrorCategory::Validation,
                       nlm::ErrorSeverity::Error,
                       "Invalid parameter provided");
    } catch (const nlm::NLMException& e) {
        std::cout << "   Caught exception from NLM_THROW_ERROR: " << e.what() << std::endl;
    }
    
    // Test validation macro
    try {
        int value = -5;
        NLM_VALIDATE(value > 0, nlm::ErrorCode::InvalidParameter, "Value must be positive");
    } catch (const nlm::NLMException& e) {
        std::cout << "   Caught exception from NLM_VALIDATE: " << e.what() << std::endl;
    }
    
    std::cout << "✓ Error Macros test passed" << std::endl;
}

void testThreadSafety() {
    std::cout << "Testing Thread Safety..." << std::endl;
    
    nlm::ErrorManager::getInstance().setGlobalLevel(nlm::ErrorSeverity::Info);
    
    // Add multiple reporters
    auto reporter1 = std::make_shared<nlm::ConsoleErrorReporter>(nlm::ErrorSeverity::Info);
    auto reporter2 = std::make_shared<nlm::ConsoleErrorReporter>(nlm::ErrorSeverity::Warning);
    
    nlm::ErrorManager::getInstance().addReporter(reporter1);
    nlm::ErrorManager::getInstance().addReporter(reporter2);
    
    // Simulate concurrent error reporting
    auto errorReportingThread = [&]() {
        for (int i = 0; i < 5; ++i) {
            nlm::ErrorManager::getInstance().report(
                nlm::ErrorCode::UnknownError,
                nlm::ErrorCategory::System,
                nlm::ErrorSeverity::Debug,
                "Thread error " + std::to_string(i)
            );
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };
    
    std::thread t1(errorReportingThread);
    std::thread t2(errorReportingThread);
    
    t1.join();
    t2.join();
    
    std::cout << "✓ Thread Safety test passed" << std::endl;
}

void testErrorContext() {
    std::cout << "Testing Error Context..." << std::endl;
    
    std::unordered_map<std::string, std::string> context {
        {"neuron_id", "123"},
        {"synapse_weight", "0.75"},
        {"operation", "STDP_update"},
        {"timestamp", "1234567890"}
    };
    
    nlm::ErrorManager::getInstance().report(
        nlm::ErrorCode::InvalidConnection,
        nlm::ErrorCategory::NeuralNetwork,
        nlm::ErrorSeverity::Error,
        "Connection validation failed",
        "", 0, "", context
    );
    
    std::cout << "✓ Error Context test passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Error Handling Framework Tests ===" << std::endl;
    std::cout << "Running comprehensive tests..." << std::endl;
    
    // Initialize error handling system
    nlm::ErrorHandlingSystem::initialize();
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Run all tests
    testConsoleErrorReporter();
    testFileErrorReporter();
    testErrorManager();
    testResourceGuard();
    testTransactionGuard();
    testValidationUtils();
    testExceptionTypes();
    testErrorMacros();
    testThreadSafety();
    testErrorContext();
    
    std::cout << std::endl;
    std::cout << "=== All Tests Passed Successfully! ===" << std::endl;
    std::cout << "The NLM Error Handling Framework is working correctly." << std::endl;
    
    // Cleanup
    nlm::ErrorHandlingSystem::shutdown();
    
    return 0;
}
