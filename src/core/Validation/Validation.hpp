#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class Config;

class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ConfigValidator() = default;
    
    // Validate configuration values against schema
    ValidationResult validate(const Config& config) const;
    
    // Validate JSON/YAML configuration file format
    ValidationResult validateConfigFile(const std::string& filepath) const;
    
    // Load configuration with validation
    bool loadConfigWithValidation(const std::string& filepath, std::shared_ptr<Config>& config) const;
    
    // Get validation error messages
    static std::string getValidationErrorMessages(const ValidationResult& result);
    
private:
    void validateRandomSeed(const Config& config, ValidationResult& result) const;
    void validateNeuronCount(const Config& config, ValidationResult& result) const;
    void validateRegionCount(const Config& config, ValidationResult& result) const;
    void validateConnectionProbability(const Config& config, ValidationResult& result) const;
    void validateTimestep(const Config& config, ValidationResult& result) const;
    void validatePlasticityParameters(const Config& config, ValidationResult& result) const;
    void validateStructuralPlasticity(const Config& config, ValidationResult& result) const;
    
    bool isValidKey(const std::string& key) const;
    bool isValidValue(const std::string& value) const;
};

} // namespace nlm
