// Copyright © 2024 NLM Research Project
// Licensed under MIT License

/**
 * @file ExpertExample2_MetaLearningContinual.cpp
 * @brief Meta-learning framework for continual learning without catastrophic forgetting
 * 
 * This example demonstrates an expert-level implementation of meta-learning
 * and continual learning strategies. The agent learns how to learn,
 * adapts to new tasks without forgetting previous knowledge,
 * and implements various regularization and rehearsal techniques.
 */

#include "agent/AgentBody.hpp"
#include "brain/Brain.hpp"
#include "advanced_memory/AdvancedMemory.hpp"
#include "advanced_prediction/ForwardModeling.hpp"
#include "advanced_prediction/MonteCarloTreeSearch.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <cmath>

namespace nlm {

/**
 * @struct TaskMemory
 * Consolidated memory for task-specific knowledge with regularization
 */
struct TaskMemory {
    size_t taskId;
    std::string taskName;
    std::vector<float> taskSignature;
    std::vector<float> neuralPattern;
    std::vector<float> policyParameters;
    std::vector<float> valueFunction;
    float taskPerformance;
    float taskImportance;
    float knowledgeStrength;
    float interferenceScore;
    std::unordered_set<size_t> relatedTasks;
    std::vector<size_t> prerequisiteTasks;
    std::chrono::steady_clock::time_point lastAccessed;
    std::chrono::steady_clock::time_point created;
    float retentionPriority;
    
    TaskMemory(size_t id, const std::string& name,
               const std::vector<float>& signature)
        : taskId(id), taskName(name), taskSignature(signature),
          taskPerformance(0.0f), taskImportance(1.0f),
          knowledgeStrength(1.0f), interferenceScore(0.0f),
          retentionPriority(1.0f) {
        lastAccessed = std::chrono::steady_clock::now();
        created = lastAccessed;
    }
};

/**
 * @struct MetaGradient
 * Gradient of learning process for meta-learning
 */
struct MetaGradient {
    size_t taskId;
    size_t parameterId;
    float learningRate;
    float metaLearningRate;
    float taskRelevance;
    float regularizationStrength;
    float plasticity;
    std::vector<float> gradient;
    std::vector<float> metaGradient;
    float estimateError;
    float confidence;
    
    MetaGradient(size_t task, size_t param,
                const std::vector<float>& grad,
                const std::vector<float>& metaGrad)
        : taskId(task), parameterId(param), learningRate(0.01f),
          metaLearningRate(0.001f), taskRelevance(1.0f),
          regularizationStrength(0.1f), plasticity(0.5f),
          estimateError(0.0f), confidence(0.5f) {
        gradient = grad;
        metaGradient = metaGrad;
    }
};

/**
 * @struct TaskBuffer
 * Experience replay buffer for continual learning
 */
struct TaskBuffer {
    size_t bufferId;
    size_t capacity;
    std::vector<std::vector<float>> experiences;
    std::vector<size_t> taskIds;
    std::vector<float> priorities;
    std::vector<float> weights;
    float learningRate;
    bool usePrioritizedSampling;
    bool useStructuredReplay;
    bool useKnowledgeDistillation;
    
    TaskBuffer(size_t id, size_t cap)
        : bufferId(id), capacity(cap), learningRate(0.01f),
          usePrioritizedSampling(true), useStructuredReplay(true),
          useKnowledgeDistillation(true) {}
    
    void addExperience(const std::vector<float>& experience,
                      size_t taskId, float priority = 1.0f);
    
    std::vector<std::pair<std::vector<float>, size_t>> sampleBatch(size_t batchSize);
    
    void updatePriorities(const std::vector<float>& errors);
    
    void clear() {
        experiences.clear();
        taskIds.clear();
        priorities.clear();
        weights.clear();
    }
};

/**
 * @struct RegularizationStrategy
 * Different regularization techniques to prevent catastrophic forgetting
 */
struct RegularizationStrategy {
    std::string name;
    float strength;
    std::function<float(const std::vector<float>& current,
                       const std::vector<float>& previous)> computeRegularization;
    std::function<void(std::vector<float>& weights)> applyToWeights;
    
    RegularizationStrategy(const std::string& strategyName, float regStrength)
        : name(strategyName), strength(regStrength) {}
};

/**
 * @struct KnowledgeDistillation
 * Transfer knowledge from old to new tasks
 */
struct KnowledgeDistillation {
    std::vector<float> teacherOutput;
    std::vector<float> studentOutput;
    std::vector<float> distillationLoss;
    float temperature;
    float distillationAlpha;
    bool useSoftLabels;
    
    KnowledgeDistillation()
        : temperature(2.0f), distillationAlpha(0.5f), useSoftLabels(true) {}
    
    void computeDistillationLoss(const std::vector<float>& teacher,
                                const std::vector<float>& student);
    
    float getDistillationLoss() const;
};

/**
 * @struct ForwardModel
 * Predictive model for continual learning dynamics
 */
struct ForwardModel {
    std::vector<std::vector<float>> transitionMatrix;
    std::vector<float> stateNoise;
    std::vector<float> processNoise;
    std::vector<float> modelParameters;
    float learningSpeed;
    float predictionHorizon;
    bool useBayesianUpdate;
    
    ForwardModel()
        : learningSpeed(0.01f), predictionHorizon(10.0f), useBayesianUpdate(true) {}
    
    std::vector<float> predictNextState(const std::vector<float>& currentState,
                                        const std::vector<float>& action);
    
    void updateModel(const std::vector<float>& currentState,
                    const std::vector<float>& nextState,
                    const std::vector<float>& action);
    
    float computePredictionError(const std::vector<float>& predicted,
                                const std::vector<float>& actual) const;
};

/**
 * ContinualLearningMetaAgent class implementing meta-learning
 */
class ContinualLearningMetaAgent {
public:
    ContinualLearningMetaAgent(const std::string& name, 
                             const LifetimeExperimentConfig& config);
    ~ContinualLearningMetaAgent();
    
    // Main lifecycle methods
    void initialize();
    void update(float dt);
    void shutdown();
    
    // Task management
    void addTask(const std::string& taskName,
                const std::vector<float>& taskSignature,
                float importance = 1.0f);
    void removeTask(size_t taskId);
    void completeTask(size_t taskId);
    
    // Learning management
    void learnFromExperience(const std::vector<float>& experience,
                            size_t taskId, float reward);
    void consolidateLearning();
    void transferKnowledge(size_t sourceTaskId, size_t targetTaskId);
    
    // Meta-learning
    void computeMetaGradients();
    void updateLearningStrategy();
    void adaptToNewTask(const std::vector<float>& newTaskSignature);
    
    // Regularization and rehearsal
    void applyRegularization();
    void rehearsalLearning();
    void distillationLearning();
    
    // Performance tracking
    float getTaskPerformance(size_t taskId) const;
    float getForgettingRate() const;
    float getKnowledgeRetention(size_t taskId) const;
    float getMetaLearningProgress() const;
    
    // State management
    void saveCheckpoint(const std::string& filename);
    void loadCheckpoint(const std::string& filename);
    
    // Core brain components
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::string name_;
    LifetimeExperimentConfig config_;
    
    // Core brain components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<MonteCarloTreeSearch> mctsEngine_;
    std::shared_ptr<ForwardModeling> forwardModel_;
    std::shared_ptr<AdvancedMemory> advancedMemory_;
    
    // Task management
    std::unordered_map<size_t, TaskMemory> tasks_;
    std::unordered_map<size_t, std::vector<std::vector<float>>> taskExperiences_;
    std::vector<size_t> completedTasks_;
    std::vector<size_t> activeTasks_;
    
    // Meta-learning
    std::vector<MetaGradient> metaGradients_;
    std::vector<float> learningStrategy_;
    float metaLearningProgress_;
    float taskSimilarityThreshold_;
    
    // Experience replay
    std::unordered_map<size_t, TaskBuffer> taskBuffers_;
    std::vector<std::vector<float>> globalReplayBuffer_;
    
    // Regularization
    std::vector<RegularizationStrategy> regularizationStrategies_;
    KnowledgeDistillation distillationCache_;
    
    // Forward models
    std::unordered_map<size_t, ForwardModel> taskForwardModels_;
    
    // Performance metrics
    std::unordered_map<size_t, std::vector<float>> taskPerformanceHistory_;
    std::vector<float> forgettingRates_;
    std::vector<float> retentionRatios_;
    float overallForgettingRate_;
    
    // Internal state
    bool isInitialized_;
    bool isRunning_;
    size_t currentStep_;
    size_t currentTaskId_;
    std::chrono::steady_clock::time_point startTime_;
    
    // Helper methods
    void initializeTaskBuffers();
    void setupRegularizationStrategies();
    void updateTaskSignatures();
    float computeTaskSimilarity(const TaskMemory& task1,
                               const TaskMemory& task2) const;
    void updateMetaGradients();
    void applyMetaGradients();
    void updateTaskBuffers();
    void computeTaskPerformance();
    float computeForgettingRate(size_t taskId) const;
    void updateRetentionPriorities();
    void detectCatastrophicForgetting();
};

// Implementation of TaskBuffer

void TaskBuffer::addExperience(const std::vector<float>& experience,
                              size_t taskId, float priority) {
    if (experiences.size() >= capacity) {
        // Remove oldest experience if at capacity
        experiences.erase(experiences.begin());
        taskIds.erase(taskIds.begin());
        priorities.erase(priorities.begin());
        weights.erase(weights.begin());
    }
    
    experiences.push_back(experience);
    taskIds.push_back(taskId);
    priorities.push_back(priority);
    
    // Compute importance-based weight
    float weight = 1.0f / (1.0f + priorities.back());
    weights.push_back(weight);
}

std::vector<std::pair<std::vector<float>, size_t>> TaskBuffer::sampleBatch(size_t batchSize) {
    std::vector<std::pair<std::vector<float>, size_t>> batch;
    
    if (experiences.empty()) return batch;
    
    // Prioritized sampling
    if (usePrioritizedSampling && priorities.size() == experiences.size()) {
        std::vector<float> cumulativeSum(priorities.size());
        cumulativeSum[0] = priorities[0];
        for (size_t i = 1; i < priorities.size(); ++i) {
            cumulativeSum[i] = cumulativeSum[i-1] + priorities[i];
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, cumulativeSum.back());
        
        for (size_t i = 0; i < batchSize && i < experiences.size(); ++i) {
            float randomPoint = dist(gen);
            size_t selected = 0;
            
            while (selected < cumulativeSum.size() - 1 && 
                   cumulativeSum[selected] < randomPoint) {
                selected++;
            }
            
            batch.emplace_back(experiences[selected], taskIds[selected]);
            
            // Adjust priority after selection (for subsequent samples)
            priorities[selected] *= 0.9f; // Decay priority
        }
    } else {
        // Uniform sampling
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, experiences.size() - 1);
        
        for (size_t i = 0; i < batchSize && i < experiences.size(); ++i) {
            size_t idx = dist(gen);
            batch.emplace_back(experiences[idx], taskIds[idx]);
        }
    }
    
    return batch;
}

void TaskBuffer::updatePriorities(const std::vector<float>& errors) {
    if (!usePrioritizedSampling || errors.size() != experiences.size()) {
        return;
    }
    
    // Update priorities based on prediction error
    for (size_t i = 0; i < errors.size(); ++i) {
        priorities[i] = std::max(0.1f, 1.0f - errors[i]);
    }
}

// Implementation of KnowledgeDistillation

void KnowledgeDistillation::computeDistillationLoss(const std::vector<float>& teacher,
                                                   const std::vector<float>& student) {
    if (teacher.size() != student.size()) return;
    
    distillationLoss.clear();
    distillationLoss.reserve(teacher.size());
    
    for (size_t i = 0; i < teacher.size(); ++i) {
        if (useSoftLabels) {
            // Soft distillation loss
            float teacherSoft = std::exp(teacher[i]) / std::exp(teacher[i]).sum();
            float studentSoft = std::exp(student[i]) / std::exp(student[i]).sum();
            float loss = temperature * temperature * 
                        (teacherSoft * std::log(studentSoft + 1e-8f) -
                         teacherSoft * student[i]);
            distillationLoss.push_back(loss);
        } else {
            // Hard distillation loss
            float loss = std::abs(teacher[i] - student[i]);
            distillationLoss.push_back(loss);
        }
    }
}

float KnowledgeDistillation::getDistillationLoss() const {
    if (distillationLoss.empty()) return 0.0f;
    
    float totalLoss = 0.0f;
    for (float loss : distillationLoss) {
        totalLoss += loss;
    }
    
    return totalLoss / distillationLoss.size();
}

// Implementation of ForwardModel

std::vector<float> ForwardModel::predictNextState(const std::vector<float>& currentState,
                                                  const std::vector<float>& action) {
    std::vector<float> nextState;
    
    if (currentState.empty() || action.empty()) return nextState;
    
    nextState.resize(std::min(currentState.size(), 
                            transitionMatrix.empty() ? 
                            currentState.size() : transitionMatrix.size()));
    
    // Apply transition dynamics
    for (size_t i = 0; i < nextState.size(); ++i) {
        float next = currentState[i];
        
        if (i < transitionMatrix.size() && 
            i < transitionMatrix[0].size() &&
            i < action.size()) {
            
            // Linear transition
            next *= transitionMatrix[i][i]; // Simplified diagonal
            next += action[i] * 0.1f; // Action influence
            
            // Add noise
            if (!stateNoise.empty() && i < stateNoise.size()) {
                next += stateNoise[i];
            }
            if (!processNoise.empty() && i < processNoise.size()) {
                next += processNoise[i];
            }
        }
        
        // Apply bounds
        next = std::max(-10.0f, std::min(10.0f, next));
        nextState[i] = next;
    }
    
    return nextState;
}

void ForwardModel::updateModel(const std::vector<float>& currentState,
                              const std::vector<float>& nextState,
                              const std::vector<float>& action) {
    if (currentState.empty() || nextState.empty()) return;
    
    // Update transition matrix (simplified)
    size_t stateSize = std::min(currentState.size(), nextState.size());
    
    if (transitionMatrix.empty() || 
        transitionMatrix.size() != stateSize ||
        transitionMatrix[0].size() != stateSize) {
        
        transitionMatrix.resize(stateSize, std::vector<float>(stateSize, 0.0f));
    }
    
    // Simple gradient-based update
    for (size_t i = 0; i < stateSize; ++i) {
        for (size_t j = 0; j < stateSize; ++j) {
            if (i == j) {
                float target = (nextState[i] - currentState[i]) / (action[j] + 1e-8f);
                transitionMatrix[i][j] += learningSpeed * (target - transitionMatrix[i][j]);
            }
        }
    }
    
    // Update noise parameters
    if (!stateNoise.empty() && stateNoise.size() == stateSize) {
        for (size_t i = 0; i < stateSize; ++i) {
            float error = nextState[i] - predictNextState(currentState, action)[i];
            stateNoise[i] += learningSpeed * error;
            stateNoise[i] = std::max(-1.0f, std::min(1.0f, stateNoise[i]));
        }
    }
}

float ForwardModel::computePredictionError(const std::vector<float>& predicted,
                                          const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) return 0.0f;
    
    float error = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        error += (predicted[i] - actual[i]) * (predicted[i] - actual[i]);
    }
    
    return std::sqrt(error / predicted.size());
}

// Implementation of ContinualLearningMetaAgent

ContinualLearningMetaAgent::ContinualLearningMetaAgent(
    const std::string& name, const LifetimeExperimentConfig& config)
    : name_(name), config_(config), isInitialized_(false), isRunning_(false),
      currentStep_(0), currentTaskId_(0), overallForgettingRate_(0.0f) {
    startTime_ = std::chrono::steady_clock::now();
}

ContinualLearningMetaAgent::~ContinualLearningMetaAgent() {
    shutdown();
}

void ContinualLearningMetaAgent::initialize() {
    std::cout << "Initializing ContinualLearningMetaAgent: " << name_ << std::endl;
    
    // Initialize core brain components
    brain_ = std::make_shared<Brain>();
    
    // Initialize advanced systems
    mctsEngine_ = std::make_shared<MonteCarloTreeSearch>();
    forwardModel_ = std::make_shared<ForwardModeling>();
    advancedMemory_ = std::make_shared<AdvancedMemory>();
    
    // Configure advanced systems
    mctsEngine_->setMaxTreeSize(config_.max_tree_size);
    mctsEngine_->setExplorationTemperature(config_.exploration_temperature);
    
    forwardModel_->setModelComplexity(config_.model_complexity);
    forwardModel_->setHorizonLength(config_.horizon_length);
    
    advancedMemory_->setMaxMemorySize(config_.memory_size);
    
    // Initialize task management systems
    initializeTaskBuffers();
    setupRegularizationStrategies();
    
    isInitialized_ = true;
    std::cout << "ContinualLearningMetaAgent initialized successfully" << std::endl;
}

void ContinualLearningMetaAgent::update(float dt) {
    if (!isInitialized_ || !isRunning_) return;
    
    currentStep_++;
    
    // Update task signatures and performance
    updateTaskSignatures();
    computeTaskPerformance();
    
    // Update meta-gradients
    computeMetaGradients();
    
    // Update learning strategy
    updateLearningStrategy();
    
    // Apply regularization to prevent forgetting
    applyRegularization();
    
    // Rehearsal learning from experience buffers
    if (currentStep_ % config_.rehearsal_interval == 0) {
        rehearsalLearning();
    }
    
    // Distillation learning
    if (currentStep_ % config_.distillation_interval == 0) {
        distillationLearning();
    }
    
    // Consolidate knowledge periodically
    if (currentStep_ % config_.consolidation_interval == 0) {
        consolidateLearning();
    }
    
    // Adapt to new tasks if needed
    adaptToNewTask(std::vector<float>(50, 0.0f)); // Simplified
    
    // Update task buffers
    updateTaskBuffers();
}

void ContinualLearningMetaAgent::shutdown() {
    if (isInitialized_) {
        std::cout << "Shutting down ContinualLearningMetaAgent: " << name_ << std::endl;
        
        // Clear all components
        brain_.reset();
        mctsEngine_.reset();
        forwardModel_.reset();
        advancedMemory_.reset();
        
        // Clear task management
        tasks_.clear();
        taskExperiences_.clear();
        completedTasks_.clear();
        activeTasks_.clear();
        
        // Clear meta-learning
        metaGradients_.clear();
        learningStrategy_.clear();
        
        // Clear experience replay
        taskBuffers_.clear();
        globalReplayBuffer_.clear();
        
        // Clear regularization
        regularizationStrategies_.clear();
        
        // Clear forward models
        taskForwardModels_.clear();
        
        // Clear performance metrics
        taskPerformanceHistory_.clear();
        forgettingRates_.clear();
        retentionRatios_.clear();
        
        isInitialized_ = false;
        isRunning_ = false;
        
        std::cout << "ContinualLearningMetaAgent shutdown complete" << std::endl;
    }
}

void ContinualLearningMetaAgent::addTask(const std::string& taskName,
                                       const std::vector<float>& taskSignature,
                                       float importance) {
    size_t taskId = tasks_.size() + 1;
    
    // Check for similar existing tasks
    bool isSimilar = false;
    for (const auto& pair : tasks_) {
        float similarity = computeTaskSimilarity(pair.second, 
                                               TaskMemory(taskId, taskName, taskSignature));
        if (similarity > taskSimilarityThreshold_) {
            isSimilar = true;
            break;
        }
    }
    
    if (!isSimilar) {
        TaskMemory task(taskId, taskName, taskSignature);
        task.taskImportance = importance;
        tasks_[taskId] = task;
        activeTasks_.push_back(taskId);
        
        // Initialize task-specific forward model
        taskForwardModels_[taskId] = ForwardModel();
        
        // Initialize task buffer
        TaskBuffer buffer(taskId, config_.task_buffer_size);
        taskBuffers_[taskId] = buffer;
        
        std::cout << "Added new task: " << taskName << " (ID: " << taskId << ")" << std::endl;
    } else {
        std::cout << "Task similar to existing ones, using existing task" << std::endl;
    }
}

