#pragma once

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Types/Types.hpp"
#include "../neuromodulation/NeuralDynamics.hpp"
#include <vector>
#include <memory>

namespace nlm {

class PlasticityManager {
public:
    PlasticityManager(std::unique_ptr<STDP> stdp,
                     std::unique_ptr<Hebbian> hebbian,
                     std::unique_ptr<StructuralPlasticity> structuralPlasticity,
                     NeuralDynamics* neuralDynamics = nullptr);
    
    ~PlasticityManager() = default;
    
    // Apply all plasticity rules to a synapse
    void applyPlasticity(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt,
                        float neuromodulationFactor = 1.0f);
    
    // Apply plasticity rules to all regions in the brain
    void applyToAllRegions(std::vector<NeuralRegion*>& regions,
                          TimestepDuration dt,
                          float neuromodulationFactor = 1.0f,
                          SimulationStep currentStep = 0);
    
    // Apply structural plasticity periodically
    bool shouldApplyStructuralPlasticity(SimulationStep currentStep) const;
    
    // Apply structural plasticity to all regions
    void applyStructuralPlasticityToRegions(std::vector<NeuralRegion*>& regions,
                                          TimestepDuration dt,
                                          SimulationStep currentStep);
    
    // Getters and setters for plasticity parameters
    void setStdpLearningRate(float rate);
    float getStdpLearningRate() const;
    
    void setHebbianLearningRate(float rate);
    float getHebbianLearningRate() const;
    
    void setStructuralPlasticityInterval(size_t steps);
    size_t getStructuralPlasticityInterval() const;
    
    // Eligibility trace management
    void updateEligibilityTrace(Synapse* synapse, float rewardPrediction);
    void decayEligibilityTraces(float decayRate);
    
    // Weight management with bounds checking
    void clampWeight(Synapse* synapse, float minWeight = -1.0f, float maxWeight = 1.0f) const;
    
    // Reset all eligibility traces
    void resetEligibilityTraces();
    
    // Check if synapse is eligible for plasticity
    bool isSynapseEligible(Synapse* synapse) const;
    
        // Update synapse state after plasticity
    void updateSynapseState(Synapse* synapse, Timestamp currentTime);

    // Current time tracking
    void setCurrentTime(Timestamp time) {
        currentTime_ = time;
    }
    
    Timestamp getCurrentTime() const {
        return currentTime_;
    }

private:
    // Plasticity rules
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
        // Neural dynamics manager for spike handling
    std::unique_ptr<NeuralDynamics> neuralDynamics_;
    
    // Plasticity parameters
    float stdpLearningRate_;
    float hebbianLearningRate_;
    size_t structuralPlasticityInterval_;
    
    // Statistics and tracking
    size_t totalSTDPUpdates_;
    size_t totalHebbianUpdates_;
    size_t totalStructuralUpdates_;
    
    // Current time tracking
    Timestamp currentTime_;
};

} // namespace nlm