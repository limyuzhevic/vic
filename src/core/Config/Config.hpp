#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <chrono>
#include <map>
#include <unordered_map>

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

// Performance monitoring data structure
struct PerformanceMetrics {
    std::chrono::high_resolution_clock::time_point simulationStart;
    std::chrono::high_resolution_clock::time_point simulationEnd;
    size_t totalStepsExecuted;
    size_t totalSpikesGenerated;
    double averageFiringRate;
    double simulationTimeSeconds;
    double stepsPerSecond;
    std::unordered_map<std::string, double> customMetrics;
    
    PerformanceMetrics() : totalStepsExecuted(0), totalSpikesGenerated(0), 
                          averageFiringRate(0.0), simulationTimeSeconds(0.0), 
                          stepsPerSecond(0.0) {}
    
    void reset() {
        simulationStart = std::chrono::high_resolution_clock::now();
        totalStepsExecuted = 0;
        totalSpikesGenerated = 0;
        averageFiringRate = 0.0;
        simulationTimeSeconds = 0.0;
        stepsPerSecond = 0.0;
        customMetrics.clear();
    }
    
    void update(size_t steps, size_t spikes, double firingRate) {
        totalStepsExecuted = steps;
        totalSpikesGenerated = spikes;
        averageFiringRate = firingRate;
        simulationEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(simulationEnd - simulationStart);
        simulationTimeSeconds = duration.count();
        stepsPerSecond = (simulationTimeSeconds > 0) ? (totalStepsExecuted / simulationTimeSeconds) : 0.0;
    }
    
    std::string getReport() const {
        std::stringstream ss;
        ss << "Performance Metrics:" << std::endl;
        ss << "  Total steps: " << totalStepsExecuted << std::endl;
        ss << "  Total spikes: " << totalSpikesGenerated << std::endl;
        ss << "  Average firing rate: " << averageFiringRate << " Hz" << std::endl;
        ss << "  Simulation time: " << simulationTimeSeconds << " seconds" << std::endl;
        ss << "  Steps per second: " << stepsPerSecond << std::endl;
        ss << "  Spikes per second: " << (totalSpikesGenerated / (simulationTimeSeconds > 0 ? simulationTimeSeconds : 1.0)) << std::endl;
        return ss.str();
    }
};

// Debugging session information
struct DebugInfo {
    bool debugMode;
    bool profileMode;
    bool validateMode;
    bool logNeuronActivity;
    bool logSynapseActivity;
    bool logMemoryActivity;
    bool logPerformanceMetrics;
    bool exportVisualizationData;
    std::string debugOutputFile;
    size_t activityLogSampleRate;
    
    DebugInfo() : debugMode(false), profileMode(false), validateMode(false),
                  logNeuronActivity(false), logSynapseActivity(false),
                  logMemoryActivity(false), logPerformanceMetrics(false),
                  exportVisualizationData(false), activityLogSampleRate(100) {}
    
    bool isDebugEnabled() const { return debugMode || profileMode || validateMode; }
    bool shouldLogActivity() const { return isDebugEnabled() && activityLogSampleRate > 0; }
    bool shouldExportVisualization() const { return exportVisualizationData; }
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
    
    // Performance monitoring methods
    PerformanceMetrics& getPerformanceMetrics() { return performanceMetrics; }
    const PerformanceMetrics& getPerformanceMetrics() const { return performanceMetrics; }
    void recordPerformanceStep(size_t steps, size_t spikes, double firingRate);
    
    // Debug mode methods
    void enableDebugMode(bool enabled = true);
    void enableProfileMode(bool enabled = true);
    void enableValidationMode(bool enabled = true);
    void enableActivityLogging(bool neurons = false, bool synapses = false, bool memory = false);
    void enablePerformanceLogging(bool enabled = true);
    void enableVisualizationExport(bool enabled = true, const std::string& outputFile = "");
    
    bool isDebugModeEnabled() const { return debugInfo.debugMode; }
    bool isProfileModeEnabled() const { return debugInfo.profileMode; }
    bool isValidationModeEnabled() const { return debugInfo.validateMode; }
    bool shouldLogNeuronActivity() const { return debugInfo.logNeuronActivity; }
    bool shouldLogSynapseActivity() const { return debugInfo.logSynapseActivity; }
    bool shouldLogMemoryActivity() const { return debugInfo.logMemoryActivity; }
    bool shouldLogPerformanceMetrics() const { return debugInfo.logPerformanceMetrics; }
    bool shouldExportVisualization() const { return debugInfo.shouldExportVisualization(); }
    
    const DebugInfo& getDebugInfo() const { return debugInfo; }
    std::string getDebugReport() const;
    
    // Configuration validation
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationWarnings() const;
    
    // Advanced configuration utilities
    std::vector<std::pair<std::string, double>> getConfigurationSensitivity(const std::string& baseConfigFile = "") const;
    std::unordered_map<std::string, double> getPerformanceParameters() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Performance monitoring
    PerformanceMetrics performanceMetrics;
    
    // Debug information
    DebugInfo debugInfo;
    
    // Validation
    mutable std::vector<std::string> validationErrors;
    mutable std::vector<std::string> validationWarnings;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Performance calculation helpers
    double calculateFiringRate(size_t spikes, double duration) const;
    void updatePerformanceMetrics(size_t steps, size_t spikes, double duration) const;
    
    // Debug report generation
    std::string generateDebugReport() const;
    std::string generateValidationReport() const;
    
    // Configuration validation helpers
    void validateBasicParameters() const;
    void validatePlasticityParameters() const;
    void validateMemoryParameters() const;
    void validateSimulationParameters() const;
    void validateNeuromodulationParameters() const;
    
    // Configuration sensitivity analysis
    std::vector<std::pair<std::string, double>> analyzeParameterSensitivity(const std::string& baseConfigFile) const;
    
    // Performance parameter extraction
    std::unordered_map<std::string, double> extractPerformanceParameters() const;
};

} // namespace nlm
