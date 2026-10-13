// NeuralSelfModel.cpp
#include "NeuralSelfModel.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralSelfModel::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralSelfModel::NeuralSelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , noveltyWeight_(1.0f)
    , curiosityWeight_(0.8f)
    , predictionErrorWeight_(0.6f)
    , rewardWeight_(1.0f)
{
    planningConfidenceCache_.resize(10, 0.5f);  // 10 action types
}

NeuralSelfModel::~NeuralSelfModel() = default;

void NeuralSelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSelfModel initialized");
}

void NeuralSelfModel::updateWithPlanning(const std::vector<float>& planningContext,
                                       const std::vector<ActionType>& plannedActions,
                                       float planningConfidence) {
    if (!brain_) return;
    
    // Update body schema with planning context
    if (!planningContext.empty()) {
        for (size_t i = 0; i < std::min(planningContext.size(), bodySchema_.size()); ++i) {
            // Hebbian-like update: strengthen connections for planned actions
            bodySchema_[i] = bodySchema_[i] * 0.9f + planningContext[i] * 0.1f * planningConfidence;
        }
    }
    
    // Update planning confidence cache
    for (const auto& action : plannedActions) {
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < planningConfidenceCache_.size()) {
            planningConfidenceCache_[actionIdx] = 
                planningConfidenceCache_[actionIdx] * 0.9f + planningConfidence * 0.1f;
        }
    }
}

void NeuralSelfModel::updateWithConcept(const DiscoveredConcept& concept,
                                      const std::vector<float>& contextualInfo,
                                      float noveltySignal) {
    if (!brain_ || concept.id == 0) return;
    
    // Integrate concept prototype into body schema
    const std::vector<float>& prototype = concept.prototype;
    if (!prototype.empty() && !bodySchema_.empty()) {
        float conceptWeight = concept.avgStability * (1.0f + noveltySignal * 0.5f);
        
        // Update body schema with concept information
        for (size_t i = 0; i < std::min(prototype.size(), bodySchema_.size()); ++i) {
            // Novelty modulates learning rate
            float learningRate = 0.2f * (1.0f + noveltySignal * 0.3f);
            bodySchema_[i] = bodySchema_[i] * (1.0f - learningRate) + prototype[i] * learningRate;
        }
    }
    
    // Update planning confidence based on concept stability
    for (size_t i = 0; i < planningConfidenceCache_.size(); ++i) {
        planningConfidenceCache_[i] *= concept.avgStability;
    }
}

std::vector<float> NeuralSelfModel::predictActionConsequence(ActionType action,
                                                          const std::vector<float>& currentState,
                                                          const std::vector<float>& planningContext) const {
    std::vector<float> predictedState = currentState;
    
    if (bodySchema_.empty()) return predictedState;
    
    // Apply body schema modulation to action prediction
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= planningConfidenceCache_.size()) return predictedState;
    
    float actionConfidence = planningConfidenceCache_[actionIdx];
    
    // Body schema influences prediction: higher body schema activation means 
    // stronger, more reliable predictions
    for (size_t i = 0; i < std::min(predictedState.size(), bodySchema_.size()); ++i) {
        float schemaActivation = bodySchema_[i];
        float predictionStrength = actionConfidence * schemaActivation;
        
        // Modify prediction based on body schema
        predictedState[i] = predictedState[i] * (1.0f - predictionStrength) +
                          schemaActivation * predictionStrength;
    }
    
    return predictedState;
}

float NeuralSelfModel::getPlanningConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= planningConfidenceCache_.size()) return 0.5f;
    
    return planningConfidenceCache_[actionIdx];
}

float NeuralSelfModel::getSelfCapabilityLevel(const std::vector<float>& planningContext) const {
    if (bodySchema_.empty()) return 0.5f;
    
    // Compute capability level as average body schema activation
    float totalActivation = std::accumulate(bodySchema_.begin(), bodySchema_.end(), 0.0f);
    return totalActivation / bodySchema_.size();
}

float NeuralSelfModel::getActionPlanningConfidence(ActionType action) const {
    // Combine body schema confidence with planning confidence cache
    float baseConfidence = getPlanningConfidence(action);
    
    if (bodySchema_.empty()) return baseConfidence;
    
    // Higher body schema activation increases confidence
    float bodySchemaInfluence = 0.0f;
    for (float activation : bodySchema_) {
        bodySchemaInfluence += activation;
    }
    bodySchemaInfluence /= bodySchema_.size();
    
    return std::min(1.0f, baseConfidence * (1.0f + bodySchemaInfluence * 0.5f));
}

ActionType NeuralSelfModel::getPreferredAction(const std::vector<float>& state,
                                             const std::vector<float>& planningContext) const {
    ActionType bestAction = ActionType::Wait;
    float bestValue = -1000.0f;
    
    // Get self-model confidence for each action
    for (size_t i = 0; i < planningConfidenceCache_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Check if this action has body schema influence
        float bodySchemaInfluence = 0.0f;
        if (!bodySchema_.empty() && i < bodySchema_.size()) {
            bodySchemaInfluence = bodySchema_[i];
        }
        
        // Compute action value based on confidence and body schema
        float actionValue = planningConfidenceCache_[i] * (1.0f + bodySchemaInfluence * 0.5f);
        
        if (actionValue > bestValue) {
            bestValue = actionValue;
            bestAction = action;
        }
    }
    
    return bestAction;
}

void NeuralSelfModel::applyNeuromodulation(float noveltyLevel, float curiosityLevel,
                                          float predictionError, float rewardPrediction) {
    // Update neuromodulation weights based on current signals
    noveltyWeight_ = std::min(2.0f, noveltyWeight_ * (1.0f + noveltyLevel * 0.1f));
    curiosityWeight_ = std::min(2.0f, curiosityLevel * 0.5f);
    predictionErrorWeight_ = std::min(2.0f, predictionError * 0.5f);
    rewardWeight_ = std::min(2.0f, rewardPrediction * 0.5f);
    
    // Apply neuromodulation effects on body schema
    if (!bodySchema_.empty()) {
        // Novelty and curiosity enhance body schema learning
        float cognitivePlasticity = 1.0f + 
            (noveltyWeight_ - 1.0f) * 0.2f +
            (curiosityWeight_ - 1.0f) * 0.3f;
        
        // Reward prediction error stabilizes body schema
        float adaptationRate = 0.1f * cognitivePlasticity;
        float stabilizationFactor = 1.0f - predictionErrorWeight_ * 0.1f;
        
        for (auto& activation : bodySchema_) {
            activation = activation * stabilizationFactor + 
                       (1.0f - stabilizationFactor) * 0.5f * adaptationRate;
            activation = std::clamp(activation, 0.0f, 1.0f);
        }
    }
}

void NeuralSelfModel::clear() {
    bodySchema_.clear();
    bodySchema_.resize(10, 0.5f);  // Default 10-dimensional body schema
    planningConfidenceCache_.assign(10, 0.5f);
    
    noveltyWeight_ = 1.0f;
    curiosityWeight_ = 0.8f;
    predictionErrorWeight_ = 0.6f;
    rewardWeight_ = 1.0f;
}

} // namespace nlm