#include "BCMRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMRule::Impl {
    // BCM rule parameters
    float learningRate;     // Learning rate (η)
    float threshold;        // BCM threshold (θ)
    float thetaMinus;       // θ- for long-term threshold
    float thetaPlus;        // θ+ for short-term threshold
    
    // Weight bounds
    float maxWeight;         // Maximum weight bound
    float minWeight;         // Minimum weight bound
    
    // Internal state
    float slidingThreshold;  // Current sliding threshold value
    float synapticActivity; // Current synaptic activity (firing rate)
    float runningAverage;    // Running average of synaptic activity
    float adaptationRate;    // Rate of threshold adaptation
    
    Impl() : learningRate(0.01f), threshold(0.5f), thetaMinus(0.3f), thetaPlus(0.7f),
             maxWeight(1.0f), minWeight(-1.0f), slidingThreshold(0.5f),
             synapticActivity(0.0f), runningAverage(0.0f), adaptationRate(0.01f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate post-synaptic activity
    // This is a simplified implementation using spike counts
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Update synaptic activity average
    pImpl->runningAverage = pImpl->adaptationRate * postActivity + (1.0f - pImpl->adaptationRate) * pImpl->runningAverage;
    pImpl->synapticActivity = postActivity;
    
    // Update sliding threshold
    // θ(t+1) = θ(t) + η*(postActivity - θ(t))
    pImpl->slidingThreshold += pImpl->adaptationRate * (postActivity - pImpl->slidingThreshold);
    
    // Calculate weight change using BCM rule
    // Δw = η * x * y * (y - θ)
    // Where x is pre-synaptic activity, y is post-synaptic activity, θ is threshold
    float delta = pImpl->learningRate * preActivity * postActivity * (postActivity - pImpl->slidingThreshold);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCMRule";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 2.0f);
}

float BCMRule::getThreshold() const {
    return pImpl->threshold;
}

void BCMRule::setThetaMinus(float thetaMinus) {
    pImpl->thetaMinus = std::clamp(thetaMinus, 0.0f, 2.0f);
}

float BCMRule::getThetaMinus() const {
    return pImpl->thetaMinus;
}

void BCMRule::setThetaPlus(float thetaPlus) {
    pImpl->thetaPlus = std::clamp(thetaPlus, 0.0f, 2.0f);
}

float BCMRule::getThetaPlus() const {
    return pImpl->thetaPlus;
}

void BCMRule::setMaxWeight(float max) {
    pImpl->maxWeight = std::clamp(max, 0.0f, 10.0f);
}

float BCMRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void BCMRule::setMinWeight(float min) {
    pImpl->minWeight = std::clamp(min, -10.0f, 0.0f);
}

float BCMRule::getMinWeight() const {
    return pImpl->minWeight;
}

void BCMRule::updateThreshold(Timestamp currentTime) {
    // Update sliding threshold based on activity
    // This method can be called from the simulation loop
    pImpl->slidingThreshold = pImpl->thetaMinus + (pImpl->thetaPlus - pImpl->thetaMinus) * pImpl->synapticActivity;
}

void BCMRule::resetThreshold() {
    // Reset threshold to θ-
    pImpl->slidingThreshold = pImpl->thetaMinus;
}

float BCMRule::getThreshold() const {
    // Get current threshold (same as sliding threshold in this simplified implementation)
    return pImpl->slidingThreshold;
}

float BCMRule::getSlidingThreshold() const {
    return pImpl->slidingThreshold;
}

} // namespace nlm