#include "ConfigSchema.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <regex>

namespace nlm {

ConfigSchema* ConfigSchema::instance = nullptr;

void ConfigSchema::registerParameter(const ConfigParameterDefinition& param) {
    parameters[param.name] = param;
}

ConfigParameterDefinition ConfigSchema::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        return it->second;
    }
    return ConfigParameterDefinition();
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

void ConfigSchema::clear() {
    parameters.clear();
}

namespace {
    bool validateInt(const std::string& value, const ConfigParameterDefinition& param) {
        try {
            size_t pos;
            int val = std::stoi(value, &pos);
            if (pos != value.size()) return false;
            
            if (!param.minValue.empty()) {
                int minVal = std::stoi(param.minValue);
                if (val < minVal) return false;
            }
            if (!param.maxValue.empty()) {
                int maxVal = std::stoi(param.maxValue);
                if (val > maxVal) return false;
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateDouble(const std::string& value, const ConfigParameterDefinition& param) {
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            if (pos != value.size()) return false;
            
            if (!param.minValue.empty()) {
                double minVal = std::stod(param.minValue);
                if (val < minVal) return false;
            }
            if (!param.maxValue.empty()) {
                double maxVal = std::stod(param.maxValue);
                if (val > maxVal) return false;
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateBool(const std::string& value) {
        return value == "true" || value == "false" || value == "1" || value == "0";
    }
    
    bool validateString(const std::string& value, const ConfigParameterDefinition& param) {
        if (!param.validationRegex.empty()) {
            std::regex regex(param.validationRegex);
            return std::regex_match(value, regex);
        }
        return !value.empty();
    }
    
    bool validateVector(const std::string& value, ConfigParameterType type) {
        if (value.empty() || value.front() != '[' || value.back() != ']') {
            return false;
        }
        return true;
    }
}

bool ConfigSchema::validateParameter(const std::string& name, const std::string& value, 
                                    ConfigParameterType expectedType) const {
    auto param = getParameter(name);
    if (param.name.empty() && expectedType != ConfigParameterType::String) {
        return false;
    }
    
    switch (expectedType) {
        case ConfigParameterType::Int:
            return validateInt(value, param);
        case ConfigParameterType::Int64:
            return validateInt(value, param);
        case ConfigParameterType::Double:
            return validateDouble(value, param);
        case ConfigParameterType::Bool:
            return validateBool(value);
        case ConfigParameterType::String:
            return validateString(value, param);
        case ConfigParameterType::VectorInt:
            return validateVector(value, expectedType);
        case ConfigParameterType::VectorDouble:
            return validateVector(value, expectedType);
        case ConfigParameterType::VectorString:
            return validateVector(value, expectedType);
        default:
            return false;
    }
}

bool ConfigSchema::convertValue(const std::string& valueStr, ConfigParameterType type,
                               ConfigValue& outValue) const {
    try {
        switch (type) {
            case ConfigParameterType::Int: {
                size_t pos;
                int val = std::stoi(valueStr, &pos);
                if (pos != valueStr.size()) return false;
                outValue = val;
                break;
            }
            case ConfigParameterType::Int64: {
                size_t pos;
                int64_t val = std::stoll(valueStr, &pos);
                if (pos != valueStr.size()) return false;
                outValue = val;
                break;
            }
            case ConfigParameterType::Double: {
                size_t pos;
                double val = std::stod(valueStr, &pos);
                if (pos != valueStr.size()) return false;
                outValue = val;
                break;
            }
            case ConfigParameterType::Bool: {
                if (valueStr == "true" || valueStr == "1") {
                    outValue = true;
                } else if (valueStr == "false" || valueStr == "0") {
                    outValue = false;
                } else {
                    return false;
                }
                break;
            }
            case ConfigParameterType::String: {
                outValue = valueStr;
                break;
            }
            case ConfigParameterType::VectorInt: {
                std::vector<int> vec;
                std::istringstream iss(valueStr.substr(1, valueStr.size() - 2));
                std::string token;
                while (std::getline(iss, token, ',')) {
                    size_t pos;
                    int val = std::stoi(token, &pos);
                    if (pos != token.size()) return false;
                    vec.push_back(val);
                }
                outValue = vec;
                break;
            }
            case ConfigParameterType::VectorDouble: {
                std::vector<double> vec;
                std::istringstream iss(valueStr.substr(1, valueStr.size() - 2));
                std::string token;
                while (std::getline(iss, token, ',')) {
                    size_t pos;
                    double val = std::stod(token, &pos);
                    if (pos != token.size()) return false;
                    vec.push_back(val);
                }
                outValue = vec;
                break;
            }
            case ConfigParameterType::VectorString: {
                std::vector<std::string> vec;
                std::istringstream iss(valueStr.substr(1, valueStr.size() - 2));
                std::string token;
                while (std::getline(iss, token, ',')) {
                    if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
                        token = token.substr(1, token.size() - 2);
                    }
                    vec.push_back(token);
                }
                outValue = vec;
                break;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::optional<ConfigValue> ConfigSchema::getDefaultValue(const std::string& name) const {
    auto it = parameters.find(name);
    if (it == parameters.end()) {
        return std::nullopt;
    }
    
    const auto& param = it->second;
    if (!param.defaultValue) {
        return std::nullopt;
    }
    
    ConfigValue defaultValue;
    if (convertValue(*param.defaultValue, param.type, defaultValue)) {
        return defaultValue;
    }
    
    return std::nullopt;
}

} // namespace nlm
