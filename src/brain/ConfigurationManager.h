// ConfigurationManager.h - Manages configuration loading and parameter management
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <any>
#include <functional>

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Get parameter by key and type
    template<typename T>
    T get(const std::string& key) const;
    
    // Get parameter with default value
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    // Get parameter with type checking
    template<typename T>
    bool get(const std::string& key, T& value) const;
    
    // Set parameter
    template<typename T>
    void set(const std::string& key, T value);
    
    // Check if parameter exists
    bool has(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration source
    const std::string& getSource() const;
    
    // Clear all parameters
    void clear();
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get parameter description/type
    std::string getType(const std::string& key) const;
    
    // Add parameter validation callback
    void addValidator(const std::string& key,
                      std::function<bool(const std::any&)> validator);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
