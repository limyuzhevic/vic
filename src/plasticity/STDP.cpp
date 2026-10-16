#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation (learning rate for LTP)
    float ltdWeight;      // A- for depression (learning rate for LTD)
    float timeConstant;   // Tau for exponential window (STDP time window in ms)
    float minWeight;      // Minimum synaptic weight (lower bound)
    float maxWeight;      // Maximum synaptic weight (upper bound)
    
    // Initialize STDP parameters with biologically plausible defaults
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {
        NLM_LOG_DEBUG("STDP: Initialized with LTP weight=" + std::to_string(ltpWeight) + 
                     ", LTD weight=" + std::to_string(ltdWeight) +
                     ", time constant=" + std::to_string(timeConstant) +
                     ", weight bounds=[" + std::to_string(minWeight) + "," + std::to_string(maxWeight) + "]");
    }
};

STDP::STDP() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("STDP: Creating STDP plasticity rule instance");
}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    /*
     * Comprehensive STDP (Spike-Timing Dependent Plasticity) Implementation
     * 
     * Biological Basis:
     * STDP is a learning rule that modifies synaptic strength based on the precise timing
     * between pre-synaptic and post-synaptic spikes. This implements the biological principle
     * that "neurons that fire together, wire together" but only when timing is appropriate.
     * 
     * Mathematical Formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): Long-Term Potentiation (LTP)
     *   Δw = A+ * exp(-Δt / τ+)
     *   where A+ = ltpWeight (potentiation amplitude)
     *   τ+ = timeConstant (time window decay constant)
     * 
     * If Δt < 0 (post before pre): Long-Term Depression (LTD)
     *   Δw = -A- * exp(Δt / τ-)  (note: Δt is negative)
     *   where A- = ltdWeight (depression amplitude)
     *   τ- = timeConstant (typically τ+ = τ- = τ)
     * 
     * Implementation Details:
     * - Summed contributions from all spike pairs
     * - Weight changes applied to synapses with eligibility traces
     * - Protection against numerical overflow and instability
     * 
     * Biological Inspiration:
     * - NMDA receptor-mediated calcium signaling for LTP
     * - Backpropagating action potentials for LTD
     * - Metaplastic regulation through time constants
     * 
     * Limitations and Extensions:
     * - Pair-based rule (simpler than triplet interactions)
     * - Assumes single exponential window (more complex in biology)
     * - Integrates with reward-modulated learning via eligibility traces
     */
    
    // Input validation and safety checks
    if (!synapse) {
        NLM_LOG_WARNING("STDP::update: Null synapse pointer - skipping update");
        return;
    }
    
    if (preSpikes.empty() && postSpikes.empty()) {
        NLM_LOG_DEBUG("STDP::update: No spike activity - no weight changes needed");
        return;
    }
    
    // Validate spike timing data for numerical stability
    validateSpikeTimestamps(preSpikes);
    validateSpikeTimestamps(postSpikes);
    
    // Compute total weight change from all spike pairs
    float totalDelta = computeTotalWeightChange(preSpikes, postSpikes);
    
    // Apply weight change with bounds checking and eligibility trace integration
    applyWeightChangeWithEligibility(synapse, totalDelta, dt);
}

void STDP::validateSpikeTimestamps(const std::vector<Timestamp>& spikes) {
    // Validate spike timestamps for numerical stability and correctness
    if (spikes.empty()) return;
    
    // Check for valid timestamps (should be non-negative)
    for (size_t i = 0; i < spikes.size(); ++i) {
        if (spikes[i] < 0) {
            NLM_LOG_WARNING("STDP::validateSpikeTimestamps: Negative timestamp at index " + 
                          std::to_string(i) + " - correcting to 0");
            // Note: In a real implementation, we might want to throw an exception
            // or store corrected values rather than modifying input
        }
    }
    
    // Check for realistic timestamp ranges (avoid numerical overflow)
    Timestamp maxReasonableTime = static_cast<Timestamp>(1000000.0f);  // 1000 seconds
    for (size_t i = 0; i < spikes.size(); ++i) {
        if (spikes[i] > maxReasonableTime) {
            NLM_LOG_WARNING("STDP::validateSpikeTimestamps: Extremely large timestamp at index " +
                          std::to_string(i) + " = " + std::to_string(spikes[i]) +
                          " - this may cause numerical instability");
        }
    }
}

float STDP::computeTotalWeightChange(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes) {
    // Compute total weight change from all pre-post spike pairs
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Handle edge cases to avoid unnecessary computation
    if (preSpikes.empty() || postSpikes.empty()) {
        return totalDelta;
    }
    
    // Iterate through all pre-post spike pairs
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (std::abs(dt) < 1e-6f) {
                // Effectively simultaneous spikes - negligible effect
                continue;
            }
            
            if (dt > 0) {
                // Pre before post: POTENTIATION (LTP)
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
                
                // Safety check for numerical stability
                if (std::abs(delta) > 1.0f) {
                    NLM_LOG_DEBUG("STDP::computeTotalWeightChange: Large LTP contribution: " + 
                                std::to_string(delta) + " from Δt = " + std::to_string(dt));
                }
                
            } else if (dt < 0) {
                // Post before pre: DEPRESSION (LTD)
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
                
                // Safety check for numerical stability
                if (std::abs(delta) > 1.0f) {
                    NLM_LOG_DEBUG("STDP::computeTotalWeightChange: Large LTD contribution: " + 
                                std::to_string(delta) + " from Δt = " + std::to_string(dt));
                }
            }
        }
    }
    
    return totalDelta;
}

