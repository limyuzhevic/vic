#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Now implements real biological dynamics and integration

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (normalized 0.0-1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns a modulation factor (1.0 = normal, <1.0 = suppressed, >1.0 = enhanced)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on inputs and time
    virtual void update(TimestepDuration dt, const std::unordered_map<std::string, float>& inputs) = 0;
    
    // Get historical trace for analysis
    virtual const std::vector<float>& getHistory() const = 0;
    virtual void clearHistory() = 0;
    
    // Reset to baseline state
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements reward prediction error (RPE) based on temporal difference learning

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const std::unordered_map<std::string, float>& inputs) override;
    
    // Reward signaling
    void signalReward(float reward, float rewardPrediction);
    void signalRewardPredictionError(float error);
    
    // Dopamine dynamics
    void setBaseline(float baseline);
    void setPeakLevel(float peak);
    void setDecayRate(float decayRate);
    void setReleaseRate(float releaseRate);
    void setTimeConstant(float tau);
    
    // History management
    const std::vector<float>& getHistory() const override;
    void clearHistory() override;
    
    // Reset
    void reset() override;
    
    // Get prediction error components
    float getPredictionError() const;
    float getTemporalDifference() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements attentional modulation and working memory enhancement

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const std::unordered_map<std::string, float>& inputs) override;
    
    // Attention modulation
    void signalAttention(float attentionSignal);
    void signalWorkingMemoryLoad(float load);
    void signalPerceptualInput(float inputStrength);
    
    // History management
    const std::vector<float>& getHistory() const override;
    void clearHistory() override;
    
    // Reset
    void reset() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements locus coeruleus firing rate dynamics

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const std::unordered_map<std::string, float>& inputs) override;
    
    // Arousal signaling
    void signalNovelty(float novelty);
    void signalThreateningStimulus(float threatLevel);
    void signalUnexpectedEvent(float surprise);
    
    // History management
    const std::vector<float>& getHistory() const override;
    void clearHistory() override;
    
    // Reset
    void reset() override;
    
    // Get firing rate information
    float getFiringRate() const;
    float getReactivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements raphe nucleus dynamics and behavioral modulation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const std::unordered_map<std::string, float>& inputs) override;
    
    // Mood modulation
    void signalSocialContact(float socialScore);
    void signalPhysicalNeed(float needLevel);
    void signalPunishment(float punishmentStrength);
    
    // History management
    const std::vector<float>& getHistory() const override;
    void clearHistory() override;
    
    // Reset
    void reset() override;
    
    // Get behavioral modulation parameters
    float getImpulsivity() const;
    float getRiskTaking() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
