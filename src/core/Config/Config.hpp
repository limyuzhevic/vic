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
    
    // Configuration validation
    bool validate() const;
    
    // Get configuration summary with validation
    std::string summaryWithValidation() const;
    
    // Configuration inheritance
    bool inheritsFrom(const Config& other) const;
    
    // Configuration merging
    void merge(const Config& other);
    
    // Configuration filtering
    Config filter(const std::vector<std::string>& keyPatterns) const;
    
    // Configuration keys by prefix
    std::vector<std::string> getKeysByPrefix(const std::string& prefix) const;
    
    // Check if key exists with any source
    bool hasKeyWithSource(const std::string& key, ConfigSource source) const;
    
    // Get keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Configuration statistics
    std::string getStatistics() const;
    
    // Deep copy
    Config copy() const;
    
    // Merge from file
    bool mergeFromFile(const std::string& filepath);
    
    // Export configuration
    std::string exportConfig() const;
    
    // Import configuration
    bool importConfig(const std::string& configData);
    
    // Configuration is valid
    bool isValid() const;
    
    // Configuration has errors
    bool hasErrors() const;
    
    // Get error messages
    std::vector<std::string> getErrorMessages() const;
    
    // Clear errors
    void clearErrors();
    
    // Add configuration description
    void addDescription(const std::string& key, const std::string& description);
    
    // Get configuration description
    std::string getDescription(const std::string& key) const;
    
    // Check if description exists
    bool hasDescription(const std::string& key) const;
    
    // Get all descriptions
    std::map<std::string, std::string> getAllDescriptions() const;
    
    // Validate specific key
    bool validateKey(const std::string& key) const;
    
    // Get validation errors for specific key
    std::vector<std::string> getKeyValidationErrors(const std::string& key) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration difference
    Config diff(const Config& other) const;
    
    // Configuration patch
    bool patch(const Config& patchConfig);
    
    // Configuration reset
    void resetToDefaults();
    
    // Check if defaults are set
    bool hasDefaults() const;
    
    // Get default value
    std::optional<ConfigValue> getDefault(const std::string& key) const;
    
    // Set default value
    void setDefault(const std::string& key, const ConfigValue& value);
    
    // Remove default value
    void removeDefault(const std::string& key);
    
    // Configuration snapshot
    std::string snapshot() const;
    
    // Restore from snapshot
    bool restoreFromSnapshot(const std::string& snapshot);
    
    // Configuration backup
    bool backup(const std::string& filepath) const;
    
    // Restore from backup
    bool restoreBackup(const std::string& filepath);
    
    // Configuration diff with backup
    bool diffWithBackup(const std::string& filepath, std::vector<std::string>& changes) const;
    
    // Check if backup exists
    bool hasBackup() const;
    
    // Clean up backups
    void cleanupBackups();
    
    // Get backup files
    std::vector<std::string> getBackupFiles() const;
    
    // Configuration validation
    bool validateConfig() const;
    
    // Configuration repair
    bool repairConfig();
    
    // Configuration optimization
    bool optimizeConfig();
    
    // Configuration compression
    std::string compressConfig() const;
    
    // Configuration decompression
    bool decompressConfig(const std::string& compressedData);
    
    // Configuration encryption (placeholder)
    std::string encryptConfig(const std::string& password) const;
    
    // Configuration decryption (placeholder)
    bool decryptConfig(const std::string& encryptedData, const std::string& password);
    
    // Configuration hash
    std::string hashConfig() const;
    
    // Configuration compare
    bool configEquals(const Config& other) const;
    
    // Configuration merge
    void configMerge(const Config& other);
    
    // Configuration filter by type
    Config filterByType(const std::type_index& type) const;
    
    // Configuration keys by type
    std::vector<std::string> getKeysByType(const std::type_index& type) const;
    
    // Configuration value by type
    std::optional<ConfigValue> getValueByType(const std::string& key, const std::type_index& type) const;
    
    // Configuration set by type
    bool setValueByType(const std::string& key, const ConfigValue& value, const std::type_index& type);
    
    // Configuration remove by type
    bool removeValueByType(const std::string& key, const std::type_index& type);
    
    // Configuration validate by type
    bool validateByType(const std::string& key, const std::type_index& type) const;
    
    // Configuration get size
    size_t size() const;
    
    // Configuration empty
    bool empty() const;
    
    // Configuration max size
    size_t maxSize() const;
    
    // Configuration reserve
    void reserve(size_t capacity);
    
    // Configuration shrink to fit
    void shrinkToFit();
    
    // Configuration swap
    void swap(Config& other);
    
    // Configuration move
    Config& operator=(Config&& other) noexcept;
    
    // Configuration copy constructor
    Config(const Config& other);
    
    // Default constructor
    Config() = default;
    
    // Destructor
    ~Config() = default;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper function to set config value based on key
    void setConfigValue(const std::string& key, const std::string& value);
};

} // namespace nlm
