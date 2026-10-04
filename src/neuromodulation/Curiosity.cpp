#include "Curiosity.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

struct Curiosity::Impl {
    // Bayesian uncertainty estimation
    struct {
        float priorMean;           // Prior mean of reward distribution
        float priorVariance;       // Prior variance of reward distribution
        float posteriorMean;       // Posterior mean after observation
        float posteriorVariance;   // Posterior variance after observation
        float learningRate;        // Learning rate for Bayesian updating
        float explorationConstant; // Exploration constant (information gain)
        float confidenceThreshold; // Confidence threshold for exploration
        std::vector<float> rewardHistory; // History of observed rewards
        size_t historySize;        // Size of reward history buffer
    } bayesian;
    
    // Action selection and risk assessment
    struct {
        std::unordered_map<size_t, float> actionValues;    // Estimated value of each action
        std::unordered_map<size_t, float> actionRisks;     // Risk estimate for each action
        std::unordered_map<size_t, float> actionUncertainties; // Uncertainty of each action
        float explorationFactor;    // Balance between exploration and exploitation
        float riskAversion;         // Degree of risk aversion
        float softmaxTemperature;   // Temperature for softmax action selection
        size_t bestAction;          // Currently best action
        size_t explorationCount;    // Number of exploration steps
    } actionSelection;
    
    // Context-dependent modulation
    struct {
        float contextualWeights[4];  // Weights for different context dimensions
        float contextChangeRate;     // Rate of context change
        float memoryDecay;           // Decay rate for context memory
        std::vector<float> contextHistory; // History of context states
        std::vector<float> rewardHistory;  // History of rewards in current context
    } context;
    
    // Intrinsic reward computation
    struct {
        float noveltyBonus;         // Bonus for novel actions/experiences
        float predictionErrorBonus; // Bonus for prediction errors
        float uncertaintyBonus;     // Bonus for high uncertainty
        float totalIntrinsicReward; // Total intrinsic reward
        float baselineIntrinsic;    // Baseline intrinsic motivation level
    } intrinsic;
    
    // Implementation tracking
    bool initialized;              // Whether curiosity system has been initialized
    bool enabled;                   // Whether curiosity system is enabled
    float lastUpdateTime;           // Last update timestamp
    float learningProgress;         // Overall learning progress
    
    Impl() {
        // Initialize Bayesian uncertainty estimation
        bayesian.priorMean = 0.0f;
        bayesian.priorVariance = 1.0f;
        bayesian.posteriorMean = 0.0f;
        bayesian.posteriorVariance = 1.0f;
        bayesian.learningRate = 0.1f;
        bayesian.explorationConstant = 1.0f;
        bayesian.confidenceThreshold = 0.8f;
        bayesian.historySize = 50;
        
        // Initialize action selection
        actionSelection.explorationFactor = 0.3f;
        actionSelection.riskAversion = 0.5f;
        actionSelection.softmaxTemperature = 1.0f;
        actionSelection.bestAction = 0;
        actionSelection.explorationCount = 0;
        
        // Initialize context-dependent modulation
        for (int i = 0; i < 4; ++i) {
            context.contextualWeights[i] = 1.0f / 4.0f;
        }
        context.contextChangeRate = 0.05f;
        context.memoryDecay = 0.95f;
        
        // Initialize intrinsic reward computation
        intrinsic.noveltyBonus = 0.1f;
        intrinsic.predictionErrorBonus = 0.2f;
        intrinsic.uncertaintyBonus = 0.3f;
        intrinsic.totalIntrinsicReward = 0.0f;
        intrinsic.baselineIntrinsic = 0.05f;
        
        // Implementation tracking
        initialized = false;
        enabled = true;
        lastUpdateTime = 0.0f;
        learningProgress = 0.0f;
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize Curiosity system with null brain");
        return;
    }
    
    pImpl->brain = brain;
    pImpl->initialized = true;
    
