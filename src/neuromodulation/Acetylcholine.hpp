#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <queue>
#include <random>

namespace nlm {

// Acetylcholine: Cholinergic burst-pause signaling, attention modulation, memory consolidation
// Based on basal forebrain cholinergic system dynamics
// Implements phasic bursts during novelty/attention and tonic baseline signaling

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Burst-pause dynamics for attention modulation
    void signalBurst();  // Phasic burst for attention
    void signalPause();  // Phasic pause for memory consolidation
    void updateAttention(float attentionLevel, float noveltyLevel);
    
    // Memory consolidation during pauses
    void consolidateMemory(float memoryStrength);
    
    // Cholinergic receptor dynamics
    float getMuscarinicEffect() const;
    float getNicotinicEffect() const;
    
    // Vesicular release and reuptake
    void triggerVesicularRelease();
    void setReuptakeRate(float rate);
    
    // Time constants for different processes
    float getRiseTime() const { return riseTime_; }
    float getFallTime() const { return fallTime_; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm