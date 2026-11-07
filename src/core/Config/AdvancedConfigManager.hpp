// Advanced NLM Configuration Management
// Provides enhanced configuration capabilities with validation, serialization,
// and validation for NLM simulations

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Configuration schema definitions
struct ConfigSchema {
    std::string key;
    std::string type;
    std::string description;
    std::string defaultValue;
    std::string validationPattern;
    std::vector<std::string> allowedValues;
    bool required;
    bool isAdvanced;
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Advanced configuration manager
class AdvancedConfigManager {
public:
    AdvancedConfigManager() = default;
    ~AdvancedConfigManager() = default;
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const json& j);
    bool loadFromCommandLine(int argc, char* argv[]);
    
    // Configuration access
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    template<typename T>
    T get(const std::string& key, ConfigSource source) const;
    
    // Configuration setting
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    ValidationResult validate() const;
    bool isValid() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    
    // Configuration serialization
    json serialize() const;
    bool serializeToFile(const std::string& filepath) const;
    bool deserializeFromFile(const std::string& filepath);
    bool deserializeFromJSON(const json& j);
    
    // Configuration management
    std::vector<std::string> getAllKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    std::vector<std::string> getAdvancedKeys() const;
    size_t getKeyCount() const;
    
    // Schema management
    void registerSchema(const ConfigSchema& schema);
    bool hasSchema(const std::string& key) const;
    const ConfigSchema* getSchema(const std::string& key) const;
    
    // Preset management
    bool savePreset(const std::string& name);
    bool loadPreset(const std::string& name);
    std::vector<std::string> getPresetNames() const;
    bool deletePreset(const std::string& name);
    
    // Comparison and merging
    bool hasChanges() const;
    void acceptChanges();
    std::shared_ptr<AdvancedConfigManager> createDiff(const AdvancedConfigManager& other) const;
    void merge(const AdvancedConfigManager& other);
    
    // Configuration groups
    struct ConfigGroup {
        std::string name;
        std::string description;
        std::vector<std::string> keys;
        bool isAdvanced;
    };
    
    void registerGroup(const ConfigGroup& group);
    std::vector<ConfigGroup> getGroups() const;
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Static methods
    static std::vector<ConfigSchema> getDefaultSchema();
    static std::vector<ConfigGroup> getDefaultGroups();
    static std::shared_ptr<AdvancedConfigManager> createDefault();

private:
    std::shared_ptr<Config> config_;
    std::map<std::string, ConfigSchema> schema_;
    std::vector<ConfigGroup> groups_;
    std::map<std::string, std::vector<std::string>> presetStorage_;
    std::map<std::string, json> presetData_;
    
    // Change tracking
    bool hasChanges_;
    
    // Validation helpers
    bool validateValue(const std::string& key, const std::string& value) const;
    bool validateRange(const std::string& key, double value) const;
    bool validateAllowedValues(const std::string& key, const std::string& value) const;
    
    // Serialization helpers
    void toJSON(const Config& config, json& j) const;
    void fromJSON(const json& j, Config& config) const;
    
    // Category helpers
    std::string getKeyCategory(const std::string& key) const;
};

// Specialized configuration types for NLM
struct NLMSimulationConfig {
    int64_t neuronCount;
    int64_t regionCount;
    float connectionProbability;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float synaptogenesisRate;
    float pruningRate;
    double simulationTimestep;
    int64_t maxSteps;
    std::string logLevel;
    bool enablePlasticity;
    bool enableStructuralPlasticity;
    bool enableDevelopment;
    bool enableNeuromodulation;
    bool enableMemory;
    bool enablePrediction;
    bool enableCognition;
    std::string environmentType;
    int64_t environmentSize;
};

struct NLMPerformanceConfig {
    size_t benchmarkNeuronCount;
    size_t benchmarkSteps;
    bool enableProfiling;
    bool enableTiming;
    bool enableMemoryTracking;
    bool enableThreadInfo;
    double performanceSampleInterval;
};

struct NLMVisualizationConfig {
    bool enableVisualization;
    double visualizationFPS;
    double visualizationUpdateInterval;
    std::string visualizationBackend;
    bool showNeuronStates;
    bool showSynapseWeights;
    bool showSpikeTraces;
    bool showNetworkTopology;
};

struct NLMExperimentalConfig {
    std::string experimentName;
    std::string experimentType;
    double experimentDuration;
    std::map<std::string, std::string> experimentalParameters;
    bool enableCheckpointing;
    bool enableReplay;
    std::string checkpointFile;
    std::string replayFile;
};

// Helper functions for configuration
class ConfigHelper {
public:
    static NLMSimulationConfig getSimulationConfig(const AdvancedConfigManager& config);
    static NLMPerformanceConfig getPerformanceConfig(const AdvancedConfigManager& config);
    static NLMVisualizationConfig getVisualizationConfig(const AdvancedConfigManager& config);
    static NLMExperimentalConfig getExperimentalConfig(const AdvancedConfigManager& config);
    
    static std::string configToString(const AdvancedConfigManager& config);
    static void printConfig(const AdvancedConfigManager& config);
    static void printConfigValidation(const AdvancedConfigManager& config);
    
    static bool validateSimulationConfig(const NLMSimulationConfig& config);
    static bool validatePerformanceConfig(const NLMPerformanceConfig& config);
    static bool validateVisualizationConfig(const NLMVisualizationConfig& config);
    static bool validateExperimentalConfig(const NLMExperimentalConfig& config);
    
    static std::string generateConfigTemplate(const std::string& templateType);
    static json createConfigSchema(const std::vector<std::string>& keys);
};

} // namespace nlm