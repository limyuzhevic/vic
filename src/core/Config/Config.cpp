#include "Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::map<std::string, std::pair<ConfigValue, ConfigSource>> values;
    std::vector<ConfigSchema> schema;
    
    Impl() {
        // Initialize with default schema
        initializeDefaultSchema();
    }
    
    void initializeDefaultSchema() {
        // Core simulation parameters
        schema.push_back({"simulation_timestep", "double", ConfigSource::Default, 
                         3.0, "Simulation timestep in milliseconds"});
        schema.push_back({"neuron_count", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(1000), "Number of neurons"});
        schema.push_back({"region_count", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(1), "Number of brain regions"});
        schema.push_back({"connection_probability", "float", ConfigSource::Default, 
                          0.1f, "Probability of connection between neurons"});
        schema.push_back({"random_seed", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(42), "Random seed for reproducibility"});
        
        // Plasticity parameters
        schema.push_back({"stdp_ltp_weight", "float", ConfigSource::Default, 
                          0.01f, "STDP LTP weight"});
        schema.push_back({"stdp_ltd_weight", "float", ConfigSource::Default, 
                          0.012f, "STDP LTD weight"});
        schema.push_back({"stdp_tau", "float", ConfigSource::Default, 
                          20.0f, "STDP time constant (ms)"});
        schema.push_back({"synaptogenesis_rate", "float", ConfigSource::Default, 
                          0.0001f, "Rate of new synapse formation"});
        schema.push_back({"pruning_rate", "float", ConfigSource::Default, 
                          0.00001f, "Rate of synapse elimination"});
        
        // Memory parameters
        schema.push_back({"working_memory_capacity", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(100), "Working memory capacity"});
        schema.push_back({"episodic_memory_max_episodes", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(1000), "Maximum episodic memory episodes"});
        schema.push_back({"assoc_memory_pattern_count", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(100), "Associative memory pattern count"});
        
        // Neuromodulation parameters
        schema.push_back({"dopamine_initial_level", "float", ConfigSource::Default, 
                          0.5f, "Initial dopamine level"});
        schema.push_back({"curiosity_initial_level", "float", ConfigSource::Default, 
                          0.3f, "Initial curiosity level"});
        schema.push_back({"novelty_initial_level", "float", ConfigSource::Default, 
                          0.2f, "Initial novelty level"});
        
        // Development parameters
        schema.push_back({"development_enabled", "bool", ConfigSource::Default, 
                          true, "Enable developmental processes"});
        schema.push_back({"development_stages", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(4), "Number of developmental stages"});
        
        // Checkpoint parameters
        schema.push_back({"checkpoint_dir", "string", ConfigSource::Default, 
                          "./checkpoints", "Checkpoint directory"});
        schema.push_back({"checkpoint_frequency", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(1000), "Checkpoint save frequency (steps)"});
        schema.push_back({"checkpoint_compression", "bool", ConfigSource::Default, 
                          true, "Enable checkpoint compression"});
        
        // Experiment parameters
        schema.push_back({"experiment_max_steps", "int64", ConfigSource::Default, 
                          static_cast<int64_t>(10000), "Maximum experiment steps"});
        schema.push_back({"experiment_reward_function", "string", ConfigSource::Default, 
                          "default", "Reward function type"});
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

bool Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source) {
    // Check if key exists in schema
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                                 [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->schema.end()) {
        NLM_LOG_ERROR(std::string("Unknown configuration key: ") + key);
        return false;
    }
    
    // Validate the value
    if (schemaIt->validateFn && !((schemaIt->*schemaIt->validateFn)(value))) {
        NLM_LOG_ERROR(std::string("Invalid value for configuration key ") + key);
        return false;
    }
    
    // Check source priority
    auto sourcePriority = [](ConfigSource src) {
        switch (src) {
            case ConfigSource::Default: return 0;
            case ConfigSource::File: return 1;
            case ConfigSource::CommandLine: return 2;
            case ConfigSource::Programmatic: return 3;
        }
        return 0;
    };
    
    auto existingIt = pImpl->values.find(key);
    if (existingIt != pImpl->values.end()) {
        // Only allow higher or equal source priority to override
        if (sourcePriority(source) <= sourcePriority(existingIt->second.second)) {
            NLM_LOG_INFO(std::string("Ignoring lower priority config update for ") + key);
            return false;
        }
    }
    
    pImpl->values[key] = {value, source};
    NLM_LOG_INFO(std::string("Config set ") + key + " = " + std::to_string(value));
    return true;
}

