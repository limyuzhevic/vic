#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float efficacyMultiplier;  // Multiplicative factor for synaptic efficacy
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), efficacyMultiplier(1.0f) {}
};
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
    
    // More efficient implementation: sort spikes and process in order
    std::vector<Timestamp> sortedSpikes;
    sortedSpikes.reserve(preSpikes.size() + postSpikes.size());
    sortedSpikes.insert(sortedSpikes.end(), preSpikes.begin(), preSpikes.end());
    sortedSpikes.insert(sortedSpikes.end(), postSpikes.begin(), postSpikes.end());
    std::sort(sortedSpikes.begin(), sortedSpikes.end());
    
    // Track which spikes have been paired
    std::vector<bool> preUsed(preSpikes.size(), false);
    std::vector<bool> postUsed(postSpikes.size(), false);
    
    // Find optimal pairing for maximum potentiation
    for (size_t i = 0; i < sortedSpikes.size(); ++i) {
        // Determine spike type
        bool isPre = false;
        bool isPost = false;
        size_t preIdx = 0, postIdx = 0;
        
        // Find the next unpaired spike of each type
        for (size_t j = 0; j < preSpikes.size(); ++j) {
            if (!preUsed[j] && preSpikes[j] == sortedSpikes[i]) {
                isPre = true;
                preIdx = j;
                break;
            }
        }
        
        if (!isPre) {
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                if (!postUsed[j] && postSpikes[j] == sortedSpikes[i]) {
                    isPost = true;
                    postIdx = j;
                    break;
                }
            }
        }
        
        if (isPre && !postUsed[postIdx]) {
            // Find nearest post spike after this pre
            float minDt = std::numeric_limits<float>::max();
            size_t bestPost = 0;
            
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                if (!postUsed[j]) {
                    float dt = static_cast<float>(postSpikes[j] - sortedSpikes[i]);
                    if (dt > 0 && dt < minDt) {
                        minDt = dt;
                        bestPost = j;
                    }
                }
            }
            
            if (minDt < std::numeric_limits<float>::max()) {
                // Pair with the nearest post spike
                postUsed[bestPost] = true;
                float delta = pImpl->ltpWeight * std::exp(-minDt / tau);
                totalDelta += delta;
            }
        }
        
        if (isPost && !preUsed[preIdx]) {
            // Find nearest pre spike after this post
            float minDt = std::numeric_limits<float>::max();
            size_t bestPre = 0;
            
            for (size_t j = 0; j < preSpikes.size(); ++j) {
                if (!preUsed[j]) {
                    float dt = static_cast<float>(sortedSpikes[i] - preSpikes[j]);
                    if (dt > 0 && dt < minDt) {
                        minDt = dt;
                        bestPre = j;
                    }
                }
            }
            
            if (minDt < std::numeric_limits<float>::max()) {
                // Pair with the nearest pre spike
                preUsed[bestPre] = true;
                float delta = -pImpl->ltdWeight * std::exp(-minDt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        totalDelta *= pImpl->efficacyMultiplier;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        // More sophisticated: use exponential trace decay
        float traceDecay = std::exp(-dt / pImpl->timeConstant);
        synapse->setEligibilityTrace(currentTrace * traceDecay + totalDelta);
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

void STDP::setWeightBounds(float minWeight, float maxWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 10.0f);
    pImpl->maxWeight = std::clamp(maxWeight, -10.0f, 10.0f);
    if (pImpl->minWeight > pImpl->maxWeight) {
        std::swap(pImpl->minWeight, pImpl->maxWeight);
    }
}

float STDP::getMinWeight() const {
    return pImpl->minWeight;
}

float STDP::getMaxWeight() const {
    return pImpl->maxWeight;
}

float STDP::getEfficacyMultiplier() const {
    return pImpl->efficacyMultiplier;
}

void STDP::setEfficacyMultiplier(float multiplier) {
    pImpl->efficacyMultiplier = std::clamp(multiplier, 0.0f, 10.0f);
}

} // namespace nlm
