#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <type_traits>

namespace nlm {

// Enhanced Configuration System - Phase 3

// Error hierarchy
class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message, const std::string& code = "CONFIG_ERROR", 
                const std::string& context = "", int line = 0, const std::string& file = "")
        : std::runtime_error(formatMessage(message, code, context, line, file)),
          code_(code), context_(context), line_(line), file_(file) {}
    
    const char* what() const noexcept override { return std::runtime_error::what(); }
    const std::string& getCode() const { return code_; }
    const std::string& getContext() const { return context_; }
    int getLine() const { return line_; }
    const std::string& getFile() const { return file_; }
    
private:
    static std::string formatMessage(const std::string& message, const std::string& code,
                                     const std::string& context, int line, const std::string& file) {
        std::ostringstream oss;
        oss << "[" << code << "] " << message;
        if (!context.empty()) oss << " (Context: " << context << ")";
        if (!file.empty()) oss << " (File: " << file << ":" << line << ")";
        return oss.str();
    }
    
    std::string code_;
    std::string context_;
    int line_;
    std::string file_;
};

class InvalidConfigValueError : public ConfigError {
public:
    InvalidConfigValueError(const std::string& key, const std::string& expectedType,
                           const std::string& actualType, const std::string& context = "")
        : ConfigError("Invalid configuration value", "INVALID_VALUE", 
                     context + " for key: " + key, 0, "") {
        expectedType_ = expectedType;
        actualType_ = actualType;
        key_ = key;
    }
    
    const std::string& getExpectedType() const { return expectedType_; }
    const std::string& getActualType() const { return actualType_; }
    const std::string& getKey() const { return key_; }
    
private:
    std::string expectedType_;
    std::string actualType_;
    std::string key_;
};

class MissingConfigError : public ConfigError {
public:
    MissingConfigError(const std::string& key, const std::string& context = "")
        : ConfigError("Missing configuration value", "MISSING_VALUE", 
                     context + " for key: " + key, 0, "") {
        key_ = key;
    }
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& message, const std::string& context = "")
        : ConfigError("Configuration validation failed", "VALIDATION_ERROR", context) {}
};

class ConfigLoadError : public ConfigError {
public:
    ConfigLoadError(const std::string& filepath, const std::string& reason)
        : ConfigError("Failed to load configuration file", "LOAD_ERROR",
                     "File: " + filepath + ", Reason: " + reason) {
        filepath_ = filepath;
    }
    
    const std::string& getFilepath() const { return filepath_; }
    
private:
    std::string filepath_;
};

class ConfigSaveError : public ConfigError {
public:
    ConfigSaveError(const std::string& filepath, const std::string& reason)
        : ConfigError("Failed to save configuration file", "SAVE_ERROR",
                     "File: " + filepath + ", Reason: " + reason) {
        filepath_ = filepath;
    }
    
    const std::string& getFilepath() const { return filepath_; }
    
private:
    std::string filepath_;
};

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<std::vector<double>>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Template,
    Preset
};

// Configuration validation rule
class ValidationRule {
public:
    enum class RuleType {
        Range,
        Type,
        Pattern,
        Required,
        Custom
    };
    
    ValidationRule(RuleType type, const std::string& description)
        : type_(type), description_(description) {}
    
    virtual ~ValidationRule() = default;
    virtual bool validate(const ConfigValue& value) const = 0;
    RuleType getType() const { return type_; }
    const std::string& getDescription() const { return description_; }
    
protected:
    RuleType type_;
    std::string description_;
};

class RangeRule : public ValidationRule {
public:
    RangeRule(double min, double max, const std::string& description)
        : ValidationRule(RuleType::Range, description), min_(min), max_(max) {}
    
    bool validate(const ConfigValue& value) const override {
        if (auto val = std::get_if<double>(&value)) {
            return *val >= min_ && *val <= max_;
        } else if (auto val = std::get_if<int>(&value)) {
            return static_cast<double>(*val) >= min_ && static_cast<double>(*val) <= max_;
        } else if (auto val = std::get_if<int64_t>(&value)) {
            return static_cast<double>(*val) >= min_ && static_cast<double>(*val) <= max_;
        }
        return false;
    }
    
private:
    double min_;
    double max_;
};

class TypeRule : public ValidationRule {
public:
    TypeRule(const std::string& typeName, const std::string& description)
        : ValidationRule(RuleType::Type, description), typeName_(typeName) {}
    
