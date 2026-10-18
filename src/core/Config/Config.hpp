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
class Config {
public:
    // Configuration validation
    bool validate(std::string& errorMessage) const;
    
    // Specific validation for common parameter types
    static bool validateNeuronCount(int64_t count, std::string& errorMessage);
    static bool validateConnectionProbability(float probability, std::string& errorMessage);
    static bool validateTimestep(double timestep, std::string& errorMessage);
    static bool validateWeight(float weight, std::string& errorMessage);
    static bool validateRate(float rate, std::string& errorMessage);
    static bool validateString(const std::string& value, std::string& errorMessage);
    
    // Get validation errors for all parameters
    std::vector<std::string> getValidationErrors() const;
    
    // Check if specific parameter is valid
    bool isValid(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
