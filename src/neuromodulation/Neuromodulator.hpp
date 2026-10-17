#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 6: Full integration of neuromodulation system

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity (1.0 = baseline)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with elapsed time
    virtual void update(TimestepDuration dt) = 0;
    
    // Cross-modulator interactions
    virtual void interactWith(const Neuromodulator& other) = 0;
    
    // Get influence on attention (0.0 to 1.0)
    virtual float getAttentionInfluence() const = 0;
    
    // Get influence on memory consolidation (0.0 to 1.0)
    virtual float getMemoryInfluence() const = 0;
    
    // Get influence on arousal (0.0 to 1.0)
    virtual float getArousalInfluence() const = 0;
    
    // Get influence on decision making (0.0 to 1.0)
    virtual float getDecisionInfluence() const = 0;
    
    // Get influence on behavioral flexibility (0.0 to 1.0)
    virtual float getFlexibilityInfluence() const = 0;
    
    // Get influence on learning rate (0.0 to 1.0)
    virtual float getLearningRate() const = 0;
    
    // Reset neuromodulator to baseline state
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Phase 6: Enhanced dopamine system with memory consolidation, attention, and prediction error
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void interactWith(const Neuromodulator& other) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Memory consolidation
    void signalMemoryConsolidation();
    void enhanceMemoryEncoding(float encodingStrength);
    
    // Attention modulation
    void modulateAttention(float currentAttention);
    void enhanceFocus(float focusLevel);
    
    // Structural plasticity
    void influenceStructuralPlasticity();
    void guideSynaptogenesis(bool strengthen, bool prune);
    
    // Getters for specific effects
    float getRewardSignal() const;
    float getMemoryConsolidation() const;
    float getAttentionModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Phase 6: Full cholinergic system with attentional gating and memory encoding
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void interactWith(const Neuromodulator& other) override;
    
    // Attention modulation
    void enhanceSelectiveAttention();
    void enhanceSensoryGating(const std::vector<float>& sensoryInput);
    void increaseAttentionSpan(float duration);
    
    // Memory encoding
    void enhanceMemoryEncoding();
    void strengthenWorkingMemory(float capacity);
    void enhanceEpisodicEncoding(float eventSalience);
    
    // Neural excitability
    void modulateNeuralExcitability();
    void increaseSensoryGain(float gainFactor);
    void decreaseNoise(float noiseReduction);
    
    // Cross-system interactions
    void enableTheatricalAttentionSwitching();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Phase 6: Full norepinephrine system with stress response and novelty detection
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void interactWith(const Neuromodulator& other) override;
    
    // Arousal effects
    void increaseAlertness(float level);
    void enhanceVigilance(float duration);
    void modulateStressResponse(float stressLevel);
    
    // Attention modulation
    void enhanceFocusedAttention(float focusDuration);
    void improveReactionTime(float improvementFactor);
    
    // Novelty and arousal
    void respondToNovelty(float noveltyScore);
    void enhanceNoveltyDetection();
    void modulateArousalState(float state);
    
    // Behavioral activation
    void activateBehavioralResponse();
    void enhanceMotivation(float drive);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and behavioral flexibility
// Phase 6: Full serotonin system with mood regulation and behavioral control
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void interactWith(const Neuromodulator& other) override;
    
    // Mood regulation
    void regulateMood(float moodState);
    void reduceImpulsivity(float reductionFactor);
    void enhanceEmotionalStability(float stability);
    
    // Decision making
    void influenceDecisionMaking(float decisionQuality);
    void modulateRiskTaking(float riskLevel);
    void enhanceCognitiveControl(float controlStrength);
    
    // Behavioral flexibility
    void enhanceBehavioralFlexibility();
    void reduceHabitualResponses();
    void increaseExplorationDrive(float driveLevel);
    
    // Social behavior
    void modulateSocialBehavior(float socialityLevel);
    void enhanceSocialLearning();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
