// PlasticityRulePluginInterface.cpp
#include "interfaces/PlasticityRulePluginInterface.hpp"
#include <stdexcept>

namespace nlm {

// Default implementations for PlasticityRulePluginInterface
std::string PlasticityRulePluginInterface::getName() const {
    return "Default Plasticity Rule Plugin";
}

std::string PlasticityRulePluginInterface::getVersion() const {
    return "1.0.0";
}

std::string PlasticityRulePluginInterface::getDescription() const {
    return "Default plasticity rule interface";
}

std::string PlasticityRulePluginInterface::getAuthor() const {
    return "NLM Team";
}

bool PlasticityRulePluginInterface::initialize(const Config& config) {
    setConfiguration(config);
    return validateConfiguration();
}

void PlasticityRulePluginInterface::shutdown() {
    // Default implementation does nothing
}

bool PlasticityRulePluginInterface::isInitialized() const {
    return true;
}

const std::type_info& PlasticityRulePluginInterface::getTypeInfo() const {
    return typeid(PlasticityRulePluginInterface);
}

void PlasticityRulePluginInterface::setConfiguration(const Config& config) {
    // Default implementation - override in concrete implementations
}

Config PlasticityRulePluginInterface::getConfiguration() const {
    Config config;
    // Return default configuration
    return config;
}

bool PlasticityRulePluginInterface::isEnabled() const {
    return true;
}

void PlasticityRulePluginInterface::setEnabled(bool enabled) {
    // Default implementation - can be overridden
}

bool PlasticityRulePluginInterface::validateConfiguration() const {
    return true;
}

std::string PlasticityRulePluginInterface::getValidationError() const {
    return "";
}

std::string PlasticityRulePluginInterface::getId() const {
    return "plasticity_plugin_default_v1.0";
}

std::chrono::steady_clock::time_point PlasticityRulePluginInterface::getLoadTime() const {
    return std::chrono::steady_clock::now();
}

std::unique_ptr<PlasticityRule> PlasticityRulePluginInterface::createRule(const Config& config) {
    return createRule(PlasticityFlags::STDP | PlasticityFlags::Hebbian, config);
}

bool PlasticityRulePluginInterface::configureRule(PlasticityRule* rule, const Config& config) {
    return true;
}

bool PlasticityRulePluginInterface::validateRuleConfiguration(const Config& config) const {
    return true;
}

PlasticityFlags PlasticityRulePluginInterface::getSupportedPlasticityFlags() const {
    return PlasticityFlags::STDP | PlasticityFlags::Hebbian | PlasticityFlags::RewardModulated | PlasticityFlags::Structural;
}

std::string PlasticityRulePluginInterface::getPlasticityTypeName() const {
    return "Default Plasticity";
}

bool PlasticityRulePluginInterface::supportsCustomLearningRate() const {
    return false;
}

float PlasticityRulePluginInterface::getDefaultLearningRate() const {
    return 0.01f;
}

float PlasticityRulePluginInterface::getLearningRateRangeMin() const {
    return 0.0f;
}

float PlasticityRulePluginInterface::getLearningRateRangeMax() const {
    return 1.0f;
}

bool PlasticityRulePluginInterface::supportsCustomSTDP() const {
    return false;
}

float PlasticityRulePluginInterface::getSTDPTimeConstant() const {
    return 20.0f;
}

bool PlasticityRulePluginInterface::supportsCustomHebbian() const {
    return false;
}

std::string PlasticityRulePluginInterface::getDefaultConfigurationKey() const {
    return "plasticity_plugin_config";
}

Config PlasticityRulePluginInterface::getDefaultConfiguration() const {
    Config config;
    config.set("learning_rate", 0.01f);
    config.set("stdp_time_constant", 20.0f);
    config.set("hebbian_strength", 0.1f);
    config.set("plasticity_enabled", true);
    return config;
}

float PlasticityRulePluginInterface::calculateExpectedWeightChange(const PlasticityRule& rule, float preSpikeTime, float postSpikeTime) const {
    float dt = postSpikeTime - preSpikeTime;
    return dt * 0.01f;
}

float PlasticityRulePluginInterface::calculateLearningRate(const Config& config) const {
    return config.getOr<float>("learning_rate", 0.01f);
}

} // namespace nlm