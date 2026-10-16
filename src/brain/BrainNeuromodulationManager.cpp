#include "BrainNeuromodulationManager.hpp"
#include "BrainNeuralCore.hpp"
#include "BrainPlasticityManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct BrainNeuromodulationManager::Impl {
    std::shared_ptr<Config> config;
    
    // Neuromodulation system components
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    // Current observation for novelty detection
    std::vector<float> lastObservation;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {}
};

BrainNeuromodulationManager::BrainNeuromodulationManager(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainNeuromodulationManager::~BrainNeuromodulationManager() = default;

bool BrainNeuromodulationManager::initialize(BrainNeuralCore* neuralCore) {
    NLM_LOG_INFO("Initializing NLM Brain Neuromodulation Manager...");
    
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    novelty = std::make_unique<Novelty>();
    predictionError = std::make_unique<PredictionError>();
    
    // Initialize with brain reference (neural core)
    curiosity->initialize(neuralCore);
    novelty->initialize(neuralCore);
    predictionError->initialize(neuralCore);
    
    // Configure parameters from config
    if (curiosity) {
        curiosity->setNoveltyWeight(pImpl->config->getOr<float>("curiosity_novelty_weight", 0.5f));
        curiosity->setPredictionErrorWeight(pImpl->config->getOr<float>("curiosity_pe_weight", 0.5f));
    }
    
    NLM_LOG_INFO("NLM Brain Neuromodulation Manager initialization complete");
    return true;
}

void BrainNeuromodulationManager::update(TimestepDuration timestep, const class Observation* observation) {
    // Get current observation if available
    if (observation) {
        pImpl->lastObservation = observation->getData();
    }
    
    // Update novelty detection if we have last observation
    if (!pImpl->lastObservation.empty()) {
        std::vector<float> currentPattern = pImpl->lastObservation; // In real implementation, would get current
        novelty->detectNovelty(currentPattern, currentPattern); // Placeholder
    }
    
    // Update novelty (decay)
    novelty->update(timestep);
    
    // Update curiosity based on novelty and prediction error
    float noveltyLevel = novelty->getLevel();
    float predictionErrorLevel = predictionError->getError();
    curiosity->update(noveltyLevel, predictionErrorLevel, timestep);
    
    // Update dopamine level
    dopamine->update(timestep);
    
    // Update prediction error
    predictionError->updatePrediction(0.0f); // Placeholder
}

void BrainNeuromodulationManager::signalReward(float reward) {
    if (dopamine) {
        dopamine->signalReward(reward);
        NLM_LOG_INFO("Reward signal sent (value: " + std::to_string(reward) + ")");
    }
}

void BrainNeuromodulationManager::signalPredictionError(float error) {
    if (predictionError) {
        predictionError->computeError(0.0f, error); // Placeholder
        NLM_LOG_INFO("Prediction error signal sent (value: " + std::to_string(error) + ")");
    }
}

void BrainNeuromodulationManager::signalNovelty(const class Observation& observation) {
    if (novelty) {
        if (!pImpl->lastObservation.empty()) {
            novelty->detectNovelty(observation.getData(), pImpl->lastObservation);
            pImpl->lastObservation = observation.getData();
        }
        NLM_LOG_INFO("Novelty signal sent (level: " + std::to_string(novelty->getLevel()) + ")");
    }
}

void BrainNeuromodulationManager::computeCuriosity() {
    if (curiosity) {
        curiosity->update(novelty ? novelty->getLevel() : 0.0f,
                         predictionError ? predictionError->getError() : 0.0f,
                         0.001f); // Placeholder timestep
    }
}

void BrainNeuromodulationManager::applyEffectsToNeuralCore(BrainNeuralCore* neuralCore) {
    if (dopamine && neuralCore) {
        // Apply dopamine effects on neural excitability
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : neuralCore->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainNeuromodulationManager::applyEffectsToPlasticity(BrainPlasticityManager* plasticityManager) {
    if (dopamine && plasticityManager) {
        // Apply neuromodulation effects on plasticity
        float modulation = dopamine->getPlasticityFactor();
        plasticityManager->applyNeuromodulation(*dopamine);
    }
}

Dopamine* BrainNeuromodulationManager::getDopamine() {
    return dopamine.get();
}

const Dopamine* BrainNeuromodulationManager::getDopamine() const {
    return dopamine.get();
}

Curiosity* BrainNeuromodulationManager::getCuriosity() {
    return curiosity.get();
}

const Curiosity* BrainNeuromodulationManager::getCuriosity() const {
    return curiosity.get();
}

Novelty* BrainNeuromodulationManager::getNovelty() {
    return novelty.get();
}

const Novelty* BrainNeuromodulationManager::getNovelty() const {
    return novelty.get();
}

PredictionError* BrainNeuromodulationManager::getPredictionError() {
    return predictionError.get();
}

const PredictionError* BrainNeuromodulationManager::getPredictionError() const {
    return predictionError.get();
}

void BrainNeuromodulationManager::reset() {
    NLM_LOG_INFO("Resetting NLM Brain Neuromodulation Manager...");
    
    if (dopamine) dopamine->reset();
    if (curiosity) curiosity->reset();
    if (novelty) novelty->reset();
    if (predictionError) predictionError->reset();
    
    pImpl->lastObservation.clear();
    
    NLM_LOG_INFO("NLM Brain Neuromodulation Manager reset complete");
}

std::shared_ptr<const Config> BrainNeuromodulationManager::getConfig() const {
    return pImpl->config;
}

void BrainNeuromodulationManager::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Neuromodulation Manager Status ===");
    if (dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    }
    if (curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosity->getLevel()));
    }
    if (novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(novelty->getLevel()));
    }
    if (predictionError) {
        NLM_LOG_INFO("Prediction error: " + std::to_string(predictionError->getError()));
    }
}

} // namespace nlm