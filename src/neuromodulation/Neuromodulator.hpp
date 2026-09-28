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
    
    // Checkpointing support
    virtual bool checkpointData(std::vector<uint8_t>& data) const { return false; }
    virtual bool restoreFromCheckpoint(const std::vector<uint8_t>& data) { return false; }
    
    // Get neuromodulator statistics
    virtual uint64_t getSpikeCount() const { return 0; }
    virtual float getAverageLevel() const { return 0.0f; }
    virtual float getMinLevel() const { return 0.0f; }
    virtual float getMaxLevel() const { return 0.0f; }
    
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
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal processing
    void signalAttentionalSelection(float salience);
    void signalWorkingMemoryLoad(float load);
    void signalEpisodicEncoding(float coherence);
    void signalSleepConsolidation(float sleepQuality);
    void modulatePlasticity(float& ltpWeight, float& ltdWeight) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal processing
    void signalArousal(float arousal);
    void signalVigilance(float vigilance);
    void signalThreatResponse(float threatLevel);
    void signalSleepArchitecture(float slowWaveRatio);
    void modulateAttention(float& excitationGain, float& inhibitionGain) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal processing
    void signalActionSelection(float actionValue);
    void signalPatience(float waitingTime);
    void signalSocialLearning(float socialReward);
    void signalExplorationExploitation(float explorationBias);
    void signalRewardPredictionError(float error);
    void modulateActionSelection(float& actionValue, float baseValue) const;
    void modulateExploration(float& explorationRate, float baseRate) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
