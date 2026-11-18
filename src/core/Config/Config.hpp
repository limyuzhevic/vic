#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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
    
    // Configuration validation and schema support
    bool validate(const std::string& schema = "") const;
    bool validate(const Config& reference, std::string& errors) const;
    bool validate(const Config& reference, std::string& errors, const std::string& schema) const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationErrors(const Config& reference) const;
    std::vector<std::string> getValidationErrors(const Config& reference, const std::string& schema) const;
    
    // Configuration schema management
    void setSchema(const std::string& schema);
    std::string getSchema() const;
    void clearSchema();
    
    // Configuration templates
    bool applyTemplate(const std::string& template_name);
    bool saveTemplate(const std::string& template_name) const;
    std::vector<std::string> getAvailableTemplates() const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::string getStats() const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    bool isSubset(const Config& other) const;
    bool equals(const Config& other) const;
    
    // Configuration diff
    std::vector<std::string> diff(const Config& other) const;
    std::string diffSummary(const Config& other) const;
    
    // Configuration backup and restore
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    bool restore(const std::string& filepath, ConfigSource source);
    
    // Configuration monitoring
    bool isModified() const;
    size_t getModificationCount() const;
    std::chrono::system_clock::time_point getLastModifiedTime() const;
    
    // Configuration export/import
    std::string exportToJSON() const;
    std::string exportToString() const;
    bool importFromJSON(const std::string& json, ConfigSource source = ConfigSource::File);
    bool importFromString(const std::string& data, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration history
    void enableHistory(bool enable);
    bool isHistoryEnabled() const;
    std::vector<Config> getHistory() const;
    bool hasHistory() const;
    void clearHistory();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods for JSON parsing
    bool parseJSON(const std::string& json, ConfigSource source);
    void skipWhitespace(const std::string& str, size_t& pos);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
