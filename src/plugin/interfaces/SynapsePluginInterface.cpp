// SynapsePluginInterface.cpp
#include "interfaces/SynapsePluginInterface.hpp"
#include <stdexcept>

namespace nlm {

// Default implementations for SynapsePluginInterface
std::string SynapsePluginInterface::getName() const {
    return "Default Synapse Plugin";
}

std::string SynapsePluginInterface::getVersion() const {
    return "1.0.0";
}

std::string SynapsePluginInterface::getDescription() const {
    return "Default synapse model interface";
}

std::string SynapsePluginInterface::getAuthor() const {
    return "NLM Team";
}

bool SynapsePluginInterface::initialize(const Config& config) {
    setConfiguration(config);
    return validateConfiguration();
}

void SynapsePluginInterface::shutdown() {
    // Default implementation does nothing
}

bool SynapsePluginInterface::isInitialized() const {
    return true;
}

const std::type_info& SynapsePluginInterface::getTypeInfo() const {
    return typeid(SynapsePluginInterface);
}

void SynapsePluginInterface::setConfiguration(const Config& config) {
    // Default implementation - override in concrete implementations
}

Config SynapsePluginInterface::getConfiguration() const {
    Config config;
    // Return default configuration
    return config;
}

bool SynapsePluginInterface::isEnabled() const {
    return true;
}

void SynapsePluginInterface::setEnabled(bool enabled) {
    // Default implementation - can be overridden
}

bool SynapsePluginInterface::validateConfiguration() const {
    return true;
}

std::string SynapsePluginInterface::getValidationError() const {
    return "";
}

std::string SynapsePluginInterface::getId() const {
    return "synapse_plugin_default_v1.0";
}

std::chrono::steady_clock::time_point SynapsePluginInterface::getLoadTime() const {
    return std::chrono::steady_clock::now();
}

std::unique_ptr<Synapse> SynapsePluginInterface::createSynapse(const Config& config) {
    return createSynapse(0, 0, config);
}

bool SynapsePluginInterface::configureSynapse(Synapse* synapse, const Config& config) {
    return true;
}

bool SynapsePluginInterface::validateSynapseConfiguration(const Config& config) const {
    return true;
}

SynapticWeight SynapsePluginInterface::getDefaultWeight() const {
    return 0.5f;
}

Delay SynapsePluginInterface::getDefaultDelay() const {
    return 1;
}

float SynapsePluginInterface::getWeightRangeMin() const {
    return -1.0f;
}

float SynapsePluginInterface::getWeightRangeMax() const {
    return 1.0f;
}

Delay SynapsePluginInterface::getDelayRangeMin() const {
    return 0;
}

Delay SynapsePluginInterface::getDelayRangeMax() const {
    return 10;
}

bool SynapsePluginInterface::supportsCustomDynamics() const {
    return false;
}

std::string SynapsePluginInterface::getDynamicModelName() const {
    return "Standard Synapse";
}

float SynapsePluginInterface::getTimeConstant() const {
    return 1.0f;
}

bool SynapsePluginInterface::supportsCustomPlasticity() const {
    return false;
}

std::string SynapsePluginInterface::getPlasticityModelName() const {
    return "Standard Plasticity";
}

std::string SynapsePluginInterface::getDefaultConfigurationKey() const {
    return "synapse_plugin_config";
}

Config SynapsePluginInterface::getDefaultConfiguration() const {
    Config config;
    config.set("weight", 0.5f);
    config.set("delay", 1);
    config.set("plasticity_enabled", true);
    return config;
}

} // namespace nlm