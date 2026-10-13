#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "DA"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { 
        // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
        // PLACEHOLDER: Higher dopamine increases plasticity
        return 0.5f + 0.5f * pImpl->level;
    }
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};
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
    class Acetylcholine : public Neuromodulator {
    public:
        Acetylcholine();
        ~Acetylcholine() override;
        
        const char* getName() const override { return "ACh"; }
        float getLevel() const override { return pImpl->level; }
        void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
        float getPlasticityFactor() const override { 
            // ACh enhances attention and working memory
            return 0.8f + 0.4f * pImpl->level; 
        }
        void update(TimestepDuration dt) override;
        
        // Attention modulation
        void signalAttention(float attentionSignal);
        
        // Memory consolidation
        void triggerMemoryConsolidation();
        
        // Get attention level
        float getAttentionLevel() const { return pImpl->attentionLevel; }
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Norepinephrine: Arousal and vigilance
    class Norepinephrine : public Neuromodulator {
    public:
        Norepinephrine();
        ~Norepinephrine() override;
        
        const char* getName() const override { return "NE"; }
        float getLevel() const override { return pImpl->level; }
        void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
        float getPlasticityFactor() const override { 
            // NE enhances signal detection and focus
            return 0.6f + 0.3f * pImpl->level;
        }
        void update(TimestepDuration dt) override;
        
        // Arousal signaling
        void signalArousal(float arousalLevel);
        
        // Vigilance modulation
        void signalVigilance(float vigilance);
        
        // Get arousal level
        float getArousalLevel() const { return pImpl->arousalLevel; }
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Serotonin: Mood, impulsivity, and social behavior
    class Serotonin : public Neuromodulator {
    public:
        Serotonin();
        ~Serotonin() override;
        
        const char* getName() const override { return "5-HT"; }
        float getLevel() const override { return pImpl->level; }
        void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
        float getPlasticityFactor() const override { 
            // 5-HT modulates impulsivity and social learning
            return 0.7f + 0.2f * pImpl->level;
        }
        void update(TimestepDuration dt) override;
        
        // Mood signaling
        void signalMood(float moodLevel);
        
        // Social behavior modulation
        void signalSocialBehavior(float socialFactor);
        
        // Get mood level
        float getMoodLevel() const { return pImpl->moodLevel; }
        
    private:
        struct Impl;
        Impl* pImpl;
    };

} // namespace nlm
