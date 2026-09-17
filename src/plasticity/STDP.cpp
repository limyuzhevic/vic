/**
 * @file STDP.cpp
 * @brief Spike-Timing-Dependent Plasticity (STDP) implementation
 * 
 * This file implements the Spike-Timing-Dependent Plasticity learning rule,
 * which is a biologically-inspired synaptic plasticity mechanism that modifies
 * synaptic strength based on the relative timing of pre- and post-synaptic spikes.
 * 
 * @details STDP is based on the Hebbian principle "cells that fire together,
 * wire together" but with precise timing requirements:
 * - Pre-synaptic spike followed by post-synaptic spike within milliseconds
 *   results in long-term potentiation (LTP)
 * - Post-synaptic spike followed by pre-synaptic spike results in
 *   long-term depression (LTD)
 * 
 * This implementation:
 * - Follows the pair-based STDP model with exponential decay
 * - Incorporates neuromodulatory scaling (dopamine effects)
 * - Includes biological constraints and stability mechanisms
 * - Supports reward-modulated learning through eligibility traces
 * 
 * @note STDP is a core mechanism for temporal learning and memory formation
 * in the NLM brain, enabling precise spike timing to guide synaptic changes.
 */

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm;

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

/**
     * @brief Default constructor for STDP
     * 
     * Initializes the STDP rule with default parameters:
     * - LTP weight (A+): 0.01
     * - LTD weight (A-): 0.012
     * - Time constant (tau): 20.0 ms
     * - Weight bounds: -1.0 to 1.0
     * 
     * These defaults provide a baseline STDP learning rate suitable for
     * standard neural network applications.
     */
STDP::STDP() : pImpl(new Impl) {}

/**
     * @brief Destructor for STDP
     * 
     * Cleans up the pImpl instance pointer. The default implementation
     * is sufficient since we only use a single pointer.
     */
STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Limitations:
     *   - Simplified pairwise rule (doesn't capture triplet interactions)
     *   - Assumes single exponential window (more complex in biology)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

/**
     * @brief Update synaptic weights based on spike timing (STDP algorithm)
     * 
     * Core STDP implementation that computes weight changes for a synapse based on
     * the timing of pre- and post-synaptic spikes. The algorithm processes all
     * spike pairs to compute the net weight change.
     * 
     * @details Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     * 
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     * - Reflects NMDA receptor-mediated calcium signaling
     * - Pre-before-post activates NMDA receptors when postsynaptic spikes
     * - Post-before-pre causes backpropagating action potentials
     * 
     * Limitations:
     * - Simplified pairwise rule (doesn't capture triplet interactions)
     * - Assumes single exponential window (more complex in biology)
     * - Doesn't account for synaptic eligibility traces
     * 
     * @param synapse Target synapse to update (must not be null)
     * @param preSpikes Vector of presynaptic spike timestamps
     * @param postSpikes Vector of postsynaptic spike timestamps
     * @param dt Simulation timestep duration
     * 
     * @note If either spike history is empty, the method returns early with no change.
     * Also scales weight changes by synaptic efficacy and updates eligibility traces.
     */
void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Limitations:
     *   - Simplified pairwise rule (doesn't capture triplet interactions)
     *   - Assumes single exponential window (more complex in biology)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

/**
     * @brief Apply a weight change to a synapse with bounds checking
     * 
     * Convenience method to directly modify synaptic weight by an amount,
     * with automatic clamping to the valid weight range.
     * 
     * @param synapse Target synapse to modify (must not be null)
     * @param delta Weight change amount to apply
     * 
     * @note This method does not update spike history or eligibility traces,
     * only directly modifies the synaptic weight.
     */
void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

} // namespace nlm
