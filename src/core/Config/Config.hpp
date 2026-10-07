#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <set>
#include <regex>
#include <filesystem>
#include <chrono>
#include <functional>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <limits>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;
class ConfigProfile;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source enumeration
enum class ConfigSource {
    Default,
    File,
    Environment,
    CommandLine,
    Runtime,
    Python,
    Profile,
    Merged
};

// Profile enumeration
enum class Profile {
    Development,
    Testing,
    Staging,
    Production,
    Research
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    Profile profile = Profile::Development;
    std::chrono::system_clock::time_point timestamp;
    bool encrypted = false;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), profile(Profile::Development),
          timestamp(std::chrono::system_clock::now()), encrypted(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), profile(Profile::Development),
          timestamp(std::chrono::system_clock::now()), encrypted(false) {}
};

// Schema validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// Configuration change event
struct ConfigChangeEvent {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
};

// Statistics structure
struct ConfigStatistics {
    size_t totalEntries = 0;
    size_t entriesBySource[static_cast<size_t>(ConfigSource::Merged) + 1] = {0};
    size_t entriesByProfile[static_cast<size_t>(Profile::Research) + 1] = {0};
    std::chrono::system_clock::time_point lastModified;
    size_t memoryUsageBytes = 0;
    size_t changeCount = 0;
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Legacy API (backward compatibility)
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string valueToString(const ConfigValue& value);
    static ConfigValue parseValue(const std::string& valueStr);
};

} // namespace nlm