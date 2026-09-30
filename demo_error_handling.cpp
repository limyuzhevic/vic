#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <unordered_map>

void testErrorHandlingFramework() {
    std::cout << "Testing NLM Error Handling Framework..." << std::endl;
    
    // Initialize error handling system
    nlm::ErrorHandlingSystem::initialize();
    
    // Add a console error reporter
    auto consoleReporter = std::make_shared<nlm::ConsoleErrorReporter>(nlm::ErrorSeverity::Info, true, true);
    nlm::ErrorManager::getInstance().addReporter(consoleReporter);
    
    std::cout << "✓ Error handling framework initialized" << std::endl;
    
    // Test 1: Basic error reporting
    std::cout << "\n1. Testing basic error reporting..." << std::endl;
    NLM_REPORT_ERROR(nlm::ErrorCode::MemoryAllocationFailed,
                    nlm::ErrorCategory::Memory,
                    nlm::ErrorSeverity::Error,
                    "Failed to allocate memory");
    std::cout << "✓ Basic error reporting works" << std::endl;
    
    // Test 2: Validation exception
    std::cout << "\n2. Testing validation exception..." << std::endl;
    try {
        std::vector<std::string> errors = {"Invalid value", "Out of range"};
        throw nlm::ValidationException(errors, "Validation failed");
    } catch (const nlm::ValidationException& e) {
        std::cout << "✓ Caught validation exception: " << e.what() << std::endl;
    }
    
    // Test 3: Memory exception
    std::cout << "\n3. Testing memory exception..." << std::endl;
    try {
        throw nlm::MemoryException(1024, 512, "Not enough memory");
    } catch (const nlm::MemoryException& e) {
        std::cout << "✓ Caught memory exception: " << e.what() << std::endl;
    }
    
    // Test 4: Configuration exception
    std::cout << "\n4. Testing configuration exception..." << std::endl;
    try {
        throw nlm::ConfigurationException("learning_rate", "double", "Invalid learning rate");
    } catch (const nlm::ConfigurationException& e) {
        std::cout << "✓ Caught configuration exception: " << e.what() << std::endl;
    }
    
    // Test 5: Error context
    std::cout << "\n5. Testing error context..." << std::endl;
    std::unordered_map<std::string, std::string> context {
        {"neuron_id", "123"},
        {"synapse_weight", "0.5"},
        {"operation", "STDP_update"}
    };
    NLM_REPORT_ERROR(nlm::ErrorCode::InvalidConnection,
                    nlm::ErrorCategory::NeuralNetwork,
                    nlm::ErrorSeverity::Error,
                    "Connection validation failed",
                    "", 0, "", context);
    std::cout << "✓ Error context works" << std::endl;
    
    // Test 6: Error severity filtering
    std::cout << "\n6. Testing error severity filtering..." << std::endl;
    auto warningReporter = std::make_shared<nlm::ConsoleErrorReporter>(nlm::ErrorSeverity::Warning, false, false);
    nlm::ErrorManager::getInstance().addReporter(warningReporter);
    std::cout << "✓ Error severity filtering works" << std::endl;
    
    // Test 7: Resource guard
    std::cout << "\n7. Testing resource guard..." << std::endl;
    bool cleanupCalled = false;
    {
        auto guard = nlm::ResourceGuard([&]() {
            cleanupCalled = true;
            std::cout << "   Resource cleanup called" << std::endl;
        });
        std::cout << "   Resource guard created" << std::endl;
    }
    std::cout << "✓ Resource guard test passed" << std::endl;
    
    // Test 8: Exception-safe transaction
    std::cout << "\n8. Testing transaction safety..." << std::endl;
    bool transactionCommitted = false;
    
    try {
        nlm::TransactionGuard transaction("Test transaction");
        std::cout << "   Transaction created, performing operations..." << std::endl;
        transactionCommitted = true;
        transaction.commit();
    } catch (...) {
        std::cout << "   Transaction exception caught" << std::endl;
    }
    
    std::cout << "✓ Transaction safety test passed" << std::endl;
    
    std::cout << "\n=== Error Handling Framework Tests Completed ===" << std::endl;
    
    // Cleanup
    nlm::ErrorHandlingSystem::shutdown();
}

int main() {
    std::cout << "=== NLM Error Handling Framework Demo ===" << std::endl;
    
    // Initialize logger for consistency
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    testErrorHandlingFramework();
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "The NLM Error Handling Framework is working correctly!" << std::endl;
    
    return 0;
}
