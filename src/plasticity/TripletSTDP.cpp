#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeight1;      // A+ for pre-post pairing (LTP)
    float ltdWeight1;      // A- for post-pre pairing (LTD)
    float ltpWeight2;      // A++ for post-post (LTP)
    float ltdWeight2;      // A-- for pre-pre (LTD)
    float timeConstant;   // Tau for exponential windows
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Triple history for triplet interactions
    std::unordered_map<NeuronId, std::vector<Timestamp>> preSpikeHistory;
    std::unordered_map<NeuronId, std::vector<Timestamp>> postSpikeHistory;
    
    Impl() : ltpWeight1(0.01f), ltdWeight1(0.012f), ltpWeight2(0.005f), ltdWeight2(0.006f), 
             timeConstant(20.0f), minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    /*
     * Triplet STDP implementation with synaptic history
     * 
     * Extends standard STDP by considering recent spike triplets:
     * - Pre-post-post interactions (pre before first post, pre before second post)
     * - Post-pre-pre interactions (post before first pre, post before second pre)
     * 
     * Mathematical formulation:
     * 
     * For pre-post pairing (standard STDP):
     * Δw = A+ * exp(-Δt/τ+) - A- * exp(Δt/τ-)
     * 
     * For post-post pairing (triple interaction):
     * Δw = A++ * exp(-Δt_postpost/τ++) * (number of previous posts)
     * 
     * For pre-pre pairing (triple interaction):
     * Δw = -A-- * exp(-Δt_prepre/τ--) * (number of previous pres)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Record spike history for this synapse
    NeuronId sourceNeuron = synapse->getSourceNeuron().index();
    NeuronId targetNeuron = synapse->getDestinationNeuron().index();
    
    Timestamp now = postSpikes.back(); // Assume posts are in order
    
    // Store current spikes in history
    pImpl->preSpikeHistory[sourceNeuron].insert(pImpl->preSpikeHistory[sourceNeuron].end(),
                                                 preSpikes.begin(), preSpikes.end());
    pImpl->postSpikeHistory[targetNeuron].insert(pImpl->postSpikeHistory[targetNeuron].end(),
                                                 postSpikes.begin(), postSpikes.end());
    
    // Clean old spikes (older than 3*timewindow)
    size_t maxHistory = 100;
    if (pImpl->preSpikeHistory[sourceNeuron].size() > maxHistory) {
        pImpl->preSpikeHistory[sourceNeuron].erase(
            pImpl->preSpikeHistory[sourceNeuron].begin(),
            pImpl->preSpikeHistory[sourceNeuron].begin() + 
            (pImpl->preSpikeHistory[sourceNeuron].size() - maxHistory / 2));
    }
    if (pImpl->postSpikeHistory[targetNeuron].size() > maxHistory) {
        pImpl->postSpikeHistory[targetNeuron].erase(
            pImpl->postSpikeHistory[targetNeuron].begin(),
            pImpl->postSpikeHistory[targetNeuron].begin() + 
            (pImpl->postSpikeHistory[targetNeuron].size() - maxHistory / 2));
    }
    
    // Calculate weight changes based on all spike interactions
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // 1. Standard STDP (pre-post and post-pre pairs)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                totalDelta += pImpl->ltpWeight1 * std::exp(-dt / tau);
                
                // Post-post interaction (if this is not the first post)
                if (std::find(postSpikes.begin(), postSpikes.end(), postTime) != std::find(postSpikes.begin(), postSpikes.end(), preTime)) {
                    // Count previous posts that occurred before this post
                    size_t previousPosts = 0;
                    for (Timestamp pt : pImpl->postSpikeHistory[targetNeuron]) {
                        if (pt < postTime) previousPosts++;
                    }
                    if (previousPosts > 0) {
                        // Post-post potentiation
                        totalDelta += pImpl->ltpWeight2 * std::exp(-dt / tau) * previousPosts;
                    }
                }
            } else if (dt < 0) {
                // Post before pre: depression
                totalDelta -= pImpl->ltdWeight1 * std::exp(dt / tau);
                
                // Pre-pre interaction (if this is not the first pre)
                if (std::find(preSpikes.begin(), preSpikes.end(), preTime) != std::find(preSpikes.begin(), preSpikes.end(), postTime)) {
                    // Count previous pres that occurred before this pre
                    size_t previousPres = 0;
                    for (Timestamp pt : pImpl->preSpikeHistory[sourceNeuron]) {
                        if (pt < preTime) previousPres++;
                    }
                    if (previousPres > 0) {
                        // Pre-pre depression (less than standard LTD)
                        totalDelta -= pImpl->ltdWeight2 * std::exp(-dt / tau) * previousPres;
                    }
                }
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for eligibility-based learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.1f);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::configure(float ltpWeight1, float ltdWeight1, float ltpWeight2, float ltdWeight2) {
    pImpl->ltpWeight1 = std::clamp(ltpWeight1, 0.0f, 1.0f);
    pImpl->ltdWeight1 = std::clamp(ltdWeight1, 0.0f, 1.0f);
    pImpl->ltpWeight2 = std::clamp(ltpWeight2, 0.0f, 1.0f);
    pImpl->ltdWeight2 = std::clamp(ltdWeight2, 0.0f, 1.0f);
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

} // namespace nlm