// ExperimentConfig implementation - common configuration handling
#include "ExperimentInfrastructure.hpp"
#include <fstream>

namespace nlm {

struct ExperimentConfig::Impl {
    std::shared_ptr<Config> config;
    std::string configFile;
};

ExperimentConfig::ExperimentConfig() {
    pImpl = std::make_unique<Impl>();
    pImpl->config = std::make_shared<Config>();
    
    // Set default configuration values
    pImpl->config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    pImpl->config->set("simulation_timestep", 0.001, ConfigSource::Default);
    pImpl->config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    pImpl->config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    pImpl->config->set("connection_probability", 0.15f, ConfigSource::Default);
}

ExperimentConfig::~ExperimentConfig() = default;

bool ExperimentConfig::loadConfig(const std::string& configFile) {
    pImpl->configFile = configFile;
    return pImpl->config->loadFromFile(configFile);
}

template<typename T>
T ExperimentConfig::getValue(const std::string& key, const T& defaultValue) const {
    return pImpl->config->getOr<T>(key, defaultValue);
}

void ExperimentConfig::setValue(const std::string& key, const std::string& value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
}

bool ExperimentConfig::validate() const {
    bool valid = true;
    
    // Check required parameters
    if (!pImpl->config->has("random_seed")) {
        valid = false;
    }
    
    if (!pImpl->config->has("simulation_timestep")) {
        valid = false;
    } else if (pImpl->config->getOr<double>("simulation_timestep", 0.0) <= 0.0) {
        valid = false;
    }
    
    if (!pImpl->config->has("neuron_count")) {
        valid = false;
    } else if (pImpl->config->getOr<int64_t>("neuron_count", 0) <= 0) {
        valid = false;
    }
    
    return valid;
}

std::vector<std::string> ExperimentConfig::getKeys() const {
    return pImpl->config->getKeys();
}

} // namespace nlm
