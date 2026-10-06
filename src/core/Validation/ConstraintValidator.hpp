#pragma once

#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "Config.hpp"

namespace nlm {

class ConstraintValidator {
public:
    ConstraintValidator() = default;

    // Type validation
    bool isValidType(const std::string& key, const ConfigValue& value, 
                    const std::string& expectedType) const;
    bool isValidArrayType(const std::string& key, const ConfigValue& value,
                         const std::string& expectedElementType) const;

    // Range validation
    bool isValidRange(const std::string& key, const ConfigValue& value,
                     double min, double max) const;

    // Format validation
    bool isValidPattern(const std::string& value, const std::string& pattern) const;
    bool isValidFormat(const std::string& key, const std::string& value,
                      const std::string& format) const;

    // Enum validation
    bool isValidEnum(const std::string& key, const ConfigValue& value,
                    const std::vector<std::string>& validValues) const;

    // Custom validation
    bool validateCustom(const std::string& key, const ConfigValue& value,
                       const std::function<bool(const ConfigValue&)>& validator) const;

    // Type checking helpers
    static bool isValidTypeString(const std::string& type);
    static bool getTypeFromString(const std::string& typeStr, 
                                 std::vector<std::type_index>& types);
    static std::string getTypeName(const std::type_index& typeIndex);
    static bool configValueMatchesType(const ConfigValue& value, 
                                       const std::type_index& expectedType);
};

} // namespace nlm
