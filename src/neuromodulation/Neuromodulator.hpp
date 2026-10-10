#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get working memory modulation strength for dopamine
    virtual float getWorkingMemoryModulation() const { return 1.0f; }
    
    // Get attentional selection modulation strength for dopamine
    virtual float getAttentionModulation() const { return 1.0f; }
    
    // Get episodic memory consolidation strength for dopamine
    virtual float getMemoryConsolidationStrength() const { return 1.0f; }
    
    // Get prediction error integration weight for dopamine
    virtual float getPredictionErrorWeight() const { return 1.0f; }
    
    // Get metaplasticity modulation for dopamine
    virtual float getMetaPlasticityMod() const { return 1.0f; }
    
    // Get motivational state influence for dopamine
    virtual float getMotivationalState() const { return 0.0f; }
    
    // Get curiosity integration weight for dopamine
    virtual float getCuriosityWeight() const { return 1.0f; }
    
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // ACh-specific effects
    void configure(const Config& config) override;
    void applyEffect(NeuralSystem& system) override;
    
    // Memory enhancement
    float getWorkingMemoryModulation() const override;
    float getMemoryRetrievalFacilitation() const;
    
    // Attention enhancement  
    float getAttentionalSelection() const override;
    float getSensoryProcessingGain() const;
    
    // Episodic memory
    float getEpisodicMemoryEncoding() const override;
    
    // Sleep consolidation
    float getSleepConsolidation() const;
    
    // Metaplasticity
    float getMetaPlasticityMod() const override;
    
    // Cognitive control
    float getCognitiveControl() const;
    
    // Working memory consolidation
    void applyWorkingMemoryEffects(NeuralWorkingMemory* workingMemory);
    
    // Attentional selection effects
    void applyAttentionEffects(AttentionalSelection* attention);
    
    // Episodic memory effects
    void applyMemoryConsolidationEffects(NeuralEpisodicMemory* episodicMemory);
    
    // Plasticity effects
    void applyPlasticityEffects(PlasticityRule* plasticity);
    
    // Sleep/rest cycle integration
    void applySleepConsolidationEffects(Brain* brain);
    
    // Cognitive development effects
    void applyDevelopmentEffects(DevelopmentSystem* development);

private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
