#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty: Novelty detection and surprise computation
// Detects when sensory input or neural activity patterns are novel/unexpected
// Novelty drives curiosity and exploration motivation

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current novelty level
    float getLevel() const;
    
    // Set novelty level directly (for testing)
    void setLevel(float level);
    
    // Detect novelty from pattern differences
    void detectNovelty(const std::vector<float>& currentPattern,
                      const std::vector<float>& previousPattern);
    
    // Update novelty (decay over time)
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get last observed pattern
    const std::vector<float>& getLastPattern() const;
    
    // Set novelty threshold (higher = harder to detect novelty)
    void setNoveltyThreshold(float threshold) { pImpl->noveltyThreshold = threshold; }
    
    // Reset novelty system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
