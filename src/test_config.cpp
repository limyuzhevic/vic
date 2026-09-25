# Simple test program for config.cpp
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

int main() {
    std::cout << "=== NLM Config Test Program ===" << std::endl;
    
    // Create test config directory
    std::filesystem::create_directories("configs");
    
    // Test 1: Load from JSON file
    {
        std::cout << "\n1. Testing JSON file load..." << std::endl;
        nlm::Config config;
        
        // First, create a test JSON file
        std::ofstream jsonFile("configs/test_config.json");
        jsonFile << "{\"app_name\": \"NLM Test\", \"version\": \"1.0.0\", \"value\": 123}";
        jsonFile.close();
        
        // Load it
        bool success = config.loadFromFile("configs/test_config.json");
        std::cout << "   Load result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            std::cout << "   app_name: " << config.getOr<std::string>("app_name", "N/A") << std::endl;
            std::cout << "   version: " << config.getOr<std::string>("version", "N/A") << std::endl;
            std::cout << "   value: " << config.getOr<int>("value", 0) << std::endl;
        }
        
        // Test validation
        bool valid = config.validate();
        std::cout << "   Validation: " << (valid ? "PASSED" : "FAILED") << std::endl;
        
        // Clean up
        std::filesystem::remove("configs/test_config.json");
    }
    
    // Test 2: Save to JSON file
    {
        std::cout << "\n2. Testing JSON file save..." << std::endl;
        nlm::Config config;
        
        config.set("test_string", std::string("Hello Config"));
        config.set("test_int", 42);
        config.set("test_double", 3.14159);
        config.set("test_bool", true);
        config.set("test_array", std::vector<int>({1, 2, 3, 4, 5}));
        
        bool success = config.saveToFile("configs/output_config.json");
        std::cout << "   Save result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            // Read it back
            nlm::Config config2;
            config2.loadFromFile("configs/output_config.json");
            std::cout << "   test_string: " << config2.getOr<std::string>("test_string", "N/A") << std::endl;
            std::cout << "   test_int: " << config2.getOr<int>("test_int", 0) << std::endl;
            std::cout << "   test_double: " << config2.getOr<double>("test_double", 0.0) << std::endl;
            std::cout << "   test_bool: " << config2.getOr<bool>("test_bool", false) << std::endl;
        }
        
        // Clean up
        std::filesystem::remove("configs/output_config.json");
    }
    
    // Test 3: Backward compatibility (simple key=value parser)
    {
        std::cout << "\n3. Testing backward compatibility (simple parser)..." << std::endl;
        nlm::Config config;
        
        // Create simple config file
        std::ofstream simpleFile("configs/simple_config.txt");
        simpleFile << "# Simple config file\n";
        simpleFile << "key1 = value1\n";
        simpleFile << "key2 = 123\n";
        simpleFile << "key3 = true\n";
        simpleFile << "key4 = \"quoted value\"\n";
        simpleFile.close();
        
        bool success = config.loadFromFileSimple("configs/simple_config.txt");
        std::cout << "   Simple load result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            std::cout << "   key1: " << config.getOr<std::string>("key1", "N/A") << std::endl;
            std::cout << "   key2: " << config.getOr<int>("key2", 0) << std::endl;
            std::cout << "   key3: " << config.getOr<bool>("key3", false) << std::endl;
            std::cout << "   key4: " << config.getOr<std::string>("key4", "N/A") << std::endl;
        }
        
        // Clean up
        std::filesystem::remove("configs/simple_config.txt");
    }
    
    // Test 4: Error handling
    {
        std::cout << "\n4. Testing error handling..." << std::endl;
        nlm::Config config;
        
        // Test loading non-existent file
        try {
            config.loadFromFile("configs/does_not_exist.json");
            std::cout << "   ERROR: Should have thrown exception for non-existent file" << std::endl;
        } catch (const nlm::ConfigError& e) {
            std::cout << "   Correctly caught exception: " << e.what() << std::endl;
        }
        
        // Test saving to non-existent directory (should be created automatically)
        std::string nestedDir = "configs/nested/deep/path";
        bool success = config.saveToFile(nestedDir + "/test.json");
        std::cout << "   Auto-create directory: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        // Clean up
        std::filesystem::remove_all("configs/nested");
    }
    
    // Test 5: JSON string load
    {
        std::cout << "\n5. Testing JSON string load..." << std::endl;
        nlm::Config config;
        
        std::string jsonString = R"({"from_string": true, "value": 456, "name": "Test String"})";
        bool success = config.loadFromJsonString(jsonString);
        std::cout << "   JSON string load: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            std::cout << "   from_string: " << config.getOr<bool>("from_string", false) << std::endl;
            std::cout << "   value: " << config.getOr<int>("value", 0) << std::endl;
            std::cout << "   name: " << config.getOr<std::string>("name", "N/A") << std::endl;
        }
    }
    
    std::cout << "\n=== All tests completed ===" << std::endl;
    return 0;
}
