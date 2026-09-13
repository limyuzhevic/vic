#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Novelty: Detects novel sensory patterns and drives exploration
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection for exploration
    void detectNovelty(float activation, float noveltyThreshold);
    
    // Integration with other systems
    void integrateWithCuriosity(float curiosityLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
