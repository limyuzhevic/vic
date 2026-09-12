#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::update(float error, TimestepDuration dt) {
    pImpl->error = error;
    pImpl->predictedValue = error;
    pImpl->actualValue = error;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::applyPlasticityModulation(const Neuromodulator& source) const {
    // Apply plasticity modulation based on prediction error
    // Positive error (better than predicted) increases plasticity
    // Negative error (worse than predicted) decreases plasticity
    float magnitude = std::abs(pImpl->error);
    if (magnitude > 0.0f) {
        // Modulate plasticity rate based on error
        // This would normally affect synaptic plasticity
        // For now, just store the effect
        pImpl->actualValue = magnitude;
    }
}

void PredictionError::applyDopamineModulation(const Dopamine& dopamine) {
    // Prediction error interacts with dopamine for reward-based learning
    // Stronger prediction errors modulate dopamine signaling
    float magnitude = std::abs(pImpl->error);
    if (magnitude > 0.0f) {
        // Scale dopamine based on prediction error magnitude
        float dopamineBoost = magnitude * 2.0f;
        dopamine.setLevel(dopamine.getLevel() + dopamineBoost * 0.1f);
    }
}

} // namespace nlm
