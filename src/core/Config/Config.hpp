#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>

namespace nlm {

// Configuration validation exceptions
class ConfigValidationException : public std::exception {
private:
    std::string message_;
    
public:
    explicit ConfigValidationException(const std::string& message) : message_(message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
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
    std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>> value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& v, ConfigSource s, const std::string& desc = "")
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
    
    // Load from file (key=value format)
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
    
    // Configuration validation
    void validate() const;
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
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
    
    // Apply defaults for Phase 2
    void applyPhase2Defaults();
    
    // Validate specific configuration values
    void validateNeurons(int64_t neurons) const;
    void validateTimestep(double timestep) const;
    void validateRegionCount(int64_t regions) const;
    void validateConnectionProbability(float prob) const;
    void validatePlasticityParameters() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validation helpers
    std::vector<std::string> validationErrors_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Value conversion and validation helpers
    static bool isPositive(int64_t value) { return value > 0; }
    static bool isNonNegative(int64_t value) { return value >= 0; }
    static bool isPositive(double value) { return value > 0.0; }
    static bool isNonNegative(double value) { return value >= 0.0; }
    static bool isBetween(double value, double min, double max) { return value >= min && value <= max; }
    static bool isBoolean(const std::string& value);
    static bool isNumeric(const std::string& value);
    static std::string convertToString(const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value);
    
    // Reset validation errors
    void clearValidationErrors() { validationErrors_.clear(); }
    
    // Add validation error
    void addValidationError(const std::string& error) { validationErrors_.push_back(error); }
    
    // Check if validation has errors
    bool hasValidationErrors() const { return !validationErrors_.empty(); }
};

} // namespace nlm
