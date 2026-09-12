#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

// Neuromodulator: Integrated neuromodulation system for the Phase 6 artificial brain
// Manages all neuromodulatory signals that influence neural plasticity, memory, attention, and learning
// This is the central hub for all neuromodulatory effects

class Neuromodulator {
public:
    Neuromodulator();
    ~Neuromodulator();
    
    // Get neuromodulation level (sum of all modulators)
    float getNeuromodulationLevel() const;

    // Get individual neuromodulator levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }

    // Update all neuromodulators for the next timestep
    void update(TimestepDuration dt);
    
    // Apply all neuromodulatory effects to the brain
    void applyToBrain(Brain* brain);
    
    // Initialize with brain reference and configure all subsystems
    void initialize(Brain* brain);
    
    // Reset state for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }

    // Configuration for enabling/disabling neuromodulatory subsystems
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

    // Motor command processing from brain activity
    MotorCommand decodeMotorCommand();
    
    // Process sensory percept for learning and prediction
    void processSensoryInput(const SensoryPercept& percept);
    
    // Apply reward prediction error to modulate dopamine and plasticity
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update developmental stage and maturation effects
    void updateDevelopment(double timestep);
    
    // Get current developmental stage (age-based plasticity modulation)
    DevelopmentalStage getDevelopmentalStage() const;

    // Get current cognitive/emotional state for decision making
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }

private:
    std::shared_ptr<Brain> brain_;

    // Motor neuron groups organized by action type
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;

    // Sensory neuron groups organized by modality
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;

    // Neuromodulatory signal levels
    float dopamineLevel_;     // Reward prediction error signal
    float noveltyLevel_;      // Novelty detection signal
    float curiosityLevel_;    // Exploration drive signal
    float predictionError_;   // Prediction error signal
    float expectedReward_;    // Expected value signal

    // Developmental state
    double developmentalAge_;  // Simulation time since initialization
    float plasticityModifier_; // Age-dependent plasticity scaling factor

    // Subsystem activation flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;

    // Previous sensory state for computing novelty and prediction error
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;  // Rate at which novelty signals decay
};

} // namespace nlm

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
