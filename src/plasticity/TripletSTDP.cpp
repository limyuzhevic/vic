// Implementation of TripletSTDP
// Triplet STDP: advanced spike-timing dependent plasticity with triplet interactions

#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeightA;              // LTP weight for pre-post pairs (A+)
    float ltdWeightA;              // LTD weight for post-pre pairs (A-)
    float ltpWeightB;              // LTP weight for single pre spikes (B+)
    float ltdWeightB;              // LTD weight for single post spikes (B-)
    float timeConstant;            // Time constant (ms)
    float minWeight;               // Minimum synaptic weight
    float maxWeight;               // Maximum synaptic weight
    
    // State for triplet interactions
    float recentPreSpikes;         // Count of recent presynaptic spikes
    float recentPostSpikes;        // Count of recent postsynaptic spikes
    
    Impl() : ltpWeightA(0.01f), ltdWeightA(0.012f), ltpWeightB(0.003f), ltdWeightB(0.003f),
             timeConstant(20.0f), minWeight(-1.0f), maxWeight(1.0f),
             recentPreSpikes(0.0f), recentPostSpikes(0.0f) {}
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
     * Mathematical formulation (Gerstner et al., 1996):
     * Δw = Σ_i [A+ * exp(-(t_post - t_pre)/τ+) - A- * exp(-(t_pre - t_post)/τ-)]
     *     + Σ_i [B+ * exp(-t_post/τ+) * recent_pre_spikes - B- * exp(-t_pre/τ+) * recent_post_spikes]
     * 
     * Where:
     *   A+, A- = Pair-wise STDP weights
     *   B+, B- = Triplet interaction weights
     *   τ+ = τ- = timeConstant
     *   recent_pre_spikes = number of presynaptic spikes in recent window
     *   recent_post_spikes = number of postsynaptic spikes in recent window
     *   
     * Biological inspiration:
     *   - Synaptic dynamics with calcium dynamics
     *   - Short-term plasticity interactions
     *   - Presynaptic and postsynaptic spike history
     *   - More accurate biological modeling
     *   
     * Key features:
     *   - Pair-wise STDP between spike pairs
     *   - Triplet interactions between spikes and recent activity
     *   - Asymmetric weight changes for potentiation vs depression
     *   - Captures more complex biological dynamics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Update recent spike counts
    pImpl->recentPreSpikes = static_cast<float>(preSpikes.size());
    pImpl->recentPostSpikes = static_cast<float>(postSpikes.size());
    
    // Calculate pair-wise STDP contributions
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Pair-wise component
                float pairDelta = pImpl->ltpWeightA * std::exp(-dt / tau);
                totalDelta += pairDelta;
                
                // Triplet component (depends on recent postsynaptic spikes)
                if (pImpl->recentPostSpikes > 0) {
                    float tripletDelta = pImpl->ltpWeightB * std::exp(-dt / tau) * pImpl->recentPostSpikes;
                    totalDelta += tripletDelta;
                }
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Pair-wise component
                float pairDelta = -pImpl->ltdWeightA * std::exp(dt / tau);
                totalDelta += pairDelta;
                
                // Triplet component (depends on recent presynaptic spikes)
                if (pImpl->recentPreSpikes > 0) {
                    float tripletDelta = -pImpl->ltdWeightB * std::exp(dt / tau) * pImpl->recentPreSpikes;
                    totalDelta += tripletDelta;
                }
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
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

void TripletSTDP::setLTPWeightA(float weight) {
    pImpl->ltpWeightA = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeightA() const {
    return pImpl->ltpWeightA;
}

void TripletSTDP::setLTDWeightA(float weight) {
    pImpl->ltdWeightA = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeightA() const {
    return pImpl->ltdWeightA;
}

void TripletSTDP::setLTPWeightB(float weight) {
    pImpl->ltpWeightB = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeightB() const {
    return pImpl->ltpWeightB;
}

void TripletSTDP::setLTDWeightB(float weight) {
    pImpl->ltdWeightB = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeightB() const {
    return pImpl->ltdWeightB;
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

} // namespace nlm