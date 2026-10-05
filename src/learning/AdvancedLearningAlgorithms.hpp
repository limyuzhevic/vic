// Enhanced learning algorithms for NLM
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <chrono>

namespace nlm {

// Advanced learning algorithm flags
enum class AdvancedLearningFlags {
    None = 0,
    TraceBasedLearning = 1 << 0,        // Eligibility traces
    PredictionBasedLearning = 1 << 1,   // Prediction-based updates
    MetaLearning = 1 << 2,              // Learning to learn
    TransferLearning = 1 << 3,          // Knowledge transfer
    ContinualLearning = 1 << 4,         // Lifelong learning
    HierarchicalLearning = 1 << 5,      // Multi-level learning
    NeuromodulationBased = 1 << 6       // Neuromodulation-driven
};

// Trace-based learning for delayed reward propagation
class TraceBasedLearning {
public:
    TraceBasedLearning(float lambda = 0.9f, float alpha = 0.01f)
        : eligibilityTraceDecay_(lambda), learningRate_(alpha) {}
    
    void updateTrace(Synapse* synapse, float reward, float discount = 0.95f) {
        // Compute eligibility trace update
        float trace = eligibilityTraceDecay_ * synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(trace + 1.0f);
        
        // Delayed reward propagation
        float weightChange = learningRate_ * reward * trace;
        synapse->addToWeight(weightChange);
    }
    
    void resetTraces(float decay = 0.0f) {
        // Reset all eligibility traces
        for (auto& synapse : synapses_) {
            synapse->setEligibilityTrace(decay);
        }
    }
    
    void addSynapse(Synapse* synapse) {
        synapses_.push_back(synapse);
    }
    
private:
    std::vector<Synapse*> synapses_;
    float eligibilityTraceDecay_;
    float learningRate_;
};

// Prediction-based learning for forward modeling
class PredictionBasedLearning {
public:
    PredictionBasedLearning(float predictionWeight = 0.5f, float errorWeight = 0.5f)
        : predictionWeight_(predictionWeight), errorWeight_(errorWeight) {}
    
    // Forward model prediction
    float predictActionOutcome(const Brain& brain, const Action& action, 
                               double timestep) {
        // Use internal forward model to predict outcome
        // This is a simplified version - would involve predictive coding
        return action.value * predictionWeight_;
    }
    
    // Prediction error learning
    void learnFromPredictionError(const Brain& brain, float predictedReward,
                                 float actualReward, float error) {
        float learningSignal = errorWeight_ * error;
        
        // Update synaptic weights based on prediction error
        for (const auto& region : brain.getRegions()) {
            for (auto* synapse : region->getSynapses()) {
                // Reward-modulated STDP based on prediction error
                float delta = learningSignal * synapse->getEligibilityTrace() * 
                             (predictedReward - actualReward);
                synapse->addToWeight(delta);
            }
        }
    }
    
    // Learn internal forward model
    void learnForwardModel(const Brain& brain, const Action& action,
                          float outcome, float predictionError) {
        // Update internal forward model parameters
        modelAccuracy_ += predictionError;
    }
    
    float getModelAccuracy() const { return modelAccuracy_; }
    
private:
    float predictionWeight_;
    float errorWeight_;
    float modelAccuracy_;
};

// Meta-learning system for learning to learn
class MetaLearningSystem {
public:
    MetaLearningSystem(float plasticityFactor = 1.0f, float adaptationRate = 0.01f)
        : plasticityFactor_(plasticityFactor), adaptationRate_(adaptationRate) {}
    
    // Adapt learning rates based on performance
    void adaptLearningRate(const std::vector<float>& recentRewards) {
        if (recentRewards.empty()) return;
        
        // Calculate performance metrics
        float avgReward = std::accumulate(recentRewards.begin(), recentRewards.end(), 0.0f) / recentRewards.size();
        float rewardVariance = calculateVariance(recentRewards, avgReward);
        
        // Adapt learning rate based on performance
        float optimalLearningRate = calculateOptimalLearningRate(avgReward, rewardVariance);
        plasticityFactor_ = plasticityFactor_ + adaptationRate_ * (optimalLearningRate - plasticityFactor_);
        
        // Clamp to reasonable range
        plasticityFactor_ = std::clamp(plasticityFactor_, 0.1f, 5.0f);
    }
    
