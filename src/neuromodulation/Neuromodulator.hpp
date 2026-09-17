#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced implementation with better error handling and statistics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor to multiply synaptic plasticity by
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator statistics for debugging
    virtual std::string getStatistics() const = 0;
    
    // Reset neuromodulator to initial state
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced implementation with proper dopamine dynamics
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::string getStatistics() const override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get dopamine history for analysis
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Configuration
    void setBaseline(float baseline);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    
    float getBaseline() const;
    float getDecayRate() const;
    float getReleaseRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced implementation with attention modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::string getStatistics() const override;
    void reset() override;
    
    // Attention control
    void setAttentionGain(float gain);
    void setMemoryConsolidation(bool enable);
    
    float getAttentionGain() const;
    bool isMemoryConsolidationEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced implementation with stress and arousal regulation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::string getStatistics() const override;
    void reset() override;
    
    // Arousal control
    void setArousalThreshold(float threshold);
    void setStressResponse(bool enable);
    
    float getArousalThreshold() const;
    bool isStressResponseEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced implementation with social behavior modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::string getStatistics() const override;
    void reset() override;
    
    // Social behavior control
    void setSocialBonding(bool enable);
    void setImpulsivityControl(bool enable);
    
    bool isSocialBondingEnabled() const;
    bool isImpulsivityControlEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
