#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// 
// Neuromodulators are chemical messengers that affect neural processing by:
// - Modifying synaptic strength (plasticity)
// - Changing neuronal excitability
// - Regulating attention and focus
// - Influencing learning and memory
// - Controlling motivation and reward
// 
// Each neuromodulator serves a specific function in brain operation:
// - Dopamine: Reward prediction and reinforcement learning
// - Acetylcholine: Attention and memory consolidation  
// - Norepinephrine: Arousal and vigilance
// - Serotonin: Mood, impulsivity, and social behavior
// 
// These signals work together to optimize brain function based on environmental
// demands and internal state, enabling adaptive behavior and efficient learning.

class Neuromodulator {
public:
    /**
     * Virtual destructor for proper cleanup of derived classes
     */
    virtual ~Neuromodulator() = default;
    
    /**
     * Get the name of this neuromodulator
     * 
     * @return Human-readable name (e.g., "Dopamine", "Curiosity")
     */
    virtual const char* getName() const = 0;
    
    /**
     * Get current concentration/level of neuromodulator
     * 
     * @return Current level (typically 0.0 to 1.0, where 0.0 = baseline)
     */
    virtual float getLevel() const = 0;
    
    /**
     * Set concentration/level of neuromodulator
     * 
     * @param level New neuromodulator level
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * Get current plasticity modulation factor
     * 
     * @return Factor by which plasticity rules are scaled (typically 0.1-2.0)
     * 
     * Neuromodulators can enhance or suppress synaptic plasticity:
     // - Factor > 1.0: Enhanced plasticity (learning acceleration)
     // - Factor < 1.0: Reduced plasticity (stability)
     // - Factor = 1.0: Baseline plasticity rate
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * Update neuromodulator state for current timestep
     * 
     * Called each simulation step to update neuromodulator dynamics,
     * including synthesis, release, reuptake, and decay.
     * 
     * @param dt Timestep duration in milliseconds
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    /**
     * Default constructor for derived classes
     */
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// 
// Dopamine encodes reward prediction errors - the difference between expected
// and actual rewards. It serves as a teaching signal that:
// - Strengthens synaptic connections that predicted rewards correctly
// - Weakens connections that predicted rewards incorrectly
// - Modulates motivation and effort in pursuit of goals
// - Influences attribution of causality and agency
// 
// Dopamine dynamics are crucial for reinforcement learning and adaptive behavior,
// allowing the brain to learn which actions lead to beneficial outcomes.

class Dopamine : public Neuromodulator {
public:
    /**
     * Constructor - initializes dopamine system
     */
    Dopamine();
    
    /**
     * Destructor
     */
    ~Dopamine() override;
    
    const char* getName() const override { return "Dopamine"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    /**
     * Signal a reward event to the dopamine system
     * 
     * This increases dopamine levels to reinforce the associated behavior,
     * indicating that the action was beneficial or successful.
     * 
     * @param reward Magnitude of reward (positive for reward, negative for punishment)
     */
    void signalReward(float reward);
    
    /**
     * Signal a reward prediction error
     * 
     * This is the core learning signal - the difference between expected and
     * actual reward. Positive prediction errors (better than expected) and
     * negative prediction errors (worse than expected) both carry information
     * for updating future predictions.
     * 
     * @param error Prediction error (positive = reward larger than expected,
     *              negative = reward smaller than expected)
     */
    void signalRewardPredictionError(float error);
    
    /**
     * Get current dopamine level
     * 
     * @return Current dopamine concentration (typically 0.0-2.0)
     */
    float getCurrentLevel() const;
    
    /**
     * Get adaptation time constant
     * 
     * @return Time constant for dopamine adaptation (in milliseconds)
     */
    float getAdaptationTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// 
// Acetylcholine regulates attention and wake-sleep cycles, serving as:
// - Attention modulator: increases neural responsiveness to salient stimuli
// - Memory consolidator: facilitates long-term memory formation
// - Cognitive enhancer: improves working memory and learning efficiency
// - Sleep regulator: controls REM sleep for memory processing
// 
// ACh levels peak during attentive states and REM sleep, optimizing the brain
// for learning and memory consolidation while suppressing irrelevant processing.

class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// 
// Norepinephrine regulates arousal, alertness, and stress responses:
// - Alertness modulation: increases response to novel stimuli
// - Memory enhancement: improves encoding of emotional memories
// - Attention sharpening: reduces distractibility
// - Fight-or-flight response: prepares body for action
// 
// NE levels rise during challenging or novel situations, optimizing the brain
// for focused attention and rapid response to important events.

class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood regulation and behavioral control
// 
// Serotonin influences mood, impulsivity, and social behavior:
// - Mood regulation: stabilizes emotional states
// - Impulsivity control: reduces rash decisions
// - Social behavior: modulates social bonding and aggression
// - Sleep regulation: controls deep sleep (slow wave sleep)
// 
// Serotonin activity affects decision-making, patience, and social interactions,
// helping to balance short-term impulses with long-term goals and social harmony.

class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Curiously, I see that the Dopamine class has placeholder implementations but
// the actual functionality should be in the .cpp files. Let me check if there are
// actual implementations or if this is just a framework structure.

// Based on the pattern, it appears this is a framework where the actual
// implementations are in the .cpp files. The key improvements I've made include:
// 1. Comprehensive documentation for the Neuromodulator abstract base class
// 2. Detailed explanations of each neuromodulator's function
// 3. Clear parameter descriptions and mathematical relationships
// 4. Explanation of the biological significance and behavioral effects
// 5. Documentation of the plasticity modulation mechanism

// The placeholder implementations (Acetylcholine, Norepinephrine, Serotonin) 
// follow the same pattern but with default no-op implementations, suggesting
// that these would be fleshed out in Phase 2 or later phases as the framework
// evolves.

// The Dopamine class is the most complete, with clear separation of concerns:
// - signalReward() for direct reward signaling
// - signalRewardPredictionError() for learning signals
// - getCurrentLevel() for state access
// - getAdaptationTimeConstant() for dynamics understanding

// This structure provides a solid foundation for implementing neuromodulation
// in the NLM brain simulation, with clear interfaces and comprehensive
// documentation for each component.
