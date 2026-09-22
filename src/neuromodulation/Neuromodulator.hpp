#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Connect to attention system for modulation of attentional focus
    virtual void updateAttention(float currentLevel, float &attentionWeight) = 0;
    
    // Connect to working memory for modulation of memory maintenance
    virtual void updateWorkingMemory(float currentLevel, float &memoryStability) = 0;
    
    // Connect to developmental plasticity for ontogenetic changes
    virtual void updateDevelopmentalPlasticity(float currentLevel, float &developmentalFactor) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced with real biological dynamics and systems integration
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Real dopamine dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void processRewardContext(const std::string& context);
    
    // Get dopamine history for prediction error computation
    const std::vector<float>& getRecentActivity() const;
    
    // Get phasic response magnitude
    float getPhasicResponse() const;
    
    // Get tonic level
    float getTonicLevel() const;
    
    // Configure dopamine parameters
    void setBaseline(float baseline);
    void setPhasicGain(float gain);
    void setTemporalFilter(float tau);
    void setAttentionCoupling(float coupling);
    
    // Connection to attention and memory
    void updateAttentionWeight(float weight);
    void updateMemoryIntegration(float integration);
    
    // Neuromodulator interface implementations
    void updateAttention(float currentLevel, float &attentionWeight) override;
    void updateWorkingMemory(float currentLevel, float &memoryStability) override;
    void updateDevelopmentalPlasticity(float currentLevel, float &developmentalFactor) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
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
