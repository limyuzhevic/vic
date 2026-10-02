#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations
// Implements biological novelty detection for exploration and learning
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
    void detectObservationNovelty(const class Observation& observation, 
                                  const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectPatternNovelty(const std::vector<float>& currentPattern,
                              const std::vector<float>& previousPattern);
    
    // Decay novelty over time (as memory becomes familiar)
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get novelty statistics
    float getMeanNovelty() const;
    float getMaxNovelty() const;
    float getMinNovelty() const;
    
    // Check if novelty is significant
    bool isSignificant() const;
    
    // Get recent novelty (for curiosity integration)
    float getRecentNovelty() const { return recentNovelty_; }
    
    // Set novelty detection parameters
    void setNoveltyThreshold(float threshold);
    void setAdaptationRate(float rate);
    void setDecaysRate(float rate);
    void setMemoryTraceDecay(float decay);
    
    // Get current parameters
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    float getAdaptationRate() const { return adaptationRate_; }
    float getDecayRate() const { return decayRate_; }
    float getMemoryTraceDecay() const { return memoryTraceDecay_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Recent novelty for feedback
    float recentNovelty_;
};

} // namespace nlm
