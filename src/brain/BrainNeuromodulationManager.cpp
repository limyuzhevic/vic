#include "BrainNeuromodulationManager.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct BrainNeuromodulationManager::Impl {
    // Neuromodulation state
    float rewardPredictionError;
    
    Impl() : rewardPredictionError(0.0f) {}
};

BrainNeuromodulationManager::BrainNeuromodulationManager() : pImpl(std::make_unique<Impl>()) {}

BrainNeuromodulationManager::~BrainNeuromodulationManager() = default;

void BrainNeuromodulationManager::initialize(Brain* brain) {
    // Initialize neuromodulation systems through brain
    if (brain->getDopamine()) {
        brain->getDopamine()->initialize(brain);
    }
    if (brain->getCuriosity()) {
        brain->getCuriosity()->initialize(brain);
    }
    if (brain->getNovelty()) {
        brain->getNovelty()->initialize(brain);
    }
    if (brain->getPredictionErrorSignal()) {
        brain->getPredictionErrorSignal()->initialize(brain);
    }
}

void BrainNeuromodulationManager::updateNeuromodulators(Brain* brain, TimestepDuration timestep) {
    // Update novelty detection
    if (brain->getNovelty()) {
        brain->getNovelty()->update(timestep);
    }
    
    // Update curiosity
    if (brain->getCuriosity()) {
        brain->getCuriosity()->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (brain->getDopamine()) {
        brain->getDopamine()->update(timestep);
    }
}

void BrainNeuromodulationManager::applyNeuromodulationToNeurons(Brain* brain, float neuromodulationLevel) {
    // Apply neuromodulation effects on neural excitability
    // Dopamine modulates neural excitability by adjusting effective current injection
    // Higher dopamine increases excitability (lower effective threshold)
    if (neuromodulationLevel > 0.0f) {
        float excitabilityMod = neuromodulationLevel * 0.5f;
        
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainNeuromodulationManager::applyNeuromodulationToPlasticity(Brain* brain, float neuromodulationLevel) {
    // Scale STDP learning rates based on neuromodulation
    if (brain->getDopamine() && neuromodulationLevel > 0.0f) {
        float plasticityMod = neuromodulationLevel;
        
        // Scale STDP learning rates
        float ltpWeight = 0.01f * plasticityMod;
        float ltdWeight = 0.012f * plasticityMod;
        float tau = 20.0f;
        
        brain->getSTDP()->configure(ltpWeight, ltdWeight, tau);
    }
}

} // namespace nlm
