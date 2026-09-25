// Advanced Learning Algorithms with Meta-Learning
// Implement sophisticated learning capabilities including meta-learning, curriculum learning, and adaptive algorithms

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <mutex>
#include <any>
#include <optional>
#include <random>
#include <set>

namespace nlm {
namespace learning {

// Learning algorithm types
enum class LearningAlgorithmType {
    Supervised,
    Unsupervised,
    Reinforcement,
    Evolutionary,
    MetaLearning,
    Curriculum,
    TransferLearning,
    Lifelong,
    Deep,  // Neural network based learning
    Adaptive,
    SelfSupervised,
    FewShot
};

// Learning objectives
enum class LearningObjective {
    Classification,
    Regression,
    Clustering,
    AnomalyDetection,
    ReinforcementLearning,
    PolicyOptimization,
    ValueEstimation,
    SequencePrediction,
    PatternRecognition,
    ConceptFormation,
    SkillAcquisition
};

// Meta-learning strategies
enum class MetaLearningStrategy {
    MAML,               // Model-Agnostic Meta-Learning
    PrototypicalNetworks,
    LearningToLearn,
    CurriculumMetaLearning,
    MultiTaskMetaLearning,
    GradientBasedMetaLearning,
    OptimizationBasedMetaLearning
};

// Learning rate adaptation methods
enum class LearningRateAdapter {
    Constant,
    StepDecay,
    ExponentialDecay,
    CosineAnnealing,
    ReduceOnPlateau,
    Adam,
    RMSprop,
    AdaGrad,
    AdaDelta,
    Custom
};

// Curriculum learning components
struct CurriculumStage {
    std::string name;
    LearningObjective objective;
    double difficulty;  // 0.0 to 1.0
    double weight;      // 0.0 to 1.0 (importance)
    double duration;    // Time or number of samples
    std::function<bool(const std::unordered_map<std::string, std::any>&)> completionCondition;
    std::unordered_map<std::string, std::any> parameters;
};

// Meta-learning episode structure
struct MetaLearningEpisode {
    uint64_t episodeId;
    double startTime;
    double endTime;
    LearningObjective objective;
    std::unordered_map<std::string, std::any> context;
    std::unordered_map<std::string, std::any> results;
    double reward;
    bool completed;
};

// Transfer learning knowledge representation
struct TransferKnowledge {
    std::string sourceTask;
    std::string targetTask;
    std::unordered_map<std::string, std::any> learnedParameters;
    std::vector<std::string> transferableSkills;
    double transferability;  // 0.0 to 1.0
    double adaptationCost;
};

// Base learning algorithm interface
class ILearningAlgorithm {
public:
    virtual ~ILearningAlgorithm() = default;
    
    // Algorithm metadata
    virtual std::string getName() const = 0;
    virtual LearningAlgorithmType getType() const = 0;
    virtual std::string getDescription() const = 0;
    virtual LearningObjective getPrimaryObjective() const = 0;
    
    // Learning configuration
    virtual bool configure(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual std::unordered_map<std, std::any> getConfiguration() const = 0;
    
    // Learning lifecycle
    virtual bool initialize() = 0;
    virtual bool train(const std::unordered_map<std::string, std::any>& trainingData) = 0;
    virtual bool validate(const std::unordered_map<std::string, std::any>& validationData) = 0;
    virtual bool test(const std::unordered_map<std::string, std::any>& testData) = 0;
    virtual bool shutdown() = 0;
    
    // Algorithm-specific operations
    virtual std::any predict(const std::unordered_map<std::string, std::any>& input) = 0;
    virtual float computeLoss(const std::unordered_map<std::string, std::any>& prediction,
                              const std::unordered_map<std::string, std::any>& target) = 0;
    virtual void updateParameters(const std::unordered_map<std::string, std::any>& gradients) = 0;
    
    // Learning management
    virtual bool reset() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    virtual bool checkpoint(const std::string& filename) const = 0;
    virtual bool restore(const std::string& filename) = 0;
    
    // Statistics and monitoring
    virtual size_t getTrainingSamplesProcessed() const = 0;
    virtual double getTrainingTime() const = 0;
    virtual float getCurrentLoss() const = 0;
    virtual float getBestLoss() const = 0;
    virtual std::unordered_map<std::string, std::any> getPerformanceMetrics() const = 0;
    
    // Algorithm capabilities
    virtual bool supportsOnlineLearning() const = 0;
    virtual bool supportsBatchLearning() const = 0;
    virtual bool supportsIncrementalLearning() const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual size_t getModelSize() const = 0;
};

// Typed learning algorithm template
template<typename T>
class LearningAlgorithmBase : public ILearningAlgorithm {
public:
    LearningAlgorithmBase(const std::string& name,
                         LearningAlgorithmType type,
                         const std::string& description,
                         LearningObjective objective)
        : name_(name), type_(type), description_(description), objective_(objective) {}
    
    std::string getName() const override { return name_; }
    LearningAlgorithmType getType() const override { return type_; }
    std::string getDescription() const override { return description_; }
    LearningObjective getPrimaryObjective() const override { return objective_; }
    
    bool configure(const std::unordered_map<std::string, std::any>& config) override {
        // Store configuration
        for (const auto& pair : config) {
            config_[pair.first] = pair.second;
        }
        return true;
    }
    
    std::unordered_map<std::string, std::any> getConfiguration() const override {
        return config_;
    }
    
    virtual bool initialize() override { return true; }
    virtual bool shutdown() override { return true; }
    virtual bool reset() override { return true; }
    virtual bool pause() override { return true; }
    virtual bool resume() override { return true; }
    
    virtual bool checkpoint(const std::string& filename) const override {
        // Implement checkpointing
        return false;
    }
    
    virtual bool restore(const std::string& filename) override {
        // Implement restoration
        return false;
    }
    
    virtual size_t getMemoryUsage() const override { return 0; }
    virtual size_t getModelSize() const override { return 0; }
    
protected:
    std::string name_;
    LearningAlgorithmType type_;
    std::string description_;
    LearningObjective objective_;
    std::unordered_map<std::string, std::any> config_;
    mutable std::mutex mutex_;
    
    // Statistics
    size_t trainingSamplesProcessed_ = 0;
    double trainingTime_ = 0.0;
    float currentLoss_ = 0.0f;
    float bestLoss_ = std::numeric_limits<float>::max();
};

// Supervised Learning Algorithm
class SupervisedLearningAlgorithm : public LearningAlgorithmBase<SupervisedLearningAlgorithm> {
public:
    SupervisedLearningAlgorithm() : LearningAlgorithmBase<SupervisedLearningAlgorithm>(
        "Supervised Learning",
        LearningAlgorithmType::Supervised,
        "Standard supervised learning with classification and regression",
        LearningObjective::Classification) {
        config_["model_type"] = std::string("neural_network");
        config_["optimizer"] = std::string("adam");
        config_["batch_size"] = 32;
        config_["epochs"] = 100;
        config_["learning_rate"] = 0.001f;
        config_["early_stopping"] = true;
        config_["validation_split"] = 0.2f;
    }
    
    bool train(const std::unordered_map<std::string, std::any>& trainingData) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Training implementation
        // Process training data and update model parameters
        
        auto endTime = std::chrono::high_resolution_clock::now();
        trainingTime_ += std::chrono::duration<double>(endTime - startTime).count();
        
        return true;
    }
    
    bool validate(const std::unordered_map<std::string, std::any>& validationData) override {
        // Validation implementation
        return true;
    }
    
    bool test(const std::unordered_map<std::string, std::any>& testData) override {
        // Testing implementation
        return true;
    }
    
    std::any predict(const std::unordered_map<std::string, std::any>& input) override {
        // Prediction implementation
        return std::any();
    }
    
    float computeLoss(const std::unordered_map<std::string, std::any>& prediction,
                      const std::unordered_map<std::string, std::any>& target) override {
        // Loss computation implementation
        return 0.0f;
    }
    
    void updateParameters(const std::unordered_map<std::string, std::any>& gradients) override {
        // Parameter update implementation
    }
    
    size_t getTrainingSamplesProcessed() const override { return trainingSamplesProcessed_; }
    double getTrainingTime() const override { return trainingTime_; }
    float getCurrentLoss() const override { return currentLoss_; }
    float getBestLoss() const override { return bestLoss_; }
    
    bool supportsOnlineLearning() const override { return true; }
    bool supportsBatchLearning() const override { return true; }
    bool supportsIncrementalLearning() const override { return true; }
    
    std::unordered_map<std::string, std::any> getPerformanceMetrics() const override {
        std::unordered_map<std::string, std::any> metrics;
        metrics["training_samples"] = trainingSamplesProcessed_;
        metrics["training_time"] = trainingTime_;
        metrics["current_loss"] = currentLoss_;
        metrics["best_loss"] = bestLoss_;
        return metrics;
    }
};

// Meta-Learning Algorithm (MAML - Model-Agnostic Meta-Learning)
class MetaLearningAlgorithm : public LearningAlgorithmBase<MetaLearningAlgorithm> {
public:
    MetaLearningAlgorithm(MetaLearningStrategy strategy = MetaLearningStrategy::MAML)
        : LearningAlgorithmBase<MetaLearningAlgorithm>(
            "Meta-Learning",
            LearningAlgorithmType::MetaLearning,
            "Meta-learning algorithms for rapid adaptation",
            LearningObjective::SkillAcquisition),
          strategy_(strategy) {
        config_["meta_learning_strategy"] = static_cast<int>(strategy_);
        config_["inner_loop_steps"] = 5;
        config_["outer_loop_steps"] = 1000;
        config_["learning_rate"] = 0.001f;
        config_["adaptation_rate"] = 0.01f;
        config_["num_tasks"] = 10;
        config_["support_set_size"] = 5;
        config_["query_set_size"] = 15;
    }
    
    bool train(const std::unordered_map<std::string, std::any>& trainingData) override {
        // Meta-learning implementation
        // Train on multiple tasks to learn how to learn
        return true;
    }
    
    bool validate(const std::unordered_map<std::string, std::any>& validationData) override {
        // Meta-validation implementation
        return true;
    }
    
    bool test(const std::unordered_map<std::string, std::any>& testData) override {
        // Meta-testing implementation
        return true;
    }
    
    std::any predict(const std::unordered_map<std::string, std::any>& input) override {
        // Meta-prediction implementation
        return std::any();
    }
    
    float computeLoss(const std::unordered_map<std::string, std::any>& prediction,
                      const std::unordered_map<std::string, std::any>& target) override {
        // Meta-loss computation
        return 0.0f;
    }
    
    void updateParameters(const std::unordered_map<std::string, std::any>& gradients) override {
        // Meta-parameter update implementation
    }
    
    std::unordered_map<std::string, std::any> getPerformanceMetrics() const override {
        std::unordered_map<std::string, std::any> metrics = LearningAlgorithmBase::getPerformanceMetrics();
        metrics["meta_learning_strategy"] = static_cast<int>(strategy_);
        metrics["inner_loop_steps"] = config_["inner_loop_steps"];
        metrics["num_tasks"] = config_["num_tasks"];
        return metrics;
    }
    
    bool supportsOnlineLearning() const override { return true; }
    bool supportsBatchLearning() const override { return true; }
    bool supportsIncrementalLearning() const override { return true; }
    
private:
    MetaLearningStrategy strategy_;
};

// Curriculum Learning Algorithm
class CurriculumLearningAlgorithm : public LearningAlgorithmBase<CurriculumLearningAlgorithm> {
public:
    CurriculumLearningAlgorithm() : LearningAlgorithmBase<CurriculumLearningAlgorithm>(
        "Curriculum Learning",
        LearningAlgorithmType::Curriculum,
        "Progressive learning through curriculum stages",
        LearningObjective::SkillAcquisition) {
        config_["num_stages"] = 5;
        config_["start_difficulty"] = 0.1f;
        config_["difficulty_increase"] = 0.2f;
        config_["min_difficulty"] = 0.0f;
        config_["max_difficulty"] = 1.0f;
        config_["convergence_threshold"] = 0.95f;
        config_["early_stage_samples"] = 100;
        config_["late_stage_samples"] = 1000;
    }
    
    bool configure(const std::unordered_map<std::string, std::any>& config) override {
        LearningAlgorithmBase::configure(config);
        
        // Parse curriculum stages
        if (config.find("curriculum_stages") != config.end()) {
            curriculumStages_ = std::any_cast<std::vector<CurriculumStage>>(config.at("curriculum_stages"));
        }
        
        // Calculate stage parameters
        size_t numStages = std::any_cast<size_t>(config_["num_stages"]);
        float startDifficulty = std::any_cast<float>(config_["start_difficulty"]);
        float increase = std::any_cast<float>(config_["difficulty_increase"]);
        
        curriculumStages_.clear();
        for (size_t i = 0; i < numStages; ++i) {
            CurriculumStage stage;
            stage.name = "Stage " + std::to_string(i + 1);
            stage.objective = LearningObjective::SkillAcquisition;  // Can be customized
            stage.difficulty = std::max(0.0f, std::min(1.0f, startDifficulty + i * increase));
            stage.weight = 1.0f / numStages;
            stage.duration = 100.0 * (1.0f + i * 0.5f);  // Longer stages for harder content
            stage.convergenceThreshold = config_["convergence_threshold"];
            stage.earlyStageSamples = std::any_cast<size_t>(config_["early_stage_samples"]);
            stage.lateStageSamples = std::any_cast<size_t>(config_["late_stage_samples"]);
            curriculumStages_.push_back(stage);
        }
        
        return true;
    }
    
    bool train(const std::unordered_map<std::string, std::any>& trainingData) override {
        // Curriculum learning implementation
        // Learn from progressively difficult examples
        return true;
    }
    
    bool validate(const std::unordered_map<std::string, std::any>& validationData) override {
        // Curriculum validation implementation
        return true;
    }
    
    bool test(const std::unordered_map<std::string, std::any>& testData) override {
        // Curriculum testing implementation
        return true;
    }
    
    std::any predict(const std::unordered_map<std::string, std::any>& input) override {
        // Curriculum prediction implementation
        return std::any();
    }
    
    float computeLoss(const std::unordered_map<std::string, std::any>& prediction,
                      const std::unordered_map<std::string, std::any>& target) override {
        // Curriculum loss computation
        return 0.0f;
    }
    
    void updateParameters(const std::unordered_map<std::string, std::any>& gradients) override {
        // Curriculum parameter update implementation
    }
    
    std::unordered_map<std::string, std::any> getPerformanceMetrics() const override {
        std::unordered_map<std::string, std::any> metrics = LearningAlgorithmBase::getPerformanceMetrics();
        metrics["num_stages"] = curriculumStages_.size();
        metrics["current_stage"] = currentStage_;
        metrics["current_difficulty"] = currentDifficulty_;
        return metrics;
    }
    
    bool supportsOnlineLearning() const override { return true; }
    bool supportsBatchLearning() const override { return true; }
    bool supportsIncrementalLearning() const override { return true; }
    
private:
    std::vector<CurriculumStage> curriculumStages_;
    size_t currentStage_ = 0;
    float currentDifficulty_ = 0.0f;
};

// Transfer Learning Algorithm
class TransferLearningAlgorithm : public LearningAlgorithmBase<TransferLearningAlgorithm> {
public:
    TransferLearningAlgorithm() : LearningAlgorithmBase<TransferLearningAlgorithm>(
        "Transfer Learning",
        LearningAlgorithmType::TransferLearning,
        "Learn from source tasks and adapt to target tasks",
        LearningObjective::SkillAcquisition) {
        config_["source_tasks"] = std::vector<std::string>{"classification", "regression", "clustering"};
        config_["target_tasks"] = std::vector<std::string>{"novel_task_1", "novel_task_2"};
        config_["transfer_mode"] = std::string("parameter_sharing");
        config_["adaptation_strategy"] = std::string("fine_tuning");
        config_["freeze_base_layers"] = true;
        config_["adaptation_learning_rate"] = 0.001f;
        config_["min_transferability"] = 0.7f;
    }
    
    bool train(const std::unordered_map<std::string, std::any>& trainingData) override {
        // Transfer learning implementation
        // Learn from source tasks and transfer knowledge
        return true;
    }
    
    bool validate(const std::unordered_map<std::string, std::any>& validationData) override {
        // Transfer validation implementation
        return true;
    }
    
    bool test(const std::unordered_map<std::string, std::any>& testData) override {
        // Transfer testing implementation
        return true;
    }
    
    std::any predict(const std::unordered_map<std::string, std::any>& input) override {
        // Transfer prediction implementation
        return std::any();
    }
    
    float computeLoss(const std::unordered_map<std::string, std::any>& prediction,
                      const std::unordered_map<std::string, std::any>& target) override {
        // Transfer loss computation
        return 0.0f;
    }
    
    void updateParameters(const std::unordered_map<std::string, std::any>& gradients) override {
        // Transfer parameter update implementation
    }
    
    std::unordered_map<std::string, std::any> getPerformanceMetrics() const override {
        std::unordered_map<std::string, std::any> metrics = LearningAlgorithmBase::getPerformanceMetrics();
        metrics["transfer_mode"] = config_["transfer_mode"];
        metrics["adaptation_strategy"] = config_["adaptation_strategy"];
        metrics["num_source_tasks"] = std::any_cast<std::vector<std::string>>(config_["source_tasks"]).size();
        metrics["num_target_tasks"] = std::any_cast<std::vector<std::string>>(config_["target_tasks"]).size();
        return metrics;
    }
    
    bool supportsOnlineLearning() const override { return true; }
    bool supportsBatchLearning() const override { return true; }
    bool supportsIncrementalLearning() const override { return true; }
    
    // Transfer learning specific operations
    std::vector<TransferKnowledge> getTransferKnowledge() const {
        return transferKnowledge_;
    }
    
    void addTransferKnowledge(const TransferKnowledge& knowledge) {
        transferKnowledge_.push_back(knowledge);
    }
    
private:
    std::vector<TransferKnowledge> transferKnowledge_;
};

// Learning Algorithm Engine
class LearningAlgorithmEngine {
public:
    LearningAlgorithmEngine();
    ~LearningAlgorithmEngine();
    
    // Algorithm management
    bool registerAlgorithm(std::unique_ptr<ILearningAlgorithm> algorithm);
    bool unregisterAlgorithm(const std::string& name);
    std::shared_ptr<ILearningAlgorithm> getAlgorithm(const std::string& name) const;
    std::vector<std::shared_ptr<ILearningAlgorithm>> getAlgorithms() const;
    std::vector<std::shared_ptr<ILearningAlgorithm>> getAlgorithmsByType(LearningAlgorithmType type) const;
    std::vector<std::shared_ptr<ILearningAlgorithm>> getAlgorithmsByObjective(LearningObjective objective) const;
    
    // Algorithm lifecycle
    bool activateAlgorithm(const std::string& name);
    bool deactivateAlgorithm(const std::string& name);
    bool configureAlgorithm(const std::string& name, const std::unordered_map<std::string, std::any>& config);
    
    // Training and inference
    bool trainAlgorithm(const std::string& name, const std::unordered_map<std::string, std::any>& trainingData);
    bool validateAlgorithm(const std::string& name, const std::unordered_map<std::string, std::any>& validationData);
    bool testAlgorithm(const std::string& name, const std::unordered_map<std::string, std::any>& testData);
    std::any predictAlgorithm(const std::string& name, const std::unordered_map<std::string, std::any>& input);
    
    // Meta-learning operations
    bool createMetaLearningTask(const std::string& name, const std::unordered_map<std::string, std::any>& taskConfig);
    bool executeMetaLearningEpisode(const std::string& name, MetaLearningEpisode& episode);
    std::vector<MetaLearningEpisode> getMetaLearningEpisodes(const std::string& name) const;
    
    // Curriculum learning operations
    bool createCurriculum(const std::string& name, const std::vector<CurriculumStage>& stages);
    bool advanceCurriculum(const std::string& name);
    CurriculumStage getCurrentCurriculumStage(const std::string& name) const;
    size_t getCurriculumStageIndex(const std::string& name) const;
    
    // Transfer learning operations
    bool addSourceTask(const std::string& name, const std::string& taskName);
    bool addTargetTask(const std::string& name, const std::string& taskName);
    TransferKnowledge getTransferKnowledge(const std::string& sourceTask, const std::string& targetTask) const;
    void updateTransferKnowledge(const TransferKnowledge& knowledge);
    
    // Algorithm management
    bool startAllAlgorithms();
    bool stopAllAlgorithms();
    bool shutdownAllAlgorithms();
    
    // Configuration management
    void setGlobalLearningRate(float rate);
    float getGlobalLearningRate() const;
    void setGlobalConfig(const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getGlobalConfig() const;
    
    // Statistics and monitoring
    std::unordered_map<std::string, std::any> getEngineStatistics() const;
    std::unordered_map<std::string, std::any> getAlgorithmStatistics(const std::string& name) const;
    
    // Algorithm discovery
    std::vector<std::string> discoverAvailableAlgorithms() const;
    bool loadAlgorithmFromFile(const std::string& filename);
    
    // Algorithm backup and restoration
    bool saveAlgorithm(const std::string& name, const std::string& filename) const;
    bool loadAlgorithm(const std::string& filename);
    
    // Learning algorithm utilities
    std::vector<std::string> getSupportedObjectives() const;
    std::vector<std::string> getSupportedAlgorithmsByObjective(LearningObjective objective) const;
    std::unordered_map<std::string, std::any> getAlgorithmDefaults(LearningAlgorithmType type) const;
    
    // Performance optimization
    void optimizeAlgorithms();
    void balanceWorkload();
    float getSystemLoad() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    LearningAlgorithmEngine(const LearningAlgorithmEngine&) = delete;
    LearningAlgorithmEngine& operator=(const LearningAlgorithmEngine&) = delete;
};

// Learning Algorithm Registry
class LearningAlgorithmRegistry {
public:
    static LearningAlgorithmRegistry& getInstance();
    
    // Algorithm registration
    bool registerAlgorithm(const std::string& name, std::unique_ptr<ILearningAlgorithm> algorithm);
    bool unregisterAlgorithm(const std::string& name);
    
    // Algorithm queries
    std::shared_ptr<ILearningAlgorithm> getAlgorithm(const std::string& name) const;
    std::vector<std::string> getAlgorithmNames() const;
    std::vector<std::string> getAlgorithmNamesByType(LearningAlgorithmType type) const;
    std::vector<std::string> getAlgorithmNamesByObjective(LearningObjective objective) const;
    
    // Algorithm discovery
    std::vector<std::string> discoverAvailableAlgorithms() const;
    bool loadAlgorithmFromFile(const std::string& filename);
    
    // Registry management
    void clear();
    size_t getAlgorithmCount() const;
    
private:
    LearningAlgorithmRegistry() = default;
    ~LearningAlgorithmRegistry() = default;
    
    // Prevent copying
    LearningAlgorithmRegistry(const LearningAlgorithmRegistry&) = delete;
    LearningAlgorithmRegistry& operator=(const LearningAlgorithmRegistry&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Learning Algorithm Factory
class LearningAlgorithmFactory {
public:
    static LearningAlgorithmFactory& getInstance();
    
    // Factory methods for different algorithm types
    std::unique_ptr<ILearningAlgorithm> createSupervisedAlgorithm(const std::unordered_map<std::string, std::any>& config = {});
    std::unique_ptr<ILearningAlgorithm> createMetaLearningAlgorithm(MetaLearningStrategy strategy = MetaLearningStrategy::MAML,
                                                                    const std::unordered_map<std::string, std::any>& config = {});
    std::unique_ptr<ILearningAlgorithm> createCurriculumLearningAlgorithm(const std::unordered_map<std::string, std::any>& config = {});
    std::unique_ptr<ILearningAlgorithm> createTransferLearningAlgorithm(const std::unordered_map<std::string, std::any>& config = {});
    
    // Custom algorithm creation
    std::unique_ptr<ILearningAlgorithm> createCustomAlgorithm(const std::string& name,
                                                               LearningAlgorithmType type,
                                                               const std::unordered_map<std::string, std::any>& config = {});
    
    // Algorithm discovery
    std::vector<LearningAlgorithmType> getAvailableAlgorithmTypes() const;
    std::vector<LearningObjective> getAvailableObjectives() const;
    std::unordered_map<std::string, std::any> getAlgorithmConfig(LearningAlgorithmType type) const;
    
    // Configuration utilities
    static std::unordered_map<std::string, std::any> createDefaultConfig(LearningAlgorithmType type);
    static bool validateConfig(LearningAlgorithmType type, const std::unordered_map<std::string, std::any>& config);
    
private:
    LearningAlgorithmFactory() = default;
    ~LearningAlgorithmFactory() = default;
    
    // Prevent copying
    LearningAlgorithmFactory(const LearningAlgorithmFactory&) = delete;
    LearningAlgorithmFactory& operator=(const LearningAlgorithmFactory&) = delete;
};

// Learning Algorithm Utilities
namespace utils {
    // Algorithm validation
    bool validateLearningAlgorithm(const ILearningAlgorithm& algorithm);
    bool validateLearningData(const std::unordered_map<std::string, std::any>& data);
    bool validateLearningConfig(const std::unordered_map<std::string, std::any>& config);
    
    // Algorithm performance metrics
    std::unordered_map<std::string, float> computePerformanceMetrics(const ILearningAlgorithm& algorithm,
                                                                       const std::unordered_map<std::string, std::any>& data);
    
    // Learning rate adaptation
    float adaptLearningRate(const LearningRateAdapter& adapter, float currentRate,
                            const std::unordered_map<std::string, std::any>& metrics);
    
    // Meta-learning utilities
    MetaLearningEpisode createMetaLearningEpisode(uint64_t episodeId, LearningObjective objective,
                                                  const std::unordered_map<std::string, std::any>& context);
    float computeMetaLearningReward(const MetaLearningEpisode& episode);
    
    // Curriculum learning utilities
    bool isCurriculumStageCompleted(const CurriculumStage& stage,
                                     const std::unordered_map<std::string, std::any>& metrics);
    float calculateCurriculumProgress(const std::vector<CurriculumStage>& stages, size_t currentStage);
    
    // Transfer learning utilities
    float computeTransferability(const TransferKnowledge& knowledge);
    float computeAdaptationCost(const TransferKnowledge& knowledge);
    
    // Algorithm serialization
    std::string serializeAlgorithm(const ILearningAlgorithm& algorithm);
    bool deserializeAlgorithm(ILearningAlgorithm& algorithm, const std::string& serialized);
    
    // Thread-safe utilities
    template<typename Func>
    auto threadSafeLearningOperation(Func&& func) -> decltype(func()) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        return func();
    }
}

} // namespace learning
} // namespace nlm

// Learning algorithm registration macros
#define REGISTER_LEARNING_ALGORITHM(AlgorithmClass, AlgorithmName) \
    class AlgorithmName##_registrar { \
    public: \
        AlgorithmName##_registrar() { \
            nlm::learning::LearningAlgorithmRegistry::getInstance().registerAlgorithm(#AlgorithmName, std::make_unique<AlgorithmClass>()); \
        } \
    }; \
    static AlgorithmName##_registrar AlgorithmName##_instance;

#define LEARNING_OBJECTIVE(Name) \
    nlm::learning::LearningObjective::Name

#define LEARNING_ALGORITHM_TYPE(Name) \
    nlm::learning::LearningAlgorithmType::Name

namespace nlm {
namespace learning {
    // Global instances
    extern LearningAlgorithmRegistry& getLearningAlgorithmRegistry();
    extern LearningAlgorithmFactory& getLearningAlgorithmFactory();
    
    // Convenience functions
    template<typename... Args>
    std::unique_ptr<ILearningAlgorithm> createLearningAlgorithm(LearningAlgorithmType type, Args&&... args);
    
    std::vector<std::unique_ptr<ILearningAlgorithm>> createMultipleLearningAlgorithms(
        const std::vector<LearningAlgorithmType>& types);
}
} // namespace nlm

// Learning algorithm implementation
namespace nlm {
namespace learning {

struct LearningAlgorithmEngine::Impl {
    std::unordered_map<std::string, std::shared_ptr<ILearningAlgorithm>> algorithms;
    std::unordered_map<std::string, bool> algorithmActive;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> algorithmConfigs;
    
    std::unordered_map<std::string, std::vector<MetaLearningEpisode>> metaLearningEpisodes;
    std::unordered_map<std::string, std::vector<CurriculumStage>> curriculums;
    std::unordered_map<std::string, std::unordered_map<std::string, TransferKnowledge>> transferKnowledge;
    
    float globalLearningRate = 0.001f;
    std::unordered_map<std::string, std::any> globalConfig;
    
    mutable std::mutex algorithmsMutex;
    mutable std::mutex metaLearningMutex;
    mutable std::mutex curriculumMutex;
    mutable std::mutex transferMutex;
    mutable std::mutex configMutex;
    
    // Statistics
    size_t totalAlgorithmsLoaded = 0;
    size_t totalTrainingSamplesProcessed = 0;
    double totalTrainingTime = 0.0;
    
    // Performance tracking
    std::unordered_map<std::string, double> algorithmExecutionTimes;
    std::unordered_map<std::string, size_t> algorithmSampleCounts;
};

LearningAlgorithmEngine::LearningAlgorithmEngine() : pImpl(std::make_unique<Impl>()) {}

LearningAlgorithmEngine::~LearningAlgorithmEngine() {
    for (auto& pair : pImpl->algorithms) {
        if (pair.second) {
            pair.second->shutdown();
        }
    }
}

bool LearningAlgorithmEngine::registerAlgorithm(std::unique_ptr<ILearningAlgorithm> algorithm) {
    if (!algorithm) {
        return false;
    }
    
    std::string name = algorithm->getName();
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    // Check if algorithm already exists
    if (pImpl->algorithms.find(name) != pImpl->algorithms.end()) {
        return false;  // Algorithm already registered
    }
    
    // Initialize algorithm
    if (!algorithm->initialize()) {
        return false;
    }
    
    pImpl->algorithms[name] = std::move(algorithm);
    pImpl->algorithmActive[name] = false;
    pImpl->totalAlgorithmsLoaded++;
    
    return true;
}

bool LearningAlgorithmEngine::unregisterAlgorithm(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    // Deactivate algorithm if active
    if (pImpl->algorithmActive[name]) {
        it->second->deactivate();
    }
    
    // Shutdown algorithm
    if (!it->second->shutdown()) {
        return false;
    }
    
    pImpl->algorithms.erase(it);
    pImpl->algorithmActive.erase(name);
    pImpl->algorithmConfigs.erase(name);
    
    return true;
}

std::shared_ptr<ILearningAlgorithm> LearningAlgorithmEngine::getAlgorithm(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    return it != pImpl->algorithms.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<ILearningAlgorithm>> LearningAlgorithmEngine::getAlgorithms() const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    std::vector<std::shared_ptr<ILearningAlgorithm>> algorithms;
    algorithms.reserve(pImpl->algorithms.size());
    
    for (const auto& pair : pImpl->algorithms) {
        algorithms.push_back(pair.second);
    }
    
    return algorithms;
}

std::vector<std::shared_ptr<ILearningAlgorithm>> LearningAlgorithmEngine::getAlgorithmsByType(LearningAlgorithmType type) const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    std::vector<std::shared_ptr<ILearningAlgorithm>> algorithms;
    
    for (const auto& pair : pImpl->algorithms) {
        if (pair.second && pair.second->getType() == type) {
            algorithms.push_back(pair.second);
        }
    }
    
    return algorithms;
}

std::vector<std::shared_ptr<ILearningAlgorithm>> LearningAlgorithmEngine::getAlgorithmsByObjective(LearningObjective objective) const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    std::vector<std::shared_ptr<ILearningAlgorithm>> algorithms;
    
    for (const auto& pair : pImpl->algorithms) {
        if (pair.second && pair.second->getPrimaryObjective() == objective) {
            algorithms.push_back(pair.second);
        }
    }
    
    return algorithms;
}

bool LearningAlgorithmEngine::activateAlgorithm(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    if (pImpl->algorithmActive[name]) {
        return true;  // Already active
    }
    
    if (!it->second->activate()) {
        return false;
    }
    
    pImpl->algorithmActive[name] = true;
    return true;
}

bool LearningAlgorithmEngine::deactivateAlgorithm(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    if (!pImpl->algorithmActive[name]) {
        return true;  // Already inactive
    }
    
    if (!it->second->deactivate()) {
        return false;
    }
    
    pImpl->algorithmActive[name] = false;
    return true;
}

bool LearningAlgorithmEngine::configureAlgorithm(const std::string& name,
                                                 const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    if (!it->second->configure(config)) {
        return false;
    }
    
    pImpl->algorithmConfigs[name] = config;
    return true;
}

bool LearningAlgorithmEngine::trainAlgorithm(const std::string& name,
                                             const std::unordered_map<std::string, std::any>& trainingData) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    if (!pImpl->algorithmActive[name]) {
        return false;  // Algorithm must be active
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = it->second->train(trainingData);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    pImpl->totalTrainingTime += std::chrono::duration<double>(endTime - startTime).count();
    
    return success;
}

bool LearningAlgorithmEngine::validateAlgorithm(const std::string& name,
                                                const std::unordered_map<std::string, std::any>& validationData) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    return it->second->validate(validationData);
}

bool LearningAlgorithmEngine::testAlgorithm(const std::string& name,
                                            const std::unordered_map<std::string, std::any>& testData) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    return it->second->test(testData);
}

std::any LearningAlgorithmEngine::predictAlgorithm(const std::string& name,
                                                   const std::unordered_map<std::string, std::any>& input) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return std::any();  // Algorithm not found
    }
    
    return it->second->predict(input);
}

void LearningAlgorithmEngine::setGlobalLearningRate(float rate) {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    pImpl->globalLearningRate = rate;
}

float LearningAlgorithmEngine::getGlobalLearningRate() const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    return pImpl->globalLearningRate;
}

void LearningAlgorithmEngine::setGlobalConfig(const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->globalConfig = config;
}

std::unordered_map<std::string, std::any> LearningAlgorithmEngine::getGlobalConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->globalConfig;
}

std::unordered_map<std::string, std::any> LearningAlgorithmEngine::getEngineStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    std::unordered_map<std::string, std::any> stats;
    stats["total_algorithms_loaded"] = pImpl->totalAlgorithmsLoaded;
    stats["total_training_samples"] = pImpl->totalTrainingSamplesProcessed;
    stats["total_training_time"] = pImpl->totalTrainingTime;
    
    if (pImpl->totalTrainingTime > 0) {
        stats["average_training_time"] = pImpl->totalTrainingTime / pImpl->totalAlgorithmsLoaded;
    }
    
    return stats;
}

std::unordered_map<std::string, std::any> LearningAlgorithmEngine::getAlgorithmStatistics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    std::unordered_map<std::string, std::any> stats;
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return stats;  // Algorithm not found
    }
    
    stats["active"] = pImpl->algorithmActive[name];
    stats["execution_time"] = pImpl->algorithmExecutionTimes[name];
    stats["sample_count"] = pImpl->algorithmSampleCounts[name];
    
    if (pImpl->algorithmSampleCounts[name] > 0) {
        stats["average_execution_time"] = pImpl->algorithmExecutionTimes[name] / pImpl->algorithmSampleCounts[name];
    }
    
    // Get algorithm-specific statistics
    auto algorithmStats = it->second->getPerformanceMetrics();
    stats.insert(algorithmStats.begin(), algorithmStats.end());
    
    return stats;
}

