#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced for attention and planning support

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
    
protected:
    Neuromodulator() = default;
};

// NeuromodulatorManager: Manages all neuromodulators and their integration
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update all neuromodulators
    void updateAll(float novelty, float predictionError, float reward,
                   float workingMemoryLoad, TimestepDuration dt);
    
    // Get neuromodulator by type
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    Norepinephrine* getNorepinephrine() { return norepinephrine_.get(); }
    AcetylCholine* getAcetylcholine() { return acetylcholine_.get(); }
    Serotonin* getSerotonin() { return serotonin_.get(); }
    
    // Get integrated modulation factors
    float getAttentionEnhancement() const { return attentionEnhancement_; }
    float getWorkingMemoryEnhancement() const { return workingMemoryEnhancement_; }
    float getLearningEnhancement() const { return learningEnhancement_; }
    float getDecisionStability() const { return decisionStability_; }
    float getExplorationDrive() const { return explorationDrive_; }
    
    // Connect neuromodulators to cognitive systems
    void connectToAttention(AttentionalSelection* attention);
    void connectToNeuralPlanner(NeuralPlanner* planner);
    
    // Reset all neuromodulators
    void reset();
    
    // Get total neuromodulatory load
    float getTotalModulation() const { return totalModulation_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Individual neuromodulators
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Norepinephrine> norepinephrine_;
    std::unique_ptr<AcetylCholine> acetylcholine_;
    std::unique_ptr<Serotonin> serotonin_;
    
    // Integrated modulation factors
    float attentionEnhancement_;
    float workingMemoryEnhancement_;
    float learningEnhancement_;
    float decisionStability_;
    float explorationDrive_;
    float totalModulation_;
    
    // Cognitive system references
    AttentionalSelection* attention_;
    NeuralPlanner* planner_;
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

// AcetylCholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class AcetylCholine : public Neuromodulator {
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