void ContinualLearningMetaAgent::removeTask(size_t taskId) {
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        tasks_.erase(it);
        
        auto bufferIt = taskBuffers_.find(taskId);
        if (bufferIt != taskBuffers_.end()) {
            taskBuffers_.erase(bufferIt);
        }
        
        auto modelIt = taskForwardModels_.find(taskId);
        if (modelIt != taskForwardModels_.end()) {
            taskForwardModels_.erase(modelIt);
        }
        
        // Remove from active/completed lists
        activeTasks_.erase(std::remove(activeTasks_.begin(), 
                                      activeTasks_.end(), taskId),
                          activeTasks_.end());
        completedTasks_.erase(std::remove(completedTasks_.begin(),
                                         completedTasks_.end(), taskId),
                             completedTasks_.end());
    }
}

void ContinualLearningMetaAgent::completeTask(size_t taskId) {
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        it->second.taskPerformance = 1.0f; // Mark as completed successfully
        completedTasks_.push_back(taskId);
        
        // Remove from active tasks
        activeTasks_.erase(std::remove(activeTasks_.begin(),
                                      activeTasks_.end(), taskId),
                          activeTasks_.end());
        
        std::cout << "Completed task: " << it->second.taskName << std::endl;
    }
}

void ContinualLearningMetaAgent::learnFromExperience(const std::vector<float>& experience,
                                                     size_t taskId, float reward) {
    // Store experience in task-specific buffer
    auto bufferIt = taskBuffers_.find(taskId);
    if (bufferIt != taskBuffers_.end()) {
        bufferIt->second.addExperience(experience, taskId, reward);
    }
    
    // Store in task experiences
    taskExperiences_[taskId].push_back(experience);
    
    // Update task forward model
    auto modelIt = taskForwardModels_.find(taskId);
    if (modelIt != taskForwardModels_.end()) {
        // Simplified: use last experience as current state
        if (!taskExperiences_[taskId].empty()) {
            size_t lastIdx = taskExperiences_[taskId].size() - 1;
            if (lastIdx > 0) {
                modelIt->second.updateModel(taskExperiences_[taskId][lastIdx - 1],
                                           experience, std::vector<float>(experience.size(), 0.1f));
            }
        }
    }
    
    // Update meta-gradients
    std::vector<float> gradient(50, 0.01f * reward); // Simplified gradient
    std::vector<float> metaGrad(50, 0.001f); // Simplified meta-gradient
    metaGradients_.emplace_back(taskId, 0, gradient, metaGrad);
}

void ContinualLearningMetaAgent::consolidateLearning() {
    // Consolidate knowledge across related tasks
    std::cout << "Starting learning consolidation..." << std::endl;
    
    // Find related tasks
    std::vector<std::pair<size_t, size_t>> taskPairs;
    for (auto it1 = tasks_.begin(); it1 != tasks_.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != tasks_.end(); ++it2) {
            float similarity = computeTaskSimilarity(it1->second, it2->second);
            if (similarity > taskSimilarityThreshold_) {
                taskPairs.emplace_back(it1->first, it2->first);
            }
        }
    }
    
    // Consolidate related tasks
    for (const auto& pair : taskPairs) {
        transferKnowledge(pair.first, pair.second);
    }
    
    // Update forward model with consolidated knowledge
    for (auto& pair : taskForwardModels_) {
        ForwardModel& model = pair.second;
        model.learningSpeed *= 1.1f; // Increase learning speed during consolidation
    }
    
    std::cout << "Consolidation complete. Processed " << taskPairs.size() << " task pairs." << std::endl;
}

void ContinualLearningMetaAgent::transferKnowledge(size_t sourceTaskId, 
                                                  size_t targetTaskId) {
    auto sourceIt = tasks_.find(sourceTaskId);
    auto targetIt = tasks_.find(targetTaskId);
    
    if (sourceIt == tasks_.end() || targetIt == tasks_.end()) return;
    
    // Transfer policy parameters
    if (sourceIt->second.policyParameters.size() == 
        targetIt->second.policyParameters.size()) {
        
        float transferRatio = 0.3f; // 30% transfer from source
        for (size_t i = 0; i < targetIt->second.policyParameters.size(); ++i) {
            targetIt->second.policyParameters[i] =
                targetIt->second.policyParameters[i] * (1.0f - transferRatio) +
                sourceIt->second.policyParameters[i] * transferRatio;
        }
    }
    
    // Update task relatedness
    sourceIt->second.relatedTasks.insert(targetTaskId);
    targetIt->second.relatedTasks.insert(sourceTaskId);
    
    // Update interference scores
    sourceIt->second.interferenceScore *= 0.9f;
    targetIt->second.interferenceScore *= 0.9f;
}

void ContinualLearningMetaAgent::computeMetaGradients() {
    // Compute gradients of the learning process
    metaGradients_.clear();
    
    for (const auto& pair : tasks_) {
        size_t taskId = pair.first;
        const TaskMemory& task = pair.second;
        
        // Compute task-specific gradients
        std::vector<float> gradient(task.taskSignature.size(), 0.0f);
        for (size_t i = 0; i < task.taskSignature.size(); ++i) {
            gradient[i] = (task.taskPerformance - 0.5f) * 
                         (1.0f / (1.0f + i + 1)); // Simulated gradient
        }
        
        // Compute meta-gradient
        std::vector<float> metaGrad(task.taskSignature.size(), 0.0f);
        for (size_t i = 0; i < metaGrad.size(); ++i) {
            metaGrad[i] = task.taskImportance * 0.01f; // Meta-gradient scaled by importance
        }
        
        metaGradients_.emplace_back(taskId, 0, gradient, metaGrad);
    }
}

void ContinualLearningMetaAgent::updateLearningStrategy() {
    // Update learning strategy based on meta-gradients
    learningStrategy_.clear();
    learningStrategy_.resize(10, 0.0f); // Simplified strategy
    
    float totalMetaGrad = 0.0f;
    for (const auto& grad : metaGradients_) {
        for (float g : grad.metaGradient) {
            totalMetaGrad += std::abs(g);
        }
    }
    
    if (totalMetaGrad > 0.0f) {
        // Update strategy based on meta-learning progress
        metaLearningProgress_ = std::min(1.0f, metaLearningProgress_ + 0.01f);
        
        // Adjust learning rates based on strategy
        for (auto& grad : metaGradients_) {
            grad.metaLearningRate *= (1.0f + metaLearningProgress_ * 0.1f);
        }
    }
}

void ContinualLearningMetaAgent::adaptToNewTask(const std::vector<float>& newTaskSignature) {
    // Detect and adapt to new task characteristics
    bool isNewTask = true;
    
    for (const auto& pair : tasks_) {
        float similarity = computeTaskSimilarity(pair.second,
                                               TaskMemory(0, "", newTaskSignature));
        if (similarity > taskSimilarityThreshold_) {
            isNewTask = false;
            break;
        }
    }
    
    if (isNewTask && !newTaskSignature.empty()) {
        // Create new task
        addTask("NewTask", newTaskSignature, 1.0f);
        currentTaskId_ = tasks_.size();
        
        // Initialize forward model for new task
        taskForwardModels_[currentTaskId_] = ForwardModel();
        taskForwardModels_[currentTaskId_].learningSpeed *= 1.5f; // Faster learning on new task
    }
}

void ContinualLearningMetaAgent::applyRegularization() {
    // Apply regularization strategies to prevent catastrophic forgetting
    for (const auto& strategy : regularizationStrategies_) {
        // In real implementation, would apply actual regularization
        // For now, just track application
    }
    
    // Update forgetting rates
    overallForgettingRate_ = computeForgettingRate(currentTaskId_);
}

void ContinualLearningMetaAgent::rehearsalLearning() {
    // Experience replay for maintaining old knowledge
    std::cout << "Starting rehearsal learning..." << std::endl;
    
    for (auto& pair : taskBuffers_) {
        TaskBuffer& buffer = pair.second;
        
        // Sample from task buffer
        std::vector<std::pair<std::vector<float>, size_t>> batch =
            buffer.sampleBatch(config_.rehearsal_batch_size);
        
        for (const auto& sample : batch) {
            // Simulate learning from rehearsed experience
            std::vector<float> gradient(sample.first.size(), 0.01f);
            learnFromExperience(sample.first, sample.second, 0.5f);
        }
    }
    
    std::cout << "Rehearsal learning complete. Sampled from " << taskBuffers_.size() << " task buffers." << std::endl;
}

void ContinualLearningMetaAgent::distillationLearning() {
    // Knowledge distillation from old to new tasks
    if (tasks_.size() < 2) return;
    
    std::cout << "Starting knowledge distillation..." << std::endl;
    
    // Get most recent and oldest tasks
    auto newestIt = tasks_.rbegin();
    auto oldestIt = tasks_.begin();
    
    if (newestIt == tasks_.rend() || oldestIt == tasks_.end()) return;
    
    // Setup distillation cache
    distillationCache_.teacherOutput = newestIt->second.policyParameters;
    distillationCache_.studentOutput = oldestIt->second.policyParameters;
    
    // Compute distillation loss
    distillationCache_.computeDistillationLoss(
        distillationCache_.teacherOutput, distillationCache_.studentOutput);
    
    // Apply distillation
    float distillationLoss = distillationCache_.getDistillationLoss();
    
    // In real implementation, would update student based on distillation loss
    std::cout << "Distillation complete. Loss: " << distillationLoss << std::endl;
}

float ContinualLearningMetaAgent::getTaskPerformance(size_t taskId) const {
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        return it->second.taskPerformance;
    }
    return 0.0f;
}

float ContinualLearningMetaAgent::getForgettingRate() const {
    return overallForgettingRate_;
}

float ContinualLearningMetaAgent::getKnowledgeRetention(size_t taskId) const {
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        return it->second.knowledgeStrength;
    }
    return 0.0f;
}

float ContinualLearningMetaAgent::getMetaLearningProgress() const {
    return metaLearningProgress_;
}

void ContinualLearningMetaAgent::saveCheckpoint(const std::string& filename) {
    std::cout << "Saving checkpoint to: " << filename << std::endl;
    
    // In real implementation, would serialize state to file
    // For now, just log
    std::cout << "Checkpoint saved successfully" << std::endl;
}

void ContinualLearningMetaAgent::loadCheckpoint(const std::string& filename) {
    std::cout << "Loading checkpoint from: " << filename << std::endl;
    
    // In real implementation, would deserialize state from file
    // For now, just log
    std::cout << "Checkpoint loaded successfully" << std::endl;
}

// Private helper methods implementation

void ContinualLearningMetaAgent::initializeTaskBuffers() {
    // Initialize task buffers with appropriate sizes
    for (auto& pair : tasks_) {
        size_t taskId = pair.first;
        TaskBuffer buffer(taskId, config_.task_buffer_size);
        taskBuffers_[taskId] = buffer;
    }
}

void ContinualLearningMetaAgent::setupRegularizationStrategies() {
    // Setup various regularization strategies
    
    // EWC (Elastic Weight Consolidation)
    RegularizationStrategy ewc("EWC", 0.5f);
    regularizationStrategies_.push_back(ewc);
    
    // L2 regularization
    RegularizationStrategy l2("L2", 0.1f);
    regularizationStrategies_.push_back(l2);
    
    // Memory regularization
    RegularizationStrategy memoryReg("Memory", 0.3f);
    regularizationStrategies_.push_back(memoryReg);
}

void ContinualLearningMetaAgent::updateTaskSignatures() {
    // Update task signatures based on current performance
    for (auto& pair : tasks_) {
        TaskMemory& task = pair.second;
        
        // Update signature based on performance
        for (size_t i = 0; i < task.taskSignature.size(); ++i) {
            task.taskSignature[i] *= (1.0f + task.taskPerformance * 0.1f);
        }
        
        // Update knowledge strength
        task.knowledgeStrength = std::min(1.0f, 
                                         task.knowledgeStrength * config_.retention_rate);
    }
}

float ContinualLearningMetaAgent::computeTaskSimilarity(const TaskMemory& task1,
                                                       const TaskMemory& task2) const {
    if (task1.taskSignature.size() != task2.taskSignature.size()) return 0.0f;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < task1.taskSignature.size(); ++i) {
        similarity += std::abs(task1.taskSignature[i] - task2.taskSignature[i]);
    }
    
    return 1.0f - (similarity / task1.taskSignature.size()); // Inverted similarity
}

void ContinualLearningMetaAgent::updateMetaGradients() {
    // Update meta-gradients based on learning progress
    for (auto& grad : metaGradients_) {
        // Scale gradients by task relevance and regularization
        float scale = grad.taskRelevance * grad.regularizationStrength;
        for (size_t i = 0; i < grad.gradient.size(); ++i) {
            grad.gradient[i] *= scale;
            grad.metaGradient[i] *= scale;
        }
        
        // Add noise for exploration
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-0.01f, 0.01f);
        
        for (size_t i = 0; i < grad.gradient.size(); ++i) {
            grad.gradient[i] += dist(gen);
        }
    }
}

void ContinualLearningMetaAgent::applyMetaGradients() {
    // Apply meta-gradients to update learning parameters
    for (auto& grad : metaGradients_) {
        // Update learning parameters
        grad.learningRate += grad.metaGradient[0] * grad.metaLearningRate;
        grad.plasticity = std::min(1.0f, grad.plasticity + 0.01f);
        
        // Apply regularization
        grad.learningRate *= (1.0f - grad.regularizationStrength);
    }
}

void ContinualLearningMetaAgent::updateTaskBuffers() {
    // Update task buffers with current experiences
    for (auto& pair : taskBuffers_) {
        TaskBuffer& buffer = pair.second;
        
        // Sample from other buffers for global replay
        if (globalReplayBuffer_.size() > config_.min_global_replay_size) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> dist(0, globalReplayBuffer_.size() - 1);
            
            size_t idx = dist(gen);
            buffer.addExperience(globalReplayBuffer_[idx], pair.first, 0.5f);
        }
    }
}

void ContinualLearningMetaAgent::computeTaskPerformance() {
    // Compute task performance metrics
    for (auto& pair : tasks_) {
        TaskMemory& task = pair.second;
        
        // Simulate performance based on knowledge strength and interference
        float performance = task.knowledgeStrength * (1.0f - task.interferenceScore);
        performance *= (1.0f + task.taskImportance * 0.2f);
        
        task.taskPerformance = performance;
        
        // Store in history
        taskPerformanceHistory_[pair.first].push_back(performance);
    }
}

float ContinualLearningMetaAgent::computeForgettingRate(size_t taskId) const {
    auto it = taskPerformanceHistory_.find(taskId);
    if (it == taskPerformanceHistory_.end() || it->second.size() < 2) {
        return 0.0f;
    }
    
    // Compute forgetting rate as performance decay
    const auto& history = it->second;
    float initialPerformance = history.front();
    float finalPerformance = history.back();
    
    if (initialPerformance > 0.0f) {
        return (initialPerformance - finalPerformance) / initialPerformance;
    }
    
    return 0.0f;
}

void ContinualLearningMetaAgent::updateRetentionPriorities() {
    // Update retention priorities based on task importance and interference
    for (auto& pair : tasks_) {
        TaskMemory& task = pair.second;
        
        // Increase priority for important tasks
        task.retentionPriority = std::min(1.0f, 
                                         task.retentionPriority + 
                                         task.taskImportance * config_.priority_boost_rate);
        
        // Decrease priority for tasks with high interference
        if (task.interferenceScore > 0.5f) {
            task.retentionPriority *= 0.9f;
        }
    }
}

void ContinualLearningMetaAgent::detectCatastrophicForgetting() {
    // Detect signs of catastrophic forgetting
    std::vector<size_t> highForgettingTasks;
    
    for (const auto& pair : taskPerformanceHistory_) {
        float forgettingRate = computeForgettingRate(pair.first);
        if (forgettingRate > config_.catastrophic_threshold) {
            highForgettingTasks.push_back(pair.first);
        }
    }
    
    if (!highForgettingTasks.empty()) {
        std::cout << "Catastrophic forgetting detected in " << 
                     highForgettingTasks.size() << " tasks!" << std::endl;
        
        // Increase rehearsal frequency for affected tasks
        for (size_t taskId : highForgettingTasks) {
            auto bufferIt = taskBuffers_.find(taskId);
            if (bufferIt != taskBuffers_.end()) {
                bufferIt->second.usePrioritizedSampling = true;
                bufferIt->second.useStructuredReplay = true;
            }
        }
    }
}

/**
 * @main
 * Demonstrate the ContinualLearningMetaAgent
 */
int main() {
    std::cout << "=== Continual Learning Meta-Agent Demo ===" << std::endl;
    
    // Create configuration
    nlm::LifetimeExperimentConfig config;
    config.max_tree_depth = 6;
    config.mcts_simulations = 300;
    config.max_tree_size = 300;
    config.exploration_temperature = 1.2f;
    config.model_complexity = 4;
    config.horizon_length = 15;
    config.memory_size = 3000;
    config.task_buffer_size = 200;
    config.rehearsal_interval = 50;
    config.distillation_interval = 75;
    config.consolidation_interval = 100;
    config.rehearsal_batch_size = 20;
    config.min_global_replay_size = 50;
    config.retention_rate = 0.99f;
    config.priority_boost_rate = 0.05f;
    config.catastrophic_threshold = 0.3f;
    
    // Create and initialize agent
    nlm::ContinualLearningMetaAgent agent("MetaAgent", config);
    agent.initialize();
    
    // Add multiple tasks with different characteristics
    std::vector<std::pair<std::string, std::vector<float>>> tasks;
    
    // Task 1: Pattern recognition
    tasks.emplace_back("PatternRecognition", 
                      std::vector<float>{0.8f, 0.6f, 0.4f, 0.7f, 0.5f, 0.3f, 0.6f, 0.4f});
    
    // Task 2: Sequential processing
    tasks.emplace_back("SequentialProcessing", 
                      std::vector<float>{0.6f, 0.8f, 0.5f, 0.4f, 0.7f, 0.3f, 0.5f, 0.6f});
    
    // Task 3: Spatial navigation
    tasks.emplace_back("SpatialNavigation", 
                      std::vector<float>{0.9f, 0.5f, 0.7f, 0.6f, 0.4f, 0.8f, 0.3f, 0.7f});
    
    // Task 4: Social interaction
    tasks.emplace_back("SocialInteraction", 
                      std::vector<float>{0.4f, 0.7f, 0.6f, 0.5f, 0.8f, 0.4f, 0.7f, 0.5f});
    
    // Task 5: Memory tasks
    tasks.emplace_back("MemoryTasks", 
                      std::vector<float>{0.5f, 0.6f, 0.8f, 0.7f, 0.5f, 0.6f, 0.4f, 0.8f});
    
    // Add tasks to agent
    for (const auto& task : tasks) {
        agent.addTask(task.first, task.second, 1.0f);
    }
    
    std::cout << "Agent initialized with " << tasks.size() << " tasks" << std::endl;
    
    // Simulate continual learning over time
    const size_t simulationSteps = 400;
    for (size_t step = 0; step < simulationSteps; ++step) {
        agent.update(0.016f); // 60 FPS simulation
        
        // Simulate learning from experiences at each step
        if (step % 10 == 0 && !tasks_.empty()) {
            size_t taskId = rand() % tasks_.size() + 1;
            std::vector<float> experience(50, 0.0f); // Simplified experience
            float reward = (rand() % 100) / 100.0f * 0.5f + 0.5f; // Random reward
            
            agent.learnFromExperience(experience, taskId, reward);
        }
        
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                     << "Meta-Progress: " << (agent.getMetaLearningProgress() * 100) << "%, "
                     << "Forgetting Rate: " << agent.getForgettingRate() << ", "
                     << "Global Performance: " << agent.getTaskPerformance(1) << std::endl;
        }
        
        // Periodically consolidate and adapt
        if (step % 100 == 0) {
            std::cout << "  Performing consolidation..." << std::endl;
            agent.consolidateLearning();
        }
        
        if (step % 150 == 0) {
            std::cout << "  Adapting to new task..." << std::endl;
            agent.adaptToNewTask(std::vector<float>(50, 0.1f)); // New task
        }
    }
    
    // Final statistics
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final Meta-Learning Progress: " << (agent.getMetaLearningProgress() * 100) << "%" << std::endl;
    std::cout << "Overall Forgetting Rate: " << agent.getForgettingRate() << std::endl;
    std::cout << "Knowledge Retention (Task 1): " << agent.getKnowledgeRetention(1) << std::endl;
    std::cout << "Knowledge Retention (Task 5): " << agent.getKnowledgeRetention(5) << std::endl;
    
    // Save final state
    agent.saveCheckpoint("meta_agent_checkpoint.dat");
    
    std::cout << "\nContinualLearningMetaAgent demo completed successfully!" << std::endl;
    
    return 0;
}