std::vector<std::string> LearningAlgorithmEngine::discoverAvailableAlgorithms() const {
    std::vector<std::string> algorithms;
    
    // Check registry for built-in algorithms
    auto& registry = LearningAlgorithmRegistry::getInstance();
    algorithms = registry.getAlgorithmNames();
    
    // Could also scan filesystem for external algorithms
    // For now, just return built-in algorithms
    
    return algorithms;
}

bool LearningAlgorithmEngine::loadAlgorithmFromFile(const std::string& filename) {
    // For now, placeholder implementation
    // Would need to implement algorithm file loading from serialized format
    return false;
}

bool LearningAlgorithmEngine::saveAlgorithm(const std::string& name, const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->algorithmsMutex);
    
    auto it = pImpl->algorithms.find(name);
    if (it == pImpl->algorithms.end()) {
        return false;  // Algorithm not found
    }
    
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Serialize algorithm
        std::string serialized = it->second->getName() + "\n";  // Simple serialization
        file.write(serialized.c_str(), serialized.size());
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool LearningAlgorithmEngine::loadAlgorithm(const std::string& filename) {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple deserialization
        std::string line;
        if (!std::getline(file, line)) {
            return false;
        }
        
        // For now, just create a placeholder algorithm
        auto algorithm = std::make_unique<SupervisedLearningAlgorithm>();
        
        return registerAlgorithm(std::move(algorithm));
        
    } catch (const std::exception&) {
        return false;
    }
}

LearningAlgorithmRegistry& LearningAlgorithmRegistry::getInstance() {
    static LearningAlgorithmRegistry instance;
    return instance;
}

bool LearningAlgorithmRegistry::registerAlgorithm(const std::string& name, std::unique_ptr<ILearningAlgorithm> algorithm) {
    if (!algorithm || name.empty()) {
        return false;
    }
    
    // For now, placeholder - would need to access the engine to register
    return true;
}

bool LearningAlgorithmRegistry::unregisterAlgorithm(const std::string& name) {
    // For now, placeholder - would need to access the engine to unregister
    return true;
}

std::shared_ptr<ILearningAlgorithm> LearningAlgorithmRegistry::getAlgorithm(const std::string& name) const {
    // For now, placeholder - would need to access the engine
    return nullptr;
}

std::vector<std::string> LearningAlgorithmRegistry::getAlgorithmNames() const {
    // For now, placeholder - would need to access the engine
    return std::vector<std::string>{"Supervised", "MetaLearning", "Curriculum", "Transfer"};
}

std::vector<std::string> LearningAlgorithmRegistry::getAlgorithmNamesByType(LearningAlgorithmType type) const {
    // For now, placeholder - would need to access the engine
    std::vector<std::string> names;
    
    switch (type) {
        case LearningAlgorithmType::Supervised:
            names = {"Supervised"};
            break;
        case LearningAlgorithmType::MetaLearning:
            names = {"MetaLearning"};
            break;
        case LearningAlgorithmType::Curriculum:
            names = {"Curriculum"};
            break;
        case LearningAlgorithmType::TransferLearning:
            names = {"Transfer"};
            break;
        default:
            break;
    }
    
    return names;
}

std::vector<std::string> LearningAlgorithmRegistry::discoverAvailableAlgorithms() const {
    // For now, placeholder
    return getAlgorithmNames();
}

bool LearningAlgorithmRegistry::loadAlgorithmFromFile(const std::string& filename) {
    // For now, placeholder implementation
    return false;
}

void LearningAlgorithmRegistry::clear() {
    // For now, placeholder
}

size_t LearningAlgorithmRegistry::getAlgorithmCount() const {
    // For now, placeholder
    return 4;
}

LearningAlgorithmRegistry::LearningAlgorithmRegistry() : pImpl(std::make_unique<Impl>()) {}

LearningAlgorithmRegistry::~LearningAlgorithmRegistry() = default;

LearningAlgorithmFactory& LearningAlgorithmFactory::getInstance() {
    static LearningAlgorithmFactory instance;
    return instance;
}

std::unique_ptr<ILearningAlgorithm> LearningAlgorithmFactory::createSupervisedAlgorithm(const std::unordered_map<std::string, std::any>& config) {
    auto algorithm = std::make_unique<SupervisedLearningAlgorithm>();
    if (!config.empty()) {
        algorithm->configure(config);
    }
    return algorithm;
}

std::unique_ptr<ILearningAlgorithm> LearningAlgorithmFactory::createMetaLearningAlgorithm(MetaLearningStrategy strategy,
                                                                                     const std::unordered_map<std::string, std::any>& config) {
    auto algorithm = std::make_unique<MetaLearningAlgorithm>(strategy);
    if (!config.empty()) {
        algorithm->configure(config);
    }
    return algorithm;
}

std::unique_ptr<ILearningAlgorithm> LearningAlgorithmFactory::createCurriculumLearningAlgorithm(const std::unordered_map<std::string, std::any>& config) {
    auto algorithm = std::make_unique<CurriculumLearningAlgorithm>();
    if (!config.empty()) {
        algorithm->configure(config);
    }
    return algorithm;
}

std::unique_ptr<ILearningAlgorithm> LearningAlgorithmFactory::createTransferLearningAlgorithm(const std::unordered_map<std::string, std::any>& config) {
    auto algorithm = std::make_unique<TransferLearningAlgorithm>();
    if (!config.empty()) {
        algorithm->configure(config);
    }
    return algorithm;
}

std::unique_ptr<ILearningAlgorithm> LearningAlgorithmFactory::createCustomAlgorithm(const std::string& name,
                                                                                LearningAlgorithmType type,
                                                                                const std::unordered_map<std::string, std::any>& config) {
    // Create custom algorithm - this would need a factory system
    // For now, return a placeholder
    return nullptr;
}

