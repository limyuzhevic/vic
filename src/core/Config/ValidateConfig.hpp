#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "Config.hpp"

namespace nlm {

// Validation result containing any issues found
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::string result;
        if (!errors.empty()) {
            result += "Errors: ";
            for (size_t i = 0; i < errors.size(); ++i) {
                if (i > 0) result += ", ";
                result += errors[i];
            }
        }
        if (!warnings.empty()) {
            if (!result.empty()) result += "; ";
            result += "Warnings: ";
            for (size_t i = 0; i < warnings.size(); ++i) {
                if (i > 0) result += ", ";
                result += warnings[i];
            }
        }
        return result.empty() ? "Valid configuration" : result;
    }
};

// Configuration validation class
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    
    // Validate configuration file
    ValidationResult validateFile(const std::string& filepath) const;
    
    // Validate configuration object
    ValidationResult validateConfig(const Config& config) const;
    
    // Set required configuration keys
    void setRequiredKeys(const std::vector<std::string>& keys);
    
    // Add validation rule for specific key
    void addValidationRule(const std::string& key, const std::string& rule);
    
    // Validate configuration file path (exists, readable, etc.)
    ValidationResult validateFilePath(const std::string& filepath) const;
    
    // Check for duplicate keys in configuration file
    ValidationResult checkDuplicates(const std::string& filepath) const;
    
    // Validate configuration values are within expected ranges
    ValidationResult validateValueRanges(const std::string& filepath) const;
    
    // Get list of required keys
    std::vector<std::string> getRequiredKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation methods
    ValidationResult validateFileContent(const std::string& filepath) const;
    ValidationResult validateConfigObject(const Config& config) const;
    void parseAndValidateFile(const std::string& filepath, Config& config) const;
};

} // namespace nlm
