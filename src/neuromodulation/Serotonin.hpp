#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Phase 2 implementation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social functions
    void signalMood(float mood);
    void reduceImpulsivity(const std::vector<NeuronId>& neurons, float strength);
    void enableSocialLearning(Brain* socialPartner);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm