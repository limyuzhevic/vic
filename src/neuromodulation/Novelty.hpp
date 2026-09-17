#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations

class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Get neuromodulator type
    NeuromodulatorType getType() const override;
    
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
