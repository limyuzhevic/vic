#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_set>

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
    
    // Main validation method
    bool validate(const Config& config, std::string& errorMessage) const;
    
    // Check for required keys
    bool validateRequiredKeys(const Config& config, 
                             const std::vector<std::string>& requiredKeys,
                             std::string& errorMessage) const;
    
    // Validate numeric ranges
    bool validateRange(const Config& config, const std::string& key,
                      double min, double max, const std::string& description = "",
                      std::string& errorMessage) const;
    
    // Validate neuron count
    bool validateNeuronCount(const Config& config, std::string& errorMessage) const;
    
    // Validate region count
    bool validateRegionCount(const Config& config, std::string& errorMessage) const;
    
    // Validate connection probability
    bool validateConnectionProbability(const Config& config, std::string& errorMessage) const;
    
    // Validate STDP parameters
    bool validateSTDPParameters(const Config& config, std::string& errorMessage) const;
    
    // Validate structural plasticity parameters
    bool validateStructuralPlasticityParameters(const Config& config, std::string& errorMessage) const;
    
    // Validate development parameters
    bool validateDevelopmentParameters(const Config& config, std::string& errorMessage) const;
    
    // Validate neuromodulation parameters
    bool validateNeuromodulationParameters(const Config& config, std::string& errorMessage) const;
    
    // Get list of all validation rules
    std::vector<std::string> getValidationRules() const;
    
    // Check if key is valid
    bool isValidKey(const std::string& key) const;
    
    // Get valid value ranges for key
    std::string getValidRanges(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

}} // namespace nlm