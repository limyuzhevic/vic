#include "CompetitiveHebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

struct CompetitiveHebbian::Impl {
    // Competitive Hebbian parameters
    float learningRate;     // Learning rate (η)
    float competitionFactor;  // Competition strength (α)
    
    // Weight bounds
    float maxWeight;         // Maximum weight bound
    float minWeight;         // Minimum weight bound
    
    // Competition state
    std::vector<float> weightHistory;      // History of weights for each synapse
    std::vector<float> activityHistory;    // History of activities for each synapse
    float globalActivitySum;              // Sum of activities for all competing synapses
    float globalWeightSum;                // Sum of weights for all competing synapses
    size_t maxHistorySize;                // Maximum size of history buffer
    size_t competitionCycle;              // Current competition cycle number
    
    Impl() : learningRate(0.01f), competitionFactor(0.5f), maxWeight(1.0f),
             minWeight(-1.0f), globalActivitySum(0.0f), globalWeightSum(0.0f),
             maxHistorySize(100), competitionCycle(0) {}
};

CompetitiveHebbian::CompetitiveHebbian() : pImpl(new Impl) {}

CompetitiveHebbian::~CompetitiveHebbian() = default;

void CompetitiveHebbian::update(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate pre-synaptic activity
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // For now, apply basic Hebbian learning with competition
    // In a full implementation, this would be called for each synapse
    // competing for the same post-synaptic neuron
    float delta = pImpl->learningRate * preActivity * postActivity;
    
    // Apply competition adjustment
    float currentWeight = synapse->getWeight();
    float targetActivity = pImpl->globalActivitySum > 0 ? pImpl->globalActivitySum : 1.0f;
    
    float competitionScore = calculateCompetitionScore(currentWeight, targetActivity);
    delta *= (1.0f - pImpl->competitionFactor * competitionScore);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void CompetitiveHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* CompetitiveHebbian::getName() const {
    return "CompetitiveHebbian";
}

void CompetitiveHebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float CompetitiveHebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void CompetitiveHebbian::setCompetitionFactor(float factor) {
    pImpl->competitionFactor = std::clamp(factor, 0.0f, 1.0f);
}

float CompetitiveHebbian::getCompetitionFactor() const {
    return pImpl->competitionFactor;
}

void CompetitiveHebbian::setMaxWeight(float max) {
    pImpl->maxWeight = std::clamp(max, 0.0f, 10.0f);
}

float CompetitiveHebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void CompetitiveHebbian::setMinWeight(float min) {
    pImpl->minWeight = std::clamp(min, -10.0f, 0.0f);
}

float CompetitiveHebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void CompetitiveHebbian::updateSynapseWeights(std::vector<Synapse*>& synapses, float postActivity, float baseline) {
    // Update weights for all synapses competing for the same post-synaptic neuron
    if (synapses.empty()) return;
    
    // Calculate global statistics
    pImpl->globalActivitySum = 0.0f;
    pImpl->globalWeightSum = 0.0f;
    
    for (auto* syn : synapses) {
        float weight = syn->getWeight();
        float activity = (weight > 0) ? postActivity : 0.0f;  // Simplified activity model
        pImpl->globalActivitySum += activity;
        pImpl->globalWeightSum += weight;
    }
    
    // Update each synapse's weight with competition
    for (auto* syn : synapses) {
        float currentWeight = syn->getWeight();
        float activity = (currentWeight > 0) ? postActivity : 0.0f;
        
        // Calculate Hebbian contribution
        float hebbianDelta = pImpl->learningRate * activity * baseline;
        
        // Calculate competition score
        float competitionScore = calculateCompetitionScore(currentWeight, pImpl->globalActivitySum);
        
        // Apply competition
        float totalDelta = hebbianDelta * (1.0f - pImpl->competitionFactor * competitionScore);
        
        // Apply weight change
        float newWeight = currentWeight + totalDelta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        syn->setWeight(newWeight);
        
        // Update history
        pImpl->weightHistory.push_back(newWeight);
        pImpl->activityHistory.push_back(activity);
        
        // Limit history size
        if (pImpl->weightHistory.size() > pImpl->maxHistorySize) {
            pImpl->weightHistory.erase(pImpl->weightHistory.begin());
            pImpl->activityHistory.erase(pImpl->activityHistory.begin());
        }
    }
}

float CompetitiveHebbian::calculateCompetitionScore(float currentWeight, float targetActivity) const {
    // Calculate competition score based on current weight vs target
    float normalizedWeight = (currentWeight - pImpl->minWeight) / (pImpl->maxWeight - pImpl->minWeight);
    float normalizedTarget = targetActivity / (pImpl->maxWeight - pImpl->minWeight);
    
    // Higher score indicates stronger competition
    return std::abs(normalizedWeight - normalizedTarget);
}

void CompetitiveHebbian::resetActivityHistory() {
    // Reset activity history for new competition cycle
    pImpl->weightHistory.clear();
    pImpl->activityHistory.clear();
    pImpl->globalActivitySum = 0.0f;
    pImpl->globalWeightSum = 0.0f;
    pImpl->competitionCycle++;
}

} // namespace nlm