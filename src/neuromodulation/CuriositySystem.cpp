#include "CuriositySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace nlm {

struct CuriositySystem::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<NeuromodulationEngine> engine;
    CuriosityState state;
    ExplorationParams params;
    IntrinsicMotivation intrinsicMotivation;
    ActionSelectionState actionSelection;
    
    // History tracking
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<std::vector<float>> actionValueHistories;
    
    // Integration state
    float dopamineIntegration;
    float noveltyIntegration;
    float predictionErrorIntegration;
    
    // Performance metrics
    float explorationEfficiency;
    float intrinsicMotivationEfficiency;
    float noveltyUtilization;
    
    // Safety bounds
    float minExplorationDrive;
    float maxExplorationDrive;
    float minIntrinsicMotivation;
    float maxIntrinsicMotivation;
    
    Impl() : brain(nullptr), engine(nullptr),
             dopamineIntegration(0.0f), noveltyIntegration(0.0f),
             predictionErrorIntegration(0.0f),
             explorationEfficiency(0.0f), intrinsicMotivationEfficiency(0.0f),
             noveltyUtilization(0.0f),
             minExplorationDrive(0.1f), maxExplorationDrive(2.0f),
             minIntrinsicMotivation(0.0f), maxIntrinsicMotivation(1.0f) {
        
        // Initialize action value histories (for up to 10 actions)
        actionValueHistories.resize(10);
        for (auto& history : actionValueHistories) {
            history.reserve(1000);
        }
        
        // Initialize novelty and prediction error histories
        noveltyHistory.reserve(1000);
        predictionErrorHistory.reserve(1000);
    }
};

CuriositySystem::CuriositySystem() : pImpl(new Impl) {
    NLM_LOG_INFO("CuriositySystem created");
}

CuriositySystem::~CuriositySystem() {
    NLM_LOG_INFO("CuriositySystem destroyed");
}

CuriositySystem::CuriositySystem(CuriositySystem&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {
    if (pImpl) {
        pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
        pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
    }
}

CuriositySystem& CuriositySystem::operator=(CuriositySystem&& other) noexcept {
    if (this != &other) {
        shutdown();
        pImpl = std::exchange(other.pImpl, nullptr);
        if (pImpl) {
            pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
            pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
        }
    }
    return *this;
}

bool CuriositySystem::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine) {
    try {
        pImpl->brain = brain;
        pImpl->engine = engine;
        
        if (!pImpl->brain || !pImpl->engine) {
            NLM_LOG_ERROR("CuriositySystem initialization failed: invalid dependencies");
            return false;
        }
        
        NLM_LOG_INFO("CuriositySystem initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("CuriositySystem initialization failed: " + std::string(e.what()));
        return false;
    }
}

void CuriositySystem::update(TimestepDuration dt, SimulationStep step) {
    // Update intrinsic motivation components
    computeIntrinsicMotivation();
    
    // Update exploration drive
    computeExplorationDrive({});  // Would need action values in real system
    
    // Update state
    updateState(dt);
    
    // Update exploration bonus
    pImpl->state.explorationBonus -= pImpl->params.explorationBonusDecay * static_cast<float>(dt);
    pImpl->state.explorationBonus = std::max(0.0f, pImpl->state.explorationBonus);
}

void CuriositySystem::updateCuriosity(float novelty, float predictionError, float reward, float timeStep) {
    // Store in history
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    
    if (pImpl->noveltyHistory.size() > 1000) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
    }
    if (pImpl->predictionErrorHistory.size() > 1000) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Update novelty sensitivity based on prediction error
    pImpl->state.noveltySensitivity = 1.0f + predictionError * 0.5f;
    
    // Update habituation based on novelty exposure
    if (enableHabituation_) {
        pImpl->state.habituationLevel += novelty * pImpl->params.habituationRate * timeStep;
        pImpl->state.habituationLevel = std::min(1.0f, pImpl->state.habituationLevel);
    }
    
    // Update last novelty time
    pImpl->state.lastNoveltyTime = timeStep;
    
    // Compute intrinsic motivation components
    computeIntrinsicMotivation();
    
    // Update exploration drive
    computeExplorationDrive({});  // Would need action values
}

void CuriositySystem::computeIntrinsicMotivation(const class EnvironmentState& environmentState) {
    // Compute empirical curiosity from prediction errors
    computeEmpiricalCuriosity(pImpl->state.noveltySensitivity);
    
    // Compute intrinsic pleasure from novel stimuli
    computeIntrinsicPleasure(pImpl->state.noveltySensitivity);
    
    // Compute goal irrelevance
    computeGoalIrrelevance();
    
    // Compute efficiency motivation
    computeEfficiencyMotivation({});  // Would need action values
    
    // Compute flow state motivation
    if (enableFlowState_) {
        // Simple flow state computation based on challenge-skill balance
        float challenge = 0.5f;  // Would be computed from environment
        float skill = 0.5f;     // Would be estimated from past performance
        computeFlowStateMotivation(challenge, skill);
    }
}

void CuriositySystem::computeExplorationDrive(const std::vector<float>& actionValues) {
    // Base exploration drive from intrinsic and extrinsic motivation
    pImpl->state.explorationDrive = 
        pImpl->state.intrinsicMotivation * pImpl->params.intrinsicMotivationWeight +
        pImpl->state.extrinsicMotivation * pImpl->params.extrinsicMotivationWeight;
    
    // Modulate by curiosity amplification
    pImpl->state.explorationDrive *= pImpl->params.curiosityAmplification;
    
    // Apply exploration bonus
    pImpl->state.explorationDrive += pImpl->state.explorationBonus;
    
    // Apply bounds
    enforceBounds();
}

void CuriositySystem::updateActionSelection(std::vector<float>& actionValues) {
    // Apply exploration bonuses to action values
    applyExplorationBonus(actionValues);
    
    // Select best action (placeholder - would use epsilon-greedy or similar)
    if (!actionValues.empty()) {
        float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
        int bestAction = std::distance(actionValues.begin(), 
                                     std::max_element(actionValues.begin(), actionValues.end()));
        
        // Update habituation for selected action
        if (enableHabituation_) {
            updateHabitationForAction(bestAction, true);
        }
    }
}

void CuriositySystem::computeEmpiricalCuriosity(float predictionError) {
    // Empirical curiosity is based on prediction errors
    pImpl->intrinsicMotivation.empiricalCuriosity = 
        std::min(1.0f, predictionError * pImpl->params.uncertaintyWeight * 2.0f);
}

void CuriositySystem::computeIntrinsicPleasure(float novelty) {
    // Intrinsic pleasure from discovering novel stimuli
    pImpl->intrinsicMotivation.intrinsicPleasure = 
        std::min(1.0f, novelty * pImpl->state.noveltySensitivity);
}

void CuriositySystem::computeGoalIrrelevance() {
    // Goal irrelevance drives exploration of irrelevant associations
    // This is inspired by randomwalk curiosity
    pImpl->intrinsicMotivation.goalIrrelevance = 0.3f;  // Fixed for simplicity
}

void CuriositySystem::computeEfficiencyMotivation(const std::vector<float>& actionValues) {
    if (actionValues.empty()) return;
    
    // Efficiency motivation encourages finding efficient behaviors
    float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
    float minValue = *std::min_element(actionValues.begin(), actionValues.end());
    
    pImpl->intrinsicMotivation.efficiencyMotivation = 
        std::min(1.0f, (maxValue - minValue) / (maxValue + 1e-6f) * 2.0f);
}

void CuriositySystem::computeFlowStateMotivation(float challenge, float skill) {
    // Flow state occurs when challenge matches skill level
    float challengeSkillRatio = challenge / (skill + 1e-6f);
    
    // Optimal flow zone is around ratio of 1.0
    pImpl->intrinsicMotivation.flowStateMotivation = 
        std::exp(-std::abs(challengeSkillRatio - 1.0f) * 2.0f);
}

void CuriositySystem::updateState(TimestepDuration dt) {
    // Update exploration drive with smooth transitions
    float targetExploration = pImpl->state.explorationDrive;
    float transitionRate = 0.1f;
    
    float delta = targetExploration - pImpl->state.explorationDrive;
    pImpl->state.explorationDrive += delta * transitionRate * static_cast<float>(dt);
    
    // Update intrinsic motivation components with decay
    pImpl->intrinsicMotivation.empiricalCuriosity *= (1.0f - pImpl->params.habituationRate * static_cast<float>(dt));
    pImpl->intrinsicMotivation.intrinsicPleasure *= (1.0f - pImpl->params.habituationRate * static_cast<float>(dt));
    
    // Update habituation
    if (enableHabituation_) {
        pImpl->state.habituationLevel *= (1.0f - pImpl->params.habituationRate * static_cast<float>(dt));
    }
}

void CuriositySystem::reset() {
    NLM_LOG_INFO("CuriositySystem reset");
    
    // Reset state
    pImpl->state = CuriosityState();
    pImpl->intrinsicMotivation = IntrinsicMotivation();
    pImpl->actionSelection = ActionSelectionState();
    
    // Reset histories
    clearHistory();
}

void CuriositySystem::setExplorationParams(const ExplorationParams& params) {
    pImpl->params = params;
}

void CuriositySystem::integrateWithDopamine(float dopamineLevel) {
    pImpl->dopamineIntegration = dopamineLevel;
    
    // Modulate exploration based on dopamine
    if (dopamineLevel > 0.5f) {
        // High dopamine enhances exploration
        pImpl->state.explorationDrive *= 1.2f;
    } else if (dopamineLevel < 0.2f) {
        // Low dopamine reduces exploration
        pImpl->state.explorationDrive *= 0.8f;
    }
}

void CuriositySystem::integrateWithNovelty(float noveltyLevel) {
    pImpl->noveltyIntegration = noveltyLevel;
    
    // Enhance novelty sensitivity
    pImpl->state.noveltySensitivity = std::min(2.0f, 
                                              pImpl->state.noveltySensitivity + 
                                              noveltyLevel * 0.1f);
}

void CuriositySystem::integrateWithPredictionError(float predictionError) {
    pImpl->predictionErrorIntegration = predictionError;
    
    // Adjust prediction error sensitivity
    pImpl->state.predictionErrorSensitivity = 1.0f + std::abs(predictionError) * 0.3f;
}

float CuriositySystem::computeActionValue(int actionIndex, const std::vector<float>& baseValues) {
    if (actionIndex < 0 || actionIndex >= static_cast<int>(baseValues.size())) {
        return 0.0f;
    }
    
    float value = baseValues[actionIndex];
    
    // Apply exploration bonus
    applyExplorationBonus(const_cast<std::vector<float>&>(baseValues));
    
    value = baseValues[actionIndex];
    
    // Apply habituation penalty
    if (enableHabituation_) {
        value *= (1.0f - pImpl->actionSelection.habituationValues[actionIndex]);
    }
    
    return value;
}

void CuriositySystem::applyExplorationBonus(std::vector<float>& actionValues) {
    if (actionValues.empty()) return;
    
    float explorationFactor = pImpl->state.explorationDrive / pImpl->state.maxExplorationDrive;
    
    for (size_t i = 0; i < actionValues.size(); ++i) {
        // Random exploration bonus
        float randomBonus = static_cast<float>(rand()) / RAND_MAX * explorationFactor * 0.1f;
        
        // Novelty-based exploration bonus
        float noveltyBonus = 0.0f;
        if (pImpl->state.lastNoveltyTime > 0.0f) {
            noveltyBonus = std::exp(-(actionValues.size() - i) / pImpl->state.lastNoveltyTime) * 0.05f;
        }
        
        actionValues[i] += randomBonus + noveltyBonus;
        
        // Store exploration bonus in action selection state
        pImpl->actionSelection.explorationValues[i] = randomBonus + noveltyBonus;
    }
}

void CuriositySystem::updateHabitationForAction(int actionIndex, bool actionTaken) {
    if (actionIndex < 0 || actionIndex >= static_cast<int>(pImpl->actionSelection.habituationValues.size())) {
        return;
    }
    
    if (actionTaken) {
        // Increase habituation for repeated actions
        pImpl->actionSelection.habituationValues[actionIndex] += 0.05f;
        pImpl->actionSelection.habituationValues[actionIndex] = 
            std::min(1.0f, pImpl->actionSelection.habituationValues[actionIndex]);
    } else {
        // Decrease habituation when action is not taken
        pImpl->actionSelection.habituationValues[actionIndex] *= 0.95f;
    }
}

const std::vector<float>& CuriositySystem::getNoveltyHistory() const {
    return pImpl->noveltyHistory;
}

const std::vector<float>& CuriositySystem::getPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

const std::vector<float>& CuriositySystem::getActionValueHistory(int actionIndex) const {
    if (actionIndex >= 0 && actionIndex < static_cast<int>(pImpl->actionValueHistories.size())) {
        return pImpl->actionValueHistories[actionIndex];
    }
    static const std::vector<float> empty;
    return empty;
}

void CuriositySystem::clearHistory() {
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
    for (auto& history : pImpl->actionValueHistories) {
        history.clear();
    }
}

