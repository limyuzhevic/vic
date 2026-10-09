#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace nlm {

// Configuration source
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
};

// Forward declarations
class Config;

// Configuration value types - using a type-safe approach instead of std::variant
class ConfigValue {
public:
    enum class Type {
        None = 0,
        Int = 1,
        Int64 = 2,
        Double = 3,
        Bool = 4,
        String = 5,
        StringVector = 6,
        DoubleVector = 7,
        IntVector = 8
    };

    // Constructors for different types
    ConfigValue() : type_(Type::None) {}
    ConfigValue(int value) : type_(Type::Int), intValue_(value) {}
    ConfigValue(int64_t value) : type_(Type::Int64), int64Value_(value) {}
    ConfigValue(double value) : type_(Type::Double), doubleValue_(value) {}
    ConfigValue(bool value) : type_(Type::Bool), boolValue_(value) {}
    ConfigValue(const std::string& value) : type_(Type::String), stringValue_(value) {}
    ConfigValue(const std::vector<std::string>& value) : type_(Type::StringVector), stringVectorValue_(value) {}
    ConfigValue(const std::vector<double>& value) : type_(Type::DoubleVector), doubleVectorValue_(value) {}
    ConfigValue(const std::vector<int>& value) : type_(Type::IntVector), intVectorValue_(value) {}

    // Type checking methods
    Type getType() const { return type_; }
    bool isNull() const { return type_ == Type::None; }
    bool isInt() const { return type_ == Type::Int; }
    bool isInt64() const { return type_ == Type::Int64; }
    bool isDouble() const { return type_ == Type::Double; }
    bool isBool() const { return type_ == Type::Bool; }
    bool isString() const { return type_ == Type::String; }
    bool isStringVector() const { return type_ == Type::StringVector; }
    bool isDoubleVector() const { return type_ == Type::DoubleVector; }
    bool isIntVector() const { return type_ == Type::IntVector; }

    // Value access methods with type safety
    int getInt() const {
        if (!isInt()) throw std::runtime_error("Value is not an integer");
        return intValue_;
    }
    int64_t getInt64() const {
        if (!isInt64()) throw std::runtime_error("Value is not an int64");
        return int64Value_;
    }
    double getDouble() const {
        if (!isDouble()) throw std::runtime_error("Value is not a double");
        return doubleValue_;
    }
    bool getBool() const {
        if (!isBool()) throw std::runtime_error("Value is not a boolean");
        return boolValue_;
    }
    const std::string& getString() const {
        if (!isString()) throw std::runtime_error("Value is not a string");
        return stringValue_;
    }
    const std::vector<std::string>& getStringVector() const {
        if (!isStringVector()) throw std::runtime_error("Value is not a string vector");
        return stringVectorValue_;
    }
    const std::vector<double>& getDoubleVector() const {
        if (!isDoubleVector()) throw std::runtime_error("Value is not a double vector");
        return doubleVectorValue_;
    }
    const std::vector<int>& getIntVector() const {
        if (!isIntVector()) throw std::runtime_error("Value is not an int vector");
        return intVectorValue_;
    }

    // Type conversion methods
    int asInt() const { return getInt(); }
    int64_t asInt64() const { return getInt64(); }
    double asDouble() const { return getDouble(); }
    bool asBool() const { return getBool(); }
    const std::string& asString() const { return getString(); }
    const std::vector<std::string>& asStringVector() const { return getStringVector(); }
    const std::vector<double>& asDoubleVector() const { return getDoubleVector(); }
    const std::vector<int>& asIntVector() const { return getIntVector(); }

    // Assignment operators
    ConfigValue& operator=(int value) {
        type_ = Type::Int;
        intValue_ = value;
        return *this;
    }
    ConfigValue& operator=(int64_t value) {
        type_ = Type::Int64;
        int64Value_ = value;
        return *this;
    }
    ConfigValue& operator=(double value) {
        type_ = Type::Double;
        doubleValue_ = value;
        return *this;
    }
    ConfigValue& operator=(bool value) {
        type_ = Type::Bool;
        boolValue_ = value;
        return *this;
    }
    ConfigValue& operator=(const std::string& value) {
        type_ = Type::String;
        stringValue_ = value;
        return *this;
    }
    ConfigValue& operator=(const std::vector<std::string>& value) {
        type_ = Type::StringVector;
        stringVectorValue_ = value;
        return *this;
    }
    ConfigValue& operator=(const std::vector<double>& value) {
        type_ = Type::DoubleVector;
        doubleVectorValue_ = value;
        return *this;
    }
    ConfigValue& operator=(const std::vector<int>& value) {
        type_ = Type::IntVector;
        intVectorValue_ = value;
        return *this;
    }

    // JSON serialization
    nlohmann::json toJson() const {
        switch (type_) {
            case Type::None: return nullptr;
            case Type::Int: return intValue_;
            case Type::Int64: return int64Value_;
            case Type::Double: return doubleValue_;
            case Type::Bool: return boolValue_;
            case Type::String: return stringValue_;
            case Type::StringVector: return stringVectorValue_;
            case Type::DoubleVector: return doubleVectorValue_;
            case Type::IntVector: return intVectorValue_;
            default: return nullptr;
        }
    }

    // JSON deserialization
    static ConfigValue fromJson(const nlohmann::json& json) {
        ConfigValue result;
        
        if (json.is_null()) {
            result.type_ = Type::None;
        } else if (json.is_number_integer()) {
            // Try to fit into available types
            int64_t intVal = json.get<int64_t>();
            if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                result.type_ = Type::Int;
                result.intValue_ = static_cast<int>(intVal);
            } else {
                result.type_ = Type::Int64;
                result.int64Value_ = intVal;
            }
        } else if (json.is_number_float()) {
            result.type_ = Type::Double;
            result.doubleValue_ = json.get<double>();
        } else if (json.is_boolean()) {
            result.type_ = Type::Bool;
            result.boolValue_ = json.get<bool>();
        } else if (json.is_string()) {
            result.type_ = Type::String;
            result.stringValue_ = json.get<std::string>();
        } else if (json.is_array()) {
            // Try to determine array type
            bool allStrings = true;
            bool allDoubles = true;
            bool allInts = true;
            
            for (const auto& item : json) {
                if (!item.is_string()) allStrings = false;
                if (!item.is_number()) allDoubles = false;
                if (!item.is_number_integer()) allInts = false;
            }
            
            if (allStrings && !json.empty()) {
                result.type_ = Type::StringVector;
                result.stringVectorValue_ = json.get<std::vector<std::string>>();
            } else if (allInts && !json.empty()) {
                result.type_ = Type::IntVector;
                result.intVectorValue_ = json.get<std::vector<int>>();
            } else if (!allStrings && !allInts && !allDoubles) {
                // Mixed array - convert to string vector as fallback
                result.type_ = Type::StringVector;
                std::vector<std::string> strVec;
                for (const auto& item : json) {
                    strVec.push_back(item.dump());
                }
                result.stringVectorValue_ = strVec;
            } else {
                result.type_ = Type::DoubleVector;
                result.doubleVectorValue_ = json.get<std::vector<double>>();
            }
        } else {
            result.type_ = Type::None;
        }
        
