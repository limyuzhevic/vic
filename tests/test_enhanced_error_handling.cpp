#include "core/Config/Config.hpp"
#include "core/Config/ConfigInitializer.hpp"
#include "core/Config/ConfigValidator.hpp"
#include "core/Exceptions/Exceptions.hpp"
#include "core/ExceptionHandler/ExceptionHandler.hpp"
#include <iostream>
#include <any>
#include <unordered_map>

int main() {
    std::cout << "=== NLM Enhanced Error Handling System ===\n" << std::endl;
    
    // Initialize configuration system
    std::cout << "1. Initializing configuration system..." << std::endl;
    nlm::ConfigInitializer::initializeAllParameters();
    
    // Initialize exception handler
    std::cout << "2. Initializing exception handler..." << std::endl;
    auto& exception_handler = nlm::ExceptionHandler::getInstance();
    exception_handler.setEnabled(true);
    
    // Set up custom exception callback
    exception_handler.setExceptionCallback([](const std::shared_ptr<nlm::NLMException>& exc) {
        std::cout << "    [Custom Handler] Exception occurred: " << exc->toString() << std::endl;
    });
    
    // Test exception creation and handling
    std::cout << "\n3. Testing exception creation..." << std::endl;
    
    // Test configuration exception
    try {
        auto config_exc = nlm::ExceptionFactory::createConfigurationException(
            "Invalid configuration value",
            nlm::ErrorSeverity::Error,
            "stdp_ltp_weight"
        );
        nlm::ExceptionHandler::getInstance().handleException(config_exc);
    } catch (...) {
        std::cout << "    Configuration exception handled successfully" << std::endl;
    }
    
    // Test I/O exception
    try {
        auto io_exc = nlm::ExceptionFactory::createIOError(
            "Cannot open configuration file: config.cfg",
            nlm::ErrorSeverity::Critical,
            "configs/config.cfg"
        );
        nlm::ExceptionHandler::getInstance().handleException(io_exc);
    } catch (...) {
        std::cout << "    I/O exception handled successfully" << std::endl;
    }
    
    // Test memory exception
    try {
        auto mem_exc = nlm::ExceptionFactory::createMemoryException(
            "Memory allocation failed",
            nlm::ErrorSeverity::Critical,
            1024 * 1024 * 1024  // 1GB
        );
        nlm::ExceptionHandler::getInstance().handleException(mem_exc);
    } catch (...) {
        std::cout << "    Memory exception handled successfully" << std::endl;
    }
    
    // Test runtime exception
    try {
        auto runtime_exc = nlm::ExceptionFactory::createRuntimeException(
            "Neutron simulation failed to initialize",
            nlm::ErrorSeverity::Error,
            "initializeNeutronSimulation"
        );
        nlm::ExceptionHandler::getInstance().handleException(runtime_exc);
    } catch (...) {
        std::cout << "    Runtime exception handled successfully" << std::endl;
    }
    
    // Test validation exception
    try {
        auto valid_exc = nlm::ExceptionFactory::createValidationException(
            "Value must be between 0 and 1",
            nlm::ErrorSeverity::Warning,
            "connection_probability"
        );
        nlm::ExceptionHandler::getInstance().handleException(valid_exc);
    } catch (...) {
        std::cout << "    Validation exception handled successfully" << std::endl;
    }
    
    // Test safe operation with exception handling
    std::cout << "\n4. Testing safe operations..." << std::endl;
    
    // Operation that will fail
    auto safe_result = nlm::ExceptionUtils::safeOperation(
        [](const std::string&) { throw nlm::ExceptionFactory::createRuntimeException("Simulated error"); },
        std::string("test")
    );
    std::cout << "    Safe operation result: " << (safe_result ? "SUCCESS" : "FAILED") << std::endl;
    
    // Operation that will succeed
    auto safe_result2 = nlm::ExceptionUtils::safeOperation(
        [](const std::string&) { return; },  // Do nothing, succeed
        std::string("test2")
    );
    std::cout << "    Safe operation result: " << (safe_result2 ? "SUCCESS" : "FAILED") << std::endl;
    
    // Show exception statistics
    std::cout << "\n5. Exception Statistics:" << std::endl;
    auto stats = exception_handler.getExceptionStatistics();
    for (const auto& pair : stats) {
        std::cout << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    // Show recent exceptions
    std::cout << "\n6. Recent Exceptions:" << std::endl;
    auto recent = exception_handler.getRecentExceptions(5);
    for (size_t i = 0; i < recent.size(); ++i) {
        std::cout << "    " << (i + 1) << ". " << recent[i]->toString() << std::endl;
    }
    
    std::cout << "\n=== Enhanced Error Handling System Complete ===\n" << std::endl;
    
    return 0;
}