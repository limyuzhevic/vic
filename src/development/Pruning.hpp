#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Synapse pruning: biologically realistic removal of weak or unused synapses
// Implements activity-dependent, competitive, and homeostatic pruning mechanisms

class Pruning {
public:
    Pruning();
    ~Pruning();
    
    // Configuration parameters
    void setThreshold(float threshold);              // Minimum synaptic weight for pruning (0-1)
    void setPruningRate(float rate);                 // Base pruning rate (synapses/ms)
    void setCompetitionStrength(float strength);    // Strength of competitive pruning (0-1)
    void setActivityThreshold(float threshold);     // Minimum activity for survival (Hz)
    void setResourceCapacity(float capacity);       // Maximum synapses before pruning (0-1)
    void setTimeConstant(float tau);                // Time constant for pruning dynamics (ms)
    
    // Get current state
    float getThreshold() const;
    float getPruningRate() const;
    float getCompetitionStrength() const;
    float getActivityThreshold() const;
    float getResourceCapacity() const;
    float getTimeConstant() const;
    
    // Update pruning processes
    void update(class Brain* brain, class RandomGenerator& rng, TimestepDuration dt);
    void update(class Brain* brain, class RandomGenerator& rng);
    
    // Get pruning statistics
    size_t getPrunedSynapseCount() const;            // Total number of pruned synapses
    size_t getCurrentPrunedCount() const;            // Currently pruned synapses
    float getPruningEfficiency() const;             // Effectiveness of pruning (0-1)
    float getResourceRelief() const;                // Resource savings from pruning (0-1)
    
    // Reset and initialize
    void reset();
    void initialize(class Brain* brain, class RandomGenerator& rng);
    
    // Pruning control methods
    bool pruneSynapse(class Synapse* synapse);        // Force prune specific synapse
    bool isSynapsePrunable(class Synapse* synapse) const; // Check if synapse is eligible for pruning
    
    // Biological mechanisms
    void setHebbianProtection(float strength);       // Strength of Hebbian protection for synapses
    void setHomeostaticPruning(float strength);     // Strength of homeostatic pruning
    void setActivityDependentPruning(bool enable);  // Enable activity-dependent pruning
    void setCompetitivePruning(bool enable);        // Enable competitive pruning
    
    // Get mechanism status
    bool isActivityDependentPruningEnabled() const;
    bool isCompetitivePruningEnabled() const;
    float getHebbianProtectionStrength() const;      // Protection from Hebbian mechanisms
    float getHomeostaticPruningStrength() const;    // Contribution from homeostatic mechanisms
    
    // Get synapse evaluation scores
    float getSynapsePrunabilityScore(class Synapse* synapse) const; // Overall prunability score
    float getActivityScore(class Synapse* synapse) const;          // Activity-based pruning score
    float getStrengthScore(class Synapse* synapse) const;         // Weight-based pruning score
    float getRecencyScore(class Synapse* synapse) const;          // Recent activity pruning score
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