template<typename T>
bool Config::get(const std::string& key, T& value) const {
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        return false;
    }
    
    try {
        value = std::get<T>(it->second.first);
        return true;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR(std::string("Type mismatch for config key ") + key);
        return false;
    }
}

template<typename T>
T Config::getOr(const std::string& key, T defaultValue) const {
    T value;
    if (get(key, value)) {
        return value;
    }
    return defaultValue;
}

bool Config::has(const std::string& key) const {
    return pImpl->values.find(key) != pImpl->values.end();
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string("Cannot open config file: ") + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key = value
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_ERROR(std::string("Invalid config line ") + std::to_string(lineNum) + ": " + line);
            success = false;
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Parse value based on type
        ConfigValue configValue;
        bool parsed = false;
        
        // Try different types
        try {
            size_t decimalPos = value.find('.');
            if (decimalPos != std::string::npos) {
                configValue = std::stod(value);
                parsed = true;
            } else {
                size_t intPos = value.find_first_not_of("0123456789-");
                if (intPos == std::string::npos || intPos == 0) {
                    configValue = std::stoll(value);
                    parsed = true;
                }
            }
        } catch (...) {}
        
        if (!parsed) {
            // Try boolean
            if (value == "true" || value == "1" || value == "yes") {
                configValue = true;
                parsed = true;
            } else if (value == "false" || value == "0" || value == "no") {
                configValue = false;
                parsed = true;
            }
        }
        
        if (!parsed) {
            // Treat as string
            configValue = value;
            parsed = true;
        }
        
        if (parsed) {
            set(key, configValue, ConfigSource::File);
        } else {
            NLM_LOG_ERROR(std::string("Failed to parse config value: ") + value);
            success = false;
        }
    }
    
    file.close();
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                std::string key = arg.substr(0, arg.find('='));
                std::string value = arg.substr(arg.find('=') + 1);
                set(key, ConfigValue(std::stod(value)), ConfigSource::CommandLine);
            } else if (i + 1 < argc) {
                std::string key = arg;
                std::string value = argv[++i];
                set(key, ConfigValue(std::stod(value)), ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 8) == "--neuron") {
            set("neuron_count", ConfigValue(std::stoll(arg.substr(8))), ConfigSource::CommandLine);
        } else if (arg.substr(0, 9) == "--region") {
            set("region_count", ConfigValue(std::stoll(arg.substr(9))), ConfigSource::CommandLine);
        }
    }
    
    return success;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->values) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Config::validate() const {
    ConfigValidator validator;
    return validator.validate(*this);
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        return ConfigSource::Default;
    }
    return it->second.second;
}

void Config::clear() {
    pImpl->values.clear();
}

// Template instantiation
template bool Config::get<int64_t>(const std::string& key, int64_t& value) const;
template bool Config::get<float>(const std::string& key, float& value) const;
template bool Config::get<double>(const std::string& key, double& value) const;
template bool Config::get<std::string>(const std::string& key, std::string& value) const;
template bool Config::get<bool>(const std::string& key, bool& value) const;

template int64_t Config::getOr<int64_t>(const std::string& key, int64_t defaultValue) const;
template float Config::getOr<float>(const std::string& key, float defaultValue) const;
template double Config::getOr<double>(const std::string& key, double defaultValue) const;
template std::string Config::getOr<std::string>(const std::string& key, std::string defaultValue) const;
template bool Config::getOr<bool>(const std::string& key, bool defaultValue) const;

// Global config instance
std::shared_ptr<Config> globalConfig = std::make_shared<Config>();

void initializeConfig() {
    NLM_LOG_INFO("NLM Configuration System Initialized");
}

Config& getConfig() {
    return *globalConfig;
}

// ConfigValidator implementation
struct ConfigValidator::Impl {
    std::vector<std::string> errors;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::addValidation(const std::string& key, bool (ConfigValidator::*validator)(const ConfigValue&)) const {
    // In a real implementation, this would register validation rules
    // For now, it's a placeholder
}

bool ConfigValidator::validate(const Config& config) const {
    // In a real implementation, this would validate against schema
    // For now, return true if no errors
    return pImpl->errors.empty();
}

std::vector<std::string> ConfigValidator::getErrors() const {
    return pImpl->errors;
}

} // namespace nlm