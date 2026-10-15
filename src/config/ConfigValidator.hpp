#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../core/Config/Config.hpp"
#include "AdvancedConfig.hpp"

namespace nlm {

class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying, enable moving
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    ConfigValidator(ConfigValidator&&) noexcept;
    ConfigValidator& operator=(ConfigValidator&&) noexcept;
    
    // Validation lifecycle
    bool initialize(const std::string& schemaFile = "");
    void update(double timestep);
    void shutdown();
    
    // Schema validation
    ValidationResult validate(const Config& config) const;
    ValidationResult validate(const std::string& configKey, const ConfigValue& value) const;
    
    // Schema management
    bool addSchema(const std::string& schemaName, const std::string& schemaDefinition);
    bool removeSchema(const std::string& schemaName);
    bool updateSchema(const std::string& schemaName, const std::string& schemaDefinition);
    std::vector<std::string> getAvailableSchemas() const;
    
    // Custom validators
    bool addCustomValidator(const std::string& name, 
                          std::function<ValidationResult(const Config&)> validator);
    bool removeCustomValidator(const std::string& name);
    std::vector<std::string> getCustomValidators() const;
    
    // Configuration recommendations
    std::vector<std::string> getRecommendations(const Config& config) const;
    std::map<std::string, std::string> getDependencyGraph() const;
    
    // Validation statistics
    size_t getTotalValidations() const;
    size_t getSuccessfulValidations() const;
    double getValidationSuccessRate() const;
    std::string getValidationStats() const;
    
    // File format support
    bool loadSchemaFromYAML(const std::string& filepath);
    bool loadSchemaFromJSON(const std::string& filepath);
    std::string exportSchemaToYAML() const;
    std::string exportSchemaToJSON() const;
    
    // Configuration constraints
    bool addConstraint(const std::string& constraintName, 
                      std::function<bool(const Config&)> constraint);
    bool removeConstraint(const std::string& constraintName);
    std::vector<std::string> getConstraints() const;
    
    // Schema validation rules
    struct ValidationRule {
        std::string ruleName;
        std::string ruleType; // "range", "enum", "pattern", "required", "custom"
        std::string ruleDefinition;
        std::string errorMessage;
        bool isCritical;
        
        ValidationRule() : isCritical(false) {}
    };
    
    bool addValidationRule(const std::string& schemaName, const ValidationRule& rule);
    bool removeValidationRule(const std::string& schemaName, const std::string& ruleName);
    std::vector<ValidationRule> getValidationRules(const std::string& schemaName) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
