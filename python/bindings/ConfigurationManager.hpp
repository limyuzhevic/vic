#pragma once

#include <string>
#include <memory>
#include "../../src/core/EnhancedConfig.hpp"

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    void registerPreset(const ConfigurationPreset& preset);
    bool applyPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    ConfigurationPreset getPreset(const std::string& name) const;
    
    bool setValue(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool validateConfiguration() const;
    std::map<std::string, std::pair<std::string, std::string>> compareWith(const EnhancedConfig& other) const;
    std::map<std::string, std::string> getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm