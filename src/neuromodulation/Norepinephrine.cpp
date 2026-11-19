#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float facilitationRate;
    
    // NE effects on arousal and vigilance
    float arousalLevel;
    float vigilanceFactor;
    float stressLevel;
    
    // Track neurally excited neurons
    std::unordered_map<uint64_t, float> excitedNeurons;
    std::unordered_map<uint64_t, float> arousalHistory;
    
    // For learning acceleration under stress
    float stressLearningBoost;
    float arousalThreshold;
    
    Impl() : 
        level(0.0f), 
        baseline(0.0f), 
        peak(1.0f), 
        decayRate(0.08f), 
        releaseRate(2.0f), 
        facilitationRate(0.5f),
        arousalLevel(0.0f),
        vigilanceFactor(1.0f),
        stressLevel(0.0f),
        stressLearningBoost(0.0f),
        arousalThreshold(0.5f) {}
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
    // NE modulates plasticity: moderate levels enhance LTP, high levels suppress it
    if (pImpl->level < 0.4f) {
        return 1.0f + pImpl->level * 1.5f;  // Boost plasticity for learning under moderate arousal
    } else {
        return std::max(0.3f, 1.2f - pImpl->level);  // High NE suppresses plasticity for stability
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics modeling fight-or-flight responses
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Facilitation from stress/arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->facilitationRate * pImpl->stressLevel);
    
    // Update arousal and vigilance based on current NE level
    pImpl->arousalLevel = pImpl->level;
    pImpl->vigilanceFactor = 0.5f + 0.5f * pImpl->level;
    
    // Stress builds up over time and increases with negative events
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + pImpl->level * 0.1f);
}

void Norepinephrine::signalStress(float stressLevel) {
    // Stress signal increases NE level and arousal
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressLevel);
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate);
    
    // Learning boost under stress
    pImpl->stressLearningBoost = std::min(1.0f, pImpl->stressLevel * 0.5f);
}

void Norepinephrine::signalNovelty(float novelty) {
    // Novelty detection increases NE level for enhanced processing
    if (novelty > pImpl->arousalThreshold) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.7f);
        pImpl->vigilanceFactor = std::min(2.0f, pImpl->vigilanceFactor + novelty * 0.3f);
    }
}

void Norepinephrine::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    // Apply NE effects to neural dynamics
    float neMod = 1.0f + pImpl->level;  // Up to 2x modulation at max NE
    
    // Enhance arousal-dependent processing
    for (auto& pair : pImpl->excitedNeurons) {
        uint64_t neuronId = pair.first;
        float excitation = pair.second;
        
        // NE increases gain on excited neurons
        pImpl->arousalHistory[neuronId] += 0.05f * neMod * excitation;
    }
    
    // Update vigilance - modulates attention and information processing priority
    // Higher vigilance = more focused processing, less distractibility
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceFactor() const {
    return pImpl->vigilanceFactor;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

} // namespace nlm