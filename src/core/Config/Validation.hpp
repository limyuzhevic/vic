// Configuration validation system for NLM
// Provides schema-based validation and type checking for configuration values

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <optional>
#include <variant>
#include <typeindex>
#include <stdexcept>
#include <any>
#include <utility>
#include "Config.hpp"

namespace nlm {

// Validation error types
class ValidationError : public std::runtime_error {
public:
    enum class ErrorType {
        MissingRequiredKey,
        InvalidType,
        InvalidRange,
        InvalidValue,
        MissingFile,
        ParseError
    };

    ValidationError(ErrorType type, const std::string& message, const std::string& key = "")
        : std::runtime_error(message), errorType(type), key(key) {}

    ErrorType getType() const { return errorType; }
    const std::string& getKey() const { return key; }

private:
    ErrorType errorType;
    std::string key;
};

// Validation result
struct ValidationResult {
    bool valid = true;
    std::vector<ValidationError> errors;
    std::vector<std::string> warnings;
    
    void addError(ValidationError::ErrorType type, const std::string& message, const std::string& key = "") {
        valid = false;
        errors.emplace_back(type, message, key);
    }
    
    void addWarning(const std::string& message) {
        warnings.push_back(message);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getErrorSummary() const {
        std::string result;
        for (const auto& error : errors) {
            if (!result.empty()) result += "; ";
            result += error.what();
        }
        return result;
    }
};

// Type validator function
using TypeValidator = std::function<bool(const ConfigValue&)>;

// Range validator for numeric types
struct RangeValidator {
    enum class BoundType { Inclusive, Exclusive };
    BoundType minType = BoundType::Inclusive;
    BoundType maxType = BoundType::Inclusive;
    double minValue = 0.0;
    double maxValue = 0.0;
    
    bool validate(const ConfigValue& value) const;
};

// Custom validator function
using CustomValidator = std::function<ValidationResult(const Config& config, const std::string& key, const ConfigValue& value)>;

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    bool required = false;
    std::string description;
    TypeValidator typeValidator;
    std::optional<RangeValidator> rangeValidator;
    CustomValidator customValidator;
    
    ConfigSchemaEntry(const std::string& k, const std::type_index& t, bool req = false, 
                     const std::string& desc = "", const TypeValidator& tv = nullptr)
        : key(k), type(t), required(req), description(desc), typeValidator(tv) {}
    
    bool validate(const ConfigValue& value) const;
};

// Configuration schema
class ConfigSchema {
private:
    std::vector<ConfigSchemaEntry> entries;
    std::unordered_map<std::string, ConfigSchemaEntry*> entryMap;
    
public:
    // Add a schema entry for basic type validation
    template<typename T>
    ConfigSchema& addEntry(const std::string& key, bool required, 
                          const std::string& description = "") {
        TypeValidator validator = [](const ConfigValue& val) {
            try {
                std::get<T>(val);
                return true;
            } catch (const std::bad_variant_access&) {
                return false;
            }
        };
        
        entries.emplace_back(key, typeid(T), required, description, validator);
        entryMap[key] = &entries.back();
        return *this;
    }
    
