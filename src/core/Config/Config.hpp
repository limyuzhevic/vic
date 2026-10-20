#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

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

// Configuration key for better organization
class ConfigKey {
public:
    enum class Category {
        Neural,           // Neural network parameters
        Simulation,       // Simulation parameters
        Plasticity,       // Plasticity parameters
        Memory,           // Memory system parameters
        Neuromodulation,  // Neuromodulation parameters
        Development,      // Developmental parameters
        System,           // System-level parameters
        Debug             // Debug and testing parameters
    };
    
    ConfigKey(const std::string& key, Category category = Category::System, 
              const std::string& description = "")
        : key_(key), category_(category), description_(description) {}
    
    const std::string& getKey() const { return key_; }
    Category getCategory() const { return category_; }
    const std::string& getDescription() const { return description_; }
    
    bool operator<(const ConfigKey& other) const { return key_ < other.key_; }
    bool operator==(const ConfigKey& other) const { return key_ == other.key_; }
    
private:
    std::string key_;
    Category category_;
    std::string description_;
};

// Typed configuration value wrapper for better type safety
class TypedConfigValue {
public:
    enum class Type {
        Integer,
        Int64,
        Double,
        Bool,
        String,
        IntVector,
        DoubleVector,
        StringVector
    };
    
    TypedConfigValue() : type_(Type::String), string_value_("") {}
    
    template<typename T>
    static TypedConfigValue from(const T& value);
    
    template<typename T>
    T get() const;
    
    template<typename T>
    bool has() const;
    
    Type getType() const { return type_; }
    const std::string& getKey() const { return key_; }
    void setKey(const std::string& key) { key_ = key; }
    
    // Serialization and validation
    virtual bool validate() const { return true; }
    virtual std::string toString() const { return "";
    virtual bool equals(const TypedConfigValue& other) const { return false; }
    
protected:
    Type type_;
    std::string key_;
    
    // Type-specific storage
    union {
        int int_val;
        int64_t int64_val;
        double double_val;
        bool bool_val;
        std::string string_val;
        std::vector<int> int_vec_val;
        std::vector<double> double_vec_val;
        std::vector<std::string> string_vec_val;
    };
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
