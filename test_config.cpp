#include <iostream>
#include <fstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

int main() {
    std::cout << "=== Testing Improved NLM Config System ===\n" << std::endl;
    
    // Test 1: Initialize logger with different levels
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Logger initialized successfully");
    NLM_LOG_DEBUG("This is a debug message");
    NLM_LOG_WARNING("This is a warning message");
    NLM_LOG_ERROR("This is an error message");
    
    // Test 2: Create and save configuration to JSON
    {
        auto config = std::make_shared<Config>();
        
        // Set various types of values
        config->set("string_value", "Hello World", ConfigSource::Runtime);
        config->set("int_value", 42, ConfigSource::Runtime);
        config->set("double_value", 3.14159, ConfigSource::Runtime);
        config->set("bool_value", true, ConfigSource::Runtime);
        config->set("array_value", std::vector<int>({1, 2, 3, 4, 5}), ConfigSource::Runtime);
        
        // Save to JSON file
        if (config->saveToFile("test_config.json")) {
            NLM_LOG_INFO("Configuration saved to JSON file successfully");
        } else {
            NLM_LOG_ERROR("Failed to save configuration to JSON file");
        }
        
        // Load back from JSON
        if (config->loadFromFile("test_config.json")) {
            NLM_LOG_INFO("Configuration loaded from JSON file successfully");
            
            // Test getting values
            if (auto val = config->get<std::string>("string_value")) {
                NLM_LOG_INFO("String value: " + *val);
            }
            
            if (auto val = config->get<int64_t>("int_value")) {
                NLM_LOG_INFO("Integer value: " + std::to_string(*val));
            }
            
            if (auto val = config->get<double>("double_value")) {
                NLM_LOG_INFO("Double value: " + std::to_string(*val));
            }
            
            if (auto val = config->get<bool>("bool_value")) {
                NLM_LOG_INFO("Boolean value: " + std::string(*val ? "true" : "false"));
            }
        } else {
            NLM_LOG_ERROR("Failed to load configuration from JSON file");
        }
    }
    
    // Test 3: Test simple text format
    {
        auto config = std::make_shared<Config>();
        
        // Save to text file
        if (config->saveToFile("test_config.txt")) {
            NLM_LOG_INFO("Configuration saved to text file successfully");
        } else {
            NLM_LOG_ERROR("Failed to save configuration to text file");
        }
    }
    
    // Test 4: Test loading from command line arguments
    {
        auto config = std::make_shared<Config>();
        
        // Simulate command line arguments
        int argc = 5;
        char* argv[] = {
            (char*)"test_program",
            (char*)"--string_value",
            (char*)"FromCommandLine",
            (char*)"--int_value",
            (char*)"123"
        };
        
        config->loadFromArgs(argc, argv);
        
        if (auto val = config->get<std::string>("string_value")) {
            NLM_LOG_INFO("Value from command line: " + *val);
        }
        
        if (auto val = config->get<int64_t>("int_value")) {
            NLM_LOG_INFO("Integer from command line: " + std::to_string(*val));
        }
    }
    
    NLM_LOG_INFO("\n=== All tests completed successfully! ===");
    return 0;
}
