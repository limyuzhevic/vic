#include "ConfigSchema.hpp"
#include <regex>
#include <sstream>
#include <algorithm>

namespace nlm {

// Helper function for trimming strings
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

void ConfigSchema::addParameter(const ConfigParameter& param) {
    parameters[param.name] = param;
}

const ConfigParameter* ConfigSchema::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        return &it->second;
    }
    return nullptr;
}

bool ConfigSchema::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

std::vector<std::string> ConfigSchema::getParameterNames() const {
    std::vector<std::string> names;
    names.reserve(parameters.size());
    for (const auto& pair : parameters) {
        names.push_back(pair.first);
    }
    return names;
}

bool ConfigSchema::validate(const std::string& paramName, const std::any& value, 
                          std::string& error) const {
    const ConfigParameter* param = getParameter(paramName);
    if (!param) {
        error = "Unknown parameter: '" + paramName + "'";
        return false;
    }
    
    // Check if value is provided when parameter is required
    if (param->required && !value.has_value()) {
        error = "Parameter '" + paramName + "' is required but not provided";
        return false;
    }
    
    if (!value.has_value()) {
        // If not required and no value provided, it's valid (will use default)
        return true;
    }
    
    // Validate type
    if (!isCorrectType(value, param->type)) {
        error = "Type mismatch for parameter '" + paramName + "': expected " + 
                getTypeName(param->type) + " but got " + typeToString(value);
        return false;
    }
    
    // Validate against range/constraints
    switch (param->type) {
        case ConfigType::INT:
        case ConfigType::INT64:
        case ConfigType::DOUBLE:
        case ConfigType::FLOAT:
            if (!validateAgainstRange(value, param->numberRange, error)) {
                error = "Range validation failed for parameter '" + paramName + "': " + error;
                return false;
            }
            break;
        case ConfigType::STRING:
            if (!validateString(value, param->stringConstraints, error)) {
                error = "String validation failed for parameter '" + paramName + "': " + error;
                return false;
            }
            break;
        case ConfigType::VECTOR_INT:
        case ConfigType::VECTOR_DOUBLE:
        case ConfigType::VECTOR_STRING:
            if (!validateVector(value, param->vectorConstraints, error)) {
                error = "Vector validation failed for parameter '" + paramName + "': " + error;
                return false;
            }
            break;
        case ConfigType::BOOL:
            // No additional constraints for bool
            break;
    }
    
    // Run custom validator if provided
    if (param->customValidator && !param->customValidator(value, error)) {
        error = "Custom validation failed for parameter '" + paramName + "': " + error;
        return false;
    }
    
    return true;
}

bool ConfigSchema::convertString(const std::string& paramName, const std::string& strValue,
                                std::any& outValue, std::string& error) const {
    const ConfigParameter* param = getParameter(paramName);
    if (!param) {
        error = "Unknown parameter: '" + paramName + "'";
        return false;
    }
    
    if (!convertToType(param->type, strValue, outValue)) {
        error = "Cannot convert string to " + getTypeName(param->type) + " for parameter '" + 
                paramName + "': value = '" + strValue + "'";
        return false;
    }
    
    // Validate the converted value
    return validate(paramName, outValue, error);
}

std::string ConfigSchema::getDescription(const std::string& name) const {
    const ConfigParameter* param = getParameter(name);
    return param ? param->description : "";
}

bool ConfigSchema::isRequired(const std::string& name) const {
    const ConfigParameter* param = getParameter(name);
    return param ? param->required : false;
}

std::any ConfigSchema::getDefaultValue(const std::string& name) const {
    const ConfigParameter* param = getParameter(name);
    return param ? param->defaultValue : std::any();
}

std::string ConfigSchema::getTypeName(ConfigType type) {
    switch (type) {
        case ConfigType::INT: return "int";
        case ConfigType::INT64: return "int64_t";
        case ConfigType::DOUBLE: return "double";
        case ConfigType::FLOAT: return "float";
        case ConfigType::BOOL: return "bool";
        case ConfigType::STRING: return "string";
        case ConfigType::VECTOR_INT: return "vector<int>";
        case ConfigType::VECTOR_DOUBLE: return "vector<double>";
        case ConfigType::VECTOR_STRING: return "vector<string>";
        default: return "unknown";
    }
}

bool ConfigSchema::convertToType(ConfigType type, const std::string& str, std::any& outValue) {
    std::istringstream iss(str);
    
    switch (type) {
        case ConfigType::INT: {
            int val;
            if (iss >> val && iss.eof()) {
                outValue = val;
                return true;
            }
            break;
        }
        case ConfigType::INT64: {
            int64_t val;
            if (iss >> val && iss.eof()) {
                outValue = val;
                return true;
            }
            break;
        }
        case ConfigType::DOUBLE: {
            double val;
            if (iss >> val && iss.eof()) {
                outValue = val;
                return true;
            }
            break;
        }
        case ConfigType::FLOAT: {
            float val;
            if (iss >> val && iss.eof()) {
                outValue = val;
                return true;
            }
            break;
        }
        case ConfigType::BOOL: {
            bool val;
            std::string lower = str;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
                val = true;
                outValue = val;
                return true;
            } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
                val = false;
                outValue = val;
                return true;
            }
            break;
        }
        case ConfigType::STRING: {
            outValue = str;
            return true;
        }
        case ConfigType::VECTOR_INT: {
            // Parse as comma-separated list
            std::vector<int> result;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                int val;
                if (std::istringstream(token) >> val) {
                    result.push_back(val);
                } else {
                    return false;
                }
            }
            outValue = result;
            return true;
        }
        case ConfigType::VECTOR_DOUBLE: {
            std::vector<double> result;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                double val;
                if (std::istringstream(token) >> val) {
                    result.push_back(val);
                } else {
                    return false;
                }
            }
            outValue = result;
            return true;
        }
        case ConfigType::VECTOR_STRING: {
            // Parse as comma-separated list
            std::vector<std::string> result;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, ',')) {
                result.push_back(trim(token));
            }
            outValue = result;
            return true;
        }
    }
    return false;
}

