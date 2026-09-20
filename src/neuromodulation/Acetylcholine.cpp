#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    // Core cholinergic state
    float level;                    // Current ACh concentration
    float baseline;                  // Tonic baseline level
    float phasicLevel;               // Phasic burst amplitude
    float pauseLevel;                // Phasic pause effect
    float decayRate;                 // General decay rate
    float riseRate;                  // Rise rate for bursts
    float reuptakeRate;              // Vesicular reuptake rate
    
    // Receptor subtype dynamics
    float muscarinicEffect;          // Effect on muscarinic receptors (slow, modulatory)
    float nicotinicEffect;           // Effect on nicotinic receptors (fast, excitatory)
    
    // Memory consolidation state
    float memoryConsolidationTimer;  // Timer for consolidation processes
    float memoryStrength;            // Accumulated memory strength
    
    // Attention modulation state
    float attentionHistory[10];       // Recent attention levels
    size_t attentionHistoryPos;      // Current position in history buffer
    
    // Burst-pause statistics
    float lastBurstTime;             // Time of last burst
    float burstFrequency;            // Recent burst frequency
    float pauseFrequency;            // Recent pause frequency
    
    // Vesicular dynamics
    float vesicularReleaseProb;      // Probability of vesicular release
    float vesiclePool;               // Available vesicle pool
    float vesicleRefillRate;         // Vesicle refill rate
    
    // Biological time constants (ms)
    float riseTime_;                 // ~10-50ms for rapid burst elevation
    float fallTime_;                 // ~100-500ms for decay
    float muscarinicTimeConstant_;   // ~1-5s for slow modulatory effects
    float nicotinicTimeConstant_;    // ~5-20ms for fast nicotinic effects
    
    // Random number generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.05f), phasicLevel(0.0f), pauseLevel(0.0f),
             decayRate(0.05f), riseRate(0.1f), reuptakeRate(0.02f),
             muscarinicEffect(0.0f), nicotinicEffect(0.0f),
             memoryConsolidationTimer(0.0f), memoryStrength(0.0f),
             attentionHistoryPos(0), lastBurstTime(0.0f), burstFrequency(0.0f),
             pauseFrequency(0.0f), vesicularReleaseProb(0.3f), vesiclePool(1.0f),
             vesicleRefillRate(0.1f), riseTime_(30.0f), fallTime_(200.0f),
             muscarinicTimeConstant_(2000.0f), nicotinicTimeConstant_(10.0f),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

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
    // ACh modulates plasticity through muscarinic receptors (LTP/LTD balance)
    float muscarinicModulation = pImpl->muscarinicEffect * 0.5f;
    float baselineModulation = 1.0f + muscarinicModulation * 0.2f;
    return std::clamp(baselineModulation, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt) / 1000.0f; // Convert to seconds
    
    // Decay level towards baseline
    float targetLevel = pImpl->baseline;
    pImpl->level += (targetLevel - pImpl->level) * pImpl->decayRate * deltaTime;
    
    // Add phasic burst contribution
    pImpl->level = std::min(pImpl->level + pImpl->phasicLevel * pImpl->riseRate * deltaTime, 1.0f);
    
    // Apply nicotinic receptor dynamics (fast, brief modulation)
    if (pImpl->nicotinicEffect > 0.0f) {
        pImpl->nicotinicEffect = std::max(0.0f, pImpl->nicotinicEffect - deltaTime / pImpl->nicotinicTimeConstant_);
    }
    
    // Apply muscarinic receptor dynamics (slow, sustained modulation)
    if (pImpl->muscarinicEffect > 0.0f) {
        pImpl->muscarinicEffect = std::max(0.0f, pImpl->muscarinicEffect - deltaTime / pImpl->muscarinicTimeConstant_);
    }
    
    // Update memory consolidation timer
    if (pImpl->memoryConsolidationTimer > 0.0f) {
        pImpl->memoryConsolidationTimer -= deltaTime;
        if (pImpl->memoryConsolidationTimer <= 0.0f) {
            // Consolidation completed, apply memory strength
            pImpl->memoryStrength = std::min(pImpl->memoryStrength + 0.1f, 1.0f);
            NLM_LOG_DEBUG("Acetylcholine: Memory consolidation completed, strength: {}", pImpl->memoryStrength);
        }
    }
    
    // Update vesicle pool
    pImpl->vesiclePool = std::min(pImpl->vesiclePool + pImpl->vesicleRefillRate * deltaTime, 1.0f);
}

