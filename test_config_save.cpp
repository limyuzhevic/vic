// Test program to verify Config.saveToFile() fix
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>

int main() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello world", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_vector", std::vector<std::string>("a", "b", "c"), nlm::ConfigSource::Runtime);
    
    // Test saveToFile
    bool saveSuccess = config.saveToFile("test_config.txt");
    if (saveSuccess) {
        std::cout << "✓ Config.saveToFile() executed successfully" << std::endl;
        
        // Verify the file content
        std::ifstream file("test_config.txt");
        if (file.is_open()) {
            std::cout << "✓ Config file created successfully" << std::endl;
            
            std::string line;
            bool foundInt = false, foundDouble = false, foundString = false, foundBool = false;
            
            while (std::getline(file, line)) {
                if (line.find("int_value") != std::string::npos && line.find("42") != std::string::npos) {
                    foundInt = true;
                    std::cout << "  Found int_value = 42" << std::endl;
                }
                if (line.find("double_value") != std::string::npos && line.find("3.14") != std::string::npos) {
                    foundDouble = true;
                    std::cout << "  Found double_value = 3.14" << std::endl;
                }
                if (line.find("string_value") != std::string::npos && line.find("\"hello world\"") != std::string::npos) {
                    foundString = true;
                    std::cout << "  Found string_value = \"hello world\"" << std::endl;
                }
                if (line.find("bool_value") != std::string::npos && line.find("true") != std::string::npos) {
                    foundBool = true;
                    std::cout << "  Found bool_value = true" << std::endl;
                }
            }
            
            if (foundInt && foundDouble && foundString && foundBool) {
                std::cout << "✓ All configuration values saved correctly" << std::endl;
            } else {
                std::cout << "✗ Some configuration values missing from file" << std::endl;
                std::cout << "  Found int: " << (foundInt ? "✓" : "✗") 
                          << ", Double: " << (foundDouble ? "✓" : "✗") 
                          << ", String: " << (foundString ? "✓" : "✗") 
                          << ", Bool: " << (foundBool ? "✓" : "✗") << std::endl;
            }
            
            file.close();
        } else {
            std::cout << "✗ Failed to open config file for reading" << std::endl;
        }
        
        // Clean up
        remove("test_config.txt");
    } else {
        std::cout << "✗ Config.saveToFile() failed" << std::endl;
    }
    
    return 0;
}
