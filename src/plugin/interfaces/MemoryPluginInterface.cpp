// MemoryPluginInterface.cpp
#include "interfaces/MemoryPluginInterface.hpp"
#include <stdexcept>

namespace nlm {

// Default implementations for MemoryPluginInterface

std::string MemoryPluginInterface::getName() const {
    return "Default Memory Plugin";
}

std::string MemoryPluginInterface::getVersion() const {
    return "1.0.0";
}

std::string MemoryPluginInterface::getDescription() const {
    return "Default memory system interface";
}

std::string MemoryPluginInterface::getAuthor() const {
    return "NLM Team";
}

bool MemoryPluginInterface::initialize(const Config& config) {
    setConfiguration(config);
    return validateConfiguration();
}

void MemoryPluginInterface::shutdown() {
    // Default implementation does nothing
}

bool MemoryPluginInterface::isInitialized() const {
    return true;
}

const std::type_info& MemoryPluginInterface::getTypeInfo() const {
    return typeid(MemoryPluginInterface);
}

void MemoryPluginInterface::setConfiguration(const Config& config) {
    // Default implementation - override in concrete implementations
}

Config MemoryPluginInterface::getConfiguration() const {
    Config config;
    // Return default configuration
    return config;
}

bool MemoryPluginInterface::isEnabled() const {
    return true;
}

void MemoryPluginInterface::setEnabled(bool enabled) {
    // Default implementation - can be overridden
}

bool MemoryPluginInterface::validateConfiguration() const {
    return true;
}

std::string MemoryPluginInterface::getValidationError() const {
    return "";
}

std::string MemoryPluginInterface::getId() const {
    return "memory_plugin_default_v1.0";
}

std::chrono::steady_clock::time_point MemoryPluginInterface::getLoadTime() const {
    return std::chrono::steady_clock::now();
}

std::unique_ptr<NeuralWorkingMemory> MemoryPluginInterface::createMemorySystem(const Config& config) {
    return createMemorySystem(MemoryType::Working, config);
}

std::unique_ptr<NeuralWorkingMemory> MemoryPluginInterface::createMemorySystem(MemoryType type, const Config& config) {
    // Default implementation - would create appropriate memory system
    return std::make_unique<NeuralWorkingMemory>(config);
}

bool MemoryPluginInterface::configureMemorySystem(NeuralWorkingMemory* system, const Config& config) {
    return true;
}

bool MemoryPluginInterface::validateMemorySystemConfiguration(const Config& config) const {
    return true;
}

// Note: MemoryType enum values need to be defined in the codebase
// For now, this provides placeholder implementations

size_t MemoryPluginInterface::getDefaultCapacity() const {
    return 100;
}

float MemoryPluginInterface::getDefaultDecayRate() const {
    return 0.01f;
}

float MemoryPluginInterface::getDefaultConsolidationThreshold() const {
    return 0.7f;
}

size_t MemoryPluginInterface::getMaxCapacity() const {
    return 1000;
}

size_t MemoryPluginInterface::getMinCapacity() const {
    return 10;
}

std::vector<std::string> MemoryPluginInterface::getSupportedMemoryTypes() const {
    return {"Working", "Episodic", "Semantic", "Procedural", "Associative"};
}

bool MemoryPluginInterface::supportsCustomEncoding() const {
    return false;
}

bool MemoryPluginInterface::supportsCustomRetrieval() const {
    return false;
}

std::string MemoryPluginInterface::getDefaultConfigurationKey() const {
    return "memory_plugin_config";
}

Config MemoryPluginInterface::getDefaultConfiguration() const {
    Config config;
    config.set("capacity", 100);
    config.set("decay_rate", 0.01f);
    config.set("consolidation_threshold", 0.7f);
    return config;
}

float MemoryPluginInterface::calculateEncodingStrength(const Config& config) const {
    return config.getOr<float>("encoding_strength", 0.5f);
}

float MemoryPluginInterface::calculateRetrievalProbability(float age, const Config& config) const {
    return std::exp(-age * config.getOr<float>("decay_rate", 0.01f));
}

bool MemoryPluginInterface::shouldConsolidate(float age, const Config& config) const {
    return age > config.getOr<float>("consolidation_threshold", 0.7f);
}

} // namespace nlm