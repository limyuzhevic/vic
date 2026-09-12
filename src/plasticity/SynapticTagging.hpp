#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @class SynapticTagging
 * @brief Synaptic tagging and capture for late-phase LTP/LTD
 * 
 * SynapticTagging implements a mechanism for tagging synapses to capture plasticity
 * signals for late-phase long-term potentiation (LTP) and long-term depression (LTD).
 * This rule addresses the temporal distinction between early-phase (protein-synthesis-independent)
 * and late-phase (protein-synthesis-dependent) synaptic modifications, allowing synapses
 * to "tag" themselves for future strengthening or weakening based on accumulated activity.
 * 
 * The synaptic tagging and capture (STC) mechanism:
 * 1. Activity-dependent tag formation at synapses
 * 2. Capture of plasticity-related proteins (PRPs) by tagged synapses
 * 3. Late-phase LTP/LTD consolidation
 * 
 * Tag dynamics:
 * dT/dt = -T/τ_t + Σδ(t - t_spike)
 * τ_t = tag decay time constant
 * 
 * Late-phase LTP/LTD:
 * Δw_late = k * T * PRP
 * PRP = plasticity-related protein concentration
 * 
 * Biological inspiration:
 *   - Late-phase LTP/LTD requires protein synthesis
 *   - Synaptic tags determine which synapses receive PRP
 *   - Combines with neuromodulatory signals for specificity
 *   - Essential for memory consolidation
 * 
 * Applications:
 *   - Long-term memory formation
 *   - Synaptic specificity in learning
 *   - Persistent synaptic changes
 *   - Integration with neuromodulatory systems
 * 
 * @note This rule provides the foundation for lasting synaptic modifications
 *       beyond the immediate early-phase effects.
 * @note Thread safety is ensured for all public methods.
 * 
 * @see EligibilityTrace
 * @see HomeostaticPlasticity
 * @see NeuromodulatorPlasticity
 * @see PlasticityRule
 * 
 * @ingroup plasticity
 */

class SynapticTagging : public PlasticityRule {
public:
    /**
     * @brief Constructs a SynapticTagging instance with default parameters
     * 
     * Creates a synaptic tagging plasticity rule with default parameters for
     * late-phase LTP/LTD. The default parameters provide a balanced initial
     * condition for synaptic tagging and capture dynamics.
     * 
     * @note The actual implementation details are hidden by the PImpl idiom.
     *       All parameter settings are forwarded to the implementation class.
     * 
     * @see configure
     * @see setTagDecayTimeConstant
     * @see setTagCaptureThreshold
     */
    SynapticTagging();
    
    /**
     * @brief Virtual destructor
     * 
     * Cleans up SynapticTagging resources and performs finalization of
     * synaptic tagging and capture algorithms.
     */
    ~SynapticTagging() override;
    
    /**
     * @brief Update synaptic tags based on spike timing
     * 
     * Implements the synaptic tagging dynamics by computing tag formation
     * from pre- and post-synaptic spike pairs and updating the tag value.
     * This method integrates spike timing information into the synaptic tag
     * without immediately applying weight changes, preparing for late-phase
     * potentiation or depression.
     * 
     * @param synapse Pointer to the synapse whose tag will be updated.
     *               Must be a valid synapse pointer within the neural network.
     * @param preSpikes Vector of pre-synaptic spike times in milliseconds.
     *                 Contains timestamps of all spikes that occurred at the
     *                 pre-synaptic neuron since the last plasticity update.
     *                 Sorted in chronological order.
     * @param postSpikes Vector of post-synaptic spike times in milliseconds.
     *                  Contains timestamps of all spikes that occurred at the
     *                 post-synaptic neuron since the last plasticity update.
     *                 Sorted in chronological order.
     * @param dt Simulation timestep duration in milliseconds. Used for
     *           numerical integration of the tag dynamics differential equation.
     * 
     * @note The tag evolves according to: dT/dt = -T/τ_t + Σδ(t - t_spike)
     *       where tags are formed at spike times and decay exponentially.
     * @note This method is thread-safe and can be called concurrently for
     *       different synapses.
     * @note The tag value is maintained internally and can be queried independently.
     * 
     * @see applyWeightChange
     * @see getTagValue
     * @see captureWithPRPs
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply late-phase weight change using captured tags
     * 
     * Applies synaptic weight changes based on the current tag value and
     * available plasticity-related proteins (PRPs). This method finalizes the
     * learning process by combining stored tag information with PRP concentration
     * to produce late-phase LTP/LTD effects.
     * 
     * @param synapse Pointer to the synapse to modify.
     * @param delta Weight change amount, typically computed as k * T * PRP where
     *             T is the tag value and PRP is the plasticity-related protein concentration.
     * 
     * @note The actual capture of PRPs is typically handled by captureWithPRPs().
     *       This method is called internally by captureWithPRPs().
     * 
     * @see captureWithPRPs
     * @see getTagValue
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get the human-readable name of the synaptic tagging rule
     * 
     * Returns a descriptive string identifying this as a synaptic tagging
     * plasticity rule. This is useful for configuration, logging, and debugging.
     * 
     * @return Const char pointer to the string "SynapticTagging".
     * 
     * @see update
     * @see applyWeightChange
     */
    const char* getName() const override;
    
