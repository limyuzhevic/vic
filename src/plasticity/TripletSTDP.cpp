#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float preTraceTau;          // Time constant for pre-synaptic trace (tau1)
    float postTraceTau;         // Time constant for post-synaptic trace (tau2)
    float learningRate;         // Learning rate for triplet interactions
    float thirdOrderRate;       // Learning rate for third-order interactions
    float minWeight;            // Minimum synaptic weight
    float maxWeight;            // Maximum synaptic weight
    
    Impl() : preTraceTau(20.0f), postTraceTau(20.0f), learningRate(0.01f),
             thirdOrderRate(0.001f), minWeight(-1.0f), maxWeight(1.0f) {}
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
     * Mathematical formulation (Gerstner et al., 2014):
     * Δw = A1 * f1(x1, x2) + A2 * f2(x1, x2) + A3 * f3(x1, x2)
     * 
     * Where:
     *   x1 = pre-synaptic spike trace
     *   x2 = post-synaptic spike trace
     *   f1, f2, f3 = triplet interaction functions
     *   A1, A2, A3 = triplet coefficients
     * 
     * Biological inspiration:
     *   - Captures first-order (paired) and second-order (triplet) interactions
     *   - Accounts for depression induced by a single postsynaptic spike
     *   - Accounts for potentiation induced by simultaneous pre-post spike pairs
     *   - Accounts for potentiation induced by pre-post spike pairs with a third spike
     *   - More biologically realistic than standard pairwise STDP
     *   - Better captures calcium dynamics in neurons
     *   
     * Applications:
     *   - More accurate modeling of cortical plasticity
     *   - Better prediction of neural adaptation in learning tasks
     *   - Improved performance in spiking neural network models
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Optimization: Limit spike history processing to recent spikes
    const size_t MAX_PROCESSING = 30;
    size_t preCount = std::min(preSpikes.size(), MAX_PROCESSING);
    size_t postCount = std::min(postSpikes.size(), MAX_PROCESSING);
    
    // Compute spike traces (first-order effects)
    float preTrace = 0.0f;
    float postTrace = 0.0f;
    
    // Calculate pre-synaptic trace from recent spikes
    for (size_t i = 0; i < preCount; ++i) {
        Timestamp preTime = preSpikes[preSpikes.size() - 1 - i];  // Most recent first
        float timeSinceSpike = static_cast<float>(dt * 1000.0f - preTime);  // Convert to ms
        preTrace += std::exp(-timeSinceSpike / pImpl->preTraceTau);
    }
    
    // Calculate post-synaptic trace from recent spikes
    for (size_t i = 0; i < postCount; ++i) {
        Timestamp postTime = postSpikes[postSpikes.size() - 1 - i];  // Most recent first
        float timeSinceSpike = static_cast<float>(dt * 1000.0f - postTime);  // Convert to ms
        postTrace += std::exp(-timeSinceSpike / pImpl->postTraceTau);
    }
    
    // Compute first-order effects (paired spike interactions)
    float firstOrderEffect = 0.0f;
    
    // Process spike pairs for first-order effects
    for (size_t i = 0; i < preCount; ++i) {
        Timestamp preTime = preSpikes[preSpikes.size() - 1 - i];
        
        for (size_t j = 0; j < postCount; ++j) {
            Timestamp postTime = postSpikes[postSpikes.size() - 1 - j];
            
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (std::abs(dt) < 0.000001f) {
                continue;  // Skip simultaneous spikes
            }
            
            if (dt > 0) {
                // Pre before post: potentiation
                firstOrderEffect += pImpl->learningRate * std::exp(-dt / pImpl->postTraceTau);
            } else {
                // Post before pre: depression
                firstOrderEffect += -pImpl->learningRate * std::exp(dt / pImpl->preTraceTau);
            }
        }
    }
    
    // Compute second-order effects (triplet interactions)
    float secondOrderEffect = 0.0f;
    
    // Simple triplet approximation: use traces to estimate third spike interactions
    secondOrderEffect = pImpl->thirdOrderRate * preTrace * postTrace;
    
    // Total weight change
    totalDelta = firstOrderEffect + secondOrderEffect;
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update spike history traces in synapse for next iteration
        // (Traces are maintained in synapse state, not here)
    }
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

void TripletSTDP::setPreTraceTimeConstant(float tau1) {
    pImpl->preTraceTau = std::clamp(tau1, 5.0f, 100.0f);
}

float TripletSTDP::getPreTraceTimeConstant() const {
    return pImpl->preTraceTau;
}

void TripletSTDP::setPostTraceTimeConstant(float tau2) {
    pImpl->postTraceTau = std::clamp(tau2, 5.0f, 100.0f);
}

float TripletSTDP::getPostTraceTimeConstant() const {
    return pImpl->postTraceTau;
}

void TripletSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float TripletSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void TripletSTDP::setThirdOrderLearningRate(float rate) {
    pImpl->thirdOrderRate = std::clamp(rate, 0.0f, 0.01f);
}

float TripletSTDP::getThirdOrderLearningRate() const {
    return pImpl->thirdOrderRate;
}

} // namespace nlm