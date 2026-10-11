#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

struct Curiosity::Impl {
    Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float explorationTemperature;
    float informationGainWeight;
    float activeExplorationThreshold;
    float adaptiveThreshold;
    
    // Active exploration state
    std::vector<std::vector<float>> recentExperiences;
    std::vector<float> experienceRewards;
    std::vector<ActionType> actionHistory;
    std::vector<std::vector<float>> stateHistory;
    
    // Learning metrics
    std::vector<float> informationGainHistory;
    std::vector<float> explorationRewardsHistory;
    float totalExplorationReward;
    size_t explorationCount;
    
    // Internal state for curiosity-driven learning
    float metaLearningRate;
    float curiosityModulation;
    float explorationExploitationBalance;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , explorationTemperature(0.5f)
        , informationGainWeight(0.3f)
        , activeExplorationThreshold(0.7f)
        , adaptiveThreshold(0.5f)
        , totalExplorationReward(0.0f)
        , explorationCount(0)
        , metaLearningRate(0.01f)
        , curiosityModulation(1.0f)
        , explorationExploitationBalance(0.5f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with active learning loop");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with adaptive rate based on learning history
    float adaptiveRate = pImpl->metaLearningRate * pImpl->curiosityModulation;
    pImpl->level += (target - pImpl->level) * adaptiveRate;
    
    // Decay with temperature parameter (affects exploration/exploitation balance)
    float decay = pImpl->decayRate * static_cast<float>(dt) * pImpl->explorationTemperature;
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Modulate exploration-exploitation balance based on curiosity level
    pImpl->explorationExploitationBalance = 0.5f + pImpl->level * 0.4f;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::driveActiveExploration(Brain* brain) {
    if (!brain || pImpl->level < pImpl->activeExplorationThreshold) return;
    
    // Integrate with prediction system for active exploration
    PredictionSystem* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        // Use prediction error to drive exploration
        float predictionError = predictionSystem->getPredictionError();
        
        // Modify action selection based on curiosity
        float explorationModulation = pImpl->level * 0.3f * (1.0f + predictionError);
        
        // Log active exploration
        NLM_LOG_INFO("Active exploration driven by curiosity (level: " + 
                    std::to_string(pImpl->level) + 
                    ", prediction error: " + std::to_string(predictionError) + ")");
    }
    
    // Update exploration statistics
    pImpl->explorationCount++;
    pImpl->totalExplorationReward += pImpl->level * 0.1f;
}

void Curiosity::implementCuriosityDrivenLearning() {
    if (!pImpl->brain) return;
    
    // Information gain calculation for curiosity-driven learning
    calculateInformationGain();
    
    // Update curiosity based on information gain
    if (!pImpl->informationGainHistory.empty()) {
        float recentInformationGain = pImpl->informationGainHistory.back();
        pImpl->level += recentInformationGain * pImpl->informationGainWeight * 0.1f;
        
        // Cap the learning rate
        pImpl->level = std::min(1.0f, pImpl->level);
    }
    
    // Meta-learning: adjust curiosity parameters based on learning outcomes
    metaLearningUpdate();
}

void Curiosity::modulateActionSelection(float curiosityLevel) {
    if (!pImpl->brain) return;
    
    // Use curiosity to modulate action selection
    float explorationBonus = curiosityLevel * pImpl->explorationTemperature;
    
    // Update neural planner with curiosity modulation
    NeuralPlanner* planner = pImpl->brain->getPlanner();
    if (planner) {
        planner->modulateCuriosityDrivenExploration(explorationBonus);
    }
    
    // Update action probabilities based on curiosity
    updateActionProbabilities(explorationBonus);
}

void Curiosity::integrateWithPredictionSystem() {
    if (!pImpl->brain) return;
    
    // Integrate with prediction system for forward model learning
    PredictionSystem* predictionSystem = pImpl->brain->getPredictionSystem();
    if (predictionSystem) {
        // Use curiosity to drive prediction error learning
        float curiosityContribution = pImpl->level * 0.3f;
        
        // This would integrate with prediction error computation
        // For now, just log the integration
        if (pImpl->level > 0.3f) {
            NLM_LOG_INFO("Curiosity driving prediction system learning (level: " + 
                        std::to_string(pImpl->level) + ")");
        }
    }
}

void Curiosity::calculateInformationGain() {
    // Calculate information gain for new experiences
    if (pImpl->stateHistory.size() < 2) return;
    
    // Compute surprise based on prediction errors
    float currentState = computeStateSurprise();
    
    // Information gain is negative log probability of surprise
    float informationGain = -std::log(std::max(0.01f, currentState));
    
    pImpl->informationGainHistory.push_back(informationGain);
    
    // Keep history bounded
    if (pImpl->informationGainHistory.size() > 1000) {
        pImpl->informationGainHistory.erase(pImpl->informationGainHistory.begin());
    }
}

void Curiosity::metaLearningUpdate() {
    // Meta-learning: adjust curiosity parameters based on learning efficiency
    if (pImpl->informationGainHistory.empty()) return;
    
    // Calculate average information gain
    float avgInformationGain = 0.0f;
    for (float gain : pImpl->informationGainHistory) {
        avgInformationGain += gain;
    }
    avgInformationGain /= pImpl->informationGainHistory.size();
    
    // Adapt learning rate based on information gain
    if (avgInformationGain > 0.5f) {
        // High information gain - reduce learning rate to avoid overfitting
        pImpl->metaLearningRate *= 0.9f;
        pImpl->curiosityModulation *= 0.95f;
    } else if (avgInformationGain < 0.1f) {
        // Low information gain - increase exploration
        pImpl->explorationTemperature = std::min(1.0f, pImpl->explorationTemperature * 1.1f);
    }
}

void Curiosity::updateActionProbabilities(float explorationBonus) {
    // Update action selection probabilities based on curiosity
    if (!pImpl->brain) return;
    
    // This would integrate with the action selection system
    // For now, just log the effect
    if (explorationBonus > 0.2f) {
        NLM_LOG_INFO("Curiosity increasing exploration bonus (bonus: " + 
                    std::to_string(explorationBonus) + ")");
    }
}

float Curiosity::computeStateSurprise() const {
    if (pImpl->stateHistory.size() < 2) return 0.5f; // Default surprise
    
    // Compute state prediction surprise
    const auto& currentState = pImpl->stateHistory.back();
    const auto& previousState = pImpl->stateHistory[pImpl->stateHistory.size() - 2];
    
    if (currentState.size() != previousState.size() || currentState.empty()) {
        return 0.5f;
    }
    
    // Compute KL divergence as surprise measure
    float klDivergence = 0.0f;
    for (size_t i = 0; i < currentState.size(); ++i) {
        float p = currentState[i];  // Current state probability
        float q = previousState[i];  // Previous state probability
        
        if (p > 0.0f && q > 0.0f) {
            klDivergence += p * std::log(p / q);
        }
    }
    
    return std::min(1.0f, klDivergence);
}

void Curiosity::recordExperience(const std::vector<float>& state,
                                ActionType action,
                                float reward) {
    pImpl->stateHistory.push_back(state);
    pImpl->actionHistory.push_back(action);
    pImpl->experienceRewards.push_back(reward);
    
    // Keep history bounded
    size_t maxHistorySize = 1000;
    if (pImpl->stateHistory.size() > maxHistorySize) {
        pImpl->stateHistory.erase(pImpl->stateHistory.begin());
        pImpl->actionHistory.erase(pImpl->actionHistory.begin());
        pImpl->experienceRewards.erase(pImpl->experienceRewards.begin());
    }
}

float Curiosity::getAverageExplorationReward() const {
    if (pImpl->explorationCount == 0) return 0.0f;
    return pImpl->totalExplorationReward / pImpl->explorationCount;
}

float Curiosity::getInformationGain() const {
    if (pImpl->informationGainHistory.empty()) return 0.0f;
    
    float totalGain = 0.0f;
    for (float gain : pImpl->informationGainHistory) {
        totalGain += gain;
    }
    return totalGain / pImpl->informationGainHistory.size();
}

void Curiosity::setExplorationTemperature(float temperature) {
    pImpl->explorationTemperature = std::clamp(temperature, 0.1f, 2.0f);
}

void Curiosity::setActiveExplorationThreshold(float threshold) {
    pImpl->activeExplorationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Curiosity::setAdaptiveThreshold(float threshold) {
    pImpl->adaptiveThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->stateHistory.clear();
    pImpl->actionHistory.clear();
    pImpl->experienceRewards.clear();
    pImpl->informationGainHistory.clear();
    pImpl->explorationRewardsHistory.clear();
    pImpl->totalExplorationReward = 0.0f;
    pImpl->explorationCount = 0;
    pImpl->metaLearningRate = 0.01f;
    pImpl->curiosityModulation = 1.0f;
    pImpl->explorationExploitationBalance = 0.5f;
}

} // namespace nlm
