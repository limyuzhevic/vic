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

// Reward-modulated STDP (R-STDP) implementation
// Enhanced STDP with dopamine-based neuromodulation for reward learning

struct STDP::RewardModulatedSTDP::Impl {
    float ltpWeight;          // A+ for potentiation
    float ltdWeight;          // A- for depression
    float timeConstant;       // Tau for exponential window (ms)
    float dopamineFactor;     // Scales STDP with dopamine signal
    float neuromodulationGain; // Modulation strength for reward signals
    float eligibilityDecay;   // Trace decay rate
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             dopamineFactor(1.0f), neuromodulationGain(1.0f),
             eligibilityDecay(0.001f),  // 0.1% decay per ms
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

STDP::RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void STDP::RewardModulatedSTDP::update(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP) implementation
     * 
     * This is an enhanced version of STDP that incorporates neuromodulatory
     * signals (like dopamine) to modulate synaptic learning based on reward
     * prediction errors. This enables the brain to learn which neural
     * activity patterns lead to rewarded outcomes.
     * 
     * Key features:
     * 1. Eligibility traces: Store synaptic changes until reward signals arrive
     * 2. Neuromodulation: Dopamine signals scale STDP weight changes
     * 3. Reward prediction errors: Integrate reward signals with expected outcomes
     * 4. Credit assignment: Assign credit to specific neural activity patterns
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Nearest-neighbour STDP: only the most recent pre/post pair is used.
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000./le:
    if (dtMs > 0) {
        totalDelta += pImpl->ltpWeight * std::exp(-dtMs / tau);
    } else if (dtMs < 0) {
        totalDelta += -pImpl->ltdWeight * std::exp(dtMs / tau);
    }
    
    // Scale by neuromodulatory signals
    // Dopamine acts as a global reward signal that modulates synaptic learning
    float neuromodulation = pImpl->dopamineFactor;  // In real systems, this comes from dopamine neurons
    totalDelta *= neuromodulation;
    
    // Apply neuromodulation gain for more complex reward processing
    totalDelta *= pImpl->neuromodulationGain;
    
    // Update eligibility trace
    float currentTrace = synapse->getEligibilityTrace();
    float eligibilityIncrement = totalDelta * dtMs * 0.01f;  // Convert to eligibility
    currentTrace += eligibilityIncrement;
    
    // Decay eligibility trace over time
    currentTrace *= (1.0f - pImpl->eligibilityDecay * dtMs);
    synapse->setEligibilityTrace(currentTrace);
    
    // Store the trace for later reward-based weight updates
    synapse->setPreSpikeHistory(preSpikes);
    synapse->setPostSpikeHistory(postSpikes);
}

void STDP::RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply stored eligibility trace when reward signal arrives
    float eligibility = synapse->getEligibilityTrace();
    
    // Neuromodulation: reward signals scale the eligibility trace
    float rewardSignal = 1.0f;  // In real systems, this comes from dopamine release
    float modulatedChange = eligibility * rewardSignal;
    
    // Apply the modulated weight change
    float newWeight = synapse->getWeight() + modulatedChange;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
    
    // Clear eligibility trace after reward processing
    synapse->setEligibilityTrace(0.0f);
    
    // Reset spike histories after reward processing
    synapse->clearHistory();
}

void STDP::RewardModulatedSTDP::setDopamineFactor(float factor) {
    pImpl->dopamineFactor = std::clamp(factor, 0.0f, 10.0f);
}

float STDP::RewardModulatedSTDP::getDopamineFactor() const {
    return pImpl->dopamineFactor;
}

void STDP::RewardModulatedSTDP::setNeuromodulationGain(float gain) {
    pImpl->neuromodulationGain = std::clamp(gain, 0.0f, 5.0f);
}

float STDP::RewardModulatedSTDP::getNeuromodulationGain() const {
    return pImpl->neuromodulationGain;
}

void STDP::RewardModulatedSTDP::setEligibilityTraceDecay(float decayRate) {
    pImpl->eligibilityDecay = std::clamp(decayRate, 0.0001f, 0.1f);
}

float STDP::RewardModulatedSTDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityDecay;
}

void STDP::RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau,
                                         float dopamineFactor, float neuromodulationGain) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setDopamineFactor(dopamineFactor);
    setNeuromodulationGain(neuromodulationGain);
}

// Triplet STDP implementation
// Enhanced STDP that captures residual calcium dynamics and multiple spike interactions

struct STDP::TripletSTDP::Impl {
    float umax;               // Maximum utilization for facilitation (Tsodyks-Markram model)
    float tauF;               // Facilitation time constant (ms)
    float tauD;               // Depression time constant (ms)
    float ltpWeight;          // A+ for potentiation
    float ltdWeight;          // A- for depression
    float timeConstant;       // STDP time constant (ms)
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    
    Impl() : umax(1.0f), tauF(100.0f), tauD(200.0f),
             ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1. famosasf(1.0f) {}
};

STDP::TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

STDP::TripletSTDP::~TripletSTDP() = default;

