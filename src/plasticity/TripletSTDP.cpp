#include "../plasticity/TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float A_plus;         // LTP rate for pre-post pair
    float A_minus;        // LTD rate for post-pre pair
    float W_plus;         // Pre-post pair weight contribution
    float W_minus;        // Post-pre pair weight contribution
    float timeConstant;   // STDP time constant
    float tripletContribution;  // Weight of triplet contributions
    
    Impl() : A_plus(0.01f), A_minus(0.005f), W_plus(0.01f), W_minus(0.005f),
             timeConstant(20.0f), tripletContribution(0.5f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // OPTIMIZATION: Replace O(n²) nested loops with O(n log n) algorithm
    // Sort spike times to enable efficient pair processing using binary search
    std::vector<Timestamp> sortedPre = preSpikes;
    std::vector<Timestamp> sortedPost = postSpikes;
    std::sort(sortedPre.begin(), sortedPre.end());
    std::sort(sortedPost.begin(), sortedPost.end());
    
    // Calculate pairwise contributions (base STDP) using binary search
    // This reduces complexity from O(n²) to O((n+m) log m) 
    float totalWeightChange = 0.0f;
    
    for (Timestamp preTime : sortedPre) {
        // Use binary search to find relevant post-spikes within 200ms window
        auto lowerBound = std::lower_bound(sortedPost.begin(), sortedPost.end(), preTime);
        auto upperBound = std::upper_bound(sortedPost.begin(), sortedPost.end(), preTime + 200);
        
        // Process only post-spikes within the relevant time window
        for (auto it = lowerBound; it != upperBound; ++it) {
            float deltaT = static_cast<float>(*it - preTime);
            
            if (deltaT > 0) {  // Post after pre: LTP
                totalWeightChange += pImpl->A_plus * 
                                     std::exp(-deltaT / pImpl->timeConstant);
            }
        }
        
        // Process pre-spikes that occur before post-spikes (for LTD)
        auto lowerBoundPre = std::lower_bound(sortedPost.begin(), sortedPost.end(), preTime - 200);
        auto upperBoundPre = std::upper_bound(sortedPost.begin(), sortedPost.end(), preTime);
        
        for (auto it = lowerBoundPre; it != upperBoundPre; ++it) {
            float deltaT = static_cast<float>(preTime - *it);  // Reverse for LTD
            
            totalWeightChange += -pImpl->A_minus * 
                                 std::exp(-deltaT / pImpl->timeConstant);
        }
    }
    
    // Add triplet contributions if enabled
    if (pImpl->tripletContribution > 0.0f) {
        float tripletEffect = calculateTripletEffect(sortedPre, sortedPost, dt);
        totalWeightChange += pImpl->tripletContribution * tripletEffect;
    }
    
    // Apply total weight change
    if (std::abs(totalWeightChange) > 1e-6f) {
        applyWeightChange(synapse, totalWeightChange);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, 0.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA_plus(float A_plus) {
    pImpl->A_plus = std::clamp(A_plus, 0.0f, 0.1f);
}

float TripletSTDP::getA_plus() const {
    return pImpl->A_plus;
}

void TripletSTDP::setA_minus(float A_minus) {
    pImpl->A_minus = std::clamp(A_minus, 0.0f, 0.1f);
}

float TripletSTDP::getA_minus() const {
    return pImpl->A_minus;
}

void TripletSTDP::setW_plus(float W_plus) {
    pImpl->W_plus = std::clamp(W_plus, 0.0f, 0.1f);
}

float TripletSTDP::getW_plus() const {
    return pImpl->W_plus;
}

void TripletSTDP::setW_minus(float W_minus) {
    pImpl->W_minus = std::clamp(W_minus, 0.0f, 0.1f);
}

float TripletSTDP::getW_minus() const {
    return pImpl->W_minus;
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(1.0f, tau);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void TripletSTDP::setTripletContribution(float contribution) {
    pImpl->tripletContribution = std::clamp(contribution, 0.0f, 1.0f);
}

float TripletSTDP::getTripletContribution() const {
    return pImpl->tripletContribution;
}

float TripletSTDP::calculateTripletEffect(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt) {
    // OPTIMIZATION: Replace O(n²) nested loops with O(n log n) algorithm
    // Calculate triplet contributions using efficient pair counting
    float tripletEffect = 0.0f;
    
    // OPTIMIZATION: Use two-pointer technique for counting pre-post pairs
    // Count pre-post pairs with short intervals (≤ 50ms)
    size_t prePostPairs = 0;
    size_t i = 0, j = 0;
    
    while (i < preSpikes.size() && j < postSpikes.size()) {
        Timestamp preTime = preSpikes[i];
        Timestamp postTime = postSpikes[j];
        Timestamp timeDiff = postTime - preTime;
        
        if (timeDiff > 0 && timeDiff <= 50) {  // 50ms window for pre-post pairs
            ++prePostPairs;
            // Move both pointers since each pre spike can pair with multiple post spikes
            // For correct counting, we need to continue checking
            ++j;
        } else if (timeDiff <= 0) {
            // Pre spike is too early, move to later pre spike
            ++i;
        } else {
            // Post spike is too early for this pre spike, move to later post spike
            ++j;
        }
    }
    
    // OPTIMIZATION: Use two-pointer technique for counting post-pre pairs
    // Count post-pre pairs with short intervals (≤ 50ms)
    size_t postPrePairs = 0;
    i = 0;
    j = 0;
    
    while (i < postSpikes.size() && j < preSpikes.size()) {
        Timestamp postTime = postSpikes[i];
        Timestamp preTime = preSpikes[j];
        Timestamp timeDiff = preTime - postTime;
        
        if (timeDiff > 0 && timeDiff <= 50) {  // 50ms window for post-pre pairs
            ++postPrePairs;
            // Move both pointers
            ++j;
        } else if (timeDiff <= 0) {
            // Post spike is too early, move to later post spike
            ++i;
        } else {
            // Pre spike is too early for this post spike, move to later pre spike
            ++j;
        }
    }
    
    tripletEffect = pImpl->W_plus * prePostPairs - pImpl->W_minus * postPrePairs;
    return tripletEffect;
}

} // namespace nlm