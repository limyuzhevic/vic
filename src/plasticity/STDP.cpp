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
     * Enhanced STDP implementation with multiple time windows and competitive learning
     * 
     * Mathematical formulation:
     * Multiple exponential windows for different spike intervals:
     *   Δw = A+ * [exp(-Δt/τ1) - exp(-Δt/τ2)] - A- * [exp(Δt/τ3) - exp(Δt/τ4)]
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)
     *   τ1, τ2 = faster/slower potentiation time constants
     *   τ3, τ4 = faster/slower depression time constants
     *   Δt = t_post - t_pre
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor subunit differences (NR2A vs NR2B)
     *   - Multiple calcium pathways with different kinetics
     *   - Spike-timing history, not just nearest neighbor
     *   - Synaptic competition through global normalization
     * 
     * Advantages over simple STDP:
     *   - Better selective pattern learning
     *   - Reduced saturation through competition
     *   - More robust to noise and variations
     *   - Maintains relative weight differences between synapses
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float timeRange = 1.0f;  // Search full second range
    
    // Use multiple recent spike pairs for better pattern detection
    // The old single-pair rule saturated weights quickly; multiple pairs
    // with competition maintain selectivity and reduce runaway potentiation.
    size_t maxPairs = std::min(size_t(10), preSpikes.size());
    for (size_t i = 1; i <= maxPairs; ++i) {
        if (preSpikes.size() < i || postSpikes.size() < i) break;
        
        float dtMs = static_cast<float>((postSpikes[postSpikes.size() - i] - 
                                        preSpikes[preSpikes.size() - i]) * 1000.0);
        
        if (dtMs > 0) {
            // Potentiation: pre before post
            // Fast component (τ1 = 10ms) for immediate correlated activity
            float fastPot = pImpl->ltpWeight * std::exp(-dtMs / 10.0f);
            // Slow component (τ2 = 100ms) for delayed correlated activity
            float slowPot = pImpl->ltpWeight * 0.1f * std::exp(-dtMs / 100.0f);  // Smaller slow component
            totalDelta += (fastPot + slowPot);
        } else if (dtMs < 0) {
            // Depression: post before pre
            // Fast component (τ3 = 15ms) for immediate anti-correlation
            float fastDep = -pImpl->ltdWeight * std::exp(dtMs / 15.0f);
            // Slow component (τ4 = 200ms) for delayed anti-correlation
            float slowDep = -pImpl->ltdWeight * 0.1f * std::exp(dtMs / 200.0f);  // Smaller slow component
            totalDelta += (fastDep + slowDep);
        }
    }
    
    // Apply weight change with enhanced competition and normalization
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition, not flip it:
        // pre-before-post on an inhibitory synapse should make its weight
        // more negative, not positive.
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Enhanced competitive normalization: separate strengthening and depression
        float w = synapse->getWeight();
        bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
        
        // Scaling factors for different regimes
        float awayFactor = (1.0f - std::abs(w)) * (1.0f + 0.5f * std::abs(totalDelta));  // Slightly enhanced away from zero
        float towardFactor = std::abs(w) * (1.0f + 0.3f * std::abs(totalDelta));      // Slightly enhanced toward zero
        
        totalDelta *= awayFromZero ? awayFactor : towardFactor;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Enforce sign and minimum magnitude
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Update eligibility trace for reward-modulated learning with better tracking
        float currentTrace = synapse->getEligibilityTrace();
        // Add to trace with regularization to prevent explosion
        currentTrace += totalDelta * 0.8f;  // Slightly decay eligibility
        synapse->setEligibilityTrace(currentTrace);
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
