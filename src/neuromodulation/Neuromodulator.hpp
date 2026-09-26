#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

/**
 * Neuromodulator: Abstract base class for neuromodulatory signals
 * 
 * Neuromodulators are chemical signals that broadly affect neural activity
 * and synaptic plasticity. Unlike fast synaptic transmission, neuromodulation
 * acts globally across distributed neural systems, creating sustained changes
 * in how neural networks process information.
 * 
 * Key characteristics:
 * - Slow onset and prolonged duration
 * - Global effects on neural excitability
 * - Modulation of plasticity mechanisms
 * - Regulation of attention and arousal
 * 
 * Biological importance:
 * - Essential for adaptive behavior
 * - Underlying motivation and emotion
 * - Critical for learning and memory
 * - Basis of mood disorders and neurological conditions
 * 
 * @see https://en.wikipedia.org/wiki/Neuromodulation
 */
class Neuromodulator {
public:
    /**
     * @brief Virtual destructor for polymorphic cleanup
     */
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the name of the neuromodulator
     * 
     * @return Name string (e.g., "DA", "ACh", "NE")
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get current neuromodulator concentration
     * 
     * @return Current level (typically normalized 0.0-1.0)
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set neuromodulator concentration
     * 
     * @param level New concentration level
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Calculate plasticity modulation factor
     * 
     * @return Plasticity scaling factor (typically 0.1-2.0)
     * @details This factor modifies learning rates by scaling plasticity
     *          mechanisms like STDP and Hebbian learning.
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state over time
     * 
     * @param dt Time step (in seconds)
     * @details This method implements the temporal dynamics of neuromodulator
     *          release, decay, and clearance mechanisms.
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

/**
 * Dopamine: Reward prediction error and reinforcement learning
 * 
 * Biological basis:
 * - Produced in ventral tegmental area (VTA) and substantia nigra
 * - Projects to prefrontal cortex, basal ganglia, and limbic system
 * - Mediates reward prediction error (RPE) signaling
 * - Critical for reinforcement learning and motivation
 * 
 * Neuromodulatory effects:
 * - Reinforces reward-seeking behavior
 * - Enhances working memory for rewarding stimuli
 * - Reduces investigation of unrewarding options
 * - Modulates motor activity and action selection
 * 
 * Clinical relevance:
 * - Parkinson's disease: dopamine deficiency
 * - Schizophrenia: dopamine hyperactivity
 * - Addiction: pathological reward sensitization
 * - ADHD: dopamine dysregulation
 * 
 * @see Schultz, W. (2015). Dopamine systems in reward and learning.
 *      Annual Review of Neuroscience, 38, 205-226.
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
     * @brief Signal reward delivery
     * 
     * @param reward Magnitude of reward received (0.0-1.0)
     * @details This method increases dopamine levels in response to rewards,
     *          signaling prediction error when unexpected rewards occur.
     *          Forms the basis of reinforcement learning.
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal reward prediction error
     * 
     * @param error Reward prediction error (-1.0 to 1.0)
     * @details This method adjusts dopamine levels based on prediction errors,
     *          strengthening expected rewards and weakening unexpected ones.
     *          Central to temporal difference learning.
     */
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Acetylcholine: Attention, learning, and memory consolidation
 * 
 * Biological basis:
 * - Produced in basal forebrain and brainstem nuclei
 * - Projects to cortex, hippocampus, and thalamus
 * - Enhances signal-to-noise ratio in neural processing
 * - Critical for attention and cognitive control
 * 
 * Neuromodulatory effects:
 * - Increases neural excitability and firing rates
 * - Enhances long-term potentiation (LTP)
 * - Improves working memory capacity
 * - Reduces response to irrelevant stimuli
 * - Mediates sleep-wake cycles
 * 
 * Clinical relevance:
 * - Alzheimer's disease: cholinergic deficiency
 * - Schizophrenia: cholinergic dysregulation
 * - Attention disorders: cholinergic abnormalities
 * 
 * @see Hasselmo, M.E. (2006). The role of acetylcholine in learning and memory.
 *      Nature Reviews Neuroscience, 7(5), 317-327.
 */
class Acetylcholine : public Neuromodulator {
public:
    /**
     * @brief Get acetylcholine name
     * 
     * @return "ACh" (standard abbreviation)
     */
    const char* getName() const override { return "ACh"; }
    
    /**
     * @brief Get current acetylcholine level
     * 
     * @return Current level (placeholder: 0.0f)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set acetylcholine level
     * 
     * @param level New acetylcholine level (placeholder: ignored)
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get acetylcholine plasticity factor
     * 
     * @return Plasticity factor (placeholder: 1.0f, no modulation)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update acetylcholine state
     * 
     * @param dt Time step (placeholder: ignored)
     */
    void update(TimestepDuration dt) override {}
};

/**
 * Norepinephrine: Arousal, vigilance, and stress response
 * 
 * Biological basis:
 * - Produced in locus coeruleus brainstem nucleus
 * - Projects diffusely to cortex, thalamus, and subcortical areas
 * - Mediates fight-or-flight response
 * - Critical for attention and vigilance
 * 
 * Neuromodulatory effects:
 * - Increases arousal and alertness
 * - Enhances signal detection and discrimination
 * - Modulates working memory updating
 * - Reduces exploratory behavior when not needed
 * - Prepares body for stress response
 * 
 * Clinical relevance:
 * - ADHD: norepinephrine dysregulation
 * - Depression: norepinephrine deficiency
 * - Anxiety: norepinephrine hyperactivity
 * - Sleep disorders: norepinephrine abnormalities
 * 
 * @see Sara, S.J. (2009). The locus coeruleus and noradrenergic modulation
 *      of cognition. Nature Reviews Neuroscience, 10(3), 211-223.
 */
class Norepinephrine : public Neuromodulator {
public:
    /**
     * @brief Get norepinephrine name
     * 
     * @return "NE" (standard abbreviation)
     */
    const char* getName() const override { return "NE"; }
    
