#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Config.hpp"

namespace nlm {

class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ValidationResult validate(const Config& config) const;
    
    // Parameter validation
    bool validateParameter(const std::string& key, const ConfigValue& value) const;
    
    // Type validation
    template<typename T>
    bool validateType(const std::string& key, const ConfigValue& value) const;
    
private:
    // Parameter definitions
    struct ParameterInfo {
        std::string name;
        std::string type;
        std::string description;
        bool required;
        std::vector<std::string> validValues;
        std::pair<double, double> range;  // min, max for numeric types
    };
    
    std::unordered_map<std::string, ParameterInfo> parameterDefinitions_;
    
    void initializeParameterDefinitions();
};

} // namespace nlm
