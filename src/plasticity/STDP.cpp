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

STDP::STDP() : pImpl(new Impl) {
    /**
     * Constructor initializes STDP with default biological parameters
     * 
     * Default parameters based on typical cortical synapses:
     * - LTP weight: 0.01 (1% weight increase per optimal timing)
     * - LTD weight: 0.012 (1.2% weight decrease per optimal timing)
     * - Time constant: 20ms (typical NMDA receptor time window)
     * - Weight bounds: [-1.0, 1.0] (exc. positive, inh. negative)
     */
}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /**
     * Apply Spike-Timing-Dependent Plasticity (STDP)
     * 
     * STDP is a Hebbian learning rule where synaptic strength depends on
     * the relative timing of pre- and post-synaptic spikes.
     * 
     * Mathematical formulation:
     * - For Δt > 0 (pre before post): LTP (potentiation)
     *   Δw = A+ × exp(-Δt / τ+) where A+ = ltpWeight
     * - For Δt < 0 (post before pre): LTD (depression)
     *   Δw = A- × exp(Δt / τ-) where A- = ltdWeight
     * 
     * Biological inspiration:
     * - Pre-before-post activates NMDA receptors when postsynaptic neurons spike
     * - Postsynaptic spikes cause calcium influx that strengthens synapses
     * - Pre-before-post patterns are associative (neurons that fire together strengthen)
     * - Post-before-pre patterns are anti-associative (neurons that fire apart weaken)
     * 
     * @param synapse Target synaptic connection to update
     * @param preSpikes History of presynaptic spike times
     * @param postSpikes History of postsynaptic spike times
     * @param dt Simulation timestep for normalization
     * 
     * @note Uses nearest-neighbor approach for efficiency (vs. all-pairs)
     * @note Inhibitory synapses flip sign to preserve inhibition
     * @see https://en.wikipedia.org/wiki/Spike-timing_dependent_plasticity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Nearest-neighbor STDP: only the most recent pre/post pair is used.
    // This is biologically plausible and computationally efficient:
    // cortical synapses typically respond to the most recent spike pair
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
       (Float clipping for weight-dependent plasticity)
        // Promatement (away from zero) now scales by (1 - |w|);
        // depression (toward zero) by |w|,
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

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
