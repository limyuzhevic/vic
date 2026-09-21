#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations

class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Neuromodulator overrides
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void setDecayRate(float rate) override;
    
    // Detect novelty from observation
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Decay novelty over time
    // (update override already declared above)
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Set decay rate
    // (setDecayRate override already declared above)
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
