#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <ctime>

#include "ConfigSchema.hpp"
#include "ConfigError.hpp"
#include "ConfigHelp.hpp"

namespace nlm {

// Enhanced configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Profile
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::chrono::system_clock::time_point timestamp;
    std::string validatorKey;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category(),
                    timestamp(std::chrono::system_clock::now()), validatorKey() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc = "", const std::string& cat = "General")
        : key(k), value(v), source(s), description(desc), category(cat),
          timestamp(std::chrono::system_clock::now()), validatorKey() {}
    
    // Type-safe getters with validation
    template<typename T>
    std::optional<T> getTyped() const {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Validation helpers
    std::string getSourceString() const {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            case ConfigSource::Environment: return "Environment";
            case ConfigSource::Profile: return "Profile";
            default: return "Unknown";
        }
    }
    
    std::string getTimestampString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Configuration backup and recovery
struct ConfigBackup {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    std::chrono::system_clock::time_point created;
    std::size_t version;
    
    ConfigBackup(const std::string& n, const std::string& desc) 
        : name(n), description(desc), version(1), created(std::chrono::system_clock::now()) {}
    
    std::string getCreatedString() const {
        auto time = std::chrono::system_clock::to_time_t(created);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading from file with JSON/YAML support
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Enhanced loading from command line with better parsing
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::map<std::string, std::string>& envVars = {});
    
    // Load from profile
    bool loadFromProfile(ConfigProfile profile, const ConfigSchema& schema);
    
    // Enhanced saving to file with format options
    bool saveToFile(const std::string& filepath, const std::string& format = "simple") const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced validation
    bool validate(const ConfigSchema& schema) const;
    std::vector<ConfigError> validateParameter(const std::string& key, const ConfigSchema& schema) const;
    std::vector<ConfigError> validateAll(const ConfigSchema& schema) const;
    
    // Configuration help and documentation
    std::string getHelp(const std::string& key, const ConfigSchema& schema) const;
    std::string getCategoryHelp(const std::string& category, const ConfigSchema& schema) const;
    std::string getAllHelp(const ConfigSchema& schema) const;
    std::string getProfileHelp() const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    void clear();
    
    // Configuration backup and recovery
    bool createBackup(const std::string& name, const std::string& description = "");
    bool restoreBackup(const std::string& name);
    std::vector<std::string> getBackupNames() const;
    std::string getBackupInfo(const std::string& name) const;
    
    // Configuration comparison and diff
    std::string diff(const Config& other) const;
    bool equals(const Config& other) const;
    
    // Configuration summary with enhanced details
    std::string summary() const;
    std::string summary(const ConfigSchema& schema) const;
    
    // Get configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCountBySource(ConfigSource source) const;
    double getValidationScore(const ConfigSchema& schema) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration profiles
    bool setProfile(ConfigProfile profile, const ConfigSchema& schema);
    ConfigProfile getProfile() const { return currentProfile; }
    std::string getProfileName(ConfigProfile profile) const;
    
    // Smart defaults
    void applySmartDefaults(const ConfigSchema& schema, ConfigProfile profile = ConfigProfile::None);
    std::string getSmartDefaults(const ConfigSchema& schema, ConfigProfile profile = ConfigProfile::None) const;
    
    // Validation helpers
    static bool isValidType(const ConfigParameter& param, const ConfigValue& value);
    static bool isValidRange(const ConfigParameter& param, const ConfigValue& value);
    static bool isValidPositive(const ConfigValue& value);
    static bool isValidNonNegative(const ConfigValue& value);
    static std::vector<std::string> getValidValueStrings(const ConfigParameter& param);
    
    // Error handling helpers
    static std::string getErrorSuggestion(const ConfigError& error);
    static std::string getErrorDetails(const ConfigError& error);
    
    // Configuration value suggestions
    static std::vector<std::string> suggestValues(const std::string& key, const ConfigSchema& schema);
    
    // Configuration validation
    class Validator {
    public:
        static bool validate(const ConfigValue& value, const ConfigParameter& param);
        static std::vector<ConfigError> validateAll(const std::map<std::string, ConfigValue>& values,
                                                   const ConfigSchema& schema);
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string formatValue(const ConfigValue& value);
    static ConfigValue parseValue(const std::string& str);
    
    // Configuration management
    void addEntry(const std::string& key, const ConfigValue& value, ConfigSource source,
                  const std::string& description = "", const std::string& category = "General");
    
    // File I/O helpers
    static bool saveSimpleFormat(const std::string& filepath, const std::vector<ConfigEntry>& entries) const;
    static bool loadSimpleFormat(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // Command line parsing helpers
    static bool parseArg(const std::string& arg, std::string& key, std::string& value);
    static std::string normalizeKey(const std::string& key);
    
    // Environment variable helpers
    static std::string envKeyToConfigKey(const std::string& envKey);
    
    // Backup helpers
    std::string getBackupFilePath(const std::string& name) const;
    static std::string generateBackupName();
    
    ConfigProfile currentProfile;
    std::map<std::string, ConfigBackup> backups;
    std::chrono::system_clock::time_point lastModified;
};