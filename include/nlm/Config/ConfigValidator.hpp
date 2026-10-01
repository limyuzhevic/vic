// NLM (熙然) - Neural Learning Machine
// Configuration Validation System
//
// This system provides validation for configuration values,
// error handling, and helps users understand configuration requirements.

#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <optional>
#include "core/Config/Config.hpp"

namespace nlm {

// Validation result
enum class ValidationResult {
    Valid,
    Invalid,
    Warning
};

// Validation error details
struct ValidationError {
    std::string key;
    std::string message;
    ValidationResult result;
    std::string suggestion;
    
    ValidationError() : key(), message(), result(ValidationResult::Valid), suggestion() {}
    ValidationError(const std::string& k, const std::string& msg, 
                    ValidationResult res, const std::string& sug = "")
        : key(k), message(msg), result(res), suggestion(sug) {}
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator() = default;

    // Add validation rule
    void addValidation(const std::string& key, 
                      const std::function<bool(const ConfigValue&)> validator,
                      const std::string& errorMessage,
                      ValidationResult result = ValidationResult::Invalid,
                      const std::string& suggestion = "");

    // Validate configuration
    std::vector<ValidationError> validate(const Config& config) const;

    // Get error summary
    std::string getErrorSummary(const std::vector<ValidationError>& errors) const;

    // Get warning summary
    std::string getWarningSummary(const std::vector<ValidationError>& errors) const;

    // Standard validators
    static bool validatePositiveInt(const ConfigValue& value);
    static bool validatePositiveDouble(const ConfigValue& value);
    static bool validateNonNegativeInt(const ConfigValue& value);
    static bool validateProbability(const ConfigValue& value);
    static bool validateStringNotEmpty(const ConfigValue& value);
    static bool validateRange(const ConfigValue& value, double min, double max);
    static bool validateEnum(const ConfigValue& value, 
                           const std::vector<std::string>& validValues);

    // Configuration metadata
    static bool hasMetadata(const std::string& key);
    static std::string getDescription(const std::string& key);
    static std::string getType(const std::string& key);
    static std::string getUnits(const std::string& key);
    static std::string getExample(const std::string& key);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration error reporter
class ConfigErrorReporter {
public:
    static std::string formatError(const ValidationError& error);
    static std::string formatError(const ValidationError& error, bool includeSuggestion);
    static void printErrors(const std::vector<ValidationError>& errors, 
                           std::ostream& out = std::cerr);
    static void printWarnings(const std::vector<ValidationError>& errors,
                             std::ostream& out = std::cerr);
    static bool hasErrors(const std::vector<ValidationError>& errors);
    static bool hasWarnings(const std::vector<ValidationError>& errors);
    static size_t countErrors(const std::vector<ValidationError>& errors);
    static size_t countWarnings(const std::vector<ValidationError>& errors);
};

// Configuration helper functions
namespace ConfigValidation {
    // Validate common configuration patterns
    std::vector<ValidationError> validateBasicConfiguration(const Config& config);
    std::vector<ValidationError> validateBrainConfiguration(const Config& config);
    std::vector<ValidationError> validatePlasticityConfiguration(const Config& config);
    std::vector<ValidationError> validateMemoryConfiguration(const Config& config);
    std::vector<ValidationError> validateNeuromodulationConfiguration(const Config& config);

    // Get configuration recommendations
    std::string getConfigurationRecommendations(const Config& config);
    std::string getPerformanceRecommendations(const Config& config);
    std::string getMemoryRecommendations(const Config& config);

    // Configuration validation presets
    void setupDefaultValidations(ConfigValidator& validator);
    void setupBrainValidations(ConfigValidator& validator);
    void setupLearningValidations(ConfigValidator& validator);
}

} // namespace nlm
