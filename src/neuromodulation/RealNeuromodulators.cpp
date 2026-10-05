#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    
    float getLevel() const override {
        return pImpl->level;
    }
    
    void setLevel(float level) override {
        pImpl->level = std::clamp(level, 0.0f, 2.0f);
    }
    
    float getPlasticityFactor() const override {
        // ACh modulates plasticity: high ACh increases LTP probability
        return 0.5f + 0.5f * std::min(1.0f, pImpl->level / 1.0f);
    }
    
    void update(TimestepDuration dt) override {
        // ACh decays toward baseline but can be activated by attention tasks
        float decayRate = pImpl->decayRate;
        pImpl->level = std::max(0.0f, pImpl->level - decayRate * static_cast<float>(dt));
        
        // Stochastic fluctuations for realism
        if (pImpl->rng) {
            float fluctuation = pImpl->rng->uniformReal(-0.001f, 0.001f) * static_cast<float>(dt);
            pImpl->level += fluctuation;
            pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
        }
    }
    
    // Boost ACh for attention tasks
    void signalAttention(float attentionLevel) {
        pImpl->level += attentionLevel * pImpl->attentionGain;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    }
    
private:
    struct Impl {
        float level;
        float decayRate;
        float attentionGain;
        class RandomGenerator* rng;
        
        Impl() 
            : level(0.1f)  // Baseline ACh level
            , decayRate(0.05f)
            , attentionGain(1.0f)
            , rng(nullptr) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    
    float getLevel() const override {
        return pImpl->level;
    }
    
    void setLevel(float level) override {
        pImpl->level = std::clamp(level, 0.0f, 3.0f);
    }
    
    float getPlasticityFactor() const override {
        // NE affects plasticity: moderate NE enhances LTP, high NE may promote LTD
        if (pImpl->level < 0.5f) {
            return 0.8f + 0.2f * pImpl->level;  // Enhancement
        } else {
            return 1.2f - 0.3f * (pImpl->level - 0.5f);  // Suppression
        }
    }
    
    void update(TimestepDuration dt) override {
        // NE dynamics: responding to stress, novelty, and arousal
        float decayRate = pImpl->decayRate;
        float baseline = pImpl->baseline;
        
        // Decay toward baseline
        pImpl->level = std::max(baseline, pImpl->level - decayRate * static_cast<float>(dt));
        
        // Random fluctuations for vigilance
        if (pImpl->rng) {
            float fluctuation = pImpl->rng->uniformReal(-0.002f, 0.002f) * static_cast<float>(dt);
            pImpl->level += fluctuation;
            pImpl->level = std::clamp(pImpl->level, 0.0f, 3.0f);
        }
        
        // Brief surges for novelty detection
        pImpl->noveltyTimer -= static_cast<float>(dt);
        if (pImpl->noveltyTimer <= 0.0f) {
            pImpl->level += pImpl->noveltySurge;
            pImpl->noveltyTimer = pImpl->noveltyInterval;
        }
    }
    
    // React to novel or surprising events
    void signalSurprise(float surpriseLevel) {
        pImpl->level += surpriseLevel * pImpl->surpriseGain;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 3.0f);
        
        // Update novelty timer
        pImpl->noveltyTimer = pImpl->noveltyInterval;
    }
    
private:
    struct Impl {
        float level;
        float baseline;
        float decayRate;
        float surpriseGain;
        float noveltySurge;
        float noveltyInterval;
        float noveltyTimer;
        class RandomGenerator* rng;
        
        Impl()
            : level(0.1f)  // Baseline NE level
            , baseline(0.1f)
            , decayRate(0.02f)
            , surpriseGain(0.5f)
            , noveltySurge(0.3f)
            , noveltyInterval(5.0f)
            , noveltyTimer(5.0f)
            , rng(nullptr) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    
    float getLevel() const override {
        return pImpl->level;
    }
    
    void setLevel(float level) override {
        pImpl->level = std::clamp(level, 0.0f, 2.5f);
    }
    
    float getPlasticityFactor() const override {
        // Serotonin affects plasticity: low 5-HT may increase impulsivity, moderate levels enhance learning
        float normalized = std::min(1.0f, pImpl->level / 1.5f);
        return 0.6f + 0.4f * std::pow(normalized, 0.5f);  // Sublinear relationship
    }
    
    void update(TimestepDuration dt) override {
        // Serotonin dynamics: responding to social feedback and rewards
        float decayRate = pImpl->decayRate;
        float baseline = pImpl->baseline;
        
        // Decay toward baseline
        pImpl->level = std::max(baseline, pImpl->level - decayRate * static_cast<float>(dt));
        
        // Stochastic fluctuations for mood variability
        if (pImpl->rng) {
            float fluctuation = pImpl->rng->uniformReal(-0.0015f, 0.0015f) * static_cast<float>(dt);
            pImpl->level += fluctuation;
            pImpl->level = std::clamp(pImpl->level, 0.0f, 2.5f);
        }
        
        // Social feedback responses
        pImpl->socialTimer -= static_cast<float>(dt);
        if (pImpl->socialTimer <= 0.0f) {
            pImpl->level += pImpl->socialResponse * pImpl->socialGain;
            pImpl->socialTimer = pImpl->socialInterval;
        }
    }
    
    // Process social feedback (acceptance/rejection)
    void processSocialFeedback(float feedback) {
        // Positive social feedback increases serotonin
        // Negative feedback decreases it
        pImpl->level += feedback * pImpl->socialGain;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 2.5f);
        
        // Update social timer
        pImpl->socialTimer = pImpl->socialInterval;
    }
    
private:
    struct Impl {
        float level;
        float baseline;
        float decayRate;
        float socialGain;
        float socialInterval;
        float socialTimer;
        float socialResponse;
        class RandomGenerator* rng;
        
        Impl()
            : level(0.15f)  // Baseline 5-HT level
            , baseline(0.15f)
            , decayRate(0.03f)
            , socialGain(0.3f)
            , socialInterval(3.0f)
            , socialTimer(3.0f)
            , socialResponse(0.2f)
            , rng(nullptr) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm