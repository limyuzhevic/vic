#include "ConfigurationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <unordered_set>
#include <cctype>
#include <numeric>

namespace nlm {

// ConfigurationManager implementation
class ConfigurationManager::Impl {
public:
    Config config;
    std::unordered_map<std::string, std::string> envVars;
    std::string loadedFromFile;
    bool dynamicUpdatesEnabled;
    std::function<void(const std::string&, const ConfigValue&)> updateCallback;
    std::chrono::system_clock::time_point lastUpdateCheck;
    
    Impl() : dynamicUpdatesEnabled(false), lastUpdateCheck(std::chrono::system_clock::now()) {}
};

ConfigurationManager::ConfigurationManager() : 
    dynamicUpdatesEnabled_(false), updateCallback_(nullptr) {
    pImpl = std::make_unique<Impl>();
    schema_ = getDefaultConfigSchema();
    
    NLM_LOG_INFO("ConfigurationManager initialized");
}

bool ConfigurationManager::loadConfiguration(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        bool success = true;
        
        // Load from file if specified
        if (!filepath.empty()) {
            success = loadFromFile(filepath);
            if (!success) {
                NLM_LOG_WARN("Failed to load configuration from file: " + filepath);
                // Try default locations
                if (filepath != "configs/default.cfg" && 
                    loadFromFile("configs/default.cfg")) {
                    NLM_LOG_INFO("Loaded default configuration from configs/default.cfg");
                }
            }
        }
        
        // Load from environment variables
        success = loadFromEnvironment() && success;
        
        // Validate configuration
        if (!validate()) {
            NLM_LOG_ERROR("Configuration validation failed");
            for (const auto& error : getValidationErrors()) {
                NLM_LOG_ERROR("Validation error: Key='" + error.getKey() + "', Message='" + 
                           error.getMessage() + "', Type=" + 
                           std::to_string(static_cast<int>(error.getErrorType())));
            }
            return false;
        }
        
        NLM_LOG_INFO("Configuration loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during configuration loading: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->config.clear();
    pImpl->loadedFromFile = filepath;
    
    try {
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            line = ConfigUtils::trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = ConfigUtils::trim(line.substr(0, pos));
                std::string value = ConfigUtils::trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::loadFromEnvironment() {
    try {
        const char* envRandomSeed = std::getenv("NLM_RANDOM_SEED");
        if (envRandomSeed) {
            set("random_seed", ConfigUtils::parseInt(envRandomSeed));
        }
        
        const char* envNeuronCount = std::getenv("NLM_NEURON_COUNT");
        if (envNeuronCount) {
            set("neuron_count", ConfigUtils::parseInt(envNeuronCount));
        }
        
        const char* envConnectionProb = std::getenv("NLM_CONNECTION_PROBABILITY");
        if (envConnectionProb) {
            set("connection_probability", ConfigUtils::parseDouble(envConnectionProb));
        }
        
        const char* envDevelopment = std::getenv("NLM_ENABLE_DEVELOPMENT");
        if (envDevelopment) {
            set("enable_development", ConfigUtils::parseBool(envDevelopment));
        }
        
        const char* envLogLevel = std::getenv("NLM_LOG_LEVEL");
        if (envLogLevel) {
            set("log_level", std::string(envLogLevel));
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading from environment variables: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::loadFromCommandLine(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value);
        }
    }
    return true;
}

void ConfigurationManager::set(const std::string& key, const ConfigValue& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check for duplicates in file-based configs
    if (!pImpl->loadedFromFile.empty() && pImpl->config.has(key)) {
        auto source = pImpl->config.getOptional<ConfigSource>(key);
        if (source && source.value() == ConfigSource::File) {
            // Override file-based configuration with new value
            pImpl->config.remove(key);
        }
    }
    
    pImpl->config.set(key, value, ConfigSource::Runtime);
    
    // Call update callback if enabled
    if (dynamicUpdatesEnabled_ && updateCallback_) {
        updateCallback_(key, value);
    }
    
    // Update validation errors
    validateCurrent();
}

void ConfigurationManager::set(const std::string& key, const std::string& value) {
    set(key, ConfigValue(value));
}

void ConfigurationManager::set(const std::string& key, int value) {
    set(key, ConfigValue(value));
}

void ConfigurationManager::set(const std::string& key, int64_t value) {
    set(key, ConfigValue(value));
}

void ConfigurationManager::set(const std::string& key, double value) {
    set(key, ConfigValue(value));
}

void ConfigurationManager::set(const std::string& key, bool value) {
    set(key, ConfigValue(value));
}

bool ConfigurationManager::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->config.has(key);
}

std::vector<std::string> ConfigurationManager::getAllKeys() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->config.getKeys();
}

std::string ConfigurationManager::getSummary() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->config.summary();
}

void ConfigurationManager::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->config.clear();
    validateCurrent();
}

