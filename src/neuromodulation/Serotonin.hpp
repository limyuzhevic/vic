#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Signal positive social interaction
    void signalPositiveSocialInteraction(float strength, float duration);
    
    // Signal negative social experience
    void signalNegativeSocialExperience(float strength, float duration);
    
    // Reset social state
    void resetSocialState();
    
    // Get current mood
    float getMood() const;
    
    // Check if satiated
    bool isSatiated() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm