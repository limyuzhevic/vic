#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    // Core 5-HT state
    float level;                    // Current serotonin concentration
    float baseline;                  // Tonic baseline level
    float burstLevel;                // Burst firing amplitude
    float inhibitionLevel;           // Inhibitory signaling level
    float decayRate;                 // General decay rate
    float riseRate;                  // Rise rate for bursts
    float reuptakeRate;              // Vesicular reuptake rate
    
    // Receptor subtype dynamics
    float fiveHT1AEffect;            // 5-HT1A receptor activation (anxiolytic, inhibitory)
    float fiveHT2AEffect;            // 5-HT2A receptor activation (excitatory, cognitive)
    float fiveHT3Effect;             // 5-HT3 receptor activation (fast, excitatory)
    
    // Raphe nucleus firing statistics
    float tonicFiringRate;           // Current tonic firing rate (Hz)
    float burstFiringRate;           // Burst firing rate (Hz)
    float lastBurstTime;             // Time of last burst
    float firingPatternHistory[30];   // Recent firing pattern history
    size_t firingPatternHistoryPos;  // Current position in history buffer
    
    // Social behavior state
    float socialCohesion;            // Current social cohesion level
    float socialBonding;             // Social bonding strength
    float impulseControl;            // Impulse control level
    
    // Reuptake and clearance
    float vesicularReleaseProb;      // Probability of vesicular release
    float vesiclePool;               // Available vesicle pool
    float vesicleRefillRate;         // Vesicle refill rate
    
    // Biological time constants (ms)
    float riseTime_;                 // ~10-50ms for rapid burst elevation
    float fallTime_;                 // ~100-300ms for decay
    float fiveHT1ATimeConstant_;     // ~1-5s for 5-HT1A signaling
    float fiveHT2ATimeConstant_;     // ~100-500ms for 5-HT2A signaling
    float fiveHT3TimeConstant_;      // ~10-50ms for 5-HT3 signaling
    float rapheAdaptationTime_;      // Raphe nucleus firing adaptation (seconds)
    
    // Random number generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.1f), burstLevel(0.0f), inhibitionLevel(0.0f),
             decayRate(0.04f), riseRate(0.12f), reuptakeRate(0.025f),
             fiveHT1AEffect(0.0f), fiveHT2AEffect(0.0f), fiveHT3Effect(0.0f),
             tonicFiringRate(0.8f), burstFiringRate(0.0f), lastBurstTime(0.0f),
             firingPatternHistoryPos(0), socialCohesion(0.5f), socialBonding(0.5f),
             impulseControl(0.5f), vesicularReleaseProb(0.35f), vesiclePool(1.0f),
             vesicleRefillRate(0.12f), riseTime_(30.0f), fallTime_(250.0f),
             fiveHT1ATimeConstant_(3000.0f), fiveHT2ATimeConstant_(400.0f),
             fiveHT3TimeConstant_(30.0f), rapheAdaptationTime_(8000.0f),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f) {
        for (int i = 0; i < 30; ++i) firingPatternHistory[i] = 0.0f;
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity through 5-HT2A receptors (LTP/LTD balance)
    float fiveHT2AModulation = pImpl->fiveHT2AEffect * 0.2f;
    float baselineModulation = 1.0f + fiveHT2AModulation * 0.2f;
    return std::clamp(baselineModulation, 0.5f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt) / 1000.0f; // Convert to seconds
    
    // Decay level towards baseline
    float targetLevel = pImpl->baseline;
    pImpl->level += (targetLevel - pImpl->level) * pImpl->decayRate * deltaTime;
    
    // Add burst contribution
    pImpl->level = std::min(pImpl->level + pImpl->burstLevel * pImpl->riseRate * deltaTime, 1.0f);
    
    // Apply 5-HT1A receptor dynamics (inhibitory, anxiolytic)
    if (pImpl->fiveHT1AEffect > 0.0f) {
        pImpl->fiveHT1AEffect = std::max(0.0f, pImpl->fiveHT1AEffect - deltaTime / pImpl->fiveHT1ATimeConstant_);
    }
    
    // Apply 5-HT2A receptor dynamics (excitatory, cognitive)
    if (pImpl->fiveHT2AEffect > 0.0f) {
        pImpl->fiveHT2AEffect = std::max(0.0f, pImpl->fiveHT2AEffect - deltaTime / pImpl->fiveHT2ATimeConstant_);
    }
    
    // Apply 5-HT3 receptor dynamics (fast, excitatory)
    if (pImpl->fiveHT3Effect > 0.0f) {
        pImpl->fiveHT3Effect = std::max(0.0f, pImpl->fiveHT3Effect - deltaTime / pImpl->fiveHT3TimeConstant_);
    }
    
    // Update vesicle pool
    pImpl->vesiclePool = std::min(pImpl->vesiclePool + pImpl->vesicleRefillRate * deltaTime, 1.0f);
}

void Serotonin::signalModulatory() {
    // Modulatory signaling for general mood regulation
    pImpl->tonicFiringRate = std::min(pImpl->tonicFiringRate + 0.05f, 2.0f);
    
    // Increase baseline level gradually
    pImpl->level = std::min(pImpl->level + 0.005f, 0.2f);
    
    // Activate 5-HT1A receptors (anxiolytic)
    pImpl->fiveHT1AEffect = std::min(pImpl->fiveHT1AEffect + 0.2f, 1.0f);
    
    NLM_LOG_DEBUG("Serotonin: Modulatory signaling activated, 5-HT1A effect: {}", pImpl->fiveHT1AEffect);
}

void Serotonin::signalBurst() {
    // Burst firing for social behavior and reward processing
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        
        // Rapid increase in 5-HT release (burst response)
        pImpl->burstLevel = std::min(pImpl->burstLevel + 0.8f, 1.0f);
        pImpl->level = std::min(pImpl->level + pImpl->burstLevel * 3.0f, 1.0f);
        
        // Update receptor activations
        pImpl->fiveHT2AEffect = std::min(pImpl->fiveHT2AEffect + pImpl->burstLevel * 0.5f, 1.0f);
        pImpl->fiveHT3Effect = std::min(pImpl->fiveHT3Effect + pImpl->burstLevel, 1.0f);
        
        // Update firing statistics
        pImpl->firingPatternHistory[pImpl->firingPatternHistoryPos] = pImpl->burstFiringRate;
        pImpl->firingPatternHistoryPos = (pImpl->firingPatternHistoryPos + 1) % 30;
        pImpl->lastBurstTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        
        // Update social bonding based on burst firing
        pImpl->socialBonding = std::min(pImpl->socialBonding + 0.1f, 1.0f);
        
        NLM_LOG_DEBUG("Serotonin: Burst firing activated, 5-HT2A effect: {}", pImpl->fiveHT2AEffect);
    }
}

void Serotonin::signalInhibition() {
    // Inhibitory signaling for anxiety suppression and impulse control
    pImpl->inhibitionLevel = 0.3f;
    
    // Decrease baseline level temporarily
    pImpl->baseline = std::max(0.01f, pImpl->baseline - 0.05f);
    
    // Activate 5-HT1A receptors (anxiolytic, inhibitory)
    pImpl->fiveHT1AEffect = std::min(pImpl->fiveHT1AEffect + 0.5f, 1.0f);
    
    // Increase impulse control
    pImpl->impulseControl = std::min(pImpl->impulseControl + 0.2f, 1.0f);
    
    NLM_LOG_DEBUG("Serotonin: Inhibitory signaling activated, impulse control: {}", pImpl->impulseControl);
}

void Serotonin::updateSocialContext(float socialCohesion, float stressLevel) {
    // Update social behavior based on social context
    pImpl->socialCohesion = std::max(0.0f, pImpl->socialCohesion - 0.01f);
    pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + socialCohesion * 0.05f);
    
    // Stress affects serotonin release
    if (stressLevel > 0.5f) {
        pImpl->signalInhibition();
    }
    
    // High social cohesion promotes modulatory signaling
    if (pImpl->socialCohesion > 0.7f && pImpl->uniformDist(pImpl->rng) < 0.1f) {
        pImpl->signalBurst();
    }
}

void Serotonin::updateImpulseControl(float currentBehavior, float targetBehavior) {
    // Update impulse control based on behavioral regulation
    float behaviorError = std::abs(currentBehavior - targetBehavior);
    
    // Large errors trigger inhibitory signaling
    if (behaviorError > 0.3f) {
        pImpl->signalInhibition();
    }
    
    // Gradual adjustment of impulse control
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + behaviorError * 0.1f);
}

void Serotonin::setReuptakeRate(float rate) {
    pImpl->reuptakeRate = std::clamp(rate, 0.0f, 1.0f);
}

void Serotonin::triggerVesicularRelease() {
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        NLM_LOG_DEBUG("Serotonin: Vesicular release triggered");
    }
}

float Serotonin::getFiveHT1AEffect() const {
    return pImpl->fiveHT1AEffect;
}

float Serotonin::getFiveHT2AEffect() const {
    return pImpl->fiveHT2AEffect;
}

float Serotonin::getFiveHT3Effect() const {
    return pImpl->fiveHT3Effect;
}

} // namespace nlm