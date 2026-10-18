#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <unordered_map>
#include <vector>

namespace nlm {

// Advanced Metaplasticity Manager
// Manages multiple metaplastic mechanisms simultaneously
class MetaplasticityManager {
public:
    MetaplasticityManager();
    ~MetaplasticityManager();
    
    void initialize(class Brain* brain);
    
    // Learning rate homeostasis - prevents runaway excitation/inhibition
    void enforceLearningRateHomeostasis(float& learningRate) const;
    
    // History-dependent plasticity - adapts based on recent learning experience
    void applyHistoryDependentPlasticity(class Synapse* synapse, float baseLearningRate, 
                                        const std::vector<float>& learningHistory) const;
    
    // Resource-limited plasticity - allocates plasticity budget across synapses
    bool allocatePlasticityBudget(float requiredBudget, const SynapseId& synapseId) const;
    void releasePlasticityBudget(float budgetReleased, const SynapseId& synapseId) const;
    float getRemainingPlasticityBudget() const;
    
    // Metaplastic thresholds - change how easily synapses can change
    void setMetaplasticThreshold(float threshold, const SynapseId& synapseId);
    float getMetaplasticThreshold(const SynapseId& synapseId) const;
    void updateThresholdBasedOnRecentLearning(const SynapseId& synapseId, float learningRate);
    
    // Synaptic tagging and capture
    void tagSynapseForLearning(const SynapseId& synapseId, float tagStrength);
    bool isSynapseTagged(const SynapseId& synapseId) const;
    float getTagStrength(const SynapseId& synapseId) const;
    void clearTag(const SynapseId& synapseId);
    
    // BCM-like rules - sliding threshold for LTP/LTD
    void setBCMThreshold(float thetaPlus, float thetaMinus);
    float getBCMThetaPlus() const;
    float getBCMThetaMinus() const;
    void updateBCMThreshold(float avgPostSynapticActivity);
    
    // Homeostatic plasticity - maintain stability
    void enforceHomeostaticPlasticity(float targetActivityLevel);
    void adjustSynapticWeightsToMaintainHomeostasis() const;
    
    // Integration with neuromodulators
    void integrateNeuromodulation(const Neuromodulator& modulator);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
