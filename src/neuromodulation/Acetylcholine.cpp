#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    // ACh state variables
    float level;                // Current acetylcholine concentration (nM)
    float baseline;             // Resting ACh level (nM)
    float peak;                 // Maximum achievable level (nM)
    float decayRate;            // Decay rate (1/s)
    float releaseRate;          // Release rate coefficient
    float attention;            // Attention level (0-1)
    float memory;               // Memory modulation strength (0-1)
    float signalNoiseRatio;     // Signal-to-noise ratio enhancement
    
    // Biological parameters
    float tHalfMax;             // Time for half-maximal activation (s)
    float acetylcholineBaseline; // Tonic ACh baseline rate (Hz)
    float cholinergicGain;       // Cholinergic response gain
    
    // History tracking
    std::vector<float> history;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : level(5.0f)            // Baseline ACh ~5 nM
        , baseline(5.0f)
        , peak(50.0f)            // Phasic peak ~50 nM
        , decayRate(0.8f)         // Decay half-life ~0.9s
        , releaseRate(3.0f)
        , attention(0.5f)
        , memory(0.5f)
        , signalNoiseRatio(1.0f)
        , tHalfMax(0.3f)          // Fast response
        , acetylcholineBaseline(8.0f) // Tonic ACh rate
        , cholinergicGain(8.0f)   // Cholinergic gain
        , dist(0.0f, 1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized");
}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak * 2.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances synaptic plasticity through attention and arousal
    // Promotes LTP in salient stimuli
    float plasticityBoost = 1.0f;
    
    if (pImpl->level > pImpl->baseline) {
        // Phasic ACh increase enhances plasticity
        float excessACh = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
        
        // Attention component: boosts plasticity for attended stimuli
        plasticityBoost = 1.0f + excessACh * 0.5f * (1.0f + pImpl->attention);
        
        // Memory component: enhances consolidation
        plasticityBoost *= (1.0f + pImpl->memory * 0.3f);
    }
    
    // Signal-to-noise improvement
    plasticityBoost *= pImpl->signalNoiseRatio;
    
    return plasticityBoost;
}

void Acetylcholine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline with exponential decay
    float target = pImpl->baseline;
    float decayFactor = std::exp(-pImpl->decayRate * deltaTime);
    pImpl->level = target + (pImpl->level - target) * decayFactor;
    
    // Gradually reduce attention and memory signals
    pImpl->attention = std::max(0.0f, pImpl->attention - 0.01f * deltaTime);
    pImpl->memory = std::max(0.0f, pImpl->memory - 0.005f * deltaTime);
    
    // Improve signal-to-noise ratio
    pImpl->signalNoiseRatio = std::min(2.0f, pImpl->signalNoiseRatio * 0.99f + 0.01f);
    
    // Record state
    if (pImpl->history.size() < 100) {
        pImpl->history.push_back(pImpl->level);
    }
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Phasic ACh burst during attentional focus
    pImpl->attention = std::clamp(attentionStrength, 0.0f, 1.0f);
    
    // Trigger phasic release
    float burst = attentionStrength * pImpl->releaseRate;
    float noise = pImpl->dist(pImpl->rng) * 0.05f * pImpl->cholinergicGain;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst + noise);
    
    // Enhance signal-to-noise ratio
    pImpl->signalNoiseRatio = 1.0f + attentionStrength * 1.5f;
    
    NLM_LOG_DEBUG("Acetylcholine attention burst: strength=%.3f, ACh level=%.2fnM", 
                  attentionStrength, pImpl->level);
}

void Acetylcholine::signalMemoryEvent(float memoryStrength) {
    // ACh enhances memory consolidation during awake state
    pImpl->memory = std::clamp(memoryStrength, 0.0f, 1.0f);
    
    // Sustained release for memory consolidation
    float sustained = memoryStrength * pImpl->releaseRate * 0.5f;
    float noise = pImpl->dist(pImpl->rng) * 0.03f * pImpl->cholinergicGain;
    
    pImpl->level = std::min(pImpl->peak * 1.5f, pImpl->level + sustained + noise);
    
    // Long-lasting SNR improvement
    pImpl->signalNoiseRatio = std::min(3.0f, pImpl->signalNoiseRatio + memoryStrength * 2.0f);
    
    NLM_LOG_DEBUG("Acetylcholine memory event: strength=%.3f, ACh level=%.2fnM", 
                  memoryStrength, pImpl->level);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attention;
}

float Acetylcholine::getMemoryModulation() const {
    return pImpl->memory;
}

} // namespace nlm