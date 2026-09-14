#include "Oxytocin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Oxytocin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float releaseRate;
    float decayRate;
    float diffusionRate;
    float reuptakeRate;
    float socialBondingWeight;
    float trustWeight;
    float memoryIntegrationRate;
    float temporalHalfLife;
    float socialHistoryDecayRate;
    std::vector<float> socialBondHistory;
    float lastSocialEventTime;
    float accumulatedSocialCohesion;
    float trustScore;
    
    // Random generator for stochastic release
    std::mt19937 rng;
    std::uniform_real_distribution<float> releaseDistribution;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , releaseRate(2.0f)
        , decayRate(0.2f)
        , diffusionRate(0.15f)
        , reuptakeRate(0.3f)
        , socialBondingWeight(0.7f)
        , trustWeight(0.3f)
        , memoryIntegrationRate(0.1f)
        , temporalHalfLife(30.0f)
        , socialHistoryDecayRate(0.05f)
        , lastSocialEventTime(0.0f)
        , accumulatedSocialCohesion(0.0f)
        , trustScore(0.5f)
        , rng(std::random_device{}())
        , releaseDistribution(0.0f, 1.0f) {}
};

Oxytocin::Oxytocin() : pImpl(new Impl) {}

Oxytocin::~Oxytocin() = default;

void Oxytocin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Oxytocin system initialized for social bonding");
}

const char* Oxytocin::getName() const {
    return pImpl->getName();
}

float Oxytocin::getLevel() const {
    return pImpl->level;
}

void Oxytocin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Oxytocin::getPlasticityFactor() const {
    // Oxytocin enhances social memory plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Oxytocin::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Apply temporal dynamics (exponential decay based on half-life)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Diffusion - spreading effect to connected systems
    if (pImpl->brain && pImpl->diffusionRate > 0.0f) {
        // Oxytocin diffuses to influence working memory
        if (pImpl->brain->getWorkingMemory() && pImpl->brain->getWorkingMemory()->getMemoryNeurons().size() > 0) {
            float memoryLevel = pImpl->level * pImpl->diffusionRate;
            pImpl->brain->getWorkingMemory()->setSocialCohesionFactor(memoryLevel);
        }
    }
    
    // Reuptake - clearing from synaptic cleft
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->reuptakeRate * timeStep);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
    
    // Social history decay
    pImpl->socialBondHistory.clear();
    for (size_t i = 0; i < pImpl->socialBondHistory.size(); ++i) {
        pImpl->socialBondHistory[i] *= (1.0f - pImpl->socialHistoryDecayRate * timeStep);
    }
}

void Oxytocin::signalSocialBonding(float socialCohesion) {
    // Stochastic release with social context
    float releaseAmount = socialCohesion * pImpl->releaseRate;
    
    // Add random noise for biological realism
    float noise = pImpl->releaseDistribution(pImpl->rng) * 0.1f;
    releaseAmount += noise;
    
    // Cap at peak level
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    // Update social history
    pImpl->socialBondHistory.push_back(socialCohesion);
    pImpl->lastSocialEventTime = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
    
    // Integrate with memory systems
    if (pImpl->brain && pImpl->brain->getEpisodicMemory()) {
        pImpl->accumulatedSocialCohesion = (pImpl->accumulatedSocialCohesion * 0.9f) + (socialCohesion * 0.1f);
        
        // Enhance social memory formation
        if (pImpl->accumulatedSocialCohesion > 0.7f) {
            pImpl->brain->getEpisodicMemory()->enhanceSocialMemoryTrace(pImpl->accumulatedSocialCohesion);
        }
    }
    
    NLM_LOG_INFO("Oxytocin released: " + std::to_string(getLevel()) + " (social cohesion: " + std::to_string(socialCohesion) + ")");
}

void Oxytocin::signalTrust(float trustLevel) {
    // Trust signaling affects oxytocin level and social learning
    float trustSignal = trustLevel * pImpl->trustWeight;
    setLevel(std::min(pImpl->peak, getLevel() + trustSignal * 0.5f));
    
    // Update trust score
    pImpl->trustScore = (pImpl->trustScore * 0.9f) + (trustLevel * 0.1f);
    
    // Enhance social learning capacity
    if (pImpl->brain && pImpl->trustScore > 0.6f) {
        pImpl->brain->getWorkingMemory()->setTrustLevel(pImpl->trustScore);
    }
    
    NLM_LOG_INFO("Oxytocin trust modulation: " + std::to_string(getLevel()) + " (trust: " + std::to_string(pImpl->trustScore) + ")");
}

float Oxytocin::getSocialBondHistory(size_t index) const {
    if (index < pImpl->socialBondHistory.size()) {
        return pImpl->socialBondHistory[index];
    }
    return 0.0f;
}

float Oxytocin::getTrustScore() const {
    return pImpl->trustScore;
}

const char* Oxytocin::Impl::getName() const {
    return "OT";
}

} // namespace nlm