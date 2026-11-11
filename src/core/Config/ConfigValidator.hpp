#pragma once

#include <memory>

namespace nlm {

class ConfigValidator {
public:
    using Validator = std::function<bool(const ConfigValue&, std::string& errorMessage)>;
    
    ConfigValidator();
    ~ConfigValidator();
    
    // Register a validator for a configuration key
    void registerValidator(const std::string& key, 
                          Validator validator,
                          const std::string& description = "");
    
    // Validate a configuration value for a key
    bool validate(const std::string& key, const ConfigValue& value, 
                  std::string& errorMessage) const;
    
    // Validate an entire configuration
    bool validate(const Config& config, std::string& errorMessage) const;
    
    // Validate a configuration file
    bool validateFile(const std::string& filepath, std::string& errorMessage) const;
    
    // Get all registered validators
    std::vector<std::pair<std::string, std::string>> getRegisteredValidators() const;
    
    // Remove validator for a key
    void removeValidator(const std::string& key);
    
    // Clear all validators
    void clearValidators();
    
    // Get default validators for NLM configuration
    void setupDefaultValidators();
    
    // Helper functions for creating validators
    static Validator makeRangeValidator(double min, double max, bool inclusive = true);
    static Validator makePositiveValidator();
    static Validator makeStringLengthValidator(size_t minLength, size_t maxLength);
    static Validator makeEnumValidator(const std::vector<std::string>& validValues);
    static Validator makeConnectivityValidator();
    static Validator makeTimestepValidator();
    static Validator makeNeuronCountValidator();
    static Validator makeRegionCountValidator();
    static Validator makePlasticityValidator();
    static Validator makeNeuromodulationValidator();
    static Validator makeRandomSeedValidator();
    static Validator makeSimulationStepsValidator();
    static Validator makeCheckpointDirValidator();
    static Validator makeLogLevelValidator();
    static Validator makeVisualizationValidator();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm