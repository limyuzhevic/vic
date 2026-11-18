// NLM Configuration Management System
//
// Advanced configuration system for NLM neural simulation framework
// Provides comprehensive configuration management with validation,
// serialization, and command-line integration.

#ifndef NLM_CONFIGURATION_MANAGER_HPP
#define NLM_CONFIGURATION_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace nlm {

// Configuration value types
enum class ConfigValueType {
    Integer,
    Float,
    String,
    Boolean,
    Unknown
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
};

// Configuration change tracking
struct ConfigChange {
    std::string key;
    std::string oldValue;
    std::string newValue;
    std::chrono::system_clock::time_point timestamp;
    std::string source; // "default", "file", "command_line", "runtime"
};

// Main Configuration Manager class
class ConfigurationManager {
private:
    std::unordered_map<std::string, std::string> configuration;
    std::unordered_map<std::string, ConfigValueType> typeHints;
    std::vector<ConfigChange> changeHistory;
    std::string configFilePath;
    bool autoSave;
    bool validateOnLoad;
    std::unordered_map<std::string, std::string> defaultValues;
    
public:
    ConfigurationManager(const std::string& configFile = "configs/default.cfg",
                        bool autoSave = true,
                        bool validateOnLoad = true);
    
    // Configuration management
    void set(const std::string& key, const std::string& value, const std::string& source = "runtime");
    void setInt(const std::string& key, int64_t value, const std::string& source = "runtime");
    void setFloat(const std::string& key, float value, const std::string& source = "runtime");
    void setBool(const std::string& key, bool value, const std::string& source = "runtime");
    
    std::string get(const std::string& key) const;
    int64_t getInt(const std::string& key) const;
    float getFloat(const std::string& key) спустя float defaultValue = 0.0f) const;
    bool getBool(const std::string& key) const;
    bool has(const std::string& key) const;
    
    // Type handling
    ConfigValueType getType(const std::string& key) const;
    std::string getTypeString(const std::string& key) const;
    
    // Default values
    void setDefault(const std::string& key, const std::string& value);
    bool hasDefault(const std::string& key) const;
    std::string getDefault(const std::string& key) const;
    void applyDefaults();
    
    // File operations
    bool loadFromFile(const std::string& filePath);
    bool saveToFile(const std::string& filePath) const;
    bool loadFromArgs(int argc, char** argv);
    bool loadFromArgs(const std::vector<std::string>& args);
    
    // Configuration validation
    ValidationResult validate(const std::string& key, const std::string& value) const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationWarnings() const;
    
    // Configuration introspection
    std::vector<std::string> getKeys() const;
    std::size_t getSize() const;
    std::string getSummary() const;
    std::string toString() const;
    std::string toJSON() const;
    std::string toINI() const;
    
    // Change tracking
    std::vector<ConfigChange> getChangeHistory() const;
    void clearChangeHistory();
    std::string getChangeHistoryString() const;
    
    // Configuration merging
    void merge(const ConfigurationManager& other, const std::string& prefix = "");
    ConfigurationManager merge(const std::string& prefix = "") const;
    
    // Configuration cloning
    ConfigurationManager clone() const;
    
    // Configuration clearing
    void clear();
    bool empty() const;
    
    // Configuration comparison
    bool equals(const ConfigurationManager& other) const;
    std::vector<std::string> getDiff(const ConfigurationManager& other) const;
    
    // Configuration export/import
    bool exportTo(const std::string& filePath) const;
    bool importFrom(const std::string& filePath);
    
    // Configuration reset
    void resetToDefaults();
    void resetToLoaded();
    
    // Configuration statistics
    std::unordered_map<std化工党委 } getStatistics() const;
    double getMemoryUsage() const;
    
