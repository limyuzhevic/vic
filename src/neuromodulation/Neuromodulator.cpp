#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float predictionError;
    float plasticityFactor;
    std::vector<float> rewardHistory;
    
    Impl() : level(0.0f), predictionError(0.0f), plasticityFactor(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(1.0f, level));
    // Dopamine affects plasticity
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.5f;  // More dopamine = more plasticity
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::signalReward(float reward) {
    setLevel(pImpl->level * 0.9f + reward * 0.1f);  // Smooth reward signal
    pImpl->rewardHistory.push_back(reward);
    
    // High reward = more exploration
    if (reward > 0.5f) {
        pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    
    // High prediction error = more learning
    if (std::abs(error) > 0.5f) {
        pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Reward decays over time
    pImpl->level *= 0.99f;
    
    // Smooth the level
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.5f;
}

struct Acetylcholine::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Acetylcholine::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // ACh effects:
    // 1. Enhances attention to salient stimuli
    // 2. Promotes memory consolidation
    // 3. Increases cortical excitability
    
    float level = getLevel();
    
    // Enhance attention
    if (brain_->getAttention()) {
        float attentionStrength = 0.5f + level * 0.5f;
        brain_->getAttention()->setInhibitionStrength(0.5f / attentionStrength);
        brain_->getAttention()->setExcitationStrength(1.0f * attentionStrength);
    }
    
    // Enhance memory consolidation (slower, longer-term effect)
    if (brain_->getEpisodicMemory() && level > 0.3f) {
        // ACh promotes consolidation of recent memories
        auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(5);
        for (auto* episode : recentEpisodes) {
            if (episode->age < 50) {  // Recent episode
                episode->weight = std::min(1.0f, episode->weight + 0.1f);
            }
        }
    }
    
    // Enhance neural excitability
    if (level > 0.5f) {
        // Inject current to increase neural excitability
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(0.1f * level);
                }
            }
        }
    }
}

struct Norepinephrine::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Norepinephrine::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // NE effects:
    // 1. Increases arousal and vigilance
    // 2. Enhances encoding of new memories
    // 3. Modulates stress response
    
    float level = getLevel();
    
    // Increase arousal/vigilance
    if (level > 0.3f) {
        // Higher NE = higher alertness
        alertnessLevel_ = 0.5f + level * 0.5f;
        
        // Increase sensory processing
        if (brain_->getAttention()) {
            brain_->getAttention()->setExcitationStrength(1.5f + level * 0.5f);
        }
    }
    
    // Enhance memory encoding
    if (level > 0.4f && brain_->getEpisodicMemory()) {
        // NE enhances encoding of new episodic memories
        // This would be called when new sensory input is processed
    }
    
    // Stress modulation (chronic high NE = stress)
    if (level > 0.8f) {
        // High stress impairs some cognitive functions
        if (brain_->getAttention()) {
            brain_->getAttention()->setInhibitionStrength(0.8f);
        }
        
        // Stress increases arousal
        alertnessLevel_ = std::min(1.0f, alertnessLevel_ + 0.1f);
    }
}

float Norepinephrine::getAlertnessLevel() const {
    return alertnessLevel_;
}

struct Serotonin::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Serotonin::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // 5-HT effects:
    // 1. Regulates mood
    // 2. Reduces impulsivity
    // 3. Inhibits avoidance behavior
    // 4. Modulates social behavior
    
    float level = getLevel();
    
    // Mood regulation
    moodLevel_ = 0.5f + level * 0.5f - 0.2f;  // Higher 5-HT = better mood
    moodLevel_ = std::clamp(moodLevel_, 0.0f, 1.0f);
    
    // Reduce impulsivity
    if (level > 0.5f) {
        // High serotonin = less impulsive actions
        if (brain_->getAttention()) {
            brain_->getAttention()->setInhibitionStrength(0.7f);
            brain_->getAttention()->setExcitationStrength(0.8f);
        }
    }
    
    // Social behavior modulation
    if (level > 0.7f && brain_->getSocialLearning()) {
        // High serotonin enhances social learning and cooperation
        socialCooperation_ = 0.8f;
    }
    
    // Avoid negative mood states
    if (moodLevel_ < 0.3f) {
        // Low mood = increase serotonin effects
        if (brain_->getAttention()) {
            brain_->getAttention()->setInhibitionStrength(0.6f);
            brain_->getAttention()->setExcitationStrength(1.2f);
        }
    }
}

float Serotonin::getMoodLevel() const {
    return moodLevel_;
}

float Serotonin::getSocialCooperation() const {
    return socialCooperation_;
}

void Serotonin::reduceImpulsivity() {
    // Serotonin specifically reduces impulsivity
    if (brain_ && brain_->getAttention()) {
        brain_->getAttention()->setInhibitionStrength(0.7f);
        brain_->getAttention()->setExcitationStrength(0.8f);
    }
}

} // namespace nlm
