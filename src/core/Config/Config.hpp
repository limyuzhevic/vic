#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_map>

namespace nlm {

// Configuration value types - Type-safe enum classes to avoid std::variant issues
class ConfigString {
public:
    ConfigString() = default;
    explicit ConfigString(const std::string& value) : value_(value) {}
    const std::string& get() const { return value_; }
    void set(const std::string& value) { value_ = value; }
private:
    std::string value_;
};

class ConfigInt {
public:
    ConfigInt() = default;
    explicit ConfigInt(int64_t value) : value_(value) {}
    int64_t get() const { return value_; }
    void set(int64_t value) { value_ = value; }
private:
    int64_t value_;
};

class ConfigDouble {
public:
    ConfigDouble() = default;
    explicit ConfigDouble(double value) : value_(value) {}
    double get() const { return value_; }
    void set(double value) { value_ = value; }
private:
    double value_;
};

class ConfigBool {
public:
    ConfigBool() = default;
    explicit ConfigBool(bool value) : value_(value) {}
    bool get() const { return value_; }
    void set(bool value) { value_ = value; }
private:
    bool value_;
};

class ConfigVectorInt {
public:
    ConfigVectorInt() = default;
    explicit ConfigVectorInt(const std::vector<int64_t>& value) : value_(value) {}
    const std::vector<int64_t>& get() const { return value_; }
    void set(const std::vector<int64_t>& value) { value_ = value; }
private:
    std::vector<int64_t> value_;
};

class ConfigVectorDouble {
public:
    ConfigVectorDouble() = default;
    explicit ConfigVectorDouble(const std::vector<double>& value) : value_(value) {}
    const std::vector<double>& get() const { return value_; }
    void set(const std::vector<double>& value) { value_ = value; }
private:
    std::vector<double> value_;
};

class ConfigVectorString {
public:
    ConfigVectorString() = default;
    explicit ConfigVectorString(const std::vector<std::string>& value) : value_(value) {}
    const std::vector<std::string>& get() const { return value_; }
    void set(const std::vector<std::string>& value) { value_ = value; }
private:
    std::vector<std::string> value_;
};

// Union type for configuration values
class ConfigValue {
public:
    enum class Type {
        None,
        String,
        Int,
        Double,
        Bool,
        VectorInt,
        VectorDouble,
        VectorString
    };

    ConfigValue() : type_(Type::None) {}
    ConfigValue(const std::string& value) : type_(Type::String) { new (&storage_.string) ConfigString(value); }
    ConfigValue(int64_t value) : type_(Type::Int) { new (&storage_.intVal) ConfigInt(value); }
    ConfigValue(double value) : type_(Type::Double) { new (&storage_.doubleVal) ConfigDouble(value); }
    ConfigValue(bool value) : type_(Type::Bool) { new (&storage_.boolVal) ConfigBool(value); }
    ConfigValue(const std::vector<int64_t>& value) : type_(Type::VectorInt) { new (&storage_.vectorInt) ConfigVectorInt(value); }
    ConfigValue(const std::vector<double>& value) : type_(Type::VectorDouble) { new (&storage_.vectorDouble) ConfigVectorDouble(value); }
    ConfigValue(const std::vector<std::string>& value) : type_(Type::VectorString) { new (&storage_.vectorString) ConfigVectorString(value); }

    ~ConfigValue() {
        switch (type_) {
            case Type::String: storage_.string.~ConfigString(); break;
            case Type::Int: storage_.intVal.~ConfigInt(); break;
            case Type::Double: storage_.doubleVal.~ConfigDouble(); break;
            case Type::Bool: storage_.boolVal.~ConfigBool(); break;
            case Type::VectorInt: storage_.vectorInt.~ConfigVectorInt(); break;
            case Type::VectorDouble: storage_.vectorDouble.~ConfigVectorDouble(); break;
            case Type::VectorString: storage_.vectorString.~ConfigVectorString(); break;
            default: break;
        }
    }

