#include "Config.hpp"
#include <nlohmann/json-schema.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <cassert>

using json = nlohmann::json;
namespace nlm {

// Configuration schema validation
class ConfigSchema {
public:
    using Validator = std::function<bool(const ConfigValue& value)>;
    
    struct Field {
        std::string name;
        std::string type;
        bool required;
        std::string defaultValue;
        Validator validator;
        std::string description;
        
        Field(std::string name, std::string type, bool required = false,
              std::string defaultValue = "", 
              std::string description = "")
            : name(std::move(name)), type(std::move(type)), required(required),
              defaultValue(std::move(defaultValue)), description(std::move(description)) {}
        
        bool validate(const ConfigValue& value) const {
            if (!validator) return true;
            return validator(value);
        }
    };
    
    void addField(const Field& field) {
        fields_.push_back(field);
        if (!field.defaultValue.empty()) {
            // Set default value during initialization
            Config::getGlobal().set(field.name, field.defaultValue, ConfigSource::Default);
        }
    }
    
    bool validate(const Config& config) const {
        std::vector<std::string> errors;
        
        for (const auto& field : fields_) {
            if (field.required && !config.has(field.name)) {
                errors.push_back("Missing required field: " + field.name);
                continue;
            }
            
            if (config.has(field.name)) {
                auto value = config.get<std::string>(field.name);
                if (!field.validate(ConfigValue(value.value_or("")))) {
                    errors.push_back("Invalid value for field " + field.name + ": " + *value);
                }
            }
        }
        
        if (!errors.empty()) {
            std::stringstream ss;
            ss << "Configuration validation failed:";
            for (const auto& error : errors) {
                ss << "\n  - " << error;
            }
            NLM_LOG_ERROR(ss.str());
            return false;
        }
        
        return true;
    }
    
    std::vector<std::string> getErrors(const Config& config) const {
        std::vector<std::string> errors;
        
        for (const auto& field : fields_) {
            if (field.required && !config.has(field.name)) {
                errors.push_back("Missing required field: " + field.name);
                continue;
            }
            
            if (config.has(field.name)) {
                auto value = config.get<std::string>(field.name);
                if (!field.validate(ConfigValue(value.value_or("")))) {
                    errors.push_back("Invalid value for field " + field.name + ": " + *value);
                }
            }
        }
        
        return errors;
    }
    
    std::string getFieldDescription(const std::string& fieldName) const {
        for (const auto& field : fields_) {
            if (field.name == fieldName) {
                return field.description;
            }
        }
        return "";
    }
    
    std::vector<std::string> getAllFieldNames() const {
        std::vector<std::string> names;
        for (const auto& field : fields_) {
            names.push_back(field.name);
        }
        return names;
    }
    
    bool hasField(const std::string& fieldName) const {
        for (const auto& field : fields_) {
            if (field.name == fieldName) {
                return true;
            }
        }
        return false;
    }

private:
    std::vector<Field> fields_;
};

// Global configuration schema
static ConfigSchema& getGlobalSchema() {
    static ConfigSchema schema;
    return schema;
}

// Environment variable integration
class EnvironmentVariableReader {
public:
    static std::optional<std::string> getEnvVar(const std::string& name) {
        const char* value = std::getenv(name.c_str());
        if (value == nullptr) {
            return std::nullopt;
        }
        return std::string(value);
    }
    
    static std::string getEnvVarWithDefault(const std::string& name, const std::string& defaultValue = "") {
        auto value = getEnvVar(name);
        return value.value_or(defaultValue);
    }
    
    static bool isEnvVarSet(const std::string& name) {
        return getEnvVar(name).has_value();
    }
    
    // Parse environment variable with type conversion
    static std::optional<int> getEnvInt(const std::string& name) {
        auto value = getEnvVar(name);
        if (!value) return std::nullopt;
        
        try {
            return std::stoi(*value);
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }
    
    static std::optional<double> getEnvDouble(const std::string& name) {
        auto value = getEnvVar(name);
        if (!value) return std::nullopt;
        
        try {
            return std::stod(*value);
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }
    
    static std::optional<bool> getEnvBool(const std::string& name) {
        auto value = getEnvVar(name);
        if (!value) return std::nullopt;
        
        std::string lower = value.value();
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            return false;
        }
        
        return std::nullopt;
    }
};

// Configuration factory
class ConfigFactory {
public:
    static std::shared_ptr<Config> createFromEnvironment(const std::unordered_map<std::string, std::string>& envPrefixMap = {}) {
        auto config = std::make_shared<Config>();
        
        // Load from environment variables
        for (const auto& [prefix, mapping] : envPrefixMap) {
            std::string prefixUpper = prefix;
            std::transform(prefixUpper.begin(), prefixUpper.end(), prefixUpper.begin(), ::toupper);
            
            for (const auto& [envName, configKey] : mapping) {
                std::string fullName = prefixUpper + "_" + envName;
                auto value = EnvironmentVariableReader::getEnvVar(fullName);
                if (value) {
                    config->set(configKey, *value, ConfigSource::Runtime);
                }
            }
        }
        
        return config;
    }
    
    static std::shared_ptr<Config> createFromFiles(const std::vector<std::string>& filePaths) {
        auto config = std::make_shared<Config>();
        
        for (const auto& filepath : filePaths) {
            std::string ext = getFileExtension(filepath);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            
            if (ext == "json") {
                auto fileConfig = Config::createFromJSON(filepath);
                mergeConfigs(config, fileConfig);
            } else if (ext == "yaml" || ext == "yml") {
                auto fileConfig = Config::createFromYAML(filepath);
                mergeConfigs(config, fileConfig);
            } else {
                // Fall back to simple key=value format
                if (config->loadFromFile(filepath)) {
                    NLM_LOG_INFO("Loaded configuration from: " + filepath);
                } else {
                    NLM_LOG_WARNING("Failed to load configuration from: " + filepath);
                }
            }
        }
        
        return config;
    }
    
    static std::shared_ptr<Config> createFromJSON(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (config->loadFromJSON(filepath)) {
            return config;
        }
        return nullptr;
    }
    
    static std::shared_ptr<Config> createFromYAML(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (config->loadFromYAML(filepath)) {
            return config;
        }
        return nullptr;
    }
    
    static std::shared_ptr<Config> createDefault() {
        auto config = std::make_shared<Config>();
        
        // Set default values for common configuration options
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", uint64_t(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        config->set("environment_width", 100.0f, ConfigSource::Default);
        config->set("environment_height", 100.0f, ConfigSource::Default);
        
        return config;
    }
    
private:
    static void mergeConfigs(std::shared_ptr<Config> target, std::shared_ptr<Config> source) {
        if (!source) return;
        
        for (const auto& key : source->getKeys()) {
            auto value = source->get<std::string>(key);
            if (value) {
                target->set(key, *value, ConfigSource::File);
            } else {
                // Try other types
                auto intVal = source->get<int64_t>(key);
                if (intVal) {
                    target->set(key, *intVal, ConfigSource::File);
                    continue;
                }
                
                auto doubleVal = source->get<double>(key);
                if (doubleVal) {
                    target->set(key, *doubleVal, ConfigSource::File);
                    continue;
                }
                
                auto boolVal = source->get<bool>(key);
                if (boolVal) {
                    target->set(key, *boolVal, ConfigSource::File);
                    continue;
                }
            }
        }
    }
    
    static std::string getFileExtension(const std::string& filename) {
        if (filename.find_last_of('.') == std::string::npos) return "";
        return filename.substr(filename.find_last_of('.') + 1);
    }
};

// Global configuration manager
class GlobalConfigManager {
public:
    static GlobalConfigManager& getInstance() {
        static GlobalConfigManager instance;
        return instance;
    }
    
