#include "ConfigurationManager.hpp"
#include <iostream>

namespace nlm {

struct ConfigurationManager::Impl {
    std::vector<std::string> presetNames;
    std::map<std::string, std::map<std::string, std::string>> presetValues;
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {}
ConfigurationManager::~ConfigurationManager() {}

void ConfigurationManager::registerPreset(const ConfigurationPreset& preset) {
    pImpl->presetNames.push_back(preset.name);
    pImpl->presetValues[preset.name] = preset.defaultValues;
}

bool ConfigurationManager::applyPreset(const std::string& presetName) {
    if (std::find(pImpl->presetNames.begin(), pImpl->presetNames.end(), presetName) != pImpl->presetNames.end()) {
        return true;
    }
    return false;
}

std::vector<std::string> ConfigurationManager::getAvailablePresets() const {
    return pImpl->presetNames;
}

ConfigurationPreset ConfigurationManager::getPreset(const std::string& name) const {
    ConfigurationPreset preset(name, "Default preset");
    if (std::find(pImpl->presetNames.begin(), pImpl->presetNames.end(), name) != pImpl->presetNames.end()) {
        preset.defaultValues = pImpl->presetValues[name];
    }
    return preset;
}

bool ConfigurationManager::setValue(const std::string& key, const std::string& value, ConfigSource source) {
    return true;
}

ValidationResult ConfigurationManager::validateConfiguration() const {
    ValidationResult result;
    return result;
}

std::map<std::string, std::pair<std::string, std::string>> ConfigurationManager::compareWith(const EnhancedConfig& other) const {
    return std::map<std::string, std::pair<std::string, std::string>>();
}

std::map<std::string, std::string> ConfigurationManager::getSummary() const {
    std::map<std::string, std::string> summary;
    summary["total_presets"] = std::to_string(pImpl->presetNames.size());
    return summary;
}

} // namespace nlm