    // Add an entry with range validation
    ConfigSchema& addEntry(const std::string& key, bool required,
                          const RangeValidator& range, const std::string& description = "") {
        // Find corresponding basic type based on range
        TypeValidator validator;
        if (range.minValue == range.maxValue) {
            // For boolean-like values
            validator = [range](const ConfigValue& val) {
                if constexpr (std::is_same_v<bool, T>) {
                    try {
                        bool b = std::get<bool>(val);
                        if (range.minType == RangeValidator::BoundType::Inclusive && range.maxType == RangeValidator::BoundType::Inclusive) {
                            return range.minValue <= static_cast<double>(b) && static_cast<double>(b) <= range.maxValue;
                        }
                        return true;
                    } catch (const std::bad_variant_access&) {
                        return false;
                    }
                }
                return false;
            };
        } else {
            // For numeric types
            validator = [range](const ConfigValue& val) {
                try {
                    double d = std::get<double>(val);
                    if (range.minType == RangeValidator::BoundType::Inclusive && range.maxType == RangeValidator::BoundType::Inclusive) {
                        return range.minValue <= d && d <= range.maxValue;
                    } else if (range.minType == RangeValidator::BoundType::Inclusive && range.maxType == RangeValidator::BoundType::Exclusive) {
                        return range.minValue <= d && d < range.maxValue;
                    } else if (range.minType == RangeValidator::BoundType::Exclusive && range.maxType == RangeValidator::BoundType::Inclusive) {
                        return d < range.minValue && d <= range.maxValue;
                    } else {
                        return d < range.minValue && d < range.maxValue;
                    }
                } catch (const std::bad_variant_access&) {
                    return false;
                }
            };
        }
        
        entries.emplace_back(key, typeid(double), required, description, validator);
        entryMap[key] = &entries.back();
        return *this;
    }
    
    // Add an entry with custom validator
    ConfigSchema& addEntry(const std::string& key, bool required,
                          const CustomValidator& validator,
                          const std::string& description = "") {
        entries.emplace_back(key, typeid(ConfigValue), required, description, validator);
        entryMap[key] = &entries.back();
        return *this;
    }
    
    // Validate a configuration against this schema
    ValidationResult validate(const Config& config) const {
        ValidationResult result;
        
        // Check for required keys
        for (const auto& entry : entries) {
            if (entry.required && !config.has(entry.key)) {
                result.addError(ValidationError::ErrorType::MissingRequiredKey,
                               "Required configuration key missing: " + entry.key, entry.key);
            }
        }
        
        // Validate types and ranges for present keys
        for (const auto& entry : entries) {
            if (config.has(entry.key)) {
                auto value = config.get<ConfigValue>(entry.key);
                if (!value) {
                    result.addError(ValidationError::ErrorType::InvalidType,
                                   "Failed to retrieve configuration value for key: " + entry.key, entry.key);
                    continue;
                }
                
                // Type validation
                if (entry.typeValidator) {
                    if (!entry.typeValidator(*value)) {
                        result.addError(ValidationError::ErrorType::InvalidType,
                                       "Invalid type for key " + entry.key + ". Expected: " + 
                                       getTypeName(entry.type), entry.key);
                    }
                }
                
                // Range validation
                if (entry.rangeValidator && entry.typeValidator) {
                    if (entry.typeValidator(*value) && !entry.rangeValidator->validate(*value)) {
                        result.addError(ValidationError::ErrorType::InvalidRange,
                                       "Invalid range for key " + entry.key, entry.key);
                    }
                }
                
                // Custom validation
                if (entry.customValidator) {
                    ValidationResult customResult = entry.customValidator(config, entry.key, *value);
                    if (customResult.hasErrors()) {
                        for (const auto& error : customResult.errors) {
                            result.addError(error.getType(), error.what(), error.getKey());
                        }
                    }
                    if (customResult.hasWarnings()) {
                        for (const auto& warning : customResult.warnings) {
                            result.addWarning(warning);
                        }
                    }
                }
            }
        }
        
        return result;
    }
    
    // Get type name as string
    static std::string getTypeName(const std::type_index& typeIndex) {
        if (typeIndex == typeid(int)) return "int";
        if (typeIndex == typeid(int64_t)) return "int64_t";
        if (typeIndex == typeid(double)) return "double";
        if (typeIndex == typeid(bool)) return "bool";
        if (typeIndex == typeid(std::string)) return "string";
        if (typeIndex == typeid(std::vector<int>)) return "std::vector<int>";
        if (typeIndex == typeid(std::vector<double>)) return "std::vector<double>";
        if (typeIndex == typeid(std::vector<std::string>)) return "std::vector<string>";
        if (typeIndex == typeid(ConfigValue)) return "ConfigValue";
        return "unknown";
    }
};