#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations

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
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
