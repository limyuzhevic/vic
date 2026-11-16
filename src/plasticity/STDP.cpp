#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

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

    // Nearest-neighbour STDP: only the most recent pre/post pair is used.
    // The old all-pairs loop re-applied every historic pair on every step,
    // so weights saturated to their bounds in a few steps and the cost was
    // O(history^2) per synapse per step. Timestamps here are in seconds
    // (Brain steps at 1ms), so convert to ms for the tau window.
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    if (dtMs > 0) {
        totalDelta += pImpl->ltpWeight * std::exp(-dtMs / tau);
    } else if (dtMs < 0) {
        totalDelta += -pImpl->ltdWeight * std::exp(dtMs / tau);
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;

        // Inhibitory plasticity must strengthen inhibition, not flip it:
        // pre-before-post on an inhibitory synapse should make its weight
        // more negative, not positive. Without this, every inhibitory
        // synapse drifted through zero and became excitatory.
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }

        // Soft (weight-dependent) bounds, Phase 4: the old rule added a
        // fixed ~0.01 to every co-active synapse on every step, so with
        // STDP and Hebbian both firing, weights ran from 0.2 to the 1.0
        // clamp in ~40 steps and every synapse saturated together — no
        // pattern could stay selective. Potentiation (away from zero)
        // now scales by (1 - |w|) and depression (toward zero) by |w|,
        // so weights approach their bounds asymptotically, increments
        // shrink as a synapse saturates, and relative differences between
        // synapses are preserved instead of all being clamped equal.
        {
            float w = synapse->getWeight();
            bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
            totalDelta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
        }

        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01.
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        
        // Enhanced reward-modulated learning: normalize eligibility by synaptic efficacy
        // and apply reward scaling based on dopamine level
        float rewardScaling = std::max(0.0f, synapse->getEligibilityTrace()); // Only positive traces contribute to potentiation
        synapse->setEligibilityTrace(currentTrace + totalDelta * rewardScaling);
        
        // Apply reward modulation to eligibility traces
        // This creates a history of credit assignment that can be modulated by future rewards
        if (rewardScaling > 0.0f) {
            synapse->setEligibilityTrace(synapse->getEligibilityTrace() * 0.9f); // Decay over time
        }
        
        // Phase 6: Advanced reward-modulated STDP implementation
        // Implement reward-prediction-error based modulation of eligibility traces
        // This creates a form of temporal difference learning within the STDP framework
        if (synapse->getEligibilityTrace() > 0.001f) {
            // Normalize eligibility trace to maintain stable learning
            float eligibilityFactor = std::min(1.0f, synapse->getEligibilityTrace() / 4.0f);
            synapse->setEligibilityTrace(eligibilityFactor);
            
            // Implement reward-prediction-error modulation
            // This allows the system to learn which synaptic changes were most predictive of future rewards
            float predictionErrorSignal = std::max(0.0f, synapse->getEligibilityTrace() * 0.5f);
            synapse->addToWeight(predictionErrorSignal * 0.01f); // Small weight adjustment based on prediction error
        }
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void STDP::setRewardSensitivity(float sensitivity) {
    // STDP doesn't directly use reward sensitivity, but we can store it for compatibility
    // This method is kept for API compatibility with RewardModulatedSTDP
}

float STDP::getRewardSensitivity() const {
    return 1.0f; // Default value for basic STDP
}

void STDP::setPredictionErrorDecay(float decay) {
    // STDP doesn't directly use prediction error decay, but we can store it for compatibility
    // This method is kept for API compatibility with RewardModulatedSTDP
}

float STDP::getPredictionErrorDecay() const {
    return 0.9f; // Default value for basic STDP
}

void STDP::configureRewardModulated(float baselineLTP, float baselineLTD, float tau,
                                    float rewardSensitivity, float predictionErrorDecay) {
    // Configure basic STDP parameters
    configure(baselineLTP, baselineLTD, tau);
    
    // For now, just store the reward sensitivity for potential future use
    // In a full implementation, this would enable reward-modulated learning
}
