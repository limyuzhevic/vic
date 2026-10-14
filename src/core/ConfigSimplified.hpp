// Simplified configuration class with easier API
// Replaces the complex ConfigValue variant approach

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>

namespace nlm {

// Simplified configuration value - can be any basic type
// No complex variant types - just plain storage
class ConfigSimplified {
public:
    ConfigSimplified();
    ~ConfigSimplified();
    
    // Disable copying, enable moving
    ConfigSimplified(const ConfigSimplified&) = delete;
    ConfigSimplified& operator=(const ConfigSimplified&) = delete;
    ConfigSimplified(ConfigSimplified&&) noexcept;
    ConfigSimplified& operator=(ConfigSimplified&&) noexcept;
    
    // Set values using simple types (no variant needed)
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setInt64(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get values with simple API
    bool getBool(const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    int64_t getInt64(const std::string& key, int64_t defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Configuration templates and presets
    static ConfigSimplified createBrainConfig();
    static ConfigSimplified createAdvancedConfig();
    static ConfigSimplified createDevelopmentConfig();
    
    // Configuration validation
    bool validate() const;
    
    // Merge two configurations
    void merge(const ConfigSimplified& other);
    
    // Configuration source enum
    enum class ConfigSource {
        Default,
        File,
        CommandLine,
        Runtime
    };

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience factory functions
inline ConfigSimplified createDefaultConfig() {
    return ConfigSimplified::createBrainConfig();
}

inline ConfigSimplified createBrainConfig() {
    return ConfigSimplified::createBrainConfig();
}

inline ConfigSimplified createAdvancedConfig() {
    return ConfigSimplified::createAdvancedConfig();
}

inline ConfigSimplified createDevelopmentConfig() {
    return ConfigSimplified::createDevelopmentConfig();
}

} // namespace nlm