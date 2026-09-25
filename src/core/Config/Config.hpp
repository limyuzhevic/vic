#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <set>
#include <unordered_set>
#include <limits>

using json = nlohmann::json;

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

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
    Template
};

// Configuration validation error
struct ConfigValidationError {
    std::string key;
    std::string message;
    std::string code;
    bool isCritical;
    
    ConfigValidationError() : key(), message(), code(), isCritical(false) {}
    ConfigValidationError(const std::string& k, const std::string& msg, const std::string& c, bool critical = false)
        : key(k), message(msg), code(c), isCritical(critical) {}
};

// Configuration statistics
struct ConfigStatistics {
    size_t totalEntries;
    size_t runtimeEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t defaultEntries;
    double validationPassRate;
    uint64_t lastAccessTime;
    uint64_t lastModifiedTime;
    
    ConfigStatistics() : totalEntries(0), runtimeEntries(0), fileEntries(0), 
                        commandLineEntries(0), defaultEntries(0), 
                        validationPassRate(1.0), lastAccessTime(0), lastModifiedTime(0) {}
};

// Configuration comparison result
struct ConfigDiff {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource oldSource;
    ConfigSource newSource;
    std::string operation; // "added", "removed", "modified"
    
    ConfigDiff() : key(), operation("unknown") {}
    ConfigDiff(const std::string& k, const ConfigValue& oldVal, const ConfigValue& newVal,
               ConfigSource oldSrc, ConfigSource newSrc, const std::string& op)
        : key(k), oldValue(oldVal), newValue(newVal), oldSource(oldSrc), 
          newSource(newSrc), operation(op) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    bool isReadOnly;
    std::chrono::system_clock::time_point lastModified;
    std::map<std::string, std::string> metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   category(), isReadOnly(false), lastModified(), metadata() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "", bool readonly = false)
        : key(k), value(v), source(s), description(desc), category(cat), 
          isReadOnly(readonly), lastModified(std::chrono::system_clock::now()), metadata() {}
};

// Configuration schema definition
struct ConfigSchemaField {
    std::string name;
    std::string type;
    bool required;
    ConfigValue defaultValue;
    std::string description;
    std::string category;
    std::vector<ConfigValue> allowedValues;
    double minValue;
    double maxValue;
    std::string validationRule;
    
    ConfigSchemaField() : name(), type(), required(false), defaultValue(), description(), 
                         category(), minValue(std::numeric_limits<double>::lowest()),
                         maxValue(std::numeric_limits<double>::max()) {}
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::vector<ConfigSchemaField> schema;
    std::map<std::string, ConfigValue> values;
    std::string category; // development, research, robotics, etc.
    
    ConfigTemplate() : name(), description(), category() {}
};

// Configuration change event
struct ConfigChangeEvent {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigChangeEvent() : key(), oldValue(), newValue(), source(), timestamp() {}
};

