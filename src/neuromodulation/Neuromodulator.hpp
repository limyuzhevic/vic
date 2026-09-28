#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

/**
 * @class Neuromodulator
 * @brief Abstract base class for neuromodulatory signals in the brain.
 * 
 * Neuromodulators are chemical signals that modulate neural activity and plasticity
 * throughout the brain. They play key roles in reward learning, motivation, attention,
 * and other cognitive functions. This abstract class provides the interface for
 * all neuromodulatory systems.
 * 
 * @note Phase 1 implementation provides placeholder functionality.
 * Phase 2 will implement real neuromodulation effects based on biological research.
 */
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the name of this neuromodulator
     * @return String identifier for the neuromodulator (e.g., "DA" for dopamine)
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration/level of the neuromodulator
     * @return Current level between 0.0 and 1.0 (normalized)
     */
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Apply neuromodulatory effect to synaptic plasticity
     * @return Plasticity factor modifier (1.0 = no change, >1.0 = enhanced plasticity, <1.0 = reduced plasticity)
     * 
     * @note Phase 2: Implement real neuromodulation effects based on biological research
     *       Different neuromodulators have different effects on plasticity mechanisms
     *       (e.g., dopamine enhances reward prediction error learning, acetylcholine facilitates memory)
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state for the current timestep
     * @param dt Timestep duration in seconds
     * 
     * @note Phase 2: Implement real neuromodulator dynamics including:
     *       - Phasic responses to specific events (e.g., reward prediction errors)
     *       - Tonic background activity
     *       - Decay dynamics
     *       - Interaction with other neuromodulators
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

/**
 * @class Dopamine
 * @brief Dopamine neuromodulator - reward and reinforcement learning signal
 * 
 * Dopamine plays a central role in reward prediction error signaling, reinforcement
 * learning, and motivation. It modulates plasticity in reward-related brain circuits.
 * 
 * @note Phase 1 implementation provides placeholder functionality.
 * Phase 2 will implement real dopamine dynamics including phasic bursts and tonic activity.
 */
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal reward receipt
     * @param reward Reward prediction error signal (positive for better-than-expected outcomes)
     * 
     * @note Phase 2: Implement real dopamine dynamics:
     *       - Phasic bursts in response to unexpected rewards
     *       - Tonic activity reflecting reward prediction
     *       - Integration with prediction error signals
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal reward prediction error
     * @param error Reward prediction error (positive if reward > expected)
     * 
     * @note Phase 2: Implement real reward prediction error dynamics:
     *       - TD (temporal difference) learning model
     *       - Interaction with learning systems
     */
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class Acetylcholine
 * @brief Acetylcholine neuromodulator - attention and memory consolidation
 * 
 * Acetylcholine modulates attention, arousal, and memory formation. It enhances
 * the signal-to-noise ratio in neural circuits and facilitates synaptic plasticity
 * during learning.
 */
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

/**
 * @class Norepinephrine
 * @brief Norepinephrine neuromodulator - arousal and vigilance
 * 
 * Norepinephrine modulates arousal, vigilance, and attention. It enhances signal
 * detection and strengthens memory encoding during emotionally salient events.
 */
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

/**
 * @class Serotonin
 * @brief Serotonin neuromodulator - mood, impulsivity, and social behavior
 * 
 * Serotonin regulates mood, impulsivity, aggression, and social behavior. It also
 * influences learning and memory processes, particularly for aversive events.
 */
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
