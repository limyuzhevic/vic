#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal, vigilance, stress response, and adaptive processing
// 
// Key functions:
// - Regulates arousal and vigilance states
// - Enhances alertness and focused attention
// - Mediates stress response and adaptation
// - Facilitates memory consolidation during alert states
// - Modulates adaptive learning and flexibility
// - Influences emotional reactivity

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get NE level
    float getLevel() const;
    void setLevel(float level);
    
    // Get neuromodulator name
    const char* getName() const;
    
    // Get neuromodulator effects on plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Signal specific noradrenergic effects
    // Increase arousal and vigilance
    void signalArousal(float level);
    
    // Enhance alertness and focused attention
    void signalAlertness(float level);
    
    // Initiate stress response
    void signalStress(float intensity);
    
    // Enhance memory consolidation during alert states
    void signalMemoryConsolidation(float factor);
    
    // Increase adaptive processing and flexibility
    void signalAdaptation(float rate);
    
    // Get current arousal level
    float getArousal() const;
    
    // Get current alertness level
    float getAlertness() const;
    
    // Get current stress level
    float getStressLevel() const;
    
    // Get memory consolidation enhancement
    float getMemoryConsolidation() const;
    
    // Get adaptive processing rate
    float getAdaptation() const;
    
    // Reset for new learning episode
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm