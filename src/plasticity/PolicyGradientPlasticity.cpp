#include "PolicyGradientPlasticity.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PolicyGradientPlasticity::Impl {
    // Policy gradient parameters
    float learningRate;     // Learning rate (α)
    float policyTemperature;  // Policy temperature (β) for exploration
    float advantageEstimation;  // Advantage estimation factor (A)
    float valueFunctionWeight; // Value function weight (w)
    
    // Policy state
    float policyGradient;   // Policy gradient value
    float valueFunction;    // Value function estimate
    float logProbability;   // Log probability of current policy
    
    // Weight bounds
    float maxWeight;         // Maximum weight bound
    float minWeight;         // Minimum weight bound
    
    // Implementation details
    bool policyInitialized;  // Whether policy is initialized
    Timestamp lastUpdateTime; // Time of last update
    
    Impl() : learningRate(0.01f), policyTemperature(1.0f), advantageEstimation(1.0f),
             valueFunctionWeight(0.5f), policyGradient(0.0f), valueFunction(0.0f),
             logProbability(0.0f), maxWeight(1.0f), minWeight(-1.0f),
             policyInitialized(false), lastUpdateTime(0.0) {}
};

PolicyGradientPlasticity::PolicyGradientPlasticity() : pImpl(new Impl) {}

PolicyGradientPlasticity::~PolicyGradientPlasticity() = default;

void PolicyGradientPlasticity::update(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate policy gradient for weight update
    float currentWeight = synapse->getWeight();
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Estimate advantage for policy gradient
    float advantage = estimateAdvantage(currentWeight, postActivity);
    
    // Calculate policy gradient
    float gradient = calculatePolicyGradient(currentWeight, postActivity);
    
    // Update policy gradient with learning rate
    float delta = pImpl->learningRate * advantage * gradient;
    
    // Update value function
    float value = predictValue(currentWeight);
    pImpl->valueFunction = pImpl->valueFunctionWeight * value;
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
    
    // Update policy state
    pImpl->policyGradient = gradient;
    pImpl->logProbability = std::log(std::max(0.01f, currentWeight / (pImpl->maxWeight - pImpl->minWeight)));
    pImpl->lastUpdateTime = postSpikes.back();
}

void PolicyGradientPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* PolicyGradientPlasticity::getName() const {
    return "PolicyGradientPlasticity";
}

void PolicyGradientPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float PolicyGradientPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

void PolicyGradientPlasticity::setPolicyTemperature(float temperature) {
    pImpl->policyTemperature = std::clamp(temperature, 0.1f, 10.0f);
}

float PolicyGradientPlasticity::getPolicyTemperature() const {
    return pImpl->policyTemperature;
}

void PolicyGradientPlasticity::setAdvantageEstimation(float estimation) {
    pImpl->advantageEstimation = std::clamp(estimation, 0.0f, 10.0f);
}

float PolicyGradientPlasticity::getAdvantageEstimation() const {
    return pImpl->advantageEstimation;
}

void PolicyGradientPlasticity::setValueFunctionWeight(float weight) {
    pImpl->valueFunctionWeight = std::clamp(weight, 0.0f, 2.0f);
}

float PolicyGradientPlasticity::getValueFunctionWeight() const {
    return pImpl->valueFunctionWeight;
}

void PolicyGradientPlasticity::setMaxWeight(float max) {
    pImpl->maxWeight = std::clamp(max, 0.0f, 10.0f);
}

float PolicyGradientPlasticity::getMaxWeight() const {
    return pImpl->maxWeight;
}

void PolicyGradientPlasticity::setMinWeight(float min) {
    pImpl->minWeight = std::clamp(min, -10.0f, 0.0f);
}

float PolicyGradientPlasticity::getMinWeight() const {
    return pImpl->minWeight;
}

void PolicyGradientPlasticity::updatePolicyGradient(SynapticWeight weight, float advantage, float logProbability) {
    // Update policy gradient
    pImpl->policyGradient = pImpl->learningRate * advantage * (logProbability - pImpl->logProbability);
}

void PolicyGradientPlasticity::updateValueFunction(SynapticWeight weight, float value) {
    // Update value function estimate
    pImpl->valueFunction = pImpl->valueFunctionWeight * value;
}

float PolicyGradientPlasticity::calculatePolicyGradient(float weight, float action) const {
    // Calculate policy gradient using softmax policy
    float normalizedWeight = (weight - pImpl->minWeight) / (pImpl->maxWeight - pImpl->minWeight);
    float probability = std::exp(normalizedWeight / pImpl->policyTemperature) / 
                       (std::exp(weight / pImpl->policyTemperature) + std::exp(0.0f / pImpl->policyTemperature));
    
    // Gradient of log probability w.r.t. weight
    return probability * (1.0f - probability);
}

float PolicyGradientPlasticity::estimateAdvantage(float weight, float reward) const {
    // Estimate advantage using difference between received reward and baseline
    float baseline = predictValue(weight);
    return pImpl->advantageEstimation * (reward - baseline);
}

float PolicyGradientPlasticity::predictValue(float weight) const {
    // Simple value function prediction
    return weight * weight * 0.5f;  // Quadratic value function
}

} // namespace nlm