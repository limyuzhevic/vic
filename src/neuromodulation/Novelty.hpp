#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations
// Now properly inherits from Neuromodulator

class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
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
    
    // Get novelty parameters
    float getDecayRate() const;
    float getNoveltyThreshold() const;
    
    // Set novelty parameters
    void setDecayRate(float rate);
    void setNoveltyThreshold(float threshold);
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
