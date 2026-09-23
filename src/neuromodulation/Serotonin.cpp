#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float rewardWeight;
    float moodWeight;
    float impulsivityWeight;
    float socialWeight;
    float decayRate;
    float recoveryRate;
    float synapticModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , rewardWeight(1.0f)
        , moodWeight(0.8f)
        , impulsivityWeight(0.6f)
        , socialWeight(0.7f)
        , decayRate(0.12f)
        , recoveryRate(0.08f)
        , synapticModulation(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin modulates mood, reward processing, and social behavior
    if (pImpl->brain) {
        // Modulate reward processing
        if (pImpl->brain->getDopamine()) {
            // Serotonin enhances dopamine response for balanced reward signaling
            float modulation = std::max(0.0f, 1.0f - pImpl->level);
            float currentDA = pImpl->brain->getDopamine()->getLevel();
            pImpl->brain->getDopamine()->setLevel(currentDA * modulation);
        }
        
        // Enhance social behavior and reduce aggression
        if (pImpl->brain->getCuriosity()) {
            // Serotonin promotes social curiosity and exploration
            float socialCuriosity = pImpl->level * 0.5f;
            // Modify curiosity system to be more socially oriented
            pImpl->brain->getCuriosity()->setPredictionErrorWeight(0.3f);
            pImpl->brain->getCuriosity()->setNoveltyWeight(0.7f);
        }
        
        // Reduce impulsivity and increase behavioral flexibility
        if (pImpl->brain->getAttention()) {
            // Higher serotonin = better impulse control
            float impulseControl = pImpl->level;
            pImpl->brain->getAttention()->setFocusStrength(0.3f * (1.0f - impulseControl) + 0.7f * impulseControl);
        }
        
        // Modulate synaptic plasticity
        pImpl->synapticModulation = pImpl->level * 0.5f;
        
        // Enhance learning from social experiences
        if (pImpl->brain->getEpisodicMemory()) {
            // Serotonin promotes social memory consolidation
            pImpl->brain->getEpisodicMemory()->boostSocialConsolidation(pImpl->level * 0.7f);
        }
        
        // Modify stress response (serotonin buffers stress)
        if (pImpl->level > 0.6f) {
            // High serotonin reduces stress reactivity
            if (pImpl->brain->getDevelopmentSystem()) {
                pImpl->brain->getDevelopmentSystem()->setStressResponse(pImpl->level * 0.3f);
            }
        }
    }
    
    // Natural dynamics with gradual changes
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = std::min(1.0f, pImpl->level + pImpl->recoveryRate * static_cast<float>(dt));
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for balanced learning
    return 1.0f - 0.3f * pImpl->level;  // Range: [0.7, 1.0]
}

void Serotonin::signalSocialConnection(float socialConnection, TimestepDuration dt) {
    // Serotonin increases with social bonding and connection
    float socialBoost = std::min(1.0f, socialConnection * 2.0f);
    pImpl->level = std::min(1.0f, pImpl->level + socialBoost * 0.4f);
}

void Serotonin::reduceImpulsivity(float stabilityGain, TimestepDuration dt) {
    // Increase serotonin to reduce impulsive behavior
    pImpl->level = std::min(1.0f, pImpl->level + stabilityGain * 0.3f);
}

void Serotonin::setSynapticModulation(float modulation) {
    pImpl->synapticModulation = modulation;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->synapticModulation = 0.0f;
}

} // namespace nlm