    // Initialize context history
    pImpl->context.contextHistory.reserve(pImpl->context.contextHistory.capacity() + 10);
    pImpl->context.rewardHistory.reserve(pImpl->context.rewardHistory.capacity() + 10);
    
    NLM_LOG_INFO("Curiosity system initialized with Bayesian exploration");
}

float Curiosity::getLevel() const {
    if (!pImpl->initialized) return 0.0f;
    
    // Compute curiosity level as weighted combination of components
    float uncertaintyComponent = 1.0f - (pImpl->bayesian.posteriorVariance / 
                                        std::max(1.0f, pImpl->bayesian.priorVariance));
    
    float predictionErrorComponent = std::abs(pImpl->brain ? 
                                              pImpl->brain->getPredictionErrorSignal()->getError() : 0.0f);
    
    float explorationComponent = pImpl->actionSelection.explorationCount / 
                                std::max(1.0f, static_cast<float>(pImpl->context.contextHistory.size()));
    
    // Combine components
    float curiosityLevel = (uncertaintyComponent * 0.4f + 
                          predictionErrorComponent * 0.3f + 
                          explorationComponent * 0.3f);
    
    return std::min(1.0f, curiosityLevel);
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    if (!pImpl->initialized || !pImpl->enabled) return;
    
    float dtFloat = static_cast<float>(dt);
    float currentTime = pImpl->lastUpdateTime + dtFloat;
    pImpl->lastUpdateTime = currentTime;
    
    // Update Bayesian uncertainty estimation
    updateBayesianEstimation(predictionError);
    
    // Update action selection based on current uncertainty
    updateActionSelection(currentTime);
    
    // Update context-dependent modulation
    updateContextModulation(novelty);
    
    // Compute intrinsic reward
    computeIntrinsicReward(novelty, predictionError, dtFloat);
    
    // Decay learning progress
    pImpl->learningProgress *= 0.95f;
}

float Curiosity::getExplorationDrive() const {
    if (!pImpl->initialized) return 0.0f;
    
    // Compute exploration drive based on multiple factors
    float uncertaintyDrive = 1.0f - (pImpl->bayesian.posteriorVariance / 
                                    std::max(1.0f, pImpl->bayesian.priorVariance));
    
    float predictionErrorDrive = std::abs(pImpl->brain ? 
                                         pImpl->brain->getPredictionErrorSignal()->getError() : 0.0f);
    
    float intrinsicDrive = pImpl->intrinsic.totalIntrinsicReward;
    
    // Combine drives with weights
    float explorationDrive = (uncertaintyDrive * 0.4f + 
                             predictionErrorDrive * 0.3f + 
                             intrinsicDrive * 0.3f);
    
    return std::min(1.0f, explorationDrive);
}

void Curiosity::setNoveltyWeight(float weight) {
    if (pImpl->initialized) {
        pImpl->intrinsic.noveltyBonus = weight;
    }
}

void Curiosity::setPredictionErrorWeight(float weight) {
    if (pImpl->initialized) {
        pImpl->intrinsic.predictionErrorBonus = weight;
    }
}

void Curiosity::reset() {
    if (!pImpl->initialized) return;
    
    // Reset Bayesian estimates
    pImpl->bayesian.posteriorMean = 0.0f;
    pImpl->bayesian.posteriorVariance = 1.0f;
    pImpl->bayesian.rewardHistory.clear();
    
    // Reset action selection
    pImpl->actionSelection.actionValues.clear();
    pImpl->actionSelection.actionRisks.clear();
    pImpl->actionSelection.actionUncertainties.clear();
    pImpl->actionSelection.explorationCount = 0;
    
    // Reset context
    pImpl->context.contextHistory.clear();
    pImpl->context.rewardHistory.clear();
    
    // Reset intrinsic reward
    pImpl->intrinsic.totalIntrinsicReward = pImpl->intrinsic.baselineIntrinsic;
    
    // Reset learning progress
    pImpl->learningProgress = 0.0f;
    
    NLM_LOG_INFO("Curiosity system reset");
}

void Curiosity::updateBayesianEstimation(float predictionError) {
    if (!pImpl->brain) return;
    
    // Get current reward from dopamine system
    float reward = pImpl->brain->getDopamine()->getLevel();
    
    // Update reward history
    pImpl->bayesian.rewardHistory.push_back(reward);
    if (pImpl->bayesian.rewardHistory.size() > pImpl->bayesian.historySize) {
        pImpl->bayesian.rewardHistory.erase(pImpl->bayesian.rewardHistory.begin());
    }
    
    // Bayesian update for normal distribution
    if (pImpl->bayesian.rewardHistory.size() > 1) {
        // Compute empirical mean and variance from history
        float sum = 0.0f, sumSq = 0.0f;
        for (float r : pImpl->bayesian.rewardHistory) {
            sum += r;
            sumSq += r * r;
        }
        
        float n = static_cast<float>(pImpl->bayesian.rewardHistory.size());
        float empiricalMean = sum / n;
        float empiricalVariance = (sumSq - sum * sum / n) / (n - 1);
        
        // Update posterior estimates
        float effectiveSampleSize = 1.0f / pImpl->bayesian.priorVariance;
        
        pImpl->bayesian.posteriorMean = (pImpl->bayesian.priorMean * effectiveSampleSize + 
                                         empiricalMean * n) / (effectiveSampleSize + n);
        
        pImpl->bayesian.posteriorVariance = 1.0f / (effectiveSampleSize + n / empiricalVariance);
        
        // Update prior for next time step
        pImpl->bayesian.priorMean = pImpl->bayesian.posteriorMean;
        pImpl->bayesian.priorVariance = pImpl->bayesian.posteriorVariance;
    }
    
    // Update learning progress based on reduction in uncertainty
    float initialUncertainty = 1.0f / pImpl->bayesian.priorVariance;
    float currentUncertainty = 1.0f / pImpl->bayesian.posteriorVariance;
    
    if (initialUncertainty > 0.0f) {
        float learningGain = std::abs(currentUncertainty - initialUncertainty) / initialUncertainty;
        pImpl->learningProgress = std::min(1.0f, pImpl->learningProgress + learningGain * 0.1f);
    }
}

void Curiosity::updateActionSelection(float currentTime) {
    if (!pImpl->brain) return;
    
    auto* rng = pImpl->brain->getRandomGenerator();
    if (!rng) return;
    
    // Get current motor neuron activities to estimate action values
    float totalActivity = 0.0f;
    size_t activeActionCount = 0;
    
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    totalActivity += std::abs(neuron->getState().membranePotential - 
                                            neuron->getState().restingPotential);
                    activeActionCount++;
                }
            }
        }
    }
    
    float avgActivity = totalActivity / std::max(1.0f, static_cast<float>(activeActionCount));
    
    // Update action values based on current activity
    if (activeActionCount > 0) {
        // Add exploration bonus based on uncertainty
        float explorationBonus = (1.0f - pImpl->bayesian.posteriorVariance / 
                                 std::max(1.0f, pImpl->bayesian.priorVariance)) * 
                                pImpl->actionSelection.explorationFactor;
        
        // Update action values with new observations
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Motor) {
                    for (auto* neuron : pop->getNeurons()) {
                        size_t actionId = neuron->getId();
                        float actionValue = avgActivity + explorationBonus;
                        
                        // Apply risk assessment
                        float risk = calculateActionRisk(actionId, actionValue);
                        float riskAdjustedValue = actionValue * (1.0f - pImpl->actionSelection.riskAversion * risk);
                        
                        pImpl->actionSelection.actionValues[actionId] = riskAdjustedValue;
                    }
                }
            }
        }
    }
    
    // Select best action
    selectBestAction(rng->uniformReal(0.0f, 1.0f));
    
    // Increment exploration count if uncertainty is high
    if (pImpl->bayesian.posteriorVariance > pImpl->bayesian.confidenceThreshold) {
        pImpl->actionSelection.explorationCount++;
    }
}

