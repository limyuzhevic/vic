#pragma once

#include "Config.hpp"
#include "ConfigJSON.hpp"
#include <string>
#include <memory>
#include <variant>

namespace nlm {

class ConfigHelper {
public:
    // Configuration source detection
    enum class SourceType {
        Unknown,
        KeyValue,
        JSON
    };
    
    ConfigHelper();
    ~ConfigHelper();
    
    // Disable copying, enable moving
    ConfigHelper(const ConfigHelper&) = delete;
    ConfigHelper& operator=(const ConfigHelper&) = delete;
    ConfigHelper(ConfigHelper&&) noexcept;
    ConfigHelper& operator=(ConfigHelper&&) noexcept;
    
    // Load configuration from file (auto-detect format)
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from string (auto-detect format)
    bool loadFromString(const std::string& content);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration value
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Set configuration value
    template<typename T>
    bool set(const std::string& key, const T& value);
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration source type
    SourceType getSourceType() const { return sourceType_; }
    
    // Get configuration file path
    std::string getFilepath() const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Clear configuration
    void clear();
    
    // Convert configuration to JSON string
    std::string toJSON(bool pretty = false) const;
    
    // Validate configuration schema
    bool validateSchema(const std::string& schemaFile) const;
    
    // Merge with another configuration
    bool merge(const ConfigHelper& other);
    
    // Create default configuration
    static ConfigHelper createDefaultConfig();
    
    // Create example configuration
    static std::string createExampleConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    SourceType sourceType_;
    
    bool loadKeyValue(const std::string& content);
    bool loadJSON(const std::string& content);
    std::string detectFormat(const std::string& content) const;
    
    bool setValue(const std::string& key, const ConfigValue& value);
    ConfigValue getValue(const std::string& key) const;
    
    // Helper functions
    static std::string trim(const std::string& str);
    static bool isJSON(const std::string& content);
    static bool isKeyValue(const std::string& content);
};

} // namespace nlm
