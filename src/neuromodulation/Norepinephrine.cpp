#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    // Core NE state
    float level;                    // Current NE concentration
    float baseline;                  // Tonic baseline level
    float phasicLevel;               // Phasic burst amplitude
    float arousalLevel;             // Arousal modulation level
    float stressLevel;               // Stress response level
    float decayRate;                 // General decay rate
    float riseRate;                  // Rise rate for bursts
    float reuptakeRate;              // Vesicular reuptake rate
    
    // ADRB2 and ADRA1 receptor dynamics
    float betaAdrenergicEffect;      // ADRB2 receptor activation (plasticity)
    float alphaAdrenergicEffect;     // ADRA1 receptor activation (vascular tone)
    
    // Locus coeruleus firing statistics
    float tonicFiringRate;           // Current tonic firing rate (Hz)
    float phasicBurstCount;          // Number of recent phasic bursts
    float lastBurstTime;             // Time of last burst
    float firingHistory[20];         // Recent firing rate history
    size_t firingHistoryPos;         // Current position in history buffer
    
    // Reuptake and clearance
    float vesicularReleaseProb;      // Probability of vesicular release
    float vesiclePool;               // Available vesicle pool
    float vesicleRefillRate;         // Vesicle refill rate
    
    // Biological time constants (ms)
    float riseTime_;                 // ~5-20ms for rapid burst elevation
    float fallTime_;                 // ~100-300ms for decay
    float betaTimeConstant_;         // ~1-3s for ADRB2 signaling
    float alphaTimeConstant_;        // ~100-500ms for ADRA1 signaling
    float LCTimeConstant_;           // Locus coeruleus firing adaptation (seconds)
    
    // Random number generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.02f), phasicLevel(0.0f), arousalLevel(0.0f), stressLevel(0.0f),
             decayRate(0.08f), riseRate(0.15f), reuptakeRate(0.03f),
             betaAdrenergicEffect(0.0f), alphaAdrenergicEffect(0.0f),
             tonicFiringRate(0.5f), phasicBurstCount(0.0f), lastBurstTime(0.0f),
             firingHistoryPos(0), vesicularReleaseProb(0.4f), vesiclePool(1.0f),
             vesicleRefillRate(0.15f), riseTime_(15.0f), fallTime_(200.0f),
             betaTimeConstant_(2000.0f), alphaTimeConstant_(300.0f), LCTimeConstant_(5000.0f),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f) {
        for (int i = 0; i < 20; ++i) firingHistory[i] = 0.0f;
    }
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
    // NE modulates plasticity through ADRB2 receptors (LTP/LTD balance)
    float betaModulation = pImpl->betaAdrenergicEffect * 0.3f;
    float baselineModulation = 1.0f + betaModulation * 0.3f;
    return std::clamp(baselineModulation, 0.5f, 2.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt) / 1000.0f; // Convert to seconds
    
    // Decay level towards baseline
    float targetLevel = pImpl->baseline;
    pImpl->level += (targetLevel - pImpl->level) * pImpl->decayRate * deltaTime;
    
    // Add phasic burst contribution
    pImpl->level = std::min(pImpl->level + pImpl->phasicLevel * pImpl->riseRate * deltaTime, 1.0f);
    
    // Apply ADRB2 receptor dynamics (sustained modulatory effect)
    if (pImpl->betaAdrenergicEffect > 0.0f) {
        pImpl->betaAdrenergicEffect = std::max(0.0f, pImpl->betaAdrenergicEffect - deltaTime / pImpl->betaTimeConstant_);
    }
    
    // Apply ADRA1 receptor dynamics (faster signaling)
    if (pImpl->alphaAdrenergicEffect > 0.0f) {
        pImpl->alphaAdrenergicEffect = std::max(0.0f, pImpl->alphaAdrenergicEffect - deltaTime / pImpl->alphaTimeConstant_);
    }
    
    // Update vesicle pool
    pImpl->vesiclePool = std::min(pImpl->vesiclePool + pImpl->vesicleRefillRate * deltaTime, 1.0f);
}

void Norepinephrine::signalTonic() {
    // Tonic baseline firing with adenosine receptor modulation
    pImpl->tonicFiringRate = std::min(pImpl->tonicFiringRate + 0.1f, 2.0f);
    
    // Increase baseline level gradually
    pImpl->level = std::min(pImpl->level + 0.01f, 0.1f);
    
    NLM_LOG_DEBUG("Norepinephrine: Tonic firing activated, rate: {}", pImpl->tonicFiringRate);
}

void Norepinephrine::signalPhasic() {
    // Phasic burst response to salient stimuli
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        
        // Rapid increase in NE release (phasic response)
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + 1.0f, 1.0f);
        pImpl->level = std::min(pImpl->level + pImpl->phasicLevel * 5.0f, 1.0f);
        
        // Update ADRB2 receptor activation (plasticity)
        pImpl->betaAdrenergicEffect = std::min(pImpl->betaAdrenergicEffect + pImpl->phasicLevel, 1.0f);
        
        // Update firing statistics
        pImpl->firingHistory[pImpl->firingHistoryPos] = pImpl->tonicFiringRate;
        pImpl->firingHistoryPos = (pImpl->firingHistoryPos + 1) % 20;
        pImpl->lastBurstTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        pImpl->phasicBurstCount += 1.0f;
        
        NLM_LOG_DEBUG("Norepinephrine: Phasic burst fired, beta effect: {}", pImpl->betaAdrenergicEffect);
    }
}

void Norepinephrine::updateArousal(float salience, float stressLevel) {
    // Update arousal level based on salience and stress
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.01f); // Natural decay
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressLevel * 0.1f);
    
    // High salience triggers phasic bursts
    float burstProbability = salience * 0.2f;
    if (pImpl->uniformDist(pImpl->rng) < burstProbability) {
        signalPhasic();
    }
    
    // Update arousal level
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + salience * 0.05f);
    
    // Arousal affects tonic firing rate (LC adaptation)
    pImpl->tonicFiringRate = 0.5f + pImpl->arousalLevel * 1.5f;
}

void Norepinephrine::setReuptakeRate(float rate) {
    pImpl->reuptakeRate = std::clamp(rate, 0.0f, 1.0f);
}

void Norepinephrine::triggerVesicularRelease() {
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        NLM_LOG_DEBUG("Norepinephrine: Vesicular release triggered");
    }
}

float Norepinephrine::getBetaAdrenergicEffect() const {
    return pImpl->betaAdrenergicEffect;
}

float Norepinephrine::getAlphaAdrenergicEffect() const {
    return pImpl->alphaAdrenergicEffect;
}

} // namespace nlm