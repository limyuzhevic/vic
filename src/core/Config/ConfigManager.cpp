#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <unordered_set>
#include <thread>

namespace nlm {

struct ConfigManager::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schema;
    ConfigCategory currentCategory;
    ConfigManager::Profile profile;
    std::vector<nlohmann::json> history;
    
    Impl() : currentCategory(ConfigCategory::Basic), profile(ConfigManager::Profile::Release) {
        initializeSchema();
        applyProfileDefaults();
        addConfigSnapshot("initial");
    }
    
    void initializeSchema() {
        // Basic category schemas
        schema["neuronCount"] = {"neuronCount", "int", "Number of neurons", ConfigCategory::Neural,
                               "^[0-9]+", 1, 1000000, 1000, true, {}};
        schema["learningRate"] = {"learningRate", "double", "Base learning rate", ConfigCategory::Advanced,
                               "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.01, true, {}};
        schema["timeSteps"] = {"timeSteps", "int", "Simulation time steps", ConfigCategory::Basic,
                               "^[0-9]+", 1, 1000000, 100, true, {}};
        
        // Advanced category schemas
        schema["stdpLTPWeight"] = {"stdpLTPWeight", "double", "STDP long-term potentiation weight",
                                 ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.1, true, {}};
        schema["stdpLTDWeight"] = {"stdpLTDWeight", "double", "STDP long-term depression weight",
                                 ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.12, true, {}};
        schema["stdpTimeConstant"] = {"stdpTimeConstant", "double", "STDP time constant (ms)",
                                    ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 1.0, 1000.0, 20.0, true, {}};
        schema["plasticityLearningRate"] = {"plasticityLearningRate", "double", "Plasticity learning rate",
                                         ConfigCategory::Advanced, "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.05, true, {}};
        
        // Performance category schemas
        schema["threadCount"] = {"threadCount", "int", "Number of threads for parallel processing",
                                ConfigCategory::Performance, "^[0-9]+", 1, 32, 4, true, {}};
        schema["simdEnabled"] = {"simdEnabled", "bool", "Enable SIMD optimizations", ConfigCategory::Performance,
                                "^(true|false|1|0)$", 0.0, 1.0, true, true, {"true", "false"}};
        schema["memoryPoolSize"] = {"memoryPoolSize", "int", "Size of memory pool for neurons/synapses",
                                  ConfigCategory::Performance, "^[0-9]+", 1000, 1000000, 65536, true, {}};
        schema["optimizationFlags"] = {"optimizationFlags", "string", "Performance optimization flags",
                                     ConfigCategory::Performance, ".*", 0.0, 1.0, "default", false, {}};
        
        // Development category schemas
        schema["debugLevel"] = {"debugLevel", "int", "Debug logging level (0-3)", ConfigCategory::Development,
                               "^[0-3]", 0, 3, 1, true, {"0", "1", "2", "3"}};
        schema["profilingEnabled"] = {"profilingEnabled", "bool", "Enable profiling", ConfigCategory::Development,
                                    "^(true|false|1|0)$", 0.0, 1.0, false, true, {"true", "false"}};
        schema["validationEnabled"] = {"validationEnabled", "bool", "Enable validation", ConfigCategory::Development,
                                     "^(true|false|1|0)$", 0.0, 1.0, true, true, {"true", "false"}};
        
        // Neural category schemas
        schema["synapticDensity"] = {"synapticDensity", "double", "Synaptic density (0-1)", ConfigCategory::Neural,
                                   "^[0-9]*\.?[0-9]+", 0.0, 1.0, 0.1, true, {}};
        schema["populationSize"] = {"populationSize", "int", "Default population size", ConfigCategory::Neural,
                                   "^[0-9]+", 1, 10000, 100, true, {}};
        schema["regionSize"] = {"regionSize", "double", "Region size for neural regions", ConfigCategory::Neural,
                               "^[0-9]*\.?[0-9]+", 0.0, 100.0, 1.0, true, {}};
        schema["neuronTypes"] = {"neuronTypes", "string", "Comma-separated list of neuron types", ConfigCategory::Neural,
                                ".*", 0.0, 1.0, "excitatory,inhibitory", false, {}};
    }
    
    void applyProfileDefaults() {
        switch (profile) {
            case ConfigManager::Profile::Debug:
                set("debugLevel", 2, ConfigSource::Default);
                set("profilingEnabled", true, ConfigSource::Default);
                set("validationEnabled", true, ConfigSource::Default);
                break;
            case ConfigManager::Profile::Benchmark:
                set("profilingEnabled", true, ConfigSource::Default);
                set("threadCount", std::thread::hardware_concurrency(), ConfigSource::Default);
                break;
            case ConfigManager::Profile::Release:
            default:
                set("debugLevel", 1, ConfigSource::Default);
                set("profilingEnabled", false, ConfigSource::Default);
                set("validationEnabled", true, ConfigSource::Default);
                break;
        }
    }
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {}

ConfigManager::~ConfigManager() = default;

void ConfigManager::setProfile(Profile profile) {
    pImpl->profile = profile;
    applyProfileDefaults();
}

ConfigManager::Profile ConfigManager::getProfile() const {
    return pImpl->profile;
}

void ConfigManager::applyProfileDefaults() {
    pImpl->applyProfileDefaults();
}

void ConfigManager::addAdvancedConfig() {
    set("stdpLTPWeight", DefaultConfig::ADVANCED_STDP_LTP_WEIGHT, ConfigSource::Default);
    set("stdpLTDWeight", DefaultConfig::ADVANCED_STDP_LTD_WEIGHT, ConfigSource::Default);
    set("stdpTimeConstant", DefaultConfig::ADVANCED_STDP_TIME_CONST, ConfigSource::Default);
    set("plasticityLearningRate", DefaultConfig::ADVANCED_LEARNING_RATE, ConfigSource::Default);
}

void ConfigManager::addPerformanceConfig() {
    set("threadCount", DefaultConfig::PERFORMANCE_THREAD_COUNT, ConfigSource::Default);
    set("simdEnabled", DefaultConfig::PERFORMANCE_SIMD_ENABLED, ConfigSource::Default);
    set("memoryPoolSize", DefaultConfig::PERFORMANCE_MEMORY_POOL_SIZE, ConfigSource::Default);
    set("optimizationFlags", "default", ConfigSource::Default);
}

void ConfigManager::addDevelopmentConfig() {
    set("debugLevel", DefaultConfig::DEVELOPMENT_DEBUG_LEVEL, ConfigSource::Default);
    set("profilingEnabled", DefaultConfig::DEVELOPMENT_PROFILING_ENABLED, ConfigSource::Default);
    set("validationEnabled", DefaultConfig::DEVELOPMENT_VALIDATION_ENABLED, ConfigSource::Default);
}

void ConfigManager::addNeuralConfig() {
    set("synapticDensity", DefaultConfig::NEURAL_SYNAPTIC_DENSITY, ConfigSource::Default);
    set("populationSize", DefaultConfig::NEURAL_POPULATION_SIZE, ConfigSource::Default);
    set("regionSize", DefaultConfig::NEURAL_REGION_SIZE, ConfigSource::Default);
    set("neuronTypes", DefaultConfig::NEURAL_DEFAULT_TYPES, ConfigSource::Default);
}

bool ConfigManager::validateConfig(const ConfigEntry& entry) const {
    return validateConfigValue(entry).isValid;
}

bool ConfigManager::validateAllConfig() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateConfigValue(entry).isValid) {
            return false;
        }
    }
    return true;
}

nlohmann::json ConfigManager::serialize(const ConfigEntry& entry) {
    nlohmann::json json;
    json["key"] = entry.key;
    json["source"] = static_cast<int>(entry.source);
    json["category"] = static_cast<int>(entry.category);
    json["description"] = entry.description;
    
    // Serialize value based on type
    if (std::holds_alternative<int>(entry.value)) {
        json["type"] = "int";
        json["intValue"] = std::get<int>(entry.value);
    } else if (std::holds_alternative<int64_t>(entry.value)) {
        json["type"] = "int64_t";
        json["int64Value"] = std::get<int64_t>(entry.value);
    } else if (std::holds_alternative<double>(entry.value)) {
        json["type"] = "double";
        json["doubleValue"] = std::get<double>(entry.value);
    } else if (std::holds_alternative<bool>(entry.value)) {
        json["type"] = "bool";
        json["boolValue"] = std::get<bool>(entry.value);
    } else if (std::holds_alternative<std::string>(entry.value)) {
        json["type"] = "string";
        json["stringValue"] = std::get<std::string>(entry.value);
    }
    
    return json;
}

ConfigEntry ConfigManager::deserialize(const nlohmann::json& data) {
    ConfigEntry entry;
    entry.key = data.value("key", "");
    entry.source = static_cast<ConfigSource>(data.value("source", 0));
    entry.category = static_cast<ConfigCategory>(data.value("category", 0));
    entry.description = data.value("description", "");
    
    // Deserialize value based on type
    std::string type = data.value("type", "string");
    if (type == "int") {
        entry.value = data.value("intValue", 0);
    } else if (type == "int64_t") {
        entry.value = data.value("int64Value", int64_t(0));
    } else if (type == "double") {
        entry.value = data.value("doubleValue", 0.0);
    } else if (type == "bool") {
        entry.value = data.value("boolValue", false);
    } else {
        entry.value = data.value("stringValue", "");
    }
    
    return entry;
}

bool ConfigManager::backupConfigFile(const std::string& filepath) {
    try {
        std::filesystem::path path(filepath);
        std::filesystem::path backupPath = path;
        
        if (path.extension() == ".json") {
            backupPath += ConfigUtils::JSON_BACKUP_SUFFIX;
        } else {
            backupPath += ConfigUtils::BACKUP_SUFFIX;
        }
        
        std::filesystem::copy_file(path, backupPath, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigManager::loadFromParentProfile(const ConfigManager& parent) {
    // Merge parent configuration with current
    for (const auto& entry : parent.pImpl->entries) {
        if (!has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
    return true;
}

nlohmann::json ConfigManager::getDefaultTemplate(ConfigCategory category) {
    switch (category) {
        case ConfigCategory::Basic:
            return ConfigUtils::DEFAULT_BASIC_CONFIG["configuration"];
        case ConfigCategory::Advanced:
            return ConfigUtils::DEFAULT_ADVANCED_CONFIG["configuration"];
        case ConfigCategory::Performance:
            return ConfigUtils::DEFAULT_PERFORMANCE_CONFIG["configuration"];
        case ConfigCategory::Development:
            return ConfigUtils::DEFAULT_DEVELOPMENT_CONFIG["configuration"];
        case ConfigCategory::Neural:
            return ConfigUtils::DEFAULT_NEURAL_CONFIG["configuration"];
        case ConfigCategory::Experiment:
            return nlohmann::json::object();
        default:
            return nlohmann::json::object();
    }
}

std::vector<ConfigEntry> ConfigManager::getConfigByCategory(ConfigCategory category) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            result.push_back(entry);
        }
    }
    return result;
}

ConfigManager::ValidationResult ConfigManager::validateConfigValue(const ConfigEntry& entry) const {
    ValidationResult result;
    result.isValid = true;
    
    // Check if key exists in schema
    auto schemaIt = pImpl->schema.find(entry.key);
    if (schemaIt == pImpl->schema.end()) {
        result.errors.push_back("Key not found in configuration schema: " + entry.key);
        result.isValid = false;
        return result;
    }
    
    const ConfigSchema& schema = schemaIt->second;
    
    // Validate based on type
    bool typeValid = false;
    if (std::holds_alternative<int>(entry.value) && schema.type == "int") {
        typeValid = true;
        int value = std::get<int>(entry.value);
        if (schema.minValue && value < *schema.minValue) {
            result.errors.push_back("Value " + std::to_string(value) + " is below minimum " + std::to_string(*schema.minValue));
            result.isValid = false;
        }
        if (schema.maxValue && value > *schema.maxValue) {
            result.errors.push_back("Value " + std::to_string(value) + " exceeds maximum " + std::to_string(*schema.maxValue));
            result.isValid = false;
        }
    } else if (std::holds_alternative<int64_t>(entry.value) && schema.type == "int64_t") {
        typeValid = true;
    } else if (std::holds_alternative<double>(entry.value) && schema.type == "double") {
        typeValid = true;
        double value = std::get<double>(entry.value);
        if (schema.minValue && value < *schema.minValue) {
            result.errors.push_back("Value " + std::to_string(value) + " is below minimum " + std::to_string(*schema.minValue));
            result.isValid = false;
        }
        if (schema.maxValue && value > *schema.maxValue) {
            result.errors.push_back("Value " + std::to_string(value) + " exceeds maximum " + std::to_string(*schema.maxValue));
            result.isValid = false;
        }
    } else if (std::holds_alternative<bool>(entry.value) && schema.type == "bool") {
        typeValid = true;
    } else if (std::holds_alternative<std::string>(entry.value) && schema.type == "string") {
        typeValid = true;
        if (!schema.validationPattern.empty()) {
            std::regex pattern(schema.validationPattern);
            if (!std::regex_match(std::get<std::string>(entry.value), pattern)) {
                result.errors.push_back("Value does not match pattern: " + schema.validationPattern);
                result.isValid = false;
            }
        }
        if (!schema.validValues.empty()) {
            bool validEnum = false;
            for (const auto& validValue : schema.validValues) {
                if (std::get<std::string>(entry.value) == validValue) {
                    validEnum = true;
                    break;
                }
            }
            if (!validEnum) {
                result.errors.push_back("Value not in valid enum values");
                result.isValid = false;
            }
        }
    }
    
    if (!typeValid) {
        result.errors.push_back("Type mismatch or value type not supported");
        result.isValid = false;
    }
    
    return result;
}

bool ConfigManager::isCompatible(const ConfigManager& other) const {
    // Simple compatibility check based on hash
    return ConfigUtils::computeConfigHash(serializeConfig()) == ConfigUtils::computeConfigHash(other.serializeConfig());
}

void ConfigManager::addConfigSnapshot(const std::string& snapshotName) {
    nlohmann::json snapshot;
    snapshot["name"] = snapshotName;
    snapshot["timestamp"] = Config::getCurrentTimestamp();
    snapshot["config"] = serializeConfig();
    
    pImpl->history.push_back(snapshot);
}

nlohmann::json ConfigManager::getConfigHistory() const {
    nlohmann::json history;
    for (const auto& snapshot : pImpl->history) {
        history.push_back(snapshot);
    }
    return history;
}

// Implementation details
nlohmann::json ConfigManager::serializeConfig() const {
    nlohmann::json config;
    auto& entries = config["entries"];
    
    for (const auto& entry : pImpl->entries) {
        entries.push_back(serialize(entry));
    }
    
    config["profile"] = static_cast<int>(pImpl->profile);
    config["category"] = static_cast<int>(pImpl->currentCategory);
    
    return config;
}

// ConfigValidatorImpl implementation
ConfigValidatorImpl::ConfigValidatorImpl(ConfigManager::Profile profile) : profile_(profile) {}

bool ConfigValidatorImpl::validate(const ConfigEntry& entry) const {
    ValidationResult result = validateFull(entry);
    errorMessage_ = result.message;
    return result.isValid;
}

std::string ConfigValidatorImpl::getErrorMessage() const {
    return errorMessage_;
}

ConfigManager::ValidationResult ConfigValidatorImpl::validateFull(const ConfigEntry& entry) const {
    ValidationResult result;
    result.isValid = true;
    
    // Profile-specific validation
    if (profile_ == ConfigManager::Profile::Debug && entry.key == "debugLevel") {
        int value = std::get<int>(entry.value);
        if (value < 0 || value > 3) {
            result.errors.push_back("Debug level must be between 0 and 3");
            result.isValid = false;
        }
    }
    
    return result;
}

// ConfigUtils implementation
namespace ConfigUtils {
    bool validateIntRange(int value, int min, int max) {
        return value >= min && value <= max;
    }
    
    bool validateDoubleRange(double value, double min, double max) {
        return value >= min && value <= max;
    }
    
    bool validateStringPattern(const std::string& value, const std::string& pattern) {
        std::regex regex(pattern);
        return std::regex_match(value, regex);
    }
    
    bool validateEnum(const std::string& value, const std::vector<std::string>& validValues) {
        return std::find(validValues.begin(), validValues.end(), value) != validValues.end();
    }
    
    std::string getTimestampedFilename(const std::string& originalPath) {
        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        
        std::filesystem::path path(originalPath);
        return path.stem().string() + "_" + ss.str() + path.extension().string();
    }
    
    bool validateJSONStructure(const nlohmann::json& jsonData) {
        try {
            // Basic validation - check if it's a valid JSON object
            if (!jsonData.is_object()) {
                return false;
            }
            
            // Check required fields
            if (!jsonData.contains("format")) {
                return false;
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    nlohmann::json mergeConfigs(const nlohmann::json& base, const nlohmann::json& override) {
        nlohmann::json result = base;
        
        for (auto it = override.begin(); it != override.end(); ++it) {
            if (it.key() == "configuration") {
                // Merge configuration entries
                auto& baseConfig = result["configuration"];
                auto& overrideConfig = it.value();
                
                for (auto entryIt = overrideConfig.begin(); entryIt != overrideConfig.end(); ++entryIt) {
                    baseConfig[entryIt.key()] = entryIt.value();
                }
            } else {
                result[it.key()] = it.value();
            }
        }
        
        return result;
    }
    
    std::string computeConfigHash(const nlohmann::json& config) {
        std::hash<std::string> hasher;
        std::string configStr = config.dump();
        return std::to_string(hasher(configStr));
    }
}

} // namespace nlm
