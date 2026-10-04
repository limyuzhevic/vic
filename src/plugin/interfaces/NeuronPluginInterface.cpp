// NeuronPluginInterface.cpp
#include "interfaces/NeuronPluginInterface.hpp"
#include <stdexcept>

namespace nlm {

// Default implementations for NeuronPluginInterface
std::string NeuronPluginInterface::getName() const {
    return "Default Neuron Plugin";
}

std::string NeuronPluginInterface::getVersion() const {
    return "1.0.0";
}

std::string NeuronPluginInterface::getDescription() const {
    return "Default neuron model interface";
}

std::string NeuronPluginInterface::getAuthor() const {
    return "NLM Team";
}

bool NeuronPluginInterface::initialize(const Config& config) {
    setConfiguration(config);
    return validateConfiguration();
}

void NeuronPluginInterface::shutdown() {
    // Default implementation does nothing
}

bool NeuronPluginInterface::isInitialized() const {
    return true;
}

const std::type_info& NeuronPluginInterface::getTypeInfo() const {
    return typeid(NeuronPluginInterface);
}

void NeuronPluginInterface::setConfiguration(const Config& config) {
    // Default implementation - override in concrete implementations
}

Config NeuronPluginInterface::getConfiguration() const {
    Config config;
    // Return default configuration
    return config;
}

bool NeuronPluginInterface::isEnabled() const {
    return true;
}

void NeuronPluginInterface::setEnabled(bool enabled) {
    // Default implementation - can be overridden
}

bool NeuronPluginInterface::validateConfiguration() const {
    return true;
}

std::string NeuronPluginInterface::getValidationError() const {
    return "";
}

std::string NeuronPluginInterface::getId() const {
    return "neuron_plugin_default_v1.0";
}

std::chrono::steady_clock::time_point NeuronPluginInterface::getLoadTime() const {
    return std::chrono::steady_clock::now();
}

std::unique_ptr<Neuron> NeuronPluginInterface::createNeuron(const Config& config) {
    return createNeuron(NeuronType::Internal, config);
}

bool NeuronPluginInterface::configureNeuron(Neuron* neuron, const Config& config) {
    return true;
}

bool NeuronPluginInterface::validateNeuronConfiguration(const Config& config) const {
    return true;
}

NeuronType NeuronPluginInterface::getDefaultNeuronType() const {
    return NeuronType::Internal;
}

std::string NeuronPluginInterface::getNeuronTypeName(NeuronType type) const {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

bool NeuronPluginInterface::isNeuronTypeSupported(NeuronType type) const {
    return true;
}

size_t NeuronPluginInterface::getRecommendedPopulationSize() const {
    return 100;
}

size_t NeuronPluginInterface::getMinPopulationSize() const {
    return 10;
}

size_t NeuronPluginInterface::getMaxPopulationSize() const {
    return 1000;
}

bool NeuronPluginInterface::supportsCustomDynamics() const {
    return false;
}

std::string NeuronPluginInterface::getDynamicModelName() const {
    return "Standard LIF";
}

std::string NeuronPluginInterface::getDefaultConfigurationKey() const {
    return "neuron_plugin_config";
}

Config NeuronPluginInterface::getDefaultConfiguration() const {
    Config config;
    config.set("neuron_type", "Internal");
    config.set("plasticity_enabled", true);
    return config;
}

} // namespace nlm