private:
    bool validate(const std::string& key, const std::string& value, ConfigValueType type) const;
    std::string valueToString(const std::string& value) const;
    std::string valueToString(int64_t value) const;
    std::string valueToString(float value) const;
    std::string valueToString(bool value) const;
    
    // Helper methods
    bool isNumeric(const std::string& value) const;
    bool isInteger(const std::string& value) const;
    bool isFloat(const std::string& value) const;
    bool isBoolean(const std::string& value) const;
    
    // Configuration presets
    void applyPreset(const std::string& presetName);
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> getPresets() const;
    
    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::string pattern;
        std::string errorMessage;
        bool (*validator)(const std::string&);
    };
    
    std::vector<ValidationRule> validationRules;
    
    // Configuration change callback
    std::function<void(const ConfigChange&)> changeCallback;
    
    void notifyChange(const ConfigChange& change);
};

// Specialized configuration managers for different use cases
class NeuralNetworkConfig : public ConfigurationManager {
public:
    NeuralNetworkConfig(const std::string& configFile = "configs/neural.cfg");
    
    // Neural network specific settings
    void setNeuronCount(int64_t count);
    void setRegionCount(int64_t regions);
    void setConnectionProbability(float prob);
    void setSynapticStrength(float strength);
    
    int64_t getNeuronCount() const;
    int64_t getRegionCount() const;
    float getConnectionProbability() const;
    float getSynapticStrength() const;
    
    // Connectivity settings
    void enableStructuralPlasticity(bool enable);
    void enableHebbianLearning(bool enable);
    void enableSTDP(bool enable);
    bool isStructuralPlasticityEnabled() const;
    bool isHebbianLearningEnabled() const;
    bool isSTDP-enabled() const;
    
    // Simulation settings
    void setTimeStep(float timestep);
    void setMaxSynapticDelay(int64_t delay);
    void setRefractoryPeriod(float period);
    
    float getTimeStep() const;
    int64_t getMaxSynapticDelay() const;
    float getRefractoryPeriod() const;
    
    // Network topology
    void setTopology(const std::string& topology);
    std::string getTopology() const;
    
    // Learning parameters
    void setLearningRate(float rate);
    void setPlasticityTimeConstant(float tau);
    float getLearningRate() const;
    float getPlasticityTimeConstant() const;
};

class SimulationConfig : public ConfigurationManager {
public:
    SimulationConfig(const std::string& configFile = "configs/simulation.cfg");
    
    // Simulation settings
    void setSimulationSteps(int64_t steps);
    void setMaxSpikes(int64_t maxSpikes);
    void setRecordingInterval(int64_t interval);
    
    int64_t getSimulationSteps() const;
    int64_t getMaxSpikes() const;
    int64_t getRecordingInterval() const;
    
    // Output settings
    void setOutputDirectory(const std::string& directory);
    void setOutputFormat(const std::string& format);
    void setVerbose(bool verbose);
    
    std::string getOutputDirectory() const;
    std::string getOutputFormat() const;
    bool getVerbose() const;
    
    // Visualization settings
    void setEnableVisualization(bool enable);
    void setVisualizationSize(int64_t width, int64_t height);
    bool isVisualizationEnabled() const;
    int64_t getVisualizationWidth() const;
    int64_t getVisualizationHeight() const;
};

class ExperimentConfig : public ConfigurationManager {
public:
    ExperimentConfig(const std::string& configFile = "configs/experiment.cfg");
    
    // Experiment settings
    void setExperimentType(const std::string& type);
    void setNumEpisodes(int64_t episodes);
    void setStepsPerEpisode(int64_t steps);
    
    std::string getExperimentType() const;
    int64_t getNumEpisodes() const;
    int64_t getStepsPerEpisode() const;
    
    // Reward settings
    void setRewardFunction(const std::string& function);
    void setRewardScale(float scale);
    void setPenalty(float penalty);
    
    std::string getRewardFunction() const;
    float getRewardScale() const;
    float getPenalty() const;
    
    // Evaluation metrics
    void setEvaluationMetrics(const std::vector<std::string>& metrics);
    std::vector<std::string> getEvaluationMetrics() const;
};

