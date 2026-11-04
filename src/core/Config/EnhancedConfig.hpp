#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <set>
#include <chrono>
#include <ctime>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<float>,
    std::vector<bool>,
    std::vector<std::string>,
    std::vector<std::vector<double>> // Matrix support for neural parameters
>;

// Enhanced configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment // For environment variable loading
};

// Configuration metadata for validation and documentation
struct ConfigMetadata {
    std::string description;
    std::string category; // e.g., "simulation", "plasticity", "neuromodulation"
    std::string unit;     // e.g., "s", "Hz", "mV"
    double minValue;      // For numeric types
    double maxValue;      // For numeric types
    std::vector<std::string> allowedValues; // For string/enum types
    bool required;        // Whether this config must be set
    std::string defaultValue; // Human-readable default
    
    ConfigMetadata()
        : description(""), category("general"), unit(""), minValue(std::numeric_limits<double>::lowest()),
          maxValue(std::numeric_limits<double>::max()), required(false), defaultValue("") {}
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigMetadata metadata;
    std::chrono::system_clock::time_point lastModified;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   lastModified(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", 
                const ConfigMetadata& meta = ConfigMetadata())
        : key(k), value(v), source(s), description(desc), metadata(meta),
          lastModified(std::chrono::system_clock::now()) {}
};

// Configuration change callback for reactive updates
class ConfigChangeCallback {
public:
    virtual ~ConfigChangeCallback() = default;
    virtual void onConfigChanged(const std::string& key, const ConfigValue& newValue, const ConfigValue& oldValue) = 0;
    virtual void onConfigAdded(const std::string& key, const ConfigValue& value) = 0;
    virtual void onConfigRemoved(const std::string& key) = 0;
};

// Enhanced configuration with validation, change tracking, and callbacks
class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig();
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Configuration validation
    template<typename T>
    bool setValidated(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime,
                     const std::string& description = "", const ConfigMetadata& metadata = ConfigMetadata());
    
    // Configuration validation with existing value preservation
    template<typename T>
    bool updateIfChanged(const std::string& key, const T& newValue, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key) const;
    
    // Type-safe getters with conversion
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Vector getters for neural parameters
    template<typename T>
    std::vector<T> getVectorOr(const std::string& key, const std::vector<T>& defaultValue) const;
    
    // Matrix getters for neural parameter matrices
    template<typename T>
    std::vector<std::vector<T>> getMatrixOr(const std::string& key, const std::vector<std::vector<T>>& defaultValue) const;
    
    // Configuration existence and removal
    bool has(const std::string& key) const;
    bool remove(const std::string& key);
    
    // Batch operations
    void setBatch(const std::map<std::string, ConfigValue>& batch, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation methods
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Source management
    ConfigSource getSource(const std::string& key) const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Metadata management
    void setMetadata(const std::string& key, const ConfigMetadata& metadata);
    ConfigMetadata getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    
    // Configuration export/import
    std::string exportToString() const;
    bool importFromString(const std::string& configString, ConfigSource source = ConfigSource::File);
    
    // Configuration summaries and statistics
    std::string summary() const;
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    
    // Callback management
    void addChangeCallback(std::shared_ptr<ConfigChangeCallback> callback);
    void removeChangeCallback(const std::shared_ptr<ConfigChangeCallback>& callback);
    
    // Time series support for neural dynamics
    struct TimeSeriesPoint {
        std::chrono::system_clock::time_point timestamp;
        ConfigValue value;
        std::string context; // Optional context for the value
    };
    
    void recordTimeSeries(const std::string& key, const ConfigValue& value, const std::string& context = "");
    std::vector<TimeSeriesPoint> getTimeSeries(const std::string& key) const;
    void clearTimeSeries(const std::string& key);
    
    // Configuration comparison
    bool equals(const EnhancedConfig& other) const;
    std::vector<std::string> getDifferences(const EnhancedConfig& other) const;
    
    // Performance optimizations
    void optimizeForPerformance(); // Pre-compiles validation, sorts entries, etc.
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void notifyChange(const std::string& key, const ConfigValue& newValue, const ConfigValue& oldValue);
    void notifyAddition(const std::string& key, const ConfigValue& value);
    void notifyRemoval(const std::string& key);
    
    // Validation helpers
    bool validateValue(const ConfigEntry& entry) const;
    std::string getValidationError(const ConfigEntry& entry) const;
};

// Command-line argument parser with enhanced features
class CommandLineParser {
public:
    struct Argument {
        std::string key;
        std::string value;
        std::string shorthand;
        std::string description;
        std::string type; // "bool", "int", "float", "string", "vector"
        std::string category;
        bool required;
        std::string defaultValue;
        
        Argument() : required(false) {}
        Argument(const std::string& k, const std::string& v, const std::string& d = "")
            : key(k), value(v), description(d), required(false) {}
    };
    
    CommandLineParser();
    ~CommandLineParser();
    
    // Add argument definitions
    void addArgument(const std::string& key, const std::string& shorthand = "", const std::string& description = "",
                    const std::string& type = "string", const std::string& category = "general",
                    bool required = false, const std::string& defaultValue = "");
    
    // Add boolean flag
    void addFlag(const std::string& key, const std::string& description = "", const std::string& category = "general");
    
    // Add vector argument (e.g., --matrix 1,2,3,4)
    void addVectorArgument(const std::string& key, const std::string& description = "",
                          const std::string& category = "general", size_t minSize = 0, size_t maxSize = 0);
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed values
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::vector<std::string> getStringVector(const std::string& key) const;
    std::vector<int> getIntVector(const std::string& key) const;
    std::vector<double> getDoubleVector(const std::string& key) const;
    
