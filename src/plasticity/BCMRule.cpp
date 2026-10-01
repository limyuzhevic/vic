#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct BCMRule::Impl {
    float sigma;          // Sliding threshold (theta_m)
    float tauPlus;        // LTP time constant
    float tauMinus;       // LTD time constant
    float learningRate;   // Learning rate
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Post-synaptic activity history
    std::unordered_map<NeuronId, std::vector<float>> postActivityHistory;
    
    Impl() : sigma(0.5f), tauPlus(20.0f), tauMinus(40.0f), learningRate(0.01f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) plasticity rule
     * 
     * Implements sliding threshold learning theory
     * 
     * Mathematical formulation:
     * Δw = η * [φ(x) - θ_m(x)] * y
     * 
     * Where:
     *   x = total synaptic input to postsynaptic neuron
     *   φ(x) = activity function (typically sigmoid)
     *   θ_m(x) = sliding threshold (modified by average activity)
     *   y = postsynaptic spike activity
     *   η = learning rate
     * 
     * Key features:
     * - Sliding threshold adapts based on average activity
     * - Activity-dependent learning (Hebbian-like when above threshold)
     * - Depression when below threshold (anti-Hebbian-like)
     * - Provides stability in dynamic environments
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    NeuronId postNeuron = synapse->getDestinationNeuron().index();
    
    // Calculate postsynaptic activity level (simplified)
    float postActivity = 0.0f;
    if (pImpl->postActivityHistory.count(postNeuron)) {
        const auto& history = pImpl->postActivityHistory[postNeuron];
        if (!history.empty()) {
            // Average recent activity
            float sum = std::accumulate(history.end() - std::min(static_cast<size_t>(10), history.size()),
                                        history.end(), 0.0f);
            postActivity = sum / std::min(static_cast<size_t>(10), history.size());
        }
    }
    
    // Update sliding threshold based on average activity
    // Threshold increases with sustained high activity (stability)
    float previousSigma = pImpl->sigma;
    pImpl->sigma = pImpl->sigma + 0.01f * (postActivity - pImpl->sigma);
    
    // Calculate weight change using BCM rule
    float weightChange = 0.0f;
    
    // Update sliding threshold with post-synaptic spikes
    if (!postSpikes.empty()) {
        pImpl->postActivityHistory[postNeuron].insert(pImpl->postActivityHistory[postNeuron].end(),
                                                     postSpikes.begin(), postSpikes.end());
        
        // Clean old activity (older than 100 time steps)
        if (pImpl->postActivityHistory[postNeuron].size() > 100) {
            pImpl->postActivityHistory[postNeuron].erase(
                pImpl->postActivityHistory[postNeuron].begin());
        }
    }
    
    // For each pre-post spike pair, calculate BCM weight change
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: could be LTP or LTD depending on threshold
                // Calculate combined input (simplified)
                float combinedInput = 0.0f;  // Would include all presynaptic inputs
                
                // Activity function (sigmoid)
                float activity = 1.0f / (1.0f + std::exp(-2.0f * (combinedInput - previousSigma)));
                
                // BCM weight change
                float delta = pImpl->learningRate * (activity - previousSigma) * 1.0f; // Assuming postsynaptic spike
                
                weightChange += delta;
            }
            // Post before pre gives little to no change in BCM (long-term)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(weightChange) > 1e-6f) {
        synapse->addToWeight(weightChange);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + weightChange * 0.1f);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::configure(float sigma, float tauPlus, float tauMinus, float learningRate) {
    pImpl->sigma = std::clamp(sigma, 0.1f, 2.0f);
    pImpl->tauPlus = std::clamp(tauPlus, 1.0f, 100.0f);
    pImpl->tauMinus = std::clamp(tauMinus, 1.0f, 200.0f);
    pImpl->learningRate = std::clamp(learningRate, 0.0f, 0.1f);
}

void BCMRule::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.1f, 2.0f);
}

float BCMRule::getSigma() const {
    return pImpl->sigma;
}

void BCMRule::setTauPlus(float tau) {
    pImpl->tauPlus = std::clamp(tau, 1.0f, 100.0f);
}

float BCMRule::getTauPlus() const {
    return pImpl->tauPlus;
}

void BCMRule::setTauMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 1.0f, 200.0f);
}

float BCMRule::getTauMinus() const {
    return pImpl->tauMinus;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm