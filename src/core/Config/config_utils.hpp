#pragma once

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace config_utils {

// Configuration factory function - create default configuration
nlm::Config createDefaultConfig();

// Configuration factory function - create minimal configuration
nlm::Config createMinimalConfig();

// Configuration factory function - create configuration from template
nlm::Config createFromTemplate(const std::string& templateName);

// Template configurations
nlm::Config createBrainSimulationConfig();
nlm::Config createSensoryProcessingConfig();
nlm::Config createMotorControlConfig();
nlm::Config createDevelopmentConfig();
nlm::Config createNeuromodulationConfig();
nlm::Config createMemoryConfig();

// Configuration merging utilities
void mergeConfigs(nlm::Config& target, const nlm::Config& source, nlm::ConfigSource overrideSource = nlm::ConfigSource::Override);
bool configsEqual(const nlm::Config& config1, const nlm::Config& config2, bool ignoreSource = true);

// Configuration serialization utilities
std::string configToString(const nlm::Config& config, const std::string& format = "summary");
bool saveConfigToFile(const nlm::Config& config, const std::string& filepath, 
                    const std::string& format = "auto");

// Configuration validation utilities
bool validateConfig(const nlm::Config& config);
std::vector<std::string> getConfigValidationErrors(const nlm::Config& config);

// Configuration comparison utilities
bool hasSameKeys(const nlm::Config& config1, const nlm::Config& config2);
double calculateConfigSimilarity(const nlm::Config& config1, const nlm::Config& config2);

// Configuration manipulation utilities
void removeKeysWithPrefix(nlm::Config& config, const std::string& prefix);
void renameKey(nlm::Config& config, const std::string& oldKey, const std::string& newKey);
void replaceValue(nlm::Config& config, const std::string& key, const nlm::ConfigValue& newValue);

// Configuration statistics and analysis
nlm::Config::ConfigStats calculateConfigStats(const nlm::Config& config);
std::string analyzeConfigComplexity(const nlm::Config& config);
std::map<std::string, size_t> getConfigTypeDistribution(const nlm::Config& config);

// Configuration filtering utilities
nlm::Config filterBySource(const nlm::Config& config, nlm::ConfigSource source);
nlm::Config filterByPrefix(const nlm::Config& config, const std::string& prefix);
nlm::Config filterByType(const nlm::Config& config, const std::type_info& typeInfo);

// Configuration transformation utilities
nlm::Config transformValues(const nlm::Config& config, 
                          std::function<nlm::ConfigValue(nlm::ConfigValue)> transformer);
nlm::Config extractSubset(const nlm::Config& config, const std::vector<std::string>& keys);
nlm::Config expandNestedConfig(const nlm::Config& config, const std::string& prefix = "");

// Configuration import/export utilities
bool importConfig(nlm::Config& config, const std::string& filepath, bool validate = true);
bool exportConfig(const nlm::Config& config, const std::string& filepath, const std::string& format = "auto");

// Configuration optimization utilities
double optimizeConfigForPerformance(nlm::Config& config);
double optimizeConfigForMemory(nlm::Config& config);

// Configuration validation helper functions
bool validateRange(double value, double min, double max);
bool validateStringLength(const std::string& value, size_t minLen, size_t maxLen);
bool validatePattern(const std::string& value, const std::regex& pattern);
bool validatePositive(const nlm::ConfigValue& value);
bool validateNonNegative(const nlm::ConfigValue& value);
bool validateRequired(const nlm::ConfigValue& value);

// Configuration backup utilities
nlm::Config createConfigBackup(const nlm::Config& config);
bool restoreConfigFromBackup(nlm::Config& config, const nlm::Config& backup);

// Configuration diff utilities
std::vector<std::string> getConfigDifferences(const nlm::Config& config1, const nlm::Config& config2);

// Configuration logging utilities
void logConfigChanges(const nlm::Config& oldConfig, const nlm::Config& newConfig);
void logConfigSummary(const nlm::Config& config);

// Configuration profiling utilities
double profileConfigAccessPattern(const nlm::Config& config, const std::vector<std::string>& keys);
double profileConfigMemoryUsage(const nlm::Config& config);

// Configuration export utilities for different formats
std::string configToPython(const nlm::Config& config);
std::string configToJSON(const nlm::Config& config, bool prettyPrint = false);
std::string configToYAML(const nlm::Config& config);
std::string configToXML(const nlm::Config& config);
std::string configToINI(const nlm::Config& config);
std::string configToToml(const nlm::Config& config);

// Configuration import utilities for different formats
bool importFromPython(nlm::Config& config, const std::string& content);
bool importFromJSON(nlm::Config& config, const std::string& content);
bool importFromYAML(nlm::Config& config, const std::string& content);
bool importFromXML(nlm::Config& config, const std::string& content);
bool importFromINI(nlm::Config& config, const std::string& content);
bool importFromToml(nlm::Config& config, const std::string& content);

// Configuration validation rule creation utilities
nlm::ConfigValidator::ValidationFunc createRangeValidator(double min, double max);
nlm::ConfigValidator::ErrorFunc createRangeErrorHandler(double min, double max);
nlm::ConfigValidator::ValidationFunc createStringLengthValidator(size_t minLen, size_t maxLen);
nlm::ConfigValidator::ErrorFunc createStringLengthErrorHandler(size_t minLen, size_t maxLen);
nlm::ConfigValidator::ValidationFunc createPatternValidator(const std::string& pattern);
nlm::ConfigValidator::ErrorFunc createPatternErrorHandler(const std::string& pattern);

// Configuration batch operation utilities
void batchSet(nlm::Config& config, const std::map<std::string, nlm::ConfigValue>& values);
void batchUpdate(nlm::Config& config, const std::map<std::string, nlm::ConfigValue>& values);
std::map<std::string, nlm::ConfigValue> batchGet(const nlm::Config& config, const std::vector<std::string>& keys);

// Configuration cloning utilities
nlm::Config cloneConfig(const nlm::Config& config);
nlm::Config deepCloneConfig(const nlm::Config& config);

// Configuration merge strategy utilities
enum class MergeStrategy {
    Override,
    Merge,
    KeepExisting,
    KeepNew,
    Average,
    Maximum,
    Minimum
};

void mergeConfigsWithStrategy(nlm::Config& target, const nlm::Config& source, 
                           MergeStrategy strategy, nlm::ConfigSource sourceType = nlm::ConfigSource::Override);

// Configuration template utilities
template<typename T>
std::vector<T> extractValues(const nlm::Config& config, const std::string& key);

template<typename T>
nlm::Config setAllValues(nlm::Config& config, const std::vector<std::string>& keys, const T& value);

// Configuration validation utilities
nlm::ConfigValidator::ConfigValidatorBuilder createValidatorBuilder();

// Configuration export utilities for different contexts
std::string configToCStyle(const nlm::Config& config);
std::string configToJavaScript(const nlm::Config& config);
std::string configToTypeScript(const nlm::Config& config);

// Configuration import utilities for different contexts
bool importFromCStyle(nlm::Config& config, const std::string& content);
bool importFromJavaScript(nlm::Config& config, const std::string& content);
bool importFromTypeScript(nlm::Config& config, const std::string& content);

// Configuration utility constants
namespace constants {
    const std::string DEFAULT_DESCRIPTION = "Default configuration value";
    const std::string VALIDATOR_POSITIVE = "positive";
    const std::string VALIDATOR_NON_NEGATIVE = "non_negative";
    const std::string VALIDATOR_RANGE = "range";
    const std::string VALIDATOR_STRING_LENGTH = "string_length";
    const std::string VALIDATOR_PATTERN = "pattern";
    const std::string VALIDATOR_REQUIRED = "required";
    
    const double DEFAULT_MIN_VALUE = 0.0;
    const double DEFAULT_MAX_VALUE = 1.0;
    const size_t DEFAULT_MIN_LENGTH = 1;
    const size_t DEFAULT_MAX_LENGTH = 100;
    
    const std::string DEFAULT_FORMAT = "cfg";
    const std::string DEFAULT_SEPARATOR = " = ";
    const std::string DEFAULT_PREFIX = "";
}

} // namespace config_utils

