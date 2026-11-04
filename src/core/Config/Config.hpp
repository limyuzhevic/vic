#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <functional>
#include <regex>
#include <type_traits>
#include <sstream>
#include <cmath>
#include <limits>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    EnvironmentVariable,
    JSON,
    YAML
};

// Validation rule types
enum class ValidationRule {
    Range,
    AllowedValues,
    Regex,
    Custom,
    Positive,
    NonNegative,
    LessThan,
    GreaterThan,
    InRange,
    Length,
    Pattern,
    Format,
    Callback
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::string unit;
    std::vector<std::pair<ValidationRule, std::any>> validationRules;
    bool isRequired;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   type(), unit(), validationRules(), isRequired(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "", 
                const std::string& u = "")
        : key(k), value(v), source(s), description(desc), type(t), unit(u), 
          validationRules(), isRequired(false) {}
};

// Environment variable mapping
struct EnvMapping {
    std::string configKey;
    std::string envVarName;
    std::string defaultValue;
    bool isRequired;
    
    EnvMapping(const std::string& key, const std::string& env, 
               const std::string& def = "", bool req = false)
        : configKey(key), envVarName(env), defaultValue(def), isRequired(req) {}
};

// Validation rule for configuration values
struct ValidationRuleConfig {
    ValidationRule rule;
    std::any value1;
    std::any value2;
    std::string message;
    std::function<bool(const ConfigValue&)> customValidator;
    
    ValidationRuleConfig(ValidationRule r, const std::any& v1 = {}, 
                         const std::any& v2 = {}, const std::string& msg = "")
        : rule(r), value1(v1), value2(v2), message(msg), customValidator(nullptr) {}
};

// Enhanced configuration implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> descriptions;
    std::vector<EnvMapping> envMappings;
    std::map<std::string, std::vector<ValidationRuleConfig>> validationRules;
    bool validateOnSet;
    bool allowUnknownKeys;
    bool isLoaded;
    std::string lastError;
    
    Impl() : descriptions(), envMappings(), validationRules(), 
             validateOnSet(true), allowUnknownKeys(false), 
             isLoaded(false), lastError() {}
};
