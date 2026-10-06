// Copyright (c) 2024+ NLM Project
// All rights reserved.
// 
// This file is part of the Neural Learning Machine (NLM) project.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file Neuromodulator.hpp
 * @brief Neuromodulatory signals for learning and behavioral control
 * 
 * This file implements the Neuromodulator class and its specialized subclasses,
 * providing a comprehensive neuromodulation system for the NLM project. Neuromodulators
 * are chemical signals that modulate neural activity and plasticity, playing crucial
 * roles in:
 * - Reward prediction error processing
 * - Attention and salience detection
 * - Arousal and vigilance control
 * - Memory consolidation
 * - Learning rate adjustment
 * - Action selection and decision-making
 * - Behavioral adaptation
 * 
 * The neuromodulation system is designed with the following key principles:
 * - Biophysical realism with concentration dynamics
 * - Multiple neuromodulator types with distinct functions
 * - Integration with neural plasticity mechanisms
 * - State-dependent modulation
 * - Temporal dynamics and adaptation
 * - Neuromodulator interactions and cross-talk
 * - Efficient computation and resource management
 * - Thread-safe operation for parallel simulation
 * - Integration with other NLM systems
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see src/brain/Neuron.hpp - Neuron modulation by neuromodulators
 * @see src/plasticity/STDP.hpp - Neuromodulatory control of STDP
 * @see src/plasticity/Hebbian.hpp - Neuromodulatory control of Hebbian learning
 * @see src/memory/WorkingMemory.hpp - Neuromodulatory influence on working memory
 * @see src/cognition/NeuralPlanner.hpp - Neuromodulatory influence on planning
 * @see src/core/Types/Types.hpp - Neuromodulator type definitions
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 *       The neuromodulator system supports both biological realism and computational efficiency.
 */

#pragma once

// NLM filename guard
#ifndef NLM_NEUROMODULATOR_HPP
#define NLM_NEUROMODULATOR_HPP

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../memory/WorkingMemory.hpp"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <optional>
#include <map>
#include <unordered_map>
#include <memory>

namespace nlm {

/**
 * @struct NeuromodulatorParameters
 * @brief Configuration parameters for neuromodulators
 * 
 * This structure contains all the parameters required for neuromodulator behavior,
 * including concentration dynamics, modulation effects, and interaction parameters.
 * The parameters are organized to support:
 * - Biophysical modeling of neuromodulator dynamics
 * - State-dependent modulation
 * - Interaction with neural systems
 * - Temporal adaptation
 * - Resource management
 * 
 * Parameter Values (typical biological ranges):
 * - baseline_level: [0.0, 1.0] (baseline concentration)
 * - max_level: [1.0, 10.0] (maximum concentration)
 * - decay_rate: [0.001, 0.1] (concentration decay rate)
 * - rise_rate: [0.001, 0.1] (concentration rise rate)
 * - effect_strength: [0.0, 2.0] (modulation effect strength)
 * - adaptation_rate: [0.001, 0.1] (adaptation rate)
 * - noise_level: [0.0, 1.0] (biological noise level)
 */
struct NeuromodulatorParameters {
    float baseline_level;      ///< Baseline concentration level
    float max_level;           ///< Maximum concentration level
    float decay_rate;          ///< Concentration decay rate
    float rise_rate;           ///< Concentration rise rate
    float effect_strength;     ///< Modulation effect strength
    float adaptation_rate;     ///< Adaptation rate for concentration dynamics
    float noise_level;         ///< Biological noise level
    float reset_rate;          ///< Rate at which concentration resets to baseline
    
    /**
     * @brief Default constructor with biologically plausible parameters
     * 
     * Creates neuromodulator parameters with default values:
     * - Baseline level: 0.1
     * - Maximum level: 1.0
     * - Decay rate: 0.01
     * - Rise rate: 0.1
     * - Effect strength: 1.0
     * - Adaptation rate: 0.01
     * - Noise level: 0.05
     * - Reset rate: 0.1
     */
    NeuromodulatorParameters()
        : baseline_level(0.1f)
        , max_level(1.0f)
        , decay_rate(0.01f)
        , rise_rate(0.1f)
        , effect_strength(1.0f)
        , adaptation_rate(0.01f)
        , noise_level(0.05f)
        , reset_rate(0.1f) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param baseline Baseline level
     * @param max Max level
     * @param decay Decay rate
     */
    NeuromodulatorParameters(float baseline, float max, float decay)
        : baseline_level(baseline)
        , max_level(max)
        , decay_rate(decay)
        , rise_rate(0.1f)
        , effect_strength(1.0f)
        , adaptation_rate(0.01f)
        , noise_level(0.05f)
        , reset_rate(0.1f) {}
    
    /**
     * @brief Update parameters from configuration
     * 
     * @param config Parameters to merge
     * @param overwrite If true, overwrite existing parameters (default: true)
     */
    void update(const NeuromodulatorParameters& config, bool overwrite = true);
    
    /**
     * @brief Validate parameter values
     * 
     * @return bool true if parameters are valid
     */
    bool validate() const;
    
    /**
     * @brief Get parameter description
     * 
     * @param param_name Name of parameter
     * @return std::string Description of the parameter
     */
    std::string getDescription(const std::string& param_name) const;
};

/**
 * @class Neuromodulator
 * @brief Abstract base class for neuromodulatory signals
 * 
 * This class defines the interface for neuromodulatory signals that modulate
 * neural activity and plasticity. Neuromodulators are chemical signals that
 * influence multiple aspects of neural computation, including:
 * - Synaptic plasticity (STDP, Hebbian)
 * - Neural excitability
 * - Memory consolidation
 * - Attention and salience
 * - Learning rate adjustment
 * - Behavioral state
 * 
 * Key Features:
 * - Concentration dynamics with rise/decay rates
 * - State-dependent modulation
 * - Integration with neural systems
 * - Efficient computation
 * - Thread-safe operation
 * - Parameter configuration
 * - Interaction management
 * 
 * Neuromodulators are used for:
 * - Reward prediction error (dopamine)
 * - Attention modulation (acetylcholine)
 * - Arousal and vigilance (norepinephrine)
 * - Mood and impulsivity (serotonin)
 * - Curiosity and exploration (novelty)
 * - Prediction error (prediction error signals)
 * 
 * Usage Examples:
 * @code
 * #include "src/neuromodulation/Neuromodulator.hpp"
 * 
 * // Create dopamine neuromodulator
 * Dopamine dopamine;
 * dopamine.setLevel(0.5f);  // Set reward prediction error signal
 * 
 * // Apply neuromodulatory effect
 * float plasticity_factor = dopamine.getPlasticityFactor();
 * float scaled_weight_change = original_change * plasticity_factor;
 * 
 * // Update neuromodulator state
 * dopamine.update(0.01f);  // 10ms timestep
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and reduced
 *       coupling. The actual implementation is in Neuromodulator.cpp.
 */
class Neuromodulator {
public:
    /**
     * @brief Constructor
     * 
     * Creates a neuromodulator with default parameters.
     */
    Neuromodulator();
    
    /**
     * @brief Constructor with parameters
     * 
     * @param params Neuromodulator parameters
     */
    explicit Neuromodulator(const NeuromodulatorParameters& params);
    
    /**
     * @brief Destructor
     * 
     * Cleans up all allocated resources and ensures proper cleanup of neuromodulator state.
     */
    virtual ~Neuromodulator();
    