std::vector<LearningAlgorithmType> LearningAlgorithmFactory::getAvailableAlgorithmTypes() const {
    return {LearningAlgorithmType::Supervised,
            LearningAlgorithmType::MetaLearning,
            LearningAlgorithmType::Curriculum,
            LearningAlgorithmType::TransferLearning,
            LearningAlgorithmType::Evolutionary,
            LearningAlgorithmType::Reinforcement,
            LearningAlgorithmType::Unsupervised,
            LearningAlgorithmType::Deep,
            LearningAlgorithmType::Adaptive,
            LearningAlgorithmType::SelfSupervised,
            LearningAlgorithmType::FewShot};
}

std::vector<LearningObjective> LearningAlgorithmFactory::getAvailableObjectives() const {
    return {LearningObjective::Classification,
            LearningObjective::Regression,
            LearningObjective::Clustering,
            LearningObjective::AnomalyDetection,
            LearningObjective::ReinforcementLearning,
            LearningObjective::PolicyOptimization,
            LearningObjective::ValueEstimation,
            LearningObjective::SequencePrediction,
            LearningObjective::PatternRecognition,
            LearningObjective::ConceptFormation,
            LearningObjective::SkillAcquisition};
}

std::unordered_map<std::string, std::any> LearningAlgorithmFactory::getAlgorithmConfig(LearningAlgorithmType type) const {
    // Return default configuration for algorithm type
    std::unordered_map<std::string, std::any> config;
    
    switch (type) {
        case LearningAlgorithmType::Supervised:
            config["model_type"] = std::string("neural_network");
            config["optimizer"] = std::string("adam");
            config["batch_size"] = 32;
            config["epochs"] = 100;
            config["learning_rate"] = 0.001f;
            break;
        case LearningAlgorithmType::MetaLearning:
            config["meta_learning_strategy"] = static_cast<int>(MetaLearningStrategy::MAML);
            config["inner_loop_steps"] = 5;
            config["outer_loop_steps"] = 1000;
            config["learning_rate"] = 0.001f;
            break;
        case LearningAlgorithmType::Curriculum:
            config["num_stages"] = 5;
            config["start_difficulty"] = 0.1f;
            config["difficulty_increase"] = 0.2f;
            break;
        case LearningAlgorithmType::TransferLearning:
            config["source_tasks"] = std::vector<std::string>{"classification", "regression"};
            config["target_tasks"] = std::vector<std::string>{"novel_task"};
            break;
        default:
            break;
    }
    
    return config;
}

std::unordered_map<std::string, std::any> LearningAlgorithmFactory::createDefaultConfig(LearningAlgorithmType type) {
    return LearningAlgorithmFactory::getInstance().getAlgorithmConfig(type);
}

bool LearningAlgorithmFactory::validateConfig(LearningAlgorithmType type, const std::unordered_map<std::string, std::any>& config) {
    // Basic validation
    return !config.empty();
}

LearningAlgorithmFactory::LearningAlgorithmFactory() = default;

LearningAlgorithmFactory::~LearningAlgorithmFactory() = default;

LearningAlgorithmRegistry& getLearningAlgorithmRegistry() {
    return LearningAlgorithmRegistry::getInstance();
}

LearningAlgorithmFactory& getLearningAlgorithmFactory() {
    return LearningAlgorithmFactory::getInstance();
}

template<typename... Args>
std::unique_ptr<ILearningAlgorithm> createLearningAlgorithm(LearningAlgorithmType type, Args&&... args) {
    // This would need to implement a generic algorithm creation system
    // For now, return a placeholder
    return nullptr;
}

std::vector<std::unique_ptr<ILearningAlgorithm>> createMultipleLearningAlgorithms(
    const std::vector<LearningAlgorithmType>& types) {
    std::vector<std::unique_ptr<ILearningAlgorithm>> algorithms;
    algorithms.reserve(types.size());
    
    for (const auto& type : types) {
        auto algorithm = LearningAlgorithmFactory::getInstance().createSupervisedAlgorithm(); // Placeholder
        if (algorithm) {
            algorithms.push_back(std::move(algorithm));
        }
    }
    
    return algorithms;
}

} // namespace learning
} // namespace nlm

// Learning algorithm registration
namespace nlm {
namespace learning {
    // Register the built-in algorithms
    REGISTER_LEARNING_ALGORITHM(SupervisedLearningAlgorithm, SupervisedAlgorithm);
    REGISTER_LEARNING_ALGORITHM(MetaLearningAlgorithm, MetaLearningAlgorithm);
    REGISTER_LEARNING_ALGORITHM(CurriculumLearningAlgorithm, CurriculumLearningAlgorithm);
    REGISTER_LEARNING_ALGORITHM(TransferLearningAlgorithm, TransferLearningAlgorithm);
}
} // namespace learning

// Learning algorithm convenience functions
namespace nlm {
namespace learning {
    template<typename... Args>
    std::unique_ptr<ILearningAlgorithm> createLearningAlgorithm(LearningAlgorithmType type, Args&&... args) {
        return LearningAlgorithmFactory::getInstance().createSupervisedAlgorithm(); // Placeholder
    }
}
} // namespace nlm

// Learning algorithm macros
#define REGISTER_LEARNING_ALGORITHM(AlgorithmClass, AlgorithmName) \
    class AlgorithmName##_registrar { \
    public: \
        AlgorithmName##_registrar() { \
            nlm::learning::LearningAlgorithmRegistry::getInstance().registerAlgorithm(#AlgorithmName, std::make_unique<AlgorithmClass>()); \
        } \
    }; \
    static AlgorithmName##_registrar AlgorithmName##_instance;

#define LEARNING_OBJECTIVE(Name) \
    nlm::learning::LearningObjective::Name

#define LEARNING_ALGORITHM_TYPE(Name) \
    nlm::learning::LearningAlgorithmType::Name

#define META_LEARNING_STRATEGY(Name) \
    nlm::learning::MetaLearningStrategy::Name

#define LEARNING_RATE_ADAPTER(Name) \
    nlm::learning::LearningRateAdapter::Name