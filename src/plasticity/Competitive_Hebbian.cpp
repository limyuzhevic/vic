#include "Competitive_Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>

namespace nlm {

struct CompetitiveHebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float competitionStrength; // Competition strength (alpha)
    
    // Competition management
    std::map<NeuronId, float> neuronActivities;  // Track activities for competition
    float maxTotalActivity;                     // Maximum total activity for normalization
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             competitionStrength(0.1f), maxTotalActivity(1.0f) {}
};

CompetitiveHebbian::CompetitiveHebbian() : pImpl(new Impl) {}

CompetitiveHebbian::~CompetitiveHebbian() = default;

void CompetitiveHebbian::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    NeuronId preNeuron = synapse->getSourceNeuron();
    NeuronId postNeuron = synapse->getDestinationNeuron();
    
    // Update activities for competition
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    pImpl->neuronActivities[preNeuron] = preActivity;
    pImpl->neuronActivities[postNeuron] = postActivity;
    
    // Calculate total activity for normalization
    float totalActivity = 0.0f;
    for (const auto& pair : pImpl->neuronActivities) {
        totalActivity += pair.second;
    }
    
    if (totalActivity > pImpl->maxTotalActivity) {
        pImpl->maxTotalActivity = totalActivity;
    }
    
    // Competitive Hebbian rule: Δw_ij = η * (x_i - x_j) * y
    // Normalize activities by maximum total activity
    float normalizedPreActivity = preActivity / (pImpl->maxTotalActivity + 1e-6f);
    float normalizedPostActivity = postActivity / (pImpl->maxTotalActivity + 1e-6f);
    
    float activityDifference = normalizedPreActivity - normalizedPostActivity;
    float delta = pImpl->learningRate * activityDifference * postActivity;
    
    // Apply competition strength
    delta *= (1.0f - pImpl->competitionStrength);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void CompetitiveHebbian::setCompetitionStrength(float strength) {
    pImpl->competitionStrength = std::clamp(strength, 0.0f, 1.0f);
}

float CompetitiveHebbian::getCompetitionStrength() const {
    return pImpl->competitionStrength;
}

void CompetitiveHebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float CompetitiveHebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void CompetitiveHebbian::addToCompetitionGroup(NeuronId neuron) {
    pImpl->neuronActivities[neuron] = 0.0f;
}

void CompetitiveHebbian::removeFromCompetitionGroup(NeuronId neuron) {
    pImpl->neuronActivities.erase(neuron);
}

} // namespace nlm
