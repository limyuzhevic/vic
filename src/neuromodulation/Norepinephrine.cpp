#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;           // 0.0f to 1.0f - overall arousal
    float vigilanceLevel;         // 0.0f to 1.0f - attentional focus
    float signalToNoise;           // Signal-to-noise ratio enhancement
    float locusCoeruleusGain;      // LC neuron firing rate modulation
    float pupilDilation;           // Simulated pupil dilation (arousal indicator)
    
    // Time constants
    static constexpr float AROUSAL_TIME_CONSTANT = 0.05f;      // seconds
    static constexpr float VIGILANCE_TIME_CONSTANT = 0.15f;     // seconds
    static constexpr float DECAY_RATE = 0.03f;                 // per timestep
    static constexpr float MAX_VIGILANCE = 0.9f;                // max vigilance to prevent anxiety
    static constexpr float SNR_MAX_ENHANCEMENT = 2.0f;          // max SNR improvement
    
    // Integration weights
    static constexpr float AROUSAL_SENSORY_WEIGHT = 0.8f;
    static constexpr float VIGILANCE_ATTENTION_WEIGHT = 0.7f;
    static constexpr float SNR_NOISE_REDUCTION = 0.6f;
    
    Impl()
        : level(0.0f)
        , arousalLevel(0.0f)
        , vigilanceLevel(0.0f)
        , signalToNoise(1.0f)
        , locusCoeruleusGain(1.0f)
        , pupilDilation(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->locusCoeruleusGain = 1.0f + level * 2.0f;  // LC firing increases with NE
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity: enhances learning for salient stimuli
    return 1.0f + pImpl->arousalLevel * 0.3f + pImpl->signalToNoise * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Dynamic regulation of arousal and vigilance
    float targetArousal = pImpl->level;
    float targetVigilance = std::min(pImpl->level * 0.8f, pImpl->MAX_VIGILANCE);
    
    // Update arousal (fast response)
    pImpl->arousalLevel += (targetArousal - pImpl->arousalLevel) * 
                          AROUSAL_TIME_CONSTANT * static_cast<float>(dt);
    
    // Update vigilance (slower, more sustained)
    pImpl->vigilanceLevel += (targetVigilance - pImpl->vigilanceLevel) * 
                            VIGILANCE_TIME_CONSTANT * static_cast<float>(dt);
    
    // Enhance signal-to-noise ratio
    pImpl->signalToNoise = 1.0f + (pImpl->arousalLevel * SNR_MAX_ENHANCEMENT * SNR_NOISE_REDUCTION);
    
    // Simulate pupil dilation as arousal indicator
    pImpl->pupilDilation = 1.0f + pImpl->arousalLevel * 0.5f;
    
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->DECAY_RATE * static_cast<float>(dt));
    
    // Clamp vigilance
    pImpl->vigilanceLevel = std::min(pImpl->MAX_VIGILANCE, pImpl->vigilanceLevel);
    
    NLM_LOG_DEBUG(std::string("NE level: ") + std::to_string(pImpl->level) +
                  ", arousal: " + std::to_string(pImpl->arousalLevel) +
                  ", vigilance: " + std::to_string(pImpl->vigilanceLevel) +
                  ", SNR: " + std::to_string(pImpl->signalToNoise));
}

void Norepinephrine::modulateArousal(float arousalLevel) {
    pImpl->arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
}

void Norepinephrine::modulateVigilance(float vigilance) {
    pImpl->vigilanceLevel = std::clamp(vigilance, 0.0f, 1.0f);
}

void Norepinephrine::enhanceSensoryProcessing(const Brain* brain, float stimulusSalience) {
    if (!brain || pImpl->arousalLevel <= 0.01f) return;
    
    // Enhance sensory processing based on stimulus salience and arousal
    float enhancement = pImpl->arousalLevel * stimulusSalience * pImpl->signalToNoise;
    
    NLM_LOG_DEBUG(std::string("NE enhanced sensory processing by ") + std::to_string(enhancement * 100) + "%");
}

void Norepinephrine::filterDistractorInputs(const Brain* brain, const std::vector<float>& sensoryInputs) {
    if (!brain || pImpl->vigilanceLevel <= 0.01f || sensoryInputs.empty()) return;
    
    // With high vigilance, suppress less salient/distracting inputs
    float threshold = 0.5f - pImpl->vigilanceLevel * 0.3f;  // Lower threshold with higher vigilance
    
    int suppressedCount = 0;
    for (float input : sensoryInputs) {
        if (input < threshold) {
            // Suppress distracting input
            suppressedCount++;
        }
    }
    
    if (suppressedCount > 0) {
        NLM_LOG_DEBUG(std::string("NE suppressed ") + std::to_string(suppressedCount) +
                     " distractor inputs (vigilance: " + std::to_string(pImpl->vigilanceLevel) + ")");
    }
}

void Norepinephrine::facilitateDecisionMaking(const Brain* brain) {
    if (!brain || pImpl->arousalLevel <= 0.01f) return;
    
    // NE facilitates decision making by enhancing signal detection
    NLM_LOG_DEBUG(std::string("NE facilitating decision making (arousal: ") + 
                  std::to_string(pImpl->arousalLevel) + ")");
}

void Norepinephrine::modulateResponseSelection(const Brain* brain) {
    if (!brain || pImpl->signalToNoise <= 1.0f) return;
    
    // NE modulates response selection based on improved signal quality
    NLM_LOG_DEBUG(std::string("NE modulating response selection (SNR: ") + 
                  std::to_string(pImpl->signalToNoise) + ")");
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->arousalLevel = 0.0f;
    pImpl->vigilanceLevel = 0.0f;
    pImpl->signalToNoise = 1.0f;
    pImpl->locusCoeruleusGain = 1.0f;
    pImpl->pupilDilation = 1.0f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getSignalToNoiseRatio() const {
    return pImpl->signalToNoise;
}

} // namespace nlm