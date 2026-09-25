#include <unordered_map>
#include <functional>

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
    
public:
    // Validate configuration against schema
    bool validate(const std::vector<std::string>& requiredKeys = {}) const {
        return validateConfig(requiredKeys);
    }
    
    // Validate configuration
    bool validateConfig(const std::vector<std::string>& requiredKeys = {}) const;
    
    // Get validation result
    ValidationResult getValidationResult() const {
        ValidationResult result;
        result.isValid = isValid();
        result.errors = getValidationErrors();
        result.warnings = getValidationWarnings();
        result.statistics = getStatistics();
        return result;
    }
    
    // Get validation status
    bool isValid() const {
        return validationEnabled_ && validationErrors_.empty();
    }
    
    // Get all validation issues
    std::vector<std::string> getAllValidationIssues() const {
        std::vector<std::string> allIssues;
        allIssues.insert(allIssues.end(), validationErrors_.begin(), validationErrors_.end());
        allIssues.insert(allIssues.end(), validationWarnings_.begin(), validationWarnings_.end());
        return allIssues;
    }
    
    // Get validation statistics
    std::string getStatistics() const {
        return configStats_;
    }
    
    // Save configuration to file with compression option
    bool saveToFile(const std::string& filepath, bool compressed = false) const;
    
    // Load configuration from JSON file (new format)
    bool loadFromJSON(const std::string& filepath);
    
    // Get configuration by category
    std::map<std::string, std::map<std::string, std::string>> getConfigurationByCategory() const;
    
    // Apply configuration presets
    bool applyPreset(const std::string& presetName);
    
    // Reset to factory defaults
    void resetToDefaults();
    
    // Export configuration for sharing
    bool exportConfiguration(const std::string& filepath) const;
    
    // Import configuration from shared file
    bool importConfiguration(const std::string& filepath);
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const {
        return !(*this == other);
    }
    
    // Get configuration age (for tracking modifications)
    std::chrono::system_clock::time_point getCreationTime() const {
        return creationTime_;
    }
    
    // Check if configuration has been modified
    bool hasBeenModified() const {
        return creationTime_ != lastModifiedTime_;
    }
    
    // Get configuration history (last changes)
    std::string getConfigurationHistory() const {
        return modificationHistory_;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validation state
    bool validationEnabled_ = false;
    std::vector<std::string> validationErrors_;
    std::vector<std::string> validationWarnings_;
    std::string validationStatistics_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    void setValidationEnabled(bool enabled);
    void clearValidationErrors();
    void clearValidationWarnings();
    void setValidationStatistics(const std::string& stats);
    
    // Helper to access private Impl
    struct ImplAccess {
        static std::vector<ConfigEntry>& getEntries(Config& config) {
            return static_cast<Impl*>(config.pImpl.get())->entries;
        }
    };
};

} // namespace nlm
