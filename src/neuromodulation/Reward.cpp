#include "Reward.hpp"
#include "../sensory/Observation.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    float decayRate;
    float baselineReward;
    float maxReward;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), decayRate(0.01f), 
             baselineReward(0.0f), maxReward(1.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = std::clamp(value, pImpl->baselineReward, pImpl->maxReward);
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue = std::clamp(pImpl->currentValue + delta, 
                                     pImpl->baselineReward, pImpl->maxReward);
    pImpl->history.push_back(pImpl->currentValue);
    
    // Limit history size
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->currentValue = pImpl->baselineReward;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // IMPLEMENTED: Real reward computation from observation
    // Extracts meaningful features from sensory input to compute reward
    
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    // Calculate energy level reward (encourage approaching resources)
    float energy = observation.getInternal()[0];
    float energyReward = std::min(energy / 100.0f, 1.0f);
    
    // Calculate novelty reward (encourage exploration)
    float novelty = observation.getNovelty();
    float noveltyReward = novelty * 0.3f;
    
    // Calculate prediction error reward (encourage accurate predictions)
    float predictionError = observation.getPredictionError();
    float predictionErrorReward = std::min(predictionError / 50.0f, 0.5f);
    
    // Calculate exploration reward (encourage movement)
    float exploration = observation.getInternal()[1];
    float explorationReward = (exploration > 0.5f) ? 0.2f : 0.0f;
    
    // Weighted combination of reward components
    float totalReward = energyReward * 0.5f + 
                        noveltyReward * 0.2f + 
                        predictionErrorReward * 0.2f + 
                        explorationReward * 0.1f;
    
    return std::clamp(totalReward, 0.0f, 1.0f);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
