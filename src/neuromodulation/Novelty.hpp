#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../environment/Observation.hpp"

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations using sophisticated pattern recognition

class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getNoveltyLevel() const;
    void setLevel(float level) override;
    
    // Detect novelty from observation
    void detectNovelty(const Observation& observation, 
                       const Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Get novelty statistics
    float getNoveltyThreshold() const;
    void setNoveltyThreshold(float threshold);
    
    // Statistics
    float getMaxLevel() const override;
    float getMinLevel() const override;
    float getAverageLevel() const override;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Decay novelty over time
    void update(TimestepDuration dt) override;
    
    // Reset
    void reset() override;
    
    // Parameter setting
    void setBaseline(float baseline) override;
    void setDecayRate(float rate) override;
    void setReleaseRate(float rate) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