    /**
     * @brief Get current norepinephrine level
     * 
     * @return Current level (placeholder: 0.0f)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set norepinephrine level
     * 
     * @param level New norepinephrine level (placeholder: ignored)
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get norepinephrine plasticity factor
     * 
     * @return Plasticity factor (placeholder: 1.0f, no modulation)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update norepinephrine state
     * 
     * @param dt Time step (placeholder: ignored)
     */
    void update(TimestepDuration dt) override {}
};

/**
 * Serotonin: Mood regulation, impulse control, and social behavior
 * 
 * Biological basis:
 * - Produced in raphe nuclei brainstem system
 * - Projects to cortex, limbic system, and subcortical areas
 * - Regulates mood, appetite, and sleep
 * - Critical for impulse control and social behavior
 * 
 * Neuromodulatory effects:
 * - Modulates mood and emotional state
 * - Reduces impulsivity and aggression
 * - Enhances social behavior and bonding
 * - Regulates sleep-wake cycles
 * - Affects learning from punishment
 * 
 * Clinical relevance:
 * - Depression: serotonin deficiency
 * - Anxiety: serotonin dysregulation
 * - Impulsivity disorders: serotonin abnormalities
 * - OCD: serotonin circuit dysfunction
 * - Autism: serotonin system abnormalities
 * 
 * @see Jacobs, B.L., & Forstreuter, P. (2015). The role of serotonin in
 *      impulsive behavior. Neuropsychopharmacology, 40(1), 132-145.
 */
class Serotonin : public Neuromodulator {
public:
    /**
     * @brief Get serotonin name
     * 
     * @return "5-HT" (standard abbreviation for 5-hydroxytryptamine)
     */
    const char* getName() const override { return "5-HT"; }
    
    /**
     * @brief Get current serotonin level
     * 
     * @return Current level (placeholder: 0.0f)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set serotonin level
     * 
     * @param level New serotonin level (placeholder: ignored)
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get serotonin plasticity factor
     * 
     * @return Plasticity factor (placeholder: 1.0f, no modulation)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update serotonin state
     * 
     * @param dt Time step (placeholder: ignored)
     */
    void update(TimestepDuration dt) override {}
};

/**
 * Gamma-aminobutyric acid (GABA): Primary inhibitory neurotransmitter
 * 
 * Biological basis:
 * - Most abundant inhibitory neurotransmitter in brain
 * - Produced by interneurons throughout CNS
 * - Essential for preventing neural hyperexcitability
 * - Critical for sensory processing and motor control
 * 
 * Neuromodulatory effects:
 * - Reduces neuronal firing rates
 * - Inhibits neural synchronization
 * * - Sharpens sensory representations
 * - Prevents seizures and pathological activity
 * - Enables focused attention and cognitive control
 * 
 * Clinical relevance:
 * - Epilepsy: GABA deficiency
 * - Anxiety: GABA system dysfunction
 * - Sleep disorders: GABA abnormalities
 * - Schizophrenia: GABA interneuron deficits
 * 
 * @see Rubia, K., et al. (2014). GABA in schizophrenia: a systematic
 *      review and meta-analysis. Neuropsychopharmacology, 39(10), 2339-2352.
 */
class GABA : public Neuromodulator {
public:
    /**
     * @brief Get GABA name
     * 
     * @return "GABA" (standard abbreviation)
     */
    const char* getName() const override { return "GABA"; }
    
    /**
     * @brief Get current GABA level
     * 
     * @return Current level (placeholder: 0.0f)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set GABA level
     * 
     * @param level New GABA level (placeholder: ignored)
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get GABA plasticity factor
     * 
     * @return Plasticity factor (placeholder: 1.0f, no modulation)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update GABA state
     * 
     * @param dt Time step (placeholder: ignored)
     */
    void update(TimestepDuration dt) override {}
};

/**
 * Glutamate: Primary excitatory neurotransmitter and learning substrate
 * 
 * Biological basis:
 * - Most abundant excitatory neurotransmitter in brain
 * - Essential for synaptic plasticity and learning
 * - Mediates fast excitatory synaptic transmission
 * - Critical for memory formation and cognitive function
 * 
 * Neuromodulatory effects:
 * - Activates NMDA and AMPA receptors
 * - Triggers calcium influx and signaling cascades
 * - Induces long-term potentiation (LTP)
 * - Essential for memory consolidation
 * - Supports neural development and repair
 * 
 * Clinical relevance:
 * - Alzheimer's disease: glutamate excitotoxicity
 * - Stroke: glutamate excitotoxicity
 * - Depression: glutamate signaling abnormalities
 * - Anxiety: glutamate circuit dysfunction
 * 
 * @see Lisman, J.E., & Yasuda, R. (2013). The 10+ year journey of LTP.
 *      Molecular Brain, 6(1), 1-21.
 */
class Glutamate : public Neuromodulator {
public:
    /**
     * @brief Get glutamate name
     * 
     * @return "Glu" (standard abbreviation)
     */
    const char* getName() const override { return "Glu"; }
    
    /**
     * @brief Get current glutamate level
     * 
     * @return Current level (placeholder: 0.0f)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set glutamate level
     * 
     * @param level New glutamate level (placeholder: ignored)
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get glutamate plasticity factor
     * 
     * @return Plasticity factor (placeholder: 1.0f, no modulation)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update glutamate state
     * 
     * @param dt Time step (placeholder: ignored)
     */
    void update(TimestepDuration dt) override {}
};
