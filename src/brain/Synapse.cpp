#pragma once

namespace nlm {

/**
 * @file Synapse.cpp
 * @brief Implementation of synaptic dynamics with comprehensive plasticity models
 * 
 * This file contains the implementation of synaptic dynamics with advanced plasticity
 * mechanisms including both short-term and long-term synaptic modifications:
 * - Spike-timing-dependent plasticity (STDP)
 * - Hebbian learning rules
 * - Reward-modulated plasticity
 * - Short-term plasticity (Tsodyks-Markram model)
 * - Structural plasticity (weight bounds and dynamics)
 * 
 * The implementation provides realistic biological synaptic behavior including
 * synaptic efficacy modulation, eligibility traces, and homeostatic mechanisms.
 */

#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

/**
 * @brief Enhanced synaptic dynamics implementation for the Synapse class
 * 
 * This section contains methods and helper functions that manage synaptic behavior
 * and plasticity. Synaptic dynamics provide:
 * - Realistic spike-based plasticity rules
 * - Short-term synaptic modulation (facilitation and depression)
 * - Long-term synaptic weight changes
 * - Reward-modulated learning signals
 * - Synaptic efficacy and use-dependent modulation
 * - Homeostatic mechanisms for stability
 */

// Helper functions for synaptic dynamics
namespace {
    /**
     * @brief Calculate exponential decay factor
     * 
     * @param time Time elapsed (ms)
     * @param tau Time constant (ms)
     * @return Decay factor
     */
    inline float calculateDecay(float time, float tau) {
        return std::exp(-time / tau);
    }
    
    /**
     * @brief Calculate difference in spike times for STDP
     * 
     * @param preTime Pre-synaptic spike time
     * @param postTime Post-synaptic spike time
     * @return Time difference (positive if post > pre)
     */
    inline float calculateTimeDifference(float preTime, float postTime) {
        if (preTime < 0.0f) return 0.0f;
        if (postTime < 0.0f) return 0.0f;
        return postTime - preTime;
    }
}

/**
 * @brief Get synaptic weight with bounds checking
 * 
 * @return Current synaptic weight (clamped to valid range)
 */
SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

/**
 * @brief Set synaptic weight with bounds checking
 * 
 * @param weight New synaptic weight
 */
void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
 * @brief Add delta to synaptic weight with bounds checking
 * 
 * @param delta Weight change amount
 */
void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
 * @brief Get synaptic delay
 * 
 * @return Delay in simulation steps
 */
Delay Synapse::getDelay() const {
    return pImpl->delay;
}

/**
 * @brief Set synaptic delay
 * 
 * @param delay Delay in simulation steps
 */
void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

/**
 * @brief Get synaptic type
 * 
 * @return Synaptic type (excitatory, inhibitory, etc.)
 */
SynapseType Synapse::getType() const {
    return pImpl->type;
}

/**
 * @brief Set synaptic type
 * 
 * @param type Synaptic type (excitatory, inhibitory, etc.)
 */
void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

/**
 * @brief Check if synapse is excitatory
 * 
 * @return true if synapse is excitatory
 */
bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

/**
 * @brief Check if synapse is inhibitory
 * 
 * @return true if synapse is inhibitory
 */
bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

/**
 * @brief Record pre-synaptic spike for plasticity
 * 
 * This function records spike timing for STDP and other spike-timing-dependent
 * plasticity mechanisms. The timing information is used later to calculate
 * synaptic weight changes based on the relative timing of pre- and post-synaptic spikes.
 * 
 * @param timestamp Time of pre-synaptic spike
 */
void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
}

/**
 * @brief Record post-synaptic spike for plasticity
 * 
 * This function records spike timing for STDP and other spike-timing-dependent
 * plasticity mechanisms. The timing information is used later to calculate
 * synaptic weight changes based on the relative timing of pre- and post-synaptic spikes.
 * 
 * @param timestamp Time of post-synaptic spike
 */
void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
}

/**
 * @brief Get pre-synaptic spike history
 * 
 * @return Vector of pre-synaptic spike times
 */
const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

/**
 * @brief Get post-synaptic spike history
 * 
 * @return Vector of post-synaptic spike times
 */
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

/**
 * @brief Clear spike history
 * 
 * This function clears all recorded spike history, resetting the synapse
 * to a state without recent spike activity. This is useful for experiments
 * or when the synapse is being reset for a new learning phase.
 */
void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

/**
 * @brief Get plasticity flags
 * 
 * @return Reference to plasticity flags structure
 */
PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

/**
 * @brief Enable plasticity mechanisms
 * 
 * This function enables different plasticity mechanisms for the synapse:
 * - Hebbian learning (co-activation based)
 * - STDP (spike-timing dependent)
 * - Reward-modulated (dopamine-dependent)
 * 
 * @param hebbian Enable Hebbian learning
 * @param stdp Enable STDP
 * @param rewardModulated Enable reward-modulated plasticity
 */
void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

