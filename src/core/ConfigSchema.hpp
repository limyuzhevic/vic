#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <regex>

namespace nlm {

class ConfigSchema {
public:
    struct ValidationRule {
        enum class RuleType {
            Range,
            Pattern,
            Length,
            Custom,
            InList,
            RegularExpression,
            Callback
        };
        
        RuleType type;
        std::string parameter1;
        std::string parameter2;
        std::vector<std::string> allowedValues;
        std::function<bool(const std::any&)> validator;
        
        ValidationRule(RuleType t, const std::string& p1 = "", const std::string& p2 = "",
                       const std::vector<std::string>& allowed = {},
                       const std::function<bool(const std::any&)>& val = nullptr)
            : type(t), parameter1(p1), parameter2(p2), allowedValues(allowed), validator(val) {}
        
        bool validate(const std::any& value) const {
            if (validator) {
                return validator(value);
            }
            
            switch (type) {
                case RuleType::Range:
                    if (auto* d1 = std::any_cast<double>(&parameter1);
                        auto* d2 = std::any_cast<double>(&parameter2)) {
                        double val = std::any_cast<double>(value);
                        return val >= *d1 && val <= *d2;
                    }
                    return false;
                
                case RuleType::Pattern:
                    if (auto* pattern = std::any_cast<std::regex>(&parameter1)) {
                        std::string val = std::any_cast<std::string>(value);
                        return std::regex_match(val, *pattern);
                    }
                    return false;
                
                case RuleType::Length:
                    if (auto* len = std::any_cast<size_t>(&parameter1)) {
                        if (auto* s = std::any_cast<std::string>(&value)) {
                            return s->length() == *len;
                        } else if (auto* vec = std::any_cast<std::vector<std::any>>(&value)) {
                            return vec->size() == *len;
                        }
                    }
                    return false;
                
                case RuleType::InList:
                    if (auto* allowed = std::any_cast<std::vector<std::string>>(&allowedValues)) {
                        std::string val = std::any_cast<std::string>(value);
                        return std::find(allowed->begin(), allowed->end(), val) != allowed->end();
                    }
                    return false;
                
                case RuleType::RegularExpression:
                    if (auto* pattern = std::any_cast<std::regex>(&parameter1)) {
                        std::string val = std::any_cast<std::string>(value);
                        return std::regex_match(val, *pattern);
                    }
                    return false;
                
                case RuleType::Callback:
                    if (validator) {
                        return validator(value);
                    }
                    return false;
                
                default:
                    return true;
            }
        }
    };
    
    struct TypeConstraint {
        std::string key;
        std::string typeName;
        std::vector<ValidationRule> validationRules;
        std::any defaultValue;
        bool required;
        std::string description;
        std::string unit;
        std::vector<std::string> tags;
        
        TypeConstraint() : required(false) {}
        TypeConstraint(const std::string& k, const std::string& t,
                      const std::vector<ValidationRule>& rules = {},
                      const std::any& defaultVal = std::any(),
                      bool req = false, const std::string& desc = "",
                      const std::string& u = "", const std::vector<std::string>& tg = {})
            : key(k), typeName(t), validationRules(rules), defaultValue(defaultVal),
              required(req), description(desc), unit(u), tags(tg) {}
    };
    
    struct GroupConstraint {
        std::string groupName;
        std::string description;
        int priority;
        std::vector<std::string> parentGroups;
        bool isGlobal;
        
        GroupConstraint() : priority(100), isGlobal(false) {}
        GroupConstraint(const std::string& name, const std::string& desc = "", 
                       int prio = 100, bool global = false)
            : groupName(name), description(desc), priority(prio), isGlobal(global) {}
    };
    
    struct InheritanceRule {
        enum class RuleType {
            ExactMatch,
            Subset,
            Override,
            Merge
        };
        
        RuleType type;
        std::string sourceGroup;
        std::vector<std::string> excludeKeys;
        bool deepCopy;
        
        InheritanceRule(RuleType t, const std::string& source, 
                       const std::vector<std::string>& exclude = {},
                       bool deep = false)
            : type(t), sourceGroup(source), excludeKeys(exclude), deepCopy(deep) {}
    };
    
