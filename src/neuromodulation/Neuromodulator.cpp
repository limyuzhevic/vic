#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <numeric>

namespace nlm {

// Dopamine implementation
struct Dopamine::Impl {
    // Current dopamine level (normalized 0-1)
    float level;
    
    // Baseline and adaptation
    float baseline;
    float targetBaseline;
    float adaptationRate;
    
    // Dynamics parameters
    float decayRate;           // Natural decay rate
    float releaseRate;         // Release rate on reward
    float elevationRate;       // Rate of elevation
    float elevationDuration;   // Duration of elevation (steps)
    
    // Learning and prediction
    float predictedValue;      // Current value prediction
    float valuePredictionError; // Current value prediction error
    float tdLearningRate;      // TD learning rate
    float eligibilityTrace;    // Eligibility trace for reward-modulated learning
    
    // Burst and pause dynamics
    float burstIntensity;      // Current burst intensity
    float burstTimer;          // Timer for burst duration
    float pauseTimer;          // Timer for pause duration
    float lastBurstTime;       // Time of last burst
    float pauseDuration;       // Current pause duration
    
    // Learning rate modulation
    float learningRateMod;     // Current learning rate modulation
    
    // Reward statistics
    float totalReward;         // Total reward accumulated
    float averageReward;       // Average reward (moving average)
    size_t rewardCount;        // Number of reward events
    
    // Reward history for prediction
    std::vector<float> rewardHistory;
    
    // Feature patterns for novelty detection
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    
    // Random generator
    std::unique_ptr<RandomGenerator> rng;
    
    Impl() 
        : level(0.0f)
        , baseline(DopamineConstants::BASELINE_FIRING_RATE)
        , targetBaseline(DopamineConstants::BASELINE_FIRING_RATE)
        , adaptationRate(0.01f)
        , decayRate(1.0f / DopamineConstants::DOPAMINE_DECAY_TIME)
        , releaseRate(DopamineConstants::PEAK_FIRING_RATE)
        , elevationRate(10.0f)
        , elevationDuration(DopamineConstants::ELEVATION_DURATION)
        , predictedValue(0.0f)
        , valuePredictionError(0.0f)
        , tdLearningRate(DopamineConstants::TD_LEARNING_RATE)
        , eligibilityTrace(0.0f)
        , burstIntensity(0.0f)
        , burstTimer(0.0f)
        , pauseTimer(0.0f)
        , lastBurstTime(-1.0f)
        , pauseDuration(0.0f)
        , learningRateMod(1.0f)
        , totalReward(0.0f)
        , averageReward(0.0f)
        , rewardCount(0)
        , rng(nullptr) {
        rng = std::make_unique<RandomGenerator>(42);
        rewardHistory.reserve(RewardConstants::REWARD_HISTORY_SIZE);
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
    // Higher dopamine increases plasticity, but too much can impair learning
    // Using a sigmoidal function to model the inverted-U relationship
    float normalizedLevel = (pImpl->level - 0.5f) * 4.0f;  // Scale to [-2, 2]
    float plasticity = 1.0f / (1.0f + std::exp(-normalizedLevel)) * 2.0f - 0.5f;
    return std::clamp(plasticity, pImpl->learningRateMod * DopamineConstants::MIN_LEARNING_RATE_MOD,
                     pImpl->learningRateMod * DopamineConstants::MAX_LEARNING_RATE_MOD);
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Update burst/pause timers
    if (pImpl->burstTimer > 0.0f) {
        pImpl->burstTimer -= deltaTime;
        if (pImpl->burstTimer <= 0.0f) {
            pImpl->burstTimer = 0.0f;
        }
    } else if (pImpl->pauseTimer > 0.0f) {
        pImpl->pauseTimer -= deltaTime;
        if (pImpl->pauseTimer <= 0.0f) {
            pImpl->pauseTimer = 0.0f;
        }
    }
    
    // Apply natural decay
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * deltaTime);
    
    // Update eligibility trace for reward-modulated learning
    updateEligibilityTrace(1.0f - pImpl->decayRate * deltaTime);
    
    // Adapt baseline slowly towards target
    if (std::abs(pImpl->baseline - pImpl->targetBaseline) > 0.01f) {
        pImpl->baseline += (pImpl->targetBaseline - pImpl->baseline) * pImpl->adaptationRate * deltaTime;
    }
    
    // Smooth adaptation variable
    float adaptation = pImpl->level - pImpl->baseline;
    float adaptationChange = (pImpl->valuePredictionError - adaptation) * pImpl->adaptationRate * deltaTime;
    pImpl->level += adaptationChange;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->targetBaseline = DopamineConstants::BASELINE_FIRING_RATE;
    pImpl->predictedValue = 0.0f;
    pImpl->valuePredictionError = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->burstIntensity = 0.0f;
    pImpl->burstTimer = 0.0f;
    pImpl->pauseTimer = 0.0f;
    pImpl->learningRateMod = 1.0f;
    pImpl->rewardCount = 0;
    pImpl->rewardHistory.clear();
    pImpl->currentPattern.clear();
    pImpl->previousPattern.clear();
}

bool Dopamine::isActive() const {
    // Consider dopamine active if it deviates from baseline by more than 10%
    return std::abs(pImpl->level - pImpl->baseline) > pImpl->baseline * 0.1f;
}

void Dopamine::signalReward(float reward) {
    // Clamp reward to valid range
    reward = std::clamp(reward, RewardConstants::REWARD_MIN, RewardConstants::REWARD_MAX);
    
    // Update reward statistics
    pImpl->totalReward += reward;
    pImpl->rewardCount++;
    pImpl->averageReward = pImpl->totalReward / static_cast<float>(pImpl->rewardCount);
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > RewardConstants::REWARD_HISTORY_SIZE) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Signal burst based on reward magnitude
    float burstMagnitude = std::min(1.0f, std::abs(reward) / RewardConstants::REWARD_MAX);
    signalBurst(burstMagnitude, DopamineConstants::ELEVATION_DURATION);
    
    // Compute and store prediction error
    signalRewardPredictionError(reward - pImpl->predictedValue);
    
    // Update eligibility trace with reward
    updateEligibilityTrace(); // Full trace reset on reward
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->valuePredictionError = error;
    
    // Burst or pause based on error sign
    if (error > DopamineConstants::RPE_THRESHOLD_LARGE) {
        // Large positive error - unexpected reward
        signalBurst(std::min(1.0f, error / DopamineConstants::RPE_THRESHOLD_LARGE), 
                   DopamineConstants::ELEVATION_DURATION * 2.0f);
    } else if (error > DopamineConstants::RPE_THRESHOLD_SMALL) {
        // Small positive error - predicted reward
        signalBurst(0.5f, DopamineConstants::ELEVATION_DURATION);
    } else if (error < DopamineConstants::RPE_THRESHOLD_NEG) {
        // Negative error - unexpected omission
        signalPause(-error * DopamineConstants::ELEVATION_DURATION);
    }
    
    // Apply dopamine change
    pImpl->level += error * pImpl->releaseRate * DopamineConstants::TD_LEARNING_RATE;
    
    // Update prediction for next step
    updatePrediction(pImpl->predictedValue + error * DopamineConstants::TD_LEARNING_RATE);
}

void Dopamine::signalPunishment(float magnitude) {
    // Signal punishment as negative reward
    signalReward(-magnitude);
}

void Dopamine::updatePrediction(float value) {
    pImpl->predictedValue = value;
}

float Dopamine::computeTD(float reward, float currentValue, float nextValue) {
    // Temporal difference: TD = reward + gamma * nextValue - currentValue
    float td = reward + RewardConstants::TD_GAMMA * nextValue - currentValue;
    return td * pImpl->tdLearningRate;
}

