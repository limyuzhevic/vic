// Advanced NLM Configuration Management Implementation

#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace nlm {

bool AdvancedConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    try {
        json j;
        file >> j;
        return loadFromJSON(j);
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("Configuration file parse error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfigManager::loadFromJSON(const json& j) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    try {
        // Parse configuration values
        if (j.contains("neuron_count")) {
            config_->set("neuron_count", j["neuron_count"].get<int64_t>(), ConfigSource::File);
        }
        if (j.contains("region_count")) {
            config_->set("region_count", j["region_count"].get<int64_t>(), ConfigSource::File);
        }
        if (j.contains("connection_probability")) {
            config_->set("connection_probability", j["connection_probability"].get<float>(), ConfigSource::File);
        }
        if (j.contains("stdp_ltp_weight")) {
            config_->set("stdp_ltp_weight", j["stdp_ltp_weight"].get<float>(), ConfigSource::File);
        }
        if (j.contains("stdp_ltd_weight")) {
            config_->set("stdp_ltd_weight", j["stdp_ltd_weight"].get<float>(), ConfigSource::File);
        }
        if (j.contains("synaptogenesis_rate")) {
            config_->set("synaptogenesis_rate", j["synaptogenesis_rate"].get<float>(), ConfigSource::File);
        }
        if (j.contains("pruning_rate")) {
            config_->set("pruning_rate", j["pruning_rate"].get<float>(), ConfigSource::File);
        }
        if (j.contains("simulation_timestep")) {
            config_->set("simulation_timestep", j["simulation_timestep"].get<double>(), ConfigSource::File);
        }
        if (j.contains("max_steps")) {
            config_->set("max_steps", j["max_steps"].get<int64_t>(), ConfigSource::File);
        }
        if (j.contains("log_level")) {
            config_->set("log_level", j["log_level"].get<std::string>(), ConfigSource::File);
        }
        if (j.contains("enable_plasticity")) {
            config_->set("enable_plasticity", j["enable_plasticity"].get<bool>(), ConfigSource::File);
        }
        if (j.contains("enable_structural_plasticity")) {
            config_->set("enable_structural_plasticity", j["enable_structural_plasticity"].get<bool>(), ConfigSource::File);
        }
        if (j.contains("enable_development")) {
            config_->set("enable_development", j["enable_development"].get<bool>(), ConfigSource::File);
        }
        if (j.contains("enable_neuromodulation")) {
            config_->set("enable_neuromodulation", j["enable_neuromodulation"].get<bool>(), ConfigSource::File);
        }
        if (j.contains("enable_memory")) {
            config_->set("enable_memory", j["enable_memory"].get<bool>(), ConfigSource::File);
        }
        if (j.contains("enable_prediction")) {
            config_->set("enable_prediction", j["enable_prediction"].get<bool>(), ConfigSource::File);
        }
        if (j.contains("enable_cognition")) {
            config_->set("enable_cognition", j["enable_cognition"].get<bool>(), ConfigSource::File);
        }
        if (j.contains("environment_type")) {
            config_->set("environment_type", j["environment_type"].get<std::string>(), ConfigSource::File);
        }
        if (j.contains("environment_size")) {
            config_->set("environment_size", j["environment_size"].get<int64_t>(), ConfigSource::File);
        }
        
        hasChanges_ = true;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Configuration JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfigManager::loadFromCommandLine(int argc, char* argv[]) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    // Simple command line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 8) == "--neuron=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                int64_t value = std::stoll(arg.substr(pos + 1));
                config_->set("neuron_count", value, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 9) == "--region=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                int64_t value = std::stoll(arg.substr(pos + 1));
                config_->set("region_count", value, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 10) == "--steps=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                int64_t value = std::stoll(arg.substr(pos + 1));
                config_->set("max_steps", value, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 7) == "--seed=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                int64_t value = std::stoll(arg.substr(pos + 1));
                config_->set("random_seed", value, ConfigSource::CommandLine);
            }
        }
    }
    
    hasChanges_ = true;
    return true;
}

template<typename T>
T AdvancedConfigManager::get(const std::string& key, const T& defaultValue) const {
    if (!config_) {
        return defaultValue;
    }
    return config_->getOr<T>(key, defaultValue);
}

