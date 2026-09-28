#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Synapse;
class Brain;

/**
 * @class Neuromodulator
 * @brief Abstract base class for neuromodulatory signals and brain chemistry
 * 
 * The Neuromodulator class defines the interface for chemical signals that modulate
 * neural processing across multiple timescales. Neuromodulators are global signals
 * that affect many neurons and synapses simultaneously, providing top-down control
 * over neural computation.
 * 
 * **Types of Neuromodulation:**
 * - **Dopamine**: Reward prediction error, reinforcement learning
 * - **Acetylcholine**: Attention, arousal, memory consolidation
 * - **Norepinephrine**: Vigilance, stress response, locus coeruleus
 * - **Serotonin**: Mood, impulsivity, social behavior
 * 
 * **Modulation Mechanisms:**
 * - **Plasticity Modulation**: Alters synaptic learning rates and sign
 * - **Synaptic Gain**: Modifies synaptic transmission efficacy
 * - **Neuromodulatory State**: Sets overall brain excitability
 * - **Cellular Properties**: Changes intrinsic neuronal properties
 * 
 * **Computational Roles:**
 * - **Global State**: Establishes brain-wide functional state
 * - **Credit Assignment**: Helps solve temporal credit assignment problem
 * - **Adaptive Control**: Adjusts processing based on environmental demands
 * - **Motivational Influence**: Biases behavior toward goals
 * 
 * **Temporal Dynamics:**
 * - **Phasic Signals**: Brief, event-related responses (seconds)
 * - **Tonic Signals**: Sustained background activity (minutes-hours)
 * - **Integrative Time**: Combined effects across timescales
 * - **Feedback Loops**: Homeostatic regulation of neuromodulator levels
 * 
 * **Biological Plausibility:**
 * - Based on locus coeruleus-noradrenergic and other neuromodulator systems
 * - Matches experimental data from neuropharmacology
 * - Implements known biological mechanisms
 * - Incorporates release and uptake dynamics
 * 
 * @note This is an abstract base class - concrete implementations provide
 *       specific neuromodulator types (Dopamine, ACh, NE, 5-HT)
 * @see Dopamine for reward and reinforcement signaling
 * @see Acetylcholine for attention and memory modulation
 * @see Norepinephrine for arousal and vigilance
 * @see Serotonin for mood and behavioral regulation
 * @see Brain for system integration
 * 
 * @warning Neuromodulators have broad effects across the brain
 * @warning Thread safety is critical when modulating neural activity
 */

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the name of this neuromodulator
     * 
     * @return const char* Human-readable name (e.g., "Dopamine", "Acetylcholine")
     * @post Returns static string identifier
     * @note Used for debugging and system configuration
     * @see getLevel() to get concentration
     * @see getPlasticityFactor() to get modulation effect
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration or level of the neuromodulator
     * 
     * @return float Current concentration (0.0 to 1.0 typical normalized range)
     * @post Returns current neuromodulator level
     * @pre Level should be within valid bounds
     * @note Level represents both phasic and tonic components
     * @see setLevel() to modify level
     * @see update() to simulate level dynamics
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the concentration or level of the neuromodulator
     * 
     * @param level New concentration (0.0 to 1.0 typical normalized range)
     * @post Neuromodulator concentration updated
     * @pre level should be within valid bounds [0.0, 1.0]
     * @note Direct level setting bypasses natural dynamics
     * @see getLevel() to retrieve current level
     * @see update() for natural level simulation
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get the plasticity modulation factor
     * 
     * @return float Plasticity modulation factor (-1.0 to 1.0 typical range)
     * @post Returns current modulation factor
     * @note Factor multiplies synaptic plasticity updates
     * @see update() to apply modulation dynamics
     * @note Positive values enhance learning, negative values suppress
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state and dynamics for one timestep
     * 
     * @param dt Time step duration (seconds)
     * @post Neuromodulator state updated according to dynamics
     * @pre dt should be positive and valid
     * @note Simulates natural release, diffusion, and clearance
     * @note Handles both phasic responses and tonic background levels
     * @see setLevel() for direct level setting
     * @see getPlasticityFactor() to retrieve modulation effect
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

