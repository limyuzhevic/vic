#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real pharmacological implementation

class Brain;

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Get 5-HT level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Update 5-HT dynamics
    void update(TimestepDuration dt) override;
    
    // Get 5-HT effects on plasticity
    float getPlasticityFactor() const override;
    
    // Signal events that should trigger 5-HT release
    void signalPositiveSocialEvent();
    void signalNegativeSocialEvent();
    void signalSocialFeedback();
    
    // Get modulated parameters
    void setMoodState(float mood);
    float getMoodState() const;
    float getImpulsivityControl() const;
    float getSocialBehavior() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
