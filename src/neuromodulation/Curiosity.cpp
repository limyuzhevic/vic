#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float peak;
    float decayRate;
    float noveltyWeight;
    float errorWeight;
    float explorationDrive;
    
    Impl() : level(0.0f), peak(1.0f), decayRate(0.03f), 
             noveltyWeight(0.6f), errorWeight(0.4f), explorationDrive(0.5f) {
        history.clear();
    }
    
    std::vector<float> history;
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->history.clear();
}

void Curiosity::update(float novelty, float predictionError, float dt) {
    // Curiosity is weighted combination of novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + pImpl->errorWeight * predictionError;
    
    // Smooth update (exponential moving average)
    float learningRate = 0.1f;
    float delta = target - pImpl->level;
    pImpl->level += learningRate * delta * dt;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dt);
    
    // Update exploration drive
    pImpl->explorationDrive = pImpl->level;
    
    NLM_LOG_INFO(std::string("Curiosity update: ") + std::to_string(pImpl->level) +
                " (novelty=" + std::to_string(novelty) +
                ", error=" + std::to_string(predictionError) + ")");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->errorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationDrive = 0.0f;
}

const std::vector<float>& Curiosity::getHistory() const {
    return pImpl->history;
}

void Curiosity::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
