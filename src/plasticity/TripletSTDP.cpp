#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Triplet parameters (presynaptic and postsynaptic traces)
    float preTraceDecay;   // Decay rate of presynaptic trace
    float postTraceDecay;  // Decay rate of postsynaptic trace
    
    // Current trace values
    float preTraceValue;   // Current presynaptic trace
    float postTraceValue;  // Current postsynaptic trace
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), preTraceDecay(0.95f),
             postTraceDecay(0.95f), preTraceValue(0.0f), postTraceValue(0.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    /*
     * Triplet STDP implementation
     * 
     * Triplet STDP extends standard STDP by including effects of recent
     * presynaptic and postsynaptic spikes (traces) that modify weight changes
     * 
     * Mathematical formulation:
     * 
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): 
     *   Δw = A+ * exp(-Δt/τ+) * (1 + trace_contributions)
     * 
     * If Δt < 0 (post before pre):
     *   Δw = -A- * exp(-|Δt|/τ-) * (1 + trace_contributions)
     *   
     * Where trace_contributions depend on presynaptic and postsynaptic
     * activity traces that decay over time
     * 
     * Biological inspiration:
     *   - Reflects calcium dynamics in neurons
     *   - Accounts for presynaptic and postsynaptic spike history
     *   - Explains complex spike-timing dependent plasticity patterns
     *   - Found in cortical and hippocampal plasticity experiments
     *   
     * Implementation details:
     *   - Maintains presynaptic and postsynaptic activity traces
     *   - Updates traces based on recent spike activity
     *   - Combines spike-pair learning with trace contributions
     *   - Provides more accurate model of biological plasticity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update activity traces based on recent spikes
    updateActivityTraces(preSpikes, postSpikes, dt);
    
    // Calculate triplet STDP weight change
    float weightChange = calculateTripletWeightChange(synapse, preSpikes, postSpikes, dt);
    
    // Apply weight change with bounds
    applyWeightChange(synapse, weightChange);
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void TripletSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void TripletSTDP::setPreTraceDecay(float decay) {
    pImpl->preTraceDecay = std::clamp(decay, 0.1f, 0.999f);
}

float TripletSTDP::getPreTraceDecay() const {
    return pImpl->preTraceDecay;
}

void TripletSTDP::setPostTraceDecay(float decay) {
    pImpl->postTraceDecay = std::clamp(decay, 0.1f, 0.999f);
}

float TripletSTDP::getPostTraceDecay() const {
    return pImpl->postTraceDecay;
}

float TripletSTDP::calculateTripletWeightChange(Synapse* synapse,
                                                const std::vector<Timestamp>& preSpikes,
                                                const std::vector<Timestamp>& postSpikes,
                                                TimestepDuration dt) {
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float weightChange = pImpl->ltpWeight * std::exp(-dt / tau);
                
                // Add triplet contributions
                weightChange *= (1.0f + pImpl->postTraceValue);
                
                totalDelta += weightChange;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float weightChange = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                
                // Add triplet contributions
                weightChange *= (1.0f + pImpl->preTraceValue);
                
                totalDelta += weightChange;
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    return totalDelta;
}

void TripletSTDP::updateActivityTraces(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    // Decay existing traces
    pImpl->preTraceValue *= pImpl->preTraceDecay;
    pImpl->postTraceValue *= pImpl->postTraceDecay;
    
    // Update traces based on recent spikes
    float dtMs = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    
    // Presynaptic trace: each pre-spike adds 1.0 to trace
    for (Timestamp preTime : preSpikes) {
        pImpl->preTraceValue += 1.0f;
    }
    
    // Postsynaptic trace: each post-spike adds 1.0 to trace
    for (Timestamp postTime : postSpikes) {
        pImpl->postTraceValue += 1.0f;
    }
    
    // Ensure traces stay in reasonable bounds
    pImpl->preTraceValue = std::min(pImpl->preTraceValue, 10.0f);
    pImpl->postTraceValue = std::min(pImpl->postTraceValue, 10.0f);
}

} // namespace nlm