    /**
     * @brief Disable copy construction
     * 
     * Neuromodulators cannot be copied because they contain:
     * - Internal state that should not be shared
     * - Dynamic resources requiring unique ownership
     * - Concentration dynamics that would be corrupted by sharing
     */
    Neuromodulator(const Neuromodulator&) = delete;
    Neuromodulator& operator=(const Neuromodulator&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * Allows efficient transfer of neuromodulator ownership from one
     * instance to another. After move construction, the source neuromodulator
     * is in a valid but unspecified state.
     */
    Neuromodulator(Neuromodulator&&) noexcept;
    Neuromodulator& operator=(Neuromodulator&&) noexcept;
    
    /**
     * @brief Get the name of this neuromodulator
     * 
     * @return const char* Name of the neuromodulator
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration level
     * 
     * @return float Current concentration level
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the concentration level
     * 
     * @param level New concentration level
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get the neuromodulatory effect on plasticity
     * 
     * @return float Plasticity modulation factor (typically [0, 2])
     * 
     * This factor multiplies synaptic weight changes, where:
     * - 1.0: no modulation
     * - >1.0: enhancement of plasticity
     * - <1.0: reduction of plasticity
     * - 0.0: complete inhibition of plasticity
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state
     * 
     * @param dt Time step duration in seconds
     * 
     * This method updates the neuromodulator's concentration level
     * based on the time step, applying rise and decay dynamics.
     */
    virtual void update(TimestepDuration dt) = 0;
    
    /**
     * @brief Get the current neuromodulator parameters
     * 
     * @return const NeuromodulatorParameters& Current parameters
     */
    const NeuromodulatorParameters& getParameters() const;
    
    /**
     * @brief Set neuromodulator parameters
     * 
     * @param params New parameters
     */
    void setParameters(const NeuromodulatorParameters& params);
    
    /**
     * @brief Reset neuromodulator to baseline concentration
     * 
     * @param rate Reset rate (0-1)
     */
    void reset(float rate = 1.0f);
    
    /**
     * @brief Check if neuromodulator has been initialized
     * 
     * @return bool true if neuromodulator is initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Initialize neuromodulator with parameters
     * 
     * @param params Neuromodulator parameters
     * 
     * @post Neuromodulator is initialized with the specified parameters
     */
    void initialize(const NeuromodulatorParameters& params);
    
    /**
     * @brief Get neuromodulator statistics
     * 
     * @return NeuromodulatorStats Statistics about neuromodulator activity
     */
    struct NeuromodulatorStats {
        float current_level;          ///< Current concentration level
        float baseline_level;         ///< Baseline concentration level
        float max_level;              ///< Maximum concentration level
        float total_change;           ///< Total change from baseline
        float average_level;          ///< Average concentration level
        float time_above_threshold;  ///< Time spent above threshold
        Timestamp last_change;       ///< Time of last change
        Timestamp last_reset;        ///< Time of last reset
    };
    
    NeuromodulatorStats getStats() const;
    
    /**
     * @brief Check if neuromodulator is active
     * 
     * @param threshold Activity threshold
     * @return bool true if neuromodulator is above threshold
     */
    bool isActive(float threshold = 0.1f) const;
    
    /**
     * @brief Apply neuromodulatory influence to neural systems
     * 
     * @param neurons Vector of neurons to influence
     * @param synapses Vector of synapses to influence
     * @param memory Pointer to working memory system
     * @param dt Time step duration
     */
    virtual void applyInfluence(const std::vector<Neuron*>& neurons,
                              const std::vector<Synapse*>& synapses,
                              WorkingMemory* memory,
                              TimestepDuration dt);
    
    /**
     * @brief Get neuromodulatory influence on a specific synapse
     * 
     * @param synapse Synapse to evaluate
     * @return float Neuromodulatory influence factor
     */
    virtual float getInfluenceOnSynapse(Synapse* synapse) const;
    
    /**
     * @brief Get neuromodulatory influence on a specific neuron
     * 
     * @param neuron Neuron to evaluate
     * @return float Neuromodulatory influence factor
     */
    virtual float getInfluenceOnNeuron(Neuron* neuron) const;
    
    /**
     * @brief Get neuromodulatory influence on working memory
     * 
     * @param memory Working memory system
     * @return float Neuromodulatory influence factor
     */
    virtual float getInfluenceOnMemory(WorkingMemory* memory) const;
    
protected:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    
    Impl* pImpl; ///< Pointer to implementation details

private:
    /**
     * @brief Apply concentration dynamics
     * 
     * @param dt Time step duration
     * @param target_level Target concentration level
     */
    void applyConcentrationDynamics(TimestepDuration dt, float target_level);
    
    /**
     * @brief Add biological noise to concentration
     * 
     * @param current_level Current concentration level
     * @return float Concentration level with noise
     */
    float addNoise(float current_level) const;
}; // class Neuromodulator

/**
 * @class Dopamine
 * @brief Reward and reinforcement learning neuromodulator
 * 
 * This class implements dopamine neuromodulation, which is crucial for:
 * - Reward prediction error signaling
 * - Reinforcement learning
 * - Motivation and reward processing
 * - Action selection and value learning
 * - Synaptic plasticity modulation
 * - Learning rate adjustment
 * 
 * Dopamine follows a phasic and tonic coding scheme:
 * - Phasic: Brief, transient increases for reward prediction errors
 * - Tonic: Baseline levels reflecting overall reward expectations
 * 
 * The dopamine system is modeled after the physiological system with:
 * - Reward prediction error signals
 * - Learning rate modulation
 * - Synaptic weight changes
 * - Neuromodulatory effects on plasticity
 * 
 * Usage Examples:
 * @code
 * #include "src/neuromodulation/Neuromodulator.hpp"
 * 
 * // Create dopamine neuromodulator
 * Dopamine dopamine;
 * 
 * // Configure with reward prediction error
 * dopamine.setLevel(0.5f);  // Reward prediction error signal
 * 
 * // Signal reward
 * dopamine.signalReward(1.0f);  // Positive reward
 * 
 * // Signal reward prediction error
 * dopamine.signalRewardPredictionError(0.3f);  // 30% error
 * 
 * // Apply neuromodulatory effect
 * float plasticity_factor = dopamine.getPlasticityFactor();
 * float scaled_change = original_change * plasticity_factor;
 * 
 * // Update state
 * dopamine.update(0.01f);  // 10ms timestep
 * @endcode
 */
class Dopamine : public Neuromodulator {
public:
    /**
     * @brief Default constructor
     */
    Dopamine();
    
