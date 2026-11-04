#include "BrainConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {

BrainConfig::BrainConfig() {}

BrainConfig::~BrainConfig() = default;

bool BrainConfig::initialize(BrainCore* core) {
    this->core = core;
    
    if (!core) {
        NLM_LOG_ERROR("Cannot initialize config system without core brain");
        return false;
    }
    
    // Initialize advanced configuration system
    advancedConfig = std::make_shared<AdvancedConfig>();
    
    // Set up validation rules
    setValidationRules({
        "neuron_count",
        "region_count", 
        "connection_probability",
        "simulation_timestep",
        "synaptogenesis_rate",
        "pruning_rate",
        "random_seed"
    });
    
    NLM_LOG_INFO("Configuration system initialized");
    return true;
}

bool BrainConfig::loadFromFile(const std::string& filepath, const std::string& format) {
    if (!advancedConfig) {
        NLM_LOG_ERROR("Advanced configuration not initialized");
        return false;
    }
    
    return advancedConfig->loadFromFile(filepath, format);
}

bool BrainConfig::saveToFile(const std::string& filepath, const std::string& format) const {
    if (!advancedConfig) {
        NLM_LOG_ERROR("Advanced configuration not initialized");
        return false;
    }
    
    return advancedConfig->saveToFile(filepath, format);
}

bool BrainConfig::validate(std::vector<std::string>& errors) const {
    if (!advancedConfig) {
        errors.push_back("Advanced configuration not initialized");
        return false;
    }
    
    return advancedConfig->validate(errors);
}

size_t BrainConfig::getNeuronCount() const {
    if (!core) return 0;
    auto val = core->getConfig()->get<int64_t>("neuron_count");
    return val ? *val : 1000;
}

size_t BrainConfig::getRegionCount() const {
    if (!core) return 0;
    auto val = core->getConfig()->get<int64_t>("region_count");
    return val ? *val : 1;
}

float BrainConfig::getConnectionProbability() const {
    if (!core) return 0.1f;
    auto val = core->getConfig()->get<float>("connection_probability");
    return val ? *val : 0.1f;
}

double BrainConfig::getSimulationTimestep() const {
    if (!core) return 0.001;
    auto val = core->getConfig()->get<double>("simulation_timestep");
    return val ? *val : 0.001;
}

void BrainConfig::setValidationRules(const std::vector<std::string>& rules) {
    validationRules = rules;
}

bool BrainConfig::validateConfiguration(const std::string& key, const ConfigValue& value) const {
    return validateConfiguration(key, value);
}

bool BrainConfig::loadDefaultConfig() {
    return createDefaultConfig();
}

bool BrainConfig::loadQuickTestConfig() {
    return createQuickTestConfig();
}

bool BrainConfig::loadResearchConfig() {
    return createResearchConfig();
}

std::string BrainConfig::exportConfig() const {
    return configToString();
}

bool BrainConfig::importConfig(const std::string& configString) {
    return stringToConfig(configString);
}

size_t BrainConfig::getConfigurationEntryCount() const {
    if (!advancedConfig || !advancedConfig->getConfig()) return 0;
    return advancedConfig->getConfig()->getKeys().size();
}

std::vector<std::string> BrainConfig::getConfigurationKeys() const {
    if (!advancedConfig || !advancedConfig->getConfig()) return {};
    return advancedConfig->getConfig()->getKeys();
}

bool BrainConfig::validateNeuronCount(const ConfigValue& value) const {
    if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        return val > 0 && val <= 1000000;  // Reasonable upper limit
    }
    return false;
}

bool BrainConfig::validateRegionCount(const ConfigValue& value) const {
    if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        return val > 0 && val <= 1000;  // Reasonable upper limit
    }
    return false;
}

bool BrainConfig::validateConnectionProbability(const ConfigValue& value) const {
    if (std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        return val >= 0.0f && val <= 1.0f;
    }
    return false;
}

bool BrainConfig::validateSimulationTimestep(const ConfigValue& value) const {
    if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return val > 0.0 && val <= 1.0;  // Reasonable upper limit
    }
    return false;
}

bool BrainConfig::validateSinaptogenesisRate(const ConfigValue& value) const {
    if (std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        return val >= 0.0f && val <= 0.1f;  // Reasonable upper limit
    }
    return false;
}

bool BrainConfig::validatePruningRate(const ConfigValue& value) const {
    if (std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        return val >= 0.0f && val <= 0.01f;  // Reasonable upper limit
    }
    return false;
}

bool BrainConfig::createDefaultConfig() {
    if (!advancedConfig) return false;
    
    advancedConfig->setConfig(std::make_shared<Config>());
    
    // Set default values
    advancedConfig->setString("neuron_count", "1000");
    advancedConfig->setString("region_count", "1");
    advancedConfig->setString("connection_probability", "0.1");
    advancedConfig->setString("simulation_timestep", "0.001");
    advancedConfig->setString("stdp_ltp_weight", "0.01");
    advancedConfig->setString("stdp_ltd_weight", "0.012");
    advancedConfig->setString("stdp_tau", "20.0");
    advancedConfig->setString("synaptogenesis_rate", "0.0001");
    advancedConfig->setString("pruning_rate", "0.00001");
    advancedConfig->setString("random_seed", "42");
    
    NLM_LOG_INFO("Default configuration loaded");
    return true;
}

bool BrainConfig::createQuickTestConfig() {
    if (!advancedConfig) return false;
    
    advancedConfig->setConfig(std::make_shared<Config>());
    
    // Set quick test values
    advancedConfig->setString("neuron_count", "500");
    advancedConfig->setString("region_count", "1");
    advancedConfig->setString("connection_probability", "0.15");
    advancedConfig->setString("simulation_timestep", "0.001");
    advancedConfig->setString("stdp_ltp_weight", "0.02");
    advancedConfig->setString("stdp_ltd_weight", "0.015");
    advancedConfig->setString("stdp_tau", "20.0");
    advancedConfig->setString("synaptogenesis_rate", "0.0002");
    advancedConfig->setString("pruning_rate", "0.00002");
    advancedConfig->setString("random_seed", "42");
    
    NLM_LOG_INFO("Quick test configuration loaded");
    return true;
}

bool BrainConfig::createResearchConfig() {
    if (!advancedConfig) return false;
    
    advancedConfig->setConfig(std::make_shared<Config>());
    
    // Set research configuration values
    advancedConfig->setString("neuron_count", "10000");
    advancedConfig->setString("region_count", "4");
    advancedConfig->setString("connection_probability", "0.05");
    advancedConfig->setString("simulation_timestep", "0.0005");
    advancedConfig->setString("stdp_ltp_weight", "0.005");
    advancedConfig->setString("stdp_ltd_weight", "0.006");
    advancedConfig->setString("stdp_tau", "30.0");
    advancedConfig->setString("synaptogenesis_rate", "0.00005");
    advancedConfig->setString("pruning_rate", "0.000005");
    advancedConfig->setString("random_seed", "123");
    
    NLM_LOG_INFO("Research configuration loaded");
    return true;
}

std::string BrainConfig::configToString() const {
    if (!advancedConfig) return "";
    
    std::stringstream ss;
    ss << "NLM Configuration:" << std::endl;
    
    auto keys = advancedConfig->getConfig()->getKeys();
    for (const auto& key : keys) {
        auto val = advancedConfig->getConfig()->get(key);
        if (val.has_value()) {
            ss << "  " << key << " = ";
            
            std::visit([&ss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    ss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    ss << (arg ? "true" : "false");
                } else {
                    ss << arg;
                }
            }, val.value());
            ss << std::endl;
        }
    }
    
    return ss.str();
}

bool BrainConfig::stringToConfig(const std::string& configString) {
    // Placeholder implementation
    // In practice, this would parse a configuration string and apply it
    NLM_LOG_INFO("Configuration import not yet implemented");
    return true;
}

} // namespace nlm