    /**
     * @brief Set the synaptic tag decay time constant
     * 
     * Configures the time constant that controls how quickly synaptic tags
     * decay in the absence of spike activity. This parameter determines how long
     * synapses remain "tagged" for potential late-phase modifications.
     * 
     * @param tau Decay time constant in milliseconds. Typical biological
     *           values range from 10 minutes to 24 hours. Smaller values produce
     *           shorter tag durations, larger values produce longer durations.
     * 
     * @note The decay time constant appears in the differential equation:
     *       dT/dt = -T/τ_t + Σδ(t - t_spike)
     * @note This parameter is crucial for determining the credit assignment
     *       window for late-phase LTP/LTD.
     * 
     * @see getTagDecayTimeConstant
     * @see configure
     */
    void setTagDecayTimeConstant(float tau);    // Tag decay time constant (ms)
    
    /**
     * @brief Get the current synaptic tag decay time constant
     * 
     * Returns the current tag decay time constant that controls the temporal
     * dynamics of synaptic tags.
     * 
     * @return Current tag decay time constant in milliseconds (positive float value).
     * 
     * @see setTagDecayTimeConstant
     */
    float getTagDecayTimeConstant() const;
    
    /**
     * @brief Set the tag capture threshold
     * 
     * Configures the threshold value that determines when a synapse can capture
     * plasticity-related proteins (PRPs). Synapses with tag values above this
     * threshold can undergo late-phase LTP/LTD.
     * 
     * @param threshold Capture threshold. Higher values make tagging more selective.
     *                  Typical values range from 1.0 to 10.0.
     * 
     * @note This threshold determines which tagged synapses actually capture PRPs
     *       and undergo late-phase modifications.
     * @note This parameter adds specificity to the synaptic tagging mechanism.
     * 
     * @see getTagCaptureThreshold
     */
    void setTagCaptureThreshold(float threshold);
    
    /**
     * @brief Get the current tag capture threshold
     * 
     * Returns the current capture threshold that determines when synapses can
     * capture plasticity-related proteins.
     * 
     * @return Current capture threshold (positive float value).
     * 
     * @see setTagCaptureThreshold
     */
    float getTagCaptureThreshold() const;
    
    /**
     * @brief Get the current synaptic tag value
     * 
     * Returns the current synaptic tag value for the associated synapse.
     * This value represents the accumulated tagging that determines eligibility
     * for late-phase LTP/LTD when PRPs are available.
     * 
     * @return Current synaptic tag value.
     * 
     * @note This method is primarily for debugging and monitoring purposes.
     * @note The actual tag values are maintained per-synapse in the
     *       implementation layer for performance reasons.
     */
    float getTagValue() const;
    
    /**
     * @brief Capture plasticity-related proteins (PRPs) for late-phase modifications
     * 
     * Simulates the availability of plasticity-related proteins and captures them
     * on synapses that exceed the capture threshold. This method triggers late-phase
     * LTP/LTD effects on tagged synapses.
     * 
     * @param synapse Pointer to the synapse whose late-phase modification will be captured.
     * @param prpConcentration Plasticity-related protein concentration.
     *                        Higher values produce stronger late-phase effects.
     * @param neuromodulatorSignal Optional neuromodulatory signal that modulates PRP availability.
     *                           Can be null if no neuromodulation is applied.
     * 
     * @note This is the key mechanism for converting tags into late-phase
     *       synaptic modifications when PRPs are available.
     * @note PRPs are typically released in response to strong or repeated activity.
     */
    void captureWithPRPs(Synapse* synapse, float prpConcentration, float neuromodulatorSignal = 1.0f);
    
    /**
     * @brief Configure all synaptic tagging parameters in a single call
     * 
     * Convenience method to set all synaptic tagging parameters (tag decay time
     * constant and capture threshold) simultaneously. This ensures consistent
     * parameter configuration and reduces the number of separate setter calls.
     * 
     * @param decayTimeConstant Tag decay time constant in milliseconds.
     * @param captureThreshold Capture threshold value.
     * 
     * @note All parameters must have valid values before the configuration is applied.
     * @note This method is thread-safe and provides atomic configuration of
     *       all parameters.
     */
    void configure(float decayTimeConstant, float captureThreshold);
    
private:
    /**
     * @brief PImpl implementation structure for SynapticTagging
     * 
     * Internal implementation details for synaptic tagging are hidden by the
     * PImpl idiom. This structure contains the actual algorithmic implementation
     * of the synaptic tagging and capture mechanism for late-phase LTP/LTD.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the SynapticTagging implementation
     * 
     * Forward declaration of the implementation pointer used by the PImpl idiom
     * to hide the concrete implementation of SynapticTagging from the header file.
     * This improves compilation times and encapsulation.
     */
    Impl* pImpl;
};

} // namespace nlm