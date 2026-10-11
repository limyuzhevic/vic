// Plugin configuration implementation for NLM

#include "PluginConfig.hpp"
#include <stdexcept>

namespace nlm {

PluginConfig::PluginConfig() : enabled(true), priority(0) {
}

bool PluginConfig::validate() const {
    if (name.empty()) {
        return false;
    }
    if (version.empty()) {
        return false;
    }
    if (description.empty()) {
        return false;
    }
    if (author.empty()) {
        return false;
    }
    return true;
}

std::vector<std::string> PluginConfig::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (name.empty()) {
        errors.push_back("Plugin name cannot be empty");
    }
    
    if (version.empty()) {
        errors.push_back("Plugin version cannot be empty");
    }
    
    if (description.empty()) {
        errors.push_back("Plugin description cannot be empty");
    }
    
    if (author.empty()) {
        errors.push_back("Plugin author cannot be empty");
    }
    
    return errors;
}

bool PluginConfig::operator==(const PluginConfig& other) const {
    return name == other.name &&
           version == other.version &&
           type == other.type &&
           category == other.category &&
           description == other.description &&
           author == other.author &&
           contact == other.contact &&
           parameters == other.parameters &&
           dependencies == other.dependencies &&
           enabled == other.enabled &&
           priority == other.priority &&
           loadPolicy == other.loadPolicy &&
           unloadPolicy == other.unloadPolicy &&
           metadata == other.metadata;
}

bool PluginConfig::operator!=(const PluginConfig& other) const {
    return !(*this == other);
}

} // namespace nlm
