#pragma once

#include <functional>
#include <string>
#include <vector>
#include <any>
#include <map>
#include "ConfigError.hpp"
#include "ConfigSchema.hpp"

namespace nlm {

// Common validation functions for configuration values

class ValidationRules {
public:
    using ValidationFunction = std::function<ValidationResult(const ConfigValue& value, const ConfigEntry* entry = nullptr)>;
    using TypeChecker = std::function<bool(const ConfigValue& value)>;
    using RangeValidator = std::function<bool(double value, const ConfigEntry* entry = nullptr)>;
    using CustomValidator = std::function<ValidationResult(const std::string& key, const ConfigValue& value, const ConfigEntry* entry)>;
    
    // Basic type validation functions
    static bool isInt(const ConfigValue& value) {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    }
    
    static bool isDouble(const ConfigValue& value) {
        return std::holds_alternative<double>(value);
    }
    
    static bool isBool(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
    
    static bool isString(const ConfigValue& value) {
        return std::holds_alternative<std::string>(value);
    }
    
    static bool isVector(const ConfigValue& value) {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    // Range validation functions
    static ValidationFunction createRangeValidator(double min, double max, const std::string& typeName = "numeric") {
        return [min, max, typeName](const ConfigValue& value, const ConfigEntry* entry) {
            try {
                double val = convertToDouble(value);
                if (val < min || val > max) {
                    std::ostringstream oss;
                    oss << typeName << " value " << val << " is out of range [" << min << ", " << max << "]";
                    return ValidationResult::failure(oss.str());
                }
                return ValidationResult::success();
            } catch (const TypeConversionError&) {
                return ValidationResult::failure("Cannot convert to " + typeName + " for range validation");
            }
        };
    }
    
    static ValidationFunction createPositiveValidator(double min = 0.0, const std::string& typeName = "numeric") {
        return [min, typeName](const ConfigValue& value, const ConfigEntry* entry) {
            try {
                double val = convertToDouble(value);
                if (val < min) {
                    std::ostringstream oss;
                    oss << typeName << " value " << val << " must be >= " << min;
                    return ValidationResult::failure(oss.str());
                }
                return ValidationResult::success();
            } catch (const TypeConversionError&) {
                return ValidationResult::failure("Cannot convert to " + typeName + " for positive validation");
            }
        };
    }
    
    static ValidationFunction createMinSizeValidator(size_t minSize) {
        return [minSize](const ConfigValue& value, const ConfigEntry* entry) {
            if (isVector(value)) {
                size_t size = getVectorSize(value);
                if (size < minSize) {
                    std::ostringstream oss;
                    oss << "Vector size " << size << " must be >= " << minSize;
                    return ValidationResult::failure(oss.str());
                }
            }
            return ValidationResult::success();
        };
    }
    
    static ValidationFunction createStringLengthValidator(size_t minLen, size_t maxLen = SIZE_MAX) {
        return [minLen, maxLen](const ConfigValue& value, const ConfigEntry* entry) {
            if (isString(value)) {
                size_t len = std::get<std::string>(value).length();
                if (len < minLen || (maxLen != SIZE_MAX && len > maxLen)) {
                    std::ostringstream oss;
                    oss << "String length " << len << " must be in range [" << minLen << ", " << maxLen << "]";
                    return ValidationResult::failure(oss.str());
                }
            }
            return ValidationResult::success();
        };
    }
    
    static ValidationFunction createPatternValidator(const std::string& pattern) {
        return [pattern](const ConfigValue& value, const ConfigEntry* entry) {
            if (isString(value)) {
                const std::string& str = std::get<std::string>(value);
                if (!std::regex_search(str, std::regex(pattern))) {
                    std::ostringstream oss;
                    oss << "String \"" << str << "\" does not match pattern \"" << pattern << "\"";
                    return ValidationResult::failure(oss.str());
                }
            }
            return ValidationResult::success();
        };
    }
    
    static ValidationFunction createEnumValidator(const std::vector<std::string>& allowedValues) {
        return [allowedValues](const ConfigValue& value, const ConfigEntry* entry) {
            if (isString(value)) {
                const std::string& str = std::get<std::string>(value);
                if (std::find(allowedValues.begin(), allowedValues.end(), str) == allowedValues.end()) {
                    std::ostringstream oss;
                    oss << "Value \"" << str << "\" is not in allowed values: ";
                    for (const auto& val : allowedValues) {
                        oss << "\"" << val << "\", ";
                    }
                    oss.seekp(-2, std::ios_base::end);
                    return ValidationResult::failure(oss.str());
                }
            }
            return ValidationResult::success();
        };
    }
    
    // Composite validation functions
    static ValidationFunction createCompositeValidator(const std::vector<ValidationFunction>& validators) {
        return [validators](const ConfigValue& value, const ConfigEntry* entry) {
            for (const auto& validator : validators) {
                ValidationResult result = validator(value, entry);
                if (!result.isValid) {
                    return result;
                }
            }
            return ValidationResult::success();
        };
    }
    
    static ValidationFunction createConditionalValidator(
        const std::string& dependentKey,
        const std::function<bool(const ConfigValue&)> condition,
        const ValidationFunction& validator) {
        return [dependentKey, condition, validator](const ConfigValue& value, const ConfigEntry* entry) {
            ValidationResult result = validator(value, entry);
            if (!result.isValid && condition(value)) {
                // If the condition is true, add context about the dependent key
                result.details["dependent_key"] = dependentKey;
            }
            return result;
        };
    }
    
    // Common predefined validation rules
    static const ValidationFunction IntRange(double min, double max) {
        return createRangeValidator(min, max, "integer");
    }
    
    static const ValidationFunction DoubleRange(double min, double max) {
        return createRangeValidator(min, max, "floating-point");
    }
    
    static const ValidationFunction PositiveInt(double min = 0.0) {
        return createPositiveValidator(min, "integer");
    }
    
    static const ValidationFunction PositiveDouble(double min = 0.0) {
        return createPositiveValidator(min, "floating-point");
    }
    
    static const ValidationFunction StringMinLength(size_t minLen) {
        return createStringLengthValidator(minLen);
    }
    
    static const ValidationFunction StringMaxLength(size_t maxLen) {
        return createStringLengthValidator(0, maxLen);
    }
    
    static const ValidationFunction ValidPattern(const std::string& pattern) {
        return createPatternValidator(pattern);
    }
    
    static const ValidationFunction AllowedValues(const std::vector<std::string>& values) {
        return createEnumValidator(values);
    }
    
    // Helper functions
    static double convertToDouble(const ConfigValue& value) {
        if (isInt(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else if (isDouble(value)) {
            return std::get<double>(value);
        } else {
            throw TypeConversionError("Cannot convert to double");
        }
    }
    
    static size_t getVectorSize(const ConfigValue& value) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value).size();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value).size();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value).size();
        }
        return 0;
    }
    
    static std::string getValueAsString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
};

} // namespace nlm