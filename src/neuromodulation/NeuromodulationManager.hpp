#pragma once

#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/NeuralDynamics.hpp"

namespace nlm {

class NeuromodulationManager {
public:
    NeuromodulationManager() = default;
    ~NeuromodulationManager() = default;
    
    // Initialize neuromodulation manager with brain reference for state updates
    void initialize(Brain* brain) {
        brain_ = brain;
        // Initialize statistics
        stats_.totalSteps = 0;
        stats_.plasticityUpdateCount = 0;
        stats_.stateUpdateCount = 0;
    }
    
    // Main update function - handles all neuromodulation systems
    void update(TimestepDuration dt) {
        if (!brain_) return;
        
        stats_.totalSteps++;
        
        // Update neuromodulator systems
        updateNeuromodulators(dt);
        
        // Apply neuromodulation effects to neural excitability
        applyNeuromodulationEffects();
        
        // Calculate and apply neuromodulation factors for plasticity
        applyPlasticityFactors();
        
        stats_.stateUpdateCount++;
    }
    
    // Get neuromodulation statistics
    const NeuromodulationStats& getStats() const { return stats_; }
    
    // Check if neuromodulator is initialized
    bool isInitialized() const { return brain_ != nullptr; }
    
    // Reset statistics
    void resetStats() {
        stats_.totalSteps = 0;
        stats_.plasticityUpdateCount = 0;
        stats_.stateUpdateCount = 0;
    }
    
private:
    // Update all neuromodulator systems
    void updateNeuromodulators(TimestepDuration dt) {
        if (!brain_) return;
        
        // Update novelty detection
        if (brain_->getNovelty()) {
            brain_->getNovelty()->update(dt);
        }
        
        // Update curiosity system
        if (brain_->getCuriosity()) {
            brain_->getCuriosity()->update(dt);
        }
        
        // Update prediction error signal
        if (brain_->getPredictionErrorSignal()) {
            brain_->getPredictionErrorSignal()->update(dt);
        }
        
        // Update dopamine for reward prediction error
        if (brain_->getDopamine()) {
            brain_->getDopamine()->update(dt);
        }
    }
    
    // Apply neuromodulation effects to neural excitability
    void applyNeuromodulationEffects() {
        if (!brain_) return;
        
        // Apply dopamine effects on neural excitability
        if (brain_->getDopamine()) {
            applyDopamineExcitabilityEffects();
        }
        
        // Apply other neuromodulator effects (curiosity, novelty, prediction error)
        applyCuriosityEffects();
        applyNoveltyEffects();
        applyPredictionErrorEffects();
    }
    
    // Apply dopamine modulation to neural excitability
    void applyDopamineExcitabilityEffects() {
        if (!brain_->getDopamine()) return;
        
        float dopamineLevel = brain_->getDopamine()->getLevel();
        
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine increases excitability (lower effective threshold)
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Apply curiosity effects (placeholder for future implementation)
    void applyCuriosityEffects() {
        if (!brain_->getCuriosity()) return;
        
        float curiosityLevel = brain_->getCuriosity()->getLevel();
        // Placeholder for curiosity effects implementation
    }
    
    // Apply novelty effects (placeholder for future implementation)
    void applyNoveltyEffects() {
        if (!brain_->getNovelty()) return;
        
        float noveltyLevel = brain_->getNovelty()->getLevel();
        // Placeholder for novelty effects implementation
    }
    
    // Apply prediction error effects (placeholder for future implementation)
    void applyPredictionErrorEffects() {
        if (!brain_->getPredictionErrorSignal()) return;
        
        float predictionError = brain_->getPredictionErrorSignal()->getLevel();
        // Placeholder for prediction error effects implementation
    }
    
    // Calculate and apply neuromodulation factors for plasticity
    void applyPlasticityFactors() {
        if (!brain_ || !brain_->getDopamine()) return;
        
        // Calculate plasticity modulation factor
        float plasticityMod = brain_->getDopamine()->getPlasticityFactor();
        
        // Update plasticity manager with current time
        if (brain_->getNeuralDynamics()) {
            brain_->getNeuralDynamics()->updateStep(
                brain_->getCurrentStep(), 
                brain_->getCurrentTime()
            );
        }
        
        // Apply plasticity rules to all regions
        brain_->getPlasticityManager()->applyToAllRegions(
            brain_->getRegions(), 
            brain_->getTimestep(), 
            plasticityMod, 
            brain_->getCurrentStep()
        );
        
        // Update statistics
        stats_.plasticityUpdateCount++;
    }
    
private:
    // Reference to brain for accessing neuromodulators and neural regions
    Brain* brain_ = nullptr;
    
    // Neuromodulation statistics
    NeuromodulationStats stats_;
};

} // namespace nlm