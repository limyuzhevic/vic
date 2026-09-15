#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AntiHebbian::Impl {
    float learningRate;            // Learning rate for anti-Hebbian learning
    float depressionStrength;     // Maximum depression strength
    float minWeight;              // Most negative weight possible
    float maxWeight;              // Least negative weight possible
    float coactivityThreshold;    // Activity level to trigger depression
    float antiHebbianLearningRate; // Strength of anti-Hebbian effect
    
    // Activity history for comparison
    std::map<SynapseId, float> preActivityHistory;
    std::map<SynapseId, float> postActivityHistory;
    
    Impl() : learningRate(0.01f), depressionStrength(0.1f), minWeight(-0.5f), maxWeight(0.05f),
             coactivityThreshold(0.1f), antiHebbianLearningRate(0.05f) {}
};

AntiHebbian::AntiHebbian() : pImpl(new Impl) {}

AntiHebbian::~AntiHebbian() = default;

void AntiHebbian::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate current activity levels
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Store activity history
    pImpl->preActivityHistory[synapse->getId()] = preActivity;
    pImpl->postActivityHistory[synapse->getId()] = postActivity;
    
    // Anti-Hebbian principle: neurons that fire together, wire away
    // This means we want to decrease weight when pre and post neurons fire together
    // For inhibitory synapses (which are typically anti-Hebbian), this makes sense
    
    float totalDelta = 0.0f;
    
    // Calculate coactivity level (normalized)
    float coactivity = preActivity * postActivity;
    
    if (coactivity > pImpl->coactivityThreshold) {
        // Apply anti-Hebbian learning: weight decreases with coactivity
        // For inhibitory synapses, more negative weights are stronger inhibition
        float coactivityFactor = (coactivity - pImpl->coactivityThreshold) / pImpl->coactivityThreshold;
        coactivityFactor = std::min(1.0f, coactivityFactor);
        
        // Apply depression with anti-Hebbian learning rate
        float depression = pImpl->depressionStrength * coactivityFactor * pImpl->antiHebbianLearningRate;
        totalDelta = -depression;  // Negative delta = weight decrease
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void AntiHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply anti-Hebbian bounds (for inhibitory connections)
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* AntiHebbian::getName() const {
    return "AntiHebbian";
}

void AntiHebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float AntiHebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbian::setDepressionStrength(float strength) {
    pImpl->depressionStrength = std::clamp(strength, 0.01f, 1.0f);
}

float AntiHebbian::getDepressionStrength() const {
    return pImpl->depressionStrength;
}

void AntiHebbian::setMinimumWeight(float min) {
    pImpl->minWeight = std::clamp(min, -1.0f, 0.0f);
}

float AntiHebbian::getMinimumWeight() const {
    return pImpl->minWeight;
}

void AntiHebbian::setMaximumWeight(float max) {
    pImpl->maxWeight = std::clamp(max, -0.1f, 0.1f);
}

float AntiHebbian::getMaximumWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbian::setCoactivityThreshold(float threshold) {
    pImpl->coactivityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float AntiHebbian::getCoactivityThreshold() const {
    return pImpl->coactivityThreshold;
}

void AntiHebbian::setAntiHebbianLearningRate(float rate) {
    pImpl->antiHebbianLearningRate = std::clamp(rate, 0.0f, 0.1f);
}

float AntiHebbian::getAntiHebbianLearningRate() const {
    return pImpl->antiHebbianLearningRate;
}

void AntiHebbian::configure(float learningRate, float depressionStrength) {
    setLearningRate(learningRate);
    setDepressionStrength(depressionStrength);
}

} // namespace nlm