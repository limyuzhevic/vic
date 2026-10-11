#pragma once

#include "Config.hpp"
#include <string>
#include <unordered_map>
#include <map>
#include <chrono>

namespace nlm {

// Configuration manager with profiles and categories
class ConfigManager {
public:
    enum class Profile {
        Debug,
        Release,
        Benchmark
    };
    
    ConfigManager();
    ~ConfigManager();
    
    // Profile management
    void setProfile(Profile profile);
    Profile getProfile() const;
    void applyProfileDefaults();
    
    // Configuration categories
    void addAdvancedConfig();
    void addPerformanceConfig();
    void addDevelopmentConfig();
    void addNeuralConfig();
    
    // Validation
    bool validateConfig(const ConfigEntry& entry) const;
    bool validateAllConfig() const;
    
    // JSON support
    static nlohmann::json serialize(const ConfigEntry& entry);
    static ConfigEntry deserialize(const nlohmann::json& data);
    
    // Backup and inheritance
    static bool backupConfigFile(const std::string& filepath);
    bool loadFromParentProfile(const ConfigManager& parent);
    
    // Configuration templates
    static nlohmann::json getDefaultTemplate(ConfigCategory category);
    
    // Get config by category
    std::vector<ConfigEntry> getConfigByCategory(ConfigCategory category) const;
    
    // Configuration validation with comprehensive checks
    struct ValidationResult {
        bool isValid;
        std::string message;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ValidationResult validateConfigValue(const ConfigEntry& entry) const;
    
    // Configuration comparison
    bool isCompatible(const ConfigManager& other) const;
    
    // Configuration history
    void addConfigSnapshot(const std::string& snapshotName);
    nlohmann::json getConfigHistory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration file utilities
namespace ConfigUtils {
    // File backup and recovery
    static constexpr const char* BACKUP_SUFFIX = ".backup";
    static constexpr const char* JSON_BACKUP_SUFFIX = ".json.backup";
    
    // Default configuration templates
    static const nlohmann::json DEFAULT_BASIC_CONFIG = {
        {"format", "NLM v1.0"},
        {"configuration", {
            {"neuronCount", 1000},
            {"learningRate", 0.01},
            {"timeSteps", 100},
            {"debugLevel", 1},
            {"profilingEnabled", false},
            {"validationEnabled", true}
        }}
    };
    
    static const nlohmann::json DEFAULT_ADVANCED_CONFIG = {
        {"format", "NLM v1.0"},
        {"configuration", {
            {"stdpLTPWeight", 0.1},
            {"stdpLTDWeight", 0.12},
            {"stdpTimeConstant", 20.0},
            {"plasticityLearningRate", 0.05}
        }}
    };
    
    static const nlohmann::json DEFAULT_PERFORMANCE_CONFIG = {
        {"format", "NLM v1.0"},
        {"configuration", {
            {"threadCount", 4},
            {"simdEnabled", true},
            {"memoryPoolSize", 65536},
            {"optimizationFlags", "default"}
        }}
    };
    
    static const nlohmann::json DEFAULT_DEVELOPMENT_CONFIG = {
        {"format", "NLM v1.0"},
        {"configuration", {
            {"debugLevel", 2},
            {"profilingEnabled", true},
            {"validationEnabled", true}
        }}
    };
    
    static const nlohmann::json DEFAULT_NEURAL_CONFIG = {
        {"format", "NLM v1.0"},
        {"configuration", {
            {"synapticDensity", 0.1},
            {"populationSize", 100},
            {"regionSize", 1.0},
            {"neuronTypes", "excitatory,inhibitory"}
        }}
    };
    
    // Validation utilities
    static bool validateIntRange(int value, int min, int max);
    static bool validateDoubleRange(double value, double min, double max);
    static bool validateStringPattern(const std::string& value, const std::string& pattern);
    static bool validateEnum(const std::string& value, const std::vector<std::string>& validValues);
    
    // File operations
    static std::string getTimestampedFilename(const std::string& originalPath);
    static bool validateJSONStructure(const nlohmann::json& jsonData);
    
    // Configuration merging
    static nlohmann::json mergeConfigs(const nlohmann::json& base, const nlohmann::json& override);
    
    // Hash for config comparison
    static std::string computeConfigHash(const nlohmann::json& config);
};

// Configuration validators implementation
class ConfigValidatorImpl : public ConfigValidator {
public:
    ConfigValidatorImpl(ConfigManager::Profile profile);
    
    bool validate(const ConfigEntry& entry) const override;
    std::string getErrorMessage() const override;
    
    ValidationResult validateFull(const ConfigEntry& entry) const;
    
private:
    ConfigManager::Profile profile_;
    mutable std::string errorMessage_;
};

// Default configuration providers
namespace DefaultConfig {
    // Basic defaults
    static constexpr int BASIC_NEURONS = 1000;
    static constexpr double BASIC_LEARNING_RATE = 0.01;
    static constexpr int BASIC_TIME_STEPS = 100;
    static constexpr int BASIC_DEBUG_LEVEL = 1;
    static constexpr bool BASIC_VALIDATION_ENABLED = true;
    
    // Advanced defaults
    static constexpr double ADVANCED_STDP_LTP_WEIGHT = 0.1;
    static constexpr double ADVANCED_STDP_LTD_WEIGHT = 0.12;
    static constexpr double ADVANCED_STDP_TIME_CONST = 20.0;
    static constexpr double ADVANCED_LEARNING_RATE = 0.05;
    
    // Performance defaults
    static constexpr int PERFORMANCE_THREAD_COUNT = 4;
    static constexpr bool PERFORMANCE_SIMD_ENABLED = true;
    static constexpr size_t PERFORMANCE_MEMORY_POOL_SIZE = 65536;
    static constexpr bool PERFORMANCE_OPTIMIZATION_ENABLED = true;
    
    // Development defaults
    static constexpr int DEVELOPMENT_DEBUG_LEVEL = 1;
    static constexpr bool DEVELOPMENT_PROFILING_ENABLED = false;
    static constexpr bool DEVELOPMENT_VALIDATION_ENABLED = true;
    
    // Neural defaults
    static constexpr double NEURAL_SYNAPTIC_DENSITY = 0.1;
    static constexpr int NEURAL_POPULATION_SIZE = 100;
    static constexpr double NEURAL_REGION_SIZE = 1.0;
    static constexpr const char* NEURAL_DEFAULT_TYPES = "excitatory,inhibitory";
}
