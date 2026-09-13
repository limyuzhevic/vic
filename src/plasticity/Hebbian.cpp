#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;     // Hebbian learning rate (η)
    float maxWeight;        // Maximum synaptic weight
    float minWeight;        // Minimum synaptic weight
    float decayRate;        // Weight decay rate (for homeostasis)
    
    // Anti-Hebbian variant (for inhibitory connections)
    bool antiHebbian;       // If true, weakens connections instead of strengthening
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f), 
             decayRate(0.0001f), antiHebbian(false) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (basic):
     * For each pre-post spike pair: Δw = η * (f_pre * f_post)
     * 
     * Where:
     *   η = learningRate (learning rate)
     *   f_pre = presynaptic firing (binary: 1 if spike occurred)
     *   f_post = postsynaptic firing (binary: 1 if spike occurred)
     * 
     * Advanced formulations:
     *   - BCM rule: Δw = φ(w) * (f_post - θ), where φ(w) = w / sqrt(1 + w²)
     *   - Oja's rule: Δw = η * (f_pre * f_post - w * f_post²)
     *   - Mean-field theory for population learning
     * 
     * Biological inspiration:
     *   - "Cells that fire together, wire together"
     *   - NMDA receptor-dependent calcium signaling
     *   - Ubiquitous in cortex for associative learning
     * 
     * Limitations:
     *   - Can lead to runaway excitation without normalization
     *   - Doesn't account for neuromodulation
     *   - Requires careful weight bounding
     */
    
    float totalDelta = 0.0f;
    
    // Calculate firing statistics from spike history
    // Convert spike times to binary firing indicators
    bool preFired = !preSpikes.empty();
    bool postFired = !postSpikes.empty();
    
    // Basic Hebbian rule: co-activation strengthens connection
    if (preFired && postFired) {
        // Determine if synapse should be strengthened or weakened
        if (!pImpl->antiHebbian) {
            // Standard Hebbian: strengthen connections
            totalDelta = pImpl->learningRate * 
                        (pImpl->maxWeight - synapse->getWeight());  // Saturates at max
        } else {
            // Anti-Hebbian: weaken connections  
            totalDelta = -pImpl->learningRate * 
                        (synapse->getWeight() - pImpl->minWeight);  // Saturates at min
        }
    }
    
    // Apply BCM (Bienenstock-Cooper-Munro) rule for stability
    // BCM provides a sliding threshold θ that adapts with average activity
    float currentWeight = synapse->getWeight();
    float phi = currentWeight / std::sqrt(1.0f + currentWeight * currentWeight);  // B function
    
    // Simplified BCM: θ depends on weight and average firing
    float theta = 0.5f + 0.1f * currentWeight;  // Sliding threshold
    float postFiringRate = postFired ? 1.0f : 0.0f;
    
    // BCM delta: Δw = φ(w) * (f_post - θ)
    float bcmDelta = phi * (postFiringRate - theta);
    
    // Combine with basic Hebbian
    totalDelta += bcmDelta * pImpl->learningRate * 0.5f;
    
    // Apply Oja's normalization to prevent runaway excitation
    // Δw = η * (f_pre * f_post - w * f_post²)
    float ojaDelta = pImpl->learningRate * (preFired * postFired - 
                                            currentWeight * postFiringRate * postFiringRate);
    
    totalDelta += ojaDelta * 0.5f;  // Combine with previous
    
    // Apply weight decay for homeostasis
    float decayDelta = -pImpl->decayRate * currentWeight;
    totalDelta += decayDelta;
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply the weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        currentTrace += totalDelta;
        synapse->setEligibilityTrace(currentTrace);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with homeostatic constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Apply weight bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply homeostatic scaling to prevent runaway excitation/depression
    // Soft bound: weights are pushed toward an ideal range
    float idealRange = (pImpl->maxWeight - pImpl->minWeight) * 0.5f + pImpl->minWeight;
    float distanceFromIdeal = std::abs(newWeight - idealRange);
    
    // Scale down weights that are too far from ideal
    if (distanceFromIdeal > 0.8f * (pImpl->maxWeight - pImpl->minWeight)) {
        float scaleFactor = std::min(1.0f, idealRange / std::abs(newWeight));
        if (std::abs(newWeight) > 0.01f) {
            newWeight *= scaleFactor;
        }
    }
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return pImpl->antiHebbian ? "Anti-Hebbian" : "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.1f, 2.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -2.0f, -0.1f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::setDecayRate(float decay) {
    pImpl->decayRate = std::clamp(decay, 0.0f, 0.001f);
}

float Hebbian::getDecayRate() const {
    return pImpl->decayRate;
}

void Hebbian::setAntiHebbian(bool anti) {
    pImpl->antiHebbian = anti;
}

bool Hebbian::isAntiHebbian() const {
    return pImpl->antiHebbian;
}

void Hebbian::configureBiologically(float learningRate, float weightRange) {
    // Configure with biologically plausible parameters
    setLearningRate(learningRate);
    setMaxWeight(weightRange);
    setMinWeight(-weightRange);
    setDecayRate(0.00005f);  // Slow homeostasis
    
    // Most cortical synapses are standard Hebbian
    setAntiHebbian(false);
}

void Hebbian::configureForInhibition(float learningRate, float weightRange) {
    // Configure for inhibitory synapses (often anti-Hebbian)
    setLearningRate(learningRate * 0.5f);  // Slower learning for inhibition
    setMaxWeight(weightRange);
    setMinWeight(-weightRange);
    setDecayRate(0.00002f);
    setAntiHebbian(true);  // Inhibitory synapses often weaken with co-activation
}

} // namespace nlm