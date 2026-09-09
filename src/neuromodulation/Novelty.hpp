#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// Real novelty detection based on prediction errors
// Change detection and surprise computation
// Exploration-exploitation trade-off

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Update novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Set novelty parameters
    void setNoveltyThreshold(float threshold);
    void setDecayRate(float rate);
    void setIntegrationFactor(float factor);
    void setSurpriseFactor(float factor);
    
    // Compute surprise from prediction error
    float computeSurprise(float predictionError) const;
    
    // Get exploration value for novelty-based exploration
    float getExplorationValue() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
