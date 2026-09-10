// FIXED: AntiHebbianRule.cpp - Full Implementation
#include "AntiHebbianRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float baselineActivity;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             baselineActivity(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // OPTIMIZATION: Replace O(n²) nested loops with O(n log n) algorithm
    // Sort spike times to enable efficient pair counting using binary search
    std::vector<Timestamp> sortedPre = preSpikes;
    std::vector<Timestamp> sortedPost = postSpikes;
    std::sort(sortedPre.begin(), sortedPre.end());
    std::sort(sortedPost.begin(), sortedPost.end());
    
    // Count correlated spike pairs using two-pointer technique
    // This reduces complexity from O(n²) to O(n + m) where n,m are spike counts
    size_t coincidences = 0;
    size_t i = 0, j = 0;
    
    while (i < sortedPre.size() && j < sortedPost.size()) {
        Timestamp preTime = sortedPre[i];
        Timestamp postTime = sortedPost[j];
        float timeDiff = static_cast<float>(postTime - preTime);
        
        if (std::abs(timeDiff) < 10.0f) {
            ++coincidences;
            // Advance both pointers since this pair is within window
            ++i;
            ++j;
        } else if (timeDiff < -10.0f) {
            // Post spike is too early, need later pre spike
            ++i;
        } else {
            // Post spike is too late, need earlier post spike
            ++j;
        }
    }
    
    if (coincidences > 0) {
        float delta = -pImpl->learningRate * static_cast<float>(coincidences);
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
