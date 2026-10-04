#include "NeuromodulatorManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

struct NeuromodulatorManager::Impl {
    class Brain* brain;
    
    // Neuromodulator instances
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Reward> reward;
    
    // Generic neuromodulator storage (for template method)
    std::unordered_map<std::string, std::unique_ptr<Neuromodulator>> neuromodulators;
    
    // Configuration parameters
    float learningRate;
    float explorationBonus;
    float predictionErrorDecay;
    float noveltyThreshold;
    
    // State tracking
    float lastReward;
    float lastPredictionError;
    std::vector<float> observationHistory;
    size_t historySize;
    
    // Bayesian parameters for curiosity
    float uncertaintySum;
    float experienceCount;
    
    Impl() : brain(nullptr)
        , learningRate(0.001f)
        , explorationBonus(0.1f)
        , predictionErrorDecay(0.95f)
        , noveltyThreshold(0.3f)
        , lastReward(0.0f)
        , lastPredictionError(0.0f)
        , historySize(100)
        , uncertaintySum(0.0f)
        , experienceCount(0.0f) {
    }
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl) {}

NeuromodulatorManager::~NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(class Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuromodulatorManager with null brain");
        return;
    }
    
    pImpl->brain = brain;
    
    // Create neuromodulator instances
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->reward = std::make_unique<Reward>();
    
    // Initialize neuromodulators with brain reference
    pImpl->curiosity->initialize(brain);
    pImpl->novelty->initialize(brain);
    pImpl->predictionError->initialize(brain);
    
    NLM_LOG_INFO("NeuromodulatorManager initialized with " + std::to_string(pImpl->historySize) + " observation history size");
}

template<typename T>
void NeuromodulatorManager::addNeuromodulator(const std::string& name) {
    static_assert(std::is_base_of<Neuromodulator, T>::value, "T must be derived from Neuromodulator");
    
    // Check if already exists
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        NLM_LOG_WARNING("Neuromodulator '" + name + "' already exists");
        return;
    }
    
    // Create new neuromodulator
    auto neuromodulator = std::make_unique<T>();
    neuromodulator->initialize(pImpl->brain);
    pImpl->neuromodulators[name] = std::move(neuromulator);
    
    NLM_LOG_INFO("Added neuromodulator: " + name);
}

Neuromodulator* NeuromodulatorManager::getNeuromodulator(const std::string& name) {
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        return it->second.get();
    }
    return nullptr;
}

const Neuromodulator* NeuromodulatorManager::getNeuromodulator(const std::string& name) const {
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        return it->second.get();
    }
    return nullptr;
}

Dopamine* NeuromodulatorManager::getDopamine() {
    return pImpl->dopamine.get();
}

const Dopamine* NeuromodulatorManager::getDopamine() const {
    return pImpl->dopamine.get();
}

Curiosity* NeuromodulatorManager::getCuriosity() {
    return pImpl->curiosity.get();
}

const Curiosity* NeuromodulatorManager::getCuriosity() const {
    return pImpl->curiosity.get();
}

Novelty* NeuromodulatorManager::getNovelty() {
    return pImpl->novelty.get();
}

const Novelty* NeuromodulatorManager::getNovelty() const {
    return pImpl->novelty.get();
}

PredictionError* NeuromodulatorManager::getPredictionError() {
    return pImpl->predictionError.get();
}

const PredictionError* NeuromodulatorManager::getPredictionError() const {
    return pImpl->predictionError.get();
}

Reward* NeuromodulatorManager::getReward() {
    return pImpl->reward.get();
}

const Reward* NeuromodulatorManager::getReward() const {
    return pImpl->reward.get();
}

std::vector<std::string> NeuromodulatorManager::getNeuromodulatorNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->neuromodulators.size());
    for (const auto& pair : pImpl->neuromodulators) {
        names.push_back(pair.first);
    }
    return names;
}

void NeuromodulatorManager::processRewardEvent(float reward) {
    if (!pImpl->dopamine || !pImpl->predictionError) return;
    
    pImpl->lastReward = reward;
    pImpl->dopamine->signalReward(reward);
    
    // Compute prediction error based on reward prediction
    float expectedReward = pImpl->predictionError->getPredictedValue();
    float error = reward - expectedReward;
    pImpl->predictionError->computeError(expectedReward, reward);
    
    // Update Bayesian uncertainty for curiosity
    pImpl->uncertaintySum += 1.0f / (pImpl->experienceCount + 1.0f);
    pImpl->experienceCount += 1.0f;
    
    NLM_LOG_DEBUG("Processed reward event: " + std::to_string(reward) + ", prediction error: " + std::to_string(error));
}

