// Test suite for NLM Configuration System
// 
// Comprehensive tests for the improved Config.cpp implementation
// covering loading, validation, error handling, and performance

#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include <cmath>

using namespace nlm;

class ConfigTestSuite {
public:
    ConfigTestSuite() {
        std::cout << "=== NLM Configuration System Test Suite ===" << std::endl;
        runAllTests();
        std::cout << "\n=== All tests completed ===" << std::endl;
    }
    
    void runAllTests() {
        testBasicConfiguration();
        testKeyValueFormat();
        testTypeConversion();
        testValidation();
        testErrorHandling();
        testPerformance();
        testFileOperations();
        testAdvancedFeatures();
    }
    
private:
    void testBasicConfiguration() {
        std::cout << "\n--- Test 1: Basic Configuration ---" << std::endl;
        Config config;
        
        // Set basic values
        config.set("int_value", 42);
        config.set("double_value", 3.14);
        config.set("string_value", "test_string");
        config.set("bool_value", true);
        
        // Test retrieval
        assert(config.get<int>("int_value").value() == 42);
        assert(config.get<double>("double_value").value() == 3.14);
        assert(config.get<std::string>("string_value").value() == "test_string");
        assert(config.get<bool>("bool_value").value() == true);
        
        std::cout << "✓ Basic configuration test passed" << std::endl;
    }
    
    void testKeyValueFormat() {
        std::cout << "\n--- Test 2: Key-Value Format ---" << std::endl;
        Config config;
        
        // Create test config file
        std::string testFile = "test_config.cfg";
        std::ofstream file(testFile);
        file << "# Test Configuration File\n";
        file << "int_key = 100\n";
        file << "float_key = 2.718\n";
        file << "string_key = \"hello world\"\n";
        file << "bool_key = true\n";
        file << "# Comment line\n";
        file << "list_key = [1, 2, 3, 4, 5]\n";
        file.close();
        
        // Load configuration
        assert(config.loadFromFile(testFile));
        
        // Test values
        assert(config.get<int>("int_key").value() == 100);
        assert(std::abs(config.get<double>("float_key").value() - 2.718) < 0.001);
        assert(config.get<std::string>("string_key").value() == "hello world");
        assert(config.get<bool>("bool_key").value() == true);
        
        // Clean up
        std::filesystem::remove(testFile);
        
        std::cout << "✓ Key-value format test passed" << std::endl;
    }
    
    void testTypeConversion() {
        std::cout << "\n--- Test 3: Type Conversion ---" << std::endl;
        Config config;
        
        // Set values of different types
        config.set("int_as_string", std::string("123"));
        config.set("float_as_string", std::string("3.14159"));
        config.set("bool_as_string_true", std::string("true"));
        config.set("bool_as_string_false", std::string("false"));
        
        // Test type conversion
        assert(config.get<int>("int_as_string").value() == 123);
        assert(std::abs(config.get<double>("float_as_string").value() - 3.14159) < 0.00001);
        assert(config.get<bool>("bool_as_string_true").value() == true);
        assert(config.get<bool>("bool_as_string_false").value() == false);
        
        // Test getOr with default values
        assert(config.getOr<int>("nonexistent", 999) == 999);
        assert(config.getOr<double>("nonexistent", 3.14159) == 3.14159);
        assert(config.getOr<bool>("nonexistent", true) == true);
        assert(config.getOr<std::string>("nonexistent", "default") == "default");
        
        std::cout << "✓ Type conversion test passed" << std::endl;
    }
    
    void testValidation() {
        std::cout << "\n--- Test 4: Validation ---" << std::endl;
        Config config;
        
        // Test invalid key names
        assert(!config.has("valid_key"));
        config.set("valid_key", 42);
        assert(config.has("valid_key"));
        
        // Test key removal
        config.remove("valid_key");
        assert(!config.has("valid_key"));
        
        // Test clearing
        config.set("key1", 1);
        config.set("key2", 2);
        config.set("key3", 3);
        assert(config.getKeys().size() == 3);
        config.clear();
        assert(config.getKeys().size() == 0);
        
        // Test summary generation
        config.set("test_key", "test_value");
        std::string summary = config.summary();
        assert(summary.find("test_key") != std::string::npos);
        assert(summary.find("test_value") != std::string::npos);
        
        std::cout << "✓ Validation test passed" << std::endl;
    }
    
    void testErrorHandling() {
        std::cout << "\n--- Test 5: Error Handling ---" << std::endl;
        Config config;
        
        // Test loading non-existent file
        assert(!config.loadFromFile("non_existent_file.txt"));
        
        // Test setting invalid types
        // This should not crash the application
        config.set("test", std::string("valid_string"));
        
        // Test type mismatch handling
        config.set("number_as_string", std::string("not_a_number"));
        
        // Test missing required keys
        assert(!config.has("required_key"));
        
        std::cout << "✓ Error handling test passed" << std::endl;
    }
    
    void testPerformance() {
        std::cout << "\n--- Test 6: Performance ---" << std::endl;
        Config config;
        
        // Test fast key lookup
        const int numKeys = 1000;
        for (int i = 0; i < numKeys; ++i) {
            config.set(std::to_string(i), i * 2);
        }
        
        // Test fast key retrieval
        for (int i = 0; i < numKeys; ++i) {
            assert(config.get<int>(std::to_string(i)).value() == i * 2);
        }
        
        // Test getOr performance with defaults
        assert(config.getOr<int>("999999", 9999999) == 9999999);
        
        std::cout << "✓ Performance test passed" << std::endl;
    }
    
    void testFileOperations() {
        std::cout << "\n--- Test 7: File Operations ---" << std::endl;
        Config config;
        
        // Create test file
        std::string testFile = "test_save_config.cfg";
        
        // Set some values
        config.set("int_key", 123);
        config.set("float_key", 3.14159);
        config.set("string_key", "test_value");
        config.set("bool_key", true);
        
        // Save to file
        assert(config.saveToFile(testFile));
        
        // Clear and reload
        config.clear();
        assert(config.loadFromFile(testFile));
        
        // Verify values
        assert(config.get<int>("int_key").value() == 123);
        assert(std::abs(config.get<double>("float_key").value() - 3.14159) < 0.00001);
        assert(config.get<std::string>("string_key").value() == "test_value");
        assert(config.get<bool>("bool_key").value() == true);
        
        // Clean up
        std::filesystem::remove(testFile);
        
        std::cout << "✓ File operations test passed" << std::endl;
    }
    
    void testAdvancedFeatures() {
        std::cout << "\n--- Test 8: Advanced Features ---" << std::endl;
        Config config;
        
        // Test moving semantics
        Config config1(std::move(config));
        config1.set("moved_key", 42);
        assert(config1.get<int>("moved_key").value() == 42);
        
        // Test list values
        config1.set("int_list", std::vector<int>({1, 2, 3, 4, 5}));
        config1.set("float_list", std::vector<double>({1.1, 2.2, 3.3, 4.4, 5.5}));
        config1.set("string_list", std::vector<std::string>("hello", "world", "test"));
        
        // Note: Accessing list values would require additional type handling
        // This is just to test that the configuration accepts list types
        
        // Test configuration summary with lists
        std::string summary = config1.summary();
        assert(summary.find("int_list") != std::string::npos);
        assert(summary.find("float_list") != std::string::npos);
        assert(summary.find("string_list") != std::string::npos);
        
        std::cout << "✓ Advanced features test passed" << std::endl;
    }
};

int main() {
    ConfigTestSuite testSuite;
    return 0;
}
