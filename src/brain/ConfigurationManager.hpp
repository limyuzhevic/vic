#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// ConfigurationManager: Parameter management and initialization
// Manages configuration loading, parameter validation, and defaults
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Configuration loading
    bool loadConfig(const std::string& filepath);
    bool loadFromString(const std::string& configData);
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    Config* getConfig() { return config_.get(); }
    
    // Parameter access
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    template<typename T>
    bool has(const std::string& key) const;
    
    // Configuration validation
    bool validate() const;
    
    // Default parameters
    static std::unordered_map<std::string, std::string> getDefaultParameters();
    
    // Configuration statistics
    size_t getParameterCount() const { return parameterCount_; }
    std::vector<std::string> getParameterKeys() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Configuration instance
    std::shared_ptr<Config> config_;
    
    // Parameter statistics
    size_t parameterCount_;
};

} // namespace nlm
