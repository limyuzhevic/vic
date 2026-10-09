#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <typeinfo>
#include <cmath>

namespace nlm {

class Config::Impl {
public:
    std::map<std::string, std::any> values;
    std::string version;
    std::string description;
    
    Impl() : version("2.0"), description("NLM Configuration System") {}
};

Config::Config() : pImpl(new Impl) {}

Config::~Config() = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return loadFromString(buffer.str());
}

bool Config::loadFromString(const std::string& configString) {
    clear();
    
    std::istringstream stream(configString);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t delimiter = line.find('=');
        if (delimiter == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, delimiter);
        std::string value = line.substr(delimiter + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Try to parse different types
        try {
            // Try integer
            size_t pos;
            long intVal = std::stol(value, &pos);
            if (pos == value.length()) {
                set<long>(key, intVal);
                continue;
            }
        } catch (...) {}
        
        try {
            // Try float
            size_t pos;
            float floatVal = std::stof(value, &pos);
            if (pos == value.length()) {
                set<float>(key, floatVal);
                continue;
            }
        } catch (...) {}
        
        // Treat as string
        set<std::string>(key, value);
    }
    
    return true;
}

template<typename T>
T Config::get(const std::string& key) const {
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        throw ConfigError("Key not found: " + key);
    }
    
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        throw ConfigError("Type mismatch for key: " + key);
    }
}

template<typename T>
T Config::getOr(const std::string& key, T defaultValue) const {
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        return defaultValue;
    }
    
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        return defaultValue;
    }
}

template<typename T>
void Config::set(const std::string& key, T value) {
    pImpl->values[key] = value;
}

bool Config::has(const std::string& key) const {
    return pImpl->values.find(key) != pImpl->values.end();
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->values.size());
    
    for (const auto& pair : pImpl->values) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

std::string Config::toString() const {
    std::stringstream stream;
    
    for (const auto& pair : pImpl->values) {
        stream << pair.first << " = ";
        
        if (pair.second.type() == typeid(int)) {
            stream << std::any_cast<int>(pair.second);
        } else if (pair.second.type() == typeid(float)) {
            stream << std::any_cast<float>(pair.second);
        } else if (pair.second.type() == typeid(std::string)) {
            stream << "\"" << std::any_cast<std::string>(pair.second) << "\"";
        } else {
            stream << "unknown";
        }
        
        stream << std::endl;
    }
    
    return stream.str();
}

void Config::merge(const Config& other) {
    for (const auto& pair : other.pImpl->values) {
        pImpl->values[pair.first] = pair.second;
    }
}

void Config::clear() {
    pImpl->values.clear();
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toString();
    return true;
}

bool Config::validate() const {
    return ConfigValidator::validate(*this);
}

std::string Config::getDescription() const {
    return pImpl->description;
}

namespace nlm {

bool ConfigValidator::validate(const Config& config) {
    std::vector<std::string> errors = getValidationErrors(config);
    return errors.empty();
}

std::vector<std::string> ConfigValidator::getValidationErrors(const Config& config) {
    std::vector<std::string> errors;
    
    if (config.has("simulation_timestep") && config.get<float>("simulation_timestep") <= 0.0f) {
        errors.push_back("simulation_timestep must be positive");
    }
    
    if (config.has("neuron_count") && config.get<size_t>("neuron_count") == 0) {
        errors.push_back("neuron_count must be positive");
    }
    
    if (config.has("connection_probability") && 
        (config.get<float>("connection_probability") < 0.0f || 
         config.get<float>("connection_probability") > 1.0f)) {
        errors.push_back("connection_probability must be between 0.0 and 1.0");
    }
    
    if (config.has("stdp_ltp_weight") && config.get<float>("stdp_ltp_weight") < 0.0f) {
        errors.push_back("stdp_ltp_weight must be non-negative");
    }
    
    if (config.has("stdp_ltd_weight") && config.get<float>("stdp_ltd_weight") < 0.0f) {
        errors.push_back("stdp_ltd_weight must be non-negative");
    }
    
    return errors;
}

Config ConfigBuilder::createDefaultConfig() {
    Config config;
    
    config.set<size_t>("neuron_count", 1000);
    config.set<size_t>("region_count", 1);
    config.set<float>("connection_probability", 0.1f);
    
    config.set<double>("simulation_timestep", 0.001);
    config.set<size_t>("max_steps", 10000);
    
    config.set<float>("dopamine_baseline", 0.1f);
    config.set<float>("dopamine_peak", 1.0f);
    config.set<float>("dopamine_decay_rate", 0.1f);
    config.set<float>("dopamine_release_rate", 1.0f);
    
    config.set<size_t>("working_memory_capacity", 100);
    config.set<size_t>("episodic_memory_capacity", 1000);
    config.set<float>("memory_decay_rate", 0.95f);
    
    config.set<float>("exploration_bonus", 0.1f);
    config.set<float>("intrinsic_weight", 0.5f);
    config.set<float>("extrinsic_weight", 0.5f);
    
    config.set<float>("stdp_ltp_weight", 0.01f);
    config.set<float>("stdp_ltd_weight", 0.012f);
    config.set<float>("stdp_tau", 20.0f);
    config.set<float>("hebbian_learning_rate", 0.1f);
    config.set<float>("bcm_learning_rate", 0.05f);
    
    config.set<float>("synaptogenesis_rate", 0.0001f);
    config.set<float>("pruning_rate", 0.00001f);
    
    config.set<uint64_t>("random_seed", 42);
    
    config.set<size_t>("replay_interval", 100);
    config.set<size_t>("consolidation_interval", 1000);
    
    return config;
}

Config ConfigBuilder::createMinimalConfig() {
    Config config;
    
    config.set<size_t>("neuron_count", 100);
    config.set<float>("connection_probability", 0.05f);
    config.set<double>("simulation_timestep", 0.001);
    
    return config;
}

Config ConfigBuilder::createResearchConfig() {
    Config config = createDefaultConfig();
    
    config.set<size_t>("neuron_count", 5000);
    config.set<size_t>("region_count", 3);
    config.set<float>("connection_probability", 0.2f);
    config.set<float>("exploration_bonus", 0.2f);
    config.set<float>("intrinsic_weight", 0.6f);
    config.set<float>("stdp_tau", 50.0f);
    config.set<float>("synaptogenesis_rate", 0.001f);
    
    return config;
}

Config ConfigBuilder::createPerformanceConfig() {
    Config config = createDefaultConfig();
    
    config.set<size_t>("neuron_count", 500);
    config.set<size_t>("region_count", 2);
    config.set<float>("connection_probability", 0.08f);
    config.set<float>("stdp_tau", 10.0f);
    config.set<float>("synaptogenesis_rate", 0.00005f);
    config.set<float>("pruning_rate", 0.000005f);
    
    return config;
}

Config ConfigBuilder::loadFromFile(const std::string& filepath) {
    Config config;
    config.loadFromFile(filepath);
    return config;
}

Config ConfigBuilder::parseString(const std::string& configString) {
    Config config;
    config.loadFromString(configString);
    return config;
}

void ConfigBuilder::applyConfigToComponent(Config& config) {
    // Apply configuration to brain components
}

} // namespace nlm
