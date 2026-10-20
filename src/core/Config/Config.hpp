// Enhanced Configuration system with runtime modification and validation
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& message) : std::runtime_error("Config Validation Error: " + message) {}
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, ConfigSource source)>;

// Configuration value types with enhanced type information
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// Enhanced configuration metadata
struct ConfigMetadata {
    std::string description;
    std::string category;
    std::string type; // "int", "double", "string", "bool", "array"
    bool required = false;
    std::variant<int, int64_t, double, bool, std::string> defaultValue;
    std::optional<std::pair<double, double>> range; // For numeric types
    std::optional<size_t> minArraySize;
    std::optional<size_t> maxArraySize;
    
    // Validation function
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigMetadata() : description(), category(), type(), required(false), validator(nullptr) {}
    ConfigMetadata(const std::string& desc, const std::string& cat, const std::string& t, 
                   bool req = false, const std::variant<int, int64_t, double, bool, std::string>& def = {}, 
                   const std::optional<std::pair<double, double>>& rng = std::nullopt,
                   const std::optional<size_t>& minArr = std::nullopt,
                   const std::optional<size_t>& maxArr = std::nullopt)
        : description(desc), category(cat), type(t), required(req), defaultValue(def), 
          range(rng), minArraySize(minArr), maxArraySize(maxArr), validator(nullptr) {}
};

// Configuration history entry for runtime modification tracking
struct ConfigHistoryEntry {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string sessionId;
    
    ConfigHistoryEntry(const std::string& k, const ConfigValue& oldVal, const ConfigValue& newVal, 
                      ConfigSource src, const std::string& session)
        : key(k), oldValue(oldVal), newValue(newVal), source(src), timestamp(std::chrono::system_clock::now()), sessionId(session) {}
};

// Configuration change event
struct ConfigChangeEvent {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string sessionId;
    
    ConfigChangeEvent(const std::string& k, const ConfigValue& val, ConfigSource src, const std::string& session)
        : key(k), value(val), source(src), timestamp(std::chrono::system_clock::now()), sessionId(session) {}
};

// Enhanced configuration entry with metadata
struct EnhancedConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::chrono::system_clock::time_point lastModified;
    std::string lastModifiedSession;
    
    EnhancedConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category(), 
                           lastModified(std::chrono::system_clock::now()), lastModifiedSession() {}
    
    EnhancedConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                       const std::string& desc = "", const std::string& cat = "")
        : key(k), value(v), source(s), description(desc), category(cat),
          lastModified(std::chrono::system_clock::now()), lastModifiedSession() {}
};

// Main configuration class with runtime modification and validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading with error handling and validation
    bool loadFromFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath, const std::string& sessionId);
    
    // Enhanced loading from command line with validation
    bool loadFromArgs(int argc, char** argv);
    bool loadFromArgs(int argc, char** argv, const std::string& sessionId);
    
    // Save to file with enhanced formatting
    bool saveToFile(const std::string& filepath) const;
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    
    // Enhanced get methods with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced set methods with validation and callbacks
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& sessionId = "default");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& sessionId = "default");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& sessionId = "default");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& sessionId = "default");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& sessionId = "default");
    
    // Runtime modification with change tracking
    bool modify(const std::string& key, const ConfigValue& newValue, 
                const std::string& sessionId = "runtime", bool validate = true);
    
    // Check existence with metadata access
    bool has(const std::string& key) const;
    bool has(const std::string& key) const;
    
    // Remove with change tracking
    void remove(const std::string& key, const std::string& sessionId = "default");
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all with change tracking
    void clear(const std::string& sessionId = "default");
    
    // Enhanced configuration management
    std::string getDescription(const std::string& key) const;
    std::string getCategory(const std::string& key) const;
    std::string getType(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    
    // Default value management
    template<typename T>
    void setDefault(const std::string& key, const T& defaultValue, 
                   const std::string& description = "", const std::string& category = "",
                   const std::string& type = "auto", bool required = false);
    
    // Validation
    bool validate(const std::string& key) const;
    bool validateAll() const;
    
    // Configuration history and change tracking
    std::vector<ConfigHistoryEntry> getHistory(const std::string& key, size_t limit = 100) const;
    std::vector<ConfigChangeEvent> getRecentChanges(size_t limit = 50) const;
    size_t getChangeCount() const;
    
    // Session management
    void setSessionId(const std::string& sessionId);
    std::string getSessionId() const;
    std::vector<std::string> getActiveSessions() const;
    void cleanupSession(const std::string& sessionId);
    
    // Callbacks
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    void clearChangeCallbacks();
    
    // Enhanced configuration summary
    std::string summary() const;
    std::string summary(bool includeHistory = false) const;
    std::string summary(const std::string& sessionId) const;
    
    // Configuration presets
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createExplorationConfig();
    static std::shared_ptr<Config> createNavigationConfig();
    static std::shared_ptr<Config> createLearningConfig();
    static std::shared_ptr<Config> createPerformanceConfig();
    
    // Configuration validation schemas
    static void registerConfigSchema(const std::string& category, const std::unordered_map<std::string, ConfigMetadata>& schema);
    static bool validateConfigSchema(const std::string& category, const Config& config);
    static std::vector<std::string> getAllCategories();
    
    // Change rollback
    bool rollback(const std::string& key, const ConfigHistoryEntry& historyEntry);
    std::vector<ConfigHistoryEntry> getAllChanges() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    int compare(const Config& other) const; // Returns -1, 0, 1
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Internal methods
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    void addHistoryEntry(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, 
                        ConfigSource source, const std::string& sessionId);
    void notifyChangeCallbacks(const std::string& key, const ConfigValue& oldValue, 
                              const ConfigValue& newValue, ConfigSource source);
    ConfigValue stringToConfigValue(const std::string& str) const;
    std::string configValueToString(const ConfigValue& value) const;
    
    // Schema validation
    bool validateAgainstSchema(const std::string& key, const ConfigValue& value) const;
    
    // Session management
    void updateSessionTime(const std::string& sessionId);
    bool isSessionActive(const std::string& sessionId) const;
};

