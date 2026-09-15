#pragma once

#include <string>
#include <vector>
#include <functional>
#include <variant>
#include <unordered_map>

namespace nlm {

// Configuration validation rules
enum class ValidationRule {
    Required,        // Value must be present
    Positive,        // Value must be positive
    NonNegative,     // Value must be non-negative
    Range,           // Value must be within a range
    Choice,          // Value must be one of specified choices
    Pattern,         // Value must match pattern
    Callback         // Value must pass custom validation function
};

// Validation error
struct ValidationError {
    std::string key;
    std::string message;
    ValidationRule rule;
    std::string expected;
    std::string actual;
};

// Configuration validation system
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    
    // Add validation rule
    void addRule(const std::string& key, ValidationRule rule, 
                 const std::string& message = "",
                 const std::variant<std::string, double, int, std::vector<std::string>>& value = {});
    
    // Add custom validator
    void addValidator(const std::string& key, 
                     std::function<bool(const std::variant<int, int64_t, double, bool, std::string,
                                      std::vector<int>, std::vector<double>, std::vector<std::string>>> validator,
                     const std::string& message = "");
    
    // Validate configuration
    std::vector<ValidationError> validate(const class Config& config) const;
    
    // Check if configuration is valid
    bool isValid(const class Config& config) const;
    
    // Get all errors for a key
    std::vector<ValidationError> getErrorsForKey(const class Config& config, const std::string& key) const;
    
    // Clear all rules
    void clear();
    
    // Load validation rules from config file
    bool loadFromFile(const std::string& filepath);
    
    // Save validation rules to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get validation summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm