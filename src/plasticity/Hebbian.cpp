#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

enum class HebbianRuleType {
    BASIC,
    COVARIANCE,
    OJA,
    COMPETITIVE,
    METAPLASTIC,
    HARD_LIMIT
};

// Forward declarations for advanced rule classes
class AdvancedCovarianceHebbian;
class OjaHebbian;
class CompetitiveHebbian;
class MetaplasticHebbian;
class HardLimitHebbian;

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Configuration for different rule variants
    bool useOjaNormalization;
    bool useCompetitiveLearning;
    bool useMetaplasticity;
    bool useHardLimit;
    float competitionStrength;
    float metaplasticDecay;
    float historyWindow;  // For activity history
    
    // Activity history for advanced rules
    std::unordered_map<size_t, float> preSpikeHistory;
    std::unordered_map<size_t, float> postSpikeHistory;
    float currentActivity;
    float baselineActivity;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), useOjaNormalization(false),
             useCompetitiveLearning(false), useMetaplasticity(false),
             useHardLimit(false), competitionStrength(0.1f),
             metaplasticDecay(0.001f), historyWindow(100.0f),
             currentActivity(0.0f), baselineActivity(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Apply different Hebbian variants based on configuration
    if (pImpl->useOjaNormalization) {
        totalDelta = applyOjaHebbian(synapse, preSpikes, postSpikes, dt);
    } else if (pImpl->useCompetitiveLearning) {
        totalDelta = applyCompetitiveHebbian(synapse, preSpikes, postSpikes, dt);
    } else if (pImpl->useMetaplasticity) {
        totalDelta = applyMetaplasticHebbian(synapse, preSpikes, postSpikes, dt);
    } else if (pImpl->useHardLimit) {
        totalDelta = applyHardLimitHebbian(synapse, preSpikes, postSpikes, dt);
    } else {
        totalDelta = applyAdvancedCovarianceRule(synapse, preSpikes, postSpikes, dt);
    }
    
    // Apply with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

float Hebbian::applyAdvancedCovarianceRule(Synapse* synapse,
                                           const std::vector<Timestamp>& preSpikes,
                                           const std::vector<Timestamp>& postSpikes,
                                           TimestepDuration dt) {
    // Advanced Covariance Rule: Δw = η × (⟨pre·post⟩ - ⟨pre⟩×⟨post⟩)
    
    // Update activity history
    size_t currentTime = static_cast<size_t>(dt.count());
    pImpl->currentActivity = static_cast<float>(preSpikes.size() + postSpikes.size());
    
    // Clean old history entries
    for (auto it = pImpl->preSpikeHistory.begin(); it != pImpl->preSpikeHistory.end();) {
        if (currentTime - it->first > pImpl->historyWindow) {
            it = pImpl->preSpikeHistory.erase(it);
        } else {
            ++it;
        }
    }
    
    for (auto it = pImpl->postSpikeHistory.begin(); it != pImpl->postSpikeHistory.end();) {
        if (currentTime - it->first > pImpl->historyWindow) {
            it = pImpl->postSpikeHistory.erase(it);
        } else {
            ++it;
        }
    }
    
    // Calculate mean activities
    float meanPre = 0.0f, meanPost = 0.0f;
    float meanPrePost = 0.0f;
    
    if (!pImpl->preSpikeHistory.empty()) {
        float sumPre = 0.0f;
        for (const auto& entry : pImpl->preSpikeHistory) {
            sumPre += entry.second;
        }
        meanPre = sumPre / pImpl->preSpikeHistory.size();
    }
    
    if (!pImpl->postSpikeHistory.empty()) {
        float sumPost = 0.0f;
        for (const auto& entry : pImpl->postSpikeHistory) {
            sumPost += entry.second;
        }
        meanPost = sumPost / pImpl->postSpikeHistory.size();
    }
    
    // Calculate covariance term ⟨pre·post⟩
    float covarianceTerm = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(std::abs(postTime - preTime));
            if (dt < pImpl->historyWindow) {
                covarianceTerm += 1.0f; // Simplified spike contribution
            }
        }
    }
    
    // Normalize covariance
    size_t maxPairs = std::max(preSpikes.size(), postSpikes.size()) * std::max(preSpikes.size(), postSpikes.size());
    float normalizedCovariance = (maxPairs > 0) ? covarianceTerm / maxPairs : 0.0f;
    
    // Apply covariance rule
    float baseline = meanPre * meanPost;
    float delta = pImpl->learningRate * (normalizedCovariance - baseline);
    
    return delta;
}

float Hebbian::applyOjaHebbian(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    // Oja's Rule: Δw = η × (y × (x - w × y))
    // Where y = preSpike × postSpike (postsynaptic activity)
    //       x = preSpike (presynaptic input)
    //       w = current weight
    
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Postsynaptic activity (coincidence detection)
    float y = preActivity * postActivity;
    
    // Presynaptic input
    float x = preActivity;
    
    // Current weight
    float w = synapse->getWeight();
    
    // Oja's learning rule
    float delta = pImpl->learningRate * y * (x - w * y);
    
    // Stability constraint (automatically provided by Oja's rule)
    return delta;
}