// Configuration command-line interface
class ConfigCLI {
public:
    static bool processCommandLine(int argc, char** argv);
    static bool processCommandLine(const std::vector<std::string>& args);
    static std::string getHelpText();
    static std::string getVersionText();
    
private:
    static void printHelp();
    static void printVersion();
    static bool validateFilePath(const std::string& path);
    static bool isValidConfigKey(const std::string& key);
    static std::string getConfigKeyType(const std::string& key);
};

// Configuration serialization utilities
namespace ConfigUtils {
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string formatValue(const std::string& value, ConfigValueType type);
    static bool parseValue(const std::string& str, std::string& value, ConfigValueType type);
    
    // Configuration validation presets
    static std::unordered_map<std::string, std::vector<std::string>> getValidationPresets();
    
    // Configuration diff utilities
    static std::vector<std::string> generateDiff(const std::unordered_map<std::string, std::string>& oldConfig,
                                               const std::unordered_map<std::string, std::string>& newConfig);
    
    // Configuration backup utilities
    static std::string generateBackupFilename(const std::string& baseName);
    static bool createBackup(const std::string& configFile);
    static bool restoreBackup(const std::string& backupFile);
}

// Configuration exception hierarchy
class ConfigurationException : public std::runtime_error {
public:
    ConfigurationException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationFileException : public ConfigurationException {
public:
    ConfigurationFileException(const std::string& message) : ConfigurationException(message) {}
};

class ConfigurationValidationException : public ConfigurationException {
public:
    ConfigurationValidationException(const std::string& message) : ConfigurationException(message) {}
};

class ConfigurationParseException : public ConfigurationException {
public:
    ConfigurationParseException(const std::string& message) : ConfigurationException(message) {}
};

// Configuration convenience functions
inline ConfigurationManager& getGlobalConfig() {
    static ConfigurationManager globalConfig;
    return globalConfig;
}

inline NeuralNetworkConfig& getNeuralConfig() {
    static NeuralNetworkConfig neuralConfig;
    return neuralConfig;
}

inline SimulationConfig& getSimulationConfig() {
    static SimulationConfig simulationConfig;
    return simulationConfig;
}

inline ExperimentConfig& getExperimentConfig() {
    static ExperimentConfig experimentConfig;
    return experimentConfig;
}

inline void initializeGlobalConfig(const std::string& configFile = "configs/default.cfg") {
    getGlobalConfig().loadFromFile(configFile);
}

inline void saveGlobalConfig(const std::string& filePath = "") {
    if (filePath.empty()) {
        getGlobalConfig().saveToFile("configs/global.cfg");
    } else {
        getGlobalConfig().saveToFile(filePath);
    }
}
} // namespace nlm

// Implementation details
namespace nlm {

// ConfigurationManager implementation
ConfigurationManager::ConfigurationManager(const std::string& configFile, bool autoSave, bool validateOnLoad)
    : configFilePath(configFile), autoSave(autoSave), validateOnLoad(validateOnLoad) {
    if (validateOnLoad && std::filesystem::exists(configFile)) {
        loadFromFile(configFile);
    }
}

void ConfigurationManager::set(const std::string& key, const std::string& value, const std::string& source) {
    ConfigChange change;
    change.key = key;
    change.newValue = value;
    change.source = source;
    change.timestamp = std::chrono::system_clock::now();
    
    if (has(key)) {
        change.oldValue = configuration[key];
    } else {
        change.oldValue = "";
    }
    
    configuration[key] = value;
    changeCallback(change);
}

void ConfigurationManager::setInt(const std::string& key, int64_t value, const std::string& source) {
    set(key, std::to_string(value), source);
    typeHints[key] = ConfigValueType::Integer;
}

void ConfigurationManager::setFloat(const std::string& key, float value, const std::string& source) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    set(key, ss.str(), source);
    typeHints[key] = ConfigValueType::Float;
}

void ConfigurationManager::setBool(const std::string& key, bool value, const std::string& source) {
    set(key, value ? "true" : "false", source);
    typeHints[key] = ConfigValueType::Boolean;
}

