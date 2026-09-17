#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    // Serotonin state variables
    float level;                // Current serotonin concentration (nM)
    float baseline;             // Resting serotonin level (nM)
    float peak;                 // Maximum achievable level (nM)
    float decayRate;            // Decay rate (1/s)
    float releaseRate;          // Release rate coefficient
    float mood;                 // Mood level (0-1)
    float impulsivity;          // Impulsivity level (0-1)
    float socialBehavior;       // Social behavior modulation (0-1)
    float rapheGain;            // DRN/DRN response gain
    
    // Biological parameters
    float tHalfMax;             // Time for half-maximal activation (s)
    float serotonergicBaseline; // Tonic serotonin rate (Hz)
    float phasic5HTgain;         // Phasic serotonin gain
    
    // History tracking
    std::vector<float> history;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : level(12.0f)           // Baseline 5-HT ~12 nM
        , baseline(12.0f)
        , peak(120.0f)           // Phasic peak ~120 nM
        , decayRate(0.4f)         // Decay half-life ~1.7s
        , releaseRate(3.5f)
        , mood(0.5f)
        , impulsivity(0.5f)
        , socialBehavior(0.5f)
        , rapheGain(7.0f)
        , tHalfMax(0.5f)          // Fast response
        , serotonergicBaseline(15.0f) // Tonic 5-HT rate
        , phasic5HTgain(8.0f)     // Phasic 5-HT gain
        , dist(0.0f, 1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin neuromodulator initialized");
}

Serotonin::~Serotonin() {
    delete pImpl;
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak * 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulation of plasticity based on mood and social behavior
    float plasticityMod = 1.0f;
    
    if (pImpl->level > pImpl->baseline) {
        // Phasic 5-HT increase
        float excess5HT = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
        
        // Mood component: positive mood enhances plasticity
        plasticityMod *= (1.0f + pImpl->mood * 0.4f);
        
        // Social behavior component: prosocial behavior promotes adaptive learning
        plasticityMod *= (1.0f + pImpl->socialBehavior * 0.3f);
        
        // Impulsivity component: high impulsivity reduces plasticity
        plasticityMod *= (1.0f - pImpl->impulsivity * 0.5f);
    }
    
    return plasticityMod;
}

void Serotonin::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline with exponential decay
    float target = pImpl->baseline;
    float decayFactor = std::exp(-pImpl->decayRate * deltaTime);
    pImpl->level = target + (pImpl->level - target) * decayFactor;
    
    // Gradually normalize states
    pImpl->mood = std::max(0.0f, pImpl->mood - 0.015f * deltaTime);
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - 0.01f * deltaTime);
    pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - 0.008f * deltaTime);
    
    // Record state
    if (pImpl->history.size() < 100) {
        pImpl->history.push_back(pImpl->level);
    }
}

void Serotonin::signalMoodChange(float moodLevel) {
    // Phasic 5-HT change during mood modulation
    pImpl->mood = std::clamp(moodLevel, 0.0f, 1.0f);
    
    // Moderate release based on mood
    float burst = moodLevel * pImpl->releaseRate;
    float noise = pImpl->dist(pImpl->rng) * 0.04f * pImpl->rapheGain;
    
    pImpl->level = std::min(pImpl->peak * 1.2f, pImpl->level + burst + noise);
    
    // Reduce impulsivity with positive mood
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - moodLevel * 0.3f);
    
    NLM_LOG_DEBUG("Serotonin mood change: mood=%.3f, 5-HT level=%.2fnM", 
                  moodLevel, pImpl->level);
}

void Serotonin::signalImpulseControl(float impulseStrength) {
    // 5-HT regulates impulsivity and impulse control
    pImpl->impulsivity = std::clamp(impulseStrength, 0.0f, 1.0f);
    
    // Burst release for impulse control
    float burst = (1.0f - impulseStrength) * pImpl->releaseRate * 0.8f;
    float noise = pImpl->dist(pImpl->rng) * 0.03f * pImpl->rapheGain;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst + noise);
    
    // Better impulse control increases social behavior
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + impulseStrength * 0.2f);
    
    NLM_LOG_DEBUG("Serotonin impulse control: strength=%.3f, 5-HT level=%.2fnM", 
                  impulseStrength, pImpl->level);
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // 5-HT modulation of social behavior
    pImpl->socialBehavior = std::clamp(socialStrength, 0.0f, 1.0f);
    
    // Moderate release for social behavior
    float burst = socialStrength * pImpl->releaseRate * 0.6f;
    float noise = pImpl->dist(pImpl->rng) * 0.035f * pImpl->rapheGain;
    
    pImpl->level = std::min(pImpl->peak * 1.5f, pImpl->level + burst + noise);
    
    NLM_LOG_DEBUG("Serotonin social behavior: strength=%.3f, 5-HT level=%.2fnM", 
                  socialStrength, pImpl->level);
}

float Serotonin::getMoodLevel() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivity;
}

void Serotonin::setReceptorSensitivity(float sensitivity) {
    pImpl->rapheGain = sensitivity * 10.0f; // Scale sensitivity
}

} // namespace nlm