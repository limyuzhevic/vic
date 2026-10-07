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
// ENHANCED - Phase 6 implementation with comprehensive neuromodulatory effects
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
    
    // Enhanced neuromodulatory effects
    float getExcitationModulation() const;
    float getMemoryConsolidationFactor() const;
    float getAttentionGain() const;
    
    // Apply effects to brain components
    void applyToNeuron(class Neuron* neuron, TimestepDuration dt);
    void applyToSynapse(class Synapse* synapse, float predictionError, TimestepDuration dt);
    void applyToMemory(class NeuralEpisodicMemory* memory, float reward, TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
