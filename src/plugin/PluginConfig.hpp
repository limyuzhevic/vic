#pragma once

#include <map>
#include <string>
#include <vector>
#include "PluginBase.hpp"

namespace nlm {

class PluginConfig {
public:
    PluginConfig();
    
    // Basic configuration
    std::string name;
    std::string version;
    PluginType type;
    std::string category;
    std::string description;
    std::string author;
    std::string contact;
    
    // Parameters
    std::map<std::string, std::string> parameters;
    
    // Dependencies
    std::vector<std::string> dependencies;
    
    // State
    bool enabled = true;
    int priority = 0;
    
    // Policies
    PluginLoadPolicy loadPolicy = PluginLoadPolicy::AUTO;
    PluginUnloadPolicy unloadPolicy = PluginUnloadPolicy::NORMAL;
    
    // Metadata
    std::map<std::string, std::string> metadata;
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Comparison
    bool operator==(const PluginConfig& other) const;
    bool operator!=(const PluginConfig& other) const;
};

} // namespace nlm
