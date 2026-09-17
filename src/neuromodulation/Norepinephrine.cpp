#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    // NE state variables
    float level;                // Current norepinephrine concentration (nM)
    float baseline;             // Resting NE level (nM)
    float peak;                 // Maximum achievable level (nM)
    float decayRate;            // Decay rate (1/s)
    float releaseRate;          // Release rate coefficient
    float arousal;             // Arousal level (0-1)
    float vigilance;            // Vigilance level (0-1)
    float stress;               // Stress level (0-1)
    float locusCoeruleusGain;   // LC response gain
    
    // Biological parameters
    float tHalfMax;             // Time for half-maximal activation (s)
    float noradrenergicBaseline; // Tonic NE baseline rate (Hz)
    float phasicNEgain;          // Phasic NE gain
    
    // History tracking
    std::vector<float> history;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : level(8.0f)            // Baseline NE ~8 nM
        , baseline(8.0f)
        , peak(80.0f)            // Phasic peak ~80 nM
        , decayRate(0.6f)         // Decay half-life ~1.2s
        , releaseRate(4.0f)
        , arousal(0.5f)
        , vigilance(0.5f)
        , stress(0.1f)
        , locusCoeruleusGain(6.0f)
        , tHalfMax(0.4f)          // Fast response
        , noradrenergicBaseline(12.0f) // Tonic NE rate
        , phasicNEgain(10.0f)     // Phasic NE gain
        , dist(0.0f, 1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak * 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulation of plasticity based on arousal and stress
    // Moderate NE enhances plasticity, high NE can impair
    float plasticityMod = 1.0f;
    
    if (pImpl->level > pImpl->baseline) {
        // Phasic NE increase
        float excessNE = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
        
        // Arousal component: moderate arousal enhances plasticity (inverted U)
        if (pImpl->arousal < 0.5f) {
            plasticityMod = 1.0f + excessNE * 0.8f * pImpl->arousal;
        } else {
            plasticityMod = 1.0f + (1.0f - pImpl->arousal) * 0.5f * excessNE;
        }
        
        // Vigilance component: enhances signal detection
        plasticityMod *= (1.0f + pImpl->vigilance * 0.3f);
    }
    
    // Stress component: high stress can suppress plasticity
    if (pImpl->stress > 0.7f) {
        plasticityMod *= (1.0f - (pImpl->stress - 0.7f) * 1.5f);
    }
    
    return plasticityMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline with exponential decay
    float target = pImpl->baseline;
    float decayFactor = std::exp(-pImpl->decayRate * deltaTime);
    pImpl->level = target + (pImpl->level - target) * decayFactor;
    
    // Gradually normalize states
    pImpl->arousal = std::max(0.0f, pImpl->arousal - 0.02f * deltaTime);
    pImpl->vigilance = std::max(0.0f, pImpl->vigilance - 0.01f * deltaTime);
    pImpl->stress = std::max(0.0f, pImpl->stress - 0.005f * deltaTime);
    
    // Record state
    if (pImpl->history.size() < 100) {
        pImpl->history.push_back(pImpl->level);
    }
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Phasic NE burst during arousal/fight-or-flight response
    pImpl->arousal = std::clamp(arousalLevel, 0.0f, 1.0f);
    
    // Trigger phasic release from locus coeruleus
    float burst = arousalLevel * pImpl->releaseRate;
    float noise = pImpl->dist(pImpl->rng) * 0.08f * pImpl->phasicNEgain;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst + noise);
    
    NLM_LOG_DEBUG("Norepinephrine arousal burst: level=%.3f, NE level=%.2fnM", 
                  arousalLevel, pImpl->level);
}

void Norepinephrine::signalVigilance(float vigilanceStrength) {
    // NE increase during attentional vigilance
    pImpl->vigilance = std::clamp(vigilanceStrength, 0.0f, 1.0f);
    
    // Moderate release for vigilance
    float burst = vigilanceStrength * pImpl->releaseRate * 0.7f;
    float noise = pImpl->dist(pImpl->rng) * 0.06f * pImpl->phasicNEgain;
    
    pImpl->level = std::min(pImpl->peak * 1.2f, pImpl->level + burst + noise);
    
    NLM_LOG_DEBUG("Norepinephrine vigilance burst: strength=%.3f, NE level=%.2fnM", 
                  vigilanceStrength, pImpl->level);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousal;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stress;
}

void Norepinephrine::setStressSensitivity(float sensitivity) {
    pImpl->stress = std::clamp(sensitivity, 0.0f, 1.0f);
}

} // namespace nlm