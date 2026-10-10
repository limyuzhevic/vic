#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Neuromodulator::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

Neuromodulator::~Neuromodulator() = default;

float Neuromodulator::getLevel() const {
    return pImpl->level;
}

void Neuromodulator::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Neuromodulator::update(TimestepDuration dt) {
    // Default implementation - decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Dopamine implementation

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeConstant;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), timeConstant(5.0f) {}
    
    std::vector<float> history;
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

const char* Dopamine::getEffectType() const {
    return "Reward";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    if (reward > 0.0f) {
        // Calcium spike - burst of dopamine
        float burst = std::min(reward * pImpl->releaseRate * 2.0f, pImpl->peak - pImpl->level);
        pImpl->level += burst;
        pImpl->history.push_back(pImpl->level);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    if (error != 0.0f) {
        float change = error * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(0.0f, pImpl->level + change);
        pImpl->history.push_back(pImpl->level);
    }
}

float Dopamine::getPlasticityFactor() const {
    // Inverted U-shaped relationship: moderate dopamine optimizes learning
    float normalized = pImpl->level / pImpl->peak;
    return 0.5f + 0.8f * std::exp(-4.0f * std::pow(normalized - 0.5f, 2.0f));
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slow baseline adaptation
    if (pImpl->baseline < 0.3f && pImpl->level > 0.3f) {
        pImpl->baseline = std::min(0.3f, pImpl->baseline + 0.01f * static_cast<float>(dt));
    }
}

void Dopamine::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

// Acetylcholine implementation

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionalGain;
    float memoryBoost;
    float arousalLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f),
             attentionalGain(1.0f), memoryBoost(1.0f), arousalLevel(0.5f) {}
    
    std::vector<float> history;
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

const char* Acetylcholine::getEffectType() const {
    return "Attention";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    updateAttentionalGain();
}

void Acetylcholine::signalAttention(float salience) {
    float boost = salience * 0.8f;
    pImpl->level += boost;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    pImpl->level *= (1.0f + strength * 0.3f);
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    pImpl->memoryBoost = std::min(pImpl->memoryBoost, 1.0f + strength * 0.5f);
}

float Acetylcholine::getAttentionalGain() const {
    return pImpl->attentionalGain;
}

float Acetylcholine::getMemoryBoost() const {
    return pImpl->memoryBoost;
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateAttentionalGain();
    
    if (pImpl->memoryBoost > 1.0f) {
        pImpl->memoryBoost = std::max(1.0f, pImpl->memoryBoost - 0.01f * static_cast<float>(dt));
    }
}

void Acetylcholine::updateAttentionalGain() {
    float normalized = pImpl->level / pImpl->peak;
    pImpl->attentionalGain = 1.0f + 1.5f * std::exp(-4.0f * std::pow(normalized - 0.6f, 2.0f));
}

void Acetylcholine::signalArousal(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
    if (pImpl->arousalLevel > 0.7f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
    }
}

float Acetylcholine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Acetylcholine::getHistory() const {
    return pImpl->history;
}

void Acetylcholine::clearHistory() {
    pImpl->history.clear();
}

// Norepinephrine implementation

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float stressLevel;
    float focusGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f),
             arousalLevel(0.5f), stressLevel(0.0f), focusGain(1.0f) {}
    
    std::vector<float> history;
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

const char* Norepinephrine::getEffectType() const {
    return "Arousal";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    updateArousalState();
}

void Norepinephrine::signalStress(float stress) {
    pImpl->stressLevel = std::clamp(stress, 0.0f, 1.0f);
    
    float response = pImpl->stressLevel * 0.6f;
    pImpl->level += response;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
}

void Norepinephrine::signalFocus(float focus) {
    float focusBoost = focus * 0.4f;
    pImpl->level += focusBoost;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    pImpl->focusGain = std::min(pImpl->focusGain, 1.0f + focus * 0.5f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getFocusGain() const {
    return pImpl->focusGain;
}

float Norepinephrine::getPlasticityFactor() const {
    if (pImpl->stressLevel > 0.5f) {
        return 0.5f * (1.0f + pImpl->level * 0.5f);
    } else {
        float normalized = pImpl->level / pImpl->peak;
        return 0.8f + 0.5f * normalized;
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateArousalState();
    
    if (pImpl->stressLevel > 0.0f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.005f * static_cast<float>(dt));
    }
    
    if (pImpl->focusGain > 1.0f) {
        pImpl->focusGain = std::max(1.0f, pImpl->focusGain - 0.01f * static_cast<float>(dt));
    }
}

void Norepinephrine::updateArousalState() {
    float normalized = pImpl->level / pImpl->peak;
    pImpl->arousalLevel = 0.3f + 0.7f * normalized;
}

void Norepinephrine::habituate(float amount) {
    if (amount > 0.0f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - amount * 0.5f);
    }
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Norepinephrine::getHistory() const {
    return pImpl->history;
}

void Norepinephrine::clearHistory() {
    pImpl->history.clear();
}

// Serotonin implementation

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulsivity;
    float socialBonding;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f),
             moodLevel(0.5f), impulsivity(0.5f), socialBonding(0.5f) {}
    
    std::vector<float> history;
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

const char* Serotonin::getEffectType() const {
    return "Mood";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    updateMoodState();
}

void Serotonin::signalPositiveMood(float mood) {
    if (mood > 0.0f) {
        float boost = mood * 0.5f;
        pImpl->level += boost;
        
        if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
        pImpl->history.push_back(pImpl->level);
    }
}

void Serotonin::signalSocialBonding(float bonding) {
    float bondingBoost = bonding * 0.4f;
    pImpl->level += bondingBoost;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    pImpl->socialBonding = std::min(pImpl->socialBonding, 1.0f + bonding * 0.3f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityFactor() const {
    return 1.0f - 0.8f * (pImpl->level / pImpl->peak);
}

float Serotonin::getSocialBondingFactor() const {
    return pImpl->socialBonding;
}

float Serotonin::getPlasticityFactor() const {
    float normalized = pImpl->level / pImpl->peak;
    
    if (normalized < 0.3f) {
        return 0.6f + 0.3f * normalized;
    } else {
        return 0.9f + 0.1f * std::exp(-2.0f * (normalized - 0.7f));
    }
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateMoodState();
    
    if (pImpl->socialBonding > 1.0f) {
        pImpl->socialBonding = std::max(1.0f, pImpl->socialBonding - 0.02f * static_cast<float>(dt));
    }
}

void Serotonin::updateMoodState() {
    float normalized = pImpl->level / pImpl->peak;
    
    if (normalized < 0.2f) {
        pImpl->moodLevel = 0.2f + 0.6f * normalized;
    } else if (normalized < 0.8f) {
        pImpl->moodLevel = 0.5f + 0.5f * normalized;
    } else {
        pImpl->moodLevel = 0.9f + 0.1f * (normalized - 0.8f);
    }
}

void Serotonin::signalNegativeExperience(float severity) {
    float reduction = severity * 0.3f;
    pImpl->level = std::max(0.0f, pImpl->level - reduction);
    pImpl->history.push_back(pImpl->level);
}

float Serotonin::getResiliencyFactor() const {
    return 0.5f + 0.5f * pImpl->baseline;
}

void Serotonin::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Serotonin::getHistory() const {
    return pImpl->history;
}

void Serotonin::clearHistory() {
    pImpl->history.clear();
}

// Novelty implementation

struct Novelty::Impl {
    float level;
    float peak;
    float decayRate;
    size_t memoryWindow;
    float detectionThreshold;
    float surpriseMagnitude;
    
    Impl() : level(0.0f), peak(1.0f), decayRate(0.05f), memoryWindow(100),
             detectionThreshold(0.3f), surpriseMagnitude(0.5f) {}
    
    std::vector<float> history;
    std::vector<float> lastPattern;
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->history.clear();
    pImpl->lastPattern.clear();
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.size() != previousPattern.size() || currentPattern.empty()) {
        return;
    }
    
    float distance = 0.0f;
    for (size_t i = 0; i < currentPattern.size(); ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        distance += diff * diff;
    }
    distance = std::sqrt(distance);
    
    float novelty = std::min(distance / pImpl->detectionThreshold, 1.0f) * pImpl->surpriseMagnitude;
    
    level_ = novelty;
    pImpl->history.push_back(level_);
}

void Novelty::update(float dt) {
    level_ = std::max(0.0f, level_ - pImpl->decayRate * dt);
}

float Novelty::getLevel() const {
    return level_;
}

void Novelty::setLevel(float level) {
    level_ = std::clamp(level, 0.0f, pImpl->peak);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

// PredictionError implementation

struct PredictionError::Impl {
    float error;
    float confidence;
    float learningRate;
    
    Impl() : error(0.0f), confidence(0.5f), learningRate(0.1f) {}
    
    std::vector<float> errorHistory;
    std::vector<float> predictionHistory;
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
    pImpl->errorHistory.clear();
}

void PredictionError::computeError(float predicted, float actual) {
    float newError = actual - predicted;
    
    float learningFactor = pImpl->learningRate * pImpl->confidence;
    float adaptedError = pImpl->error + learningFactor * (newError - pImpl->error);
    
    pImpl->error = adaptedError;
    pImpl->errorHistory.push_back(std::abs(pImpl->error));
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictionHistory.push_back(newPrediction);
    
    if (!pImpl->predictionHistory.empty()) {
        float avgPrediction = 0.0f;
        for (float pred : pImpl->predictionHistory) {
            avgPrediction += pred;
        }
        avgPrediction /= pImpl->predictionHistory.size();
        
        float predictionChange = pImpl->learningRate * pImpl->error * (1.0f - pImpl->confidence);
        float updatedPrediction = avgPrediction + predictionChange;
        
        pImpl->error = (newPrediction - updatedPrediction) * 0.5f;
    }
}

float PredictionError::getError() const {
    return pImpl->error;
}

float PredictionError::getConfidence() const {
    return pImpl->confidence;
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

void PredictionError::update(float dt) {
    pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.05f * dt);
    
    if (std::abs(pImpl->error) > 0.1f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.1f * dt);
    }
}

const std::vector<float>& PredictionError::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionError::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionHistory.clear();
}

// Curiosity implementation

struct Curiosity::Impl {
    float level;
    float peak;
    float decayRate;
    float noveltyWeight;
    float errorWeight;
    float explorationDrive;
    
    Impl() : level(0.0f), peak(1.0f), decayRate(0.03f), noveltyWeight(0.6f), errorWeight(0.4f), explorationDrive(0.5f) {}
    
    std::vector<float> history;
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->history.clear();
}

void Curiosity::update(float novelty, float predictionError, float dt) {
    float target = pImpl->noveltyWeight * novelty + pImpl->errorWeight * predictionError;
    
    float learningRate = 0.1f;
    float delta = target - pImpl->level;
    pImpl->level += learningRate * delta * dt;
    
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dt);
    
    pImpl->explorationDrive = pImpl->level;
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->errorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationDrive = 0.0f;
}

const std::vector<float>& Curiosity::getHistory() const {
    return pImpl->history;
}

void Curiosity::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
