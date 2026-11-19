#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include "../Logger/Logger.hpp"
#include "StringUtils.hpp"

namespace nlm {

/**
 * Configuration manager with enhanced features
 */
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line
    bool loadFromCommandLine(int argc, char* argv[]);
    
    // Get configuration value
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    template<typename T>
    void set(const std::string& key, const T& value);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get all keys
    std::vector<std::string> getAllKeys() const;
    
    // Print configuration
    void printConfiguration() const;
    
    // Reset to default values
    void resetToDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Helper function to convert strings to different types
template<typename T>
T convertToType(const std::string& str, const T& defaultValue);

// Template specializations for common types
// Forward declarations for template specializations
template int64_t convertToType<int64_t>(const std::string&, const int64_t&);
template float convertToType<float>(const std::string&, const float&);
template double convertToType<double>(const std::string&, const double&);
template std::string convertToType<std::string>(const std::string&, const std::string&);
template bool convertToType<bool>(const std::string&, const bool&);

} // namespace nlm
