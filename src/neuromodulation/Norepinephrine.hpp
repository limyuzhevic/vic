#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Biologically inspired neuromodulator that enhances alertness and attention
// Mechanisms:
// - Increase arousal for important stimuli
// - Enhance working memory encoding
// - Modulate sleep-wake cycles
// - Influence emotional learning

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get norepinephrine level
    float getLevel() const;
    void setLevel(float level);
    
    // Update norepinephrine dynamics
    void update(TimestepDuration dt);
    
    // Signal arousal boost from salient stimuli
    void signalArousal(float arousalSignal);
    
    // Modulate vigilance/alertness
    void modulateVigilance(float vigilanceLevel);
    
    // Get current alertness level
    float getAlertness() const;
    
    // Get plasticity factor for this neuromodulator
    float getPlasticityFactor() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm