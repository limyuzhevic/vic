#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Synaptogenesis: biologically realistic synapse formation
// Implements activity-dependent and competitive synaptogenesis mechanisms

class Synaptogenesis {
public:
    Synaptogenesis();
    ~Synaptogenesis();
    
    // Configuration parameters
    void setFormationRate(float rate);              // Base formation rate (synapses per ms)
    void setTargetDensity(float density);           // Target synapse density (0-1)
    void setActivityThreshold(float threshold);     // Minimum activity for stabilization
    void setCompetitionStrength(float strength);    // Strength of competitive processes
    void setResourceLimit(float limit);             // Maximum synapses available
    void setTimeConstant(float tau);                // Time constant for dynamics
    
    // Get current state
    float getFormationRate() const;
    float getTargetDensity() const;
    float getActivityThreshold() const;
    float getCompetitionStrength() const;
    float getResourceLimit() const;
    float getTimeConstant() const;
    
    // Update synapse formation (full version)
    void update(class Brain* brain, class RandomGenerator& rng, TimestepDuration dt);
    
    // Update synapse formation (simple version)
    void update(class Brain* brain, class RandomGenerator& rng);
    
    // Get statistics
    size_t getCurrentSynapseCount() const;          // Current number of formed synapses
    float getSynapseDensity() const;                // Current synapse density
    float getActivitySatisfaction() const;          // How close to target activity level
    float getResourceUtilization() const;           // Current resource usage (0-1)
    
    // Reset and initialize
    void reset();
    void initialize(class Brain* brain, class RandomGenerator& rng);
    
    // Get formation candidates for analysis
    const std::vector<SynapseHandle>& getFormationCandidates() const;
    const std::vector<float>& getFormationProbabilities() const;
    
    // Biological mechanisms
    void setHebbianStrength(float strength);        // Strength of Hebbian-like component
    void setNonHebbianStrength(float strength);      // Strength of activity-independent component
    void enableActivityDependent(bool enable);      // Enable/disable activity-dependent synaptogenesis
    void enableCompetitive(bool enable);            // Enable/disable competitive processes
    
    // Get biological mechanism status
    bool isActivityDependentEnabled() const;
    bool isCompetitiveEnabled() const;
    float getHebbianContribution() const;            // Contribution from Hebbian mechanisms
    float getNonHebbianContribution() const;        // Contribution from non-Hebbian mechanisms
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