// Explicit template instantiations
namespace config_utils {
template std::vector<int> extractValues<int>(const nlm::Config& config, const std::string& key);
template std::vector<double> extractValues<double>(const nlm::Config& config, const std::string& key);
template std::vector<std::string> extractValues<std::string>(const nlm::Config& config, const std::string& key);
template std::vector<bool> extractValues<bool>(const nlm::Config& config, const std::string& key);

template std::vector<nlm::Config> setAllValues<int>(nlm::Config& config, const std::vector<std::string>& keys, const int& value);
template std::vector<nlm::Config> setAllValues<double>(nlm::Config& config, const std::vector<std::string>& keys, const double& value);
template std::vector<nlm::Config> setAllValues<std::string>(nlm::Config& config, const std::vector<std::string>& keys, const std::string& value);
template std::vector<nlm::Config> setAllValues<bool>(nlm::Config& config, const std::vector<std::string>& keys, const bool& value);
} // namespace config_utils

// Implementation of createDefaultConfig factory function
inline nlm::Config createDefaultConfig() {
    nlm::Config config;
    
    // Set default values for core configuration
    config.set("random_seed", 42, nlm::ConfigSource::Default, "Random seed for reproducible experiments");
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default, "Simulation timestep in seconds");
    config.set("neuron_count", 1000, nlm::ConfigSource::Default, "Number of neurons in the brain");
    config.set("region_count", 2, nlm::ConfigSource::Default, "Number of brain regions");
    config.set("connection_probability", 0.1, nlm::ConfigSource::Default, "Connection probability for random connectivity");
    
    // Set default values for plasticity settings
    config.set("plasticity_learning_rate", 0.01, nlm::ConfigSource::Default, "Plasticity learning rate");
    config.set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Default, "STDP LTP weight");
    config.set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Default, "STDP LTD weight");
    config.set("stdp_time_constant", 20.0, nlm::ConfigSource::Default, "STDP time constant");
    
    // Set default values for development settings
    config.set("development_synaptogenesis_rate", 0.001, nlm::ConfigSource::Default, "Development synaptogenesis rate");
    config.set("development_pruning_rate", 0.0001, nlm::ConfigSource::Default, "Development pruning rate");
    
    // Set default values for neuromodulation settings
    config.set("dopamine_baseline", 0.1, nlm::ConfigSource::Default, "Dopamine baseline");
    config.set("dopamine_learning_rate", 0.01, nlm::ConfigSource::Default, "Dopamine learning rate");
    
    // Set default values for reward settings
    config.set("reward_discount_factor", 0.99, nlm::ConfigSource::Default, "Reward discount factor");
    
    // Set default values for environment settings
    config.set("environment_name", "GridWorld", nlm::ConfigSource::Default, "Environment name");
    config.set("environment_width", 10, nlm::ConfigSource::Default, "Environment width");
    config.set("environment_height", 10, nlm::ConfigSource::Default, "Environment height");
    
    // Set default values for logging settings
    config.set("log_level", "INFO", nlm::ConfigSource::Default, "Log level");
    config.set("log_to_file", false, nlm::ConfigSource::Default, "Log to file");
    config.set("log_filename", "nlm.log", nlm::ConfigSource::Default, "Log filename");
    
    // Set default values for simulation settings
    config.set("max_simulation_steps", 10000, nlm::ConfigSource::Default, "Maximum simulation steps");
    config.set("simulation_time_limit", 0.0, nlm::ConfigSource::Default, "Simulation time limit");
    
    // Set default values for visualization settings
    config.set("visualization_enabled", false, nlm::ConfigSource::Default, "Visualization enabled");
    config.set("visualization_update_rate", 30.0, nlm::ConfigSource::Default, "Visualization update rate");
    
    return config;
}