    ConfigValue(const ConfigValue& other) : type_(other.type_) {
        switch (type_) {
            case Type::String: new (&storage_.string) ConfigString(other.storage_.string); break;
            case Type::Int: new (&storage_.intVal) ConfigInt(other.storage_.intVal); break;
            case Type::Double: new (&storage_.doubleVal) ConfigDouble(other.storage_.doubleVal); break;
            case Type::Bool: new (&storage_.boolVal) ConfigBool(other.storage_.boolVal); break;
            case Type::VectorInt: new (&storage_.vectorInt) ConfigVectorInt(other.storage_.vectorInt); break;
            case Type::VectorDouble: new (&storage_.vectorDouble) ConfigVectorDouble(other.storage_.vectorDouble); break;
            case Type::VectorString: new (&storage_.vectorString) ConfigVectorString(other.storage_.vectorString); break;
            default: break;
        }
    }

    ConfigValue& operator=(const ConfigValue& other) {
        if (this != &other) {
            this->~ConfigValue();
            new (this) ConfigValue(other);
        }
        return *this;
    }

    Type getType() const { return type_; }

    // Type-safe getters
    const std::string& getString() const { return storage_.string.get(); }
    int64_t getInt() const { return storage_.intVal.get(); }
    double getDouble() const { return storage_.doubleVal.get(); }
    bool getBool() const { return storage_.boolVal.get(); }
    const std::vector<int64_t>& getVectorInt() const { return storage_.vectorInt.get(); }
    const std::vector<double>& getVectorDouble() const { return storage_.vectorDouble.get(); }
    const std::vector<std::string>& getVectorString() const { return storage_.vectorString.get(); }

    // Type-safe setters
    void setString(const std::string& value) { storage_.string.set(value); }
    void setInt(int64_t value) { storage_.intVal.set(value); }
    void setDouble(double value) { storage_.doubleVal.set(value); }
    void setBool(bool value) { storage_.boolVal.set(value); }
    void setVectorInt(const std::vector<int64_t>& value) { storage_.vectorInt.set(value); }
    void setVectorDouble(const std::vector<double>& value) { storage_.vectorDouble.set(value); }
    void setVectorString(const std::vector<std::string>& value) { storage_.vectorString.set(value); }

    bool isEmpty() const { return type_ == Type::None; }

private:
    Type type_;
    union Storage {
        ConfigString string;
        ConfigInt intVal;
        ConfigDouble doubleVal;
        ConfigBool boolVal;
        ConfigVectorInt vectorInt;
        ConfigVectorDouble vectorDouble;
        ConfigVectorString vectorString;
        Storage() {}
        ~Storage() {}
    } storage_;
};

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
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<int64_t> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::vector<int64_t>> getVectorInt(const std::string& key) const;
    std::optional<std::vector<double>> getVectorDouble(const std::string& key) const;
    std::optional<std::vector<std::string>> getVectorString(const std::string& key) const;
    
    // Get values with default values
    std::string getStringOr(const std::string& key, const std::string& defaultValue) const;
    int64_t getIntOr(const std::string& key, int64_t defaultValue) const;
    double getDoubleOr(const std::string& key, double defaultValue) const;
    bool getBoolOr(const std::string& key, bool defaultValue) const;
    std::vector<int64_t> getVectorIntOr(const std::string& key, const std::vector<int64_t>& defaultValue) const;
    std::vector<double> getVectorDoubleOr(const std::string& key, const std::vector<double>& defaultValue) const;
    std::vector<std::string> getVectorStringOr(const std::string& key, const std::vector<std::string>& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int64_t>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Validate configuration values
    bool validate() const;
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t defaultEntries;
        size_t fileEntries;
        size_t commandLineEntries;
        size_t runtimeEntries;
        size_t stringEntries;
        size_t intEntries;
        size_t doubleEntries;
        size_t boolEntries;
        size_t vectorEntries;
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions for configuration
namespace ConfigHelper {
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    bool isValidKey(const std::string& key);
    std::string sanitizeKey(const std::string& key);
}

} // namespace nlm