void Dopamine::updateEligibilityTrace(float traceDecay) {
    pImpl->eligibilityTrace *= traceDecay;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::addEligibilityContribution(float weight, float preSpikeTime, float postSpikeTime) {
    // Spike-timing dependent eligibility contribution
    float dt = postSpikeTime - preSpikeTime;
    float eligibility = std::exp(-std::abs(dt) / DopamineConstants::ADAPTATION_TIME);
    pImpl->eligibilityTrace += weight * eligibility;
}

void Dopamine::signalBurst(float magnitude, float duration) {
    pImpl->burstIntensity = magnitude;
    pImpl->burstTimer = duration;
    pImpl->lastBurstTime = pImpl->burstTimer;
    
    // Elevate dopamine level for burst
    pImpl->level = std::min(pImpl->baseline + magnitude * DopamineConstants::PEAK_FIRING_RATE, 
                          DopamineConstants::PEAK_FIRING_RATE);
}

void Dopamine::signalPause(float duration) {
    pImpl->pauseTimer = duration;
    pImpl->pauseDuration = duration;
    
    // Pause dopamine level
    pImpl->level = pImpl->baseline * 0.1f;  // Strong suppression
}

bool Dopamine::isBursting() const {
    return pImpl->burstTimer > 0.0f && pImpl->burstIntensity > 0.0f;
}

float Dopamine::getBurstIntensity() const {
    return pImpl->burstIntensity;
}

float Dopamine::getLearningRateModulation() const {
    return pImpl->learningRateMod;
}

void Dopamine::setLearningRateModulation(float modulation) {
    pImpl->learningRateMod = std::clamp(modulation, 0.0f, DopamineConstants::MAX_LEARNING_RATE_MOD);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->targetBaseline = std::clamp(baseline, 0.0f, 1.0f);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 1.0f);
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::clamp(rate, 0.0f, 10.0f);
}

void Dopamine::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.0f, 0.1f);
}

void Dopamine::setTDLearningRate(float rate) {
    pImpl->tdLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Dopamine::getPredictedValue() const {
    return pImpl->predictedValue;
}

float Dopamine::getValuePredictionError() const {
    return pImpl->valuePredictionError;
}

float Dopamine::getAverageReward() const {
    return pImpl->averageReward;
}

float Dopamine::getTotalReward() const {
    return pImpl->totalReward;
}

size_t Dopamine::getRewardCount() const {
    return pImpl->rewardCount;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float attentionGain;
    float attentionBaseline;
    float attentionBoostRate;
    float attentionDecayRate;
    float memoryConsolidationRate;
    float saturationLevel;
    
    Impl()
        : level(0.0f)
        , attentionGain(1.0f)
        , attentionBaseline(0.5f)
        , attentionBoostRate(5.0f)
        , attentionDecayRate(0.1f)
        , memoryConsolidationRate(0.05f)
        , saturationLevel(2.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity for attention and memory
    return 1.0f + pImpl->attentionGain * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay attention signal
    pImpl->attentionGain = std::max(pImpl->attentionBaseline, 
                                   pImpl->attentionGain - pImpl->attentionDecayRate * static_cast<float>(dt));
    
    // Level based on attention gain
    pImpl->level = std::min(pImpl->attentionGain / pImpl->saturationLevel, 1.0f);
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->attentionGain = pImpl->attentionBaseline;
}

bool Acetylcholine::isActive() const {
    return pImpl->level > 0.1f;
}

void Acetylcholine::enhanceAttention(float magnitude) {
    pImpl->attentionGain = std::min(pImpl->attentionGain + magnitude * pImpl->attentionBoostRate, 
                                   pImpl->saturationLevel);
}

void Acetylcholine::sustainAttention(float duration) {
    // Sustain attention over time
    pImpl->attentionGain = std::max(pImpl->attentionGain, 
                                   pImpl->attentionBaseline + 0.5f * duration);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float baselineArousal;
    float alertnessGain;
    float focusGain;
    float arousalDecayRate;
    float stressLevel;
    
    Impl()
        : level(0.0f)
        , arousalLevel(0.0f)
        , baselineArousal(0.5f)
        , alertnessGain(1.0f)
        , focusGain(1.0f)
        , arousalDecayRate(0.05f)
        , stressLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for arousal and vigilance
    return 1.0f + pImpl->arousalLevel * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay arousal
    pImpl->arousalLevel = std::max(baselineArousal,
                                  pImpl->arousalLevel - pImpl->arousalDecayRate * static_cast<float>(dt));
    
    // Level based on arousal
    pImpl->level = std::min(pImpl->arousalLevel, 1.0f);
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->arousalLevel = pImpl->baselineArousal;
    pImpl->stressLevel = 0.0f;
}

bool Norepinephrine::isActive() const {
    return pImpl->level > 0.1f;
}

void Norepinephrine::signalAlertness(float magnitude) {
    pImpl->arousalLevel = std::min(pImpl->arousalLevel + magnitude, 2.0f);
}

void Norepinephrine::signalFocus(float magnitude) {
    pImpl->focusGain = std::min(pImpl->focusGain + magnitude, 2.0f);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float moodLevel;
    float baselineMood;
    float satietyLevel;
    float impulsivityLevel;
    float patienceLevel;
    float serotoninDecayRate;
    float stressSensitivity;
    
    Impl()
        : level(0.0f)
        , moodLevel(0.5f)
        , baselineMood(0.5f)
        , satietyLevel(0.5f)
        , impulsivityLevel(1.0f)
        , patienceLevel(1.0f)
        , serotoninDecayRate(0.03f)
        , stressSensitivity(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for mood and social behavior
    return 1.0f - std::abs(pImpl->moodLevel - 0.5f) * 0.5f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay mood
    pImpl->moodLevel = std::max(baselineMood,
                               pImpl->moodLevel - serotoninDecayRate * static_cast<float>(dt));
    
    // Level based on mood
    pImpl->level = std::min(pImpl->moodLevel, 1.0f);
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->moodLevel = pImpl->baselineMood;
    pImpl->satietyLevel = 0.5f;
    pImpl->impulsivityLevel = 1.0f;
    pImpl->patienceLevel = 1.0f;
}

bool Serotonin::isActive() const {
    return pImpl->level > 0.1f;
}

void Serotonin::signalMood(float mood) {
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
}

void Serotonin::signalSatiety(float satiety) {
    pImpl->satietyLevel = std::clamp(satiety, 0.0f, 1.0f);
    
    // High satiety reduces impulsivity
    pImpl->impulsivityLevel = std::max(0.1f, 1.0f - satiety * 0.8f);
}

void Serotonin::reduceImpulsivity(float magnitude) {
    pImpl->patienceLevel = std::min(pImpl->patienceLevel + magnitude, 2.0f);
    pImpl->impulsivityLevel = std::max(0.1f, pImpl->impulsivityLevel - magnitude);
}

void Serotonin::increasePatience(float magnitude) {
    pImpl->patienceLevel = std::min(pImpl->patienceLevel + magnitude, 2.0f);
}

// NeuromodulatorSystem implementation
struct NeuromodulatorSystem::Impl {
    class Brain* brain;
    
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
    std::vector<float> rewardHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> noveltyHistory;
    
    float totalReward;
    float averageReward;
    
    Impl()
        : brain(nullptr)
        , totalReward(0.0f)
        , averageReward(0.0f) {
        rewardHistory.reserve(RewardConstants::REWARD_HISTORY_SIZE);
        predictionErrorHistory.reserve(RewardConstants::REWARD_HISTORY_SIZE);
        noveltyHistory.reserve(NoveltyConstants::PATTERN_HISTORY_SIZE);
    }
};

NeuromodulatorSystem::NeuromodulatorSystem() : pImpl(new Impl) {}

NeuromodulatorSystem::~NeuromodulatorSystem() = default;

void NeuromodulatorSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->acetylcholine = std::make_unique<Acetylcholine>();
    pImpl->norepinephrine = std::make_unique<Norepinephrine>();
    pImpl->serotonin = std::make_unique<Serotonin>();
    
    NLM_LOG_INFO("Neuromodulator system initialized");
}

void NeuromodulatorSystem::update(TimestepDuration dt) {
    // Update all neuromodulators
    pImpl->dopamine->update(dt);
    pImpl->acetylcholine->update(dt);
    pImpl->norepinephrine->update(dt);
    pImpl->serotonin->update(dt);
    
    // Smooth average reward
    if (!pImpl->rewardHistory.empty()) {
        pImpl->totalReward = std::accumulate(pImpl->rewardHistory.begin(), pImpl->rewardHistory.end(), 0.0f);
        pImpl->averageReward = pImpl->totalReward / static_cast<float>(pImpl->rewardHistory.size());
    }
}

void NeuromodulatorSystem::applyToNeuron(Neuron* neuron) {
    if (!neuron) return;
    
    // Apply neuromodulatory effects to neuron excitability
    float neuromodulation = 1.0f;
    
    // Dopamine increases excitability
    neuromodulation *= (1.0f + pImpl->dopamine->getLevel() * 0.5f);
    
    // Acetylcholine enhances attention-related excitability
    neuromodulation *= (1.0f + pImpl->acetylcholine->getLevel() * 0.3f);
    
    // Norepinephrine increases arousal-related excitability
    neuromodulation *= (1.0f + pImpl->norepinephrine->getLevel() * 0.2f);
    
    // Serotonin has complex effects on excitability
    float serotoninLevel = pImpl->serotonin->getLevel();
    neuromodulation *= (1.0f + (serotoninLevel - 0.5f) * 0.1f); // Optimal at 0.5
    
    // Apply neuromodulation to neuron
    if (neuromodulation != 1.0f) {
        neuron->injectCurrent((neuromodulation - 1.0f) * 0.1f);
    }
}

void NeuromodulatorSystem::recordReward(float reward, const Observation* observation) {
    // Clamp reward
    reward = std::clamp(reward, RewardConstants::REWARD_MIN, RewardConstants::REWARD_MAX);
    
    // Update reward history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > RewardConstants::REWARD_HISTORY_SIZE) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Record in dopamine system
    if (pImpl->dopamine) {
        pImpl->dopamine->signalReward(reward);
    }
    
    // Update prediction error if we have observation
    if (observation && pImpl->dopamine) {
        float predicted = pImpl->dopamine->getPredictedValue();
        float error = reward - predicted;
        pImpl->dopamine->signalRewardPredictionError(error);
        
        pImpl->predictionErrorHistory.push_back(error);
        if (pImpl->predictionErrorHistory.size() > RewardConstants::REWARD_HISTORY_SIZE) {
            pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
        }
    }
}

void NeuromodulatorSystem::recordPredictionError(float error, const Observation* observation) {
    pImpl->predictionErrorHistory.push_back(error);
    if (pImpl->predictionErrorHistory.size() > RewardConstants::REWARD_HISTORY_SIZE) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Update dopamine with prediction error
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(error);
    }
}

void NeuromodulatorSystem::recordNovelty(float novelty) {
    pImpl->noveltyHistory.push_back(novelty);
    if (pImpl->noveltyHistory.size() > NoveltyConstants::PATTERN_HISTORY_SIZE) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
    }
}

