// NeuromodulationManager implementation
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

NeuromodulationManager::NeuromodulationManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config)
    : brain_(brain), config_(config) {
    // Initialize neuromodulation systems
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
}

bool NeuromodulationManager::initialize() {
    NLM_LOG_INFO("Initializing NeuromodulationManager...");
    
    // Initialize neuromodulation systems with brain reference
    dopamine_->initialize(brain_.get());
    curiosity_->initialize(brain_.get());
    novelty_->initialize(brain_.get());
    predictionError_->initialize(brain_.get());
    
    NLM_LOG_INFO("NeuromodulationManager initialized successfully");
    return true;
}

void NeuromodulationManager::reset() {
    NLM_LOG_INFO("Resetting NeuromodulationManager...");
    if (dopamine_) dopamine_->reset();
    if (curiosity_) curiosity_->reset();
    if (novelty_) novelty_->reset();
    if (predictionError_) predictionError_->reset();
    NLM_LOG_INFO("NeuromodulationManager reset complete");
}

void NeuromodulationManager::update() {
    // Neuromodulation is updated during simulation steps
}

void NeuromodulationManager::logStatus() const {
    NLM_LOG_INFO("=== NeuromodulationManager Status ===");
    if (dopamine_) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine_->getLevel()));
    }
    if (curiosity_) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosity_->getLevel()));
    }
    if (novelty_) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(novelty_->getLevel()));
    }
    if (predictionError_) {
        NLM_LOG_INFO("Prediction error: " + std::to_string(predictionError_->getLevel()));
    }
}

void NeuromodulationManager::updateDopamine(SimulationStep step, Timestamp time) {
    if (dopamine_) {
        dopamine_->update(time);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = dopamine_->getLevel();
        for (auto& region : brain_->getRegions()) {
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

void NeuromodulationManager::updateCuriosity(SimulationStep step, Timestamp time) {
    if (curiosity_) {
        curiosity_->update(time);
    }
}

void NeuromodulationManager::updateNovelty(SimulationStep step, Timestamp time) {
    if (novelty_) {
        novelty_->update(time);
    }
}

void NeuromodulationManager::updatePredictionError(SimulationStep step, Timestamp time) {
    if (predictionError_) {
        predictionError_->update(time);
    }
}

void NeuromodulationManager::applyDopamineEffects(SimulationStep step, Timestamp time) {
    updateDopamine(step, time);
}

void NeuromodulationManager::applyCuriosityEffects(SimulationStep step, Timestamp time) {
    updateCuriosity(step, time);
}

float NeuromodulationManager::getDopamineLevel() const {
    return dopamine_ ? dopamine_->getLevel() : 0.0f;
}

float NeuromodulationManager::getCuriosityLevel() const {
    return curiosity_ ? curiosity_->getLevel() : 0.0f;
}

float NeuromodulationManager::getNoveltyLevel() const {
    return novelty_ ? novelty_->getLevel() : 0.0f;
}

float NeuromodulationManager::getPredictionErrorLevel() const {
    return predictionError_ ? predictionError_->getLevel() : 0.0f;
}

} // namespace nlm