template<typename T>
T AdvancedConfigManager::get(const std::string& key, ConfigSource source) const {
    if (!config_) {
        throw std::runtime_error("Configuration not initialized");
    }
    return config_->getOr<T>(key);
}

template<typename T>
void AdvancedConfigManager::set(const std::string& key, const T& value, ConfigSource source) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    config_->set(key, value, source);
    hasChanges_ = true;
}

ValidationResult AdvancedConfigManager::validate() const {
    ValidationResult result;
    
    if (!config_) {
        result.addError("Configuration not initialized");
        return result;
    }
    
    // Validate required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "connection_probability"};
    for (const auto& key : requiredKeys) {
        if (schema_.find(key) != schema_.end()) {
            const auto& schema = schema_.at(key);
            if (schema.required && !config_->hasKey(key)) {
                result.addError("Required configuration key missing: " + key);
            }
        }
    }
    
    // Validate range and format for specific keys
    auto neuronCount = config_->getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0 || neuronCount > 100000) {
        result.addError("neuron_count must be between 1 and 100000");
    }
    
    auto regionCount = config_->getOr<int64_t>("region_count", 0);
    if (regionCount <= 0 || regionCount > 1000) {
        result.addError("region_count must be between 1 and 1000");
    }
    
    auto connectionProbability = config_->getOr<float>("connection_probability", 0.0f);
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        result.addError("connection_probability must be between 0.0 and 1.0");
    }
    
    auto simulationTimestep = config_->getOr<double>("simulation_timestep", 0.0);
    if (simulationTimestep <= 0.0 || simulationTimestep > 1.0) {
        result.addError("simulation_timestep must be between 0.0 and 1.0");
    }
    
    auto maxSteps = config_->getOr<int64_t>("max_steps", 0);
    if (maxSteps < 0 || maxSteps > 1000000) {
        result.addError("max_steps must be between 0 and 1000000");
    }
    
    // Validate against schema if available
    for (const auto& [key, schema] : schema_) {
        if (config_->hasKey(key)) {
            bool valid = validateValue(key, config_->getAsString(key));
            if (!valid) {
                result.addError("Invalid value for configuration key: " + key);
            }
        }
    }
    
    return result;
}

bool AdvancedConfigManager::isValid() const {
    return validate().isValid;
}

std::vector<std::string> AdvancedConfigManager::getErrors() const {
    return validate().errors;
}

std::vector<std::string> AdvancedConfigManager::getWarnings() const {
    return validate().warnings;
}

json AdvancedConfigManager::serialize() const {
    json j;
    
    if (!config_) {
        return j;
    }
    
    // Add all configuration values
    j["neuron_count"] = config_->getOr<int64_t>("neuron_count", 1000);
    j["region_count"] = config_->getOr<int64_t>("region_count", 1);
    j["connection_probability"] = config_->getOr<float>("connection_probability", 0.1f);
    j["stdp_ltp_weight"] = config_->getOr<float>("stdp_ltp_weight", 0.01f);
    j["stdp_ltd_weight"] = config_->getOr<float>("stdp_ltd_weight", 0.012f);
    j["synaptogenesis_rate"] = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
    j["pruning_rate"] = config_->getOr<float>("pruning_rate", 0.00001f);
    j["simulation_timestep"] = config_->getOr<double>("simulation_timestep", 0.001);
    j["max_steps"] = config_->getOr<int64_t>("max_steps", 10000);
    j["log_level"] = config_->getOr<std::string>("log_level", "INFO");
    
    // Add advanced options
    if (config_->hasKey("enable_plasticity")) {
        j["enable_plasticity"] = config_->getOr<bool>("enable_plasticity", true);
    }
    if (config_->hasKey("enable_structural_plasticity")) {
        j["enable_structural_plasticity"] = config_->getOr<bool>("enable_structural_plasticity", true);
    }
    if (config_->hasKey("enable_development")) {
        j["enable_development"] = config_->getOr<bool>("enable_development", true);
    }
    if (config_->hasKey("enable_neuromodulation")) {
        j["enable_neuromodulation"] = config_->getOr<bool>("enable_neuromodulation", true);
    }
    if (config_->hasKey("enable_memory")) {
        j["enable_memory"] = config_->getOr<bool>("enable_memory", true);
    }
    if (config_->hasKey("enable_prediction")) {
        j["enable_prediction"] = config_->getOr<bool>("enable_prediction", true);
    }
    if (config_->hasKey("enable_cognition")) {
        j["enable_cognition"] = config_->getOr<bool>("enable_cognition", true);
    }
    if (config_->hasKey("environment_type")) {
        j["environment_type"] = config_->getOr<std::string>("environment_type", "GridWorld");
    }
    if (config_->hasKey("environment_size")) {
        j["environment_size"] = config_->getOr<int64_t>("environment_size", 10);
    }
    
    return j;
}

