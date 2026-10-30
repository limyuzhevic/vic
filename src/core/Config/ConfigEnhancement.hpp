#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>

namespace nlm {

// Configuration schema validation
class ConfigSchema {
public:
    using Validator = std::function<bool(const ConfigValue&)>;
    using DefaultGenerator = std::function<ConfigValue()>

    struct Field {
        std::string key;
        std::type_index type;
        Validator validator;
        ConfigValue defaultValue;
        std::string description;
        bool required;
    };

    void addField(const std::string& key, const std::type_index& type, 
                  const Validator& validator, const ConfigValue& defaultValue,
                  const std::string& description, bool required = false);
    
    bool validate(const Config& config) const;
    std::string getValidationErrors(const Config& config) const;
    
    bool hasField(const std::string& key) const;
    std::string getFieldDescription(const std::string& key) const;
    
private:
    std::unordered_map<std::string, Field> fields_;
};

// JSON configuration loader
class JsonConfigLoader {
public:
    struct JsonConfig {
        std::unordered_map<std::string, ConfigValue> values;
        std::string filename;
        bool valid;
        std::string errorMessage;
    };

    static JsonConfig loadFromJson(const std::string& filepath);
    static std::string getJsonError() const;
    static bool isJsonAvailable() const;
    
private:
    static bool jsonAvailable_;
    static std::string lastError_;
};

// Configuration presets for different experiment types
class ConfigPresets {
public:
    static void applyPreset(Config& config, const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    static Config createConfigForPhase(int phase);
    static Config createConfigForExperiment(const std::string& experimentName);

private:
    static Config createPhase2Config();
    static Config createPhase3Config();
    static Config createPhase4Config();
    static Config createPhase6Config();
};

// Enhanced command line parser
class EnhancedCommandLineParser {
public:
    struct CommandLineOptions {
        std::string configFile;
        std::string experimentName;
        int numEpisodes;
        int stepsPerEpisode;
        bool benchmarkMode;
        bool verboseLogging;
        std::string outputFile;
        double customTimestep;
        int64_t customNeuronCount;
        bool help;
        bool version;
    };

    static CommandLineOptions parse(int argc, char** argv);
    static std::string getUsage();
    static std::string getVersionInfo();
    static bool validateOptions(const CommandLineOptions& options);
    static std::string getValidationErrors(const CommandLineOptions& options);

private:
    static void printHelp();
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Configuration manager for experiment management
class ExperimentConfigManager {
public:
    struct ExperimentConfig {
        std::string name;
        std::string phase;
        std::shared_ptr<Config> config;
        std::vector<std::string> requiredFiles;
        std::vector<std::string> optionalFiles;
        bool validated;
        std::string validationError;
    };

    static ExperimentConfig loadExperimentConfig(const std::string& experimentName);
    static bool saveExperimentConfig(const std::string& experimentName, const Config& config);
    static std::vector<std::string> getAvailableExperiments();
    static bool validateConfig(const Config& config, const std::string& experimentName);
    static void migrateConfig(const Config& oldConfig, Config& newConfig, const std::string& targetVersion);

private:
    static std::unordered_map<std::string, ExperimentConfig> experimentConfigs_;
    static const std::string CONFIG_DIR;
    static const std::string EXPERIMENTS_DIR;
};

// Configuration constants for all experiments
class ConfigConstants {
public:
    // Neural system constants
    static constexpr int64_t MIN_NEURONS = 10;
    static constexpr int64_t MAX_NEURONS = 10000;
    static constexpr double MIN_TIMESTEP = 0.0001;
    static constexpr double MAX_TIMESTEP = 1.0;
    static constexpr double DEFAULT_TIMESTEP = 0.001;
    
    // Plasticity constants
    static constexpr float MIN_STDP_LTP = 0.001;
    static constexpr float MAX_STDP_LTP = 0.1;
    static constexpr float DEFAULT_STDP_LTP = 0.02;
    static constexpr float MIN_STDP_LTD = 0.001;
    static constexpr float MAX_STDP_LTD = 0.1;
    static constexpr float DEFAULT_STDP_LTD = 0.015;
    static constexpr float MIN_STDP_TAU = 1.0;
    static constexpr float MAX_STDP_TAU = 100.0;
    static constexpr float DEFAULT_STDP_TAU = 20.0;
    
    // Memory constants
    static constexpr size_t MIN_WORKING_MEMORY_TRACES = 0;
    static constexpr size_t MAX_WORKING_MEMORY_TRACES = 1000;
    static constexpr float DEFAULT_WORKING_MEMORY_FRACTION = 0.1f;
    
    // Development constants
    static constexpr float MIN_SYNAPTOGENESIS_RATE = 0.00001;
    static constexpr float MAX_SYNAPTOGENESIS_RATE = 0.1;
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001;
    static constexpr float MIN_PRUNING_RATE = 0.000001;
    static constexpr float MAX_PRUNING_RATE = 0.01;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001;
    
    // Experiment constants
    static constexpr int DEFAULT_EPISODES = 5;
    static constexpr int DEFAULT_STEPS_PER_EPISODE = 200;
    static constexpr int MAX_EPISODES = 1000;
    static constexpr int MAX_STEPS_PER_EPISODE = 10000;
    
    // Performance constants
    static constexpr int DEFAULT_CPU_THREADS = 4;
    static constexpr bool DEFAULT_PARALLEL_ENABLED = false;
    static constexpr double DEFAULT_PERFORMANCE_LOG_INTERVAL = 1.0;
    
    // Neural types
    static constexpr int DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    static constexpr float MIN_CONNECTION_PROBABILITY = 0.001f;
    static constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;
    
    // Helper methods
    static bool isValidNeuronCount(int64_t count);
    static bool isValidTimestep(double timestep);
    static bool isValidConnectionProbability(float prob);
    static std::string getDefaultConfigFile(const std::string& experimentName);
    static std::string getConfigBackupPath(const std::string& experimentName);
};

} // namespace nlm