#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

NeuromodulationManager::NeuromodulationManager(Brain* brain)
    : brain_(brain)
    , dopamine(std::make_unique<Dopamine>())
    , curiosity(std::make_unique<Curiosity>())
    , predictionError(std::make_unique<PredictionError>())
    , novelty(std::make_unique<Novelty>()) {
}

NeuromodulationManager::~NeuromodulationManager() = default;

bool NeuromodulationManager::initialize() {
    NLM_LOG_INFO("Initializing Neuromodulation Manager...");
    
    if (!brain_) return false;
    
    // Initialize neuromodulation systems
    dopamine->initialize(brain_);
    curiosity->initialize(brain_);
    predictionError->initialize(brain_);
    novelty->initialize(brain_);
    
    NLM_LOG_INFO("Neuromodulation Manager initialized successfully");
    return true;
}

void NeuromodulationManager::reset() {
    // Neuromodulation systems maintain state across steps
}

void NeuromodulationManager::step(double timestep) {
    applyDopamineEffects(timestep);
    applyCuriosityEffects(timestep);
    applyNoveltyEffects(timestep);
    applyPredictionErrorEffects(timestep);
}

void NeuromodulationManager::applyDopamineEffects(double timestep) {
    if (dopamine) {
        dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : brain_->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates neural excitability by injecting additional current
                    // Higher dopamine increases excitability (lower effective threshold)
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void NeuromodulationManager::applyCuriosityEffects(double timestep) {
    if (curiosity) {
        curiosity->update(
            novelty ? novelty->getLevel() : 0.0f,
            predictionError ? predictionError->getError() : 0.0f,
            timestep
        );
    }
}

void NeuromodulationManager::applyNoveltyEffects(double timestep) {
    if (novelty) {
        novelty->update(timestep);
    }
}

void NeuromodulationManager::applyPredictionErrorEffects(double timestep) {
    if (predictionError) {
        predictionError->update(timestep);
        
        // Apply prediction error effects on plasticity
        float predictionErrorLevel = predictionError->getError();
        brain_->applyNeuromodulation(*predictionError);
    }
}

} // namespace nlm
