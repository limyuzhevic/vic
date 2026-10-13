#pragma once

#include "Config.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

namespace nlm {

// Configuration validation result
struct ConfigurationValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
};

// Configuration section
class ConfigurationSection {
public:
    ConfigurationSection(const std::string& name);
    
    // Get configuration value
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        auto it = values_.find(key);
        if (it != values_.end()) {
            return std::get<T>(it->second);
        }
        return defaultValue;
    }
    
    // Set configuration value
    template<typename T>
    void set(const std::string& key, const T& value) {
        values_[key] = value;
        validate();
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return values_.find(key) != values_.end();
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : values_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Remove key
    void remove(const std::string& key) {
        values_.erase(key);
    }
    
    // Get section name
    const std::string& getName() const { return name_; }
    
    // Merge with another section
    void merge(const ConfigurationSection& other);
    
    // Validate section
    virtual ConfigurationValidationResult validate() const { return {true, ""}; }
    
    // Get all values
    const std::unordered_map<std::string, std::any>& getValues() const { return values_; }
    
protected:
    std::string name_;
    std::unordered_map<std::string, std::any> values_;
};

// Brain-specific configuration section
class BrainConfigurationSection : public ConfigurationSection {
public:
    BrainConfigurationSection();
    
    ConfigurationValidationResult validate() const override;
    
    // Neural network configuration
    void setNeuronCount(int64_t count) { set("neuron_count", count); }
    int64_t getNeuronCount() const { return get("neuron_count", static_cast<int64_t>(500)); }
    
    void setRegionCount(int64_t count) { set("region_count", count); }
    int64_t getRegionCount() const { return get("region_count", static_cast<int64_t>(1)); }
    
    void setConnectionProbability(float prob) { set("connection_probability", prob); }
    float getConnectionProbability() const { return get("connection_probability", 0.15f); }
    
    void setRandomSeed(int64_t seed) { set("random_seed", seed); }
    int64_t getRandomSeed() const { return get("random_seed", static_cast<int64_t>(42)); }
    
    void setSimulationTimestep(double timestep) { set("simulation_timestep", timestep); }
    double getSimulationTimestep() const { return get("simulation_timestep", 0.001); }
    
    // Plasticity configuration
    void enableHebbian() { set("plasticity.hebbian.enable", true); }
    void disableHebbian() { set("plasticity.hebbian.enable", false); }
    bool isHebbianEnabled() const { return get("plasticity.hebbian.enable", true); }
    
    void enableSTDP() { set("plasticity.stdp.enable", true); }
    void disableSTDP() { set("plasticity.stdp.enable", false); }
    bool isSTDPEnabled() const { return get("plasticity.stdp.enable", true); }
    
    void enableStructural() { set("plasticity.structural.enable", true); }
    void disableStructural() { set("plasticity.structural.enable", false); }
    bool isStructuralEnabled() const { return get("plasticity.structural.enable", true); }
    
    void setSTDPWeightLTP(float weight) { set("plasticity.stdp.ltp_weight", weight); }
    float getSTDPWeightLTP() const { return get("plasticity.stdp.ltp_weight", 0.02f); }
    
    void setSTDPWeightLTD(float weight) { set("plasticity.stdp.ltd_weight", weight); }
    float getSTDPWeightLTD() const { return get("plasticity.stdp.ltd_weight", 0.015f); }
    
    void setSTDPTimeConstant(float tau) { set("plasticity.stdp.tau", tau); }
    float getSTDPTimeConstant() const { return get("plasticity.stdp.tau", 20.0f); }
    
    void setSynaptogenesisRate(float rate) { set("plasticity.structural.synaptogenesis_rate", rate); }
    float getSynaptogenesisRate() const { return get("plasticity.structural.synaptogenesis_rate", 0.0001f); }
    
    void setPruningRate(float rate) { set("plasticity.structural.pruning_rate", rate); }
    float getPruningRate() const { return get("plasticity.structural.pruning_rate", 0.00001f); }
    
    // Neuromodulation configuration
    void enableRewardModulation() { set("neuromodulation.reward.enable", true); }
    void disableRewardModulation() { set("neuromodulation.reward.enable", false); }
    bool isRewardModulationEnabled() const { return get("neuromodulation.reward.enable", true); }
    
    void enableCuriosity() { set("neuromodulation.curiosity.enable", true); }
    void disableCuriosity() { set("neuromodulation.curiosity.enable", false); }
    bool isCuriosityEnabled() const { return get("neuromodulation.curiosity.enable", true); }
    
    void enableNovelty() { set("neuromodulation.novelty.enable", true); }
    void disableNovelty() { set("neuromodulation.novelty.enable", false); }
    bool isNoveltyEnabled() const { return get("neuromodulation.novelty.enable", true); }
    
    // Development configuration
    void enableDevelopment() { set("development.enable", true); }
    void disableDevelopment() { set("development.enable", false); }
    bool isDevelopmentEnabled() const { return get("development.enable", true); }
    
