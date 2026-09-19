// ===== ADVANCED AGENT FEATURES HEADER =====

#pragma once

#include "../agent/AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>

namespace nlm {

// ===== EXPLORATION STRATEGY ENUM =====

enum class ExplorationStrategy {
    EPSILON_GREEDY,
    THOMPSON_SAMPLING,
    UCB1,
    ENTROPY_BASED,
    NOVELTY_DRIVEN,
    MULTI_ARM_BANDIT
};

// ===== ADVANCED EXPLORATION SYSTEM =====

class AdaptiveExplorationSystem {
public:
    AdaptiveExplorationSystem(std::shared_ptr<RandomGenerator> rng, size_t actionCount = 0);
    ~AdaptiveExplorationSystem();
    
    // Initialize action space for exploration
    void initializeActionSpace(size_t actionCount);
    
    // Thompson Sampling - Bayesian exploration-exploitation
    MotorCommand selectActionThompsonSampling();
    
    // UCB1 algorithm - Upper confidence bound exploration
    MotorCommand selectActionUCB(double explorationFactor = 1.0);
    
    // Entropy-based exploration bonuses
    double calculateEntropyBonus(const std::vector<float>& actionProbabilities) const;
    
    // Combined exploration scoring
    double calculateExplorationScore(size_t actionId, const std::vector<float>& qValues, 
                                   const std::vector<size_t>& actionCounts) const;
    
    // Reset exploration statistics
    void reset();
    
    // Learn from observed rewards (Q-learning style)
    void updateActionValue(size_t actionId, float reward, double confidence = 1.0);
    
    // Set current state for state-dependent exploration
    void setCurrentState(const std::string& state);
    
    // Getters
    const std::vector<float>& getActionValues() const { return qValues_; }
    const std::vector<size_t>& getActionCounts() const { return actionCounts_; }
    double getExplorationRate() const { return explorationRate_; }
    
    // Configuration
    void setExplorationStrategy(ExplorationStrategy strategy) { strategy_ = strategy; }
    void setExplorationRate(double rate) { explorationRate_ = rate; }
    void setUCBExplorationFactor(double factor) { ucbFactor_ = factor; }
    
private:
    struct ActionState {
        float qValue = 0.0f;
        size_t visitCount = 0;
        double confidence = 1.0;
        double bonus = 0.0;
    };
    
    // Thompson sampling internal state
    std::vector<ActionState> actionStates_;
    ExplorationStrategy strategy_ = ExplorationStrategy::THOMPSON_SAMPLING;
    double explorationRate_ = 0.1;
    double ucbFactor_ = 1.0;
    std::string currentState_;
    std::mt19937 rng_;
    
    // Entropy calculation utilities
    std::vector<double> softmaxTempered(const std::vector<double>& logits, double temperature) const;
    double calculateEntropy(const std::vector<double>& probabilities) const;
};

// ===== ENHANCED LEARNING MECHANISMS =====

class EnhancedLearningMechanisms {
public:
    EnhancedLearningMechanisms(std::shared_ptr<Brain> brain, std::shared_ptr<RandomGenerator> rng);
    ~EnhancedLearningMechanisms();
    
    // Reward shaping with intrinsic motivation
    float applyRewardShaping(float externalReward, const std::string& context = "") const;
    
    // Metacognitive performance monitoring
    bool monitorLearning() const;
    
    // Curriculum learning system
    void setCurrentSkill(int skillId) { currentSkill_ = skillId; }
    void advanceSkill() { currentSkill_ = std::min(currentSkill_ + 1, static_cast<int>(skillHierarchy_.size()) - 1); }
    bool isSkillMastered(int skillId) const { return skillMastery_[skillId]; }
    void setSkillMastery(int skillId, bool mastered) { skillMastery_[skillId] = mastered; }
    
    // Learning metrics
    struct LearningMetrics {
        double learningRate = 0.0;
        double convergenceScore = 0.0;
        size_t adaptationCount = 0;
        double intrinsicMotivation = 0.0;
        double metacognitiveConfidence = 0.0;
    };
    
    LearningMetrics getLearningMetrics() const;
    
    // Skill hierarchy management
    void setSkillHierarchy(const std::vector<std::string>& skills) {
        skillHierarchy_ = skills;
        skillMastery_.resize(skills.size(), false);
    }
    
private:
    struct Skill {
        std::string name;
        double difficulty = 0.0;
        double complexity = 0.0;
        double masteryThreshold = 0.8;
        bool mastered = false;
        double learningProgress = 0.0;
    };
    
    std::vector<Skill> skillHierarchy_;
    std::vector<bool> skillMastery_;
    int currentSkill_ = 0;
    mutable std::vector<double> learningHistory_;
    mutable double metaConfidence_ = 1.0;
    
