#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Real dopamine dynamics: 
    // - Low DA (0-0.3): High prediction error, exploratory learning
    // - Medium DA (0.3-0.7): Optimal learning state, balanced exploration/exploitation
    // - High DA (0.7-1.0): High prediction error, intense learning
    // - Very high DA: Synaptic saturation, reduced plasticity
    if (pImpl->level < 0.3f) {
        return 1.2f + 0.5f * pImpl->level;  // Increasing from baseline
    } else if (pImpl->level <= 0.7f) {
        return 1.35f;  // Peak plasticity zone
    } else {
        return 1.35f - 0.8f * (pImpl->level - 0.7f);  // Decreasing at high levels
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay towards baseline with exponential decay
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: dopamine bursts proportional to reward prediction error
    float burstSize = std::min(reward, 1.0f) * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize);
    
    if (reward > 0.0f) {
        NLM_LOG_INFO("Dopamine reward signal: +" + std::to_string(burstSize) + 
                    " (level = " + std::to_string(pImpl->level) + ")");
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Positive error = unexpected reward, increases dopamine
    // Negative error = expected but missing reward, decreases dopamine
    float errorResponse = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + errorResponse);
    
    if (std::abs(error) > 0.01f) {
        NLM_LOG_INFO("Dopamine prediction error: " + std::to_string(error) + 
                    " (level = " + std::to_string(pImpl->level) + ")");
    }
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionWeight;
    float noveltyWeight;
    
    Impl()
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.2f)
        , releaseRate(1.5f)
        , attentionWeight(0.7f)
        , noveltyWeight(0.3f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = clampLevel(level);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity: optimal level (around 0.3-0.5) enhances learning,
    // very high levels can impair plasticity ("attention narrowing")
    if (pImpl->level < 0.3f) {
        return 0.8f + 0.4f * pImpl->level;  // Increasing from baseline
    } else if (pImpl->level <= 0.6f) {
        return 1.2f;  // Optimal learning zone
    } else {
        return 1.2f - 0.5f * (pImpl->level - 0.6f);  // Decreasing at high levels
    }
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float intensity) {
    if (intensity > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->attentionWeight);
        NLM_LOG_INFO("ACh attention signal: level = " + std::to_string(pImpl->level));
    }
}

void Acetylcholine::signalNovelty(float noveltyLevel) {
    if (noveltyLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyLevel * pImpl->noveltyWeight);
        NLM_LOG_INFO("ACh novelty signal: level = " + std::to_string(pImpl->level));
    }
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalWeight;
    float vigilanceWeight;
    
    Impl()
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.3f)
        , releaseRate(2.0f)
        , arousalWeight(0.6f)
        , vigilanceWeight(0.4f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = clampLevel(level);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE affects arousal and vigilance:
    // - Low NE: Poor attention, low learning efficiency
    // - Medium NE: Optimal attention, good learning
    // - High NE: Stress, may impair complex learning
    return 0.7f + 0.6f * std::tanh(pImpl->level / 0.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay towards baseline with faster decay than ACh
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    if (arousal > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->arousalWeight);
        NLM_LOG_INFO("NE arousal signal: level = " + std::to_string(pImpl->level));
    }
}

void Norepinephrine::signalVigilance(float vigilance) {
    if (vigilance > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + vigilance * pImpl->vigilanceWeight);
        NLM_LOG_INFO("NE vigilance signal: level = " + std::to_string(pImpl->level));
    }
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodWeight;
    float impulsivityWeight;
    
    Impl()
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.15f)
        , releaseRate(1.0f)
        , moodWeight(0.5f)
        , impulsivityWeight(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = clampLevel(level);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood and impulsivity:
    // - Low 5-HT: High impulsivity, poor behavioral regulation
    // - Medium 5-HT: Optimal mood, stable learning
    // - High 5-HT: Reduced impulsivity, potentially reduced exploration
    if (pImpl->level < 0.3f) {
        return 0.9f - 0.4f * pImpl->level;  // Decreasing from baseline
    } else {
        return 0.9f + 0.2f * (1.0f - pImpl->level);  // Decreasing at high levels
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    if (mood > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->moodWeight);
        NLM_LOG_INFO("5-HT mood signal: level = " + std::to_string(pImpl->level));
    }
}

void Serotonin::signalImpulsivity(float impulsivity) {
    if (impulsivity > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + impulsivity * pImpl->impulsivityWeight);
        NLM_LOG_INFO("5-HT impulsivity signal: level = " + std::to_string(pImpl->level));
    }
}

} // namespace nlm
