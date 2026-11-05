// CognitiveModules.cpp - Cognitive module system implementation

#include "CognitiveModules.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

void CognitiveModules::SelfModel::initialize(std::shared_ptr<Brain> b) {
    brain = b;
    initialized = true;
    NLM_LOG_INFO("Self-model initialized");
}

void CognitiveModules::SelfModel::record(ActionType action,
                                        const std::vector<float>& beforeVision,
                                        const std::vector<float>& afterVision) {
    ActionRecord record;
    record.action = action;
    record.beforeVision = beforeVision;
    record.afterVision = afterVision;
    record.confidence = 0.0f; // TODO: Calculate actual confidence
    
    records.push_back(record);
}

float CognitiveModules::SelfModel::getConfidence(ActionType action) const {
    float total = 0.0f;
    int count = 0;
    
    for (const auto& record : records) {
        if (record.action == action) {
            total += record.confidence;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0.0f;
}

float CognitiveModules::SelfModel::getOverallConfidence() const {
    if (records.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& record : records) {
        total += record.confidence;
    }
    
    return total / records.size();
}

ActionType CognitiveModules::SelfModel::getMostConfidentAction() const {
    ActionType bestAction = ActionType::Wait;
    float bestConfidence = 0.0f;
    
    std::unordered_map<ActionType, float> confidences;
    for (const auto& record : records) {
        confidences[record.action] += record.confidence;
    }
    
    for (const auto& pair : confidences) {
        if (pair.second > bestConfidence) {
            bestConfidence = pair.second;
            bestAction = pair.first;
        }
    }
    
    return bestAction;
}

float CognitiveModules::SelfModel::getConfidenceGap() const {
    if (records.size() < 2) return 0.0f;
    
    float maxConfidence = 0.0f;
    for (const auto& record : records) {
        maxConfidence = std::max(maxConfidence, record.confidence);
    }
    
    float minConfidence = std::numeric_limits<float>::max();
    for (const auto& record : records) {
        minConfidence = std::min(minConfidence, record.confidence);
    }
    
    return maxConfidence - minConfidence;
}

void CognitiveModules::SocialLearning::initialize(std::shared_ptr<Brain> b) {
    brain = b;
    initialized = true;
    NLM_LOG_INFO("Social learning system initialized");
}

void CognitiveModules::SocialLearning::addObservation(ActionType action,
                                                    const std::vector<float>& beforeVision,
                                                    const std::vector<float>& afterVision) {
    Observation obs;
    obs.action = action;
    obs.beforeVision = beforeVision;
    obs.afterVision = afterVision;
    obs.timestamp = 0.0; // TODO: Get actual timestamp
    
    observations.push_back(obs);
}

std::pair<ActionType, float> CognitiveModules::SocialLearning::getImitationActionAndSimilarity(const std::vector<float>& currentVision) const {
    if (observations.empty()) {
        return {ActionType::Wait, 0.0f};
    }
    
    ActionType bestAction = ActionType::Wait;
    float bestSimilarity = 0.0f;
    
    for (const auto& obs : observations) {
        float similarity = calculateSimilarity(obs.beforeVision, currentVision);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestAction = obs.action;
        }
    }
    
    return {bestAction, bestSimilarity};
}

float CognitiveModules::SocialLearning::calculateSimilarity(const std::vector<float>& vision1,
                                                         const std::vector<float>& vision2) const {
    if (vision1.size() != vision2.size()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision1.size(); ++i) {
        totalDiff += std::abs(vision1[i] - vision2[i]);
    }
    
    return 1.0f - (totalDiff / vision1.size());
}

void CognitiveModules::ConceptSystem::initialize(std::shared_ptr<Brain> b) {
    brain = b;
    NLM_LOG_INFO("Concept formation system initialized");
}

size_t CognitiveModules::ConceptSystem::getMatchingConcept(const std::vector<float>& pattern) const {
    for (size_t i = 0; i < concepts.size(); ++i) {
        if (calculatePatternSimilarity(concepts[i].pattern, pattern) > config_.conceptPatternThreshold) {
            return i;
        }
    }
    return 0; // No matching concept found
}

float CognitiveModules::ConceptSystem::calculatePatternSimilarity(const std::vector<float>& pattern1,
                                                                const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        totalDiff += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return 1.0f - (totalDiff / pattern1.size());
}

ActionType CognitiveModules::planAction(const std::vector<float>& vision,
                                       const std::shared_ptr<Brain>& brain,
                                       float noveltyLevel,
                                       float predictionError) {
    if (!brain || !brain->getPlanner()) {
        return ActionType::Wait;
    }
    
    return brain->getPlanner()->planAction(vision);
}

void CognitiveModules::presentExperience(const std::vector<float>& pattern,
                                        const std::vector<float>& touch,
                                        float reward,
                                        const std::shared_ptr<Brain>& brain) {
    if (!brain || !brain->getConceptFormation()) {
        return;
    }
    
    conceptSystem_.presentExperience(pattern, touch, reward);
}

size_t CognitiveModules::getConceptCount(const std::shared_ptr<Brain>& brain) const {
    if (!brain || !brain->getConceptFormation()) {
        return 0;
    }
    
    return brain->getConceptFormation()->getConceptCount();
}

void CognitiveModules::recordSelfAction(ActionType action,
                                       const std::vector<float>& beforeVision,
                                       const std::vector<float>& afterVision) {
    selfModel_.record(action, beforeVision, afterVision);
}

float CognitiveModules::getSelfModelConfidence(ActionType action) const {
    return selfModel_.getConfidence(action);
}

float CognitiveModules::getSelfModelConfidence() const {
    return selfModel_.getOverallConfidence();
}

void CognitiveModules::observeAgentAction(ActionType action,
                                        const std::vector<float>& beforeVision,
                                        const std::vector<float>& afterVision) {
    socialLearning_.addObservation(action, beforeVision, afterVision);
}

size_t CognitiveModules::getObservationCount() const {
    return socialLearning_.getObservationCount();
}

std::pair<ActionType, float> CognitiveModules::getImitationActionAndSimilarity(const std::vector<float>& currentVision) const {
    return socialLearning_.getImitationActionAndSimilarity(currentVision);
}

void CognitiveModules::updateDevelopment(AgentBrain& agent, double timestep) {
    agent.updateDevelopment(timestep);
}

DevelopmentalStage CognitiveModules::getDevelopmentalStage(const std::shared_ptr<Brain>& brain) const {
    if (!brain) return DevelopmentalStage::Initial;
    return brain->getDevelopmentalStage();
}

} // namespace nlm
