#include "ConstraintValidator.hpp"
#include <algorithm>
#include <typeinfo>

namespace nlm {

bool ConstraintValidator::isValidType(const std::string& key, const ConfigValue& value,
                                     const std::string& expectedType) const {
    if (!isValidTypeString(expectedType)) {
        return false;
    }

    auto typeIndex = getTypeFromString(expectedType);
    if (typeIndex.empty()) {
        return false;
    }

    return configValueMatchesType(value, typeIndex[0]);
}

bool ConstraintValidator::isValidArrayType(const std::string& key, const ConfigValue& value,
                                          const std::string& expectedElementType) const {
    if (!isValidTypeString(expectedElementType)) {
        return false;
    }

    auto elementTypeIndexes = getTypeFromString(expectedElementType);
    if (elementTypeIndexes.empty()) {
        return false;
    }

    // Check if value is an array type
    if (std::holds_alternative<std::vector<int>>(value) ||
        std::holds_alternative<std::vector<double>>(value) ||
        std::holds_alternative<std::vector<std::string>>(value)) {
        // For now, assume array type matches
        return true;
    }

    return false;
}

bool ConstraintValidator::isValidRange(const std::string& key, const ConfigValue& value,
                                       double min, double max) const {
    try {
        // Extract numeric value from ConfigValue
        if (std::holds_alternative<int>(value)) {
            double val = static_cast<double>(std::get<int>(value));
            return val >= min && val <= max;
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= min && val <= max;
        } else if (std::holds_alternative<int64_t>(value)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            return val >= min && val <= max;
        }
    } catch (...) {}

    return true; // Cannot validate - assume valid for non-numeric types
}

bool ConstraintValidator::isValidPattern(const std::string& value, const std::string& pattern) const {
    // Simple pattern matching for demonstration
    // In production, would use proper regex library
    if (pattern == "^[a-zA-Z0-9_]+$") {
        return std::all_of(value.begin(), value.end(),
                          [](char c) { return std::isalnum(c) || c == '_'; });
    } else if (pattern == "^[0-9]+$") {
        return std::all_of(value.begin(), value.end(), ::isdigit);
    } else if (pattern == "^[a-zA-Z]+$") {
        return std::all_of(value.begin(), value.end(), ::isalpha);
    }

    // Default: accept any value for unknown patterns
    return true;
}

bool ConstraintValidator::isValidFormat(const std::string& key, const std::string& value,
                                       const std::string& format) const {
    if (format == "email") {
        return value.find("@") != std::string::npos && value.find(".") != std::string::npos;
    } else if (format == "uri") {
        return value.find("://") != std::string::npos;
    } else if (format == "ipv4") {
        // Simple IPv4 format check
        size_t octets = std::count(value.begin(), value.end(), '.');
        return octets == 3 && value.find_first_not_of("0123456789.") == std::string::npos;
    }

    return true;
}

bool ConstraintValidator::isValidEnum(const std::string& key, const ConfigValue& value,
                                      const std::vector<std::string>& validValues) const {
    std::string strValue;

    try {
        if (std::holds_alternative<std::string>(value)) {
            strValue = std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            strValue = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            strValue = std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            strValue = std::get<bool>(value) ? "true" : "false";
        }
    } catch (...) {}

    return std::find(validValues.begin(), validValues.end(), strValue) != validValues.end();
}

bool ConstraintValidator::validateCustom(const std::string& key, const ConfigValue& value,
                                        const std::function<bool(const ConfigValue&)>& validator) const {
    try {
        return validator(value);
    } catch (...) {}

    return false;
}

bool ConstraintValidator::isValidTypeString(const std::string& typeStr) {
    static const std::vector<std::string> validTypes = {
        "int", "integer", "float", "double", "bool", "boolean",
        "string", "str", "array", "list", "vector", "object"
    };

    return std::find(validTypes.begin(), validTypes.end(), typeStr) != validTypes.end();
}

bool ConstraintValidator::getTypeFromString(const std::string& typeStr,
                                           std::vector<std::type_index>& types) {
    static const std::vector<std::pair<std::string, std::type_index>> typeMappings = {
        {"int", typeid(int)},
        {"integer", typeid(int)},
        {"float", typeid(double)},
        {"double", typeid(double)},
        {"bool", typeid(bool)},
        {"boolean", typeid(bool)},
        {"string", typeid(std::string)},
        {"str", typeid(std::string)}
    };

    types.clear();
    for (const auto& mapping : typeMappings) {
        if (mapping.first == typeStr) {
            types.push_back(mapping.second);
            return true;
        }
    }

    return false;
}

std::string ConstraintValidator::getTypeName(const std::type_index& typeIndex) {
    if (typeIndex == typeid(int) || typeIndex == typeid(int64_t)) {
        return "int";
    } else if (typeIndex == typeid(double) || typeIndex == typeid(float)) {
        return "float";
    } else if (typeIndex == typeid(bool)) {
        return "bool";
    } else if (typeIndex == typeid(std::string)) {
        return "string";
    } else if (typeIndex == typeid(std::vector<int>)) {
        return "vector<int>";
    } else if (typeIndex == typeid(std::vector<double>)) {
        return "vector<double>";
    } else if (typeIndex == typeid(std::vector<std::string>)) {
        return "vector<string>";
    }

    return "unknown";
}

bool ConstraintValidator::configValueMatchesType(const ConfigValue& value,
                                                 const std::type_index& expectedType) {
    if (expectedType == typeid(int)) {
        return std::holds_alternative<int>(value);
    } else if (expectedType == typeid(double)) {
        return std::holds_alternative<double>(value);
    } else if (expectedType == typeid(bool)) {
        return std::holds_alternative<bool>(value);
    } else if (expectedType == typeid(std::string)) {
        return std::holds_alternative<std::string>(value);
    } else if (expectedType == typeid(std::vector<int>)) {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (expectedType == typeid(std::vector<double>)) {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (expectedType == typeid(std::vector<std::string>)) {
        return std::holds_alternative<std::vector<std::string>>(value);
    }

    return false;
}

} // namespace nlm
