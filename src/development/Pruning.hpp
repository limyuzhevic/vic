#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <unordered_set>
#include <string>
#include <queue>

namespace nlm {

// Forward declarations
class Brain;
class RandomGenerator;
class Neuron;
class Synapse;
class NeuralRegion;
class STDP;
class Hebbian;
class EpisodicMemoryItem;
class NeuralEpisodicMemory;
class PredictionSystem;

// Synapse pruning: removal of weak or unused synapses
// REAL IMPLEMENTATION - Phase 2 complete with biologically plausible mechanisms

class Pruning {
public:
    Pruning();
    ~Pruning();
    
    // Get pruning threshold
    float getThreshold() const;
    void setThreshold(float threshold);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Get current network stability
    float getNetworkStability() const { return pImpl->networkStability; }
    
    // Update pruning with activity-dependent mechanisms
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Update with simulation step for legacy compatibility
    void update(Brain* brain, RandomGenerator& rng) {
        update(brain, rng, 0.001);  // Default timestep
    }
    
    // Force pruning of specific synapse
    bool pruneSynapse(Synapse* synapse);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper methods for internal implementation
    float calculateSynapsePruningScore(Synapse* synapse, Brain* brain, TimestepDuration dt) const;
    float calculateSynapseActivityHistory(Synapse* synapse) const;
    float calculateSynapseMolecularStability(Synapse* synapse) const;
    float calculateMolecularPruningFactor(size_t synapseIndex) const;
    float calculateSynapticStrengtheningPotential(Synapse* synapse) const;
    float calculateNeuronActivity(Neuron* neuron) const;
    float calculateRegionSynapseDensity(NeuralRegion* region) const;
    size_t findSynapseIndex(SynapseId synapseId) const;
    float getSynapseAge(SynapseId synapseId) const;
    
    // Integration helper methods
    void updateNeuromodulationInfluences(Brain* brain);
    void updateHomeostaticRegulation(Brain* brain, TimestepDuration dt);
    void integrateWithEpisodicMemory(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void integrateWithPredictionSystem(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyHomeostaticScaling(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void applyStructuralPlasticityOptimization(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void updateDevelopmentalPruningPatterns(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void updateNetworkStabilityMetrics(Brain* brain);
    void maintainActivityHistory(Brain* brain, TimestepDuration dt);
    
    // Stage-specific implementation methods
    void applyMolecularPruningMechanisms(NeuralRegion* region, RandomGenerator& rng, TimestepDuration dt);
    void applySynapticStrengthening(NeuralRegion* region, RandomGenerator& rng, TimestepDuration dt);
    void applyAgingRelatedDecline(NeuralRegion* region, RandomGenerator& rng, TimestepDuration dt);
    void protectEpisodeRelatedSynapses(Brain* brain, const EpisodicMemoryItem* episode, float relevance);
    void prioritizeWeakSynapsePruning(NeuralRegion* region, float targetPruning, RandomGenerator& rng);
    
    float getStageMultiplier(DevelopmentalStage stage) const;
    float getDevelopmentalPruningScore(Synapse* synapse, Brain* brain) const;
    
    // Legacy step helper
    void performActivityDependentPruning(Brain* brain, RandomGenerator& rng, 
                                        TimestepDuration dt, float effectiveRate);
    void performActivityDependentFormation(Brain* brain, RandomGenerator& rng, 
                                          TimestepDuration dt, float effectiveRate);
};

} // namespace nlm
