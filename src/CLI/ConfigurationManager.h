// ConfigurationManager.h
#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>

struct DefaultConfigValue {
    std::string key;
    std::string value;
    std::string description;
    nlm::ConfigSource source;
};

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager() = default;
    
    // Validate value
    bool validateValue(const std::string& key, const std::string& value) const;
    
    // Merge with defaults
    void applyDefaults(std::shared_ptr<nlm::Config> config) const;
    
    // Get all default keys
    std::vector<std::string> getDefaultKeys() const;
    
    // Show configuration summary
    void showConfigSummary(std::shared_ptr<nlm::Config> config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
