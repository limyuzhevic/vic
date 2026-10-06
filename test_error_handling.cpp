// Simple test to verify error handling enhancements
#include "core/Config/Config.hpp"
#include "core/Error.h"
#include <iostream>
#include <fstream>
#include <filesystem>

int main() {
    std::cout << "Testing Error Handling Enhancements" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Test 1: ErrorManager is accessible
    std::cout << "\n1. Testing ErrorManager..." << std::endl;
    auto& errorManager = nlm::getErrorManager();
    errorManager.enable(true);
    errorManager.setStrictMode(false);
    
    // Test 2: ConfigError types can be created
    std::cout << "\n2. Testing ConfigError creation..." << std::endl;
    try {
        auto configError = errorManager.configFileError(
            "test.cpp", 123, "test_function",
            "Test error message",
            "/nonexistent/path/config.json",
            "test_key"
        );
        std::cout << "   ConfigFileError created successfully" << std::endl;
        std::cout << "   Error message: " << configError.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "   Exception thrown (unexpected in test): " << e.what() << std::endl;
    }
    
    // Test 3: Test Config with non-existent file (should report error)
    std::cout << "\n3. Testing Config with non-existent file..." << std::endl;
    nlm::Config config;
    bool result = config.loadFromFile("/nonexistent/path/config.json");
    std::cout << "   Config::loadFromFile() returned: " << (result ? "true" : "false") << std::endl;
    std::cout << "   Error count: " << errorManager.getErrorCount() << std::endl;
    
    // Test 4: Test Config with existing file
    std::cout << "\n4. Testing Config with existing file..." << std::endl;
    
    // Create a test config file
    std::string testConfigPath = "/tmp/test_config.txt";
    std::ofstream testFile(testConfigPath);
    testFile << "# Test configuration file\n";
    testFile << "test_key = \"test_value\"\n";
    testFile << "number_key = 42\n";
    testFile.close();
    
    result = config.loadFromFile(testConfigPath);
    std::cout << "   Config::loadFromFile() returned: " << (result ? "true" : "false") << std::endl;
    std::cout << "   Error count: " << errorManager.getErrorCount() << std::endl;
    
    if (result) {
        std::cout << "   Keys: ";
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
        
        auto value = config.get<std::string>("test_key");
        if (value) {
            std::cout << "   test_key value: " << *value << std::endl;
        }
    }
    
    // Test 5: Test save with directory that doesn't exist
    std::cout << "\n5. Testing saveToFile with non-existent directory..." << std::endl;
    bool saveResult = config.saveToFile("/nonexistent_dir/config.json");
    std::cout << "   Config::saveToFile() returned: " << (saveResult ? "true" : "false") << std::endl;
    std::cout << "   Error count: " << errorManager.getErrorCount() << std::endl;
    
    // Test 6: Test command-line argument parsing with errors
    std::cout << "\n6. Testing loadFromArgs with error cases..." << std::endl;
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--valid_key=value1",
        (char*)"-invalid",
        (char*)"--missing_equals",
        (char*)"-another_key",
        nullptr  // Add one more for safety
    };
    
    // Note: loadFromArgs expects argc to be correct
    bool argsResult = config.loadFromArgs(5, argv);
    std::cout << "   Config::loadFromArgs() returned: " << (argsResult ? "true" : "false") << std::endl;
    std::cout << "   Error count: " << errorManager.getErrorCount() << std::endl;
    
    // Clean up
    std::filesystem::remove(testConfigPath);
    
    std::cout << "\n" << std::endl;
    std::cout << "=== Error Handling Test Summary ===" << std::endl;
    std::cout << "Total errors encountered: " << errorManager.getErrorCount() << std::endl;
    std::cout << "Error reporting is: " << (errorManager.isEnabled() ? "enabled" : "disabled") << std::endl;
    std::cout << "Strict mode is: " << (errorManager.isStrictMode() ? "enabled" : "disabled") << std::endl;
    std::cout << "Should exit: " << (errorManager.shouldExit() ? "yes" : "no") << std::endl;
    
    return 0;
}