    // Get current meta-learning parameters
    float getPlasticityFactor() const { return plasticityFactor_; }
    void setPlasticityFactor(float factor) { plasticityFactor_ = std::clamp(factor, 0.1f, 5.0f); }
    
    // Record learning outcome for future adaptation
    void recordOutcome(float reward, float learningEfficiency) {
        recentOutcomes_.push_back({reward, learningEfficiency});
        
        // Maintain sliding window
        if (recentOutcomes_.size() > maxHistorySize_) {
            recentOutcomes_.erase(recentOutcomes_.begin());
        }
    }
    
    // Generate learning recommendations
    std::string getLearningRecommendations() const {
        if (recentOutcomes_.empty()) return "No data available";
        
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
    
private:
    float calculateVariance(const std::vector<float>& values, float mean) {
        float variance = 0.0f;
        for (float val : values) {
            variance += (val - mean) * (val - mean);
        }
        return variance / values.size();
    }
    
    float calculateOptimalLearningRate(float avgReward, float rewardVariance) {
        // Simple optimal learning rate calculation
        return 0.1f * (1.0f + std::min(avgReward / 10.0f, 1.0f)) /
               (1.0f + rewardVariance);
    }
    
    struct LearningOutcome {
        float reward;
        float efficiency;
    };
    
    std::vector<LearningOutcome> recentOutcomes_;
    float plasticityFactor_;
    float adaptationRate_;
    size_t maxHistorySize_ = 100;
};

// Transfer learning system for knowledge sharing
class TransferLearningSystem {
public:
    TransferLearningSystem(float transferStrength = 0.3f, 
                          float forgettingRate = 0.01f)
        : transferStrength_(transferStrength), forgettingRate_(forgettingRate) {}
    
    // Transfer knowledge from source brain to target brain
    void transferKnowledge(const Brain& source, Brain& target, 
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
    }
    
    // Create task-specific knowledge base
    void createKnowledgeBase(const Brain& brain, const std::string& taskType) {
        // Extract task-relevant features and patterns
        knowledgeBases_[taskType] = extractTaskFeatures(brain, taskType);
    }
    
    // Apply pre-learned knowledge to new task
    void applyKnowledge(const std::string& taskType, Brain& target) {
        if (knowledgeBases_.find(taskType) != knowledgeBases_.end()) {
            const auto& knowledge = knowledgeBases_[taskType];
            applyTaskKnowledge(knowledge, target);
        }
    }
    
    // Adapt transferred knowledge to new domain
    void adaptKnowledge(const std::string& sourceTask, const std::string& targetTask,
                       float adaptationRate = 0.1f) {
        if (knowledgeBases_.find(sourceTask) != knowledgeBases_.end() &&
            knowledgeBases_.find(targetTask) != knowledgeBases_.end()) {
            
            // Gradually adapt knowledge from source to target
            auto& targetKnowledge = knowledgeBases_[targetTask];
            const auto& sourceKnowledge = knowledgeBases_[sourceTask];
            
            // Implementation of adaptation logic
            adaptKnowledgeWeights(targetKnowledge, sourceKnowledge, adaptationRate);
        }
    }
    
private:
    void transferSynapticWeights(const NeuralRegion* sourceRegion, 
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
    
    bool shouldTransferSynapse(const Synapse* source, const Synapse* target) {
        // Simple matching based on neuron types and proximity
        return source->getTargetNeuron()->getType() == target->getTargetNeuron()->getType();
    }
    
    void transferConnectionPatterns(const NeuralRegion* sourceRegion, 
                                    NeuralRegion* targetRegion) {
        // Transfer connection patterns and topology
        // Implementation details would depend on specific requirements
    }
    
    // Extract task-relevant features from brain
    struct TaskKnowledge {
        std::vector<float> weightPatterns;
        std::vector<std::pair<RegionId, RegionId>> connections;
        std::vector<float> firingPatterns;
        float efficiencyScore;
    };
    
    TaskKnowledge extractTaskFeatures(const Brain& brain, const std::string& taskType) {
        TaskKnowledge knowledge;
        // Implementation would extract task-specific features
        return knowledge;
    }
    
    void applyTaskKnowledge(const TaskKnowledge& knowledge, Brain& target) {
        // Apply learned knowledge to target brain
    }
    
    void adaptKnowledgeWeights(TaskKnowledge& target, const TaskKnowledge& source, 
                              float rate) {
        // Adapt knowledge weights from source to target
    }
    
    std::unordered_map<std::string, TaskKnowledge> knowledgeBases_;
    float transferStrength_;
    float forgettingRate_;
};

// Continual learning system for lifelong adaptation
class ContinualLearningSystem {
public:
    ContinualLearningSystem(float plasticityChangeRate = 0.05f,
                           float forgettingThreshold = 0.1f)
        : plasticityChangeRate_(plasticityChangeRate),
          forgettingThreshold_(forgettingThreshold) {}
    
    // Schedule learning tasks based on plasticity and task relevance
    void scheduleLearningTask(Brain& brain, const std::string& taskType,
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
    }
    
    // Manage interference between tasks
    void manageTaskInterference(Brain& brain, const std::vector<std::string>& completedTasks,
                               const std::string& newTask) {
        // Calculate interference level
        float interference = calculateInterference(completedTasks, newTask);
        
        // Apply interference mitigation strategies
        if (interference > interferenceThreshold_) {
            applyInterferenceMitigation(brain, interference);
        }
    }
    
    // Generate learning schedule for long-term goals
    std::vector<std::pair<std::string, float>> generateLearningSchedule(
        const std::vector<std::string>& goals, float totalTime) {
        // Create hierarchical learning schedule
        std::vector<std::pair<std::string, float>> schedule;
        
        // Implementation would create time-based learning schedule
        return schedule;
    }
    
private:
    float calculateTaskDifficulty(const std::vector<float>& performance) {
        if (performance.empty()) return 0.5f;
        
        float mean = std::accumulate(performance.begin(), performance.end(), 0.0f) / performance.size();
        float variance = 0.0f;
        for (float val : performance) {
            variance += (val - mean) * (val - mean);
        }
        variance /= performance.size();
        
        return std::clamp(1.0f - std::sqrt(variance) / 10.0f, 0.1f, 1.0f);
    }
    
    float calculateTaskRelevance(const std::string& taskType) {
        // Map task types to relevance values
        static std::unordered_map<std::string, float> relevanceMap = {
            {"navigation", 0.8f},
            {"foraging", 0.9f},
            {"social", 0.7f},
            {"prediction", 0.6f},
            {"memory", 0.8f},
            {"problem_solving", 0.5f}
        };
        
        auto it = relevanceMap.find(taskType);
        return (it != relevanceMap.end()) ? it->second : 0.5f;
    }
    
    float calculateTargetPlasticity(float difficulty, float relevance) {
        // Combined plasticity target based on difficulty and relevance
        return (difficulty * 0.7f) + (relevance * 0.3f) * 3.0f;
    }
    
    void updateDevelopmentStage(Brain& brain, float difficulty, float relevance) {
        // Update developmental stage based on task characteristics
        DevelopmentalStage currentStage = brain.getDevelopmentalStage();
        
        // Simple stage progression based on challenge
        if (difficulty > 0.7f && relevance > 0.7f) {
            if (currentStage != DevelopmentalStage::Maturation) {
                brain.setDevelopmentalStage(DevelopmentalStage::Maturation);
            }
        }
    }
    
    void scheduleConsolidation(Brain& brain) {
        // Schedule memory consolidation
        // This would integrate with the existing replay system
    }
    
    float calculateInterference(const std::vector<std::string>& tasks, 
                               const std::string& newTask) {
        // Calculate interference based on task similarity
        float interference = 0.0f;
        
        for (const auto& task : tasks) {
            float similarity = calculateTaskSimilarity(task, newTask);
            interference += similarity;
        }
        
        return std::min(interference / tasks.size(), 1.0f);
    }
    
    float calculateTaskSimilarity(const std::string& task1, const std::string& task2) {
        // Simple similarity based on task type
        return (task1 == task2) ? 1.0f : 0.5f;
    }
    
    void applyInterferenceMitigation(Brain& brain, float interference) {
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
    }
    
    float interferenceThreshold_ = 0.6f;
    float plasticityChangeRate_;
    float forgettingThreshold_;
};

} // namespace nlm
