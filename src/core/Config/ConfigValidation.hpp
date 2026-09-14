// Configuration validation system
#ifndef NLM_CONFIG_VALIDATION_HPP
#define NLM_CONFIG_VALIDATION_HPP

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>

namespace nlm {

struct ValidationResult {
    bool isValid;
    std::string message;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    ValidationResult(bool valid, const std::string& msg) 
        : isValid(valid), message(msg) {}
};

class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validate a configuration section
    ValidationResult validateSection(const std::string& section) const;
    
    // Validate entire configuration
    ValidationResult validate(const Config& config) const;
    
    // Get list of all valid configuration keys
    static std::vector<std::string> getValidKeys();
    
    // Get validation error messages for a specific key
    static std::string getErrorMessage(const std::string& key);
    
    // Validate configuration from file
    static ValidationResult validateFile(const std::string& filepath);
    
    // Suggest missing configuration values
    static std::vector<std::string> suggestMissingValues(const Config& config);
    
    // Suggest corrections for invalid values
    static std::vector<std::string> suggestCorrections(const Config& config);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#endif // NLM_CONFIG_VALIDATION_HPP
