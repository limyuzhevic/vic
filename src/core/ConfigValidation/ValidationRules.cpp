#include "ValidationRules.hpp"
#include "ConfigError.hpp"
#include <numeric>
#include <algorithm>
#include <iterator>

namespace nlm {
    namespace ValidationRulesImpl {
        
        // Implementation of helper functions
        inline std::string trim(const std::string& str) {
            size_t start = str.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) return "";
            size_t end = str.find_last_not_of(" \t\r\n");
            return str.substr(start, end - start + 1);
        }
        
        inline std::string toLower(const std::string& str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }
        
    } // namespace ValidationRulesImpl
    
    // Implementation of ValidationRules methods
    
    // Type checking functions are already implemented in the header
    
    // Range validation implementation
    ValidationFunction ValidationRules::createRangeValidator(double min, double max, const std::string& typeName) {
        return [min, max, typeName](const ConfigValue& value, const ConfigEntry* entry) {
            try {
                double val = ValidationRulesImpl::convertToDouble(value);
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
    
    ValidationFunction ValidationRules::createPositiveValidator(double min, const std::string& typeName) {
        return [min, typeName](const ConfigValue& value, const ConfigEntry* entry) {
            try {
                double val = ValidationRulesImpl::convertToDouble(value);
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
    
    ValidationFunction ValidationRules::createMinSizeValidator(size_t minSize) {
        return [minSize](const ConfigValue& value, const ConfigEntry* entry) {
            if (ValidationRulesImpl::isVector(value)) {
                size_t size = ValidationRulesImpl::getVectorSize(value);
                if (size < minSize) {
                    std::ostringstream oss;
                    oss << "Vector size " << size << " must be >= " << minSize;
                    return ValidationResult::failure(oss.str());
                }
            }
            return ValidationResult::success();
        };
    }
    
    ValidationFunction ValidationRules::createStringLengthValidator(size_t minLen, size_t maxLen) {
        return [minLen, maxLen](const ConfigValue& value, const ConfigEntry* entry) {
            if (ValidationRulesImpl::isString(value)) {
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
    
    ValidationFunction ValidationRules::createPatternValidator(const std::string& pattern) {
        return [pattern](const ConfigValue& value, const ConfigEntry* entry) {
            if (ValidationRulesImpl::isString(value)) {
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
    
    ValidationFunction ValidationRules::createEnumValidator(const std::vector<std::string>& allowedValues) {
        return [allowedValues](const ConfigValue& value, const ConfigEntry* entry) {
            if (ValidationRulesImpl::isString(value)) {
                const std::string& str = std::get<std::string>(value);
                if (std::find(allowedValues.begin(), allowedValues.end(), str) == allowedValues.end()) {
                    std::ostringstream oss;
                    oss << "Value \"" << str << "\" is not in allowed values: ";
                    for (size_t i = 0; i < allowedValues.size(); ++i) {
                        oss << "\"" << allowedValues[i] << "\"";
                        if (i < allowedValues.size() - 1) oss << ", ";
                    }
                    return ValidationResult::failure(oss.str());
                }
            }
            return ValidationResult::success();
        };
    }
    
    ValidationFunction ValidationRules::createCompositeValidator(const std::vector<ValidationFunction>& validators) {
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
    
    ValidationFunction ValidationRules::createConditionalValidator(
        const std::string& dependentKey,
        const std::function<bool(const ConfigValue&)> condition,
        const ValidationFunction& validator) {
        return [dependentKey, condition, validator](const ConfigValue& value, const ConfigEntry* entry) {
            ValidationResult result = validator(value, entry);
            if (!result.isValid && condition(value)) {
                result.details["dependent_key"] = dependentKey;
            }
            return result;
        };
    }
    
    // Predefined validation rules implementation
    const ValidationFunction ValidationRules::IntRange(double min, double max) {
        return createRangeValidator(min, max, "integer");
    }
    
    const ValidationFunction ValidationRules::DoubleRange(double min, double max) {
        return createRangeValidator(min, max, "floating-point");
    }
    
    const ValidationFunction ValidationRules::PositiveInt(double min) {
        return createPositiveValidator(min, "integer");
    }
    
    const ValidationFunction ValidationRules::PositiveDouble(double min) {
        return createPositiveValidator(min, "floating-point");
    }
    
    const ValidationFunction ValidationRules::StringMinLength(size_t minLen) {
        return createStringLengthValidator(minLen);
    }
    
    const ValidationFunction ValidationRules::StringMaxLength(size_t maxLen) {
        return createStringLengthValidator(0, maxLen);
    }
    
    const ValidationFunction ValidationRules::ValidPattern(const std::string& pattern) {
        return createPatternValidator(pattern);
    }
    
    const ValidationFunction ValidationRules::AllowedValues(const std::vector<std::string>& values) {
        return createEnumValidator(values);
    }
    
    // Helper functions implementation
    double ValidationRules::convertToDouble(const ConfigValue& value) {
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
    
    size_t ValidationRules::getVectorSize(const ConfigValue& value) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value).size();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value).size();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value).size();
        }
        return 0;
    }
    
    std::string ValidationRules::getValueAsString(const ConfigValue& value) {
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
    
} // namespace nlm