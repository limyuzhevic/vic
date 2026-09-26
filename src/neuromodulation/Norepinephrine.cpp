#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float arousal;
    float vigilance;
    float decayRate;
    float burstThreshold;
    float lastBurstTime;
    std::vector<float> history;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.1f), arousal(0.0f), vigilance(0.0f), 
             decayRate(0.05f), burstThreshold(0.8f), lastBurstTime(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine affects arousal and vigilance, modulating plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance based on level
    pImpl->arousal = pImpl->level * 0.7f;
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level;
    
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and vigilance");
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress response increases norepinephrine level
    pImpl->level = std::min(1.0f, pImpl->level + stressLevel * 0.3f);
    pImpl->lastBurstTime = static_cast<float>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
}

void Norepinephrine::signalNovelty(float novelty) {
    // Novelty detection increases arousal
    if (novelty > pImpl->burstThreshold) {
        pImpl->level = std::min(1.0f, pImpl->level + novelty * 0.4f);
    }
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

} // namespace nlm