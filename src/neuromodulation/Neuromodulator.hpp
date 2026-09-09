#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PHASE 6: Complete implementation of brain neuromodulation systems
//
// Neuromodulators are diffuse chemical signals that:
// - Regulate neural excitability and plasticity globally
// - Encode reward prediction errors (dopamine)
// - Drive exploration vs exploitation (curiosity)
// - Detect novelty and unexpected events (novelty)
// - Provide error signals for prediction (prediction error)

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
    
    // Reset for new episode
    virtual void reset() {
        // Default implementation - override as needed
    }
    
    // Get modulation statistics
    virtual float getHistoryValue() const { return getLevel(); }
    
    // Integration with brain systems
    virtual void setBrain(Brain* brain) {
        // Default implementation - override as needed
        brain_ = brain;
    }
    
protected:
    Neuromodulator() = default;
    Neuromodulator(const Neuromodulator&) = default;
    Neuromodulator& operator=(const Neuromodulator&) = default;
    
    Brain* brain_ = nullptr;
};

// Dopamine: Reward and reinforcement learning signal
// Implements reward prediction error signaling (Schultz et al., 1997)
// and reward-modulated plasticity (Howard & Margolis, 2019)
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Dopamine-specific parameters
    float getBaseline() const;
    void setBaseline(float baseline);
    float getPeak() const;
    void setPeak(float peak);
    float getTimeConstant() const;
    void setTimeConstant(float tau);
    float getAdaptationRate() const;
    void setAdaptationRate(float rate);
    float getSensitivity() const;
    void setSensitivity(float sensitivity);
    
    // Neuromodulation effects
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Internal state
    float getExpectedReward() const;
    float getRewardPredictionError() const;
    float getNeuralAdaptation() const;
    
    // Reset for new episode
    void reset() override;
    
    // History and statistics
    float getHistoryValue() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements attention-based modulation of neural processing
// (Hasselmo, 1999; Hasselmo & Sarter, 2011)
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention parameters
    void setAttentionStrength(float strength);
    float getAttentionStrength() const;
    void setReverberationRate(float rate);
    float getReverberationRate() const;
    
    // Memory modulation
    void signalNovelty(float novelty);
    void signalAttentionDemand(float demand);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements locus coeruleus-neurons (LC) signaling (Aston-Jones & Bloom, 1990)
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal parameters
    void setArousalLevel(float level);
    float getArousalLevel() const;
    void setAttentionGain(float gain);
    float getAttentionGain() const;
    
    // LC-NE dynamics
    void signalWakeUp(float intensity);
    void signalSleep(float intensity);
    void signalError(float error);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements dorsal/median raphe signaling (Hespos & Maroney, 2023)
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonin parameters
    void setTonicLevel(float level);
    float getTonicLevel() const;
    void setPhasicResponse(float response);
    float getPhasicResponse() const;
    
    // Behavioral modulation
    void signalSocialStatus(float status);
    void signalSocialThreat(float threat);
    void signalPersistentState(float state);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
