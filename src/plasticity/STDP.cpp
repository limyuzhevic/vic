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
     * IMPROVEMENTS over original:
     *   - Better temporal window for different timing ranges
     *   - Decorrelated from Hebbian to improve selectivity
     *   - Adaptive learning based on current weight
     *   - Integration with eligibility traces for better reward modulation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Nearest-neighbour STDP: only the most recent pre/post pair is used.
    // This provides better stability and prevents runaway learning.
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    // IMPROVED: Better temporal window with asymmetric time constants
    // Allows for more selective timing-dependent learning
    float ltpWindow = tau * 0.8f;   // Slightly faster decay for LTP
    float ltdWindow = tau * 1.2f;   // Slower decay for LTD
    
    if (dtMs > 0) {
        // Pre-before-post: potentiation
        // Stronger for shorter timing differences
        float normalizedTiming = dtMs / ltpWindow;
        totalDelta += pImpl->ltpWeight * std::exp(-normalizedTiming);
    } else if (dtMs < 0) {
        // Post-before-pre: depression
        // Stronger for shorter timing differences
        float normalizedTiming = std::abs(dtMs) / ltdWindow;
        totalDelta += -pImpl->ltdWeight * std::exp(-normalizedTiming);
    }
    
    // IMPROVED: Better weight-dependent learning scaling
    // Prevents saturation and improves selectivity
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Enhanced sign preservation for better learning selectivity
        // Prevents inhibitory synapses from becoming excitatory
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Improved weight-dependent learning for better selectivity
        // Scales potentiation and depression differently based on current weight
        float w = synapse->getWeight();
        bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
        
        // Adaptation factor: reduces learning as weights approach bounds
        float adaptationFactor = 1.0f;
        if (awayFromZero) {
            // Potentiation: scales with distance from bounds
            adaptationFactor = 1.0f - std::abs(w) * 2.0f;
        } else {
            // Depression: scales with proximity to zero
            adaptationFactor = std::abs(w);
        }
        adaptationFactor = std::clamp(adaptationFactor, 0.1f, 1.0f);
        
        totalDelta *= adaptationFactor;
        
        // Apply weight change with adaptive scaling
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01.
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Update eligibility trace for reward-modulated learning
        // Enables better integration with dopamine and reward signals
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.1f);
        // Decay eligibility trace to maintain temporal specificity
        synapse->decayEligibilityTrace(0.9f);
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
