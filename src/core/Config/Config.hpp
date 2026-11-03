#pragma once

#include "IConfiguration.hpp"
#include "ConfigurationTemplate.hpp"
#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types (backward compatibility)
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

// Main configuration class - now implements IConfiguration interface
class Config : public IConfiguration {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // IConfiguration interface implementation
    void set(const std::string& key, const std::string& value) override;
    std::optional<std::string> getString(const std::string& key) const override;
    std::optional<int> getInt(const std::string& key) const override;
    std::optional<double> getDouble(const std::string& key) const override;
    std::optional<bool> getBool(const std::string& key) const override;
    
    std::string getString(const std::string& key, const std::string& defaultValue) const override;
    int getInt(const std::string& key, int defaultValue) const override;
    double getDouble(const std::string& key, double defaultValue) const override;
    bool getBool(const std::string& key, bool defaultValue) const override;
    
    bool has(const std::string& key) const override;
    void remove(const std::string& key) override;
    std::vector<std::string> getKeys() const override;
    void clear() override;
    
    ConfigSource getSource(const std::string& key) const override;
    std::string getDescription(const std::string& key) const override;
    
    std::shared_ptr<IConfiguration> getSection(const std::string& section) const override;
    void setSection(const std::string& section, const std::shared_ptr<IConfiguration>& config) override;
    
    void addChangeCallback(const ConfigurationChangeCallback& callback) override;
    void removeChangeCallback(const ConfigurationChangeCallback& callback) override;
    
    std::string toString() const override;
    bool fromString(const std::string& data) override;
    
    bool equals(const IConfiguration& other) const override;
    
    size_t getSize() const override;
    double getMemoryUsage() const override;
    
    // Legacy configuration methods (backward compatibility)
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Configuration validation
    bool validate() const override;
    
    // Configuration conversion
    template<typename T>
    std::optional<T> getAs(const std::string& key) const {
        return getOptional<T>(key);
    }
    
    template<typename T>
    T getAs(const std::string& key, const T& defaultValue) const {
        return get<T>(key, defaultValue);
    }
    
    // Configuration source management
    void setSource(const std::string& key, ConfigSource source);
    ConfigSource getSource(const std::string& key) const;
    
    // Configuration description
    void setDescription(const std::string& key, const std::string& description);
    std::string getDescription(const std::string& key) const;
    
    // Configuration statistics
    struct Statistics {
        size_t totalKeys = 0;
        size_t stringKeys = 0;
        size_t intKeys = 0;
        size_t int64Keys = 0;
        size_t doubleKeys = 0;
        size_t boolKeys = 0;
        size_t vectorKeys = 0;
        double memoryUsage = 0.0;
        double averageKeySize = 0.0;
    };
    
    Statistics getStatistics() const;
    
    // Configuration optimization
    void optimizeMemory() { /* Implementation for memory optimization */ }
    void optimizeSpeed() { /* Implementation for speed optimization */ }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Interface implementation helpers
    std::shared_ptr<ConfigurationTemplate> getTemplateImpl() const;
};

} // namespace nlm