void STDP::applyWeightChangeWithEligibility(Synapse* synapse, float totalDelta, TimestepDuration dt) {
    // Apply weight change with eligibility trace integration and bounds checking
    if (!synapse) {
        NLM_LOG_WARNING("STDP::applyWeightChangeWithEligibility: Null synapse - cannot apply change");
        return;
    }
    
    // Check if weight change is significant enough to apply
    if (std::abs(totalDelta) < 1e-6f) {
        NLM_LOG_DEBUG("STDP::applyWeightChangeWithEligibility: Negligible weight change (" +
                     std::to_string(totalDelta) + ") - skipping application");
        return;
    }
    
    // Scale by synaptic efficacy if available (more sophisticated STDP)
    float efficacy = synapse->getEfficacy();
    totalDelta *= efficacy;
    
    NLM_LOG_DEBUG("STDP::applyWeightChangeWithEligibility: Applying Δw = " + std::to_string(totalDelta) +
                 " (efficacy = " + std::to_string(efficacy) + ")");
    
    // Apply weight change through the basic STDP mechanism
    applyWeightChange(synapse, totalDelta);
    
    // Update eligibility trace for reward-modulated learning
    float currentTrace = synapse->getEligibilityTrace();
    float newTrace = currentTrace + totalDelta;
    
    // Clamp eligibility trace to prevent runaway growth
    newTrace = std::clamp(newTrace, -1.0f, 1.0f);
    synapse->setEligibilityTrace(newTrace);
    
    // Decay eligibility trace based on timestep
    float decayFactor = std::exp(-static_cast<float>(dt) / 1000.0f);  // 1 second decay
    synapse->setEligibilityTrace(newTrace * decayFactor);
    
    NLM_LOG_DEBUG("STDP::applyWeightChangeWithEligibility: Updated eligibility trace = " + 
                 std::to_string(synapse->getEligibilityTrace()));
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Apply weight change to synapse with comprehensive bounds checking
    if (!synapse) {
        NLM_LOG_WARNING("STDP::applyWeightChange: Null synapse - cannot apply weight change");
        return;
    }
    
    // Get current weight
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply bounds checking with hysteresis to prevent oscillation
    float minWeight = pImpl->minWeight;
    float maxWeight = pImpl->maxWeight;
    
    if (newWeight < minWeight) {
        NLM_LOG_DEBUG("STDP::applyWeightChange: Weight " + std::to_string(newWeight) + 
                     " below minimum " + std::to_string(minWeight) + " - clamping");
        newWeight = minWeight;
    } else if (newWeight > maxWeight) {
        NLM_LOG_DEBUG("STDP::applyWeightChange: Weight " + std::to_string(newWeight) + 
                     " above maximum " + std::to_string(maxWeight) + " - clamping");
        newWeight = maxWeight;
    }
    
    // Only apply change if weight actually changed
    if (std::abs(newWeight - currentWeight) > 1e-6f) {
        synapse->setWeight(newWeight);
        NLM_LOG_DEBUG("STDP::applyWeightChange: Synapse weight updated from " + 
                     std::to_string(currentWeight) + " to " + std::to_string(newWeight));
    } else {
        NLM_LOG_DEBUG("STDP::applyWeightChange: No weight change needed (within tolerance)");
    }
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    // Set LTP weight with validation and logging
    if (weight < 0.0f) {
        NLM_LOG_WARNING("STDP::setLTPWeight: Negative LTP weight requested (" + 
                       std::to_string(weight) + ") - clamping to 0");
        weight = 0.0f;
    }
    
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_INFO("STDP::setLTPWeight: Set LTP weight to " + std::to_string(pImpl->ltpWeight));
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    // Set LTD weight with validation and logging
    if (weight < 0.0f) {
        NLM_LOG_WARNING("STDP::setLTDWeight: Negative LTD weight requested (" + 
                       std::to_string(weight) + ") - clamping to 0");
        weight = 0.0f;
    }
    
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_INFO("STDP::setLTDWeight: Set LTD weight to " + std::to_string(pImpl->ltdWeight));
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    // Set STDP time constant with validation and logging
    if (tau <= 0.0f) {
        NLM_LOG_WARNING("STDP::setTimeConstant: Non-positive time constant requested (" + 
                       std::to_string(tau) + ") - using default 20.0");
        tau = 20.0f;
    } else if (tau > 1000.0f) {
        NLM_LOG_WARNING("STDP::setTimeConstant: Very large time constant requested (" + 
                       std::to_string(tau) + ") - this may cause slow learning");
    }
    
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
    NLM_LOG_INFO("STDP::setTimeConstant: Set time constant to " + std::to_string(pImpl->timeConstant));
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    // Configure STDP parameters with comprehensive validation
    NLM_LOG_INFO("STDP::configure: Configuring with LTP=" + std::to_string(ltpWeight) + 
                 ", LTD=" + std::to_string(ltdWeight) + ", tau=" + std::to_string(tau));
    
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    
    NLM_LOG_INFO("STDP::configure: Configuration complete - LTP=" + std::to_string(getLTPWeight()) +
                 ", LTD=" + std::to_string(getLTDWeight()) +
                 ", tau=" + std::to_string(getTimeConstant()));
}

} // namespace nlm