    // Reward shaping components
    static constexpr double INTRINSIC_MOTIVATION_WEIGHT = 0.3;
    static constexpr double EXTRINSIC_MOTIVATION_WEIGHT = 0.7;
    static constexpr double LEARNING_RATE_DECAY = 0.995;
};

// ===== IMPROVED STATE MANAGEMENT =====

class ImprovedStateManagement {
public:
    ImprovedStateManagement(std::shared_ptr<Brain> brain, std::shared_ptr<RandomGenerator> rng);
    ~ImprovedStateManagement();
    
    // Hierarchical state abstraction
    void updateStateAbstraction(const std::string& rawState, const std::string& higherLevelState);
    
    // Working memory consolidation
    void consolidateWorkingMemory();
    void addRecentState(const std::string& state, double timestamp);
    
    // Stress screening and recovery mechanisms
    double assessStress() const;
    bool performRecoveryAction();
    void updateStress(double stressDelta);
    
    // State retrieval
    std::string getAbstraction(const std::string& rawState) const;
    std::vector<std::string> getRecentStates(size_t count = 10) const;
    double getCurrentStress() const { return currentStress_; }
    
    // Memory management
    void clearOldStates(double cutoffTime = 1000.0);
    size_t getWorkingMemorySize() const { return recentStates_.size(); }
    
private:
    struct StateRecord {
        std::string rawState;
        std::string abstractedState;
        double timestamp = 0.0;
        double stressLevel = 0.0;
    };
    
    std::vector<StateRecord> stateHistory_;
    std::string currentState_;
    double currentStress_ = 0.0;
    double stressDecayRate_ = 0.99;
    
    // Hierarchical abstraction
    std::unordered_map<std::string, std::string> abstractionMap_;
    
    // Working memory management
    static constexpr size_t MAX_RECENT_STATES = 100;
    static constexpr double STRESS_THRESHOLD = 0.7;
};

// ===== ADVANCED NEUROMODULATION =====

class AdvancedNeuromodulation {
public:
    AdvancedNeuromodulation(std::shared_ptr<Brain> brain, std::shared_ptr<RandomGenerator> rng);
    ~AdvancedNeuromodulation();
    
    // Predictive coding for forward models
    void computePredictionErrors(const std::string& motorCommand, float actualReward);
    
    // Homeostatic regulation of neuromodulators
    void regulateHomeostasis(double timestep);
    
    // Cross-modal integration
    void integrateCrossModal(const std::vector<float>& sensoryInput, const std::vector<float>& motorOutput);
    
    // Neuromodulator state management
    struct NeuromodulatorState {
        float dopamine = 0.0f;
        float serotonin = 0.0f;
        float norepinephrine = 0.0f;
        float acetylcholine = 0.0f;
        float adrenaline = 0.0f;
        double homeostasisLevel = 1.0;
        double predictionError = 0.0;
    };
    
    NeuromodulatorState getNeuromodulatorState() const { return neuromodState_; }
    
    // Predictive coding utilities
    void setMotorPrediction(const std::string& command) { lastMotorCommand_ = command; }
    double getLastPredictionError() const { return neuromodState_.predictionError; }
    
private:
    NeuromodulatorState neuromodState_;
    
    // Predictive coding components
    std::string lastMotorCommand_;
    std::unordered_map<std::string, double> predictionModel_;
    std::unordered_map<std::string, double> predictionErrorHistory_;
    
    // Homeostatic regulation
    double homeostaticTarget_ = 1.0;
    double homeostaticElasticity_ = 0.1;
    
    // Cross-modal integration
    std::vector<float> sensoryBuffer_;
    std::vector<float> motorBuffer_;
    
    // Utility functions
    void normalizeNeuromodulators();
    void applyHomeostaticControl(double timestep);
    void updatePredictionError();
};

// ===== PERFORMANCE OPTIMIZATION =====

class PerformanceOptimization {
public:
    PerformanceOptimization(std::shared_ptr<Brain> brain);
    ~PerformanceOptimization();
    
    // Batch processing for neural operations
    void executeBatchOperation(const std::vector<Neuron*>& neurons, double timestep);
    
    // Memory pooling for efficiency
    void createMemoryPool(size_t neuronCount);
    void releaseMemoryPool();
    
    // Early termination for converged learning
    bool checkConvergence(const std::vector<float>& performanceHistory, 
                         double convergenceThreshold = 0.01,
                         size_t minSamples = 100) const;
    
    // Batch size management
    void setBatchSize(size_t batchSize) { batchSize_ = batchSize; }
    size_t getBatchSize() const { return batchSize_; }
    
    // Performance monitoring
    struct PerformanceStats {
        double throughput = 0.0;
        double memoryUsage = 0.0;
        double cpuUtilization = 0.0;
        size_t activeNeurons = 0;
        double convergenceSpeed = 0.0;
    };
    
