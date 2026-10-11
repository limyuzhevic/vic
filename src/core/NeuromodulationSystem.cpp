// NeuromodulationSystem implementation - Handles dopamine, curiosity, novelty, and prediction error

#include "NeuromodulationSystem.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>

namespace nlm {

NeuromodulationSystem::NeuromodulationSystem(std::shared_ptr<Brain> brain)
    : brain_(brain), dopamineLevel_(0.0f) {
}

NeuromodulationSystem::~NeuromodulationSystem() = default;

bool NeuromodulationSystem::initialize() {
    // Neuromodulation systems are created by Brain during its initialization
    // This system just provides access and coordination
    
    return true;
}

void NeuromodulationSystem::step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    // Update neuromodulation systems
    updateDopamineSystem(timestep);
    updateCuriositySystem(timestep);
    updateNoveltySystem(timestep);
    updatePredictionErrorSystem(timestep);
    
    // Apply neuromodulation effects on neurons
    applyNeuromodulationToNeurons();
}

void NeuromodulationSystem::updateDopamineSystem(TimestepDuration timestep) {
    if (dopamine_) {
        dopamine_->update(timestep);
        dopamineLevel_ = dopamine_->getLevel();
    }
}

void NeuromodulationSystem::updateCuriositySystem(TimestepDuration timestep) {
    if (curiosity_) {
        curiosity_->update(timestep);
    }
}

void NeuromodulationSystem::updateNoveltySystem(TimestepDuration timestep) {
    if (novelty_) {
        novelty_->update(timestep);
    }
}

void NeuromodulationSystem::updatePredictionErrorSystem(TimestepDuration timestep) {
    if (predictionError_) {
        predictionError_->update(timestep);
    }
}

void NeuromodulationSystem::applyNeuromodulationToNeurons() {
    if (dopamine_ && brain_) {
        float dopamineLevel = dopamine_->getLevel();
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = dopamineLevel * 0.5f;
        
        for (auto& region : brain_->getRegions()) {
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

void NeuromodulationSystem::applyRewardModulation(float reward, float predictedReward) {
    if (dopamine_) {
        // Compute prediction error
        float predictionError = reward - predictedReward;
        
        // Update dopamine based on prediction error
        dopamine_->update(predictionError);
    }
}

} // namespace nlm
