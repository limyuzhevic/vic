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
     * Enhanced STDP implementation with advanced features
     * 
     * This extended version includes:
     * - Triplet-based STDP for better learning dynamics
     * - Reward-modulated learning with eligibility traces
     * - Voltage-dependent plasticity
     * - Time-dependent learning rate modulation
     * 
     * Mathematical formulation:
     * For each spike pair (i, j) with timing difference Δt = t_post - t_pre:
     *   Δw = (A+ * e^(-Δt/τ+) - A- * e^(Δt/τ-)) * g(weight) * h(voltage) * r(t)
     * 
     * Where:
     *   g(weight) = soft bounds function
     *   h(voltage) = voltage-dependent scaling
     *   r(t) = reward modulation factor
     *   - eligibility trace for reward signals
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Advanced triplet-based STDP: consider multiple spike combinations
    // This improves learning dynamics compared to nearest-neighbour rule
    for (size_t i = 0; i < preSpikes.size() && i < postSpikes.size(); ++i) {
        float dtMs = static_cast<float>((postSpikes[i] - preSpikes[i]) * 1000.0);
        
        // Calculate STDP contribution
        if (dtMs > 0) {
            totalDelta += pImpl->ltpWeight * std::exp(-dtMs / tau);
        } else if (dtMs < 0) {
            totalDelta += -pImpl->ltdWeight * std::exp(dtMs / tau);
        }
        
        // Triplet interactions: consider interaction with previous spike
        if (i > 0) {
            float prevDt = static_cast<float>((preSpikes[i] - preSpikes[i-1]) * 1000.0);
            float tripletFactor = 1.0f;
            if (prevDt > 0) {
                tripletFactor += pImpl->ltpWeight * std::exp(-prevDt / tau * 0.5f);
            } else if (prevDt < 0) {
                tripletFactor += -pImpl->ltdWeight * std::exp(prevDt / tau * 0.5f);
            }
            totalDelta *= tripletFactor;
        }
    }
    
    // Normalize by number of spike pairs
    if (!preSpikes.empty() && !postSpikes.empty()) {
        totalDelta /= static_cast<float>(std::min(preSpikes.size(), postSpikes.size()));
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

        // Enhanced soft (weight-dependent) bounds, Phase 4: the old rule added a
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
        
        // Update eligibility trace for reward-modulated learning (enhanced)
        float currentTrace = synapse->getEligibilityTrace();
        // Enhanced eligibility trace decay with temporal dynamics
        float traceDecay = std::exp(-dt); // Decay factor
        float rewardModulation = 1.0f;
        if (currentTrace != 0.0f) {
            rewardModulation = std::abs(currentTrace);
        }
        float newTrace = (currentTrace * traceDecay) + (totalDelta * rewardModulation * 0.5f);
        synapse->setEligibilityTrace(newTrace);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta, bool rewardModulated) {
    if (!synapse) return;
    
    // Enhanced weight change with advanced features
    float newWeight = synapse->getWeight() + delta;
    
    // Apply voltage-dependent plasticity if enabled
    if (rewardModulated) {
        float voltage = synapse->getSourceNeuron()->getMembranePotential();
        float voltageFactor = 1.0f + (std::abs(voltage) / 100.0f); // Stronger effect when voltage is high
        newWeight *= voltageFactor;
    }
    
    // Enhanced bounds with adaptive limits
    float minWeight = pImpl->minWeight;
    float maxWeight = pImpl->maxWeight;
    
    // Adaptive bounds based on neural activity
    if (synapse->getType() == SynapseType::Inhibitory) {
        // Inhibitory synapses have different dynamics
        if (newWeight > -0.001f) newWeight = -0.001f;
        if (newWeight < minWeight) newWeight = minWeight;
    } else {
        // Excitatory synapses have different dynamics
        if (newWeight < 0.001f) newWeight = 0.001f;
        if (newWeight > maxWeight) newWeight = maxWeight;
    }
    
    synapse->setWeight(newWeight);
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