bool AdvancedConfigManager::serializeToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j = serialize();
        file << j.dump(4); // Pretty print with indentation
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Configuration serialization error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfigManager::deserializeFromFile(const std::string& filepath) {
    return loadFromFile(filepath);
}

bool AdvancedConfigManager::deserializeFromJSON(const json& j) {
    return loadFromJSON(j);
}

std::vector<std::string> AdvancedConfigManager::getAllKeys() const {
    if (!config_) {
        return {};
    }
    return config_->getAllKeys();
}

std::vector<std::string> AdvancedConfigManager::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> result;
    
    for (const auto& key : getAllKeys()) {
        if (getKeyCategory(key) == category) {
            result.push_back(key);
        }
    }
    
    return result;
}

std::vector<std::string> AdvancedConfigManager::getAdvancedKeys() const {
    std::vector<std::string> result;
    
    for (const auto& key : getAllKeys()) {
        if (schema_.find(key) != schema_.end() && schema_.at(key).isAdvanced) {
            result.push_back(key);
        }
    }
    
    return result;
}

size_t AdvancedConfigManager::getKeyCount() const {
    return getAllKeys().size();
}

void AdvancedConfigManager::registerSchema(const ConfigSchema& schema) {
    schema_[schema.key] = schema;
}

bool AdvancedConfigManager::hasSchema(const std::string& key) const {
    return schema_.find(key) != schema_.end();
}

const ConfigSchema* AdvancedConfigManager::getSchema(const std::string& key) const {
    auto it = schema_.find(key);
    if (it != schema_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool AdvancedConfigManager::savePreset(const std::string& name) {
    if (!config_) {
        return false;
    }
    
    json j = serialize();
    presetData_[name] = j;
    
    NLM_LOG_INFO("Saved configuration preset: " + name);
    return true;
}

bool AdvancedConfigManager::loadPreset(const std::string& name) {
    auto it = presetData_.find(name);
    if (it == presetData_.end()) {
        NLM_LOG_ERROR("Configuration preset not found: " + name);
        return false;
    }
    
    return deserializeFromJSON(it->second);
}

std::vector<std::string> AdvancedConfigManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& pair : presetData_) {
        names.push_back(pair.first);
    }
    return names;
}

bool AdvancedConfigManager::deletePreset(const std::string& name) {
    auto it = presetData_.find(name);
    if (it == presetData_.end()) {
        return false;
    }
    
    presetData_.erase(it);
    NLM_LOG_INFO("Deleted configuration preset: " + name);
    return true;
}

bool AdvancedConfigManager::hasChanges() const {
    return hasChanges_;
}

void AdvancedConfigManager::acceptChanges() {
    hasChanges_ = false;
}

std::shared_ptr<AdvancedConfigManager> AdvancedConfigManager::createDiff(
    const AdvancedConfigManager& other) const {
    
    auto diff = std::make_shared<AdvancedConfigManager>();
    
    if (!config_) {
        return diff;
    }
    
    // Compare configurations and create diff
    auto thisKeys = getAllKeys();
    auto otherKeys = other.getAllKeys();
    
    for (const auto& key : thisKeys) {
        if (other.config_->hasKey(key)) {
            // Compare values
            if (config_->getAsString(key) != other.config_->getAsString(key)) {
                diff->set(key, config_->getAsString(key));
            }
        } else {
            // Key only in this config
            diff->set(key, config_->getAsString(key));
        }
    }
    
    for (const auto& key : otherKeys) {
        if (!config_->hasKey(key)) {
            // Key only in other config
            diff->set(key, other.config_->getAsString(key));
        }
    }
    
    return diff;
}

void AdvancedConfigManager::merge(const AdvancedConfigManager& other) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    auto otherKeys = other.getAllKeys();
    for (const auto& key : otherKeys) {
        if (other.config_->hasKey(key)) {
            config_->set(key, other.config_->getAsString(key));
        }
    }
    
    hasChanges_ = true;
}

void AdvancedConfigManager::registerGroup(const ConfigGroup& group) {
    groups_.push_back(group);
}

std::vector<AdvancedConfigManager::ConfigGroup> AdvancedConfigManager::getGroups() const {
    return groups_;
}

std::vector<ConfigSchema> AdvancedConfigManager::getDefaultSchema() {
    std::vector<ConfigSchema> schemas;
    
    // Basic simulation parameters
    schemas.push_back({"neuron_count", "int64", "Number of neurons in the brain", "1000", "^[0-9]+$", {}, true, false});
    schemas.push_back({"region_count", "int64", "Number of brain regions", "1", "^[0-9]+$", {}, true, false});
    schemas.push_back({"connection_probability", "float", "Connection probability (0.0-1.0)", "0.1", "^([0-9]*[.])?[0-9]+$", {}, true, false});
    schemas.push_back({"simulation_timestep", "double", "Simulation timestep in seconds", "0.001", "^([0-9]*[.])?[0-9]+$", {}, true, false});
    schemas.push_back({"max_steps", "int64", "Maximum simulation steps", "10000", "^[0-9]+$", {}, false, false});
    
    // Plasticity parameters
    schemas.push_back({"stdp_ltp_weight", "float", "STDP LTP weight", "0.01", "^([0-9]*[.])?[0-9]+$", {}, false, false});
    schemas.push_back({"stdp_ltd_weight", "float", "STDP LTD weight", "0.012", "^([0-9]*[.])?[0-9]+$", {}, false, false});
    schemas.push_back({"synaptogenesis_rate", "float", "Synaptogenesis rate", "0.0001", "^([0-9]*[.])?[0-9]+$", {}, false, false});
    schemas.push_back({"pruning_rate", "float", "Pruning rate", "0.00001", "^([0-9]*[.])?[0-9]+$", {}, false, false});
    
    // Development parameters
    schemas.push_back({"enable_plasticity", "bool", "Enable plasticity", "true", "^(true|false)$", {"true", "false"}, false, false});
    schemas.push_back({"enable_structural_plasticity", "bool", "Enable structural plasticity", "true", "^(true|false)$", {"true", "false"}, false, false});
    schemas.push_back({"enable_development", "bool", "Enable development", "true", "^(true|false)$", {"true", "false"}, false, false});
    
    // Neuromodulation parameters
    schemas.push_back({"enable_neuromodulation", "bool", "Enable neuromodulation", "true", "^(true|false)$", {"true", "false"}, false, false});
    
    // Memory and cognition
    schemas.push_back({"enable_memory", "bool", "Enable memory systems", "true", "^(true|false)$", {"true", "false"}, false, false});
    schemas.push_back({"enable_prediction", "bool", "Enable prediction system", "true", "^(true|false)$", {"true", "false"}, false, false});
    schemas.push_back({"enable_cognition", "bool", "Enable cognitive functions", "true", "^(true|false)$", {"true", "false"}, false, false});
    
    // Environment parameters
    schemas.push_back({"environment_type", "string", "Environment type", "GridWorld", "^", {"GridWorld", "Continuous", "Cylindrical"}, false, false});
    schemas.push_back({"environment_size", "int64", "Environment size (for grid)", "10", "^[0-9]+$", {}, false, false});
    
    return schemas;
}

std::vector<AdvancedConfigManager::ConfigGroup> AdvancedConfigManager::getDefaultGroups() {
    std::vector<ConfigGroup> groups;
    
    // Basic settings group
    groups.push_back({
        "Basic", "Basic simulation parameters", {
            "neuron_count", "region_count", "connection_probability", "simulation_timestep", "max_steps"
        }, false
    });
    
    // Plasticity group
    groups.push_back({
        "Plasticity", "Synaptic plasticity rules", {
            "stdp_ltp_weight", "stdp_ltd_weight", "synaptogenesis_rate", "pruning_rate"
        }, false
    });
    
    // Development group
    groups.push_back({
        "Development", "Developmental processes", {
            "enable_plasticity", "enable_structural_plasticity", "enable_development"
        }, false
    });
    
    // Neuromodulation group
    groups.push_back({
        "Neuromodulation", "Brain state modulators", {
            "enable_neuromodulation"
        }, false
    });
    
    // Memory and cognition group
    groups.push_back({
        "Memory", "Memory systems and cognition", {
            "enable_memory", "enable_prediction", "enable_cognition"
        }, false
    });
    
    // Environment group
    groups.push_back({
        "Environment", "Environment settings", {
            "environment_type", "environment_size"
        }, false
    });
    
    return groups;
}

std::shared_ptr<AdvancedConfigManager> AdvancedConfigManager::createDefault() {
    auto manager = std::make_shared<AdvancedConfigManager>();
    
    // Register default schema
    auto schemas = getDefaultSchema();
    for (const auto& schema : schemas) {
        manager->registerSchema(schema);
    }
    
    // Register default groups
    auto groups = getDefaultGroups();
    for (const auto& group : groups) {
        manager->registerGroup(group);
    }
    
    // Create default config
    if (!manager->config_) {
        manager->config_ = std::make_shared<Config>();
    }
    
    // Set default values
    manager->config_->set("neuron_count", int64_t(1000), ConfigSource::Default);
    manager->config_->set("region_count", int64_t(1), ConfigSource::Default);
    manager->config_->set("connection_probability", 0.1f, ConfigSource::Default);
    manager->config_->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    manager->config_->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    manager->config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    manager->config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    manager->config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    manager->config_->set("max_steps", int64_t(10000), ConfigSource::Default);
    manager->config_->set("log_level", std::string("INFO"), ConfigSource::Default);
    manager->config_->set("enable_plasticity", true, ConfigSource::Default);
    manager->config_->set("enable_structural_plasticity", true, ConfigSource::Default);
    manager->config_->set("enable_development", true, ConfigSource::Default);
    manager->config_->set("enable_neuromodulation", true, ConfigSource::Default);
    manager->config_->set("enable_memory", true, ConfigSource::Default);
    manager->config_->set("enable_prediction", true, ConfigSource::Default);
    manager->config_->set("enable_cognition", true, ConfigSource::Default);
    manager->config_->set("environment_type", std::string("GridWorld"), ConfigSource::Default);
    manager->config_->set("environment_size", int64_t(10), ConfigSource::Default);
    
    manager->hasChanges_ = false;
    
    return manager;
}

bool AdvancedConfigManager::validateValue(const std::string& key, const std::string& value) const {
    auto it = schema_.find(key);
    if (it == schema_.end()) {
        // No schema, assume valid
        return true;
    }
    
    const auto& schema = it->second;
    
    // Check if value is in allowed values
    if (!schema.allowedValues.empty()) {
        return std::find(schema.allowedValues.begin(), schema.allowedValues.end(), value) != schema.allowedValues.end();
    }
    
    // Basic pattern matching
    if (!schema.validationPattern.empty()) {
        // Simple regex check (can be enhanced)
        return true; // Assume valid for now
    }
    
    return true; // Assume valid if no validation rules
}

bool AdvancedConfigManager::validateRange(const std::string& key, double value) const {
    // Range validation based on key
    if (key == "neuron_count" || key == "region_count" || key == "max_steps") {
        return value >= 1 && value <= 100000;
    } else if (key == "connection_probability" || key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "synaptogenesis_rate" || key == "pruning_rate") {
        return value >= 0.0 && value <= 1.0;
    } else if (key == "simulation_timestep") {
        return value > 0.0 && value <= 1.0;
    }
    
    return true;
}

bool AdvancedConfigManager::validateAllowedValues(const std::string& key, const std::string& value) const {
    auto it = schema_.find(key);
    if (it == schema_.end()) {
        return true;
    }
    
    const auto& schema = it->second;
    return std::find(schema.allowedValues.begin(), schema.allowedValues.end(), value) != schema.allowedValues.end();
}

void AdvancedConfigManager::toJSON(const Config& config, json& j) const {
    // Implementation for serialization
    j = json::object();
}

void AdvancedConfigManager::fromJSON(const json& j, Config& config) const {
    // Implementation for deserialization
}

std::string AdvancedConfigManager::getKeyCategory(const std::string& key) const {
    for (const auto& group : groups_) {
        if (std::find(group.keys.begin(), group.keys.end(), key) != group.keys.end()) {
            return group.name;
        }
    }
    
    // Default to "Unknown"
    return "Unknown";
}

} // namespace nlm