void NeuromodulatorSystem::updateCuriosity(float predictionError, float novelty, TimestepDuration dt) {
    // Update curiosity based on prediction error and novelty
    // This would update the curiosity neuromodulator in the brain
    if (pImpl->brain) {
        auto* curiosity = pImpl->brain->getCuriosity();
        if (curiosity) {
            // Combine prediction error and novelty with weighting
            float curiositySignal = (std::abs(predictionError) * 0.7f) + (novelty * 0.3f);
            // Update curiosity level (this would need to be exposed)
        }
    }
}

float NeuromodulatorSystem::selectExplorationPolicy(float value, 
                                                    const std::vector<float>& actionValues,
                                                    const std::string& policy) {
    if (actionValues.empty()) return value;
    
    if (policy == "epsilon_greedy") {
        // Simple epsilon-greedy: with probability epsilon, explore randomly
        float epsilon = 0.1f; // Could be made dynamic based on curiosity
        if (pImpl->dopamine) {
            epsilon = std::max(0.01f, 0.1f * (1.0f - pImpl->dopamine->getLevel()));
        }
        
        if (pImpl->rng->uniformReal() < epsilon) {
            return actionValues[pImpl->rng->uniformInt(0, actionValues.size() - 1)];
        } else {
            // Choose best action
            return *std::max_element(actionValues.begin(), actionValues.end());
        }
    } else if (policy == "ucb") {
        // UCB (Upper Confidence Bound)
        float ucbValue = value + std::sqrt(2.0f * std::log(pImpl->predictionErrorHistory.size() + 1) / 
                                          (actionValues.size() + 1));
        return ucbValue;
    } else if (policy == "thompson_sampling") {
        // Thompson sampling - sample from beta distribution
        float alpha = 1.0f + pImpl->predictionErrorHistory.size();
        float beta = 1.0f + pImpl->predictionErrorHistory.size() * 2.0f;
        float sample = pImpl->rng->beta(alpha, beta);
        return sample;
    }
    
    // Default: epsilon-greedy
    return selectExplorationPolicy(value, actionValues, "epsilon_greedy");
}

void NeuromodulatorSystem::reset() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->acetylcholine) pImpl->acetylcholine->reset();
    if (pImpl->norepinephrine) pImpl->norepinephrine->reset();
    if (pImpl->serotonin) pImpl->serotonin->reset();
    
    pImpl->rewardHistory.clear();
    pImpl->predictionErrorHistory.clear();
    pImpl->noveltyHistory.clear();
    pImpl->totalReward = 0.0f;
    pImpl->averageReward = 0.0f;
}

} // namespace nlm