#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Neuromodulator integration system
// Coordinates all neuromodulators and applies their effects to brain regions
// Implements complex interactions and feedback loops

class NeuromodulatorIntegration {
public:
    NeuromodulatorIntegration();
    ~NeuromodulatorIntegration();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Apply neuromodulator effects to neural excitability
    void applyToNeuralExcitability(class NeuralRegion* region);
    
    // Apply neuromodulator effects to synaptic plasticity
    void applyToSynapticPlasticity(class Synapse* synapse);
    
    // Add neuromodulator state to Brain checkpoints
    void checkpointNeuromodulatorState();
    
    // Include neuromodulator feedback loops
    void updateFeedbackLoops();
    
    // Get current neuromodulator levels
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
    // Control neuromodulator systems
    void activateRewardPathway(float rewardStrength);
    void activateStressResponse(float stressLevel);
    void activateSocialBehavior(float socialLevel);
    void activateAttentionModulation(float attentionLevel);
    void activateNoveltyDetection(const Observation& observation, 
                                  const Observation& previousObservation);
    void activateCuriosity(float novelty, float predictionError);
    void activatePredictionError(float predicted, float actual);
    
    // Get neuromodulator state information
    float getGlobalArousal() const;
    float getBehavioralFlexibility() const;
    float getExplorationDrive() const;
    float getLearningRate() const;
    
    // System interaction and synchronization
    void synchronizeNeuromodulators();
    void applySystemWideFeedback();
    
    // State management
    void reset();
    void saveState(std::vector<float>& state) const;
    void loadState(const std::vector<float>& state);
    
    // Diagnostics and monitoring
    void logNeuromodulatorState() const;
    float getSystemIntegrity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm