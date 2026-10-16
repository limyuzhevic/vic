#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

class ConfigPresets {
public:
    struct Preset {
        std::string name;
        std::string description;
        std::map<std::string, std::string> values;
        std::string environment; // "development", "testing", "production"
        bool isDefault = false;
        bool isExperimental = false;
    };
    
    ConfigPresets();
    ~ConfigPresets();
    
    void addPreset(const Preset& preset);
    bool hasPreset(const std::string& name) const;
    const Preset* getPreset(const std::string& name) const;
    
    std::vector<std::string> getPresetNames() const;
    std::vector<std::string> getPresetNames(const std::string& environment) const;
    
    bool applyPreset(const std::string& name, std::shared_ptr<Config> config) const;
    bool validatePreset(const std::string& name, const std::shared_ptr<Config> config) const;
    
    void removePreset(const std::string& name);
    
    // Get default presets
    static std::vector<Preset> getDefaultPresets();
    
    // Export presets to JSON/YAML
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    
    // Import presets from JSON/YAML
    bool importFromJSON(const std::string& json);
    bool importFromYAML(const std::string& yaml);
    
    // Preset validation rules
    struct ValidationRule {
        enum class Type {
            None,
            Required,
            Range,
            Positive,
            Negative,
            GreaterThan,
            LessThan,
            Enum,
            Pattern,
            FileExists
        };
        
        Type type;
        std::string value;
        std::string description;
        
        ValidationRule() : type(Type::None) {}
        ValidationRule(Type t, const std::string& v, const std::string& d = "")
            : type(t), value(v), description(d) {}
    };
    
    void addValidationRule(const std::string& key, const ValidationRule& rule);
    std::vector<ValidationRule> getValidationRules(const std::string& key) const;
    
    bool validateConfig(const std::shared_ptr<Config> config, const std::string& environment = "development") const;
    
    // Environment-specific presets
    static std::vector<Preset> getDevelopmentPresets();
    static std::vector<Preset> getTestingPresets();
    static std::vector<Preset> getProductionPresets();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm