#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Novelty detection from neural activity patterns
// Extracts novel features from observation patterns and neural activity
// Computes novelty as the deviation from expected patterns

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference for accessing neural data
    void initialize(class Brain* brain);
    
    // Get current novelty level (0.0 to 1.0)
    float getLevel() const;
    
    // Set novelty level
    void setLevel(float level);
    
    // Detect novelty from two observations (previous and current)
    // Extracts features from neural activity patterns
    void detectNovelty(const Observation& observation, 
                       const Observation& previousObservation);
    
    // Detect novelty from neural activity patterns
    // Computes deviation from expected patterns using neural firing rates
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Update novelty state (apply decay)
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get novelty threshold
    float getThreshold() const;
    void setThreshold(float threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
