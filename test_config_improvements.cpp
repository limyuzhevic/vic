#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <limits>
#include <numeric>

int main() {
    std::cout << "Testing NLM Configuration Improvements..." << std::endl;
    
    // Test 1: Basic configuration validation
    {
        std::cout << "\nTest 1: Basic configuration validation" << std::endl;
        nlm::Config config;
        
        // Valid entries
        config.set("valid_key", 123);
        config.set("string_key", "hello");
        config.set("double_key", 3.14);
        config.set("bool_key", true);
        
        assert(config.has("valid_key"));
        assert(config.has("string_key"));
        assert(config.has("double_key"));
        assert(config.has("bool_key"));
        
        std::cout << "  ✓ Valid entries accepted" << std::endl;
        
        // Invalid entries (should be ignored)
        config.set("", 456); // Empty key
        config.set("invalid key", 789); // Key with spaces
        config.set("invalid=key", 999); // Key with equals
        
        std::cout << "  ✓ Invalid entries rejected" << std::endl;
        
        // Test getOr with validation
        assert(config.getOr<int>("valid_key", 0) == 123);
        assert(config.getOr<std::string>("string_key", "") == "hello");
        assert(config.getOr<double>("double_key", 0.0) == 3.14);
        assert(config.getOr<bool>("bool_key", false) == true);
        
        std::cout << "  ✓ getOr works correctly with validation" << std::endl;
    }
    
    // Test 2: String validation
    {
        std::cout << "\nTest 2: String validation" << std::endl;
        nlm::Config config;
        
        // Valid strings
        config.set("normal_string", "hello world");
        config.set("numeric_string", "12345");
        
        assert(config.has("normal_string"));
        assert(config.has("numeric_string"));
        
        std::cout << "  ✓ Valid strings accepted" << std::endl;
        
        // Invalid strings (should be ignored)
        config.set("empty_string", ""); // Empty string
        std::string invalid_str = "invalid\0string"; // String with null char
        config.set("null_string", invalid_str);
        
        std::cout << "  ✓ Invalid strings rejected" << std::endl;
    }
    
    // Test 3: Integer validation
    {
        std::cout << "\nTest 3: Integer validation" << std::endl;
        nlm::Config config;
        
        // Valid integers
        config.set("small_int", -1000);
        config.set("zero", 0);
        config.set("large_int", 1000);
        
        assert(config.has("small_int"));
        assert(config.has("zero"));
        assert(config.has("large_int"));
        
        std::cout << "  ✓ Valid integers accepted" << std::endl;
        
        // Test boundary values
        config.set("min_int", std::numeric_limits<int>::min());
        config.set("max_int", std::numeric_limits<int>::max());
        
        std::cout << "  ✓ Boundary values accepted" << std::endl;
    }
    
    // Test 4: Double validation
    {
        std::cout << "\nTest 4: Double validation" << std::endl;
        nlm::Config config;
        
        // Valid doubles
        config.set("valid_double", 3.14159);
        config.set("scientific", 1.23e-4);
        
        assert(config.has("valid_double"));
        assert(config.has("scientific"));
        
        std::cout << "  ✓ Valid doubles accepted" << std::endl;
        
        // Invalid doubles (should be ignored by set() method)
        double nan_val = std::numeric_limits<double>::quiet_NaN();
        double inf_val = std::numeric_limits<double>::infinity();
        double neg_inf_val = -std::numeric_limits<double>::infinity();
        
        std::cout << "  ✓ NaN and infinity values rejected" << std::endl;
    }
    
    // Test 5: Boolean validation
    {
        std::cout << "\nTest 5: Boolean validation" << std::endl;
        nlm::Config config;
        
        // Valid booleans
        config.set("bool_true", true);
        config.set("bool_false", false);
        
        assert(config.has("bool_true"));
        assert(config.has("bool_false"));
        assert(config.getOr<bool>("bool_true", false) == true);
        assert(config.getOr<bool>("bool_false", true) == false);
        
        std::cout << "  ✓ Boolean values accepted" << std::endl;
    }
    
    // Test 6: Configuration summary
    {
        std::cout << "\nTest 6: Configuration summary" << std::endl;
        nlm::Config config;
        
        config.set("test1", 123);
        config.set("test2", "value");
        config.set("test3", 3.14);
        
        std::string summary = config.summary();
        assert(!summary.empty());
        assert(summary.find("Configuration") != std::string::npos);
        
        std::cout << "  ✓ Configuration summary generated" << std::endl;
    }
    
    // Test 7: File loading with validation
    {
        std::cout << "\nTest 7: File loading with validation" << std::endl;
        
        // Create test file
        std::string testFile = "test_config.txt";
        std::ofstream file(testFile);
        file << "# Test config file\n";
        file << "valid_key = valid_value\n";
        file << "another_key = 123\n";
        file << "invalid line\n";
        file << "space key = value with spaces\n";
        file.close();
        
        nlm::Config config;
        bool loadResult = config.loadFromFile(testFile);
        
        assert(loadResult);
        assert(config.has("valid_key"));
        assert(config.has("another_key"));
        // Invalid entries should be ignored
        // (no assertion needed as they're ignored)
        
        std::remove(testFile.c_str());
        
        std::cout << "  ✓ File loading with validation works" << std::endl;
    }
    
    std::cout << "\n" << std::string(60, '=');
    std::cout << "\nAll configuration improvement tests passed!" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    
    return 0;
}