// Main configuration class with advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // ===== BASIC OPERATIONS =====
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool validateSchema = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validateSchema = true);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool includeDefaults = false) const;
    
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
    
    // ===== ADVANCED CONFIGURATION APIs =====
    
    // Configuration validation
    bool validateSchema() const;
    std::vector<ConfigValidationError> getErrors() const;
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    // Configuration comparison
    std::vector<ConfigDiff> getDiff(const Config& other) const;
    std::vector<ConfigDiff> getSimilar(const Config& other, double threshold = 0.8) const;
    
    // Configuration export/import
    bool exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath, bool merge = true);
    
    // Configuration monitoring
    ConfigStatistics getStatistics() const;
    std::map<std::string, double> getMetrics() const;
    
    // Configuration hot-reload
    bool reloadFromFile(const std::string& filepath);
    void watchForChanges(std::function<void(const std::vector<ConfigDiff>&)> callback, 
                         std::chrono::milliseconds interval = std::chrono::milliseconds(1000));
    
    // ===== ADVANCED FEATURE SETUPS =====
    
    // Memory system configuration
    void setupMemoryConfig(double workingMemoryCapacity = 100.0, 
                          double episodicConsolidationRate = 0.1,
                          double associativeStrengthDecay = 0.95,
                          double episodicDecayRate = 0.99);
    
    // Neuromodulation configuration
    void setupNeuromodulationConfig(double dopamineBaseline = 0.5,
                                   double dopamineNoise = 0.1,
                                   double noveltyThreshold = 0.7,
                                   double curiosityLevel = 0.8,
                                   double explorationRate = 0.2);
    
    // Cognitive system configuration
    void setupCognitiveConfig(double planningDepth = 3,
                             double conceptFormationThreshold = 0.6,
                             double attentionDecayRate = 0.98,
                             double workingMemoryDecayRate = 0.95,
                             double activationThreshold = 0.1);
    
    // Prediction system configuration
    void setupPredictionConfig(double confidenceThreshold = 0.8,
                              double learningRate = 0.01,
                              double predictionDecayRate = 0.99,
                              double predictionNoise = 0.05);
    
    // Development configuration
    void setupDevelopmentConfig(double plasticityRate = 0.3,
                               double developmentalStage = 1.0,
                               double explorationPhaseDuration = 100.0,
                               double learningPhaseDuration = 1000.0,
                               bool enablePhaseTransitions = true);
    
    // System integration configuration
    void setupIntegrationConfig(double corticalCorticalCoupling = 0.7,
                               double corticalSubcorticalCoupling = 0.6,
                               double feedbackStrength = 0.8,
                               double predictionErrorGain = 1.0,
                               double learningRate = 0.01);
    
    // ===== TEMPLATE MANAGEMENT =====
    
    // Template application
    bool applyTemplate(const ConfigTemplate& template);
    bool applyTemplate(const std::string& templateName);
    
    // Template management
    bool registerTemplate(const ConfigTemplate& template);
    bool unregisterTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    ConfigTemplate getTemplate(const std::string& name) const;
    
    // Preset templates
    static ConfigTemplate createDevelopmentTemplate();
    static ConfigTemplate createResearchTemplate();
    static ConfigTemplate createPerformanceTemplate();
    static ConfigTemplate createRoboticsTemplate();
    static ConfigTemplate createExplorationTemplate();
    static ConfigTemplate createDebugTemplate();
    
    // ===== CHANGE NOTIFICATION =====
    
    // Event handling
    void addChangeListener(std::function<void(const ConfigChangeEvent&)> listener);
    void removeChangeListener(std::function<void(const ConfigChangeEvent&)>
    void clearChangeListeners();
    
    // ===== MAINTENANCE =====
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summaryByCategory(const std::string& category) const;
    
    // Schema management
    void setSchema(const std::vector<ConfigSchemaField>& schema);
    std::vector<ConfigSchemaField> getSchema() const;
    bool isValidSchema() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions for JSON serialization
static json configValueToJson(const ConfigValue& value);
static ConfigValue jsonToConfigValue(const json& j, const std::string& typeHint = "");

// Helper functions for validation
static bool validateValue(const ConfigValue& value, const ConfigSchemaField& field);
static bool validateConstraints(const ConfigValue& value, const ConfigSchemaField& field);
static bool valueInRange(const ConfigValue& value, double min, double max);
static std::string getTypeString(const ConfigValue& value);
static std::string configValueToString(const ConfigValue& value);
static bool isVariantEqual(const ConfigValue& a, const ConfigValue& b);

// JSON serialization helpers
static json entryToJson(const ConfigEntry& entry);
static ConfigEntry jsonToEntry(const json& j);

// Configuration comparison helpers
static bool compareConfigEntries(const ConfigEntry& a, const ConfigEntry& b);

// Debug and utility functions
static std::string getCurrentTimestamp();
};

} // namespace nlm

#include "Config.cpp"

// Explicit template instantiations
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;

    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}