/**
 * @class Dopamine
 * @brief Implements dopamine neuromodulation for reward and reinforcement learning
 * 
 * Dopamine is one of the most extensively studied neuromodulators, playing crucial
 * roles in reward prediction error, reinforcement learning, motivation, and
 * motor control. It implements the core computational signal for temporal
 * difference learning in biological systems.
 * 
 * **Computational Roles:**
 * - **Reward Prediction Error**: Signals discrepancy between expected and actual reward
 * - **Reinforcement Learning**: Drives synaptic plasticity for rewarded behaviors
 * - **Temporal Difference Learning**: Integrates prediction errors over time
 * - **Motivational Salience**: Biases behavior toward reward-seeking actions
 * 
 * **Neural Mechanisms:**
 * - **VTA/SNc Projections**: Ventral tegmental area and substantia nigra compacta
 * - **D1-D2 Receptor Balance**: Direct and indirect pathway modulation
 * - **Phasic Responses**: Burst firing in response to rewards
 * - **Tonic Background**: Baseline activity regulating motivation
 * 
 * **Plasticity Integration:**
 * - **LTP/LTD Modulation**: Enhances/suppresses synaptic learning
 * - ** eligibility Trace Decay**: Sets timescale for reward integration
 * - **Credit Assignment**: Solves temporal credit assignment problem
 * - **Learning Rate Control**: Adjusts plasticity based on reward history
 * 
 * **Behavioral Functions:**
 * - **Movement Selection**: Biases toward rewarded actions
 * - **Attention Focus**: Enhances processing of reward-relevant stimuli
 * - **Memory Consolidation**: Strengthens reward-associated memories
 * - **Goal Pursuit**: Maintains motivation toward delayed rewards
 * 
 * **Clinical Relevance:**
 * - **Schizophrenia**: Dopamine dysregulation affects psychosis
 * - **Parkinson's**: Dopamine loss affects motor control
 * - **Depression**: Reward system dysfunction reduces motivation
 * - **Addiction**: Reward system hijacking leads to compulsive behavior
 * 
 * @note This implementation focuses on reward prediction error signals
 * @see Neuromodulator for base neuromodulation interface
 * @see RewardModulatedSTDP for reward-integrated plasticity
 * @see PredictionSystem for reward prediction computation
 * @see Curiosity for exploration motivation
 * 
 * @warning Dopamine has widespread effects across the brain
 * @warning Imbalance can lead to pathological behaviors
 */
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling and computation
    /**
     * @brief Signal a reward outcome
     * 
     * @param reward Magnitude of the reward received (0.0 to positive values)
     * @post Dopamine level updated to reflect reward
     * @pre reward should be non-negative
     * @note Typically triggers phasic burst response
     * @see signalRewardPredictionError() for prediction error computation
     * @see getLevel() to check resulting dopamine level
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error
     * 
     * @param error Difference between expected and actual reward
     * @post Dopamine level updated to reflect prediction error
     * @pre error can be positive (better than expected) or negative (worse than expected)
     * @note Central mechanism for temporal difference learning
     * @see signalReward() for direct reward signaling
     * @see getLevel() to check resulting prediction error signal
     */
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class Acetylcholine
 * @brief Implements cholinergic modulation for attention and memory
 * 
 * Acetylcholine (ACh) is crucial for attention, arousal, and memory consolidation.
 * It implements the cholinergic system's role in regulating neural excitability
 * and coordinating cortical activity across brain regions.
 * 
 * **Computational Roles:**
 * - **Attention Modulation**: Biases processing toward relevant stimuli
 * - **Arousal Regulation**: Sets overall brain excitability level
 * - **Memory Consolidation**: Enhances long-term memory formation
 * - **Cortical Processing**: Coordinates across brain regions
 * 
 * **Neural Mechanisms:**
 * - **Basal Forebrain Projections**: Nucleus basalis and lateral septum
 * - **Alpha7 Nicotinic Receptors**: Fast signaling and synchronization
 * - **Muscarinic Receptors**: Slow, sustained modulation
 * - **Thalamic Relay**: Attention to sensory processing
 * 
 * **Attention Functions:**
 * - **Selective Attention**: Enhances target stimulus processing
 * - **Feature Binding**: Integrates across sensory modalities
 * - **Working Memory**: Maintains attentional sets
 * - **Cognitive Control**: Implements task sets and rules
 * 
 * **Memory Functions:**
 * - **Encoding**: Enhances new memory formation
 * - **Consolidation**: Strengthens memory traces over time
 * - **Reconsolidation**: Updates existing memories
 * - **Retrieval**: Facilitates memory access
 * 
 * **Clinical Relevance:**
 * - **ADHD**: Acetylcholine dysfunction affects attention
 * - **Schizophrenia**: Cognitive deficits involve cholinergic systems
 * - **Alzheimer's**: Early cholinergic loss affects memory
 * - **Learning**: ACh modulation enhances learning efficiency
 * 
 * @note This implementation focuses on attentional modulation
 * @see Neuromodulator for base neuromodulation interface
 * @see AttentionalSelection for attention implementation
 * @see EpisodicMemory for memory consolidation
 * 
 * @warning Cholinergic drugs have widespread effects
 * @warning Balance with other neuromodulators is critical
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
 * @brief Implements noradrenergic modulation for arousal and vigilance
 * 
 * Norepinephrine (NE), released by the locus coeruleus, is fundamental for
 * arousal, vigilance, attention, and stress responses. It implements the
 * brain's alerting system across multiple timescales and contexts.
 * 
 * **Computational Roles:**
 * - **Arousal Regulation**: Sets overall alertness level
 * - **Vigilance Enhancement**: Improves sensory processing
 * - **Stress Response**: Coordinates fight-or-flight responses
 * - **Learning Optimization**: Balances exploration and exploitation
 * 
 * **Neural Mechanisms:**
 * - **Locus Coeruleus Projections**: Widespread brain region innervation
 * - **Alpha2 Autoreceptors**: Negative feedback regulation
 * - **Adrenergic Receptors**: Diverse downstream effects
 * - **Response Variability**: Phasic and tonic firing patterns
 * 
 * **Arousal Functions:**
 * - **Sensory Gating**: Modulates signal-to-noise ratio
 * - **Attention Focus**: Enhances target stimulus processing
 * - **Memory Encoding**: Improves new memory formation
 * - **Cognitive Flexibility**: Adapts to changing environments
 * 
 * **Physiological Functions:**
 * - **Wake-Sleep Regulation**: Transitions between states
 * - **Stress Adaptation**: Response to environmental challenges
 * - **Metabolic Control**: Influences energy allocation
 * - **Cardiovascular Regulation**: Autonomic system control
 * 
 * **Clinical Relevance:**
 * - **PTSD**: Hyperarousal and intrusive memories
 * - **ADHD**: Attention regulation deficits
 * - **Anxiety**: Hypervigilance and worry
 * - **Depression**: Reduced arousal and motivation
 * 
 * @note This implementation focuses on arousal modulation
 * @see Neuromodulator for base neuromodulation interface
 * @see AttentionalSelection for attention mechanisms
 * @see StressResponse for stress regulation
 * 
 * @warning NE has widespread effects across the brain
 * @warning Balance with other neuromodulators is critical
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
 * @brief Implements serotonergic modulation for mood and social behavior
 * 
 * Serotonin (5-HT) regulates mood, impulsivity, aggression, and social behavior.
 * It implements the brain's social and emotional regulatory systems, influencing
 * a wide range of behaviors from prosocial to aggressive responses.
 * 
 * **Computational Roles:**
 * - **Mood Regulation**: Sets emotional tone and affective state
 * - **Behavioral Inhibition**: Controls impulsivity and aggression
 * - **Social Behavior**: Modulates prosocial and antisocial tendencies
 * - **Stress Resilience**: Influences stress coping mechanisms
 * 
 * **Neural Mechanisms:**
 * - **Raphe Nuclei Projections**: Multiple raphe nuclei widespread innervation
 * - **5-HT1A Receptors**: Anxiolytic and depressive effects
 * - **5-HT2A Receptors**: Perceptual and cognitive effects
 * - **5-HT3 Receptors**: Excitatory and sensory effects
 * 
 * **Behavioral Functions:**
 * - **Mood States**: Positive and negative affect regulation
 * - **Impulse Control**: Reduces rash and aggressive behaviors
 * - **Social Bonding**: Enhances prosocial tendencies
 * - **Anhedonia**: Reduces reward sensitivity
 * 
 * **Cognitive Functions:**
 * - **Decision Making**: Biases toward safer options
 * - **Risk Assessment**: Influences probabilistic choices
 * - **Social Cognition**: Affects theory of mind and empathy
 * - **Memory Modulation**: Influences emotional memory
 * 
 * **Clinical Relevance:**
 * - **Depression**: Serotonin deficiency reduces mood and motivation
 * - **Anxiety**: Imbalance affects worry and fear responses
 * - **Schizophrenia**: Affects negative symptoms and social withdrawal
 * - **OCD**: Impulsiovecontrol and repetitive behaviors
 * 
 * @note This implementation focuses on mood regulation
 * @see Neuromodulator for base neuromodulation interface
 * @see Novelty for novelty detection
 * @see Curiosity for exploration motivation
 * 
 * @warning Serotonin has complex effects across multiple systems
 * @warning Imbalance can lead to mood disorders
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
