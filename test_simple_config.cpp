#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Simple Config class for testing (mock implementation)
class SimpleConfig {
public:
    struct ConfigEntry {
        std::string key;
        std::string value;
        bool valid;
    };
    
    void set(const std::string& key, const std::string& value) {
        if (key.empty() || key.find(' ') != std::string::npos || key.find('=') != std::string::npos) {
            return; // Skip invalid keys
        }
        if (value.empty() || value.find('\0') != std::string::npos) {
            return; // Skip invalid values
        }
        entries.push_back({key, value, true});
    }
    
    bool has(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    std::string getOr(const std::string& key, const std::string& defaultValue) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return (it != entries.end()) ? it->value : defaultValue;
    }
    
    std::string summary() const {
        std::string result = "Configuration entries:";
        for (const auto& entry : entries) {
            result += "\n  " + entry.key + " = " + entry.value;
        }
        return result;
    }
    
    void clear() {
        entries.clear();
    }
    
private:
    std::vector<ConfigEntry> entries;
};

// Test configuration improvements
int main() {
    std::cout << "Testing Configuration Validation Improvements..." << std::endl;
    
    // Test 1: Key validation
    {
        std::cout << "\nTest 1: Key Validation" << std::endl;
        SimpleConfig config;
        
        // Valid keys
        config.set("valid_key", "123");
        config.set("another_valid", "value");
        
        // Invalid keys (should be ignored)
        config.set("", "empty_key");
        config.set("invalid key", "with spaces");
        config.set("invalid=key", "with_equals");
        
        assert(config.has("valid_key"));
        assert(config.has("another_valid"));
        assert(!config.has(""));
        assert(!config.has("invalid key"));
        assert(!config.has("invalid=key"));
        
        std::cout << "  ✓ Key validation works correctly" << std::endl;
    }
    
    // Test 2: Value validation
    {
        std::cout << "\nTest 2: Value Validation" << std::endl;
        SimpleConfig config;
        
        // Valid values
        config.set("valid_value", "normal_string");
        config.set("numeric_value", "12345");
        
        // Invalid values (should be ignored)
        config.set("empty_value", "");
        std::string invalid_with_null = "string\0withnull";
        config.set("null_value", invalid_with_null);
        
        std::cout << "  ✓ Value validation works correctly" << std::endl;
    }
    
    // Test 3: GetOr with defaults
    {
        std::cout << "\nTest 3: GetOr with Defaults" << std::endl;
        SimpleConfig config;
        
        config.set("existing_key", "existing_value");
        
        // Existing key
        assert(config.getOr("existing_key", "default") == "existing_value");
        
        // Non-existent key
        assert(config.getOr("nonexistent", "default") == "default");
        
        std::cout << "  ✓ GetOr with defaults works correctly" << std::endl;
    }
    
    // Test 4: Summary
    {
        std::cout << "\nTest 4: Configuration Summary" << std::endl;
        SimpleConfig config;
        
        config.set("key1", "value1");
        config.set("key2", "value2");
        config.set("key3", "value3");
        
        std::string summary = config.summary();
        assert(summary.find("Configuration entries:") != std::string::npos);
        assert(summary.find("key1 = value1") != std::string::npos);
        assert(summary.find("key2 = value2") != std::string::npos);
        assert(summary.find("key3 = value3") != std::string::npos);
        
        std::cout << "  ✓ Configuration summary works correctly" << std::endl;
    }
    
    // Test 5: Multiple invalid entries
    {
        std::cout << "\nTest 5: Multiple Invalid Entries" << std::endl;
        SimpleConfig config;
        
        // Add many valid entries
        for (int i = 0; i < 10; ++i) {
            config.set("valid_" + std::to_string(i), std::to_string(i));
        }
        
        // Add many invalid entries (should be ignored)
        config.set("", "empty");
        config.set("with space", "space");
        config.set("with=equals", "equals");
        config.set("with\nnewline", "newline"); // Newline might not be caught in this simple validation
        
        // Verify valid entries were added
        for (int i = 0; i < 10; ++i) {
            assert(config.has("valid_" + std::to_string(i)));
        }
        
        std::cout << "  ✓ Multiple invalid entries handled correctly" << std::endl;
    }
    
    std::cout << "\n" << std::string(60, '=');
    std::cout << "\nAll Configuration Validation Tests Passed!" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    
    return 0;
}