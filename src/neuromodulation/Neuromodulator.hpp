#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 3+: Implements real neuromodulation effects for learning and behavior
//
// Neuromodulatory signals are chemical messengers that modulate neural activity
// and plasticity across the entire brain. They play crucial roles in:
//
// 1. **Learning and Memory**: Dopamine for reward prediction error, ACh for attention
// 2. **Motivation and Emotion**: Serotonin, norepinephrine for affect and arousal
// 3. **Attention and Focus**: Acetylcholine modulates sensory processing
// 4. **Cognitive Control**: Integration of multiple signals for decision making
// 5. **Homeostasis**: Regulation of basic drives and needs
//
// Key neuromodulators and their functions:
// - Dopamine: Reward prediction error, motivation, action selection
// - Acetylcholine: Attention, working memory, sensory processing
// - Norepinephrine: Arousal, vigilance, stress response
// - Serotonin: Mood, impulsivity, social behavior, sleep
//
// The neuromodulation system:
// - Provides gain modulation to synaptic plasticity
// - Influences neural firing patterns and synchrony
// - Enables temporal abstraction of learning
// - Supports complex behavior through coordinated modulation
//
// Implementation notes:
// - Neuromodulators act globally, not just locally
// - Effects are often multiplicative rather than additive
// - Integration with prediction error systems is crucial
// - Neuromodulation enables credit assignment across time and space
//
// TODO PHASE 2: Implement real neuromodulation effects
// - Implement proper neuromodulator dynamics (release, reuptake, degradation)
// - Model concentration-dependent effects on synaptic plasticity
// - Implement interactions between different neuromodulators
// - Add synaptic targets and receptor mechanisms
// - Integrate with reward and prediction error systems
// - Implement neuromodulatory effects on neural excitability

class Neuromodulator {
public:
    // Virtual destructor for proper polymorphic cleanup
    virtual ~Neuromodulator() = default;
    
    // Get human-readable name of the neuromodulator
    // @return String identifier (e.g., "Dopamine", "Acetylcholine")
    virtual const char* getName() const = 0;
    
    // Get current concentration or level of neuromodulator
    // @return Current concentration (typically 0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    
    // Set current concentration or level
    // @param level New concentration value
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity rules
    // @return Multiplicative factor (typically 0.5 to 2.0)
    // TODO PHASE 2: Implement real modulation effects
    // Implement proper neuromodulatory influence:
    // 1. Model concentration-response curves
    // 2. Implement interaction with plasticity variables
    // 3. Add receptor saturation and inhibition
    // 4. Implement temporal dynamics of neuromodulator release
    // 5. Add baseline and phasic release mechanisms
    // 6. Integrate with prediction error signals
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on current inputs
    // @param dt Time step for updating neuromodulator dynamics
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    // Protected constructor to prevent direct instantiation
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Phase 3+: Implements real dopamine dynamics and reward prediction error
//
// Dopamine is critical for:
// - Reward prediction error computation
// - Reinforcement learning and action selection
// - Motivation and incentive salience
// - Working memory updating
//
// Key properties:
// - Phasic bursts on unexpected rewards (prediction error)
// - Tonic baseline representing expected value
// - Modulation of striatal circuits for action selection
// - Integration with prefrontal cortex for cognitive control

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "Dopamine"; }
    
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal receipt of a reward
    // @param reward Magnitude of received reward (typically 0.0 to 1.0)
    void signalReward(float reward);
    
    // Signal prediction error for reward
    // @param error Prediction error (positive = better than expected,
    //               negative = worse than expected)
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Phase 3+: Implements attention modulation and memory effects
//
// Acetylcholine is critical for:
// - Selective attention to salient stimuli
// - Working memory maintenance
// - Cognitive flexibility and task switching
// - Sensory processing enhancement
//
// Key properties:
// - Cholinergic modulation of cortical excitability
// - Interaction with memory systems (hippocampus, prefrontal cortex)
// - Role in feature binding and perceptual organization
// - Integration with arousal systems (noradrenaline)

class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// Phase 3+: Implements stress response and alertness modulation
//
// Norepinephrine is critical for:
// - Arousal and alertness
// - Stress response and fight-or-flight
// - Attentional focus and cognitive control
// - Memory consolidation during emotional events
//
// Key properties:
// - Interaction with locus coeruleus system
// - Modulation of thalamocortical circuits
// - Role in anxiety and fear responses
// - Integration with reward systems

class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// Phase 3+: Implements mood regulation and social behavior
//
// Serotonin is critical for:
// - Mood regulation and affect
// - Impulsivity and aggression control
// - Social behavior and bonding
// - Sleep and circadian rhythms
// - Pain perception and appetite
//
// Key properties:
// - Complex receptor dynamics (multiple receptor subtypes)
// - Interaction with reward and learning systems
// - Role in personality and decision making
// - Integration with stress response systems

class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