    bool validate(const ConfigValue& value) const override {
        if (typeName_ == "int" && std::get_if<int>(&value)) return true;
        if (typeName_ == "int64_t" && std::get_if<int64_t>(&value)) return true;
        if (typeName_ == "double" && std::get_if<double>(&value)) return true;
        if (typeName_ == "bool" && std::get_if<bool>(&value)) return true;
        if (typeName_ == "string" && std::get_if<std::string>(&value)) return true;
        if (typeName_ == "int_vector" && std::get_if<std::vector<int>>(&value)) return true;
        if (typeName_ == "double_vector" && std::get_if<std::vector<double>>(&value)) return true;
        if (typeName_ == "string_vector" && std::get_if<std::vector<std::string>>(&value)) return true;
        return false;
    }
    
private:
    std::string typeName_;
};

class PatternRule : public ValidationRule {
public:
    PatternRule(const std::string& pattern, const std::string& description)
        : ValidationRule(RuleType::Pattern, description), pattern_(pattern) {}
    
    bool validate(const ConfigValue& value) const override {
        if (auto str = std::get_if<std::string>(&value)) {
            return std::regex_search(*str, std::regex(pattern_));
        }
        return false;
    }
    
private:
    std::string pattern_;
};

class CustomValidationRule : public ValidationRule {
public:
    using ValidatorFunc = std::function<bool(const ConfigValue&)>;
    
    CustomValidationRule(ValidatorFunc validator, const std::string& description)
        : ValidationRule(RuleType::Custom, description), validator_(validator) {}
    
    bool validate(const ConfigValue& value) const override {
        return validator_(value);
    }
    
private:
    ValidatorFunc validator_;
};

// Configuration template
class ConfigTemplate {
public:
    struct TemplateEntry {
        std::string key;
        ConfigValue defaultValue;
        ConfigSource source = ConfigSource::Template;
        std::vector<std::unique_ptr<ValidationRule>> validationRules;
        std::string description;
    };
    