// Configuration builder for easier configuration creation
class ConfigBuilder {
public:
    ConfigBuilder();
    ~ConfigBuilder();
    
    // Disable copying, enable moving
    ConfigBuilder(const ConfigBuilder&) = delete;
    ConfigBuilder& operator=(const ConfigBuilder&) = delete;
    
    // Set methods for building configuration
    ConfigBuilder& set(const std::string& key, const std::string& value);
    ConfigBuilder& set(const std::string& key, int value);
    ConfigBuilder& set(const std::string& key, double value);
    ConfigBuilder& set(const std::string& key, bool value);
    ConfigBuilder& set(const std::string& key, const std::vector<std::string>& value);
    ConfigBuilder& set(const std::string& key, const std::vector<double>& value);
    
    // Set metadata
    ConfigBuilder& setDescription(const std::string& key, const std::string& description);
    ConfigBuilder& setCategory(const std::string& key, const std::string& category);
    ConfigBuilder& setType(const std::string& key, const std::string& type);
    ConfigBuilder& setRequired(const std::string& key, bool required);
    ConfigBuilder& setRange(const std::string& key, double min, double max);
    
    // Validation
    ConfigBuilder& addValidator(const std::string& key, const std::function<bool(const ConfigValue&)>& validator);
    ConfigBuilder& setDefault(const std::string& key, const std::variant<int, double, std::string, bool>& defaultValue);
    
    // Build configuration
    std::shared_ptr<Config> build();
    
    // Clear builder
    void clear();
    
    // Get current configuration
    std::shared_ptr<Config> getConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration comparison utilities
namespace ConfigUtils {
    // Merge two configurations (other takes precedence)
    void merge(Config& target, const Config& source);
    
    // Filter configuration by category
    std::shared_ptr<Config> filterByCategory(const Config& config, const std::string& category);
    
    // Get configuration values as specific type with fallback
    template<typename T>
    T getValue(const Config& config, const std::string& key, const T& defaultValue = T());
    
    // Convert configuration to JSON
    nlohmann::json toJson(const Config& config);
    
    // Create configuration from JSON
    std::shared_ptr<Config> fromJson(const nlohmann::json& json);
    
    // Validate configuration with external schema
    bool validateWithSchema(const Config& config, const std::string& schemaFile);
}

} // namespace nlm

// For backward compatibility
namespace nlm {
    // Original Config interface (deprecated but still available)
    class LegacyConfig {
    public:
        LegacyConfig() = default;
        ~LegacyConfig() = default;
        
        bool loadFromFile(const std::string& filepath) { return config_->loadFromFile(filepath, "legacy"); }
        bool loadFromArgs(int argc, char** argv) { return config_->loadFromArgs(argc, argv, "legacy"); }
        bool saveToFile(const std::string& filepath) const { return config_->saveToFile(filepath); }
        
        template<typename T>
        std::optional<T> get(const std::string& key) const { return config_->get<T>(key); }
        
        template<typename T>
        T getOr(const std::string& key, const T& defaultValue) const { return config_->getOr<T>(key, defaultValue); }
        
        void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            config_->set(key, value, source, "legacy");
        }
        
        bool has(const std::string& key) const { return config_->has(key); }
        std::vector<std::string> getKeys() const { return config_->getKeys(); }
        void clear() { config_->clear("legacy"); }
        
        std::string summary() const { return config_->summary(); }
        
    private:
        std::shared_ptr<Config> config_ = Config::createDefaultConfig();
    };
}

// Convenience functions for common configuration patterns
namespace nlm {
    inline std::shared_ptr<Config> createBasicConfig() {
        return Config::createDefaultConfig();
    }
    
    inline std::shared_ptr<Config> createLearningConfig() {
        return Config::createLearningConfig();
    }
    
    inline std::shared_ptr<Config> createExplorationConfig() {
        return Config::createExplorationConfig();
    }
    
    inline std::shared_ptr<Config> createNavigationConfig() {
        return Config::createNavigationConfig();
    }
    
    inline std::shared_ptr<Config> createPerformanceConfig() {
        return Config::createPerformanceConfig();
    }
    
    inline ConfigBuilder makeConfigBuilder() {
        return ConfigBuilder();
    }
    
    inline bool validateConfig(const Config& config) {
        return config.validateAll();
    }
    
    inline std::string getConfigSummary(const Config& config) {
        return config.summary();
    }
}