        return result;
    }

    // String representation
    std::string toString() const {
        switch (type_) {
            case Type::None: return "null";
            case Type::Int: return std::to_string(intValue_);
            case Type::Int64: return std::to_string(int64Value_);
            case Type::Double: return std::to_string(doubleValue_);
            case Type::Bool: return boolValue_ ? "true" : "false";
            case Type::String: return "\"" + stringValue_ + "\"";
            case Type::StringVector:
            case Type::DoubleVector:
            case Type::IntVector: {
                std::ostringstream oss;
                oss << "[";
                bool first = true;
                visit([&oss, &first](const auto& value) {
                    if (!first) oss << ", ";
                    oss << value;
                    first = false;
                });
                oss << "]";
                return oss.str();
            }
            default: return "unknown";
        }
    }

    // Comparison operators
    bool operator==(const ConfigValue& other) const {
        if (type_ != other.type_) return false;
        
        switch (type_) {
            case Type::None: return true;
            case Type::Int: return intValue_ == other.intValue_;
            case Type::Int64: return int64Value_ == other.int64Value_;
            case Type::Double: return doubleValue_ == other.doubleValue_;
            case Type::Bool: return boolValue_ == other.boolValue_;
            case Type::String: return stringValue_ == other.stringValue_;
            case Type::StringVector: return stringVectorValue_ == other.stringVectorValue_;
            case Type::DoubleVector: return doubleVectorValue_ == other.doubleVectorValue_;
            case Type::IntVector: return intVectorValue_ == other.intVectorValue_;
            default: return false;
        }
    }

    bool operator!=(const ConfigValue& other) const {
        return !(*this == other);
    }

    // Helper for visitor pattern
    template<typename TVisitor>
    void visit(const TVisitor& visitor) const {
        switch (type_) {
            case Type::None: break;
            case Type::Int: visitor(intValue_); break;
            case Type::Int64: visitor(int64Value_); break;
            case Type::Double: visitor(doubleValue_); break;
            case Type::Bool: visitor(boolValue_); break;
            case Type::String: visitor(stringValue_); break;
            case Type::StringVector: visitor(stringVectorValue_); break;
            case Type::DoubleVector: visitor(doubleVectorValue_); break;
            case Type::IntVector: visitor(intVectorValue_); break;
        }
    }

private:
    Type type_ = Type::None;
    
    // Storage for different types
    int intValue_ = 0;
    int64_t int64Value_ = 0;
    double doubleValue_ = 0.0;
    bool boolValue_ = false;
    std::string stringValue_;
    std::vector<std::string> stringVectorValue_;
    std::vector<double> doubleVectorValue_;
    std::vector<int> intVectorValue_;
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
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type inference
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters with automatic type detection
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters from JSON
    void set(const std::string& key, const nlohmann::json& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Default value setting
    void setDefaults();
    
    // Value validation
    bool validate(const std::string& key) const;
    
    // File path resolution
    static std::string resolveFilePath(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringEndsWith(const std::string& str, const std::string& suffix);
    static std::string extractExtension(const std::string& filename);
    
    // Parse JSON file
    bool loadJsonFromFile(const std::string& filepath);
    
    // Parse simple key=value file
    bool loadSimpleFromFile(const std::string& filepath);
    
    // Save as JSON
    bool saveAsJson(const std::string& filepath) const;
    
    // Save as simple format
    bool saveAsSimple(const std::string& filepath) const;
    
    // Parse command line arguments
    void parseArgs(int argc, char** argv);
    
    // Parse key=value format
    void parseKeyValue(const std::string& arg, ConfigSource source);
    
    // Parse --key=value format
    void parseEqualsFormat(const std::string& arg, ConfigSource source);
    
    // Parse -key value format
    void parseShortFormat(const std::string& key, const std::string& value, ConfigSource source);
    
    // Validate key format
    bool isValidKey(const std::string& key) const;
    
    // Validate value format
    bool isValidValue(const std::string& value) const;
    
    // Convert string to appropriate type
    ConfigValue stringToValue(const std::string& str) const;
    
    // Convert value to string
    std::string valueToString(const ConfigValue& value) const;
};

// Helper function to create a ConfigValue from various types
inline ConfigValue makeConfigValue(int value) { return ConfigValue(value); }
inline ConfigValue makeConfigValue(int64_t value) { return ConfigValue(value); }
inline ConfigValue makeConfigValue(double value) { return ConfigValue(value); }
inline ConfigValue makeConfigValue(bool value) { return ConfigValue(value); }
inline ConfigValue makeConfigValue(const std::string& value) { return ConfigValue(value); }
inline ConfigValue makeConfigValue(const std::vector<std::string>& value) { return ConfigValue(value); }
inline ConfigValue makeConfigValue(const std::vector<double>& value) { return ConfigValue(value); }
inline ConfigValue makeConfigValue(const std::vector<int>& value) { return ConfigValue(value); }

// Explicit template instantiations for common types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Add nlohmann/json dependency to CMakeLists.txt
// find_package(nlohmann_json REQUIRED)
// target_link_libraries(nlm_core PUBLIC nlohmann_json::nlohmann_json)
