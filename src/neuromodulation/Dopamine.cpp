#include "Dopamine.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float value;                    // Learned value (TD learning)
    float predictionError;          // Reward prediction error
    float expectedReward;           // Expected reward (moving average)
    float temporalDifference;       // Temporal difference error
    float learningRate;              // Learning rate for value updates
    
    // Reward history for TD learning
    std::vector<std::pair<Timestamp, float>> rewardHistory;
    float maxHistorySize;
    
    // Reward anticipation and motivation
    float expectedValue;             // Expected value of actions
    float motivationLevel;           // Current motivation
    
    Impl() : 
        level(0.0f),
        baseline(0.1f),
        peak(1.0f),
        decayRate(0.1f),
        releaseRate(1.0f),
        value(0.0f),
        predictionError(0.0f),
        expectedReward(0.0f),
        temporalDifference(0.0f),
        learningRate(0.01f),
        maxHistorySize(100),
        expectedValue(0.0f),
        motivationLevel(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized");
}

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
    // Phase 6: Real dopamine-modulated plasticity factor
    // Dopamine modulates synaptic plasticity in a bidirectional way:
    // - Phasic bursts (high DA) enhance LTP and learning
    // - Phasic dips (low DA) suppress plasticity
    // - Tonic DA sets baseline plasticity rate
    
    float burstEffect = 1.0f + std::max(0.0f, pImpl->level - 0.5f) * 2.0f;  // Burst enhances learning
    float dipEffect = 1.0f - std::max(0.0f, 0.5f - pImpl->level) * 2.0f; // Dip suppresses learning
    
    return 0.1f + 0.9f * burstEffect * dipEffect;  // Combine effects
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 6: Realistic dopamine dynamics
    // Dopamine follows phasic and tonic components:
    // - Phasic: Fast bursts on rewards/prediction errors
    // - Tonic: Slower baseline level maintained by VTA neurons
    
    // Tonic component: decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth value learning
    pImpl->value = pImpl->value + pImpl->temporalDifference * pImpl->learningRate;
    
    // Update motivation based on value difference
    pImpl->motivationLevel = std::tanh(pImpl->value * 2.0f);
    
    // Decay reward history
    if (pImpl->rewardHistory.size() > pImpl->maxHistorySize * 0.5f) {
        pImpl->rewardHistory.erase(
            pImpl->rewardHistory.begin(),
            pImpl->rewardHistory.begin() + 
            (pImpl->rewardHistory.size() - pImpl->maxHistorySize / 2)
        );
    }
}

void Dopamine::signalReward(float reward) {
    // Phase 6: Real reward signaling with temporal difference learning
    // Phasic dopamine burst encodes reward prediction error
    
    float predictionError = reward - pImpl->expectedReward;
    pImpl->predictionError = predictionError;
    
    // Compute temporal difference
    pImpl->temporalDifference = predictionError;
    
    // Update expected reward (moving average)
    pImpl->expectedReward = 0.95f * pImpl->expectedReward + 0.05f * reward;
    
    // Phasic burst in response to reward
    float burst = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->level = burst;
    
    // Record for TD learning
    recordRewardHistory(reward, /* current timestamp */ 0.0);
    
    NLM_LOG_DEBUG(std::string("Dopamine burst: reward=") + 
                  std::to_string(reward) + 
                  ", prediction_error=" + std::to_string(predictionError));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 6: Reward prediction error signaling
    // Negative error (unexpected omission) causes dip, positive error (unexpected reward) causes burst
    
    pImpl->predictionError = error;
    
    // Temporal difference from prediction error
    pImpl->temporalDifference = error;
    
    // Update expected reward
    pImpl->expectedReward += error * pImpl->learningRate;
    
    // Phasic response to prediction error
    if (error > 0.0f) {
        // Unexpected reward: burst
        pImpl->level = std::min(pImpl->peak, 
                               pImpl->level + error * pImpl->releaseRate * 2.0f);
    } else if (error < 0.0f) {
        // Omitted reward: dip
        pImpl->level = std::max(pImpl->baseline, 
                               pImpl->level + error * pImpl->releaseRate);
    }
}

void Dopamine::computeValue(float reward, TimestepDuration dt) {
    // Phase 6: Temporal difference value learning
    // Updates estimated value of states/actions based on received reward
    
    float oldValue = pImpl->value;
    
    // Update value using temporal difference
    pImpl->temporalDifference = reward - oldValue;
    pImpl->value = oldValue + pImpl->temporalDifference * pImpl->learningRate;
    
    // Record the value update
    recordRewardHistory(reward, /* current timestamp */ 0.0);
}

float Dopamine::getValue() const {
    return pImpl->value;
}

float Dopamine::getLearningModulationFactor() const {
    // Phase 6: Dopamine-dependent learning modulation
    // Determines how strongly plasticity rules are applied
    
    // Phasic burst enhances learning
    float burstEnhancement = std::max(0.0f, pImpl->level - 0.5f) * 2.0f;
    
    // Tonic level sets baseline learning
    float tonicBaseline = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    
    return 0.1f + tonicBaseline * 0.8f + burstEnhancement * 0.5f;
}

void Dopamine::recordRewardHistory(float reward, Timestamp time) {
    pImpl->rewardHistory.push_back({time, reward});
    
    // Trim if too long
    if (pImpl->rewardHistory.size() > pImpl->maxHistorySize) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

float Dopamine::getAverageReward() const {
    if (pImpl->rewardHistory.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& entry : pImpl->rewardHistory) {
        sum += entry.second;
    }
    
    return sum / pImpl->rewardHistory.size();
}

void Dopamine::setBaselineTone(float tone) {
    pImpl->baseline = std::clamp(tone, 0.0f, 1.0f);
}

float Dopamine::getBaselineTone() const {
    return pImpl->baseline;
}

void Dopamine::signalExpectedReward(float reward) {
    pImpl->expectedValue = reward;
}

float Dopamine::getExpectedReward() const {
    return pImpl->expectedReward;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->value = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->temporalDifference = 0.0f;
    pImpl->expectedValue = 0.0f;
    pImpl->motivationLevel = 0.0f;
    pImpl->rewardHistory.clear();
    
    NLM_LOG_INFO("Dopamine system reset");
}

void Dopamine::integrateWithSTDP(STDP* stdp, float learningRate) {
    if (!stdp) {
        NLM_LOG_ERROR("STDP pointer is null, cannot integrate");
        return;
    }
    
    // Phase 6: Dopamine modulates STDP plasticity
    // High dopamine enhances LTP (potentiation), low dopamine enhances LTD (depression)
    
    float modulation = getLearningModulationFactor();
    
    // Modulate STDP parameters based on dopamine level
    float ltpWeight = learningRate * modulation * (1.0f + pImpl->level);
    float ltdWeight = learningRate * modulation * std::max(0.0f, 1.0f - pImpl->level);
    
    stdp->setLTPWeight(ltpWeight);
    stdp->setLTDWeight(ltdWeight);
    
    NLM_LOG_DEBUG(std::string("Dopamine-STDP integration: LTP=") + 
                  std::to_string(ltpWeight) + 
                  ", LTD=" + std::to_string(ltdWeight));
}

void Dopamine::integrateWithHebbian(Hebbian* hebbian, float learningRate) {
    if (!hebbian) {
        NLM_LOG_ERROR("Hebbian pointer is null, cannot integrate");
        return;
    }
    
    // Phase 6: Dopamine modulates Hebbian plasticity
    // Coincidence detection enhanced by dopamine
    
    float modulation = getLearningModulationFactor();
    float hebbianRate = learningRate * modulation;
    
    // Hebbian learning depends on pre and post activity
    // Dopamine amplifies this when motivation is high
    hebbianRate *= (1.0f + pImpl->motivationLevel);
    
    // Note: Hebbian class typically has fixed learning rate
    // This would require either modifying the class or passing parameters differently
    // For now, we'll log the integration
    
    NLM_LOG_DEBUG(std::string("Dopamine-Hebbian integration: rate=") + 
                  std::to_string(hebbianRate));
}

ActionType Dopamine::selectActionWithReinforcement(
    const std::vector<ActionType>& actions,
    const std::vector<float>& actionValues
) {
    if (actions.empty() || actionValues.empty() || actions.size() != actionValues.size()) {
        return ActionType::Wait;
    }
    
    // Phase 6: Action selection with dopamine-modulated value function
    // Chooses action with highest estimated value, with exploration based on motivation
    
    // Find action with maximum value
    size_t bestAction = 0;
    float maxValue = actionValues[0];
    
    for (size_t i = 1; i < actionValues.size(); ++i) {
        if (actionValues[i] > maxValue) {
            maxValue = actionValues[i];
            bestAction = i;
        }
    }
    
    // Exploration based on motivation and dopamine level
    float explorationRate = 0.1f * pImpl->motivationLevel * pImpl->level;
    
    float r = static_cast<float>(rand()) / RAND_MAX;
    if (r < explorationRate) {
        // Explore: choose random action with value near maximum
        int attempts = 0;
        while (attempts < 10) {
            size_t randomAction = rand() % actions.size();
            if (actionValues[randomAction] > maxValue * 0.8f) {
                return actions[randomAction];
            }
            attempts++;
        }
    }
    
    // Exploit: choose best action
    return actions[bestAction];
}

} // namespace nlm
