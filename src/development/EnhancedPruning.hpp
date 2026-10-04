#pragma once

#include "Pruning.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/RandomGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <queue>

namespace nlm {

class EnhancedPruning : public Pruning {
public:
    EnhancedPruning();
    ~EnhancedPruning() override;
    
    void update(Brain* brain, RandomGenerator& rng) override;
    bool pruneSynapse(Synapse* synapse) override;
    
    // Enhanced parameters
    void setExperienceThreshold(float threshold);           // Threshold for pruning based on experience
    float getExperienceThreshold() const;
    void setActivityThreshold(float threshold);              // Threshold for activity-dependent pruning
    float getActivityThreshold() const;
    void setTargetDensity(float density);                     // Target synaptic density
    float getTargetDensity() const;
    void setHomeostaticScaling(bool enabled);                 // Enable homeostatic scaling
    bool isHomeostaticScalingEnabled() const;
    
    // Pruning strategies
    enum PruningStrategy {
        BASIC_PRUNING,              // Basic threshold-based pruning
        EXPERIENCE_DEPENDENT,       // Prunes synapses with little experience
        ACTIVITY_DEPENDENT,         // Prunes inactive synapses
        COMPETITIVE,                // Competition-based pruning
        HOMEOSTATIC,                // Homeostatically regulated pruning
        DEVELOPER_GUIDED            // Guided by developmental stages
    };
    
    void setPruningStrategy(PruningStrategy strategy);
    PruningStrategy getPruningStrategy() const;
    
    // Connectivity preservation
    void setConnectivityPreservation(float preservation);     // Maintain minimum connectivity
    float getConnectivityPreservation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