    struct SchemaOptions {
        bool allowUnknownKeys;
        bool validateTypes;
        bool validateRanges;
        bool validatePatterns;
        std::string keyPrefix;
        std::string description;
        
        SchemaOptions() : allowUnknownKeys(false), validateTypes(true), 
                         validateRanges(true), validatePatterns(true),
                         keyPrefix(""), description("") {}
    };
    
    std::map<std::string, TypeConstraint> constraints;
    std::map<std::string, GroupConstraint> groups;
    std::vector<InheritanceRule> inheritanceRules;
    SchemaOptions options;
    
    // Validation utilities
    static std::string getTypeName(const std::any& value);
    static bool isTypeCompatible(const std::string& expectedType, const std::any& value);
    static std::regex compilePattern(const std::string& pattern);
    
    // Schema building
    void addConstraint(const TypeConstraint& constraint);
    void addConstraint(const std::string& key, const std::string& typeName,
                      const std::vector<ValidationRule>& rules = {},
                      const std::any& defaultValue = std::any(),
                      bool required = false, const std::string& description = "",
                      const std::string& unit = "", const std::vector<std::string>& tags = {});
    
    void addGroup(const GroupConstraint& group);
    void addInheritanceRule(const InheritanceRule& rule);
    void setOptions(const SchemaOptions& opts);
    
    // Validation
    bool validate(const std::string& key, const std::any& value) const;
    bool validate(const std::string& key, const ConfigValue& value) const;
    bool validate(const std::string& key, const std::string& valueStr) const;
    bool validateAll(const std::vector<ConfigEntry>& entries) const;
    
    // Schema analysis
    std::vector<std::string> getRequiredKeys() const;
    std::vector<std::string> getOptionalKeys() const;
    std::vector<std::string> getKeysOfType(const std::string& typeName) const;
    std::vector<std::string> getKeysByTag(const std::string& tag) const;
    
    // Schema information
    std::string getDescription(const std::string& key) const;
    std::string getUnit(const std::string& key) const;
    std::vector<std::string> getTags(const std::string& key) const;
    std::any getDefaultValue(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    
    // Schema transformation
    std::string generateTemplate() const;
    bool isValidTemplate(const std::string& templateContent) const;
    
    // Constraint management
    TypeConstraint* getConstraint(const std::string& key);
    const TypeConstraint* getConstraint(const std::string& key) const;
    void removeConstraint(const std::string& key);
    bool hasConstraint(const std::string& key) const;
    
    // Group management
    GroupConstraint* getGroup(const std::string& groupName);
    const GroupConstraint* getGroup(const std::string& groupName) const;
    void removeGroup(const std::string& groupName);
    bool hasGroup(const std::string& groupName) const;
    
    // Inheritance management
    void clearInheritanceRules();
    size_t getInheritanceRuleCount() const;
    
    // Schema comparison
    bool equals(const ConfigSchema& other) const;
    
    // Schema serialization
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Validation helpers
    static ValidationRule rangeRule(double min, double max);
    static ValidationRule patternRule(const std::string& regexPattern);
    static ValidationRule lengthRule(size_t length);
    static ValidationRule inListRule(const std::vector<std::string>& values);
    static ValidationRule callbackRule(const std::function<bool(const std::any&)>& validator);
    
    // Type definitions for common types
    static void addIntConstraints(const std::string& key, int min = INT32_MIN, 
                                 int max = INT32_MAX, bool required = false,
                                 const std::string& description = "");
    static void addDoubleConstraints(const std::string& key, double min = -std::numeric_limits<double>::max(),
                                     double max = std::numeric_limits<double>::max(), bool required = false,
                                     const std::string& description = "");
    static void addStringConstraints(const std::string& key, size_t minLength = 0, size_t maxLength = SIZE_MAX,
                                     const std::string& pattern = "", bool required = false,
                                     const std::string& description = "");
    static void addBoolConstraints(const std::string& key, bool required = false,
                                   const std::string& description = "");
    
private:
    void initializeBuiltinTypes();
};

} // namespace nlm