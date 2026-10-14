#pragma once

#include "ConfigSchema.hpp"
#include <memory>

namespace nlm {

// Configuration initializer that sets up all parameters with their validation rules
class ConfigInitializer {
public:
    ConfigInitializer();
    
    // Get the global configuration schema
    static ConfigSchema& getSchema();
    
    // Get the global configuration validator
    static ConfigValidator& getValidator();
    
    // Initialize all configuration parameters with their validation rules
    static void initializeAllParameters();
    
    // Get parameter descriptions for documentation
    static std::string getParameterDescriptions();
    
    // Validate configuration against schema
    static bool validateConfiguration(const Config& config);
    
    // Get all configuration validation errors
    static std::vector<ValidationError> getValidationErrors();
    
    // Reset the configuration system
    static void reset();
    
private:
    // Create validation rules for each parameter type
    static std::function<bool(const std::any&)> createRangeValidator(double min, double max);
    static std::function<bool(const std::any&)> createPositiveValidator();
    static std::function<bool(const std::any&)> createNonNegativeValidator();
    static std::function<bool(const std::any&)> createProbabilityValidator();
    static std::function<bool(const std::any&)> createLogLevelValidator();
    static std::function<bool(const std::any&)> createEnvironmentNameValidator();
    static std::function<bool(const std::any&)> createEnumValidator(const std::vector<std::string>& validValues);
    
    // Helper functions
    static double getNumericValue(const std::any& value);
    static std::string getTypeName(const std::any& value);
};

} // namespace nlm