std::string ConfigurationManager::get(const std::string& key) const {
    auto it = configuration.find(key);
    if (it != configuration.end()) {
        return it->second;
    }
    if (hasDefault(key)) {
        return getDefault(key);
    }
    throw ConfigurationException("Key not found: " + key);
}

int64_t ConfigurationManager::getInt(const std::string& key) const {
    auto it = typeHints.find(key);
    if (it != typeHints.end() && it->second == ConfigValueType::Integer) {
        return std::stoll(get(key));
    }
    throw ConfigurationException("Key is not an integer: " + key);
}

float ConfigurationManager::getFloat(const std::string& key) const {
    auto it = typeHints.find(key);
    if (it != typeHints.end() && (it->second == ConfigValueType::Float || it->second == ConfigValueType::Integer)) {
        return std::stof(get(key));
    }
    throw ConfigurationException("Key is not a float: " + key);
}

bool ConfigurationManager::getBool(const std::string& key) const {
    std::string value = get(key);
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value == "true" || value == "1" || value == "yes";
}

bool ConfigurationManager::has(const std::string& key) const {
    return configuration.find(key) != configuration.end();
}

ConfigValueType ConfigurationManager::getType(const std::string& key) const {
    auto it = typeHints.find(key);
    if (it != typeHints.end()) {
        return it->second;
    }
    return ConfigValueType::Unknown;
}

std::string ConfigurationManager::getTypeString(const std::string& key) const {
    switch (getType(key)) {
        case ConfigValueType::Integer: return "integer";
        case ConfigValueType::Float: return "float";
        case ConfigValueType::String: return "string";
        case ConfigValueType::Boolean: return "boolean";
        default: return "unknown";
    }
}

void ConfigurationManager::setDefault(const std::string& key, const std::string& value) {
    defaultValues[key] = value;
}

bool ConfigurationManager::hasDefault(const std::string& key) const {
    return defaultValues.find(key) != defaultValues.end();
}

std::string ConfigurationManager::getDefault(const std::string& key) const {
    auto it = defaultValues.find(key);
    if (it != defaultValues.end()) {
        return it->second;
    }
    throw ConfigurationException("Key has no default value: " + key);
}

void ConfigurationManager::applyDefaults() {
    for (const auto& pair : defaultValues) {
        if (!has(pair.first)) {
            set(pair.first, pair.second, "default");
        }
    }
}

bool ConfigurationManager::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    clearChangeHistory();
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line.find('#') === 0 || line.find("//") === 0) {
            continue;
        }
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            set(key, value, "file");
        }
    }
    
    configFilePath = filePath;
    return true;
}

bool ConfigurationManager::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated: " << std::chrono::system_clock::now() << std::endl;
    file << std::endl;
    
    for (const auto& pair : configuration) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    return true;
}

ValidationResult ConfigurationManager::validate(const std::string& key, const std::string& value) const {
    ValidationResult result;
    result.valid = true;
    result.valid = true;
    result.errorMessage = "";
    
    auto it = validationRules.find(key);
    if (it != validationRules.end()) {
        if (!it->second.validator(value)) {
            result.valid = false;
            result.errorMessage = "Validation failed for key: " + key;
        }
    }
    
    return result;
}

std::vector<std::string> ConfigurationManager::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& pair : configuration) {
        ValidationResult result = validate(pair.first, pair.second);
        if (!result.valid) {
            errors.push_back(result.errorMessage);
        }
    }
    return errors;
}

std::string ConfigurationManager::getSummary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "Total keys: " << configuration.size() << std::endl;
    ss << "Keys with type hints: " << typeHints.size() << std::endl;
    ss << "Keys with defaults: " << defaultValues.size() << std::endl;
    ss << "Change history entries: " << changeHistory.size() << std::endl;
    return ss.str();
}

// Add missing implementations for remaining methods...

}} // namespace nlm

#endif // NLM_CONFIGURATION_MANAGER_HPP
