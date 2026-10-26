#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Configuration validation error messages
const std::string ERROR_INVALID_KEY = "Invalid configuration key: ";
const std::string ERROR_INVALID_VALUE = "Invalid value for configuration key: ";
const std::string ERROR_MISSING_REQUIRED = "Missing required configuration: ";

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // For validation tracking
    mutable std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()), validationErrors() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.synapse_density",
        "brain.connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            pImpl->validationErrors.push_back("Missing required configuration: " + key);
        }
    }
    
    // Validate numeric ranges
    auto neuronCount = get<int64_t>("brain.neuron_count");
    if (neuronCount.has_value()) {
        if (neuronCount.value() <= 0 || neuronCount.value() > 1000000) {
            pImpl->validationErrors.push_back("brain.neuron_count must be between 1 and 1,000,000");
        }
    }
    
    auto connectionProb = get<double>("brain.connection_probability");
    if (connectionProb.has_value()) {
        if (connectionProb.value() < 0.0 || connectionProb.value() > 1.0) {
            pImpl->validationErrors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::string Config::getValidationErrors() const {
    if (validate()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation failed:";
    for (const auto& error : pImpl->validationErrors) {
        oss << "\n  " << error;
    }
    return oss.str();
}

Config Config::createDefaultConfig() {
    Config config;
    
    // Standard brain configuration
    config.set("brain.neuron_count", 1000, ConfigSource::Default);
    config.set("brain.synapse_density", 0.1, ConfigSource::Default);
    config.set("brain.connection_probability", 0.05, ConfigSource::Default);
    config.set("brain.initial_weight_mean", 0.5, ConfigSource::Default);
    config.set("brain.initial_weight_std", 0.1, ConfigSource::Default);
    config.set("brain.v_thresh", -50.0, ConfigSource::Default);
    config.set("brain.v_rest", -70.0, ConfigSource::Default);
    config.set("brain.v_reset", -75.0, ConfigSource::Default);
    config.set("brain.tau_mem", 20.0, ConfigSource::Default);
    config.set("brain.tau_ref", 2.0, ConfigSource::Default);
    
    // Plasticity settings
    config.set("plasticity.stdp.enable", true, ConfigSource::Default);
    config.set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default);
    config.set("plasticity.stdp.tau_plus", 20.0, ConfigSource::Default);
    config.set("plasticity.stdp.tau_minus", 20.0, ConfigSource::Default);
    config.set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config.set("plasticity.hebbian.learning_rate", 0.001, ConfigSource::Default);
    config.set("plasticity.structural.enable", true, ConfigSource::Default);
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("plasticity.structural.pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation settings
    config.set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
    config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config.set("neuromod.curiosity.exploration_rate", 0.3, ConfigSource::Default);
    config.set("neuromod.novelty.enable", true, ConfigSource::Default);
    config.set("neuromod.novelty.sensitivity", 1.0, ConfigSource::Default);
    
    // Development settings
    config.set("development.enable", true, ConfigSource::Default);
    config.set("development.initial_plasticity", 1.0, ConfigSource::Default);
    config->set("development.plasticity_decay_rate", 0.95, ConfigSource::Default);
    config.set("development.critical_period_end", 300.0, ConfigSource::Default);
    config.set("development.adult_plasticity", 0.2, ConfigSource::Default);
    
    // Memory system settings
    config.set("memory.working.enable", true, ConfigSource::Default);
    config.set("memory.working.trace_decay_rate", 0.99, ConfigSource::Default);
    config.set("memory.episodic.enable", true, ConfigSource::Default);
    config.set("memory.episodic.max_episodes", 100, ConfigSource::Default);
    config.set("memory.associative.enable", true, ConfigSource::Default);
    config.set("memory.associative.association_strength", 0.5, ConfigSource::Default);
    
    // Simulation settings
    config.set("simulation.timestep", 0.001, ConfigSource::Default);
    config.set("simulation.max_steps", 1000, ConfigSource::Default);
    config.set("simulation.random_seed", 42, ConfigSource::Default);
    
    return config;
}

Config Config::createDevelopmentConfig() {
    Config config = createDefaultConfig();
    
    // Enhanced configuration for development/testing
    config.set("brain.neuron_count", 500, ConfigSource::Default);  // Smaller for faster testing
    config.set("plasticity.structural.synaptogenesis_rate", 0.001, ConfigSource::Default);
    config.set("plasticity.structural.pruning_rate", 0.0001, ConfigSource::Default);
    config.set("development.enable", true, ConfigSource::Default);
    config.set("development.initial_plasticity", 1.5, ConfigSource::Default);  // Higher plasticity for development
    
    return config;
}

Config Config::createPerformanceConfig() {
    Config config = createDefaultConfig();
    
    // Optimized for performance
    config.set("brain.neuron_count", 5000, ConfigSource::Default);  // More neurons for better statistics
    config.set("plasticity.structural.enable", false, ConfigSource::Default);  // Disable expensive structural plasticity
    config.set("memory.episodic.max_episodes", 50, ConfigSource::Default);  // Reduced memory storage
    config.set("simulation.max_steps", 5000, ConfigSource::Default);  // Longer simulations
    
    return config;
}

bool Config::validateKey(const std::string& key) const {
    // Check for valid key format (alphanumeric with dots, underscores)
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Validate based on key type
    if (key.find("bool") != std::string::npos) {
        return std::holds_alternative<bool>(value);
    } else if (key.find("int") != std::string::npos || key.find("count") != std::string::npos) {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (key.find("float") != std::string::npos || key.find("rate") != std::string::npos || key.find("probability") != std::string::npos) {
        return std::holds_alternative<double>(value) || std::holds_alternative<float>(value);
    } else if (key.find("string") != std::string::npos) {
        return std::holds_alternative<std::string>(value);
    }
    
    // For other keys, accept any type
    return true;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Additional error message definitions at global scope
const std::string ERROR_INVALID_KEY = "Invalid configuration key: ";
const std::string ERROR_INVALID_VALUE = "Invalid value for configuration key: ";
const std::string ERROR_MISSING_REQUIRED = "Missing required configuration: ";
