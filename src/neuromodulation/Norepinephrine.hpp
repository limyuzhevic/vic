#pragma once

#include "Neuromodulator.hpp"
#include "../../brain/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance neuromodulator
// Biological basis:
// - Locus coeruleus (LC) is the primary source of NE in the brain
// - NE modulates arousal, attention, and stress responses
// - Phasic NE responses signal novel or salient events
// - Tonic NE maintains background level of arousal
// - DA modulates LC activity via D2 receptors
// 
// Functions:
// 1. Arousal regulation: increases overall alertness
// 2. Attentional selection: enhances relevant signals
// 3. Stress response: reacts to environmental challenges
// 4. Learning modulation: affects exploration vs. exploitation balance
// 5. Memory modulation: affects memory consolidation under stress

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalArousal(float intensity, float novelty);
    
    // Stress response
    void signalStress(float stressor, float duration);
    
    // Getters for specialized functions
    float getArousalLevel() const;
    float getVigilanceScore() const;
    float getStressLevel() const;
    float getAlertness() const;
    float getExplorationBias() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm