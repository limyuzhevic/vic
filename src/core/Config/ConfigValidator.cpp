#include "ConfigValidator.hpp"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace nlm {

// ConfigValidator implementation
std::vector<ConfigError> ConfigValidator::validate(const std::map<std::string, std::any>& config,
                                                 const ConfigSchema& schema) {
    std::vector<ConfigError> errors;
    
    // Validate against schema
    std::vector<ConfigError> schemaErrors = schema.validate(config);
    errors.insert(errors.end(), schemaErrors.begin(), schemaErrors.end());
    
    // Validate range constraints
    for (const auto& param : schema.getParameters()) {
        auto it = config.find(param.name);
        if (it == config.end()) continue;
        
        const std::any& value = it->second;
        
        if (param.range) {
            std::vector<ConfigError> rangeErrors = validateRange(param.name, value, param.range.value());
            errors.insert(errors.end(), rangeErrors.begin(), rangeErrors.end());
        }
        
        if (param.choice && value.type() == typeid(std::string)) {
            std::vector<ConfigError> choiceErrors = validateChoice(param.name, std::any_cast<const std::string&>(value), param.choice.value());
            errors.insert(errors.end(), choiceErrors.begin(), choiceErrors.end());
        }
        
        // Validate vector/matrix specs
        if (param.vector) {
            std::vector<ConfigError> vectorErrors = validateVector(param.name, value, param.vector.value());
            errors.insert(errors.end(), vectorErrors.begin(), vectorErrors.end());
        }
        
        if (param.matrix) {
            std::vector<ConfigError> matrixErrors = validateMatrix(param.name, value, param.matrix.value());
            errors.insert(errors.end(), matrixErrors.begin(), matrixErrors.end());
        }
        
        // Validate dependencies
        if (param.dependency) {
            std::vector<ConfigError> depErrors = validateDependency(param.name, value, param.dependency.value(), config);
            errors.insert(errors.end(), depErrors.begin(), depErrors.end());
        }
    }
    
    return errors;
}

std::vector<ConfigError> ConfigValidator::validateRange(const std::string& key, const std::any& value,
                                                       const RangeSpec& range) const {
    std::vector<ConfigError> errors;
    
    try {
        double val = getDoubleValue(value);
        
        double min = parseDouble(range.min);
        double max = parseDouble(range.max);
        
        if (range.inclusive) {
            if (val < min || val > max) {
                errors.emplace_back(ConfigErrorType::RangeViolation, key, range.min + " to " + range.max,
                    std::to_string(val), "Value " + std::to_string(val) + " is outside range " +
                    range.min + " to " + range.max);
            }
        } else {
            if (val <= min || val >= max) {
                errors.emplace_back(ConfigErrorType::RangeViolation, key, "(" + range.min + ", " + range.max + ")",
                    std::to_string(val), "Value " + std::to_string(val) + " is outside range (" +
                    range.min + ", " + range.max + ")");
            }
        }
    } catch (...) {
        errors.emplace_back(ConfigErrorType::TypeMismatch, key, "numeric", getTypeName(value.type()),
            "Value cannot be compared with range constraints");
    }
    
    return errors;
}

std::vector<ConfigError> ConfigValidator::validateChoice(const std::string& key, const std::string& value,
                                                       const ChoiceSpec& choice) const {
    std::vector<ConfigError> errors;
    
    bool valid = false;
    for (const auto& option : choice.choices) {
        if (value == option) {
            valid = true;
            break;
        }
    }
    
    if (!valid) {
        errors.emplace_back(ConfigErrorType::TypeMismatch, key, "one of choices",
            value, "Value '" + value + "' is not a valid choice. Valid options: " +
            formatChoices(choice.choices));
    }
    
    return errors;
}

std::vector<ConfigError> ConfigValidator::validateVector(const std::string& key, const std::any& value,
                                                       const VectorSpec& vector) const {
    std::vector<ConfigError> errors;
    
    if (value.type() != typeid(std::vector<double>)) {
        errors.emplace_back(ConfigErrorType::TypeMismatch, key, "vector", getTypeName(value.type()),
            "Expected vector for parameter '" + key + "'");
        return errors;
    }
    
    const auto& vec = std::any_cast<const std::vector<double>&>(value);
    
    if (vec.size() != vector.dimensions) {
        errors.emplace_back(ConfigErrorType::RangeViolation, key, "size " + std::to_string(vector.dimensions),
            std::to_string(vec.size()), "Vector size must be " + std::to_string(vector.dimensions));
    }
    
    for (size_t i = 0; i < vec.size(); ++i) {
        std::vector<ConfigError> componentErrors = validateRange(
            key + "[" + std::to_string(i) + "]", vec[i], vector.componentRanges[i]);
        errors.insert(errors.end(), componentErrors.begin(), componentErrors.end());
    }
    
    return errors;
}

std::vector<ConfigError> ConfigValidator::validateMatrix(const std::string& key, const std::any& value,
                                                       const MatrixSpec& matrix) const {
    std::vector<ConfigError> errors;
    
    if (value.type() != typeid(std::vector<std::vector<double>>) &&
        value.type() != typeid(std::vector<std::vector<int>>)) {
        errors.emplace_back(ConfigErrorType::TypeMismatch, key, "matrix", getTypeName(value.type()),
            "Expected matrix for parameter '" + key + "'");
        return errors;
    }
    
    // Note: For simplicity, assuming vector<vector<double>>
    // Could be extended for other matrix types
    return errors;
}

std::vector<ConfigError> ConfigValidator::validateDependency(const std::string& key, const std::any& value,
                                                           const ParameterInfo& dependency,
                                                           const std::map<std::string, std::any>& config) const {
    std::vector<ConfigError> errors;
    
    auto depIt = config.find(dependency.name);
    if (depIt == config.end()) {
        return errors; // Let required parameter validation handle this
    }
    
    // Check dependency relationship
    if (dependency.name.empty()) {
        return errors; // No specific dependency to check
    }
    
    // For now, simple dependency: check that dependent parameter's value meets dependency's constraints
    if (dependency.range) {
        std::vector<ConfigError> rangeErrors = validateRange(key, value, dependency.range.value());
        errors.insert(errors.end(), rangeErrors.begin(), rangeErrors.end());
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::getConflictingParameters(const ConfigSchema& schema) const {
    std::vector<std::string> conflicts;
    
    for (const auto& param1 : schema.getParameters()) {
        for (const auto& param2 : schema.getParameters()) {
            if (param1.name == param2.name) continue;
            
            // Check for overlapping tags
            std::vector<std::string> commonTags;
            std::set_intersection(param1.tags.begin(), param1.tags.end(),
                                param2.tags.begin(), param2.tags.end(),
                                std::back_inserter(commonTags));
            
            if (!commonTags.empty()) {
                conflicts.push_back(param1.name + " and " + param2.name + " share tags: " + joinStrings(commonTags, ", "));
            }
            
            // Check for direct dependency conflicts
            if (param1.dependency && param1.dependency->name == param2.name) {
                conflicts.push_back(param1.name + " depends on " + param2.name);
            }
        }
    }
    
    return conflicts;
}

std::string ConfigValidator::formatChoices(const std::vector<std::string>& choices) const {
    std::ostringstream oss;
    for (size_t i = 0; i < choices.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << choices[i] << "\"";
    }
    return oss.str();
}

std::string ConfigValidator::joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) const {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << strings[i];
    }
    return oss.str();
}

std::string ConfigValidator::getTypeName(const std::type_info& type) const {
    std::string name = type.name();
    
    // Simplify type names
    if (name.find("int") != std::string::npos) {
        if (name.find("64") != std::string::npos) return "int64_t";
        return "int";
    } else if (name.find("double") != std::string::npos) {
        return "double";
    } else if (name.find("float") != std::string::npos) {
        return "float";
    } else if (name.find("bool") != std::string::npos) {
        return "bool";
    } else if (name.find("string") != std::string::npos) {
        return "string";
    } else if (name.find("vector") != std::string::npos) {
        return "vector";
    }
    
    return name;
}

double ConfigValidator::getDoubleValue(const std::any& value) const {
    if (value.type() == typeid(int)) {
        return std::any_cast<int>(value);
    } else if (value.type() == typeid(int64_t)) {
        return static_cast<double>(std::any_cast<int64_t>(value));
    } else if (value.type() == typeid(double)) {
        return std::any_cast<double>(value);
    } else if (value.type() == typeid(float)) {
        return static_cast<double>(std::any_cast<float>(value));
    } else if (value.type() == typeid(std::string)) {
        return std::stod(std::any_cast<std::string>(value));
    } else if (value.type() == typeid(std::vector<int>)) {
        const auto& vec = std::any_cast<const std::vector<int>&>(value);
        if (!vec.empty()) return static_cast<double>(vec[0]);
        return 0.0;
    } else if (value.type() == typeid(std::vector<double>)) {
        const auto& vec = std::any_cast<const std::vector<double>&>(value);
        if (!vec.empty()) return vec[0];
        return 0.0;
    }
    
    return 0.0;
}

double ConfigValidator::parseDouble(const std::string& str) const {
    return std::stod(str);
}

} // namespace nlm