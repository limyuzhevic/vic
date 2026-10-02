#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals

Neuromodulator::Neuromodulator() = default;

// Neuromodulator implementations

// Dopamine implementation from Dopamine.cpp (moved here for Phase 2)
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorHistory[3];
    float rewardHistory[3];
    float timeSinceLastEvent;
    float burstIntensity;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , timeSinceLastEvent(0.0f), burstIntensity(0.0f) {
        predictionErrorHistory[0] = predictionErrorHistory[1] = predictionErrorHistory[2] = 0.0f;
        rewardHistory[0] = rewardHistory[1] = rewardHistory[2] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    float normalizedLevel = pImpl->level / pImpl->peak;
    float optimalLevel = 0.6f;
    float sigma = 0.2f;
    float plasticityFactor = std::exp(-std::pow(normalizedLevel - optimalLevel, 2) / (2 * sigma * sigma));
    float temporalMod = 1.0f;
    if (pImpl->timeSinceLastEvent < 1.0f) {
        temporalMod = std::max(0.5f, 1.0f - pImpl->timeSinceLastEvent);
    }
    return plasticityFactor * temporalMod;
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    pImpl->timeSinceLastEvent += deltaTime;
    float targetLevel = pImpl->baseline;
    float decay = pImpl->decayRate;
    if (pImpl->level > pImpl->baseline * 1.1f) {
        decay *= 1.5f;
    }
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * deltaTime);
    pImpl->burstIntensity = std::max(0.0f, pImpl->burstIntensity - 0.5f * deltaTime);
}

void Dopamine::signalReward(float reward) {
    for (int i = 2; i >= 0; --i) {
        pImpl->rewardHistory[i] = pImpl->rewardHistory[i-1];
    }
    pImpl->rewardHistory[0] = reward;
    float recentRewardRate = (pImpl->rewardHistory[0] + pImpl->rewardHistory[1] + pImpl->rewardHistory[2]) / 3.0f;
    float burstAmount = reward * pImpl->releaseRate;
    if (recentRewardRate > 0.5f) {
        burstAmount *= 0.5f;
    }
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    pImpl->timeSinceLastEvent = 0.0f;
    pImpl->burstIntensity = std::min(1.0f, pImpl->burstIntensity + 0.3f * reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    for (int i = 2; i >= 0; --i) {
        pImpl->predictionErrorHistory[i] = pImpl->predictionErrorHistory[i-1];
    }
    pImpl->predictionErrorHistory[0] = error;
    float recentErrorAvg = (pImpl->predictionErrorHistory[0] + pImpl->predictionErrorHistory[1] + pImpl->predictionErrorHistory[2]) / 3.0f;
    float errorSignal = error * pImpl->releaseRate;
    float temporalFilter = 0.7f + 0.3f * std::abs(recentErrorAvg);
    errorSignal *= temporalFilter;
    float newLevel = pImpl->level + errorSignal;
    newLevel = std::max(pImpl->baseline, std::min(pImpl->peak, newLevel));
    if (std::abs(error) > 1.0f) {
        newLevel *= 0.7f;
    }
    pImpl->level = newLevel;
    pImpl->timeSinceLastEvent = 0.0f;
}

// Acetylcholine implementation from Acetylcholine.cpp (moved here for Phase 2)
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionWeight;
    float memoryConsolidationRate;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f)
        , attentionWeight(0.8f), memoryConsolidationRate(0.5f) {}
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
    float normalizedLevel = pImpl->level / pImpl->peak;
    float plasticityFactor = 1.0f + 0.5f * normalizedLevel * pImpl->attentionWeight;
    plasticityFactor *= (1.0f + pImpl->memoryConsolidationRate * 0.2f);
    return std::clamp(plasticityFactor, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * deltaTime);
}

void Acetylcholine::signalAttention(float attention) {
    float burstAmount = attention * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
}

// Norepinephrine implementation from Norepinephrine.cpp (moved here for Phase 2)
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressResponse;
    float vigilanceFactor;
    
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.2f), releaseRate(1.0f)
        , arousalLevel(0.5f), stressResponse(0.0f), vigilanceFactor(1.0f) {}
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
    float normalizedLevel = pImpl->level / pImpl->peak;
    float optimalLevel = 0.3f;
    float sigma = 0.25f;
    float plasticityFactor = std::exp(-std::pow(normalizedLevel - optimalLevel, 2) / (2 * sigma * sigma));
    if (pImpl->stressResponse > 0.0f) {
        plasticityFactor *= (1.0f - pImpl->stressResponse * 0.3f);
    }
    plasticityFactor *= pImpl->vigilanceFactor;
    return std::clamp(plasticityFactor, 0.2f, 1.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * deltaTime);
}

void Norepinephrine::signalArousal(float arousal) {
    float burstAmount = arousal * pImpl->releaseRate;
    if (arousal > 0.5f) {
        burstAmount *= 1.5f;
        pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + 0.2f * arousal);
    } else {
        burstAmount *= 0.8f;
    }
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    pImpl->vigilanceFactor = std::min(2.0f, 1.0f + 0.5f * arousal);
}

// Serotonin implementation from Serotonin.cpp (moved here for Phase 2)
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float socialBehavior;
    float anxiety;
    
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.12f), releaseRate(1.0f)
        , mood(0.5f), impulsivity(0.0f), socialBehavior(0.5f), anxiety(0.0f) {}
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
    float normalizedLevel = pImpl->level / pImpl->peak;
    float plasticityFactor = 1.2f - 0.4f * normalizedLevel;
    plasticityFactor *= (1.0f - pImpl->anxiety * 0.2f);
    if (pImpl->socialBehavior > 0.7f) {
        plasticityFactor *= 1.1f;
    }
    return std::clamp(plasticityFactor, 0.5f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * deltaTime);
}

void Serotonin::signalMood(float mood) {
    float moodResponse = mood * pImpl->releaseRate;
    if (mood > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodResponse * 0.7f);
    } else {
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodResponse * 1.2f);
    }
    pImpl->mood = mood;
    pImpl->anxiety = std::max(0.0f, 0.5f - mood);
    if (mood > 0.7f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.1f);
    } else if (mood < 0.3f) {
        pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - 0.05f);
    }
}

} // namespace nlm
