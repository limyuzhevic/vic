#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>

// Forward declarations
namespace nlm {
    class Config;
    enum class ConfigSource;
}

namespace nlm {

// Type alias for configuration values
using ConfigValue = std::variant<
    std::monostate, // Null value
    bool,
    int,
    float,
    double,
    std::string,
    std::vector<ConfigValue>,
    std::unordered_map<std::string, ConfigValue>
>;

enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Preset
};

// Research configuration options for power users
enum class ResearchMode {
    Basic,           // Standard testing
    Intensive,       // Extended testing with more data collection
    LongTerm,        // Very long-term studies (weeks/months)
    Continuous,      // Continuous learning and adaptation
    StressTest,      // Testing under adverse conditions
    Comparative     // Multiple conditions for comparison
};

// Memory management configuration
struct MemoryManagementConfig {
    size_t maxWorkingMemoryCapacity;
    size_t maxEpisodicMemoryCapacity;
    size_t consolidationInterval;
    float consolidationThreshold;
    bool enableReplay;
    bool enableCheckpointing;
    std::string checkpointPath;
    bool autoSaveEnabled;
    float memoryDecayRate;
};

// Performance monitoring configuration
struct PerformanceMonitoringConfig {
    bool enableBenchmarking;
    bool enableProfiling;
    bool enableRealTimeMonitoring;
    int benchmarkIntervalMs;
    int profileIntervalMs;
    std::string metricsOutputPath;
    bool exportMetricsToFile;
    bool enableVisualization;
    int visualizationUpdateIntervalMs;
};

// Advanced simulation configuration
struct AdvancedSimulationConfig {
    ResearchMode researchMode;
    bool enableNeuromodulation;
    bool enableDevelopment;
    bool enableStructuralPlasticity;
    bool enableRewardModulation;
    bool enableCuriosity;
    bool enableNoveltyDetection;
    float neuromodulationStrength;
    float developmentRate;
    float plasticityRate;
    float rewardSensitivity;
    float explorationBonus;
    float noveltySensitivity;
};

class Configuration {
public:
    Configuration();
    ~Configuration();
    
    // File operations
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromArgs(int argc, char** argv);
    
    // Configuration management
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Set configuration values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration values with defaults
    ConfigValue getOr(const std::string& key, const ConfigValue& defaultValue) const;
    std::string getOr(const std::string& key, const std::string& defaultValue) const;
    int getOr(const std::string& key, int defaultValue) const;
    float getOr(const std::string& key, float defaultValue) const;
    double getOr(const std::string& key, double defaultValue) const;
    bool getOr(const std::string& key, bool defaultValue) const;
    
    // Advanced configuration getters
    ResearchMode getResearchMode() const;
    void setResearchMode(ResearchMode mode);
    
    const MemoryManagementConfig& getMemoryManagementConfig() const;
    void setMemoryManagementConfig(const MemoryManagementConfig& config);
    
    const PerformanceMonitoringConfig& getPerformanceMonitoringConfig() const;
    void setPerformanceMonitoringConfig(const PerformanceMonitoringConfig& config);
    
    const AdvancedSimulationConfig& getAdvancedSimulationConfig() const;
    void setAdvancedSimulationConfig(const AdvancedSimulationConfig& config);
    
    // Research scenario presets
    void loadResearchPreset(const std::string& presetName);
    bool hasResearchPreset(const std::string& presetName) const;
    std::vector<std::string> getAvailableResearchPresets() const;
    
    // Performance optimization
    void optimizeForPerformance();
    void optimizeForAccuracy();
    void optimizeForMemory();
    
    // Validation and validation rules
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    void addValidationRule(const std::string& key, std::function<bool(const ConfigValue&)> rule);
    
    // Utility methods
    std::string toString() const;
    std::string toPrettyString() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

