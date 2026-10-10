#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulatory influence on attention
    // ACh enhances attention, NE modulates vigilance
    virtual float getAttentionModulation() const { return 1.0f; }
    
    // Get neuromodulatory influence on working memory
    // DA, NE, ACh enhance working memory capacity
    virtual float getWorkingMemoryModulation() const { return 1.0f; }
    
    // Get neuromodulatory influence on memory consolidation
    // DA, ACh enhance consolidation, 5-HT modulates persistence
    virtual float getMemoryConsolidationModulation() const { return 1.0f; }
    
    // Get neuromodulatory influence on exploration
    // DA, NE, 5-HT influence exploration/exploitation balance
    virtual float getExplorationModulation() const { return 0.0f; }
    
    // Get neuromodulatory influence on learning rate
    // DA, NE, ACh modulate learning speed
    virtual float getLearningModulation() const { return 1.0f; }
    
    // Get neuromodulatory influence on cognitive flexibility
    // 5-HT, DA modulate switching between behaviors
    virtual float getCognitiveFlexibilityModulation() const { return 1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get dopaminergic influence on attention and cognitive control
    float getAttentionModulation() const override;
    
    // Get dopaminergic influence on working memory
    float getWorkingMemoryModulation() const override;
    
    // Get dopaminergic influence on exploration
    float getExplorationModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get cholinergic influence on attention
    float getAttentionModulation() const override;
    
    // Get cholinergic influence on memory consolidation
    float getMemoryConsolidationModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get noradrenergic influence on arousal and vigilance
    float getAttentionModulation() const override;
    
    // Get noradrenergic influence on working memory
    float getWorkingMemoryModulation() const override;
    
    // Get noradrenergic influence on learning
    float getLearningModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get serotonergic influence on exploration and persistence
    float getExplorationModulation() const override;
    
    // Get serotonergic influence on cognitive flexibility
    float getCognitiveFlexibilityModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
