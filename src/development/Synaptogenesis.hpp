#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <unordered_set>
#include <string>

namespace nlm {

// Forward declarations
class Brain;
class RandomGenerator;
class Neuron;
class Synapse;
class NeuralRegion;
class STDP;
class Hebbian;

// Synaptogenesis: formation of new synapses
// REAL IMPLEMENTATION - Phase 2 complete with biologically plausible mechanisms

class Synaptogenesis {
public:
    Synaptogenesis();
    ~Synaptogenesis();
    
    // Get formation rate
    float getFormationRate() const;
    void setFormationRate(float rate);
    
    // Get target synapse density
    float getTargetDensity() const;
    void setTargetDensity(float density);
    
    // Get current synapse density
    float getCurrentSynapseDensity() const { return currentDensity_; }
    
    // Update synapse formation with activity-dependent mechanisms
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Update with simulation step for legacy compatibility
    void update(Brain* brain, RandomGenerator& rng) {
        update(brain, rng, 0.001);  // Default timestep
    }
    
    // Get stage-specific formation multiplier
    float getStageMultiplier(DevelopmentalStage stage) const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper methods for internal implementation
    float calculateCurrentSynapseDensity(Brain* brain) const;
    void updateHomeostaticRegulation(Brain* brain, TimestepDuration dt);
    void updateNeuromodulationInfluences(Brain* brain);
    void performActivityDependentFormation(Brain* brain, RandomGenerator& rng, 
                                          TimestepDuration dt, float effectiveRate);
    void performHebbianFormation(NeuralRegion* region, const std::vector<Neuron*>& neurons,
                                 RandomGenerator& rng, TimestepDuration dt,
                                 float baseProb, float effectiveRate);
    void performSTDPBasedFormation(NeuralRegion* region, const std::vector<Neuron*>& neurons,
                                  RandomGenerator& rng, TimestepDuration dt,
                                  STDP* stdp, float effectiveRate);
    void applyNeuralCompetition(NeuralRegion* region, const std::vector<Neuron*>& neurons,
                               RandomGenerator& rng, TimestepDuration dt);
    void applyTargetDensityControl(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void updateActivityHistory(Brain* brain, TimestepDuration dt);
    void applyMolecularStabilization(NeuralRegion* region, RandomGenerator& rng, TimestepDuration dt);
    
    // Molecular mechanism tracking
    std::vector<float> synapseStabilizationFactors_;
    std::vector<float> birthDates_;
    std::vector<SynapseId> recentSynapses_;
    float synapseLifespan_;
    float currentDensity_;
    
    // Activity history for Hebbian learning
    std::unordered_set<NeuronId> recentlyActiveNeurons_;
    float activityHistoryDecay_;
    
    // Helper calculation methods
    float calculateNeuronActivity(Neuron* neuron) const;
    float calculateHebbianEligibility(Neuron* neuronA, Neuron* neuronB, TimestepDuration dt) const;
    float calculateSynapseActivity(Synapse* synapse) const;
    size_t findSynapseIndex(SynapseId synapseId) const;
    void updateRecentSynapseTracking(Brain* brain, TimestepDuration dt);
};

} // namespace nlm
