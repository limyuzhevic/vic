#include "factories/NeuromodulatorFactory.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

struct NeuromodulatorFactory::Impl {
    size_t creationCount;
    std::string lastCreatedType;
};

NeuromodulatorFactory::NeuromodulatorFactory() : creationCount_(0) {
    pImpl_ = std::make_unique<Impl>();
    pImpl_->creationCount = 0;
    pImpl_->lastCreatedType = "None";
}

NeuromodulatorFactory::~NeuromodulatorFactory() = default;

std::unique_ptr<AbstractNeuromodulator> NeuromodulatorFactory::createNeuromodulator(
    NeuromodulatorType type,
    std::shared_ptr<Config> config
) {
    auto neuromodulator = createFromConfig(type, config);
    ++pImpl_->creationCount;
    ++creationCount_;
    return neuromodulator;
}

std::unique_ptr<AbstractNeuromodulator> NeuromodulatorFactory::createNeuromodulator(
    const std::string& neuromodulatorName,
    std::shared_ptr<Config> config
) {
    NeuromodulatorType type;
    
    // Map string names to types
    if (neuromodulatorName == "Dopamine") type = NeuromodulatorType::Dopamine;
    else if (neuromodulatorName == "Curiosity") type = NeuromodulatorType::Curiosity;
    else if (neuromodulatorName == "Novelty") type = NeuromodulatorType::Novelty;
    else if (neuromodulatorName == "PredictionError") type = NeuromodulatorType::PredictionError;
    else type = NeuromodulatorType::Dopamine; // Default to Dopamine
    
    return createNeuromodulator(type, config);
}

std::unique_ptr<AbstractNeuromodulator> NeuromodulatorFactory::createFromConfig(
    NeuromodulatorType type,
    std::shared_ptr<Config> config
) {
    switch (type) {
        case NeuromodulatorType::Dopamine:
            return createDopamine(config);
        case NeuromodulatorType::Curiosity:
            return createCuriosity(config);
        case NeuromodulatorType::Novelty:
            return createNovelty(config);
        case NeuromodulatorType::PredictionError:
            return createPredictionError(config);
        default:
            return nullptr;
    }
}

std::unique_ptr<AbstractNeuromodulator> NeuromodulatorFactory::createFromConfig(
    const Config& config
) {
    // Get neuromodulator type from config
    std::string neuromodulatorType = config.get_or<std::string>("neuromodulator_type", "Dopamine");
    
    // Map type name to enum
    NeuromodulatorType type;
    if (neuromodulatorType == "Dopamine") type = NeuromodulatorType::Dopamine;
    else if (neuromodulatorType == "Curiosity") type = NeuromodulatorType::Curiosity;
    else if (neuromodulatorType == "Novelty") type = NeuromodulatorType::Novelty;
    else if (neuromodulatorType == "PredictionError") type = NeuromodulatorType::PredictionError;
    else type = NeuromodulatorType::Dopamine; // Default to Dopamine
    
    return createFromConfig(type, std::make_shared<Config>(config));
}

std::unique_ptr<AbstractNeuromodulator> NeuromodulatorFactory::createFromPlugin(
    const std::string& pluginName,
    std::shared_ptr<Config> config
) {
    auto it = plugins_.find(pluginName);
    if (it != plugins_.end()) {
        auto neuromodulator = it->second(config);
        ++pImpl_->creationCount;
        ++creationCount_;
        return neuromodulator;
    }
    
    NLM_LOG_ERROR("Plugin not found: " + pluginName);
    return nullptr;
}

void NeuromodulatorFactory::configureFromConfig(AbstractNeuromodulator* neuromodulator, const Config& config) {
    if (!neuromodulator) return;
    
    NLM_LOG_INFO("Configuring neuromodulator from config");
}

void NeuromodulatorFactory::configureFromConfig(Dopamine* dopamine, const Config& config) {
    if (!dopamine) return;
    
    float initialLevel = config.get_or<float>("dopamine_initial_level", 0.5f);
    float learningRate = config.get_or<float>("dopamine_learning_rate", 0.01f);
    
    dopamine->setLevel(initialLevel);
}

void NeuromodulatorFactory::configureFromConfig(Curiosity* curiosity, const Config& config) {
    if (!curiosity) return;
    
    float noveltyWeight = config.get_or<float>("curiosity_novelty_weight", 0.5f);
    float predictionErrorWeight = config.get_or<float>("curiosity_prediction_error_weight", 0.5f);
    
    curiosity->setNoveltyWeight(noveltyWeight);
    curiosity->setPredictionErrorWeight(predictionErrorWeight);
}

void NeuromodulatorFactory::configureFromConfig(Novelty* novelty, const Config& config) {
    if (!novelty) return;
    
    float sensitivity = config.get_or<float>("novelty_sensitivity", 1.0f);
    float decayRate = config.get_or<float>("novelty_decay_rate", 0.01f);
    
    novelty->setSensitivity(sensitivity);
    novelty->setDecayRate(decayRate);
}

void NeuromodulatorFactory::configureFromConfig(PredictionError* predictionError, const Config& config) {
    if (!predictionError) return;
    
    float predictionGain = config.get_or<float>("prediction_error_gain", 1.0f);
    float errorThreshold = config.get_or<float>("prediction_error_threshold", 0.1f);
    
    predictionError->setPredictionGain(predictionGain);
    predictionError->setErrorThreshold(errorThreshold);
}

bool NeuromodulatorFactory::registerPlugin(
    const std::string& name,
    std::function<std::unique_ptr<AbstractNeuromodulator>(std::shared_ptr<Config>)> factory
) {
    plugins_[name] = factory;
    NLM_LOG_INFO("Registered neuromodulator plugin: " + name);
    return true;
}

std::unique_ptr<AbstractNeuromodulator> NeuromodulatorFactory::createFromRegisteredPlugin(
    const std::string& name,
    std::shared_ptr<Config> config
) {
    return createFromPlugin(name, config);
}

std::vector<std::string> NeuromodulatorFactory::getRegisteredPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins_) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

} // namespace nlm
