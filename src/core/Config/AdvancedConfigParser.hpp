#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

/**
 * Configuration validator for NLM
 * 
 * Validates configuration files for syntax, consistency, and completeness.
 * Provides detailed error messages and suggestions for fixes.
 */
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
        
        ValidationResult() : isValid(true) {}
        
        bool hasErrors() const { return !errors.empty(); }
        bool hasWarnings() const { return !warnings.empty(); }
        bool hasSuggestions() const { return !suggestions.empty(); }
        
        std::string getSummary() const {
            std::stringstream ss;
            ss << "Validation: " << (isValid ? "PASSED" : "FAILED") << std::endl;
            if (hasErrors()) {
                ss << "Errors: " << errors.size() << std::endl;
                for (const auto& error : errors) {
                    ss << "  - " << error << std::endl;
                }
            }
            if (hasWarnings()) {
                ss << "Warnings: " << warnings.size() << std::endl;
                for (const auto& warning : warnings) {
                    ss << "  - " << warning << std::endl;
                }
            }
            if (hasSuggestions()) {
                ss << "Suggestions: " << suggestions.size() << std::endl;
                for (const auto& suggestion : suggestions) {
                    ss << "  - " << suggestion << std::endl;
                }
            }
            return ss.str();
        }
    };
    
    ConfigValidator();
    ~ConfigValidator();
    
    // Validate configuration
    ValidationResult validate(const std::string& filepath = "");
    ValidationResult validate(const Config& config);
    
    // Validate specific sections
    ValidationResult validateRequiredKeys(const std::unordered_set<std::string>& requiredKeys);
    ValidationResult validateValueRanges(const std::unordered_map<std::string, std::pair<double, double>>& ranges);
    ValidationResult validateRelationships(const std::unordered_map<std::string, std::string>& dependencies);
    
    // Get list of all supported keys
    std::vector<std::string> getSupportedKeys() const;
    
    // Get validation errors for a specific key
    std::vector<std::string> getErrorsForKey(const std::string& key) const;
    
    // Get validation warnings for a specific key
    std::vector<std::string> getWarningsForKey(const std::string& key) const;
    
    // Add custom validation rules
    void addRequiredKey(const std::string& key, const std::string& description = "");
    void addValidationRule(const std::string& key, 
                         std::function<bool(const ConfigValue&)> validator,
                         const std::string& errorMessage = "");
    
    // Set validation severity
    void setStrictMode(bool strict) { strictMode = strict; }
    bool isStrictMode() const { return strictMode; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool strictMode;
    
    // Internal validation helpers
    ValidationResult validateFileFormat(const std::string& filepath);
    ValidationResult validateFileContent(const std::string& filepath);
    ValidationResult validateConfigStructure(const Config& config);
    
    // Key validation
    bool isValidKey(const std::string& key) const;
    bool hasValidValueType(const std::string& key, const ConfigValue& value) const;
    
    // Range validation
    ValidationResult validateRange(const std::string& key, const ConfigValue& value,
                                  double min, double max) const;
    
    // Dependency validation
    ValidationResult validateDependencies(const Config& config) const;
    
    // Initialize default validation rules
    void initializeDefaultRules();
};

/**
 * Advanced configuration file parser with JSON support
 */
class AdvancedConfigParser {
public:
    struct ParsedConfig {
        std::string filename;
        std::vector<std::string> comments;
        std::vector<std::pair<std::string, std::string>> keyValuePairs;
        std::string format; // "key-value", "json", "ini"
        bool isValid;
        std::vector<std::string> errors;
    };
    
    ParsedConfig parseFile(const std::string& filepath);
    std::string formatForDisplay(const ParsedConfig& parsed) const;
    std::string formatForOutput(const ParsedConfig& parsed, const std::string& outputFormat = "key-value") const;
    
private:
    bool parseKeyValueFormat(const std::string& content, ParsedConfig& parsed);
    bool parseJsonFormat(const std::string& content, ParsedConfig& parsed);
    bool parseIniFormat(const std::string& content, ParsedConfig& parsed);
};

} // namespace nlm

#endif // CONFIG_VALIDATOR_HPP
