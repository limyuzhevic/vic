#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// Real novelty system: neural-based novelty detection and adaptation

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
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
    
    // Get novelty statistics
    float getNoveltyMean() const;
    float getNoveltyVariance() const;
    float getLastNoveltyChange() const;
    
    // Set novelty detection parameters
    void setNoveltyThreshold(float threshold);
    void setPatternMatchingThreshold(float threshold);
    void setAdaptationRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
