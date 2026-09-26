#pragma once

#include <string>
#include <vector>
#include <variant>
#include <regex>
#include <cmath>
#include "Schema.hpp"

namespace nlm {

class ConfigurationValidator {
public:
    static ValidationResult validateParameter(const std::string& key, const ConfigValue& value, 
                                              const ParameterMeta& meta);
    
    static ValidationResult validateType(const std::string& key, const ConfigValue& value, 
                                         const ParameterMeta& meta);
    
    static ValidationResult validateConstraints(const std::string& key, const ConfigValue& value, 
                                                 const ParameterMeta& meta);
    
    static ValidationResult validateCustomRule(const std::string& key, const ConfigValue& value,
                                               const ParameterMeta& meta);
    
    static std::string typeToString(ParameterType type);
    static bool typeMatches(const ConfigValue& value, ParameterType type);
    
    static ValidationResult validateInt(const std::string& key, const ConfigValue& value,
                                        const ParameterMeta& meta);
    static ValidationResult validateInt64(const std::string& key, const ConfigValue& value,
                                          const ParameterMeta& meta);
    static ValidationResult validateDouble(const std::string& key, const ConfigValue& value,
                                           const ParameterMeta& meta);
    static ValidationResult validateBool(const std::string& key, const ConfigValue& value,
                                         const ParameterMeta& meta);
    static ValidationResult validateString(const std::string& key, const ConfigValue& value,
                                           const ParameterMeta& meta);
    static ValidationResult validateVectorInt(const std::string& key, const ConfigValue& value,
                                              const ParameterMeta& meta);
    static ValidationResult validateVectorDouble(const std::string& key, const ConfigValue& value,
                                                 const ParameterMeta& meta);
    static ValidationResult validateVectorString(const std::string& key, const ConfigValue& value,
                                                 const ParameterMeta& meta);
    
    static ValidationResult validateRange(const std::string& key, const ConfigValue& value,
                                          const ParameterMeta& meta);
    static ValidationResult validateLength(const std::string& key, const ConfigValue& value,
                                           const ParameterMeta& meta);
    static ValidationResult validateSize(const std::string& key, const ConfigValue& value,
                                         const ParameterMeta& meta);
    static ValidationResult validateAllowedValues(const std::string& key, const ConfigValue& value,
                                                   const ParameterMeta& meta);
    
    static ValidationResult validatePattern(const std::string& key, const ConfigValue& value,
                                            const std::string& pattern);
    
    static ValidationResult validateCustom(const std::string& key, const ConfigValue& value,
                                           const std::function<ValidationResult(const std::string&, const ConfigValue&)>& rule);
};

} // namespace nlm
