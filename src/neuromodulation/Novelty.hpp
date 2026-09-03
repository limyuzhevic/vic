#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// PLACEHOLDER - Phase 2 will implement real novelty detection

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
    // TODO PHASE 2: Implement real novelty detection
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