    // Type-safe getters with defaults
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Help and documentation
    void printHelp(const std::string& programName) const;
    std::string getUsageSummary() const;
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration integration
    void applyToConfig(EnhancedConfig& config) const;
    
    // Environment variable support
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    bool parseArgument(const std::string& arg);
    std::vector<std::string> split(const std::string& str, char delimiter) const;
    std::string trim(const std::string& str) const;
    bool stringToBool(const std::string& str) const;
    std::string boolToString(bool value) const;
};

// Configuration file utilities with JSON/YAML support
class ConfigFileUtils {
public:
    // File format detection
    enum class FileFormat {
        Unknown,
        Custom, // NLM's key=value format
        JSON,
        YAML,
        INI,
        TOML
    };
    
    static FileFormat detectFormat(const std::string& filepath);
    
    // File loading
    static bool loadConfig(const std::string& filepath, EnhancedConfig& config);
    static bool saveConfig(const std::string& filepath, const EnhancedConfig& config, FileFormat format = FileFormat::Custom);
    
    // Format-specific loaders
    static bool loadFromCustomFormat(const std::string& content, EnhancedConfig& config);
    static bool loadFromJSON(const std::string& content, EnhancedConfig& config);
    static bool loadFromYAML(const std::string& content, EnhancedConfig& config);
    static bool loadFromINI(const std::string& content, EnhancedConfig& config);
    static bool loadFromTOML(const std::string& content, EnhancedConfig& config);
    
    // Format-specific savers
    static std::string saveAsCustomFormat(const EnhancedConfig& config);
    static std::string saveAsJSON(const EnhancedConfig& config);
    static std::string saveAsYAML(const EnhancedConfig& config);
    static std::string saveAsINI(const EnhancedConfig& config);
    static std::string saveAsTOML(const EnhancedConfig& config);
    
    // Configuration validation
    static bool validateFile(const std::string& filepath, FileFormat format = FileFormat::Unknown);
    static std::vector<std::string> getFileValidationErrors(const std::string& filepath, FileFormat format = FileFormat::Unknown);
    
    // Configuration merging
    static bool mergeConfigs(const std::string& baseFile, const std::string& overrideFile, 
                            const std::string& outputFile, FileFormat format = FileFormat::Unknown);
    
    // Template configuration generation
    static std::string generateTemplate(const std::string& templateName = "default");
    
    // Validation of neural-specific parameters
    static bool validateNeuralParameters(const EnhancedConfig& config, std::vector<std::string>& errors);
};

// Configuration presets for common use cases
class ConfigPresets {
public:
    // Neural network presets
    static EnhancedConfig getPresetPhase2Minimal();
    static EnhancedConfig getPresetPhase2Full();
    static EnhancedConfig getPresetPhase3Exploration();
    static EnhancedConfig getPresetPhase4Learning();
    static EnhancedConfig getPresetPhase6Integration();
    
    // Application presets
    static EnhancedConfig getPresetDevelopment();
    static EnhancedConfig getPresetPerformance();
    static EnhancedConfig getPresetBenchmark();
    static EnhancedConfig getPresetDemo();
    
    // Environment-specific presets
    static EnhancedConfig getPresetDebug();
    static EnhancedConfig getPresetProduction();
    static EnhancedConfig getPresetContainer();
    
    // Hardware-specific presets
    static EnhancedConfig getPresetLowMemory();
    static EnhancedConfig getPresetHighPerformance();
    static EnhancedConfig getPresetEmbedded();
    
    // Research presets
    static EnhancedConfig getPresetReconstructive();
    static EnhancedConfig getPresetComparative();
    static EnhancedConfig getPresetValidation();
};

// Type conversion utilities for configuration
class ConfigConverters {
public:
    // String to type conversion
    static bool stringToBool(const std::string& str, bool& value);
    static bool stringToInt(const std::string& str, int64_t& value);
    static bool stringToDouble(const std::string& str, double& value);
    static bool stringToFloat(const std::string& str, float& value);
    
    // Vector/string conversion for neural parameters
    static bool stringToVector(const std::string& str, std::vector<std::string>& values, char delimiter = ',');
    static bool stringToIntVector(const std::string& str, std::vector<int64_t>& values, char delimiter = ',');
    static bool stringToDoubleVector(const std::string& str, std::vector<double>& values, char delimiter = ',');
    static bool stringToFloatVector(const std::string& str, std::vector<float>& values, char delimiter = ',');
    
    // Matrix conversion
    static bool stringToMatrix(const std::string& str, std::vector<std::vector<double>>& matrix, char rowDelimiter = ';', char colDelimiter = ',');
    
    // Neural parameter validation
    static bool validateConnectionProbability(double value, std::vector<std::string>& errors);
    static bool validateNeuronCount(int64_t value, std::vector<std::string>& errors);
    static bool validateTimeStep(double value, std::vector<std::string>& errors);
    
    // Unit conversion for neural parameters
    static double convertUnit(double value, const std::string& fromUnit, const std::string& toUnit);
    
    // Statistics and utilities
    static std::string formatValue(const ConfigValue& value);
    static std::string formatBytes(size_t bytes);
    static std::string formatTime(double seconds);
    static std::string formatRate(double rate, const std::string& unit = "Hz");
};

} // namespace nlm