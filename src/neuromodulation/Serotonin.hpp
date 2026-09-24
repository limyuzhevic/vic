#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Phase 6 Implementation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonin effects on behavior
    void signalMoodChange(float moodShift);
    void signalSocialExperience(float socialValue);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