    PerformanceStats getPerformanceStats() const { return stats_; }
    
    // Memory pool management
    bool hasMemoryPool() const { return memoryPoolActive_; }
    void updateMemoryUsage(double newUsage) { stats_.memoryUsage = newUsage; }
    
private:
    size_t batchSize_ = 100;
    bool memoryPoolActive_ = false;
    size_t allocatedNeurons_ = 0;
    PerformanceStats stats_;
    
    // Batch processing utilities
    void optimizeBatchOrder(const std::vector<Neuron*>& neurons);
    void trackBatchPerformance(size_t operations, double duration);
    
    // Memory pooling utilities
    void allocateNeurons(size_t count);
    void deallocateNeurons();
    void optimizeMemoryAccess(const std::vector<Neuron*>& neurons);
};

// ===== COMPREHENSIVE AGENT BRAIN WITH ADVANCED FEATURES =====

class AdvancedAgentBrain : public AgentBrain {
public:
    AdvancedAgentBrain(std::shared_ptr<Brain> brain);
    ~AdvancedAgentBrain();
    
    // Initialize all advanced feature systems
    void initializeAllAdvancedFeatures(const SimpleWorld& world);
    
    // Configuration methods for advanced features
    void setExplorationStrategy(ExplorationStrategy strategy);
    void setUseEntropyBonus(bool use) { useEntropyBonus_ = use; }
    void setUCBExplorationFactor(double factor) { ucbExplorationFactor_ = factor; }
    void enableMetacognitiveMonitoring(bool enable) { metacognitiveMonitoringEnabled_ = enable; }
    void setCurriculumSkill(int skill) { if (learningMechanisms_) learningMechanisms_->setCurrentSkill(skill); }
    void advanceCurriculumSkill() { if (learningMechanisms_) learningMechanisms_->advanceSkill(); }
    void enableHierarchicalStateAbstraction(bool enable) { useHierarchicalState_ = enable; }
    void enableWorkingMemoryConsolidation(bool enable) { useWorkingMemoryConsolidation_ = enable; }
    void enablePredictiveCoding(bool enable) { predictiveCodingEnabled_ = enable; }
    void enableHomeostaticRegulation(bool enable) { homeostaticRegulationEnabled_ = enable; }
    void enableCrossModalIntegration(bool enable) { crossModalIntegrationEnabled_ = enable; }
    void enableBatchProcessing(bool enable) { if (performanceOpt_) performanceOpt_->setBatchSize(enable ? 1000 : 1); }
    void enableMemoryPooling(bool enable) { if (performanceOpt_) memoryPoolEnabled_ = enable; }
    void enableEarlyTermination(bool enable) { earlyTerminationEnabled_ = enable; }
    
    // Get advanced feature statistics
    struct AdvancedFeatureStats {
        ExplorationStrategy currentStrategy;
        double explorationRate;
        double entropyBonus;
        bool metacognitiveMonitoring;
        bool hierarchicalStateUsed;
        bool workingMemoryActive;
        bool predictiveCodingActive;
        bool homeostaticRegulationActive;
        bool crossModalIntegrationActive;
        bool batchProcessingActive;
        bool memoryPoolingActive;
        bool earlyTerminationActive;
    };
    
    AdvancedFeatureStats getAdvancedFeatureStats() const;
    
    // Override existing methods to integrate advanced features
    void processSensoryInput(const SensoryPercept& percept) override;
    MotorCommand decodeMotorCommand() override;
    void applyRewardModulation(float reward, float predictedReward) override;
    void updateDevelopment(double timestep) override;
    void reset() override;
    
private:
    // Advanced feature systems
    std::unique_ptr<AdaptiveExplorationSystem> explorationSystem_;
    std::unique_ptr<EnhancedLearningMechanisms> learningMechanisms_;
    std::unique_ptr<ImprovedStateManagement> stateManagement_;
    std::unique_ptr<AdvancedNeuromodulation> neuromodulation_;
    std::unique_ptr<PerformanceOptimization> performanceOpt_;
    
    // Configuration flags
    bool useEntropyBonus_ = false;
    bool metacognitiveMonitoringEnabled_ = false;
    bool useHierarchicalState_ = false;
    bool useWorkingMemoryConsolidation_ = false;
    bool predictiveCodingEnabled_ = false;
    bool homeostaticRegulationEnabled_ = false;
    bool crossModalIntegrationEnabled_ = false;
    bool memoryPoolEnabled_ = false;
    bool earlyTerminationEnabled_ = false;
    
    // Random generator
    std::shared_ptr<RandomGenerator> rng_;
    
    // Performance statistics
    mutable size_t batchOperations_ = 0;
    mutable double totalBatchTime_ = 0.0;
};

} // namespace nlm