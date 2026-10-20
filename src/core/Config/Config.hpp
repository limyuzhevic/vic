#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>
#include <chrono>
#include <numeric>
#include <chrono>

namespace nlohmann {
    class json {
    public:
        using object_t = std::unordered_map<std::string, json>;
        using array_t = std::vector<json>;
        
        enum class value_t { null, object, array, string, boolean, number_integer, number_unsigned, number_float };
        
        json() : type_(value_t::null), value_(json_value_t{}) {}
        json(bool val) : type_(value_t::boolean), value_(val) {}
        json(double val) : type_(value_t::number_float), value_(val) {}
        json(int val) : type_(value_t::number_integer), value_(val) {}
        json(int64_t val) : type_(value_t::number_unsigned), value_(val) {}
        json(const std::string& val) : type_(value_t::string), value_(val) {}
        json(const char* val) : json(std::string(val)) {}
        json(const object_t& val) : type_(value_t::object), value_(val) {}
        json(const array_t& val) : type_(value_t::array), value_(val) {}
        
        value_t type() const { return type_; }
        
        bool is_object() const { return type_ == value_t::object; }
        bool is_array() const { return type_ == value_t::array; }
        bool is_string() const { return type_ == value_t::string; }
        bool is_boolean() const { return type_ == value_t::boolean; }
        bool is_number() const { return type_ == value_t::number_integer || type_ == value_t::number_unsigned || type_ == value_t::number_float; }
        bool is_null() const { return type_ == value_t::null; }
        
        const object_t& object() const { return std::get<object_t>(value_); }
        const array_t& array() const { return std::get<array_t>(value_); }
        const std::string& string() const { return std::get<std::string>(value_); }
        bool boolean() const { return std::get<bool>(value_); }
        int integer() const { return std::get<int>(value_); }
        int64_t unsigned_integer() const { return std::get<int64_t>(value_); }
        double number() const { return std::get<double>(value_); }
        
        bool contains(const std::string& key) const {
            return is_object() && object().find(key) != object().end();
        }
        
        const json& operator[](const std::string& key) const {
            static const json null_json;
            if (is_object() && contains(key)) {
                return object().at(key);
            }
            return null_json;
        }
        
        // JSON parsing from string
        static json parse(const std::string& jsonStr) {
            json result;
            parseInternal(jsonStr, result, 0);
            return result;
        }
        
        // JSON serialization to string
        std::string dump(int indent = -1) const {
            if (indent >= 0) {
                return dumpPretty(indent);
            }
            return dumpCompact();
        }
        
    private:
        value_t type_;
        std::variant<bool, double, int, int64_t, std::string, object_t, array_t> value_;
        
        void parseInternal(const std::string& str, json& result, size_t& pos);
        std::string dumpCompact() const;
        std::string dumpPretty(int indent) const;
    };
}

// Use nlohmann namespace for compatibility
using json = nlohmann::json;

namespace nlm {

// Configuration system exceptions
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& msg, const std::string& file = "", int line = 0)
        : ConfigError(msg), file_(file), line_(line) {}
    
    std::string what() const noexcept override {
        std::string msg = std::runtime_error::what();
        if (!file_.empty()) {
            msg += " (file: " + file_ + "";
            if (line_ > 0) msg += ", line: " + std::to_string(line_);
            msg += ")";
        }
        return msg;
    }
    
private:
    std::string file_;
    int line_;
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& msg) : ConfigError(msg) {}
};

class ConfigTypeError : public ConfigError {
public:
    explicit ConfigTypeError(const std::string& msg) : ConfigError(msg) {}
};

// Configuration value types with enhanced support
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

// Nested configuration support
struct ConfigSection {
    std::string name;
    std::unordered_map<std::string, ConfigValue> values;
    std::string description;
    std::map<std::string, std::string> metadata;
    
    ConfigSection() : name(), description() {}
    ConfigSection(const std::string& n) : name(n), description() {}
};

using ConfigGroups = std::unordered_map<std::string, ConfigSection>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    JSON,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::string type;
    bool required;
    std::string validationRegex;
    double minValue;
    double maxValue;
    std::vector<std::string> allowedValues;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category("general"),
                   type("auto"), required(false), validationRegex(), minValue(std::numeric_limits<double>::lowest()),
                   maxValue(std::numeric_limits<double>::max()), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), category("general"), type("auto"), required(false),
          validationRegex(), minValue(std::numeric_limits<double>::lowest()),
          maxValue(std::numeric_limits<double>::max()), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::string name;
    std::string version;
    std::string description;
    std::unordered_map<std::string, std::string> typeHints;
    std::vector<std::string> requiredKeys;
    std::unordered_map<std::string, std::string> defaultValues;
    std::unordered_map<std::string, std::string> categories;
    std::unordered_map<std::string, std::string> descriptions;
    
    ConfigSchema() : name(), version("1.0"), description() {}
};

// Main enhanced configuration class
class Config {
public:
    Config();
    explicit Config(const ConfigSchema& schema);
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading
    bool loadFromFile(const std::string& filepath, bool autoDetect = true);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromSimple(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool pretty = false) const;
    bool saveToJSON(const std::string& filepath, bool pretty = false) const;
    bool saveToSimple(const std::string& filepath) const;
    
    // Get values with type support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced type-specific getters
    std::optional<int> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<std::vector<int>> getIntVector(const std::string& key) const;
    std::optional<std::vector<double>> getDoubleVector(const std::string& key) const;
    std::optional<std::vector<std::string>> getStringVector(const std::string& key) const;
    std::optional<std::vector<bool>> getBoolVector(const std::string& key) const;
    
    // Enhanced set methods with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration groups/categories
    void setCategory(const std::string& key, const std::string& category);
    void setDescription(const std::string& key, const std::string& description);
    void setType(const std::string& key, const std::string& type);
    void setDefault(const std::string& key, const ConfigValue& value);
    void setRequired(const std::string& key, bool required);
    void setValidation(const std::string& key, const std::string& regex, double min, double max);
    
    // Check existence and metadata
    bool has(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    std::string getCategory(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    std::string getType(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysInCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summaryInCategory(const std::string& category) const;
    
    // Validation and schema
    bool validate() const;
    std::string validateKey(const std::string& key) const;
    void setSchema(const ConfigSchema& schema);
    
    // Default values system
    void applyDefaults();
    bool hasDefault(const std::string& key) const;
    ConfigValue getDefault(const std::string& key) const;
    
    // Copy and import/export
    void copyFrom(const Config& other, ConfigSource source = ConfigSource::Runtime);
    bool mergeFromFile(const std::string& filepath, bool overwrite = true);
    
    // Convenience methods for common tasks
    void setFromEnvironment(const std::string& prefix = "NLM_");
    bool saveToEnv(const std::string& prefix = "NLM_") const;
    
    // Serialization helpers
    std::string toJSON(bool pretty = false) const;
    bool fromJSON(const std::string& jsonStr);
    std::string toSimple(bool comments = true) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractKeyFromArg(const std::string& arg);
    static std::string extractValueFromArg(const std::string& arg);
    static bool isCommentOrEmpty(const std::string& line);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string toString(const ConfigValue& value);
    static ConfigValue fromString(const std::string& str, const std::string& type = "auto");
    static bool validateValue(const ConfigValue& value, const std::string& type, double min, double max);
    static std::string getTypeFromValue(const ConfigValue& value);
};

} // namespace nlm
