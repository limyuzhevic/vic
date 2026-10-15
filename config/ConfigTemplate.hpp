#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <variant>
#include <optional>
#include <stdexcept>
#include "core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class Config;

// Configuration template
class ConfigTemplate {
public:
    ConfigTemplate() = default;
    ConfigTemplate(const std::string& name, const std::string& description);
    
    // Template management
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void addParameter(const std::string& name, const ConfigValue& defaultValue,
                     const std::string& description, bool required = false);
    void addValidationRule(const std::string& parameter, 
                          std::function<bool(const ConfigValue&, const Config&)> validator);
    void addDependency(const std::string& parameter, const std::string& dependsOn);
    
    // Template operations
    std::shared_ptr<Config> createConfig() const;
    bool validate(const Config& config) const;
    ValidationResult validateWithDetails(const Config& config) const;
    std::shared_ptr<Config> applyTemplate(const Config& baseConfig) const;
    
    // Template properties
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    std::vector<std::string> getParameters() const;
    std::map<std::string, ConfigValue> getDefaultValues() const;
    
    // Template serialization
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Built-in template factories
    static std::shared_ptr<ConfigTemplate> createPhase2Template();
    static std::shared_ptr<ConfigTemplate> createPhase4Template();
    static std::shared_ptr<ConfigTemplate> createPhase6Template();
    static std::shared_ptr<ConfigTemplate> createCustomTemplate(const std::string& name);
    
private:
    std::string name;
    std::string description;
    std::map<std::string, std::pair<ConfigValue, std::string>> parameters; // name -> (default, description)
    std::map<std::string, std::vector<std::function<bool(const ConfigValue&, const Config&)>>> validationRules;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::set<std::string>> enumValues;
    std::map<std::string, std::pair<double, double>> ranges;
};

} // namespace nlm
