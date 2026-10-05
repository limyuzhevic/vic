#include "AdvancedLearningAlgorithms.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>

namespace nlm {

// TraceBasedLearning implementation
void TraceBasedLearning::updateTrace(Synapse* synapse, float reward, float discount) {
    if (!synapse) return;
    
    // Compute eligibility trace update
    float trace = eligibilityTraceDecay_ * synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(trace + 1.0f);
    
    // Delayed reward propagation
    float weightChange = learningRate_ * reward * trace;
    synapse->addToWeight(weightChange);
    
    NLM_LOG_DEBUG("Trace-based learning: synapse weight change = " + std::to_string(weightChange));
}

void TraceBasedLearning::resetTraces(float decay) {
    // Reset all eligibility traces
    for (auto* synapse : synapses_) {
        if (synapse) {
            synapse->setEligibilityTrace(decay);
        }
    }
    NLM_LOG_DEBUG("Reset all eligibility traces to " + std::to_string(decay));
}

// PredictionBasedLearning implementation
float PredictionBasedLearning::predictActionOutcome(const Brain& brain, 
                                                   const Action& action, 
                                                   double timestep) {
    // Use internal forward model to predict outcome
    // This is a simplified version - would involve predictive coding
    return action.value * predictionWeight_;
}

void PredictionBasedLearning::learnFromPredictionError(const Brain& brain, 
                                                      float predictedReward,
                                                      float actualReward,
                                                      float error) {
    float learningSignal = errorWeight_ * error;
    
    // Update synaptic weights based on prediction error
    for (const auto& region : brain.getRegions()) {
        for (auto* synapse : region->getSynapses()) {
            if (synapse) {
                // Reward-modulated STDP based on prediction error
                float delta = learningSignal * synapse->getEligibilityTrace() * 
                             (predictedReward - actualReward);
                synapse->addToWeight(delta);
            }
        }
    }
    
    NLM_LOG_DEBUG("Prediction-based learning: error = " + std::to_string(error) + 
                 ", learning signal = " + std::to_string(learningSignal));
}

// MetaLearningSystem implementation
void MetaLearningSystem::adaptLearningRate(const std::vector<float>& recentRewards) {
    if (recentRewards.empty()) {
        return;
    }
    
    // Calculate performance metrics
    float avgReward = std::accumulate(recentRewards.begin(), recentRewards.end(), 0.0f) / recentRewards.size();
    float rewardVariance = calculateVariance(recentRewards, avgReward);
    
    // Adapt learning rate based on performance
    float optimalLearningRate = calculateOptimalLearningRate(avgReward, rewardVariance);
    plasticityFactor_ = plasticityFactor_ + adaptationRate_ * (optimalLearningRate - plasticityFactor_);
    
    // Clamp to reasonable range
    plasticityFactor_ = std::clamp(plasticityFactor_, 0.1f, 5.0f);
    
    NLM_LOG_DEBUG("Meta-learning adapted plasticity factor to " + std::to_string(plasticityFactor_));
}

void MetaLearningSystem::recordOutcome(float reward, float learningEfficiency) {
    recentOutcomes_.push_back({reward, learningEfficiency});
    
    // Maintain sliding window
    if (recentOutcomes_.size() > maxHistorySize_) {
        recentOutcomes_.erase(recentOutcomes_.begin());
    }
}

std::string MetaLearningSystem::getLearningRecommendations() const {
    if (recentOutcomes_.empty()) {
        return "No data available";
    }
    
    float avgEfficiency = 0.0f;
    for (const auto& outcome : recentOutcomes_) {
        avgEfficiency += outcome.efficiency;
    }
    avgEfficiency /= recentOutcomes_.size();
    
    if (avgEfficiency > 0.8f) {
        return "Learning is efficient. Consider increasing exploration.";
    } else if (avgEfficiency < 0.3f) {
        return "Learning is inefficient. Consider reducing plasticity or increasing rewards.";
    } else {
        return "Learning is at optimal level.";
    }
}

// TransferLearningSystem implementation
void TransferLearningSystem::transferKnowledge(const Brain& source, Brain& target,
                                             const std::vector<RegionId>& sourceRegions,
                                             const std::vector<RegionId>& targetRegions) {
    for (size_t i = 0; i < std::min(sourceRegions.size(), targetRegions.size()); ++i) {
        RegionId sourceRegion = sourceRegions[i];
        RegionId targetRegion = targetRegions[i];
        
        const NeuralRegion* srcRegion = source.getRegion(sourceRegion);
        NeuralRegion* tgtRegion = target.getRegion(targetRegion);
        
        if (srcRegion && tgtRegion) {
            // Transfer synaptic weights and connectivity patterns
            transferSynapticWeights(srcRegion, tgtRegion);
            transferConnectionPatterns(srcRegion, tgtRegion);
        }
    }
    
    NLM_LOG_DEBUG("Transferred knowledge from " + std::to_string(sourceRegions.size()) +
                 " source regions to " + std::to_string(targetRegions.size()) + " target regions");
}

void TransferLearningSystem::createKnowledgeBase(const Brain& brain, 
                                                const std::string& taskType) {
    // Extract task-relevant features and patterns
    knowledgeBases_[taskType] = extractTaskFeatures(brain, taskType);
    
    NLM_LOG_DEBUG("Created knowledge base for task type: " + taskType);
}

void TransferLearningSystem::applyKnowledge(const std::string& taskType, Brain& target) {
    if (knowledgeBases_.find(taskType) != knowledgeBases_.end()) {
        const auto& knowledge = knowledgeBases_[taskType];
        applyTaskKnowledge(knowledge, target);
        NLM_LOG_DEBUG("Applied knowledge for task type: " + taskType);
    }
}

void TransferLearningSystem::adaptKnowledge(const std::string& sourceTask,
                                          const std::string& targetTask,
                                          float adaptationRate) {
    if (knowledgeBases_.find(sourceTask) != knowledgeBases_.end() &&
        knowledgeBases_.find(targetTask) != knowledgeBases_.end()) {
        
        auto& targetKnowledge = knowledgeBases_[targetTask];
        const auto& sourceKnowledge = knowledgeBases_[sourceTask];
        
        adaptKnowledgeWeights(targetKnowledge, sourceKnowledge, adaptationRate);
        NLM_LOG_DEBUG("Adapted knowledge from " + sourceTask + " to " + targetTask);
    }
}

void TransferLearningSystem::transferSynapticWeights(const NeuralRegion* sourceRegion,
                                                    NeuralRegion* targetRegion) {
    // Transfer synaptic weights with scaling
    for (auto* sourceSynapse : sourceRegion->getSynapses()) {
        for (auto* targetSynapse : targetRegion->getSynapses()) {
            // Match synapses based on location, type, or other criteria
            if (shouldTransferSynapse(sourceSynapse, targetSynapse)) {
                float transferredWeight = sourceSynapse->getWeight() * transferStrength_;
                targetSynapse->setWeight(transferredWeight);
            }
        }
    }
}

bool TransferLearningSystem::shouldTransferSynapse(const Synapse* source, 
                                                  const Synapse* target) {
    // Simple matching based on neuron types and proximity
    return source->getTargetNeuron()->getType() == target->getTargetNeuron()->getType();
}

// ContinualLearningSystem implementation
void ContinualLearningSystem::scheduleLearningTask(Brain& brain,
                                                 const std::string& taskType,
                                                 const std::vector<float>& taskPerformance) {
    // Calculate task difficulty and relevance
    float difficulty = calculateTaskDifficulty(taskPerformance);
    float relevance = calculateTaskRelevance(taskType);
    
    // Adjust brain plasticity based on task characteristics
    float currentPlasticity = brain.getConfig()->getOr<float>("current_plasticity", 1.0f);
    float targetPlasticity = calculateTargetPlasticity(difficulty, relevance);
    
    float newPlasticity = currentPlasticity + 
                       plasticityChangeRate_ * (targetPlasticity - currentPlasticity);
    
    // Apply plasticity adjustment
    brain.getConfig()->set("current_plasticity", newPlasticity);
    
    // Update development stage if needed
    updateDevelopmentStage(brain, difficulty, relevance);
    
    // Schedule consolidation if plasticity is high
    if (newPlasticity > 2.0f) {
        scheduleConsolidation(brain);
    }
    
    NLM_LOG_DEBUG("Scheduled learning task: " + taskType + ", difficulty: " + 
                 std::to_string(difficulty) + ", relevance: " + std::to_string(relevance) +
                 ", new plasticity: " + std::to_string(newPlasticity));
}

void ContinualLearningSystem::manageTaskInterference(Brain& brain,
                                                     const std::vector<std::string>& completedTasks,
                                                     const std::string& newTask) {
    // Calculate interference level
    float interference = calculateInterference(completedTasks, newTask);
    
    // Apply interference mitigation strategies
    if (interference > interferenceThreshold_) {
        applyInterferenceMitigation(brain, interference);
    }
    
    NLM_LOG_DEBUG("Managed task interference for new task: " + newTask + 
                 ", interference level: " + std::to_string(interference));
}

std::vector<std::pair<std::string, float>> ContinualLearningSystem::generateLearningSchedule(
    const std::vector<std::string>& goals, float totalTime) {
    // Create hierarchical learning schedule
    std::vector<std::pair<std::string, float>> schedule;
    
    // Simple implementation: distribute time evenly across goals
    float timePerGoal = totalTime / goals.size();
    for (size_t i = 0; i < goals.size(); ++i) {
        schedule.push_back({goals[i], timePerGoal});
    }
    
    NLM_LOG_DEBUG("Generated learning schedule with " + std::to_string(schedule.size()) + " tasks");
    return schedule;
}

void ContinualLearningSystem::updateDevelopmentStage(Brain& brain, 
                                                     float difficulty, 
                                                     float relevance) {
    // Update developmental stage based on task characteristics
    DevelopmentalStage currentStage = brain.getDevelopmentalStage();
    
    // Simple stage progression based on challenge
    if (difficulty > 0.7f && relevance > 0.7f) {
        if (currentStage != DevelopmentalStage::Maturation) {
            brain.setDevelopmentalStage(DevelopmentalStage::Maturation);
            NLM_LOG_DEBUG("Updated developmental stage to Maturation due to high challenge");
        }
    }
}

void ContinualLearningSystem::scheduleConsolidation(Brain& brain) {
    // Schedule memory consolidation
    // This would integrate with the existing replay system
    NLM_LOG_DEBUG("Scheduled memory consolidation");
}

void ContinualLearningSystem::applyInterferenceMitigation(Brain& brain, float interference) {
    // Apply strategies to reduce catastrophic interference
    // Such as elastic weight consolidation or regularization
    
    // Increase regularization strength
    float regularization = brain.getConfig()->getOr<float>("regularization_strength", 0.01f);
    regularization += interference * 0.05f;
    brain.getConfig()->set("regularization_strength", std::min(regularization, 0.5f));
    
    // Adjust learning rate
    float learningRate = brain.getConfig()->getOr<float>("learning_rate", 0.01f);
    learningRate *= (1.0f - interference * 0.3f);
    brain.getConfig()->set("learning_rate", std::max(learningRate, 0.001f));
    
    NLM_LOG_DEBUG("Applied interference mitigation with interference level: " + std::to_string(interference));
}

} // namespace nlm
