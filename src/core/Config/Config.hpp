#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Configuration error types
class ConfigError : public std::runtime_error {
public:
    enum class Type {
        InvalidKey,
        InvalidType,
        InvalidValue,
        ValidationFailed,
        FileError,
        ParseError,
        SerializationError
    };
    
    ConfigError(Type type, const std::string& message) 
        : std::runtime_error(message), error_type(type) {}
    
    Type getErrorType() const { return error_type; }
    
private:
    Type error_type;
};

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
    Runtime
};

// Validation callback type
using ValidationCallback = std::function<bool(const std::string&, const ConfigValue&, std::string&)>;

// Type descriptor for validation
struct ConfigType {
    std::string type_name;
    std::function<bool(const ConfigValue&)> type_checker;
    ValidationCallback custom_validator;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration metadata
struct ConfigMetadata {
    std::string description;
    std::string type_name;
    std::vector<std::string> allowed_values;
    std::pair<double, double> range;
    bool required;
    
    ConfigMetadata() : description(), type_name(), allowed_values(), range({0.0, 0.0}), required(false) {}
    
    ConfigMetadata(const std::string& desc, const std::string& type, bool req = false)
        : description(desc), type_name(type), required(req) {}
};

// Configuration metadata
struct ConfigMetadata {
    std::string description;
    std::string type_name;
    std::vector<std::string> allowed_values;
    std::pair<double, double> range;
    bool required;
    
    ConfigMetadata() : description(), type_name(), allowed_values(), range({0.0, 0.0}), required(false) {}
    
    ConfigMetadata(const std::string& desc, const std::string& type, bool req = false)
        : description(desc), type_name(type), required(req) {}
};

// Configuration template for presets
struct ConfigTemplate {
    std::string name;
    std::map<std::string, ConfigValue> values;
    std::string description;
};

} // namespace nlm
