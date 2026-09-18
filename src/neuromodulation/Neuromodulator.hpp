#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class NeuromodulatorSystem {
public:
    NeuromodulatorSystem();
    ~NeuromodulatorSystem();
    
    // Initialize all neuromodulators with brain reference
    bool initialize(Brain* brain);
    
    // Get current level of a specific neuromodulator
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Update all neuromodulators
    void update(TimestepDuration dt);
    
    // Apply neuromodulation effects
    void applyToNeurons();
    void applyToPlasticity();
    
    // Reset neuromodulator state
    void reset();
    
    // Enable/disable individual neuromodulators
    void enableDopamine(bool enable);
    void enableCuriosity(bool enable);
    void enableNovelty(bool enable);
    void enablePredictionError(bool enable);
    
    // Set neuromodulator weights and parameters
    void setDopamineBaseline(float baseline);
    void setCuriosityWeight(float noveltyW, float errorW);
    void setNoveltyThreshold(float threshold);
    
    // Get neuromodulator system status
    bool isDopamineEnabled() const;
    bool isCuriosityEnabled() const;
    bool isNoveltyEnabled() const;
    bool isPredictionErrorEnabled() const;
    
private:
    // Individual neuromodulator instances
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    
    // Reference to brain for initialization
    Brain* brain_;
    
    // System-wide neuromodulation state
    bool dopamineEnabled_;
    bool curiosityEnabled_;
    bool noveltyEnabled_;
    bool predictionErrorEnabled_;
    
    // Global neuromodulation parameters
    float globalDopamineBaseline_;
    float curiosityNoveltyWeight_;
    float curiosityErrorWeight_;
    float noveltyThreshold_;
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