    // Agent configuration
    void setAgentVisionWidth(int width) { set("agent.sensory.vision.width", width); }
    int getAgentVisionWidth() const { return get("agent.sensory.vision.width", 16); }
    
    void setAgentVisionHeight(int height) { set("agent.sensory.vision.height", height); }
    int getAgentVisionHeight() const { return get("agent.sensory.vision.height", 16); }
    
    void setAgentTouchCount(int count) { set("agent.sensory.touch.count", count); }
    int getAgentTouchCount() const { return get("agent.sensory.touch.count", 8); }
    
    // Configuration file operations
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    std::string toString() const;
    
    // Copy configuration
    BrainConfigurationSection copy() const {
        BrainConfigurationSection copySection("brain");
        copySection.values_ = values_;
        return copySection;
    }
};

// Experiment configuration section
class ExperimentConfigurationSection : public ConfigurationSection {
public:
    ExperimentConfigurationSection();
    
    ConfigurationValidationResult validate() const override;
    
    // Experiment configuration
    void setExperimentType(const std::string& type) { set("type", type); }
    std::string getExperimentType() const { return get("type", ""); }
    
    void setExperimentSteps(int steps) { set("steps", steps); }
    int getExperimentSteps() const { return get("steps", 1000); }
    
    void setExperimentReward(float reward) { set("reward", reward); }
    float getExperimentReward() const { return get("reward", 1.0f); }
    
    void enableMetrics() { set("metrics.enable", true); }
    void disableMetrics() { set("metrics.enable", false); }
    bool isMetricsEnabled() const { return get("metrics.enable", true); }
    
    // Benchmark configuration
    void enableBenchmark() { set("benchmark.enable", true); }
    void disableBenchmark() { set("benchmark.enable", false); }
    bool isBenchmarkEnabled() const { return get("benchmark.enable", false); }
    
    void setBenchmarkIterationCount(int count) { set("benchmark.iterations", count); }
    int getBenchmarkIterationCount() const { return get("benchmark.iterations", 100); }
    
    // Debug configuration
    void enableDebugMode() { set("debug.enable", true); }
    void disableDebugMode() { set("debug.enable", false); }
    bool isDebugModeEnabled() const { return get("debug.enable", false); }
    
    void setLogLevel(const std::string& level) { set("log.level", level); }
    std::string getLogLevel() const { return get("log.level", "info"); }
    
    void setLogFile(const std::string& filepath) { set("log.file", filepath); }
    std::string getLogFile() const { return get("log.file", "nlm_simulation.log"); }
};

// Main configuration manager
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Get global configuration manager instance
    static ConfigurationManager& getGlobal();
    static void setGlobal(std::shared_ptr<ConfigurationManager> manager);
    
    // Get configuration sections
    BrainConfigurationSection& getBrainConfig();
    ExperimentConfigurationSection& getExperimentConfig();
    
    // Get all sections
    std::vector<std::string> getSectionNames() const;
    
    // Merge configurations
    void merge(const ConfigurationManager& other);
    
    // Save configuration
    bool saveToFile(const std::string& filepath) const;
    bool saveToStream(std::ostream& stream) const;
    
    // Load configuration
    bool loadFromFile(const std::string& filepath);
    bool loadFromStream(std::istream& stream);
    
    // Configuration validation
    ConfigurationValidationResult validate() const;
    
    // Reset configuration
    void reset();
    
    // Apply configuration to existing Config object
    void applyTo(std::shared_ptr<Config> config) const;
    
    // Get configuration as string
    std::string toString() const;
    
    // Set configuration from string
    bool fromString(const std::string& configString);
    
    // Configuration file format
    static constexpr const char* DEFAULT_FILENAME = "nlm_config.cfg";
    static constexpr const char* DEFAULT_SECTION_PREFIX = "[";
    static constexpr const char* DEFAULT_SECTION_SUFFIX = "]";
    static constexpr const char* DEFAULT_KEY_VALUE_SEPARATOR = " = ";
    static constexpr const char* DEFAULT_VALUE_QUOTE = "\"";
    
    // Configuration validation functions
    static bool isValidLogLevel(const std::string& level);
    static bool isValidExperimentType(const std::string& type);
    static bool isValidNeuronCount(int64_t count);
    static bool isValidRegionCount(int64_t count);
    static bool isValidConnectionProbability(float prob);
    
    // Configuration help
    static std::string getBrainConfigHelp();
    static std::string getExperimentConfigHelp();
    
    // Configuration command line parsing
    static std::unordered_map<std::string, std::string> parseCommandLineArgs(int argc, char** argv);
    
    // Configuration command line help
    static std::string getCommandLineHelp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
BrainConfigurationSection& getBrainConfiguration();
ExperimentConfigurationSection& getExperimentConfiguration();
void initializeConfiguration();
void cleanupConfiguration();

} // namespace nlm
