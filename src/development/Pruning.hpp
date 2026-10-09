#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Neuron.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// Real pruning implementation with activity-dependent competition
class Pruning {
public:
    Pruning();
    ~Pruning();
    
    // Get pruning threshold
    float getThreshold() const { return threshold_; }
    void setThreshold(float threshold) { threshold_ = std::max(0.0f, std::min(threshold, 1.0f)); }
    
    // Get pruning rate
    float getPruningRate() const { return pruningRate_; }
    void setPruningRate(float rate) { pruningRate_ = std::max(0.0f, std::min(rate, 1.0f)); }
    
    // Get competition strength
    float getCompetitionStrength() const { return competitionStrength_; }
    void setCompetitionStrength(float strength) { competitionStrength_ = std::max(0.0f, std::min(strength, 1.0f)); }
    
    // Get synaptic scaling factor
    float getSynapticScaling() const { return synapticScaling_; }
    void setSynapticScaling(float scaling) { synapticScaling_ = std::max(0.0f, scaling); }
    
    // Update pruning with biological realism
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt,
                float neuromodulationFactor = 1.0f);
    
    // Prune synapse based on activity and weight
    bool pruneSynapse(Synapse* synapse, Brain* brain);
    
    // Activity-dependent pruning with competition
    bool activityDependentPrune(Synapse* synapse, Brain* brain,
                                 const RandomGenerator& rng, float globalActivity);
    
    // Weak synapse elimination based on weight and activity
    bool weakSynapsePrune(Synapse* synapse, Brain* brain,
                          float globalAverageWeight, float globalAverageActivity);
    
    // Get statistics
    size_t getTotalSynapsesPruned() const { return totalSynapsesPruned_; }
    float getPruningEfficiency() const { return pruningEfficiency_; }
    
    // Reset statistics
    void resetCounters() {
        totalSynapsesPruned_ = 0;
        pruningEfficiency_ = 0.0f;
    }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float threshold_;
    float pruningRate_;
    float competitionStrength_;
    float synapticScaling_;
    size_t totalSynapsesPruned_;
    float pruningEfficiency_;
};

} // namespace nlm