#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

struct HomeostaticPlasticity::Impl {
    float targetFiringRate;      // Target average firing rate (Hz)
    float homeostaticLearningRate; // How fast homeostasis acts
    float metaplasticityRate;     // Learning rate adaptation
    float scalingExponent;        // Power law scaling exponent (typically 0.5-2.0)
    size_t activityHistorySize;   // Number of time steps to average over
    
    // Activity history
    std::map<SynapseId, std::vector<float>> activityHistory;
    
    // Metaplasticity state
    float runningLearningRate;    // Current effective learning rate
    
    Impl() : targetFiringRate(5.0f), homeostaticLearningRate(0.001f),
             metaplasticityRate(0.0001f), scalingExponent(0.8f),
             activityHistorySize(100), runningLearningRate(0.01f) {}
};

HomeostaticPlasticity::HomeostaticPlasticity() : pImpl(new Impl) {}

HomeostaticPlasticity::~HomeostaticPlasticity() = default;

void HomeostaticPlasticity::update(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Update activity history
    float currentActivity = estimateNetworkActivity(synapse, preSpikes);
    
    if (pImpl->activityHistory.find(synapse->getId()) == pImpl->activityHistory.end()) {
        pImpl->activityHistory[synapse->getId()] = std::vector<float>(pImpl->activityHistorySize, 0.0f);
    }
    
    // Shift history and add new activity
    std::rotate(pImpl->activityHistory[synapse->getId()].rbegin(),
                pImpl->activityHistory[synapse->getId()].rbegin() + 1,
                pImpl->activityHistory[synapse->getId()].rend());
    pImpl->activityHistory[synapse->getId()].front() = currentActivity;
    
    // Calculate average activity over history
    float avgActivity = 0.0f;
    const auto& history = pImpl->activityHistory[synapse->getId()];
    if (!history.empty()) {
        avgActivity = std::accumulate(history.begin(), history.end(), 0.0f) / static_cast<float>(history.size());
    }
    
    // Apply homeostatic synaptic scaling based on activity difference from target
    float activityError = avgActivity - pImpl->targetFiringRate;
    
    if (std::abs(activityError) > 0.01f) {
        // Power law scaling: Δw ∝ (current - target)^β
        float weightChange = pImpl->homeostaticLearningRate * 
                            std::pow(std::abs(activityError), pImpl->scalingExponent) * 
                            (activityError > 0.0f ? -1.0f : 1.0f);  // Scale towards target
        
        // Apply weight change with bounds
        float newWeight = synapse->getWeight() + weightChange;
        
        // Clamp to specified bounds for scaling
        float scaleMin = -0.5f;
        float scaleMax = 0.5f;
        newWeight = std::clamp(newWeight, scaleMin, scaleMax);
        
        synapse->addToWeight(newWeight - synapse->getWeight());
    }
    
    // Update metaplasticity (learning rate adaptation)
    // Learning rate increases when error is large, decreases when error is small
    float metaplasticChange = pImpl->metaplasticityRate * (std::abs(activityError) - pImpl->runningLearningRate);
    pImpl->runningLearningRate += metaplasticChange;
    
    // Ensure learning rate stays within reasonable bounds
    pImpl->runningLearningRate = std::clamp(pImpl->runningLearningRate, 0.0001f, 0.1f);
}

void HomeostaticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply the weight change
    float newWeight = synapse->getWeight() + delta;
    
    // Apply homeostatic bounds
    float homeostaticMin = -0.5f;
    float homeostaticMax = 0.5f;
    newWeight = std::clamp(newWeight, homeostaticMin, homeostaticMax);
    
    synapse->setWeight(newWeight);
}

const char* HomeostaticPlasticity::getName() const {
    return "HomeostaticPlasticity";
}

void HomeostaticPlasticity::setTargetFiringRate(float target) {
    pImpl->targetFiringRate = std::max(0.0f, target);
}

float HomeostaticPlasticity::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

void HomeostaticPlasticity::setHomeostaticLearningRate(float rate) {
    pImpl->homeostaticLearningRate = std::max(0.0001f, rate);
}

float HomeostaticPlasticity::getHomeostaticLearningRate() const {
    return pImpl->homeostaticLearningRate;
}

void HomeostaticPlasticity::setMetaplasticityRate(float rate) {
    pImpl->metaplasticityRate = std::max(0.0f, rate);
}

float HomeostaticPlasticity::getMetaplasticityRate() const {
    return pImpl->metaplasticityRate;
}

void HomeostaticPlasticity::setScalingExponent(float exp) {
    pImpl->scalingExponent = std::max(0.1f, std::min(2.0f, exp));
}

float HomeostaticPlasticity::getScalingExponent() const {
    return pImpl->scalingExponent;
}

void HomeostaticPlasticity::setActivityHistorySize(size_t size) {
    pImpl->activityHistorySize = std::max(static_cast<size_t>(10), size);
}

size_t HomeostaticPlasticity::getActivityHistorySize() const {
    return pImpl->activityHistorySize;
}

void HomeostaticPlasticity::setScalingMinWeight(float min) {
    // Note: In implementation, these are hardcoded for now
    // Could be extended to allow per-synapse settings
}

float HomeostaticPlasticity::getScalingMinWeight() const {
    return -0.5f;
}

void HomeostaticPlasticity::setScalingMaxWeight(float max) {
    // Note: In implementation, these are hardcoded for now
}

float HomeostaticPlasticity::getScalingMaxWeight() const {
    return 0.5f;
}

void HomeostaticPlasticity::setScalingTarget(float target) {
    // Note: In implementation, target firing rate is used directly
}

float HomeostaticPlasticity::getScalingTarget() const {
    return pImpl->targetFiringRate;
}

void HomeostaticPlasticity::configure(float targetRate, float learningRate) {
    setTargetFiringRate(targetRate);
    setHomeostaticLearningRate(learningRate);
}

float HomeostaticPlasticity::estimateNetworkActivity(Synapse* synapse, const std::vector<Timestamp>& spikes) const {
    if (spikes.empty()) {
        return 0.0f;
    }
    
    // Simple firing rate estimate based on spike count and time span
    float timeSpan = static_cast<float>(spikes.back() - spikes.front());
    if (timeSpan <= 0.0f) {
        return static_cast<float>(spikes.size());
    }
    
    // Calculate firing rate (spikes per second)
    return static_cast<float>(spikes.size()) / (timeSpan / 1000.0f);
}

} // namespace nlm