#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeConstant;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), timeConstant(5.0f) {
        history.clear();
    }
    
    std::vector<float> history;
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

const char* Dopamine::getEffectType() const {
    return "Reward";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    if (reward > 0.0f) {
        float burst = std::min(reward * pImpl->releaseRate * 2.0f, pImpl->peak - pImpl->level);
        pImpl->level += burst;
        pImpl->history.push_back(pImpl->level);
        
        NLM_LOG_INFO(std::string("Dopamine burst: +") + std::to_string(burst) +
                    " (reward=" + std::to_string(reward) + ")");
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    if (error != 0.0f) {
        float change = error * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(0.0f, pImpl->level + change);
        pImpl->history.push_back(pImpl->level);
        
        NLM_LOG_INFO(std::string("Dopamine RPE: ") + (error > 0.0f ? "+" : "") +
                    std::to_string(change) + " (error=" + std::to_string(error) + ")");
    }
}

float Dopamine::getPlasticityFactor() const {
    float normalized = pImpl->level / pImpl->peak;
    return 0.5f + 0.8f * std::exp(-4.0f * std::pow(normalized - 0.5f, 2.0f));
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    if (pImpl->baseline < 0.3f && pImpl->level > 0.3f) {
        pImpl->baseline = std::min(0.3f, pImpl->baseline + 0.01f * static_cast<float>(dt));
    }
}

void Dopamine::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

} // namespace nlm