void Acetylcholine::signalBurst() {
    // Phasic burst during attention or novelty
    float burstAmplitude = pImpl->phasicLevel;
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        // Biological burst: rapid increase in ACh release
        pImpl->level = std::min(pImpl->level + burstAmplitude * 10.0f, 1.0f);
        
        // Update nicotinic receptor activation (fast, excitatory)
        pImpl->nicotinicEffect = std::min(pImpl->nicotinicEffect + burstAmplitude, 1.0f);
        
        // Update attention history
        pImpl->attentionHistory[pImpl->attentionHistoryPos] = burstAmplitude;
        pImpl->attentionHistoryPos = (pImpl->attentionHistoryPos + 1) % 10;
        
        // Calculate burst frequency
        float currentTime = pImpl->lastBurstTime;
        pImpl->burstFrequency = (pImpl->burstFrequency + 1.0f) * 0.9f + 0.1f;
        pImpl->lastBurstTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        
        NLM_LOG_DEBUG("Acetylcholine: Burst fired, nicotinic effect: {}", pImpl->nicotinicEffect);
    } else {
        NLM_LOG_DEBUG("Acetylcholine: No vesicles available for burst");
    }
}

void Acetylcholine::signalPause() {
    // Phasic pause during memory consolidation
    pImpl->pauseLevel = 0.2f; // Reduced cholinergic tone during pause
    
    // Initiate memory consolidation process
    pImpl->memoryConsolidationTimer = 1000.0f; // 1 second consolidation period
    pImpl->memoryStrength = 0.0f;
    
    NLM_LOG_DEBUG("Acetylcholine: Pause initiated for memory consolidation");
}

void Acetylcholine::updateAttention(float attentionLevel, float noveltyLevel) {
    // Integrate attention and novelty signals to modulate cholinergic activity
    float attentionBoost = attentionLevel * noveltyLevel * 0.5f;
    
    // Higher novelty and attention trigger phasic bursts
    if (pImpl->uniformDist(pImpl->rng) < attentionBoost * 0.1f) {
        signalBurst();
    }
    
    // Update baseline based on average attention
    float avgAttention = 0.0f;
    for (int i = 0; i < 10; ++i) {
        avgAttention += pImpl->attentionHistory[i];
    }
    avgAttention /= 10.0f;
    
    // Modulate baseline level based on sustained attention
    pImpl->baseline = std::min(0.2f, pImpl->baseline + avgAttention * 0.01f);
}

void Acetylcholine::consolidateMemory(float memoryStrength) {
    // Enhance memory consolidation during cholinergic pauses
    if (memoryStrength > 0.5f) {
        // Strong memories benefit more from cholinergic consolidation
        pImpl->memoryStrength += memoryStrength * 0.5f;
        NLM_LOG_DEBUG("Acetylcholine: Enhanced memory consolidation, strength: {}", pImpl->memoryStrength);
    }
}

float Acetylcholine::getMuscarinicEffect() const {
    return pImpl->muscarinicEffect;
}

float Acetylcholine::getNicotinicEffect() const {
    return pImpl->nicotinicEffect;
}

void Acetylcholine::triggerVesicularRelease() {
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        NLM_LOG_DEBUG("Acetylcholine: Vesicular release triggered");
    }
}

void Acetylcholine::setReuptakeRate(float rate) {
    pImpl->reuptakeRate = std::clamp(rate, 0.0f, 1.0f);
}

} // namespace nlm