    /**
     * @brief Constructor with parameters
     */
    explicit Dopamine(const NeuromodulatorParameters& params);
    
    /**
     * @brief Destructor
     */
    ~Dopamine() override;
    
    /**
     * @brief Disable copy construction
     */
    Dopamine(const Dopamine&) = delete;
    Dopamine& operator=(const Dopamine&) = delete;
    
    /**
     * @brief Enable move construction
     */
    Dopamine(Dopamine&&) noexcept;
    Dopamine& operator=(Dopamine&&) noexcept;
    
    /**
     * @brief Get the name of this neuromodulator
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current concentration level
     */
    float getLevel() const override;
    
    /**
     * @brief Set the concentration level
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the neuromodulatory effect on plasticity
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update neuromodulator state
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal a reward event
     * 
     * @param reward Reward value (positive for reward, negative for punishment)
     * 
     * This method signals a reward event, causing a phasic increase in dopamine
     * concentration proportional to the reward value.
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error
     * 
     * @param error Reward prediction error (positive for better than expected, negative for worse)
     */
    void signalRewardPredictionError(float error);
    
    /**
     * @brief Get the current reward prediction error
     * 
     * @return float Reward prediction error
     */
    float getRewardPredictionError() const;
    
    /**
     * @brief Get the current reward value
     * 
     * @return float Current reward value
     */
    float getCurrentReward() const;
    
