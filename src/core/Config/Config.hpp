// NLM Configuration System - Modern JSON/YAML-based Configuration
// This file provides comprehensive configuration management for NLM with:
// - JSON/YAML parsing support
// - Schema validation
// - Multiple preset configurations
// - Enhanced command-line interface support
// - Configuration inheritance and merging
// - Type-safe configuration access

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

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
    Runtime,
    Preset
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Configuration preset definition
struct Preset {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    ConfigSource source;
    
    Preset(const std::string& n, const std::string& desc) 
        : name(n), description(desc), source(ConfigSource::Preset) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "float", "string", "bool", "vector"
    std::string description;
    std::optional<ConfigValue> defaultValue;
    bool required = false;
    std::vector<std::string> allowedValues;
    std::string validationRegex;
    
    // Type conversion
    template<typename T>
    bool convertValue(const std::string& strValue, T& result) const;
};

// Modern configuration class with JSON/YAML support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from multiple sources with precedence
    bool load(const std::string& filePath = "");
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    bool loadFromFile(const std::string& filePath);
    
    // Apply preset configurations
    bool applyPreset(const Preset& preset);
    bool applyPreset(const std::string& presetName);
    
    // Command line interface support
    bool loadFromArgs(int argc, char** argv);
    bool parseCommandLineArgs(const std::vector<std::string>& args);
    
    // Get values with type safety
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    bool has(const std::string& key) const;
    
    // Set values with automatic source tracking
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation and schema
    void addSchema(const ConfigSchema& schema);
    ValidationResult validate() const;
    bool isValid() const;
    
    // Configuration management
    std::vector<std::string> getKeys() const;
    std::map<std::string, std::string> getSummary() const;
    std::string getSourceInfo(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    
    // Save configuration
    bool saveToJSON(const std::string& filePath) const;
    bool saveToYAML(const std::string& filePath) const;
    bool saveToFile(const std::string& filePath) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Preset management
    bool addPreset(const Preset& preset);
    bool removePreset(const std::string& name);
    bool hasPreset(const std::string& name) const;
    std::vector<std::string> getPresetNames() const;
    Preset getPreset(const std::string& name) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Statistics and debugging
    size_t getEntryCount() const;
    size_t getPresetCount() const;
    std::string getVersion() const;
    static std::string getCurrentVersion();
    
    // Environment detection and adaptive configuration
    void detectEnvironment();
    void autoConfigureForMode(const std::string& mode);
    
    // Configuration comparison and diff
    bool equals(const Config& other) const;
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> diff(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void initializeDefaults();
    void applyPresetInternal(const Preset& preset);
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    ConfigValue convertStringToValue(const std::string& type, const std::string& strValue) const;
    std::string valueToString(const ConfigValue& value) const;
    void mergeEntries(const std::map<std::string, ConfigValue>& newEntries, ConfigSource source);
    std::vector<std::string> getAllKeysFromPresets() const;
    ConfigValue getDefaultValue(const std::string& key) const;
    std::string getValueTypeName(const ConfigValue& value) const;
};

// Preset factory methods for easy preset creation
namespace Presets {
    // Educational presets
    Preset createEducational();
    Preset createBeginner();
    Preset createResearch();
    
    // Performance presets
    Preset createHighPerformance();
    Preset createFastSimulation();
    Preset createLargeScale();
    
    // Research presets
    Preset createLongTermLearning();
    Preset createDetailedPlasticity();
    Preset createStructuralPlasticity();
    
    // Development presets
    Preset createDevelopmentalStages();
    Preset createCriticalPeriod();
    Preset createMaturationPhase();
    
    // Application-specific presets
    Preset createNeuronalDevelopment();
    Preset createLearningExperiment();
    Preset createBehaviorSimulation();
    
    // Environment-aware presets
    Preset createLowMemory();
    Preset createRealTime();
    Preset createHighAccuracy();
}

// Command-line argument parser
class CommandLineParser {
public:
    struct Option {
        std::string name;
        char shortOpt;
        std::string description;
        std::string defaultValue;
        bool hasValue;
        bool required;
        
        Option(const std::string& n, char s, const std::string& desc, 
               const std::string& def = "", bool val = false, bool req = false)
            : name(n), shortOpt(s), description(desc), defaultValue(def), 
              hasValue(val), required(req) {}
    };
    
    CommandLineParser();
    ~CommandLineParser();
    
    // Add options
    void addOption(const Option& option);
    void addOption(const std::string& name, char shortOpt, 
                   const std::string& description, const std::string& defaultValue = "",
                   bool hasValue = false, bool required = false);
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    bool parse(const std::vector<std::string>& args);
    
    // Get parsed values
    std::optional<std::string> getValue(const std::string& name) const;
    std::optional<std::string> getValue(char shortOpt) const;
    
    // Help and usage
    std::string getHelp() const;
    bool hasHelpRequested() const;
    bool isValid() const;
    
    // Validation
    ValidationResult validate() const;
    
    // Reset
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void setupDefaultOptions();
    std::string buildUsageString() const;
    std::string buildOptionString() const;
};

// Configuration factory for creating configurations from strings/files
class ConfigFactory {
public:
    // Create configuration from JSON string
    static Config fromJSON(const std::string& jsonContent);
    
    // Create configuration from YAML string
    static Config fromYAML(const std::string& yamlContent);
    
    // Create configuration from file (auto-detect format)
    static Config fromFile(const std::string& filePath);
    
    // Create configuration from command line arguments
    static Config fromCommandLine(int argc, char** argv);
    
    // Create configuration with preset applied
    static Config withPreset(const std::string& presetName);
    static Config withPreset(const Preset& preset);
    
    // Create configuration from environment variables
    static Config fromEnvironment(const std::map<std::string, std::string>& envVars);
    
    // Create minimal configuration for testing
    static Config createMinimal();
    
    // Create configuration for specific use case
    static Config createForUseCase(const std::string& useCase);
};

// Configuration merger for merging multiple configuration sources
class ConfigMerger {
public:
    struct MergeResult {
        Config mergedConfig;
        std::vector<std::string> mergeLog;
        bool success;
        std::vector<std::string> errors;
    };
    
    static MergeResult merge(const std::vector<Config>& configs,
                            const std::vector<ConfigSource>& sources = {});
    
    static MergeResult mergeWithPreset(const Config& base,
                                      const std::string& presetName);
    
    static MergeResult mergeCommandLine(const Config& base,
                                       const std::vector<std::string>& args);
};

// Configuration validator for comprehensive validation
class ConfigValidator {
public:
    struct ValidationOptions {
        bool validateTypes = true;
        bool validateRanges = true;
        bool validateDependencies = true;
        bool validatePresets = true;
        std::string customSchemaPath;
        std::function<bool(const std::string&, const ConfigValue&)> customValidator;
    };
    
    static ValidationResult validate(const Config& config, 
                                     const ValidationOptions& options = ValidationOptions());
    
    static ValidationResult validateAgainstSchema(const Config& config,
                                                 const std::string& schemaFile);
    
    static ValidationResult validatePresetDependencies(const Config& config);
    
    static std::string getDefaultSchema();
};

// Configuration error types for better error handling
class ConfigError : public std::runtime_error {
public:
    enum class ErrorType {
        InvalidFileFormat,
        MissingRequiredValue,
        InvalidValue,
        ValidationError,
        PresetNotFound,
        CommandLineError,
        MergeError
    };
    
    ConfigError(ErrorType type, const std::string& message);
    ErrorType getType() const { return errorType; }
    const std::string& getDetails() const { return details; }
    
private:
    ErrorType errorType;
    std::string details;
};

// Configuration change notification system
class ConfigChangeNotifier {
public:
    struct Change {
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        ConfigSource source;
        std::chrono::system_clock::time_point timestamp;
        std::string description;
    };
    
    using ChangeCallback = std::function<void(const Change& change)>;
    
    ConfigChangeNotifier();
    ~ConfigChangeNotifier();
    
    void addListener(const std::string& key, const ChangeCallback& callback);
    void removeListener(const std::string& key);
    void removeAllListeners();
    
    void notifyChange(const std::string& key, const ConfigValue& oldValue,
                     const ConfigValue& newValue, ConfigSource source,
                     const std::string& description = "");
    
    std::vector<Change> getChanges() const;
    void clearChanges();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration snapshot and history management
class ConfigHistory {
public:
    struct Snapshot {
        std::chrono::system_clock::time_point timestamp;
        Config config;
        std::string description;
    };
    
    ConfigHistory();
    ~ConfigHistory();
    
    void takeSnapshot(const std::string& description = "");
    std::vector<Snapshot> getSnapshots() const;
    
    Snapshot restoreSnapshot(size_t index);
    bool removeSnapshot(size_t index);
    bool clearSnapshots();
    
    Config getCurrentConfig() const;
    
    // Merge snapshots
    Config mergeSnapshots(const std::vector<size_t>& indices) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global configuration manager
class GlobalConfig {
public:
    static GlobalConfig& getInstance();
    
    // Set the global configuration
    void setConfig(std::shared_ptr<Config> config);
    std::shared_ptr<Config> getConfig();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filePath);
    bool loadFromCommandLine(int argc, char** argv);
    
    // Get the global configuration (convenience method)
    static std::shared_ptr<Config> get();
    
    // Set global configuration (convenience method)
    static void set(std::shared_ptr<Config> config);
    
    // Get default configuration
    static Config getDefaultConfig();
    
    // Clear configuration
    static void clear();
    
private:
    GlobalConfig();
    ~GlobalConfig();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm