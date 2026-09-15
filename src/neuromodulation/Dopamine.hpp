#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declarations
class Brain;
class NeuralAttentionalSelection;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class PredictionSystem;
class MotorActionSelection;
class DevelopmentSystem;

// Dopamine: Reward and reinforcement learning signal
// Extends basic neuromodulation to affect multiple cognitive systems

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhanced dopamine effects on cognitive systems
    void applyAttentionEffect(NeuralAttentionalSelection* attention, float deltaTime);
    void applyWorkingMemoryEffect(NeuralWorkingMemory* workingMemory, float deltaTime);
    void applyEpisodicMemoryEffect(NeuralEpisodicMemory* episodicMemory, float deltaTime);
    void applyActionSelectionEffect(MotorActionSelection* actionSelection, float deltaTime);
    void applyPredictionSystemEffect(PredictionSystem* predictionSystem, float deltaTime);
    void applyDevelopmentSystemEffect(DevelopmentSystem* developmentSystem, float deltaTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
