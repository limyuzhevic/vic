//error_handling_example.cpp
// Example demonstrating the improved error handling and logging in NLM

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

void demonstrate_error_handling() {
    std::cout << "=== NLM Error Handling Demonstration ===" << std::endl << std::endl;
    
    // Initialize logger with debug level
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.setLevel(nlm::LogLevel::Debug);
    
    std::cout << "1. Configuration Loading with Error Handling" << std::endl;
    std::cout << "   Attempting to load non-existent configuration file..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    bool success = config->loadFromFile("/non/existent/path/config.cfg");
    
    if (!success) {
        std::cout << "   ✓ Configuration loading properly failed with error logging" << std::endl;
    }
    
    std::cout << std::endl << "2. Configuration Setting with Validation" << std::endl;
    std::cout << "   Setting configuration values with debug logging..." << std::endl;
    
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1);
    config->set("random_seed", static_cast<int64_t>(42));
    
    std::cout << "   Configuration summary:" << std::endl;
    std::cout << "   " << config->summary() << std::endl;
    
    std::cout << std::endl << "3. Command Line Argument Loading" << std::endl;
    std::cout << "   Simulating command line argument loading..." << std::endl;
    
    // Create test command line arguments
    int argc = 5;
    const char* argv[] = {
        "nlm",
        "--neuron_count=500",
        "--region_count=3",
        "--connection_probability=0.2",
        "--random_seed=123"
    };
    
    success = config->loadFromArgs(argc, const_cast<char**>(argv));
    
    if (success) {
        std::cout << "   ✓ Command line argument loading succeeded" << std::endl;
    }
    
    std::cout << "   Updated configuration summary:" << std::endl;
    std::cout << "   " << config->summary() << std::endl;
    
    std::cout << std::endl << "4. Configuration Saving with Error Handling" << std::endl;
    std::cout << "   Attempting to save configuration to file..." << std::endl;
    
    std::string testFile = "/tmp/test_nlm_config.cfg";
    success = config->saveToFile(testFile);
    
    if (success) {
        std::cout << "   ✓ Configuration saved successfully" << std::endl;
        
        // Try to load it back
        auto config2 = std::make_shared<nlm::Config>();
        success = config2->loadFromFile(testFile);
        
        if (success) {
            std::cout << "   ✓ Configuration reloaded successfully" << std::endl;
        }
    }
    
    std::cout << std::endl << "5. Error Code Demonstration" << std::endl;
    std::cout << "   Demonstrating different error code types..." << std::endl;
    
    // Test different error codes
    nlm::NLMError::logError(nlm::NLMError::ErrorCode::FILE_NOT_FOUND,
                          "Configuration file not found",
                          "Please check the file path and permissions");
    
    nlm::NLMError::logError(nlm::NLMError::ErrorCode::NULL_POINTER,
                          "Brain initialization failed",
                          "Brain pointer is null - initialization may have failed");
    
    nlm::NLMError::logError(nlm::NLMError::ErrorCode::INVALID_PARAMETER,
                          "Invalid configuration parameter",
                          "Parameter value is out of acceptable range");
    
    std::cout << "   ✓ Error codes logged appropriately" << std::endl;
    
    std::cout << std::endl << "=== Error Handling Demonstration Complete ===" << std::endl;
}

int main() {
    demonstrate_error_handling();
    return 0;
}
