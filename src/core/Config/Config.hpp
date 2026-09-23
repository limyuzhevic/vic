#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <set>
#include <functional>
#include <ctime>
#include <regex>
#include <stdexcept>
#include <algorithm>
#include <sstream>

namespace nlm {

// Forward declarations
class Config;
class JSONParser;
class JSONValidator;

// Enhanced configuration schema
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "double", "bool", "string", "array"
    std::string defaultValue;
    std::string description;
    bool required;
    std::string validationPattern;
    double minValue;
    double maxValue;
    std::vector<std::string> allowedValues;
    std::string validationError;
    
    ConfigSchema() : required(false), minValue(0), maxValue(0) {}
    
    bool validate(const ConfigValue& value) const {
        validationError.clear();
        
        // Type validation
        if (!validateType(value)) {
            return false;
        }
        
        // Range validation
        if (!validateRange(value)) {
            return false;
        }
        
        // Pattern validation
        if (!validationPattern.empty() && !validatePattern(value)) {
            return false;
        }
        
        // Allowed values validation
        if (!allowedValues.empty() && !validateAllowedValues(value)) {
            return false;
        }
        
        return true;
    }
    
private:
    bool validateType(const ConfigValue& value) const {
        if (type == "int" && std::holds_alternative<int>(value)) return true;
        if (type == "double" && std::holds_alternative<double>(value)) return true;
        if (type == "bool" && std::holds_alternative<bool>(value)) return true;
        if (type == "string" && std::holds_alternative<std::string>(value)) return true;
        if (type == "array_int" && std::holds_alternative<std::vector<int>>(value)) return true;
        if (type == "array_double" && std::holds_alternative<std::vector<double>>(value)) return true;
        if (type == "array_string" && std::holds_alternative<std::vector<std::string>>(value)) return true;
        
        validationError = "Type mismatch: expected " + type;
        return false;
    }
    
    bool validateRange(const ConfigValue& value) const {
        if (type == "int" && std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val < minValue || val > maxValue) {
                validationError = "Value " + std::to_string(val) + " outside range [" + 
                                 std::to_string(minValue) + ", " + std::to_string(maxValue) + "]";
                return false;
            }
        }
        else if (type == "double" && std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < minValue || val > maxValue) {
                validationError = "Value " + std::to_string(val) + " outside range [" + 
                                 std::to_string(minValue) + ", " + std::to_string(maxValue) + "]";
                return false;
            }
        }
        
        return true;
    }
    
    bool validatePattern(const ConfigValue& value) const {
        if (type == "string" && std::holds_alternative<std::string>(value)) {
            static std::regex patternCache;
            try {
                if (!patternCache.has_value()) {
                    patternCache = std::regex(validationPattern);
                }
                if (!std::regex_match(std::get<std::string>(value), patternCache.value())) {
                    validationError = "Value does not match pattern: " + validationPattern;
                    return false;
                }
            } catch (const std::regex_error&) {
                validationError = "Invalid validation pattern: " + validationPattern;
                return false;
            }
        }
        return true;
    }
    
    bool validateAllowedValues(const ConfigValue& value) const {
        if (type == "string" && std::holds_alternative<std::string>(value)) {
            if (std::find(allowedValues.begin(), allowedValues.end(), std::get<std::string>(value)) == allowedValues.end()) {
                validationError = "Value not in allowed values list";
                return false;
            }
        }
        else if (type == "int" && std::holds_alternative<int>(value)) {
            std::string valStr = std::to_string(std::get<int>(value));
            if (std::find(allowedValues.begin(), allowedValues.end(), valStr) == allowedValues.end()) {
                validationError = "Value not in allowed values list";
                return false;
            }
        }
        return true;
    }
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> errorDetails;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    void addErrorDetail(const std::string& key, const std::string& detail) {
        errorDetails[key] = detail;
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Validation Result:\n";
        oss << "  Valid: " << (isValid ? "Yes" : "No") << "\n";
        oss << "  Errors: " << errors.size() << "\n";
        oss << "  Warnings: " << warnings.size() << "\n";
        
        if (!errors.empty()) {
            oss << "\nErrors:\n";
            for (const auto& error : errors) {
                oss << "  - " << error << "\n";
            }
        }
        
        if (!warnings.empty()) {
            oss << "\nWarnings:\n";
            for (const auto& warning : warnings) {
                oss << "  - " << warning << "\n";
            }
        }
        
        return oss.str();
    }
};

// Configuration registry for schema validation
class ConfigRegistry {
public:
    static ConfigRegistry& getInstance();
    
    void registerConfig(const std::string& key, const ConfigSchema& schema);
    bool validate(const std::string& key, const ConfigValue& value) const;
    bool validateFull(const std::string& key, const ConfigValue& value) const;
    const ConfigSchema* getSchema(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;
    std::string getSchemaDescription() const;
    std::string getValidationReport(const std::map<std::string, ConfigValue>& values) const;
    std::vector<std::string> getValidationErrors(const std::map<std::string, ConfigValue>& values) const;
    bool hasSchema(const std::string& key) const;
    
private:
    ConfigRegistry() {}
    std::map<std::string, ConfigSchema> schemas;
};

// Configuration migration system
struct ConfigMigration {
    int fromVersion;
    int toVersion;
    std::function<void(Config&)> migrate;
    std::string description;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    double averageMemoryUsage;
    time_t lastModified;
    
    ConfigStats() : totalEntries(0), fileEntries(0), commandLineEntries(0), 
                   runtimeEntries(0), averageMemoryUsage(0), lastModified(0) {}
};