void Curiosity::selectBestAction(float explorationProbability) {
    if (pImpl->actionSelection.actionValues.empty()) {
        pImpl->actionSelection.bestAction = 0;
        return;
    }
    
    // Decide between exploitation and exploration
    float r = explorationProbability;
    
    if (r < pImpl->actionSelection.explorationFactor) {
        // Exploration: choose random action
        pImpl->actionSelection.bestAction = pImpl->actionSelection.explorationCount % 6; // Assume 6 actions
        NLM_LOG_DEBUG("Curiosity: Selecting random action (exploration)");
    } else {
        // Exploitation: choose action with highest estimated value
        float bestValue = -std::numeric_limits<float>::max();
        pImpl->actionSelection.bestAction = 0;
        
        for (const auto& pair : pImpl->actionSelection.actionValues) {
            if (pair.second > bestValue) {
                bestValue = pair.second;
                pImpl->actionSelection.bestAction = pair.first;
            }
        }
        
        NLM_LOG_DEBUG("Curiosity: Selecting best action " + std::to_string(pImpl->actionSelection.bestAction));
    }
}

void Curiosity::updateContextModulation(float novelty) {
    // Update context weights based on novelty
    if (novelty > 0.5f) {
        // High novelty increases context sensitivity
        for (int i = 0; i < 4; ++i) {
            pImpl->context.contextualWeights[i] *= (1.0f + pImpl->context.contextChangeRate);
        }
    }
    
    // Decay context weights
    for (int i = 0; i < 4; ++i) {
        pImpl->context.contextualWeights[i] *= pImpl->context.memoryDecay;
    }
    
    // Normalize weights
    float totalWeight = 0.0f;
    for (int i = 0; i < 4; ++i) {
        totalWeight += pImpl->context.contextualWeights[i];
    }
    
    if (totalWeight > 0.0f) {
        for (int i = 0; i < 4; ++i) {
            pImpl->context.contextualWeights[i] /= totalWeight;
        }
    }
    
    // Store context state
    pImpl->context.contextHistory.push_back(novelty);
    if (pImpl->context.contextHistory.size() > 10) {
        pImpl->context.contextHistory.erase(pImpl->context.contextHistory.begin());
    }
}

void Curiosity::computeIntrinsicReward(float novelty, float predictionError, float dt) {
    // Compute novelty bonus
    float noveltyComponent = novelty * pImpl->intrinsic.noveltyBonus;
    
    // Compute prediction error bonus
    float predictionErrorComponent = std::abs(predictionError) * pImpl->intrinsic.predictionErrorBonus;
    
    // Compute uncertainty bonus
    float uncertaintyComponent = (1.0f - pImpl->bayesian.posteriorVariance / 
                                std::max(1.0f, pImpl->bayesian.priorVariance)) * 
                               pImpl->intrinsic.uncertaintyBonus;
    
    // Combine components
    pImpl->intrinsic.totalIntrinsicReward = (noveltyComponent + 
                                           predictionErrorComponent + 
                                           uncertaintyComponent) * dt;
    
    // Add baseline intrinsic motivation
    pImpl->intrinsic.totalIntrinsicReward += pImpl->intrinsic.baselineIntrinsic * dt;
}

float Curiosity::calculateActionRisk(size_t actionId, float actionValue) {
    // Simple risk calculation based on variance of action values
    float totalValue = 0.0f;
    float totalSqValue = 0.0f;
    size_t count = 0;
    
    for (const auto& pair : pImpl->actionSelection.actionValues) {
        totalValue += pair.second;
        totalSqValue += pair.second * pair.second;
        count++;
    }
    
    if (count < 2) return 0.0f;
    
    float meanValue = totalValue / count;
    float variance = (totalSqValue - totalValue * totalValue / count) / (count - 1);
    
    // Risk is proportional to variance
    return std::sqrt(std::max(0.0f, variance));
}

} // namespace nlm