void STDP::TripletSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    /*
     * Triplet STDP implementation
     * 
     * Advanced STDP that captures residual calcium dynamics and multiple spike interactions.
     * Based on the Lancaster-Rueda triplet rule, which accounts for:
     * 1. Pre-post spike pairs (standard STDP)
     * 2. Pre-pre spike pairs (facilitation)
     * 3. Post-post spike pairs (depression)
     * 
     * Biological inspiration:
     * - Reflects residual calcium dynamics in dendritic compartments
     * - Captures synaptic short-term plasticity effects
     * - Accounts for spike train history, not just individual spike pairs
     * 
     * Mathematical formulation:
     * Δw = A+ * f(Δt, history) for Δt > 0
     * Δw = A- * g(Δt, history) for Δt < 0
     * 
     * Where f and g are triplet functions that depend on:
     * - Spike timing difference (Δt)
     * - Recent spike history (facilitation/depression)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Get the most recent spike times
    float t_post = static_cast<float>(postSpikes.back());
    float t_pre = static_cast<float>(preSpikes.back());
    
    // Calculate timing difference in milliseconds
    float dtMs = (t_post - t_pre) * 1000.0f;
    
    if (dtMs > 0) {
        // Pre before post: potentiation
        // Standard STDP term
        float stdpTerm = pImpl->ltpWeight * std::exp(-dtMs / tau);
        
        // Triplet terms: account for recent spike history
        // Facilitation from recent pre spikes
        float facilitation = calculateFacilitation(preSpikes, dtMs, pImpl->tauF);
        
        // Depression from recent post spikes  
        float depression = calculateDepression(postSpikes, dtMs, pImpl->tauD);
        
        totalDelta = stdpTerm * (1.0f + facilitation - depression);
        
    } else if (dtMs < 0) {
        // Post before pre: depression
        // Standard STDP term
        float stdpTerm = -pImpl->ltdWeight * std::exp(dtMs / tau);
        
        // Triplet terms
        float facilitation = calculateFacilitation(preSpikes, -dtMs, pImpl->tauF);
        float depression = calculateDepression(postSpikes, -dtMs, pImpl->tauD);
        
        totalDelta = stdpTerm * (1.0f + facilitation - depression);
    }
    
    // Apply to synapse with bounds
    applyWeightChange(synapse, totalDelta);
}

void STDP::TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // STDP specific weight bounds
    float w = synapse->getWeight();
    bool awayFromZero = std::abs(w + delta) > std::abs(w);
    
    // Weight-dependent scaling (Phase 4 optimization)
    delta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
    
    // Apply weight change
    synapse->addToWeight(delta);
    
    // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01
    if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
        synapse->setWeight(-0.01f);
    } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
        synapse->setWeight(0.01f);
    }
}

void STDP::TripletSTDP::setUmax(float umax) {
    pImpl->umax = std::clamp(umax, 0.0f, 2.0f);
}

float STDP::TripletSTDP::getUmax() const {
    return pImpl->umax;
}

void STDP::TripletSTDP::setTauF(float tauF) {
    pImpl->tauF = std::clamp(tauF, 10.0f, 500.0f);
}

float STDP::TripletSTDP::getTauF() const {
    return pImpl->tauF;
}

void STDP::TripletSTDP::setTauD(float tauD) {
    pImpl->tauD = std::clamp(tauD, 10.0f, 500.0f);
}

float STDP::TripletSTDP::getTauD() const {
    return pImpl->tauD;
}

void STDP::TripletSTDP::configure(float umax, float tauF, float tauD) {
    setUmax(umax);
    setTauF(tauF);
    setTauD(tauD);
}

// Helper functions for triplet STDP
float STDP::TripletSTDP::calculateFacilitation(const std::vector<Timestamp>& spikeHistory,
                                               float dt, float tau) const {
    if (spikeHistory.size() < 2) return 0.0f;
    
    // Calculate facilitation based on recent pre spikes
    float facilitation = 0.0f;
    float t_post = static_cast<float>(spikeHistory.back());
    
    for (size_t i = spikeHistory.size() - 2; i < spikeHistory.size(); --i) {
        float t_pre = static_cast<float>(spikeHistory[i]);
        float deltaT = (t_post - t_pre) * 1000.0f;  // Convert to ms
        
        if (deltaT < 0) {
            facilitation += std::exp(-deltaT / tau);
        }
    }
    
    return std::min(facilitation * pImpl->umax, 1.0f);
}

float STDP::TripletSTDP::calculateDepression(const std::vector<Timestamp>& spikeHistory,
                                             float dt, float tau) const {
    if (spikeHistory.size() < 2) return 0.0f;
    
    // Calculate depression based on recent post spikes
    float depression = 0.0f;
    float t_pre = static_cast<float>(spikeHistory.back());
    
    for (size_t i = spikeHistory.size() - 2; i < spikeHistory.size(); --i) {
        float t_post = static_cast<float>(spikeHistory[i]);
        float deltaT = (t_pre - t_post) * 1000.0f;  // Convert to ms
        
        if (deltaT < 0) {
            depression += std::exp(-deltaT / tau);
        }
    }
    
    return std::min(depression * pImpl->umax, 1.0f);
}

} // namespace nlm
