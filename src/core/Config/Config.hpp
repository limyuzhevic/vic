#pragma once

#include "../core/Types/Types.hpp"
#include "Validation/ValidationSystem.hpp"
#include "Logger/Logger.hpp"
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace nlm {

// Forward declarations
class Config;

// Parameter validation callback type
using ParameterValidator = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration value types
using ConfigValue =
    std::variant<int, int64_t, double, bool, std::string, std::vector<int>,
                 std::vector<double>, std::vector<std::string>>;

// Configuration source
enum class ConfigSource { Default, File, CommandLine, Runtime };

// Configuration entry with validation
struct ConfigEntry {
  std::string key;
  ConfigValue value;
  ConfigSource source;
  std::string description;
  std::vector<std::string> dependentParameters;
  bool required;
  std::string validationError;

  ConfigEntry()
      : key(), value(), source(ConfigSource::Default), description(), 
        dependentParameters(), required(false), validationError() {}
  ConfigEntry(const std::string &k, const ConfigValue &v, ConfigSource s,
              const std::string &desc = "")
      : key(k), value(v), source(s), description(desc), 
        dependentParameters(), required(false), validationError() {}
  ConfigEntry(const std::string &k, const ConfigValue &v, ConfigSource s,
              const std::string &desc, const std::vector<std::string> &deps)
      : key(k), value(v), source(s), description(desc), 
        dependentParameters(deps), required(false), validationError() {}
  ConfigEntry(const std::string &k, const ConfigValue &v, ConfigSource s,
              const std::string &desc, const std::vector<std::string> &deps,
              bool req)
      : key(k), value(v), source(s), description(desc), 
        dependentParameters(deps), required(req), validationError() {}
};

// Parameter range specification
struct ParameterRange {
    std::string name;
    double min;
    double max;
    std::string type; // "int", "double", "bool", "string"
    std::string unit;
    std::string description;
};

// Main configuration class with enhanced validation
class Config {
public:
  Config();
  ~Config();

  // Disable copying, enable moving
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;
  Config(Config &&) noexcept;
  Config &operator=(Config &&) noexcept;

  // Load from file (JSON format)
  bool loadFromFile(const std::string &filepath);

  // Load from command line arguments
  bool loadFromArgs(int argc, char **argv);

  // Save to file
  bool saveToFile(const std::string &filepath) const;

  // Get values
  template <typename T> std::optional<T> get(const std::string &key) const;

  template <typename T>
  T getOr(const std::string &key, const T &defaultValue) const;

  // Set values
  void set(const std::string &key, const ConfigValue &value,
           ConfigSource source = ConfigSource::Runtime);
  void set(const std::string &key, const std::string &value,
           ConfigSource source = ConfigSource::Runtime);
  void set(const std::string &key, int value,
           ConfigSource source = ConfigSource::Runtime);
  void set(const std::string &key, double value,
           ConfigSource source = ConfigSource::Runtime);
  void set(const std::string &key, bool value,
           ConfigSource source = ConfigSource::Runtime);

  // Set with validation
  bool setWithValidation(const std::string &key, const ConfigValue &value,
                        ConfigSource source = ConfigSource::Runtime);

  // Register parameter validation
  void registerParameterValidation(const std::string &key,
                                   const ParameterValidator &validator);

  // Register parameter range
  void registerParameterRange(const ParameterRange &range);

  // Register dependency between parameters
  void registerDependency(const std::string &dependentKey,
                         const std::string &requiredKey);

  // Check existence
  bool has(const std::string &key) const;

  // Remove key
  void remove(const std::string &key);

  // Get all keys
  std::vector<std::string> getKeys() const;

  // Clear all
  void clear();

  // Get configuration summary with validation status
  std::string summary() const;

  // Get validation errors
  std::vector<std::string> getValidationErrors() const;

  // Check if configuration is valid
  bool isValid() const;

  // Reset validation errors
  void resetValidationErrors();

  // Enable/disable validation
  void setValidationEnabled(bool enabled);
  bool isValidationEnabled() const;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  // Internal helpers
  static std::string trim(const std::string &str);
  static std::string toLower(const std::string &str);
  static std::optional<ConfigValue> stringToConfigValue(const std::string &str);
  static std::string configValueToString(const ConfigValue &value);
  
  // Type conversion helpers
  template <typename T>
  static std::optional<T> configValueToTyped(const ConfigValue &value);
};

// Implementation details
namespace config_impl {
  // Helper function to validate parameter ranges
  bool validateParameterRange(const std::string &key, const ConfigValue &value,
                             const std::vector<ParameterRange> &ranges);
  
  // Helper function to check parameter dependencies
  ValidationResult checkParameterDependencies(const std::string &key,
                                              const std::vector<std::string> &dependencies,
                                              const std::unordered_map<std::string, ConfigValue> &configValues);
  
  // Helper function to validate parameter with custom validator
  ValidationResult validateParameterCustom(const std::string &key,
                                          const ConfigValue &value,
                                          const ParameterValidator &validator);
}

} // namespace nlm