    void initialize(const std::vector<std::string>& configFiles = {}, 
                   const std::unordered_map<std::string, std::string>& envVars = {}) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (initialized_) {
            NLM_LOG_WARNING("GlobalConfigManager already initialized");
            return;
        }
        
        // Load configuration
        if (!configFiles.empty()) {
            config_ = ConfigFactory::createFromFiles(configFiles);
        } else {
            config_ = ConfigFactory::createDefault();
        }
        
        // Apply environment variables if provided
        if (!envVars.empty()) {
            for (const auto& [key, value] : envVars) {
                config_->set(key, value, ConfigSource::Runtime);
            }
        }
        
        // Validate configuration
        if (getGlobalSchema().validate(*config_)) {
            NLM_LOG_INFO("Global configuration initialized successfully");
        } else {
            NLM_LOG_ERROR("Global configuration validation failed");
        }
        
        initialized_ = true;
    }
    
    std::shared_ptr<Config> getConfig() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_;
    }
    
    void setConfig(std::shared_ptr<Config> config) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        initialized_ = true;
    }
    
    bool isInitialized() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return initialized_;
    }
    
    void reload() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (initialized_) {
            config_ = ConfigFactory::createDefault();
            NLM_LOG_INFO("Global configuration reloaded");
        }
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = ConfigFactory::createDefault();
        initialized_ = false;
        NLM_LOG_INFO("Global configuration reset");
    }

private:
    GlobalConfigManager() : initialized_(false) {}
    ~GlobalConfigManager() = default;
    
    GlobalConfigManager(const GlobalConfigManager&) = delete;
    GlobalConfigManager& operator=(const GlobalConfigManager&) = delete;
    
    mutable std::mutex mutex_;
    std::shared_ptr<Config> config_;
    bool initialized_;
};

// Convenience functions
namespace ConfigUtils {
    std::shared_ptr<Config> loadFromEnvironment(const std::string& prefix = "NLM") {
        std::unordered_map<std::string, std::string> mapping;
        
        // Map environment variable names to configuration keys
        mapping["NEURON_COUNT"] = "neuron_count";
        mapping["REGION_COUNT"] = "region_count";
        mapping["CONNECTION_PROB"] = "connection_probability";
        mapping["RANDOM_SEED"] = "random_seed";
        mapping["SIMULATION_TIMESTEP"] = "simulation_timestep";
        mapping["STDP_LTP_WEIGHT"] = "stdp_ltp_weight";
        mapping["STDP_LTD_WEIGHT"] = "stdp_ltd_weight";
        mapping["ENVIRONMENT_WIDTH"] = "environment_width";
        mapping["ENVIRONMENT_HEIGHT"] = "environment_height";
        
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> envPrefixMap;
        envPrefixMap[prefix] = mapping;
        
        return ConfigFactory::createFromEnvironment(envPrefixMap);
    }
    
    std::shared_ptr<Config> loadFromFiles(const std::vector<std::string>& files) {
        return ConfigFactory::createFromFiles(files);
    }
    
    std::shared_ptr<Config> loadDefault() {
        return ConfigFactory::createDefault();
    }
    
    bool saveToFiles(const std::shared_ptr<Config>& config, const std::vector<std::string>& files) {
        bool success = true;
        for (const auto& filepath : files) {
            std::string ext = getFileExtension(filepath);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            
            if (ext == "json") {
                if (!config->saveToJSON(filepath)) {
                    success = false;
                }
            } else if (ext == "yaml" || ext == "yml") {
                if (!config->saveToYAML(filepath)) {
                    success = false;
                }
            } else {
                if (!config->saveToFile(filepath)) {
                    success = false;
                }
            }
        }
        return success;
    }
    
    std::string getFileExtension(const std::string& filename) {
        if (filename.find_last_of('.') == std::string::npos) return "";
        return filename.substr(filename.find_last_of('.') + 1);
    }
}

} // namespace nlm