    /**
     * @brief Get the reward history
     * 
     * @return const std::vector<float>& Reward history
     */
    const std::vector<float>& getRewardHistory() const;
    
    /**
     * @brief Get the reward prediction error history
     * 
     * @return const std::vector<float>& Reward prediction error history
     */
    const std::vector<float>& getRewardErrorHistory() const;
    
    /**
     * @brief Reset reward history
     */
    void resetRewardHistory();
    
    /**
     * @brief Get dopamine statistics
     * 
     * @return DopamineStats Statistics about dopamine activity
     */
    struct DopamineStats {
        float current_level;          ///< Current dopamine level
        float reward_prediction_error;///< Current reward prediction error
        float total_rewards;          ///< Total rewards received
        float average_reward;         ///< Average reward value
        float time_above_threshold;  ///< Time spent above threshold
        Timestamp last_reward;       ///< Time of last reward
        Timestamp last_error;        ///< Time of last error prediction
    };
    
    DopamineStats getStats() const;
    
    /**
     * @brief Check if dopamine is in phasic mode
     * 
     * @return bool true if dopamine is in phasic (high) state
     */
    bool isPhasic() const;
    
    /**
     * @brief Get phasic level
     * 
     * @return float Phasic level
     */
    float getPhasicLevel() const;
    
    /**
     * @brief Get tonic level
     * 
     * @return float Tonic level
     */
    float getTonicLevel() const;
    
private:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class Dopamine

/**
 * @class Acetylcholine
 * @brief Attention and memory consolidation neuromodulator
 * 
 * This class implements acetylcholine neuromodulation, which is crucial for:
 * - Attention modulation
 * - Memory consolidation
 * - Synaptic plasticity control
 * - Learning rate adjustment
 * - Behavioral flexibility
 * - Cognitive processing
 * 
 * Acetylcholine modulates:
 * - Synaptic strength and efficacy
 * - Memory encoding and retrieval
 * - Attentional focus and selection
 * - Neural excitability
 * - Learning dynamics
 * 
 * The acetylcholine system is modeled after the physiological system with:
 * - Phasic increases for attention and learning
 * - Tonic baseline for sustained attention
 * - State-dependent modulation
 * - Interaction with other neuromodulators
 * - Integration with cognitive processes
 * 
 * Usage Examples:
 * @code
 * #include "src/neuromodulation/Neuromodulator.hpp"
 * 
 * // Create acetylcholine neuromodulator
 * Acetylcholine ach;
 * 
 * // Configure for attention
 * ach.setLevel(0.8f);  // High attention level
 * 
 * // Update state
 * ach.update(0.01f);  // 10ms timestep
 * 
 * // Apply neuromodulatory effect
 * float plasticity_factor = ach.getPlasticityFactor();
 * float scaled_change = original_change * plasticity_factor;
 * @endcode
 */
class Acetylcholine : public Neuromodulator {
public:
    /**
     * @brief Default constructor
     */
    Acetylcholine();
    
    /**
     * @brief Constructor with parameters
     */
    explicit Acetylcholine(const NeuromodulatorParameters& params);
    
    /**
     * @brief Destructor
     */
    ~Acetylcholine() override;
    
    /**
     * @brief Disable copy construction
     */
    Acetylcholine(const Acetylcholine&) = delete;
    Acetylcholine& operator=(const Acetylcholine&) = delete;
    
    /**
     * @brief Enable move construction
     */
    Acetylcholine(Acetylcholine&&) noexcept;
    Acetylcholine& operator=(Acetylcholine&&) noexcept;
    
    /**
     * @brief Get the name of this neuromodulator
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current concentration level
     */
    float getLevel() const override;
    
    /**
     * @brief Set the concentration level
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the neuromodulatory effect on plasticity
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update neuromodulator state
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal attentional focus
     * 
     * @param focus_level Attention focus level (0-1)
     */
    void signalAttention(float focus_level);
    
