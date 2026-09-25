// ConfigBuilder implementation
// Provides a fluent interface for configuring NLM

#include "ConfigBuilder.hpp"
#include "Config.hpp"
#include "core/Exception.hpp"
#include <stdexcept>
#include <sstream>

namespace nlm {

struct ConfigBuilder::Impl {
    // Internal storage for configuration values
    std::vector<std::tuple<std::string, ConfigValue, ConfigSource>> values;
    
    // Temporary cache for validation
    bool hasValidationErrors = false;
    
    Impl() : hasValidationErrors(false) {}
};

ConfigBuilder::ConfigBuilder() : pImpl(new Impl) {}

ConfigBuilder::~ConfigBuilder() = default;

ConfigBuilder::ConfigBuilder(ConfigBuilder&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ConfigBuilder& ConfigBuilder::operator=(ConfigBuilder&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::string& value) {
    return set(key, value, ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, int value) {
    return set(key, static_cast<int64_t>(value), ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, int64_t value) {
    return set(key, value, ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, double value) {
    return set(key, value, ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, bool value) {
    return set(key, value, ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::vector<int>& value) {
    return set(key, value, ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::vector<double>& value) {
    return set(key, value, ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::vector<std::string>& value) {
    return set(key, value, ConfigSource::Runtime);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::string& value, ConfigSource source) {
    if (key.empty()) {
        pImpl->hasValidationErrors = true;
        throw ConfigException("Cannot set empty key");
    }
    pImpl->values.emplace_back(key, ConfigValue(value), source);
    return *this;
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, int64_t value, ConfigSource source) {
    return set(key, value, source);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, double value, ConfigSource source) {
    return set(key, value, source);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, bool value, ConfigSource source) {
    return set(key, value, source);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    return set(key, value, source);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    return set(key, value, source);
}

ConfigBuilder& ConfigBuilder::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    return set(key, value, source);
}

ConfigBuilder& ConfigBuilder::setDefault(const std::string& key, const std::string& value) {
    return set(key, value, ConfigSource::Default);
}

ConfigBuilder& ConfigBuilder::setDefault(const std::string& key, int value) {
    return set(key, static_cast<int64_t>(value), ConfigSource::Default);
}

ConfigBuilder& ConfigBuilder::setDefault(const std::string& key, double value) {
    return set(key, value, ConfigSource::Default);
}

ConfigBuilder& ConfigBuilder::setDefault(const std::string& key, bool value) {
    return set(key, value, ConfigSource::Default);
}

ConfigBuilder& ConfigBuilder::setStdpParameters(float ltpWeight, float ltdWeight, float tau) {
    // Validate STDP parameters
    if (ltpWeight < 0.0f || ltpWeight > 1.0f) {
        throw ConfigException("LTP weight must be between 0.0 and 1.0");
    }
    if (ltdWeight < 0.0f || ltdWeight > 1.0f) {
        throw ConfigException("LTD weight must be between 0.0 and 1.0");
    }
    if (tau < 1.0f || tau > 100.0f) {
        throw ConfigException("STDP time constant must be between 1.0 and 100.0 ms");
    }
    
    return setDefault("stdp_ltp_weight", ltpWeight)
           .setDefault("stdp_ltd_weight", ltdWeight)
           .setDefault("stdp_tau", tau);
}

ConfigBuilder& ConfigBuilder::setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate) {
    // Validate structural plasticity parameters
    if (synaptogenesisRate < 0.0f || synaptogenesisRate > 1.0f) {
        throw ConfigException("Synaptogenesis rate must be between 0.0 and 1.0");
    }
    if (pruningRate < 0.0f || pruningRate > 1.0f) {
        throw ConfigException("Pruning rate must be between 0.0 and 1.0");
    }
    if (synaptogenesisRate < pruningRate) {
        throw ConfigException("Synaptogenesis rate must be >= pruning rate");
    }
    
    return setDefault("synaptogenesis_rate", synaptogenesisRate)
           .setDefault("pruning_rate", pruningRate);
}

ConfigBuilder& ConfigBuilder::setSimulationParameters(int neuronCount, int regionCount, float connectionProbability, double timestep) {
    // Validate simulation parameters
    if (neuronCount <= 0) {
        throw ConfigException("Neuron count must be positive");
    }
    if (regionCount <= 0) {
        throw ConfigException("Region count must be positive");
    }
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        throw ConfigException("Connection probability must be between 0.0 and 1.0");
    }
    if (timestep <= 0.0) {
        throw ConfigException("Timestep must be positive");
    }
    
    return setDefault("neuron_count", static_cast<int64_t>(neuronCount))
           .setDefault("region_count", static_cast<int64_t>(regionCount))
           .setDefault("connection_probability", connectionProbability)
           .setDefault("simulation_timestep", timestep);
}

std::shared_ptr<Config> ConfigBuilder::build() {
    if (pImpl->hasValidationErrors) {
        throw ConfigException("Configuration has validation errors");
    }
    
    auto config = std::make_shared<Config>();
    
    // Apply all configured values
    for (const auto& [key, value, source] : pImpl->values) {
        config->set(key, value, source);
    }
    
    return config;
}

std::shared_ptr<Config> ConfigBuilder::buildAndLoad(const std::string& filepath) {
    auto config = build();
    
    // Load from file if specified (overrides existing values)
    if (!filepath.empty()) {
        config->loadFromFile(filepath);
    }
    
    return config;
}

void ConfigBuilder::clear() {
    pImpl->values.clear();
    pImpl->hasValidationErrors = false;
}

std::string ConfigBuilder::summary() const {
    std::ostringstream oss;
    oss << "ConfigBuilder Summary:" << std::endl;
    oss << "  Total values configured: " << pImpl->values.size() << std::endl;
    
    for (const auto& [key, value, source] : pImpl->values) {
        oss << "  " << key << " = ";
        
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << "\"" << std::get<std::string>(value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            oss << "[int vector of size " << std::get<std::vector<int>>(value).size() << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            oss << "[double vector of size " << std::get<std::vector<double>>(value).size() << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            oss << "[string vector of size " << std::get<std::vector<std::string>>(value).size() << "]";
        }
        
        oss << " (" << static_cast<int>(source) << ")" << std::endl;
    }
    
    return oss.str();
}

// Convenience function implementations
namespace ConfigPresets {
    std::shared_ptr<Config> createDefaultConfig() {
        ConfigBuilder builder;
        return builder
            .setSimulationParameters(500, 1, 0.15f, 0.001)
            .setStdpParameters(0.02f, 0.015f, 20.0f)
            .setStructuralPlasticityParameters(0.0001f, 0.00001f)
            .setDefault("random_seed", static_cast<int64_t>(42))
            .build();
    }
    
    std::shared_ptr<Config> createMinimalConfig() {
        ConfigBuilder builder;
        return builder
            .setSimulationParameters(100, 1, 0.1f, 0.001)
            .setStdpParameters(0.01f, 0.01f, 10.0f)
            .setStructuralPlasticityParameters(0.00005f, 0.000005f)
            .build();
    }
    
    std::shared_ptr<Config> createHighPlasticityConfig() {
        ConfigBuilder builder;
        return builder
            .setSimulationParameters(1000, 2, 0.2f, 0.0005)
            .setStdpParameters(0.05f, 0.04f, 50.0f)
            .setStructuralPlasticityParameters(0.0005f, 0.00005f)
            .setDefault("random_seed", static_cast<int64_t>(123))
            .build();
    }
    
    std::shared_ptr<Config> createLowPlasticityConfig() {
        ConfigBuilder builder;
        return builder
            .setSimulationParameters(200, 1, 0.05f, 0.002)
            .setStdpParameters(0.005f, 0.003f, 5.0f)
            .setStructuralPlasticityParameters(0.00001f, 0.000001f)
            .build();
    }
    
    std::shared_ptr<Config> createResearchConfig() {
        ConfigBuilder builder;
        return builder
            .setSimulationParameters(2000, 3, 0.3f, 0.001)
            .setStdpParameters(0.03f, 0.025f, 30.0f)
            .setStructuralPlasticityParameters(0.0002f, 0.00002f)
            .setDefault("random_seed", static_cast<int64_t>(456))
            .setDefault("debug_mode", true)
            .build();
    }
}

} // namespace nlm