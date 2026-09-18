#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations

    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Get novelty components
    float getSurpriseMagnitude() const;
    float getRecentNovelty() const;
    float getSustainedNovelty() const;
    float getNoveltyVelocity() const;
    
    // Detection methods
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Configuration methods
    void setNoveltyThreshold(float threshold);
    void setDecayRate(float rate);
    void setAdaptationRate(float rate);
    void setSurpriseThreshold(float threshold);
    
    // Feature extraction
    void extractFeatures(const class Observation& observation, std::vector<float>& features) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