bool CuriositySystem::validateSafetyConstraints() const {
    if (pImpl->state.explorationDrive < pImpl->state.minExplorationDrive ||
        pImpl->state.explorationDrive > pImpl->state.maxExplorationDrive) {
        NLM_LOG_ERROR("CuriositySystem: Exploration drive out of bounds");
        return false;
    }
    
    if (pImpl->state.intrinsicMotivation < pImpl->state.minIntrinsicMotivation ||
        pImpl->state.intrinsicMotivation > pImpl->state.maxIntrinsicMotivation) {
        NLM_LOG_ERROR("CuriositySystem: Intrinsic motivation out of bounds");
        return false;
    }
    
    // Check for numerical instabilities
    if (std::isnan(pImpl->state.explorationDrive) || 
        std::isnan(pImpl->state.intrinsicMotivation)) {
        NLM_LOG_ERROR("CuriositySystem: Numerical instability detected");
        return false;
    }
    
    return true;
}

void CuriositySystem::enforceBounds() {
    pImpl->state.explorationDrive = std::clamp(pImpl->state.explorationDrive,
                                              pImpl->state.minExplorationDrive,
                                              pImpl->state.maxExplorationDrive);
    
    pImpl->state.intrinsicMotivation = std::clamp(pImpl->state.intrinsicMotivation,
                                                 pImpl->state.minIntrinsicMotivation,
                                                 pImpl->state.maxIntrinsicMotivation);
}

void CuriositySystem::exportState(std::ostream& stream) const {
    stream.write(reinterpret_cast<const char*>(&pImpl->state), sizeof(CuriosityState));
    stream.write(reinterpret_cast<const char*>(&pImpl->intrinsicMotivation), sizeof(IntrinsicMotivation));
    
    // Export histories (simplified)
    size_t noveltySize = pImpl->noveltyHistory.size();
    stream.write(reinterpret_cast<const char*>(&noveltySize), sizeof(size_t));
    if (noveltySize > 0) {
        stream.write(reinterpret_cast<const char*>(pImpl->noveltyHistory.data()),
                    noveltySize * sizeof(float));
    }
}

void CuriositySystem::importState(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&pImpl->state), sizeof(CuriosityState));
    stream.read(reinterpret_cast<char*>(&pImpl->intrinsicMotivation), sizeof(IntrinsicMotivation));
    
    // Import histories
    size_t noveltySize;
    stream.read(reinterpret_cast<char*>(&noveltySize), sizeof(size_t));
    pImpl->noveltyHistory.resize(noveltySize);
    if (noveltySize > 0) {
        stream.read(reinterpret_cast<char*>(pImpl->noveltyHistory.data()),
                   noveltySize * sizeof(float));
    }
}

float CuriositySystem::getExplorationEfficiency() const {
    if (pImpl->state.explorationDrive <= 0.0f) return 0.0f;
    
    // Simple efficiency metric: exploration drive normalized by potential maximum
    return pImpl->state.explorationDrive / pImpl->state.maxExplorationDrive;
}

float CuriositySystem::getIntrinsicMotivationEfficiency() const {
    float totalIntrinsic = pImpl->intrinsicMotivation.empiricalCuriosity +
                          pImpl->intrinsicMotivation.intrinsicPleasure +
                          pImpl->intrinsicMotivation.goalIrrelevance +
                          pImpl->intrinsicMotivation.efficiencyMotivation +
                          pImpl->intrinsicMotivation.flowStateMotivation;
    
    return totalIntrinsic / 5.0f;  // Average across components
}

float CuriositySystem::getNoveltyUtilization() const {
    if (pImpl->noveltyHistory.empty()) return 0.0f;
    
    float totalNovelty = std::accumulate(pImpl->noveltyHistory.begin(), pImpl->noveltyHistory.end(), 0.0f);
    float avgNovelty = totalNovelty / pImpl->noveltyHistory.size();
    
    return std::min(1.0f, avgNovelty / pImpl->params.noveltyThreshold * 2.0f);
}

float CuriositySystem::getIntrinsicMotivation() const {
    return pImpl->state.intrinsicMotivation;
}

float CuriositySystem::getExtrinsicMotivation() const {
    return pImpl->state.extrinsicMotivation;
}

float CuriositySystem::getExplorationDrive() const {
    return pImpl->state.explorationDrive;
}

float CuriositySystem::getCuriosityLevel() const {
    return pImpl->state.explorationDrive + pImpl->state.intrinsicMotivation;
}

float CuriositySystem::getExplorationBonus() const {
    return pImpl->state.explorationBonus;
}

float CuriositySystem::getUncertaintyEstimate() const {
    return pImpl->state.uncertaintyEstimate;
}

float CuriositySystem::getInformationGain() const {
    return pImpl->state.informationGain;
}

void CuriositySystem::shutdown() {
    if (pImpl) {
        reset();
        pImpl->brain.reset();
        pImpl->engine.reset();
    }
}

} // namespace nlm