#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm
{

// Serotonin: Mood regulation, motivation, sleep
// Modulates motivation and decision making
// Important for reward learning and punishment avoidance
// Affects mood and emotional responses

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // 5-HT modulates motivation and decision making
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Signal reward or punishment event
    void signalRewardEvent(float reward, float predictionError);
    
    // Modulate motivation and decision making
    void modulateMotivation(class Action* action, float currentLevel);
    
    // Enhance reward learning and punishment avoidance
    void enhanceLearningFromReward(const std::vector<float>& pattern, float reward);
    
    // Affect mood and emotional responses
    void affectMood(float moodLevel, float emotionalState);
    
    // Reset state
    void reset();
    
    // Get motivation level (derived from 5-HT)
    float getMotivationLevel() const;
    
    // Get reward learning enhancement factor
    float getRewardLearningFactor() const;
    
    // Get mood level (derived from 5-HT)
    float getMoodLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm