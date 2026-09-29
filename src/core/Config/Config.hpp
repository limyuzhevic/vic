#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <regex>

namespace nlm {

// Type comparison template
namespace {
template<typename T>
struct IsVector : std::false_type {};

template<typename U>
struct IsVector<std::vector<U>> : std::true_type {};

template<typename T>
struct IsNumericType : std::is_same<T, int> {};
template<> struct IsNumericType<double> : std::true_type {};
template<> struct IsNumericType<int64_t> : std::true_type {};
template<> struct IsNumericType<uint64_t> : std::true_type {};

// Helper functions for Config
inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline bool stringMatchesRegex(const std::string& str, const std::string& regexPattern) {
    if (regexPattern.empty()) return true;
    try {
        std::regex pattern(regexPattern);
        return std::regex_match(str, pattern);
    } catch (const std::regex_error&) {
        return true;
    }
}

inline std::string formatValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_arithmetic_v<T>) {
            oss << arg;
        } else {
            oss << "<" << typeid(T).name() << ">";
        }
    }, value);
    return oss.str();
}

inline bool parseSimpleFormat(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) return false;
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    return !key.empty();
}

inline std::string serializeValue(const ConfigValue& value) {
    return formatValue(value);
}

inline ConfigValue deserializeValue(const std::string& str, const std::string& typeHint = "") {
    if (typeHint == "int" || typeHint == "integer") {
        try { return std::stoi(str); } catch (...) { return std::string(str); }
    } else if (typeHint == "double" || typeHint == "float") {
        try { return std::stod(str); } catch (...) { return std::string(str); }
    } else if (typeHint == "bool" || typeHint == "boolean") {
        std::string lower = toLower(str);
        if (lower == "true" || lower == "yes" || lower == "1") return true;
        if (lower == "false" || lower == "no" || lower == "0") return false;
        return std::string(str);
    } else if (typeHint == "string") {
        return str;
    } else if (typeHint == "array" || typeHint == "vector") {
        if (str.size() >= 2 && str.front() == '[' && str.back() == ']') {
            return std::vector<double>{};
        }
        return std::string(str);
    }
    return std::string(str);
}

inline bool detectFileFormat(const std::string& filepath, std::string& format) {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json" || ext == "cfg" || ext == "config") {
        format = "json";
        return true;
    } else if (ext == "yaml" || ext == "yml") {
        format = "yaml";
        return true;
    }
    format = "auto";
    return false;
}

} // anonymous namespace

// Enhanced configuration value types with more support
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::vector<std::vector<double>>
>;

// Enhanced configuration source enum
enum class ConfigSource {
    Default,
    File,
    FileJSON,
    FileYAML,
    CommandLine,
    Environment,
    Runtime,
    Programmatic
};

// Enhanced configuration entry with better metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    bool required;
    std::string validationRegex;
    std::string minValue;
    std::string maxValue;
    std::string defaultValue;
    mutable uint32_t accessCount;
    std::chrono::system_clock::time_point lastAccess;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    category("general"), required(false), validationRegex(), 
                    minValue(), maxValue(), defaultValue(), accessCount(0),
                    lastAccess(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "general",
                bool req = false, const std::string& regex = "",
                const std::string& min = "", const std::string& max = "",
                const std::string& def = "")
        : key(k), value(v), source(s), description(desc), category(cat), 
          required(req), validationRegex(regex), minValue(min), maxValue(max),
          defaultValue(def), accessCount(0),
          lastAccess(std::chrono::system_clock::now()) {}
    
    bool isNumeric() const;
    bool isBoolean() const;
    bool isString() const;
    bool isArray() const;
    size_t getArraySize() const;
    template<typename T>
    bool isType() const;
    
    bool validateValue(const ConfigValue& val) const;
    std::string getValueTypeString() const;
    void updateAccess() const;
    uint32_t getAccessCount() const { return accessCount; }
    std::chrono::system_clock::time_point getLastAccess() const { return lastAccess; }
};

// Configuration schema for validation
class ConfigSchema {
public:
    struct ValidationRule {
        enum class RuleType {
            Range,
            Regex,
            Length,
            Required,
            Custom
        };
        
        RuleType type;
        std::string parameter1;
        std::string parameter2;
        std::function<bool(const ConfigValue&)> customValidator;
        std::string errorMessage;
    };
    
    ConfigSchema();
    ~ConfigSchema();
    
    void addValidation(const std::string& key, RuleType type, 
                      const std::string& param1 = "", 
                      const std::string& param2 = "",
                      const std::function<bool(const ConfigValue&)>& customValidator = nullptr,
                      const std::string& errorMsg = "");
    
    std::vector<std::string> validateEntry(const ConfigEntry& entry) const;
    std::vector<ValidationRule> getValidationRules(const std::string& key) const;
    void clear();
    bool hasRules(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    bool saveToJSON(const std::string& filepath, bool prettyPrint = true) const;
    bool saveToYAML(const std::string& filepath) const;
    
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    bool loadFromArgs(int argc, char** argv);
    
    void setProgrammatic(const std::string& key, const ConfigValue& value,
                         const std::string& description = "",
                         const std::string& category = "general");
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    std::string getAsString(const std::string& key) const;
    
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "general",
             bool validate = true);
    
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "general",
             bool validate = true);
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "general",
             bool validate = true);
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "general",
             bool validate = true);
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "general",
             bool validate = true);
    
    bool has(const std::string& key) const;
    bool isNumeric(const std::string& key) const;
    bool isString(const std::string& key) const;
    bool isBoolean(const std::string& key) const;
    bool isArray(const std::string& key) const;
    
    std::string getDescription(const std::string& key) const;
    std::string getCategory(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    uint32_t getAccessCount(const std::string& key) const;
    
    void remove(const std::string& key);
    void resetToDefaults();
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    void clear();
    
    std::string summary() const;
    std::string summaryByCategory() const;
    std::string getUsageStatistics() const;
    
    std::string diff(const Config& other) const;
    bool equals(const Config& other) const;
    
    void setSchema(std::shared_ptr<ConfigSchema> schema);
    std::shared_ptr<ConfigSchema> getSchema() const;
    std::vector<std::string> validateAll() const;
    
    void setBatch(const std::vector<std::pair<std::string, ConfigValue>>& values,
                  ConfigSource source = ConfigSource::Runtime);
    
    bool importFromString(const std::string& data, const std::string& format = "auto");
    std::string exportToString(const std::string& format = "json", 
                               bool prettyPrint = false) const;
    
    void optimizeMemoryLayout();
    size_t getMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Type-safe getters
namespace ConfigGetters {
    template<typename T>
    T getTyped(const Config& config, const std::string& key, const T& defaultValue);
    
    inline int getInt(const Config& config, const std::string& key, int defaultValue = 0) {
        return config.getOr<int>(key, defaultValue);
    }
    
    inline double getDouble(const Config& config, const std::string& key, double defaultValue = 0.0) {
        return config.getOr<double>(key, defaultValue);
    }
    
    inline bool getBool(const Config& config, const std::string& key, bool defaultValue = false) {
        return config.getOr<bool>(key, defaultValue);
    }
    
    inline std::string getString(const Config& config, const std::string& key, const std::string& defaultValue = "") {
        return config.getOr<std::string>(key, defaultValue);
    }
    
    inline std::vector<double> getVector(const Config& config, const std::string& key) {
        auto val = config.get<std::vector<double>>(key);
        return val.has_value() ? val.value() : std::vector<double>();
    }
}

} // namespace nlm