    /**
     * @brief Signal memory consolidation
     * 
     * @param consolidation_strength Memory consolidation strength
     */
    void signalMemoryConsolidation(float consolidation_strength);
    
    /**
     * @brief Get attentional focus level
     */
    float getAttentionLevel() const;
    
    /**
     * @brief Get memory consolidation level
     */
    float getMemoryConsolidationLevel() const;
    
    /**
     * @brief Get attentional statistics
     */
    struct AcetylcholineStats {
        float current_level;          ///< Current acetylcholine level
        float attention_level;        ///< Current attentional focus
        float memory_consolidation;   ///< Current memory consolidation level
        float time_above_threshold;  ///< Time spent above threshold
        Timestamp last_attention;    ///< Time of last attention signal
        Timestamp last_memory;       ///< Time of last memory consolidation signal
    };
    
    AcetylcholineStats getStats() const;
    
    /**
     * @brief Check if acetylcholine is modulating attention
     */
    bool isModulatingAttention() const;
    
    /**
     * @brief Check if acetylcholine is consolidating memory
     */
    bool isConsolidatingMemory() const;
private:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class Acetylcholine

/**
 * @class Norepinephrine
 * @brief Arousal and vigilance neuromodulator
 * 
 * This class implements norepinephrine neuromodulation, which is crucial for:
 * - Arousal and vigilance
 * - Alertness and attention
 * - Stress response
 * - Memory encoding
 * - Behavioral activation
 * - Cognitive control
 * 
 * Norepinephrine modulates:
 * - Neural excitability
 * - Attentional focus
 * - Memory formation
 * - Stress response
 * - Behavioral activation
 * - Learning dynamics
 * 
 * The norepinephrine system is modeled after the physiological system with:
 * - Phasic increases for alerts and stress
 * - Tonic baseline for baseline arousal
 * - State-dependent modulation
 * - Interaction with other neuromodulators
 * - Integration with stress response
 * 
 * Usage Examples:
 * @code
 * #include "src/neuromodulation/Neuromodulator.hpp"
 * 
 * // Create norepinephrine neuromodulator
 * Norepinephrine ne;
 * 
 * // Configure for alertness
 * ne.setLevel(0.7f);  // Alertness level
 * 
 * // Update state
 * ne.update(0.01f);  // 10ms timestep
 * 
 * // Apply neuromodulatory effect
 * float plasticity_factor = ne.getPlasticityFactor();
 * float scaled_change = original_change * plasticity_factor;
 * @endcode
 */
class Norepinephrine : public Neuromodulator {
public:
    /**
     * @brief Default constructor
     */
    Norepinephrine();
    
    /**
     * @brief Constructor with parameters
     */
    explicit Norepinephrine(const NeuromodulatorParameters& params);
    
    /**
     * @brief Destructor
     */
    ~Norepinephrine() override;
    
    /**
     * @brief Disable copy construction
     */
    Norepinephrine(const Norepinephrine&) = delete;
    Norepinephrine& operator=(const Norepinephrine&) = delete;
    
    /**
     * @brief Enable move construction
     */
    Norepinephrine(Norepinephrine&&) noexcept;
    Norepinephrine& operator=(Norepinephrine&&) noexcept;
    
    /**
     * @brief Get the name of this neuromodulator
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current concentration level
     */
    float getLevel() const override;
    
    /**
     * @brief Set the concentration level
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the neuromodulatory effect on plasticity
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update neuromodulator state
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal stress response
     * 
     * @param stress_level Stress level (0-1)
     */
    void signalStress(float stress_level);
    
    /**
     * @brief Signal arousal increase
     * 
     * @param arousal_level Arousal level (0-1)
     */
    void signalArousal(float arousal_level);
    
    /**
     * @brief Get stress level
     */
    float getStressLevel() const;
    
    /**
     * @brief Get arousal level
     */
    float getArousalLevel() const;
    