bool ConfigSchema::validateAgainstRange(const std::any& value, const NumberRange& range, std::string& error) {
    double val = 0.0;
    
    if (value.type() == typeid(int)) {
        val = std::any_cast<int>(value);
    } else if (value.type() == typeid(int64_t)) {
        val = static_cast<double>(std::any_cast<int64_t>(value));
    } else if (value.type() == typeid(double)) {
        val = std::any_cast<double>(value);
    } else if (value.type() == typeid(float)) {
        val = static_cast<double>(std::any_cast<float>(value));
    } else {
        error = "Value is not a numeric type";
        return false;
    }
    
    if (range.inclusive) {
        if (val < range.min || val > range.max) {
            error = "Value " + std::to_string(val) + " is outside range [" + 
                    std::to_string(range.min) + ", " + std::to_string(range.max) + "]";
            return false;
        }
    } else {
        if (val <= range.min || val >= range.max) {
            error = "Value " + std::to_string(val) + " is outside range (" + 
                    std::to_string(range.min) + ", " + std::to_string(range.max) + ")";
            return false;
        }
    }
    
    return true;
}

bool ConfigSchema::validateString(const std::any& value, const StringConstraints& constraints, std::string& error) {
    if (value.type() != typeid(std::string)) {
        error = "Value is not a string";
        return false;
    }
    
    const std::string& str = std::any_cast<const std::string&>(value);
    
    if (str.length() < constraints.minLength) {
        error = "String length " + std::to_string(str.length()) + " is less than minimum " + 
                std::to_string(constraints.minLength);
        return false;
    }
    
    if (str.length() > constraints.maxLength) {
        error = "String length " + std::to_string(str.length()) + " exceeds maximum " + 
                std::to_string(constraints.maxLength);
        return false;
    }
    
    if (!constraints.pattern.empty()) {
        std::regex regex(constraints.pattern);
        if (!std::regex_match(str, regex)) {
            error = "String does not match pattern: '" + constraints.pattern + "'";
            return false;
        }
    }
    
    if (!constraints.allowedValues.empty()) {
        bool found = false;
        for (const auto& allowed : constraints.allowedValues) {
            if (str == allowed) {
                found = true;
                break;
            }
        }
        if (!found) {
            error = "String value '" + str + "' is not in allowed values: ";
            bool first = true;
            for (const auto& allowed : constraints.allowedValues) {
                if (!first) error += ", ";
                error += "'" + allowed + "'";
                first = false;
            }
            return false;
        }
    }
    
    return true;
}

bool ConfigSchema::validateVector(const std::any& value, const VectorConstraints& constraints, std::string& error) {
    // Determine vector size based on type
    size_t size = 0;
    if (value.type() == typeid(std::vector<int>)) {
        size = std::any_cast<const std::vector<int>&>(value).size();
    } else if (value.type() == typeid(std::vector<double>)) {
        size = std::any_cast<const std::vector<double>&>(value).size();
    } else if (value.type() == typeid(std::vector<std::string>)) {
        size = std::any_cast<const std::vector<std::string>&>(value).size();
    } else {
        error = "Value is not a vector type";
        return false;
    }
    
    if (size < constraints.minSize) {
        error = "Vector size " + std::to_string(size) + " is less than minimum " + 
                std::to_string(constraints.minSize);
        return false;
    }
    
    if (size > constraints.maxSize) {
        error = "Vector size " + std::to_string(size) + " exceeds maximum " + 
                std::to_string(constraints.maxSize);
        return false;
    }
    
    return true;
}

bool ConfigSchema::isCorrectType(const std::any& value, ConfigType type) const {
    if (!value.has_value()) return true; // Optional values are valid
    
    switch (type) {
        case ConfigType::INT: return value.type() == typeid(int);
        case ConfigType::INT64: return value.type() == typeid(int64_t);
        case ConfigType::DOUBLE: return value.type() == typeid(double);
        case ConfigType::FLOAT: return value.type() == typeid(float);
        case ConfigType::BOOL: return value.type() == typeid(bool);
        case ConfigType::STRING: return value.type() == typeid(std::string);
        case ConfigType::VECTOR_INT: return value.type() == typeid(std::vector<int>);
        case ConfigType::VECTOR_DOUBLE: return value.type() == typeid(std::vector<double>);
        case ConfigType::VECTOR_STRING: return value.type() == typeid(std::vector<std::string>);
        default: return false;
    }
}

std::string ConfigSchema::typeToString(const std::any& value) const {
    if (value.type() == typeid(int)) return "int";
    if (value.type() == typeid(int64_t)) return "int64_t";
    if (value.type() == typeid(double)) return "double";
    if (value.type() == typeid(float)) return "float";
    if (value.type() == typeid(bool)) return "bool";
    if (value.type() == typeid(std::string)) return "string";
    if (value.type() == typeid(std::vector<int>)) return "vector<int>";
    if (value.type() == typeid(std::vector<double>)) return "vector<double>";
    if (value.type() == typeid(std::vector<std::string>)) return "vector<string>";
    return "unknown";
}

} // namespace nlm