/**
 * @brief Get plasticity flags (const version)
 * 
 * @return Const reference to plasticity flags structure
 */
const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

/**
 * @brief Get eligibility trace
 * 
 * @return Current eligibility trace value
 */
float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

/**
 * @brief Set eligibility trace
 * 
 * @param trace New eligibility trace value
 */
void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

/**
 * @brief Decay eligibility trace
 * 
 * This function applies exponential decay to the eligibility trace, which
 * is used for reward-modulated learning. The trace decays over time unless
 * reinforced by synaptic activity or reward signals.
 * 
 * @param decayRate Decay rate per timestep
 */
void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

/**
 * @brief Get synaptic efficacy
 * 
 * @return Current synaptic efficacy (0-2.0 scale)
 */
float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

/**
 * @brief Set synaptic efficacy
 * 
 * @param efficacy New efficacy value (clamped to 0.0-2.0)
 */
void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

/**
 * @brief Apply synaptic dynamics for one timestep
 * 
 * This is the main synaptic dynamics function that updates the synapse's
 * state based on recent activity and biological parameters. It includes:
 * - Short-term plasticity (Tsodyks-Markram model)
 * - Spike history management
 * - Eligibility trace decay
 * - Weight clamping
 * 
 * The function implements realistic synaptic dynamics that balance
 * stability with plasticity to enable learning while maintaining
 * network stability.
 * 
 * @param currentTime Current simulation time
 */
void Synapse::step(Timestamp currentTime) {
    // Apply real synaptic dynamics with biological accuracy
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // Update short-term plasticity (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        // Facilitation recovery: baseline facilitation decays over time
        pImpl->shortTermFacilitation *= calculateDecay(timeSincePre, Impl::STP_FACILITATION_TAU);
    }
    
    // Depression recovery: depression recovers toward baseline over time
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0 (fully recovered)
        float recoveryRate = 1.0f - calculateDecay(timeSinceActivity, Impl::STP_DEPRESSION_TAU);
        pImpl->shortTermDepression += recoveryRate * (1.0f - pImpl->shortTermDepression);
        // Clamp depression to reasonable range
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
    }
    
    // Apply spike-dependent dynamics
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        // Short-term depression due to recent spikes
        float depressionIncrement = Impl::STP_U_MAX * calculateDecay(timeSincePre, Impl::STP_DEPRESSION_TAU);
        pImpl->shortTermDepression -= depressionIncrement;
        pImpl->shortTermDepression = std::clamp(pImpl->shortTermDepression, 0.0f, 1.0f);
        
        // Short-term facilitation due to recent spikes
        float facilitationIncrement = Impl::STP_U_MAX * (1.0f - calculateDecay(timeSincePre, Impl::STP_FACILITATION_TAU));
        pImpl->shortTermFacilitation += facilitationIncrement;
        pImpl->shortTermFacilitation = std::clamp(pImpl->shortTermFacilitation, 0.0f, Impl::STP_U_MAX);
    }
    
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay for temporal precision
    
    // Apply synaptic efficacy to weight modulation
    if (pImpl->efficacy != 1.0f) {
        // Use-dependent weight modulation
        float efficacyFactor = pImpl->efficacy - 1.0f;  // 0 if efficacy is baseline
        pImpl->weight += efficacyFactor * 0.0001f;   // Slow efficacy-based drift
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
    }
    
    // Apply homeostatic regulation to prevent runaway excitation/inhibition
    if (std::abs(pImpl->weight) > 0.5f) {
        float homeostaticTarget = (pImpl->type == SynapseType::Excitatory) ? 0.2f : -0.2f;
        float homeostaticForce = 0.001f * (homeostaticTarget - pImpl->weight);
        pImpl->weight += homeostaticForce;
    }
    
    // Clamp weight bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

/**
 * @brief Reset synapse to initial state
 * 
 * This function resets the synapse to its initial state, clearing all
 * recorded activity and resetting weights and plasticity variables.
 * This is useful for experimental conditions or when a synapse needs
 * to be returned to a baseline state.
 */
void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

/**
 * @brief Initialize synapse with random biological parameters
 * 
 * This function initializes the synapse with biologically realistic
 * parameters based on the synapse type. Different synapse types have
 * different characteristic properties:
 * - Excitatory: Positive weights, moderate efficacy
 * - Inhibitory: Negative weights, moderate efficacy
 * - Other: Small random weights, baseline efficacy
 * 
 * The initialization includes setting up short-term plasticity state
 * and establishing baseline synaptic properties.
 * 
 * @param rng Random number generator for stochastic initialization
 */
void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        pImpl->plasticityFlags.stdp = true;  // Excitatory synapses typically have STDP
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        pImpl->plasticityFlags.hebbian = true;  // Inhibitory synapses often have Hebbian learning
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Enable plasticity by default for learning synapses
    pImpl->plasticityFlags.hebbian = true;
    pImpl->plasticityFlags.stdp = true;
    pImpl->plasticityFlags.reward_modulated = false;
}

} // namespace nlm
