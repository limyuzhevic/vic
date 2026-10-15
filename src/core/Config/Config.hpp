#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

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
    std::vector<std::string>,
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime
};

// Configuration validation rules
struct ValidationRule {
    std::string errorMessage;
    std::function<bool(const ConfigValue&)> validator;
    
    ValidationRule(const std::string& msg, std::function<bool(const ConfigValue&)> func)
        : errorMessage(msg), validator(func) {}
    
    bool validate(const ConfigValue& value) const {
        return validator(value);
    }
};

// Configuration metadata
struct ConfigMetadata {
    std::string description;
    std::vector<ValidationRule> validationRules;
    ConfigSource defaultSource;
    bool required;
    
    ConfigMetadata(const std::string& desc = "") 
        : description(desc), defaultSource(ConfigSource::Default), required(false) {}
    
    void addValidation(const std::string& errorMsg, std::function<bool(const ConfigValue&)> validator) {
        validationRules.emplace_back(errorMsg, validator);
    }
    
    bool validate(const ConfigValue& value) const {
        for (const auto& rule : validationRules) {
            if (!rule.validate(value)) {
                return false;
            }
        }
        return true;
    }
    
    std::string getError(const ConfigValue& value) const {
        for (const auto& rule : validationRules) {
            if (!rule.validate(value)) {
                return rule.errorMessage;
            }
        }
        return "";
    }
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    ConfigMetadata metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), metadata() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", ConfigSource defaultSrc = ConfigSource::Default)
        : key(k), value(v), source(s), metadata(desc, defaultSrc) {}
    
    bool validate() const {
        return metadata.validate(value);
    }
};