    /**
     * @brief Get norepinephrine statistics
     */
    struct NorepinephrineStats {
        float current_level;          ///< Current norepinephrine level
        float stress_level;           ///< Current stress level
        float arousal_level;          ///< Current arousal level
        float time_above_threshold;  ///< Time spent above threshold
        Timestamp last_stress;        ///< Time of last stress signal
        Timestamp last_arousal;       ///< Time of last arousal signal
    };
    
    NorepinephrineStats getStats() const;
    
    /**
     * @brief Check if norepinephrine is in stress response
     */
    bool isInStressResponse() const;
    
    /**
     * @brief Check if norepinephrine is increasing arousal
     */
    bool isIncreasingArousal() const;
private:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class Norepinephrine

/**
 * @class Serotonin
 * @brief Mood, impulsivity, and social behavior neuromodulator
 * 
 * This class implements serotonin neuromodulation, which is crucial for:
 * - Mood regulation
 * - Impulsivity control
 * - Social behavior
 * - Anxiety modulation
 * - Sleep regulation
 * - Appetite control
 * - Learning dynamics
 * 
 * Serotonin modulates:
 * - Emotional state and mood
 * - Impulsive behavior
 * - Social behavior
 * - Anxiety levels
 * - Cognitive processing
 * - Behavioral inhibition
 * - Learning and memory
 * 
 * The serotonin system is modeled after the physiological system with:
 * - Phasic increases for positive mood
 * - Tonic baseline for mood regulation
 * - State-dependent modulation
 * - Interaction with other neuromodulators
 * - Integration with emotional processing
 * 
 * Usage Examples:
 * @code
 * #include "src/neuromodulation/Neuromodulator.hpp"
 * 
 * // Create serotonin neuromodulator
 * Serotonin serotonin;
 * 
 * // Configure for mood regulation
 * serotonin.setLevel(0.6f);  // Mood level
 * 
 * // Update state
 * serotonin.update(0.01f);  // 10ms timestep
 * 
 * // Apply neuromodulatory effect
 * float plasticity_factor = serotonin.getPlasticityFactor();
 * float scaled_change = original_change * plasticity_factor;
 * @endcode
 */
class Serotonin : public Neuromodulator {
public:
    /**
     * @brief Default constructor
     */
    Serotonin();
    
    /**
     * @brief Constructor with parameters
     */
    explicit Serotonin(const NeuromodulatorParameters& params);
    
    /**
     * @brief Destructor
     */
    ~Serotonin() override;
    
    /**
     * @brief Disable copy construction
     */
    Serotonin(const Serotonin&) = delete;
    Serotonin& operator=(const Serotonin&) = delete;
    
    /**
     * @brief Enable move construction
     */
    Serotonin(Serotonin&&) noexcept;
    Serotonin& operator=(Serotonin&&) noexcept;
    
    /**
     * @brief Get the name of this neuromodulator
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current concentration level
     */
    float getLevel() const override;
    
    /**
     * @brief Set the concentration level
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the neuromodulatory effect on plasticity
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update neuromodulator state
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal positive mood
     * 
     * @param mood_level Mood level (0-1)
     */
    void signalPositiveMood(float mood_level);
    
    /**
     * @brief Signal negative mood
     * 
     * @param mood_level Mood level (0-1)
     */
    void signalNegativeMood(float mood_level);
    
    /**
     * @brief Get current mood level
     */
    float getMoodLevel() const;
    
    /**
     * @brief Get impulse control level
     */
    float getImpulseControl() const;
    
    /**
     * @brief Get social behavior level
     */
    float getSocialBehaviorLevel() const;
    
    /**
     * @brief Get serotonin statistics
     */
    struct SerotoninStats {
        float current_level;          ///< Current serotonin level
        float mood_level;             ///< Current mood level
        float impulse_control;        ///< Current impulse control level
        float social_behavior;       ///< Current social behavior level
        float time_above_threshold;  ///< Time spent above threshold
        Timestamp last_mood;         ///< Time of last mood signal
    };
    
