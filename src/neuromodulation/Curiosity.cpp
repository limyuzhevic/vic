#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float baselineLevel;
    float explorationDrive;
    float noveltySensitivity;
    float learningMotivation;
    float attentionAllocation;
    float rewardForExploration;
};

Curiosity::Curiosity() : pImpl(new Impl()) {
    pImpl->level = 0.0f;
    pImpl->baselineLevel = 0.02f;
    pImpl->explorationDrive = 0.5f;
    pImpl->noveltySensitivity = 1.0f;
    pImpl->learningMotivation = 0.5f;
    pImpl->attentionAllocation = 0.5f;
    pImpl->rewardForExploration = 0.1f;
}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Curiosity affects exploration drive
    pImpl->explorationDrive = 0.2f + pImpl->level * 1.5f;
    if (pImpl->explorationDrive > 1.0f) pImpl->explorationDrive = 1.0f;
    
    // Curiosity affects learning motivation
    pImpl->learningMotivation = pImpl->level;
    
    // Curiosity allocates attention to novel stimuli
    pImpl->attentionAllocation = 0.3f + pImpl->level * 0.7f;
    
    NLM_LOG_DEBUG("Curiosity level set to " + std::to_string(level) + 
                  ", exploration: " + std::to_string(pImpl->explorationDrive) +
                  ", motivation: " + std::to_string(pImpl->learningMotivation));
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity enhances plasticity for novel learning
    return 1.0f + pImpl->level * 1.0f;  // Up to 2x plasticity
}

void Curiosity::update(TimestepDuration dt) {
    // Curiosity decays with exploration rewards
    pImpl->level *= std::pow(0.95f, dt);
    
    // Gradually return to baseline
    if (pImpl->level < pImpl->baselineLevel) {
        pImpl->level += (pImpl->baselineLevel - pImpl->level) * dt * 0.1f;
    }
}

void Curiosity::signalNovelty(float novelty) {
    // Novelty triggers curiosity
    float curiosityBoost = novelty * pImpl->noveltySensitivity;
    setLevel(std::min(pImpl->level + curiosityBoost, 1.0f));
    NLM_LOG_INFO("Curiosity triggered by novelty " + std::to_string(novelty));
}

void Curiosity::signalExplorationDrive() {
    // Exploration behavior reinforces curiosity
    setLevel(std::min(pImpl->level + 0.05f, 1.0f));
    pImpl->rewardForExploration += 0.01f;
    NLM_LOG_DEBUG("Exploration reinforces curiosity");
}

void Curiosity::reduceCuriosity() {
    // Novel experiences reduce curiosity through prediction
    setLevel(pImpl->level * 0.5f);
}

} // namespace nlm