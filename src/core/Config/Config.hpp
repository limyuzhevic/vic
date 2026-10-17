#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
    Runtime
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

// Main configuration class
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <sstream>

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
    Runtime
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

// Configuration format enum
enum class ConfigFormat {
    Unknown,
    SimpleKeyValue,
    JSON,
    YAML
};

// Configuration error types
class ConfigError : public std::runtime_error {
public:
    enum class Type {
        FileNotFound,
        ParseError,
        FormatError,
        ValidationError,
        IoError
    };
    
    ConfigError(Type type, const std::string& message) 
        : std::runtime_error(message), type_(type) {}
    
    Type type() const { return type_; }
    
private:
    Type type_;
};

// Simple JSON implementation (since nlohmann/json is not available)
namespace json {
    class JsonValue;
    using JsonObject = std::map<std::string, JsonValue>;
    using JsonArray = std::vector<JsonValue>;
    
    class JsonValue {
    public:
        enum class Type { Null, Boolean, Number, String, Object, Array };
        
        JsonValue() : type_(Type::Null) {}
        JsonValue(bool b) : type_(Type::Boolean), bool_value_(b) {}
        JsonValue(double n) : type_(Type::Number), double_value_(n) {}
        JsonValue(const std::string& s) : type_(Type::String), string_value_(s) {}
        JsonValue(const JsonObject& o) : type_(Type::Object), object_value_(o) {}
        JsonValue(const JsonArray& a) : type_(Type::Array), array_value_(a) {}
        
        Type type() const { return type_; }
        
        bool isBoolean() const { return type_ == Type::Boolean; }
        bool isNumber() const { return type_ == Type::Number; }
        bool isString() const { return type_ == Type::String; }
        bool isObject() const { return type_ == Type::Object; }
        bool isArray() const { return type_ == Type::Array; }
        
        bool getBool() const { return bool_value_; }
        double getNumber() const { return double_value_; }
        const std::string& getString() const { return string_value_; }
        const JsonObject& getObject() const { return object_value_; }
        const JsonArray& getArray() const { return array_value_; }
        
    private:
        Type type_;
        union {
            bool bool_value_;
            double double_value_;
            std::string string_value_;
            JsonObject object_value_;
            JsonArray array_value_;
        };
    };
    
    // Simple JSON parsing from string
    JsonValue parse(const std::string& jsonStr);
    std::string toString(const JsonValue& value, int indent = 0);
    
    // JSON helper functions
    bool isValidJSON(const std::string& str);
    std::string escapeString(const std::string& str);
}

// Simple YAML implementation
namespace yaml {
    class YamlNode;
    using YamlMapping = std::map<std::string, YamlNode>;
    using YamlSequence = std::vector<YamlNode>;
    
    class YamlNode {
    public:
        enum class Type { Null, Scalar, Mapping, Sequence };
        
        YamlNode() : type_(Type::Null) {}
        YamlNode(const std::string& scalar) : type_(Type::Scalar), scalar_value_(scalar) {}
        YamlNode(const YamlMapping& mapping) : type_(Type::Mapping), mapping_value_(mapping) {}
        YamlNode(const YamlSequence& sequence) : type_(Type::Sequence), sequence_value_(sequence) {}
        
        Type type() const { return type_; }
        
        bool isScalar() const { return type_ == Type::Scalar; }
        bool isMapping() const { return type_ == Type::Mapping; }
        bool isSequence() const { return type_ == Type::Sequence; }
        
        const std::string& getScalar() const { return scalar_value_; }
        const YamlMapping& getMapping() const { return mapping_value_; }
        const YamlSequence& getSequence() const { return sequence_value_; }
        
    private:
        Type type_;
        union {
            std::string scalar_value_;
            YamlMapping mapping_value_;
            YamlSequence sequence_value_;
        };
    };
    
    // Simple YAML parsing from string
    YamlNode parse(const std::string& yamlStr);
    std::string toString(const YamlNode& node, int indent = 0);
    
    // YAML helper functions
    bool isValidYAML(const std::string& str);
    std::string trim(const std::string& str);
}

// Simple key-value parser (for backward compatibility)
namespace keyvalue {
    std::map<std::string, std::string> parse(const std::string& content);
    std::string toString(const std::map<std::string, std::string>& kvp);
}

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (auto-detects format: JSON, YAML, or key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string content with specified format
    bool loadFromString(const std::string& content, ConfigFormat format);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (outputs JSON by default)
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::JSON) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration format
    ConfigFormat getFormat() const { return format_; }
    
    // Set default values for critical parameters
    void setCriticalDefaults();
    
    // Validate configuration
    bool validate() const;
    
    // Convert between formats
    std::string toJsonString() const;
    std::string toYamlString() const;
    std::map<std::string, std::string> toKeyValueMap() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigFormat format_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigFormat detectFormat(const std::string& filepath);
    static bool isTextFile(const std::string& filepath);
    
    // Conversion helpers
    void fromJson(const json::JsonValue& json);
    void fromYaml(const yaml::YamlNode& yaml);
    json::JsonValue toJson() const;
    yaml::YamlNode toYaml() const;
    
    // Private constructor for factory methods
    Config(std::unique_ptr<Impl> impl) : pImpl(std::move(impl)), format_(ConfigFormat::Unknown) {}
};

} // namespace nlm