float Hebbian::applyCompetitiveHebbian(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    // Competitive Hebbian Learning: Winner-take-all dynamics
    
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Calculate competition signal
    float totalPostActivity = postActivity;
    float competitionSignal = totalPostActivity * pImpl->competitionStrength;
    
    // Apply Hebbian learning with competition
    float delta = pImpl->learningRate * preActivity * postActivity;
    delta -= competitionSignal; // Competitive term
    
    // Apply hard limit if configured
    if (pImpl->useHardLimit) {
        delta = (delta > 0.0f) ? std::min(delta, 1.0f) : std::max(delta, -1.0f);
    }
    
    return delta;
}

float Hebbian::applyMetaplasticHebbian(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    // Metaplasticity: Learning rate adapts with network activity
    
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Update baseline activity with decay
    pImpl->baselineActivity = (1.0f - pImpl->metaplasticDecay) * pImpl->baselineActivity + 
                             0.1f * (preActivity + postActivity);
    
    // Adaptive learning rate
    float adaptiveLearningRate = pImpl->learningRate * 
                                 (1.0f + pImpl->baselineActivity * 0.5f);
    
    // Apply Hebbian learning with adaptive rate
    float delta = adaptiveLearningRate * preActivity * postActivity;
    
    return delta;
}

float Hebbian::applyHardLimitHebbian(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     TimestepDuration dt) {
    // Hard Limit Hebbian: Binary learning rule
    
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Binary decision
    bool shouldPotentiate = (preActivity > 0.0f && postActivity > 0.0f);
    
    // Fixed learning increment
    float delta = shouldPotentiate ? pImpl->learningRate : -pImpl->learningRate;
    
    return delta;
}

void Hebbian::updateAdvanced(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt,
                             HebbianRuleType ruleType,
                             float learningRate,
                             bool useNormalization,
                             float competitionStrength,
                             float metaplasticDecay) {
    // Configuration for advanced rules
    pImpl->useOjaNormalization = (ruleType == HebbianRuleType::OJA || useNormalization);
    pImpl->useCompetitiveLearning = (ruleType == HebbianRuleType::COMPETITIVE || competitionStrength > 0.0f);
    pImpl->useMetaplasticity = (ruleType == HebbianRuleType::METAPLASTIC || metaplasticDecay > 0.0f);
    pImpl->useHardLimit = (ruleType == HebbianRuleType::HARD_LIMIT);
    pImpl->competitionStrength = competitionStrength;
    pImpl->metaplasticDecay = metaplasticDecay;
    
    // Update learning rate
    setLearningRate(learningRate);
    
    // Apply update
    update(synapse, preSpikes, postSpikes, dt);
}

const char* Hebbian::getRuleName(HebbianRuleType ruleType) const {
    switch (ruleType) {
        case HebbianRuleType::BASIC: return "Basic Hebbian";
        case HebbianRuleType::COVARIANCE: return "Advanced Covariance";
        case HebbianRuleType::OJA: return "Oja's Rule";
        case HebbianRuleType::COMPETITIVE: return "Competitive Hebbian";
        case HebbianRuleType::METAPLASTIC: return "Metaplastic Hebbian";
        case HebbianRuleType::HARD_LIMIT: return "Hard Limit Hebbian";
        default: return "Unknown";
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "AdvancedHebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setLearningRule(HebbianRuleType ruleType, float learningRate) {
    pImpl->useOjaNormalization = (ruleType == HebbianRuleType::OJA);
    pImpl->useCompetitiveLearning = (ruleType == HebbianRuleType::COMPETITIVE);
    pImpl->useMetaplasticity = (ruleType == HebbianRuleType::METAPLASTIC);
    pImpl->useHardLimit = (ruleType == HebbianRuleType::HARD_LIMIT);
    setLearningRate(learningRate);
}

void Hebbian::configureAdvanced(HebbianRuleType ruleType, float learningRate,
                                bool useNormalization, float competitionStrength,
                                float metaplasticDecay) {
    setLearningRule(ruleType, learningRate);
    pImpl->useOjaNormalization = useNormalization;
    pImpl->competitionStrength = competitionStrength;
    pImpl->metaplasticDecay = metaplasticDecay;
    pImpl->useHardLimit = (ruleType == HebbianRuleType::HARD_LIMIT);
}

void Hebbian::enableMetaplasticity(bool enable, float decayRate) {
    pImpl->useMetaplasticity = enable;
    pImpl->metaplasticDecay = decayRate;
}

void Hebbian::enableCompetitiveLearning(bool enable, float strength) {
    pImpl->useCompetitiveLearning = enable;
    pImpl->competitionStrength = strength;
}

void Hebbian::enableOjaNormalization(bool enable) {
    pImpl->useOjaNormalization = enable;
}

void Hebbian::enableHardLimit(bool enable) {
    pImpl->useHardLimit = enable;
}

float Hebbian::getBaselineActivity() const {
    return pImpl->baselineActivity;
}

void Hebbian::resetActivityHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->currentActivity = 0.0f;
    pImpl->baselineActivity = 0.0f;
}
