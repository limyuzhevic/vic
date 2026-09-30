#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/SleepState.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace nlm {

// Sleep-enhanced plasticity with state-specific STDP and Hebbian rules
class SleepEnhancedPlasticity {
public:
    SleepEnhancedPlasticity();
    ~SleepEnhancedPlasticity();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // State-specific STDP and Hebbian plasticity rules
    void updateSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                    const std::vector<Timestamp>& postSpikeTimes, Timestamp currentTime,
                    SleepState sleepState);
    
    void updateHebbian(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                      const std::vector<Timestamp>& postSpikeTimes, Timestamp currentTime,
                      SleepState sleepState);
    
    // Synaptic consolidation and reorganization
    void consolidateSynapticConnections(SleepState sleepState);
    void reorganizeSynapticConnections(SleepState sleepState);
    
    // Memory strengthening and pattern completion
    void strengthenMemoryConnections(const std::vector<float>& pattern, SleepState sleepState);
    void completeMemoryPattern(const std::vector<float>& pattern, SleepState sleepState);
    
    // Integration with plasticity system
    void integrateWithSTDP(STDP* stdp, SleepState sleepState);
    void integrateWithHebbian(Hebbian* hebbian, SleepState sleepState);
    void integrateWithStructuralPlasticity(StructuralPlasticity* structuralPlasticity, SleepState sleepState);
    
    // State-specific plasticity modulation
    void modulatePlasticityForState(SleepState sleepState);
    float getStateSpecificSTDPLearningRate(SleepState sleepState) const;
    float getStateSpecificHebbianLearningRate(SleepState sleepState) const;
    
    // Synaptic downscaling during deep sleep
    void applySynapticDownscaling(SleepState sleepState);
    void downscaleWeakConnections(SleepState sleepState);
    
    // Memory replay integration
    void integrateWithMemoryReplay(SleepState sleepState);
    void applyReplayedActivityToSynapticWeights(const std::vector<float>& replayedPattern, SleepState sleepState);
    
    // State management
    SleepState getCurrentSleepState() const { return currentSleepState_; }
    void setCurrentSleepState(SleepState state) { currentSleepState_ = state; }
    
    // Plasticity statistics
    float getTotalSynapticWeightChange() const { return totalSynapticWeightChange_; }
    size_t getConsolidationCount() const { return consolidationCount_; }
    size_t getReorganizationCount() const { return reorganizationCount_; }
    
    // Checkpointing support
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Error handling and recovery
    void handlePlasticityError(const std::string& errorMessage);
    void recoverFromPlasticityError();
    bool isInErrorState() const { return inErrorState_; }

private:
    // Core plasticity system components
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // State-specific plasticity parameters
    struct StatePlasticityParameters {
        float stdpLearningRate;           // STDP learning rate for this state
        float hebbianLearningRate;        // Hebbian learning rate for this state
        float consolidationFactor;        // Strength of consolidation
        float reorganizationFactor;       // Strength of reorganization
        float downscalingFactor;          // Synaptic downscaling factor
        bool enableReplayIntegration;     // Whether to integrate with memory replay
        float neuromodulationSensitivity; // Sensitivity to neuromodulation
    };
    
    std::map<SleepState, StatePlasticityParameters> statePlasticityParameters_;
    
    // Statistics
    float totalSynapticWeightChange_;
    size_t consolidationCount_;
    size_t reorganizationCount_;
    
    // Error handling
    bool inErrorState_;
    std::string lastErrorMessage_;
    size_t consecutiveErrors_;
    
    // State management
    SleepState currentSleepState_;
    
    // Synaptic homeostasis
    float synapticHomeostasisLevel_;
    float synapticScalingFactor_;
    
    // Memory replay integration
    float replayIntegrationStrength_;
    std::vector<float> lastReplayedPattern_;
    
    // Helper functions
    void updateStateSpecificPlasticity(SleepState state, Timestamp currentTime);
    void applyStateSpecificSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                               const std::vector<Timestamp>& postSpikeTimes, float learningRate);
    void applyStateSpecificHebbian(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                                   const std::vector<Timestamp>& postSpikeTimes, float learningRate);
    void calculateSynapticHomeostasis();
    void adjustSynapticWeightsForHomeostasis();
    bool validatePlasticityParameters(const StatePlasticityParameters& params) const;
    void resetPlasticityState();
    void updatePlasticityStatistics(float weightChange, bool success);
};

} // namespace nlm
