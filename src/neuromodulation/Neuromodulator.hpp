#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
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
    
    // Effects on other brain systems
    class AttentionalSelection;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    class NeuralPlanner;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    
    virtual void affectAttention(AttentionalSelection* attention) = 0;
    virtual void affectMemory(NeuralWorkingMemory* workingMemory, 
                           NeuralEpisodicMemory* episodicMemory,
                           NeuralAssociativeMemory* associativeMemory) = 0;
    virtual void affectPlasticity(STDP* stdp, Hebbian* hebbian,
                               StructuralPlasticity* structural) = 0;
    virtual void affectBehavior(NeuralPlanner* planner) = 0;
    
    // Specialized neuromodulator operations
    virtual void signalReward(float reward) {}
    virtual void signalRewardPredictionError(float error) {}
    virtual void signalNovelty(float novelty) {}
    virtual void signalArousal(float arousal) {}
    virtual void signalSalience(float salience) {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Neuromodulator effects on brain systems
    void affectAttention(class AttentionalSelection* attention) override;
    void affectMemory(class NeuralWorkingMemory* workingMemory,
                      class NeuralEpisodicMemory* episodicMemory,
                      class NeuralAssociativeMemory* associativeMemory) override;
    void affectPlasticity(class STDP* stdp, class Hebbian* hebbian,
                         class StructuralPlasticity* structural) override;
    void affectBehavior(class NeuralPlanner* planner) override;
    
    // Reward signaling
    void signalReward(float reward) override;
    void signalRewardPredictionError(float error) override;
    
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
    
    // Neuromodulator effects on brain systems
    void affectAttention(class AttentionalSelection* attention) override;
    void affectMemory(class NeuralWorkingMemory* workingMemory,
                      class NeuralEpisodicMemory* episodicMemory,
                      class NeuralAssociativeMemory* associativeMemory) override;
    void affectPlasticity(class STDP* stdp, class Hebbian* hebbian,
                         class StructuralPlasticity* structural) override;
    void affectBehavior(class NeuralPlanner* planner) override;
    
    // Specialized functions
    void signalNovelty(float novelty) override;
    void signalSalience(float salience) override;
    
private:
    struct Impl;
    Impl* pImpl;
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
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Neuromodulator effects on brain systems
    void affectAttention(class AttentionalSelection* attention) override;
    void affectMemory(class NeuralWorkingMemory* workingMemory,
                      class NeuralEpisodicMemory* episodicMemory,
                      class NeuralAssociativeMemory* associativeMemory) override;
    void affectPlasticity(class STDP* stdp, class Hebbian* hebbian,
                         class StructuralPlasticity* structural) override;
    void affectBehavior(class NeuralPlanner* planner) override;
    
    // Specialized functions
    void signalSalience(float salience) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
