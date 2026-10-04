#include "Anti_Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AntiHebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float depressionRate; // Depression rate (mu)
    float stabilizer;     // Stabilizer term (sigma)
    
    AntiHebbianType type; // Type of anti-Hebbian rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             depressionRate(0.01f), stabilizer(0.1f), type(SIMPLE_ANTIHEBBIAN) {}
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
    
    // Estimate activities from spike counts
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    float delta;
    
    switch (pImpl->type) {
        case SIMPLE_ANTIHEBBIAN:
            // Simple anti-Hebbian: Δw = -η * pre * post
            delta = -pImpl->learningRate * preActivity * postActivity;
            break;
            
        case THRESHOLD_ANTIHEBBIAN:
            // Threshold-based: Δw = -η * max(0, pre * post - threshold)
            float correlation = preActivity * postActivity;
            if (correlation > pImpl->stabilizer) {
                delta = -pImpl->learningRate * (correlation - pImpl->stabilizer);
            } else {
                delta = 0.0f;
            }
            break;
            
        case NORMALIZED_ANTIHEBBIAN:
            // Normalized: Δw = -η * (pre * post) / (pre^2 + post^2)
            float preSquared = preActivity * preActivity;
            float postSquared = postActivity * postActivity;
            float denominator = preSquared + postSquared;
            if (denominator > 1e-6f) {
                delta = -pImpl->learningRate * (preActivity * postActivity) / denominator;
            } else {
                delta = 0.0f;
            }
            break;
            
        case INTERGRANIC_ANTIHEBBIAN:
            // Inter-granic: Δw = -η * pre * post * exp(-|pre - post|)
            float activityDiff = std::abs(preActivity - postActivity);
            float expFactor = std::exp(-activityDiff);
            delta = -pImpl->learningRate * preActivity * postActivity * expFactor;
            break;
            
        default:
            delta = 0.0f;
    }
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbian::setDepressionRate(float rate) {
    pImpl->depressionRate = std::clamp(rate, 0.001f, 0.1f);
    pImpl->learningRate = pImpl->depressionRate;
}

float AntiHebbian::getDepressionRate() const {
    return pImpl->depressionRate;
}

void AntiHebbian::setStabilizer(float stabilizer) {
    pImpl->stabilizer = std::clamp(stabilizer, 0.01f, 1.0f);
}

float AntiHebbian::getStabilizer() const {
    return pImpl->stabilizer;
}

void AntiHebbian::setAntiHebbianType(AntiHebbianType type) {
    pImpl->type = type;
}

AntiHebbian::AntiHebbianType AntiHebbian::getAntiHebbianType() const {
    return pImpl->type;
}

} // namespace nlm
