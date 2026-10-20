#pragma once

#include <string>
#include <memory>
#include <map>
#include "../../src/core/EnhancedConfig.hpp"

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    void createSection(const std::string& name, const std::string& description);
    void addParameter(const std::string& section, const std::string& key, 
                     const std::string& value, const std::string& type);
    std::string getParameter(const std::string& key) const;
    
    void registerPreset(const ConfigurationPreset& preset);
    bool applyPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    ConfigurationPreset getPreset(const std::string& name) const;
    
    bool setValue(const std::string& key, const std::string& value, 
                 ConfigSource source = ConfigSource::Runtime);
    bool setValue(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool setValue(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool setValue(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    ValidationResult validateConfiguration() const;
    ValidationResult validateParameter(const std::string& key) const;
    bool addValidationRule(const ValidationRule& rule);
    void clearValidationRules();
    
    std::map<std::string, std::pair<std::string, std::string>> compareWith(const EnhancedConfig& other) const;
    std::vector<std::string> getDifferences(const EnhancedConfig& other) const;
    std::vector<std::string> getChangesSince(const std::chrono::steady_clock::time_point& since) const;
    
    std::map<std::string, std::string> getSummary() const;
    std::vector<std::string> getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm