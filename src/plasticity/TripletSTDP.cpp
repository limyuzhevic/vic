#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct TripletSTDP::Impl {
    float a1Plus;        // First spike potentiation amplitude
    float a2Plus;        // Second spike potentiation amplitude
    float a1Minus;       // First spike depression amplitude
    float a2Minus;       // Second spike depression amplitude
    float tauPlus;       // Potentiation time constant (ms)
    float tauMinus;      // Depression time constant (ms)
    float tripletWindow; // Time window for triplet interactions (ms)
    
    // Internal state for tracking spike triplets
    std::unordered_map<Timestamp, int> postSpikeCounts;    // Track post spike counts for each timestamp
    std::unordered_map<Timestamp, int> preSpikeCounts;    // Track pre spike counts for each timestamp
    
    Impl() : a1Plus(0.01f), a2Plus(0.005f), a1Minus(0.012f), a2Minus(0.006f),
             tauPlus(20.0f), tauMinus(20.0f), tripletWindow(200.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() {
    delete pImpl;
}

void TripletSTDP::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    /*
     * Triplet STDP implementation
     * 
     * Implements STDP with triplet spike interactions where the learning
     * depends not only on pre-post/post-pre spike pairs but also on triplet patterns:
     * - Pre-post-post (enhanced LTP)
     * - Post-pre-pre (enhanced LTD)
     * 
     * Key features:
     * 1. Captures second spike interactions in STDP
     * 2. Accounts for triplet patterns in synaptic learning
     * 3. Provides more biologically realistic learning rules
     * 4. Matches experimental data with complex spike patterns
     * 
     * Biological plausibility:
     * - Matches hippocampal STDP recordings with triplets
     * - Explains second spike effects in plasticity
     * - Incorporates temporal dynamics of triplet interactions
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Clear internal state for this update
    pImpl->postSpikeCounts.clear();
    pImpl->preSpikeCounts.clear();
    
    // Track spike counts for triplet detection
    Timestamp currentTime = 0; // dt is relative, use currentTime as reference
    for (Timestamp preTime : preSpikes) {
        pImpl->preSpikeCounts[preTime]++;
    }
    for (Timestamp postTime : postSpikes) {
        pImpl->postSpikeCounts[postTime]++;
    }
    
    float totalDelta = 0.0f;
    
    // Compute LTP from pre-post spike pairs with triplet contributions
    totalDelta += computeTripletLTP(preSpikes, postSpikes, currentTime);
    
    // Compute LTD from post-pre spike pairs with triplet contributions
    totalDelta += computeTripletLTD(preSpikes, postSpikes, currentTime);
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA1Plus(float weight) {
    pImpl->a1Plus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getA1Plus() const {
    return pImpl->a1Plus;
}

void TripletSTDP::setA2Plus(float weight) {
    pImpl->a2Plus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getA2Plus() const {
    return pImpl->a2Plus;
}

void TripletSTDP::setA1Minus(float weight) {
    pImpl->a1Minus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getA1Minus() const {
    return pImpl->a1Minus;
}

void TripletSTDP::setA2Minus(float weight) {
    pImpl->a2Minus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getA2Minus() const {
    return pImpl->a2Minus;
}

void TripletSTDP::setTauPlus(float tau) {
    pImpl->tauPlus = std::clamp(tau, 5.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 5.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::setTripletWindow(float window) {
    pImpl->tripletWindow = std::clamp(window, 50.0f, 1000.0f);
}

float TripletSTDP::getTripletWindow() const {
    return pImpl->tripletWindow;
}

void TripletSTDP::configure(float a1Plus, float a2Plus, float a1Minus, float a2Minus,
                          float tauPlus, float tauMinus, float window) {
    setA1Plus(a1Plus);
    setA2Plus(a2Plus);
    setA1Minus(a1Minus);
    setA2Minus(a2Minus);
    setTauPlus(tauPlus);
    setTauMinus(tauMinus);
    setTripletWindow(window);
}

float TripletSTDP::computeTripletLTP(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration currentTime) const {
    float totalLTP = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0 && dt < pImpl->tripletWindow) {
                // Base pre-post contribution
                float contribution = getPrePostContribution(dt);
                totalLTP += contribution;
                
                // Add triplet contribution (pre-post-post)
                // Count post spikes that occur after this pre spike within window
                int postCount = 0;
                for (Timestamp laterPost : postSpikes) {
                    if (laterPost > preTime && laterPost - preTime < pImpl->tripletWindow) {
                        postCount++;
                    }
                }
                if (postCount > 0) {
                    float tripletContribution = getPrePostPostContribution(dt, postCount - 1); // -1 because we already counted the postTime spike
                    totalLTP += tripletContribution;
                }
            }
        }
    }
    
    return totalLTP;
}

float TripletSTDP::computeTripletLTD(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration currentTime) const {
    float totalLTD = 0.0f;
    
    for (Timestamp postTime : postSpikes) {
        for (Timestamp preTime : preSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt < 0 && dt > -pImpl->tripletWindow) {
                // Base post-pre contribution
                float contribution = getPostPreContribution(dt);
                totalLTD += contribution;
                
                // Add triplet contribution (post-pre-pre)
                // Count pre spikes that occur before this post spike within window
                int preCount = 0;
                for (Timestamp earlierPre : preSpikes) {
                    if (earlierPre < postTime && postTime - earlierPre < pImpl->tripletWindow) {
                        preCount++;
                    }
                }
                if (preCount > 0) {
                    float tripletContribution = getPostPrePreContribution(dt, preCount - 1); // -1 because we already counted the preTime spike
                    totalLTD += tripletContribution;
                }
            }
        }
    }
    
    return totalLTD;
}

float TripletSTDP::getPrePostContribution(float dt) const {
    return pImpl->a1Plus * std::exp(-dt / pImpl->tauPlus);
}

float TripletSTDP::getPostPreContribution(float dt) const {
    // dt is negative for post before pre
    return -pImpl->a1Minus * std::exp(dt / pImpl->tauMinus);
}

float TripletSTDP::getPrePostPostContribution(float dt, int postCount) const {
    // Enhanced LTP from triplet pattern pre-post-post
    return pImpl->a2Plus * std::exp(-dt / pImpl->tauPlus) * (postCount + 1);
}

float TripletSTDP::getPostPrePreContribution(float dt, int preCount) const {
    // Enhanced LTD from triplet pattern post-pre-pre
    return -pImpl->a2Minus * std::exp(dt / pImpl->tauMinus) * (preCount + 1);
}

} // namespace nlm