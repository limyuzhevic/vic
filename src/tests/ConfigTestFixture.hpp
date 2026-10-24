#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <filesystem>

// Test fixture for configuration system tests
class ConfigTestFixture {
public:
    ConfigTestFixture() : config() {
        // Initialize with default configuration
        std::string defaultConfigFile = "configs/default.cfg";
        if (std::filesystem::exists(defaultConfigFile)) {
            config.loadFromFile(defaultConfigFile);
        }
    }
    
    ~ConfigTestFixture() = default;
    
    // Disable copying
    ConfigTestFixture(const ConfigTestFixture&) = delete;
    ConfigTestFixture& operator=(const ConfigTestFixture&) = delete;
    
    Config config;  // Configuration object under test
    
    // Helper methods for tests
    bool hasKey(const std::string& key) const {
        return config.has(key);
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        return config.getOr<std::string>(key, defaultValue);
    }
    
    int getInt(const std::string& key, int defaultValue = 0) const {
        return config.getOr<int>(key, defaultValue);
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        return config.getOr<double>(key, defaultValue);
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        return config.getOr<bool>(key, defaultValue);
    }
    
    // Test file creation helper
    bool createTestFile(const std::string& content, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        file.close();
        return true;
    }
    
    // Test file cleanup helper
    bool removeTestFile(const std::string& filename) {
        return std::filesystem::remove(filename);
    }
};