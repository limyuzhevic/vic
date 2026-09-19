#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float riseRate;
    float attentionLevel;
    float memoryEnhancementFactor;
    float learningSignals[2]; // novelty, prediction error
    std::vector<float> memoryTrace;
    std::vector<float> attentionHistory;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.05f), 
              riseRate(2.0f), attentionLevel(0.0f), memoryEnhancementFactor(1.0f) {
        std::random_device rd;
        rng.seed(rd());
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized for attention and memory consolidation");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity, particularly LTP
    float basePlasticity = 0.5f;
    float enhancement = pImpl->level * 0.6f; // Up to 1.1x baseline
    return basePlasticity + enhancement;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention level (slower dynamics)
    pImpl->attentionLevel = pImpl->level * 0.8f;
    
    // Update memory enhancement factor
    pImpl->memoryEnhancementFactor = 1.0f + pImpl->level * 0.5f;
    
    // Decay history
    if (!pImpl->attentionHistory.empty()) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
    }
    pImpl->attentionHistory.push_back(pImpl->attentionLevel);
}

void Acetylcholine::signalLearningEvent(float novelty, float predictionError) {
    // ACh responds to novel and surprising events
    float learningSignal = novelty * 0.6f + std::abs(predictionError) * 0.4f;
    
    // Bursty release in response to learning signals
    float burstMagnitude = learningSignal * pImpl->riseRate * static_cast<float>(0.1); // Short burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Store learning signals for memory
    pImpl->learningSignals[0] = novelty;
    pImpl->learningSignals[1] = predictionError;
    
    NLM_LOG_DEBUG("Acetylcholine signaling learning event: novelty=%.3f, PE=%.3f", novelty, predictionError);
}

void Acetylcholine::enhanceMemoryEncoding(const std::vector<float>& pattern, float strength) {
    if (pImpl->brain == nullptr || pattern.empty()) return;
    
    // ACh enhances memory encoding during learning
    float encodingBoost = pImpl->level * strength * pImpl->memoryEnhancementFactor;
    
    // Store pattern with ACh-enhanced strength
    for (size_t i = 0; i < std::min(pattern.size(), static_cast<size_t>(1000)); ++i) {
        float enhancedValue = pattern[i] * encodingBoost;
        pImpl->memoryTrace.push_back(enhancedValue);
    }
    
    NLM_LOG_DEBUG("Acetylcholine enhanced memory encoding with boost factor: %.3f", encodingBoost);
}

void Acetylcholine::boostAttention(float focusLevel) {
    // Increase attention level
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + focusLevel * pImpl->riseRate * 0.01f);
    
    // Can boost ACh level as well
    if (focusLevel > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + focusLevel * 0.2f);
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionLevel = 0.0f;
    pImpl->memoryEnhancementFactor = 1.0f;
    pImpl->learningSignals[0] = 0.0f;
    pImpl->learningSignals[1] = 0.0f;
    pImpl->memoryTrace.clear();
    pImpl->attentionHistory.clear();
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

float Acetylcholine::getMemoryEnhancementFactor() const {
    return pImpl->memoryEnhancementFactor;
}

} // namespace nlm