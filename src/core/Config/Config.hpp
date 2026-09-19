#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <shared_mutex>
#include "ConfigSchema.hpp"
#include "ConfigValidator.hpp"
#include "ConfigWatcher.hpp"
#include "ConfigMigration.hpp"
#include "ConfigValidationUtils.hpp"
#include "ConfigHelpSystem.hpp"
#include "ConfigDocumentation.hpp"

namespace nlm {

// Forward declarations
class ConfigChangeListener;
class ConfigWatcher;

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

// Configuration error types
enum class ConfigErrorType {
    TypeMismatch,
    RangeViolation,
    MissingDependency,
    DependencyCycle,
    SchemaValidation,
    MigrationError,
    ImportExportError
};

// Configuration error details
struct ConfigError {
    ConfigErrorType type;
    std::string key;
    std::string expected;
    std::string actual;
    std::string message;
    
    ConfigError(ConfigErrorType t, const std::string& k, const std::string& exp, const std::string& act, const std::string& msg)
        : type(t), key(k), expected(exp), actual(act), message(msg) {}
};

// Parameter types for schema definition
enum class ParameterType {
    Int,
    Int64,
    Float,
    Double,
    Bool,
    String,
    StringList,
    IntList,
    FloatList,
    Choice,
    Range,
    Vector2D,
    Vector3D,
    Matrix,
    Custom
};

// Range specification
struct RangeSpec {
    std::string min;
    std::string max;
    bool inclusive = true;
    
    RangeSpec(const std::string& min_val, const std::string& max_val, bool inc = true)
        : min(min_val), max(max_val), inclusive(inc) {}
};

// Choice specification
struct ChoiceSpec {
    std::vector<std::string> choices;
    
    ChoiceSpec(const std::vector<std::string>& c) : choices(c) {}
};

// Vector dimensions
struct VectorSpec {
    int dimensions;
    std::vector<RangeSpec> componentRanges;
    
    VectorSpec(int dim, const std::vector<RangeSpec>& ranges) : dimensions(dim), componentRanges(ranges) {}
};

// Matrix specification
struct MatrixSpec {
    int rows;
    int cols;
    std::vector<RangeSpec> valueRanges;
    
    MatrixSpec(int r, int c, const std::vector<RangeSpec>& ranges) : rows(r), cols(c), valueRanges(ranges) {}
};

// Parameter metadata for documentation
struct ParameterInfo {
    std::string name;
    std::string description;
    ParameterType type;
    std::optional<RangeSpec> range;
    std::optional<ChoiceSpec> choice;
    std::optional<VectorSpec> vector;
    std::optional<MatrixSpec> matrix;
    std::optional<ParameterInfo> dependency;
    std::vector<std::string> tags;
    bool required = false;
    bool readOnly = false;
    std::string defaultValue;
    std::string category;
    int order = 0;
    std::string unit;
    
    ParameterInfo(const std::string& n, const std::string& desc = "", ParameterType t = ParameterType::String)
        : name(n), description(desc), type(t) {}
};

// Configuration change listener
class ConfigChangeListener {
public:
    virtual ~ConfigChangeListener() = default;
    virtual void onConfigChanged(const std::string& key, const std::any& oldValue, const std::any& newValue) = 0;
    virtual void onConfigAdded(const std::string& key, const std::any& value) = 0;
    virtual void onConfigRemoved(const std::string& key) = 0;
    virtual void onValidationFailed(const ConfigError& error) = 0;
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Export/Import methods
    bool exportToYAML(const std::string& filepath) const;
    bool importFromYAML(const std::string& filepath);
    bool exportToJSON(const std::string& filepath) const;
    bool importFromJSON(const std::string& filepath);
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Advanced validation and schema methods
    void setSchemaEnabled(bool enabled);
    void setValidationEnabled(bool enabled);
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema* getSchema() const;
    
    // Configuration inheritance
    void setParentConfig(const Config& parent);
    
    // Change notification system
    void addChangeListener(std::shared_ptr<ConfigChangeListener> listener);
    void removeChangeListener(std::shared_ptr<ConfigChangeListener> listener);
    void setWatchInterval(std::chrono::milliseconds interval);
    
    // Configuration presets
    void loadQuickPreset();
    void loadBalancedPreset();
    void loadResearchPreset();
    void loadMinimalPreset();
    void loadMaximumPreset();
    
    // Migration support
    void addMigrationStep(int fromVersion, int toVersion, const std::string& description,
                         const std::function<void(std::map<std::string, std::any>&)>& migrationLogic);
    bool applyMigrations();
    int getMigrationVersion() const;
    void setMigrationVersion(int version);
    
    // Validation
    std::vector<ConfigError> validate() const;
    
    // Documentation and help
    std::string getDocumentation() const;
    std::string getHelp(const std::string& topic) const;
    
    // Configuration analysis
    std::vector<std::string> getConflictingParameters() const;
    std::vector<std::string> getDependencyCycles() const;
    
    // Error reporting
    std::string reportValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper methods
    void validateSchema() const;
    void validateParameters() const;
    bool isValueValid(const std::any& value, const ParameterInfo& param) const;
    std::any convertToAny(const ConfigValue& value) const;
    ConfigValue convertFromAny(const std::any& value) const;
    void notifyConfigChange(const std::string& key, const std::any& oldValue, const std::any& newValue);
};

} // namespace nlm