    SerotoninStats getStats() const;
    
    /**
     * @brief Check if serotonin is promoting positive mood
     */
    bool isPromotingPositiveMood() const;
    
    /**
     * @brief Check if serotonin is promoting negative mood
     */
    bool isPromotingNegativeMood() const;
private:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class Serotonin

/**
 * @class Novelty
 * @brief Novelty detection and exploration neuromodulator
 * 
 * This class implements novelty neuromodulation, which is crucial for:
 * - Novelty detection
 * - Exploration behavior
 * - Curiosity and learning
 * - Information seeking
 * - Cognitive flexibility
 * - Adaptation to new environments
 * 
 * Novelty modulates:
 * - Exploration vs. exploitation balance
 * - Information seeking behavior
 * - Learning about novel stimuli
 * - Cognitive flexibility
 * - Behavioral adaptation
 * - Curiosity and interest
 * 
 * The novelty system is modeled after the physiological system with:
 * - Phasic increases for novel stimuli
 * - Tonic baseline for exploration
 * - State-dependent modulation
 * - Integration with learning systems
 * - Adaptation to environmental changes
 * 
 * Usage Examples:
 * @code
 * #include "src/neuromodulation/Neuromodulator.hpp"
 * 
 * // Create novelty neuromodulator
 * Novelty novelty;
 * 
 * // Configure for exploration
 * novelty.setLevel(0.8f);  // Exploration level
 * 
 * // Update state
 * novelty.update(0.01f);  // 10ms timestep
 * 
 * // Apply neuromodulatory effect
 * float plasticity_factor = novelty.getPlasticityFactor();
 * float scaled_change = original_change * plasticity_factor;
 * @endcode
 */
class Novelty : public Neuromodulator {
public:
    /**
     * @brief Default constructor
     */
    Novelty();
    
    /**
     * @brief Constructor with parameters
     */
    explicit Novelty(const NeuromodulatorParameters& params);
    
    /**
     * @brief Destructor
     */
    ~Novelty() override;
    
    /**
     * @brief Disable copy construction
     */
    Novelty(const Novelty&) = delete;
    Novelty& operator=(const Novelty&) = delete;
    
    /**
     * @brief Enable move construction
     */
    Novelty(Novelty&&) noexcept;
    Novelty& operator=(Novelty&&) noexcept;
    
    /**
     * @brief Get the name of this neuromodulator
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current concentration level
     */
    float getLevel() const override;
    
    /**
     * @brief Set the concentration level
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the neuromodulatory effect on plasticity
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update neuromodulator state
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal novelty detection
     * 
     * @param novelty_level Novelty level (0-1)
     */
    void signalNovelty(float novelty_level);
    
    /**
     * @brief Signal curiosity
     * 
     * @param curiosity_level Curiosity level (0-1)
     */
    void signalCuriosity(float curiosity_level);
    
    /**
     * @brief Get novelty detection level
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get curiosity level
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty statistics
     */
    struct NoveltyStats {
        float current_level;          ///< Current novelty level
        float novelty_level;          ///< Current novelty detection level
        float curiosity_level;        ///< Current curiosity level
        float time_above_threshold;  ///< Time spent above threshold
        Timestamp last_novelty;      ///< Time of last novelty signal
        Timestamp last_curiosity;     ///< Time of last curiosity signal
    };
    
    NoveltyStats getStats() const;
    
    /**
     * @brief Check if novelty is being detected
     */
    bool isDetectingNovelty() const;
    
    /**
     * @brief Check if curiosity is present
     */
    bool isPresentingCuriosity() const;
private:
    /**
     * @brief Implementation details structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class Novelty

} // namespace nlm

// Close NLM filename guard
#endif // NLM_NEUROMODULATOR_HPP