void NeuromodulatorManager::processPredictionErrorEvent(float error) {
    if (!pImpl->predictionError) return;
    
    pImpl->lastPredictionError = error;
    pImpl->predictionError->computeError(pImpl->predictionError->getPredictedValue(), 
                                        pImpl->predictionError->getPredictedValue() + error);
    
    // Update dopamine based on prediction error (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(error);
    }
    
    NLM_LOG_DEBUG("Processed prediction error event: " + std::to_string(error));
}

void NeuromodulatorManager::processSensoryInput(const std::vector<float>& pattern) {
    if (!pImpl->novelty || pattern.empty()) return;
    
    // Add current pattern to history
    pImpl->observationHistory.push_back(pattern[0]); // Use first value for simplicity
    if (pImpl->observationHistory.size() > pImpl->historySize) {
        pImpl->observationHistory.erase(pImpl->observationHistory.begin());
    }
    
    // Detect novelty
    if (pImpl->observationHistory.size() >= 2) {
        float currentPattern = pattern[0];
        float previousPattern = pImpl->observationHistory.back();
        
        // Simple novelty detection based on statistical deviation
        float mean = 0.0f, variance = 0.0f;
        for (float val : pImpl->observationHistory) {
            mean += val;
        }
        mean /= pImpl->observationHistory.size();
        
        for (float val : pImpl->observationHistory) {
            float diff = val - mean;
            variance += diff * diff;
        }
        variance /= pImpl->observationHistory.size();
        
        float stdDev = std::sqrt(variance);
        if (stdDev > 0.0f) {
            float zScore = std::abs(currentPattern - mean) / stdDev;
            
            if (zScore > pImpl->noveltyThreshold) {
                // Set novelty level based on z-score
                float noveltyLevel = std::min(1.0f, zScore / 3.0f); // Normalize to 0-1
                pImpl->novelty->setLevel(noveltyLevel);
            }
        }
    }
}

void NeuromodulatorManager::processObservation(const Observation& observation) {
    if (!pImpl->reward) return;
    
    // Compute reward from observation
    float reward = pImpl->reward->computeReward(observation);
    
    // Process the reward
    processRewardEvent(reward);
}

void NeuromodulatorManager::reset() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    if (pImpl->predictionError) pImpl->predictionError->clearHistory();
    if (pImpl->reward) pImpl->reward->reset();
    
    pImpl->observationHistory.clear();
    pImpl->lastReward = 0.0f;
    pImpl->lastPredictionError = 0.0f;
    
    NLM_LOG_INFO("NeuromodulatorManager reset");
}

void NeuromodulatorManager::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

void NeuromodulatorManager::setExplorationBonus(float bonus) {
    pImpl->explorationBonus = std::max(0.0f, bonus);
}

void NeuromodulatorManager::setPredictionErrorDecay(float decay) {
    pImpl->predictionErrorDecay = std::clamp(decay, 0.0f, 1.0f);
}

void NeuromodulatorManager::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::max(0.0f, threshold);
}

void NeuromodulatorManager::logStatus() const {
    NLM_LOG_INFO("=== NeuromodulatorManager Status ===");
    NLM_LOG_INFO("Learning rate: " + std::to_string(pImpl->learningRate));
    NLM_LOG_INFO("Exploration bonus: " + std::to_string(pImpl->explorationBonus));
    NLM_LOG_INFO("Prediction error decay: " + std::to_string(pImpl->predictionErrorDecay));
    NLM_LOG_INFO("Novelty threshold: " + std::to_string(pImpl->noveltyThreshold));
    NLM_LOG_INFO("Experience count: " + std::to_string(pImpl->experienceCount));
    NLM_LOG_INFO("Bayesian uncertainty: " + std::to_string(pImpl->uncertaintySum / std::max(1.0f, pImpl->experienceCount)));
    
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
    }
    if (pImpl->novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->novelty->getLevel()));
    }
    if (pImpl->predictionError) {
        NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionError->getError()));
    }
    if (pImpl->reward) {
        NLM_LOG_INFO("Reward value: " + std::to_string(pImpl->reward->getValue()));
    }
}

} // namespace nlm
