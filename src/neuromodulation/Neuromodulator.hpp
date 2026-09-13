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
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced implementation with realistic biological dynamics
class Dopamine : public Neuromodulator {
public:
    Dopamine() : pImpl(new Impl) {}
    ~Dopamine() override = default;
    
    const char* getName() const override { return "DA"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { 
        pImpl->level = std::clamp(level, 0.0f, 2.0f); 
    }
    float getPlasticityFactor() const override { 
        // Dopamine modulates plasticity in a biphasic manner
        // Low levels enhance LTP, high levels promote LTD
        if (pImpl->level < 0.5f) {
            return 1.0f + pImpl->level * 2.0f;  // Range 1.0 to 2.0
        } else {
            return 2.0f - pImpl->level * 2.0f;  // Range 1.0 down to 0.0
        }
    }
    void update(TimestepDuration dt) override {
        // Dopamine dynamics with reward prediction error updates
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
        
        // RPE-driven updates
        if (pImpl->rpeInput != 0.0f) {
            pImpl->level = std::min(2.0f, pImpl->level + pImpl->rpeInput * pImpl->releaseRate);
            pImpl->rpeInput = 0.0f;
        }
    }
    
    // Dopamine-specific functions
    void signalReward(float reward) {
        // Phasic dopamine response to unexpected rewards
        if (reward > 0.0f) {
            pImpl->rpeInput += reward * pImpl->burstRate;
        }
    }
    
    void signalRewardPredictionError(float error) {
        // Core dopamine function: reward prediction error
        pImpl->rpeInput += error * pImpl->rpeGain;
        
        // Also signal to other systems
        pImpl->predictionErrorInput = std::max(0.0f, error) * 0.5f;
    }
    
    void signalCuriosity(float curiosity) {
        // Dopamine also responds to curiosity-driven exploration
        pImpl->rpeInput += curiosity * 0.3f;
    }
    
    float getPredictionErrorInput() const {
        return pImpl->predictionErrorInput;
    }
    
private:
    struct Impl {
        float level;
        float baseline;
        float decayRate;
        float releaseRate;
        float rpeInput;
        float predictionErrorInput;
        float burstRate;
        float rpeGain;
        
        Impl() 
            : level(0.1f)  // Low baseline dopamine
            , baseline(0.1f)
            , decayRate(0.05f)
            , releaseRate(1.0f)
            , rpeInput(0.0f)
            , predictionErrorInput(0.0f)
            , burstRate(5.0f)
            , rpeGain(3.0f) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced implementation with realistic biological dynamics
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine() : pImpl(new Impl) {}
    ~Acetylcholine() override = default;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { 
        pImpl->level = std::clamp(level, 0.0f, 2.0f); 
    }
    float getPlasticityFactor() const override { 
        // Acetylcholine enhances working memory encoding and consolidation
        return 0.8f + pImpl->level * 0.2f;  // Range 0.8 to 1.0
    }
    void update(TimestepDuration dt) override {
        // Acetylcholine dynamics with learning-dependent release
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
        
        // Stimuli can trigger ACh release
        if (pImpl->learningInput > 0) {
            pImpl->level = std::min(2.0f, pImpl->level + pImpl->learningInput * pImpl->releaseRate);
            pImpl->learningInput = 0.0f;
        }
    }
    
    // Acetylcholine-specific functions
    void signalLearning(float predictionError) {
        // ACh release is tied to prediction error - signals unexpected outcomes
        if (std::abs(predictionError) > 0.1f) {
            pImpl->learningInput += std::abs(predictionError) * 2.0f;
        }
    }
    
    void enhanceWorkingMemory() {
        // High ACh improves working memory capacity and fidelity
        if (pImpl->level > 0.3f) {
            pImpl->memoryEfficiency = 1.0f + pImpl->level * 0.4f;
        }
    }
    
    float getMemoryEfficiency() const {
        return pImpl->memoryEfficiency;
    }
    
private:
    struct Impl {
        float level;
        float decayRate;
        float releaseRate;
        float learningInput;
        float memoryEfficiency;
        
        Impl() 
            : level(0.2f)  // Moderate baseline acetylcholine
            , decayRate(0.1f)
            , releaseRate(1.5f)
            , learningInput(0.0f)
            , memoryEfficiency(1.0f) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced implementation with realistic biological dynamics
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine() : pImpl(new Impl) {}
    ~Norepinephrine() override = default;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { 
        pImpl->level = std::clamp(level, 0.0f, 2.0f); 
    }
    float getPlasticityFactor() const override { 
        // Norepinephrine modulates plasticity optimally - peak at moderate levels
        float mod = pImpl->level;
        if (mod > 1.0f) mod = 1.0f + (mod - 1.0f) * 0.2f;  // Diminishing returns
        return 0.5f + mod * 0.5f;  // Range 0.5 to 1.0
    }
    void update(TimestepDuration dt) override {
        // Norepinephrine dynamics with arousal decay
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
        
        // Stimuli can increase arousal
        if (pImpl->arousalInput > 0) {
            pImpl->level = std::min(2.0f, pImpl->level + pImpl->arousalInput * pImpl->releaseRate);
            pImpl->arousalInput = 0.0f;
        }
    }
    
    // Norepinephrine-specific functions
    void signalArousal(float intensity) {
        pImpl->arousalInput += intensity;
    }
    
    void signalStress(float severity) {
        // Acute stress response
        pImpl->level = std::min(2.0f, pImpl->level + severity * 0.5f);
    }
    
    void enhanceAttention() {
        // High norepinephrine improves focused attention
        if (pImpl->level > 0.5f) {
            pImpl->attentionBoost = 1.0f + pImpl->level * 0.3f;
        }
    }
    
    float getAttentionBoost() const {
        return pImpl->attentionBoost;
    }
    
private:
    struct Impl {
        float level;
        float decayRate;
        float releaseRate;
        float arousalInput;
        float attentionBoost;
        
        Impl() 
            : level(0.1f)  // Low baseline norepinephrine
            , decayRate(0.08f)
            , releaseRate(2.0f)
            , arousalInput(0.0f)
            , attentionBoost(1.0f) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced implementation with realistic biological dynamics
class Serotonin : public Neuromodulator {
public:
    Serotonin() : pImpl(new Impl) {}
    ~Serotonin() override = default;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { 
        pImpl->level = std::clamp(level, 0.0f, 2.0f); 
    }
    float getPlasticityFactor() const override { 
        // Serotonin modulates plasticity inversely - high 5-HT reduces plasticity
        return std::max(0.1f, 1.0f - pImpl->level * 0.3f); 
    }
    void update(TimestepDuration dt) override {
        // Serotonin decay and baseline regulation
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
        
        // Homeostatic regulation
        if (pImpl->level < pImpl->baseline) {
            pImpl->level += pImpl->homeostaticRate * static_cast<float>(dt);
        }
    }
    
    // Serotonin-specific functions
    void signalSocialInteraction(float intensity) {
        pImpl->level = std::min(2.0f, pImpl->level + intensity * 0.3f);
    }
    
    void signalNegativeOutcome(float severity) {
        pImpl->level = std::min(2.0f, pImpl->level + severity * 0.2f);
    }
    
    void enhanceMemoryConsolidation() {
        // High serotonin promotes memory consolidation
        if (pImpl->level > 0.5f) {
            pImpl->memoryBoost = 1.0f + pImpl->level * 0.5f;
        }
    }
    
    float getMemoryModulator() const {
        return pImpl->memoryBoost;
    }
    
private:
    struct Impl {
        float level;
        float baseline;
        float decayRate;
        float homeostaticRate;
        float memoryBoost;
        
        Impl() 
            : level(0.05f)  // Low baseline serotonin
            , baseline(0.05f)
            , decayRate(0.05f)
            , homeostaticRate(0.01f)
            , memoryBoost(1.0f) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
