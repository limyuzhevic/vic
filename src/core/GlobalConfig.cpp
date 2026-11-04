// Implementation of GlobalConfig class
#include "GlobalConfig.hpp"
#include "core/Config/Config.hpp"
#include "core/ConfigDefaults.hpp"
#include <string>
#include <vector>

namespace nlm {

struct GlobalConfig::Impl {
    std::shared_ptr<Config> config;
    bool initialized;
    std::vector<std::string> warnings;
    
    Impl() : initialized(false) {
        config = createDefaultConfig();
    }
};

GlobalConfig::GlobalConfig() : pImpl(std::make_unique<Impl>()) {}

GlobalConfig::~GlobalConfig() = default;

GlobalConfig& GlobalConfig::getInstance() {
    static GlobalConfig instance;
    return instance;
}

void GlobalConfig::initializeAllSystems() {
    // Validate configuration before initialization
    if (!validate()) {
        throw std::runtime_error("Configuration validation failed");
    }
    
    // Initialize all subsystems
    // This would be called by the main brain initialization
    // For now, just mark as initialized
    pImpl->initialized = true;
    
    // Log initialization
    if (pImpl->warnings.size() > 0) {
        std::cerr << "GlobalConfig initialization warnings:" << std::endl;
        for (const auto& warning : pImpl->warnings) {
            std::cerr << "  - " << warning << std::endl;
        }
    }
}

void GlobalConfig::updateConfig(const std::string& key, const std::string& value) {
    pImpl->config->set(key, value);
    
    // Re-validate after update
    if (!validate()) {
        pImpl->warnings.push_back("Configuration validation failed after update: " + key);
    }
}

template<typename T>
T GlobalConfig::get(const std::string& key) const {
    return pImpl->config->getOr<T>(key, T());
}

bool GlobalConfig::validate() const {
    std::vector<std::string> errors;
    return validateConfig(*pImpl->config, errors);
}

std::string GlobalConfig::status() const {
    std::ostringstream oss;
    oss << "GlobalConfig status:" << std::endl;
    oss << "  Initialized: " << (pImpl->initialized ? "yes" : "no") << std::endl;
    oss << "  Config entries: " << pImpl->config->getKeys().size() << std::endl;
    oss << "  Warnings: " << pImpl->warnings.size() << std::endl;
    return oss.str();
}

// Validation helpers
bool GlobalConfig::validateBrainConfig() const {
    auto neuronCount = pImpl->config->get<int64_t>(ConfigDefaults::BRAIN_NEURON_COUNT);
    if (!neuronCount || *neuronCount <= 0) return false;
    return true;
}

bool GlobalConfig::validatePlasticityConfig() const {
    // Check plasticity parameters are within valid ranges
    auto stdpLearningRate = pImpl->config->get<float>(ConfigDefaults::PLASTICITY_STDP_LEARNING_RATE);
    if (stdpLearningRate && (*stdpLearningRate < 0.0f || *stdpLearningRate > 10.0f)) return false;
    return true;
}

bool GlobalConfig::validateMemoryConfig() const {
    // Memory capacity validation
    auto workingCapacity = pImpl->config->get<int64_t>(ConfigDefaults::MEMORY_WORKING_CAPACITY);
    if (workingCapacity && *workingCapacity <= 0) return false;
    return true;
}

bool GlobalConfig::validateNeuromodulationConfig() const {
    // Neuromodulation parameter validation
    auto dopamineScale = pImpl->config->get<float>(ConfigDefaults::NEUROMOD_DOPAMINE_SCALE);
    if (dopamineScale && (*dopamineScale < 0.0f || *dopamineScale > 10.0f)) return false;
    return true;
}

bool GlobalConfig::validatePredictionConfig() const {
    // Prediction configuration validation
    auto predictionAlpha = pImpl->config->get<float>(ConfigDefaults::PREDICTION_ALPHA);
    if (predictionAlpha && (*predictionAlpha < 0.0f || *predictionAlpha > 1.0f)) return false;
    return true;
}

bool GlobalConfig::validateCognitionConfig() const {
    // Cognition configuration validation
    auto planningDepth = pImpl->config->get<int64_t>(ConfigDefaults::COGNITION_PLANNING_DEPTH);
    if (planningDepth && (*planningDepth <= 0 || *planningDepth > 20)) return false;
    return true;
}

bool GlobalConfig::validateDevelopmentConfig() const {
    // Development configuration validation
    auto initialAge = pImpl->config->get<float>(ConfigDefaults::DEVELOPMENT_INITIAL_AGE);
    if (initialAge && *initialAge < 0.0f) return false;
    
    auto criticalPeriodEnd = pImpl->config->get<float>(ConfigDefaults::DEVELOPMENT_CRITICAL_PERIOD_END);
    if (criticalPeriodEnd && *criticalPeriodEnd <= 0.0f) return false;
    
    return true;
}

} // namespace nlm