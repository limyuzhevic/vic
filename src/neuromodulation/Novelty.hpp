// Novelty.hpp
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Novelty: Novelty detection and exploration driver
// Detects unexpected sensory input and drives exploration behavior

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Detect novelty from sensory input
    void detectNovelty(const SensoryInput& input);
    
    // Get current novelty level
    float getLevel() const;
    
    // Get contribution to prediction error
    float getPredictionErrorContribution() const;
    
    // Update novelty system state
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm