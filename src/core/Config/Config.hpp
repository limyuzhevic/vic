/**
 * Configuration Manager for NLM
 * 
 * A comprehensive configuration system that provides:
 * - Hierarchical configuration values
 * - Type-safe accessors
 * - File I/O support
 * - Command-line override support
 * - Validation and defaults
 * - Advanced features for power users
 */

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace nlm {

/**
 * Configuration value types
 */
enum class ConfigValueType {
    Int,
    Float,
    String,
    Bool,
    VectorFloat,
    VectorString
};

/**
 * Configuration source tracking
 */
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

/**
 * Base configuration value
 */
class ConfigValue {
public:
    virtual ~ConfigValue() = default;
    
    virtual ConfigValueType getType() const = 0;
    virtual std::string toString() const = 0;
    virtual ConfigSource getSource() const { return ConfigSource::Default; }
    virtual void setSource(ConfigSource source) {}
};

/**
 * Typed configuration value
 */
template<typename T>
class TypedConfigValue : public ConfigValue {
public:
    TypedConfigValue(T value, ConfigSource source = ConfigSource::Default)
        : value_(value), source_(source) {}
    
    T getValue() const { return value_; }
    void setValue(T value) { value_ = value; }
    
    ConfigValueType getType() const override { return getConfigType(); }
    std::string toString() const override {
        std::ostringstream oss;
        oss << value_;
        return oss.str();
    }
    
    ConfigSource getSource() const override { return source_; }
    void setSource(ConfigSource source) override { source_ = source; }
    
protected:
    virtual ConfigValueType getConfigType() const = 0;
    
private:
    T value_;
    ConfigSource source_;
};

/**
 * Specialized type implementations
 */
class IntConfigValue : public TypedConfigValue<int64_t> {
public:
    IntConfigValue(int64_t value, ConfigSource source = ConfigSource::Default)
        : TypedConfigValue(value, source) {}
    
protected:
    ConfigValueType getConfigType() const override { return ConfigValueType::Int; }
};

class FloatConfigValue : public TypedConfigValue<float> {
public:
    FloatConfigValue(float value, ConfigSource source = ConfigSource::Default)
        : TypedConfigValue(value, source) {}
    
protected:
    ConfigValueType getConfigType() const override { return ConfigValueType::Float; }
};

class StringConfigValue : public TypedConfigValue<std::string> {
public:
    StringConfigValue(const std::string& value, ConfigSource source = ConfigSource::Default)
        : TypedConfigValue(value, source) {}
    
protected:
    ConfigValueType getConfigType() const override { return ConfigValueType::String; }
};

class BoolConfigValue : public TypedConfigValue<bool> {
public:
    BoolConfigValue(bool value, ConfigSource source = ConfigSource::Default)
        : TypedConfigValue(value, source) {}
    
protected:
    ConfigValueType getConfigType() const override { return ConfigValueType::Bool; }
};

class VectorFloatConfigValue : public TypedConfigValue<std::vector<float>> {
public:
    VectorFloatConfigValue(const std::vector<float>& value, ConfigSource source = ConfigSource::Default)
        : TypedConfigValue(value, source) {}
    
protected:
    ConfigValueType getConfigType() const override { return ConfigValueType::VectorFloat; }
};

class VectorStringConfigValue : public TypedConfigValue<std::vector<std::string>> {
public:
    VectorStringConfigValue(const std::vector<std::string>& value, ConfigSource source = ConfigSource::Default)
        : TypedConfigValue(value, source) {}
    
protected:
    ConfigValueType getConfigType() const override { return ConfigValueType::VectorString; }
};

/**
 * Advanced configuration manager
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Enable moving
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * Set a configuration value
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    void set(const std::string& key, const std::vector<float>& value, ConfigSource source = ConfigSource::Runtime);
    
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Get a configuration value with type-safe accessors
     */
    int64_t getOr(const std::string& key, int64_t defaultValue) const;
    
    float getOr(const std::string& key, float defaultValue) const;
    
    std::string getOr(const std::string& key, const std::string& defaultValue) const;
    
    bool getOr(const std::string& key, bool defaultValue) const;
    
    std::vector<float> getOr(const std::string& key, const std::vector<float>& defaultValue) const;
    
    std::vector<std::string> getOr(const std::string& key, const std::vector<std::string>& defaultValue) const;
    
    /**
     * Check if a key exists
     */
    bool exists(const std::string& key) const;
    
    /**
     * Get all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * Load configuration from file
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * Load configuration from command line arguments
     */
    bool loadFromArgs(int argc, char* argv[]);
    
    /**
     * Get configuration value type
     */
    ConfigValueType getType(const std::string& key) const;
    
    /**
     * Export configuration to string
     */
    std::string exportConfig() const;
    
    /**
     * Import configuration from string
     */
    bool importConfig(const std::string& configString);
    
    /**
     * Configuration validation
     */
    bool validate() const;
    
    /**
     * Reset to defaults
     */
    void resetToDefaults();
    
    /**
     * Get configuration statistics
     */
    struct ConfigStats {
        size_t totalKeys;
        size_t fileLoadedKeys;
        size_t commandLineKeys;
        size_t runtimeKeys;
        double memoryUsage;
    };
    
    ConfigStats getStats() const;
    
    /**
     * Advanced features for power users
     */
    
    /**
     * Get raw value pointer (for advanced users)
     */
    const ConfigValue* getRawValue(const std::string& key) const;
    
    /**
     * Set default values programmatically
     */
    void setDefaults(const std::unordered_map<std::string, std::string>& defaults);
    
    /**
     * Get configuration value with interpolation
     */
    std::string getWithInterpolation(const std::string& key) const;
    
    /**
     * Watch for configuration changes
     */
    class ConfigChangeListener {
    public:
        virtual ~ConfigChangeListener() = default;
        virtual void onConfigChanged(const std::string& key, const ConfigValue& value) = 0;
    };
    
    void addChangeListener(ConfigChangeListener* listener);
    void removeChangeListener(ConfigChangeListener* listener);
    
    /**
     * Configuration presets
     */
    static Config createPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    /**
     * Configuration validation rules
     */
    class ValidationRule {
    public:
        virtual ~ValidationRule() = default;
        virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
        virtual std::string getErrorMessage() const = 0;
    };
    
    void addValidationRule(const ValidationRule* rule);
    void removeValidationRule(const ValidationRule* rule);
    
private:
    struct Impl;
    Impl* pImpl;
    
    void notifyChangeListeners(const std::string& key, const ConfigValue& value);
};

} // namespace nlm

// Implementation is in Config.cpp