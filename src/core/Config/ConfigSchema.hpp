#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <variant>
#include <vector>
#include <iostream>

namespace nlm {

enum class ParamType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    StringVector,
    IntVector,
    DoubleVector
};

enum class ValidationRule {
    None,
    Min,
    Max,
    Range,
    Regex,
    NotEmpty,
    Positive,
    NonNegative,
    ExclusiveMin,
    ExclusiveMax
};

struct ParameterValidation {
    ValidationRule rule;
    std::variant<double, std::string> ruleValue;
    std::string description;
};

struct ParameterInfo {
    std::string key;
    std::string description;
    ParamType type;
    ConfigValue defaultValue;
    std::vector<ParameterValidation> validationRules;
    bool required;
    bool wasSet;
    
    ParameterInfo()
        : key(), description(), type(ParamType::String), required(false), wasSet(false) {}
    
    ParameterInfo(const std::string& k, const std::string& desc, ParamType t, 
                  const ConfigValue& def = ConfigValue(), bool req = false)
        : key(k), description(desc), type(t), defaultValue(def), required(req), wasSet(false) {}
    
    bool validate(const ConfigValue& value) const;
    std::string typeToString() const;
    static ParamType stringToType(const std::string& typeStr);
};

class Schema {
private:
    std::map<std::string, ParameterInfo> parameters;
    bool locked;
    
public:
    Schema() : locked(false) {}
    
    void addParameter(const ParameterInfo& param);
    bool hasParameter(const std::string& key) const;
    const ParameterInfo* getParameter(const std::string& key) const;
    const ParameterInfo& getParameterRequired(const std::string& key) const;
    
    std::vector<std::string> getAllKeys() const;
    std::vector<ParameterInfo> getAllParameters() const;
    
    void printSchema(std::ostream& os = std::cout) const;
    bool validate(const std::map<std::string, ConfigValue>& configData) const;
    
    void setParameterWasSet(const std::string& key);
    bool wasParameterSet(const std::string& key) const;
    
    void lock() { locked = true; }
    bool isLocked() const { return locked; }
    
    void clear();
};

} // namespace nlm