bool ConfigurationManager::validate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return validateCurrent();
}

bool ConfigurationManager::validateCurrent() const {
    std::vector<ConfigValidationError> errors;
    
    // Validate against schema
    if (!schema_.validate(pImpl->config)) {
        std::string errorMsg = schema_.getValidationError(pImpl->config);
        // Parse the error message to create structured errors
        // For simplicity, we'll create a single error
        errors.emplace_back(ConfigValidationError::ErrorType::INVALID_TYPE, 
                          "unknown", errorMsg);
    }
    
    // Additional validation rules
    auto keys = pImpl->config.getKeys();
    std::unordered_set<std::string> uniqueKeys(keys.begin(), keys.end());
    
    if (keys.size() != uniqueKeys.size()) {
        errors.emplace_back(ConfigValidationError::ErrorType::DUPLICATE_KEY,
                          "multiple", "Duplicate configuration keys found");
    }
    
    // Validate specific value ranges
    if (has("neuron_count")) {
        auto neuronCount = getOptional<size_t>("neuron_count");
        if (neuronCount && (*neuronCount > 100000)) {
            errors.emplace_back(ConfigValidationError::ErrorType::OUT_OF_RANGE,
                              "neuron_count", 
                              "Neuron count exceeds maximum allowed (100,000)");
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = getOptional<double>("connection_probability");
        if (connProb && (*connProb < 0.0 || *connProb > 1.0)) {
            errors.emplace_back(ConfigValidationError::ErrorType::OUT_OF_RANGE,
                              "connection_probability",
                              "Connection probability must be between 0.0 and 1.0");
        }
    }
    
    // Update validation errors
    validationErrors_ = errors;
    
    if (!errors.empty()) {
        NLM_LOG_WARN("Configuration validation found " + std::to_string(errors.size()) + 
                   " errors");
    }
    
    return errors.empty();
}

bool ConfigurationManager::saveToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated on " << std::chrono::system_clock::now() << "\n\n";
        
        for (const auto& key : pImpl->config.getKeys()) {
            auto value = pImpl->config.getOptional<std::string>(key);
            if (value) {
                file << key << " = " << *value << "\n";
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::saveToJSON(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        nlohmann::json jsonConfig;
        
        for (const auto& key : pImpl->config.getKeys()) {
            jsonConfig[key] = ConfigUtils::toString(pImpl->config.getOptional<ConfigValue>(key).value());
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << jsonConfig << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to JSON: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::saveToYAML(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& key : pImpl->config.getKeys()) {
            emitter << YAML::Key << key;
            emitter << YAML::Value << ConfigUtils::toString(pImpl->config.getOptional<ConfigValue>(key).value());
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to YAML: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::loadProfile(const std::string& profileName) {
    std::string filepath = "configs/profiles/" + profileName + ".cfg";
    return loadFromFile(filepath);
}

bool ConfigurationManager::saveProfile(const std::string& profileName) {
    std::string filepath = "configs/profiles/" + profileName + ".cfg";
    return saveToFile(filepath);
}

std::vector<std::string> ConfigurationManager::getAvailableProfiles() const {
    std::vector<std::string> profiles;
    std::string profilesDir = "configs/profiles/";
    
    if (std::filesystem::exists(profilesDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(profilesDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
                profiles.push_back(entry.path().stem().string());
            }
        }
    }
    
    return profiles;
}

void ConfigurationManager::enableDynamicUpdates(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    dynamicUpdatesEnabled_ = enabled;
}

void ConfigurationManager::setUpdateCallback(std::function<void(const std::string&, const ConfigValue&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    updateCallback_ = callback;
}

ConfigurationManager& ConfigurationManager::getInstance() {
    static ConfigurationManager instance;
    return instance;
}

void ConfigurationManager::resetInstance() {
    // Note: This is mainly for testing; in production, we typically want a singleton
    // that persists throughout the application lifetime.
}

// ConfigSchema implementation
void ConfigSchema::addField(const Field& field) {
    fields_.push_back(field);
    fieldMap_[field.name] = field;
}

bool ConfigSchema::validate(const Config& config) const {
    std::string errorMsg = getValidationError(config);
    return errorMsg.empty();
}

std::string ConfigSchema::getValidationError(const Config& config) const {
    std::string errors;
    
    for (const auto& field : fields_) {
        std::string error = validateField(field, ""); // We'll need to get the actual value from config
        if (!error.empty()) {
            if (!errors.empty()) errors += "; ";
            errors += field.name + ": " + error;
        }
    }
    
    return errors;
}

ConfigSchema ConfigSchema::createNeuralSimulationSchema() {
    ConfigSchema schema;
    
    // Add all the fields from the default schema
    return schema.createDefaultSchema();
}

ConfigSchema ConfigSchema::createDefaultSchema() {
    ConfigSchema schema;
    
    // Basic simulation settings
    schema.addField({"random_seed", "int", "Random seed for reproducible experiments", false, "42"});
    schema.addField({"simulation_timestep", "double", "Simulation timestep in seconds", false, "0.001"});
    schema.addField({"neuron_count", "int", "Number of neurons in the brain", false, "1000"});
    schema.addField({"region_count", "int", "Number of brain regions", false, "2"});
    schema.addField({"connection_probability", "double", "Connection probability for random connectivity", false, "0.1"});
    
    // Plasticity settings
    schema.addField({"plasticity_learning_rate", "double", "Plasticity learning rate", false, "0.01"});
    schema.addField({"stdp_ltp_weight", "double", "STDP LTP weight", false, "0.01"});
    schema.addField({"stdp_ltd_weight", "double", "STDP LTD weight", false, "0.012"});
    schema.addField({"stdp_time_constant", "double", "STDP time constant", false, "20.0"});
    
    // Development settings
    schema.addField({"development_synaptogenesis_rate", "double", "Synaptogenesis rate during development", false, "0.001"});
    schema.addField({"development_pruning_rate", "double", "Pruning rate during development", false, "0.0001"});
    
    // Neuromodulation settings
    schema.addField({"dopamine_baseline", "double", "Baseline dopamine level", false, "0.1"});
    
    // Reward settings
    schema.addField({"reward_discount_factor", "double", "Reward discount factor", false, "0.99"});
    
    // Environment settings
    schema.addField({"environment_name", "string", "Environment name", false, "GridWorld"});
    schema.addField({"environment_width", "int", "Environment width", false, "10"});
    schema.addField({"environment_height", "int", "Environment height", false, "10"});
    
    // Logging settings
    schema.addField({"log_level", "string", "Log level (DEBUG, INFO, WARN, ERROR)", false, "INFO"});
    schema.addField({"log_to_file", "bool", "Whether to log to file", false, "false"});
    schema.addField({"log_filename", "string", "Log filename", false, "nlm.log"});
    
    // Simulation settings
    schema.addField({"max_simulation_steps", "int", "Maximum simulation steps", false, "10000"});
    schema.addField({"simulation_time_limit", "double", "Simulation time limit in seconds", false, "0.0"});
    
    // Visualization settings
    schema.addField({"visualization_enabled", "bool", "Whether visualization is enabled", false, "false"});
    schema.addField({"visualization_update_rate", "double", "Visualization update rate in Hz", false, "30.0"});
    
    return schema;
}

std::string ConfigSchema::validateField(const Field& field, const std::string& value) const {
    // This is a simplified implementation; in practice, we'd need to get the actual value from the config
    // For now, we'll return a generic error if the field is missing
    return ""; // TODO: Implement proper validation
}

bool ConfigSchema::validateType(const std::string& expectedType, const std::string& value) const {
    if (expectedType == "int") {
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isdigit);
    } else if (expectedType == "double") {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (expectedType == "bool") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    }
    return true; // string type accepts any value
}

// ConfigUtils implementation
std::string ConfigUtils::toString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        }
        return "";
    }, value);
}

bool ConfigUtils::parseBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

double ConfigUtils::parseDouble(const std::string& str) {
    return std::stod(str);
}

int64_t ConfigUtils::parseInt(const std::string& str) {
    return std::stoll(str);
}

std::string ConfigUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigUtils::isEmpty(const std::string& str) {
    return str.empty() || std::all_of(str.begin(), str.end(), ::isspace);
}