    ConfigTemplate(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    void addEntry(const std::string& key, const ConfigValue& defaultValue,
                  const std::string& description = "") {
        TemplateEntry entry;
        entry.key = key;
        entry.defaultValue = defaultValue;
        entry.description = description;
        entries_.push_back(std::move(entry));
    }
    
    void addValidation(const std::string& key, std::unique_ptr<ValidationRule> rule) {
        for (auto& entry : entries_) {
            if (entry.key == key) {
                entry.validationRules.push_back(std::move(rule));
                break;
            }
        }
    }
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::vector<TemplateEntry>& getEntries() const { return entries_; }
    
    bool validateEntry(const std::string& key, const ConfigValue& value) const {
        for (const auto& entry : entries_) {
            if (entry.key == key) {
                for (const auto& rule : entry.validationRules) {
                    if (!rule->validate(value)) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<TemplateEntry> entries_;
};

// Configuration preset
class ConfigPreset {
public:
    ConfigPreset(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    void addConfig(const std::string& key, const ConfigValue& value,
                   ConfigSource source = ConfigSource::Preset) {
        config_[key] = {key, value, source, ""};
    }
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::unordered_map<std::string, ConfigEntry>& getConfig() const { return config_; }
    
    bool validate(const Config& cfg) const {
        for (const auto& pair : config_) {
            auto val = cfg.get<std::string>(pair.first);
            if (!val.has_value()) {
                return false;
            }
        }
        return true;
    }
    
private:
    std::string name_;
    std::string description_;
    std::unordered_map<std::string, ConfigEntry> config_;
};

// Enhanced configuration entry with validation
struct EnhancedConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    bool locked = false;
    
    EnhancedConfigEntry() : key(), value(), source(ConfigSource::Default), 
                           description(), timestamp(std::chrono::system_clock::now()) {}
    
    EnhancedConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                       const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::system_clock::now()) {}
    
    bool isValid(const std::vector<std::unique_ptr<ValidationRule>>& rules) const {
        for (const auto& rule : rules) {
            if (!rule->validate(value)) {
                return false;
            }
        }
        return true;
    }
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
    
    // Enhanced loading from file (JSON/YAML support)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    
    // Enhanced getters with type safety and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced setters with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation methods
    bool validate() const;
    bool validate(const std::string& key) const;
    void addValidationRule(const std::string& key, std::unique_ptr<ValidationRule> rule);
    void clearValidationRules(const std::string& key = "");
    
    // Configuration templates and presets
    void addTemplate(const ConfigTemplate& template_);
    void addPreset(const ConfigPreset& preset);
    bool applyTemplate(const std::string& templateName);
    bool applyPreset(const std::string& presetName);
    
    // Configuration inheritance
    void inheritFrom(const Config& parentConfig);
    void setInheritanceEnabled(bool enabled) { inheritanceEnabled_ = enabled; }
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and metadata
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    void lock(const std::string& key);
    void unlock(const std::string& key);
    bool isLocked(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Enhanced summary
    std::string summary() const;
    std::string summary(const std::string& key) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    bool operator==(const Config& other) const { return equals(other); }
    bool operator!=(const Config& other) const { return !equals(other); }
    
    // Serialization
    std::string toJSON() const;
    std::string toYAML() const;
    static Config fromJSON(const std::string& json);
    static Config fromYAML(const std::string& yaml);
    
    // Get source of a configuration value
    ConfigSource getSource(const std::string& key) const;
    
    // Get timestamp of when a configuration value was set
    std::chrono::system_clock::time_point getTimestamp(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    bool isTypeValid(const ConfigValue& value, const std::string& typeName) const;
    std::string getTypeName(const ConfigValue& value) const;
};

// Configuration Manager for loading, saving, validation, merging
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Load configuration with error handling
    std::shared_ptr<Config> loadConfig(const std::string& filepath);
    std::shared_ptr<Config> loadConfigFromJSON(const std::string& filepath);
    std::shared_ptr<Config> loadConfigFromYAML(const std::string& filepath);
    
    // Save configuration with error handling
    bool saveConfig(const std::shared_ptr<Config>& config, const std::string& filepath);
    bool saveConfigToJSON(const std::shared_ptr<Config>& config, const std::string& filepath);
    bool saveConfigToYAML(const std::shared_ptr<Config>& config, const std::string& filepath);
    
    // Validate configuration
    bool validateConfig(const std::shared_ptr<Config>& config);
    std::vector<ConfigError> getValidationErrors(const std::shared_ptr<Config>& config);
    
    // Merge configurations
    void mergeConfigs(std::shared_ptr<Config>& target, const std::shared_ptr<Config>& source,
                     ConfigSource sourceType = ConfigSource::Runtime);
    
    // Comparison
    bool compareConfigs(const std::shared_ptr<Config>& config1, 
                       const std::shared_ptr<Config>& config2);
    
    // Get default configurations for different categories
    std::shared_ptr<Config> getDefaultBrainConfig() const;
    std::shared_ptr<Config> getDefaultPlasticityConfig() const;
    std::shared_ptr<Config> getDefaultNeuromodulationConfig() const;
    std::shared_ptr<Config> getDefaultDevelopmentConfig() const;
    std::shared_ptr<Config> getDefaultMemoryConfig() const;
    std::shared_ptr<Config> getDefaultWorldConfig() const;
    std::shared_ptr<Config> getDefaultExperimentConfig() const;
    
    // Presets
    void addPreset(const ConfigPreset& preset);
    std::shared_ptr<Config> createConfigFromPreset(const std::string& presetName);
    
    // Templates
    void addTemplate(const ConfigTemplate& template_);
    std::shared_ptr<Config> createConfigFromTemplate(const std::string& templateName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Error handling utilities
namespace ErrorHandling {
    // Exception wrapper for safe configuration operations
    template<typename Func, typename... Args>
    auto safeExecute(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Exception in safeExecute: " + std::string(e.what()));
            throw;
        }
    }
    
    // Retry mechanism for file operations
    template<typename Func, typename... Args>
    auto retryExecute(Func&& func, int maxRetries, std::chrono::milliseconds delay, Args&&... args)
    -> decltype(func(std::forward<Args>(args)...)) {
        int attempt = 0;
        while (attempt < maxRetries) {
            try {
                return func(std::forward<Args>(args)...);
            } catch (const std::exception& e) {
                attempt++;
                if (attempt >= maxRetries) {
                    throw;
                }
                std::this_thread::sleep_for(delay);
            }
        }
        throw ConfigError("Max retries exceeded");
    }
    
    // Graceful degradation for critical operations
    template<typename Func, typename... Args>
    auto gracefulExecute(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const ConfigError& e) {
            Logger::getGlobal().warning("Configuration error: " + std::string(e.what()) + 
                                       " - Using fallback value");
            // Return a default/fallback value
            throw; // Re-throw after logging
        } catch (const std::exception& e) {
            Logger::getGlobal().critical("System error: " + std::string(e.what()));
            throw;
        }
    }
}

// Convenience functions
std::shared_ptr<Config> createDefaultBrainConfig();
std::shared_ptr<Config> createDefaultPlasticityConfig();
std::shared_ptr<Config> createDefaultNeuromodulationConfig();
std::shared_ptr<Config> createDefaultDevelopmentConfig();
std::shared_ptr<Config> createDefaultMemoryConfig();
std::shared_ptr<Config> createDefaultWorldConfig();
std::shared_ptr<Config> createDefaultExperimentConfig();

} // namespace nlm