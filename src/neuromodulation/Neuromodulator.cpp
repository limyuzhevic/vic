#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Dopamine implementation
struct Dopamine::Impl {
    float level;
    float plasticityFactor;
    float predictionError;
    const Brain* brain;
    bool rewardActive;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), predictionError(0.0f), brain(nullptr), rewardActive(false) {}
};

Dopamine::Dopamine() : pImpl(new Impl()) {}

Dopamine::~Dopamine() {
    delete pImpl;
}

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(2.0f, level));
    
    // Update plasticity factor based on dopamine level
    if (pImpl->level > 1.0f) {
        pImpl->plasticityFactor = 1.0f + (pImpl->level - 1.0f) * 0.5f;
    } else {
        pImpl->plasticityFactor = 1.0f - (1.0f - pImpl->level) * 0.3f;
    }
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Gradually decay dopamine level if no new rewards
    if (!pImpl->rewardActive) {
        pImpl->level = std::max(0.0f, pImpl->level - 0.01f * dt);
    }
    
    // Apply dopamine effects
    if (pImpl->brain && pImpl->level > 0.0f) {
        // Dopamine modulates neural excitability
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Higher dopamine increases excitability
                    float dopamineEffect = pImpl->level * 0.5f;
                    if (dopamineEffect > 0.0f) {
                        neuron->injectCurrent(dopamineEffect);
                    }
                }
            }
        }
        
        // Dopamine affects working memory consolidation
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            float consolidationBoost = pImpl->level * 0.3f;
            wm->setConsolidationBoost(consolidationBoost);
        }
        
        // Dopamine affects prediction error
        float errorMod = pImpl->predictionError * pImpl->level * 0.2f;
        if (auto* curiosity = pImpl->brain->getCuriosity()) {
            curiosity->setLevel(std::min(1.0f, curiosity->getLevel() + errorMod));
        }
    }
}

void Dopamine::initialize(const Brain* brain) {
    pImpl->brain = brain;
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardActive = true;
    pImpl->level = std::min(2.0f, pImpl->level + reward * 0.5f);
    
    NLM_LOG_INFO(std::string("Dopamine reward: ") + std::to_string(reward) + 
                 std::string(" (level: ") + std::to_string(pImpl->level) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    pImpl->level = std::min(2.0f, pImpl->level + std::abs(error) * 0.3f);
    
    NLM_LOG_INFO(std::string("Dopamine prediction error: ") + std::to_string(error) +
                 std::string(" (level: ") + std::to_string(pImpl->level) + ")");
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float attentionMod;
    float memoryMod;
    const Brain* brain;
    
    Impl() : level(0.0f), attentionMod(1.0f), memoryMod(1.0f), brain(nullptr) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl()) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(2.0f, level));
    
    // Update modulation factors
    pImpl->attentionMod = 1.0f + pImpl->level * 0.5f;
    pImpl->memoryMod = 1.0f + pImpl->level * 0.3f;
}

float Acetylcholine::getPlasticityFactor() const {
    return pImpl->attentionMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Gradually decay ACh level
    pImpl->level = std::max(0.0f, pImpl->level - 0.008f * dt);
    
    // Apply ACh effects
    if (pImpl->brain && pImpl->level > 0.0f) {
        // Enhanced attention from ACh
        if (auto* attention = pImpl->brain->getAttention()) {
            float attentionBoost = pImpl->level * 0.8f;
            float currentStrength = attention->getAttentionStrength();
            attention->setAttentionStrength(std::min(3.0f, currentStrength + attentionBoost));
            
            // ACh also affects inhibition strength
            float currentInhibition = attention->getInhibitionStrength();
            attention->setInhibitionStrength(std::max(0.1f, currentInhibition - pImpl->level * 0.2f));
        }
        
        // Modulates memory systems
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            float memoryBoost = pImpl->level * 0.6f;
            wm->setEncodingStrength(memoryBoost);
        }
        
        if (auto* em = pImpl->brain->getEpisodicMemory()) {
            float episodicBoost = pImpl->level * 0.7f;
            // Enhanced episodic memory encoding
        }
        
        // ACh affects curiosity and novelty detection
        if (auto* curiosity = pImpl->brain->getCuriosity()) {
            float curiosityBoost = pImpl->level * 0.4f;
            curiosity->setLevel(std::min(1.0f, curiosity->getLevel() + curiosityBoost));
        }
    }
}

void Acetylcholine::initialize(const Brain* brain) {
    pImpl->brain = brain;
}

float Acetylcholine::getAttentionMod() const {
    return pImpl->attentionMod;
}

float Acetylcholine::getMemoryMod() const {
    return pImpl->memoryMod;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float arousal;
    const Brain* brain;
    
    Impl() : level(0.0f), arousal(0.0f), brain(nullptr) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl()) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(2.0f, level));
    pImpl->arousal = pImpl->level;
}

float Norepinephrine::getPlasticityFactor() const {
    return pImpl->arousal;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Gradually decay NE level
    pImpl->level = std::max(0.0f, pImpl->level - 0.006f * dt);
    pImpl->arousal = pImpl->level;
    
    // Apply NE effects
    if (pImpl->brain && pImpl->level > 0.0f) {
        // Enhanced arousal affects neural excitability
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE increases baseline excitability
                    float arousalBoost = pImpl->arousal * 0.7f;
                    neuron->injectCurrent(arousalBoost * 0.5f);
                }
            }
        }
        
        // Modulates attention dynamics
        if (auto* attention = pImpl->brain->getAttention()) {
            float attentionResponse = pImpl->arousal * 0.5f;
            float currentStrength = attention->getAttentionStrength();
            attention->setAttentionStrength(std::min(2.5f, currentStrength + attentionResponse));
        }
        
        // NE affects novelty detection
        if (auto* novelty = pImpl->brain->getNovelty()) {
            float noveltyBoost = pImpl->arousal * 0.6f;
            novelty->setLevel(std::min(1.0f, novelty->getLevel() + noveltyBoost));
        }
    }
}

void Norepinephrine::initialize(const Brain* brain) {
    pImpl->brain = brain;
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float mood;
    const Brain* brain;
    
    Impl() : level(0.0f), mood(0.5f), brain(nullptr) {}
};

Serotonin::Serotonin() : pImpl(new Impl()) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(2.0f, level));
    pImpl->mood = std::min(1.0f, std::max(0.0f, 0.5f + (pImpl->level - 1.0f) * 0.2f));
}

float Serotonin::getPlasticityFactor() const {
    return pImpl->mood;
}

void Serotonin::update(TimestepDuration dt) {
    // Gradually decay 5-HT level
    pImpl->level = std::max(0.0f, pImpl->level - 0.005f * dt);
    
    // Apply 5-HT effects
    if (pImpl->brain && pImpl->level > 0.0f) {
        // Enhanced mood affects behavior
        if (pImpl->mood > 0.7f) {
            // Happy mood: increases exploration, reduces fear
            if (auto* curiosity = pImpl->brain->getCuriosity()) {
                float curiosityBoost = (pImpl->mood - 0.7f) * 1.5f;
                curiosity->setLevel(std::min(1.0f, curiosity->getLevel() + curiosityBoost));
            }
        }
        
        // 5-HT affects attention stability
        if (auto* attention = pImpl->brain->getAttention()) {
            float attentionStability = pImpl->mood * 0.8f;
            float currentStrength = attention->getAttentionStrength();
            attention->setAttentionStrength(std::min(2.0f, currentStrength + attentionStability));
        }
        
        // Serotonin affects working memory capacity
        if (auto* wm = pImpl->brain->getWorkingMemory()) {
            float capacityBoost = pImpl->mood * 0.3f;
            wm->setCapacityBoost(capacityBoost);
        }
    }
}

void Serotonin::initialize(const Brain* brain) {
    pImpl->brain = brain;
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

} // namespace nlm