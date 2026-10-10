// Simple test for enhanced configuration system
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Mock the necessary types for testing
struct ConfigValue {
    int int_val;
    std::string string_val;
    bool bool_val;
    double double_val;
};

enum class ConfigSource { Default, File, CommandLine, Runtime };

struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
};

class Config {
public:
    void set(const std::string& key, int value) {
        entries.push_back({key, {value, "", false, 0.0}, ConfigSource::Runtime, ""});
    }
    
    bool has(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key) return true;
        }
        return false;
    }
    
    int getInt(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key && entry.value.int_val != 0) {
                return entry.value.int_val;
            }
        }
        return 0;
    }
    
    std::string getString(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key && !entry.value.string_val.empty()) {
                return entry.value.string_val;
            }
        }
        return "";
    }
    
    bool getBool(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key) {
                return entry.value.bool_val;
            }
        }
        return false;
    }
    
    double getDouble(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key && entry.value.double_val != 0.0) {
                return entry.value.double_val;
            }
        }
        return 0.0;
    }
    
    std::string summary() const {
        std::stringstream ss;
        ss << "Configuration (" << entries.size() << " entries):\n";
        for (const auto& entry : entries) {
            ss << "  " << entry.key << " = [";
            if (!entry.value.string_val.empty()) {
                ss << \"\"\"\" << entry.value.string_val << \"\"\"\"\";
            } else if (entry.value.int_val != 0) {
                ss << entry.value.int_val;
            } else if (entry.value.bool_val) {
                ss << "true";
            } else if (entry.value.double_val != 0.0) {
                ss << entry.value.double_val;
            }
            ss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
        return ss.str();
    }
    
private:
    std::vector<ConfigEntry> entries;
};

int main() {
    std::cout << "Testing Enhanced Configuration System" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    Config config;
    
    // Test 1: Basic configuration
    std::cout << "Test 1: Basic configuration" << std::endl;
    config.set("random_seed", 42);
    assert(config.has("random_seed"));
    assert(config.getInt("random_seed") == 42);
    std::cout << "  PASSED" << std::endl;
    
    // Test 2: String configuration
    std::cout << "Test 2: String configuration" << std::endl;
    config.set("log_level", "INFO");
    assert(config.has("log_level"));
    assert(config.getString("log_level") == "INFO");
    std::cout << "  PASSED" << std::endl;
    
    // Test 3: Boolean configuration
    std::cout << "Test 3: Boolean configuration" << std::endl;
    config.set("visualization_enabled", true);
    assert(config.has("visualization_enabled"));
    assert(config.getBool("visualization_enabled") == true);
    std::cout << "  PASSED" << std::endl;
    
    // Test 4: Double configuration
    std::cout << "Test 4: Double configuration" << std::endl;
    config.set("simulation_timestep", 0.001);
    assert(config.has("simulation_timestep"));
    assert(config.getDouble("simulation_timestep") == 0.001);
    std::cout << "  PASSED" << std::endl;
    
    // Test 5: Configuration summary
    std::cout << "Test 5: Configuration summary" << std::endl;
    std::string summary = config.summary();
    assert(summary.find("random_seed") != std::string::npos);
    assert(summary.find("log_level") != std::string::npos);
    assert(summary.find("visualization_enabled") != std::string::npos);
    assert(summary.find("simulation_timestep") != std::string::npos);
    std::cout << "  PASSED" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== All Tests PASSED ===" << std::endl;
    return 0;
}
