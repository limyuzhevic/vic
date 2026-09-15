#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

class Brain;

// Real synapse pruning system implementing biologically plausible synaptic elimination
// Based on real neural pruning principles: weak synapse detection, activity-dependent survival,
// probabilistic removal, correlated activity preservation, and density homeostasis
class Pruning {
public:
    Pruning(std::shared_ptr<Config> config);
    ~Pruning();
    
    // Update synapse pruning based on activity and weight
    void update(Brain* brain, RandomGenerator& rng, const DevelopmentSystem& devSys);
    
    // Identify weak synapses for pruning
    void identifyWeakSynapses(Brain* brain, float activityThreshold, float weightThreshold);
    
    // Determine if synapse should survive based on activity-dependent rules
    bool shouldSurvive(Synapse* synapse, const Neuron* pre, const Neuron* post,
                      float activityLevel, float correlatedActivity) const;
    
    // Get current pruning statistics
    size_t getSynapsesPruned() const { return synapsesPruned_; }
    size_t getTotalConnectionsRemoved() const { return totalConnectionsRemoved_; }
    
    // Configuration
    void setPruningRate(float rate) { pruningRate_ = rate; }
    void setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
    void setWeightThreshold(float threshold) { weightThreshold_ = threshold; }
    void setCorrelatedActivityThreshold(float threshold) { correlatedActivityThreshold_ = threshold; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float pruningRate_;
    float activityThreshold_;
    float weightThreshold_;
    float correlatedActivityThreshold_;
    
    size_t synapsesPruned_;
    size_t totalConnectionsRemoved_;
};

} // namespace nlm
