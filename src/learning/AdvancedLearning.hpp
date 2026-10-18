#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <algorithm>
#include <random>
#include <cmath>
#include <nlohmann/json.hpp>

namespace nlm {

// Learning system configuration
struct LearningConfig {
    std::string learningAlgorithm = "td_learning"; // td_learning, q_learning, policy_gradient, reinforcement_learning
    float learningRate = 0.01f;
    float discountFactor = 0.99f;
    float explorationRate = 0.1f;
    float epsilonDecay = 0.995f;
    float minExplorationRate = 0.01f;
    size_t experienceReplaySize = 10000;
    size_t batchSize = 32;
    bool enablePreTraining = false;
    bool enableTransferLearning = false;
    std::string targetDomain = "";
    float adaptationRate = 0.1f;
    bool enableMetaLearning = false;
    float metaLearningRate = 0.001f;
    bool enableActiveLearning = false;
    float acquisitionFunctionThreshold = 0.5f;
    bool enableRewardShaping = true;
    float rewardScale = 1.0f;
    bool enableIntrinsicMotivation = false;
    float curiosityWeight = 0.5f;
    float noveltyWeight = 0.3f;
    float predictionErrorWeight = 0.2f;
};

// Learning state for a single agent
struct LearningState {
    std::vector<float> stateVector;
    ActionType action;
    float reward;
    std::vector<float> nextState;
    bool done;
    float timestamp;
    std::unordered_map<std::string, float> metadata;
    std::string episodeId;
    size_t stepInEpisode;
};

// Q-table for Q-learning
class QTable {
public:
    QTable(size_t stateSize, size_t actionCount);
    ~QTable();
    
    void update(const std::vector<float>& state, ActionType action, float reward,
                const std::vector<float>& nextState, float alpha = 0.1f, float gamma = 0.99f);
    float getQValue(const std::vector<float>& state, ActionType action) const;
    ActionType getBestAction(const std::vector<float>& state) const;
    std::vector<float> getStateEncoding(const std::vector<float>& state) const;
    
    void reset();
    size_t getStateCount() const;
    size_t getActionCount() const;
    
    nlohmann::json exportToJSON() const;
    void importFromJSON(const nlohmann::json& data);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Policy gradient implementation
class PolicyGradient {
public:
    PolicyGradient(size_t stateSize, size_t actionCount);
    ~PolicyGradient();
    
    void update(const LearningState& state, float advantage, float learningRate = 0.01f);
    ActionType getAction(const std::vector<float>& state, float epsilon = 0.1f) const;
    std::vector<float> getActionProbabilities(const std::vector<float>& state) const;
    float getAdvantage(const LearningState& state) const;
    
    void reset();
    void setPolicyNetwork(const std::string& networkConfig);
    
    nlohmann::json exportToJSON() const;
    void importFromJSON(const nlohmann::json& data);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Temporal Difference Learning
class TDLearning {
public:
    TDLearning(size_t stateSize, size_t actionCount);
    ~TDLearning();
    
    void update(const LearningState& state, float learningRate = 0.01f);
    float estimateValue(const std::vector<float>& state, ActionType action) const;
    float estimatePolicyValue(const std::vector<float>& state) const;
    
    void reset();
    void setEligibilityTraceDecay(float decay);
    
    nlohmann::json exportToJSON() const;
    void importFromJSON(const nlohmann::json& data);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experience replay buffer
class ExperienceReplay {
public:
    ExperienceReplay(size_t capacity = 10000);
    ~ExperienceReplay();
    
    void addSample(const LearningState& state);
    std::vector<LearningState> sampleBatch(size_t batchSize) const;
    size_t size() const;
    void clear();
    bool isFull() const;
    
    void setPrioritizedSampling(bool enable, float alpha = 0.6f);
    void updatePriority(size_t index, float priority);
    std::vector<LearningState> samplePrioritizedBatch(size_t batchSize) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Transfer learning manager
class TransferLearningManager {
public:
    TransferLearningManager();
    ~TransferLearningManager();
    
    // Source domains management
    void addSourceDomain(const std::string& domainName, const nlohmann::json& domainData);
    void removeSourceDomain(const std::string& domainName);
    bool hasSourceDomain(const std::string& domainName) const;
    
    // Knowledge transfer
    void transferKnowledge(const std::string& targetDomain);
    void adaptToDomain(const std::string& domainName, float adaptationRate = 0.1f);
    
    // Knowledge extraction
    nlohmann::json extractTaskFeatures(const LearningState& state) const;
    float measureDomainSimilarity(const std::string& domain1, const std::string& domain2) const;
    
    // Transfer statistics
    struct TransferStats {
        size_t totalTransfers = 0;
        size_t successfulTransfers = 0;
        float averageTransferSuccess = 0.0f;
        std::unordered_map<std::string, float> domainAdaptationRates;
        std::chrono::steady_clock::time_point lastTransfer;
    };
    
    TransferStats getStats() const;
    void resetStats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Meta-learning system
class MetaLearning {
public:
    MetaLearning();
    ~MetaLearning();
    
    // Learning strategy optimization
    void optimizeLearningStrategy(const std::vector<LearningState>& experiences);
    float evaluateLearningStrategy(const std::vector<LearningState>& experiences) const;
    
    // Parameter optimization
    void optimizeParameters(const std::vector<std::function<float(const LearningState&)>>& parameters);
    std::vector<float> getOptimizedParameters() const;
    
    // Meta-policy learning
    ActionType getMetaAction(const std::vector<float>& learningState) const;
    void updateMetaPolicy(const std::vector<LearningState>& experiences, float learningRate = 0.001f);
    
    // MAML-like implementation
    void implementMAML(const std::vector<LearningState>& fastLearningData,
                      const std::vector<LearningState>& metaLearningData);
    
    // Meta-learning statistics
    struct MetaLearningStats {
        size_t totalMetaUpdates = 0;
        float averageMetaPerformance = 0.0f;
        float convergenceRate = 0.0f;
        std::chrono::steady_clock::time_point lastUpdate;
    };
    
    MetaLearningStats getStats() const;
    void resetStats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Active learning system
class ActiveLearning {
public:
    ActiveLearning();
    ~ActiveLearning();
    
    // Sample selection
    std::vector<size_t> selectSamples(const std::vector<LearningState>& availableSamples,
                                     size_t batchSize, const std::string& acquisitionFunction = "uncertainty");
    
    // Uncertainty sampling
    float calculateUncertainty(const LearningState& state) const;
    float calculateMarginSampling(const LearningState& state) const;
    float calculateDiversitySampling(const std::vector<LearningState>& samples, size_t newSampleIndex) const;
    
    // Cost-sensitive learning
    void setCostFunction(std::function<float(const LearningState&)> costFunction);
    float calculateSampleCost(const LearningState& state) const;
    
    // Active learning statistics
    struct ActiveLearningStats {
        size_t totalSamplesEvaluated = 0;
        size_t samplesSelectedForTraining = 0;
        float averageAcquisitionScore = 0.0f;
        float averageSampleCost = 0.0f;
        std::chrono::steady_clock::time_point lastSelection;
    };
    
    ActiveLearningStats getStats() const;
    void resetStats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reinforcement learning integrator
class ReinforcementLearning {
public:
    ReinforcementLearning();
    ~ReinforcementLearning();
    
    // RL lifecycle
    void initialize(const LearningConfig& config);
    ActionType selectAction(const std::vector<float>& state);
    void update(const LearningState& state);
    void endEpisode();
    
    // RL algorithms
    void trainTDLearning(const std::vector<LearningState>& experiences);
    void trainQLearning(const std::vector<LearningState>& experiences);
    void trainPolicyGradient(const std::vector<LearningState>& experiences);
    
    // RL management
    void reset();
    bool isTraining() const;
    void setTraining(bool training);
    
    // Model management
    void saveModel(const std::string& filename) const;
    void loadModel(const std::string& filename);
    
    // RL statistics
    struct RLStats {
        size_t totalSteps = 0;
        size_t totalEpisodes = 0;
        float totalReward = 0.0f;
        float averageReward = 0.0f;
        float explorationRate = 0.1f;
        float learningRate = 0.01f;
        float discountFactor = 0.99f;
        std::chrono::steady_clock::time_point lastUpdate;
    };
    
    RLStats getStats() const;
    void resetStats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced learning system - main class
class AdvancedLearning {
public:
    AdvancedLearning();
    ~AdvancedLearning();
    
    // Configuration
    void configure(const LearningConfig& config);
    LearningConfig getConfig() const;
    
    // Training interface
    void startTraining(const std::vector<LearningState>& initialExperiences);
    void trainStep(const LearningState& state);
    void stopTraining();
    bool isTraining() const;
    
    // Learning algorithm selection
    void setLearningAlgorithm(const std::string& algorithm);
    std::vector<std::string> getAvailableAlgorithms() const;
    std::string getCurrentAlgorithm() const;
    
    // Transfer learning
    void enableTransferLearning(const std::string& sourceDomain);
    void disableTransferLearning();
    bool isTransferLearningEnabled() const;
    
    // Meta-learning
    void enableMetaLearning();
    void disableMetaLearning();
    bool isMetaLearningEnabled() const;
    
    // Active learning
    void enableActiveLearning();
    void disableActiveLearning();
    bool isActiveLearningEnabled() const;
    
    // Reward shaping
    void setRewardFunction(std::function<float(const LearningState&)> rewardFunction);
    float computeReward(const LearningState& state) const;
    
    // Experience management
    void addExperience(const LearningState& state);
    std::vector<LearningState> getRecentExperiences(size_t count = 1000) const;
    void clearExperiences();
    
    // Model persistence
    void saveModel(const std::string& filename) const;
    void loadModel(const std::string& filename);
    
    // Learning statistics
    struct LearningStats {
        size_t totalSteps = 0;
        size_t totalEpisodes = 0;
        float totalReward = 0.0f;
        float averageReward = 0.0f;
        float learningProgress = 0.0f;
        std::chrono::steady_clock::time_point sessionStart;
        std::chrono::steady_clock::time_point lastStepTime;
    };
    
    LearningStats getStats() const;
    void resetStats();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    
    // Callback system
    void addTrainingCallback(std::function<void(const LearningState&)> callback);
    void removeTrainingCallback(std::function<void(const LearningState&)> callback);
    
    // Learning analysis
    nlohmann::json analyzeLearningProgress() const;
    float calculateConvergenceRate() const;
    std::vector<float> getLearningCurve() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Learning utilities
namespace LearningUtils {
    // State preprocessing
    std::vector<float> normalizeState(const std::vector<float>& state,
                                      const std::vector<float>& mean,
                                      const std::vector<float>& stddev);
    std::vector<float> discretizeState(const std::vector<float>& state,
                                      const std::vector<float>& bins);
    std::vector<float> encodeState(const std::vector<float>& state,
                                  const std::string& encoding = "one_hot");
    
    // Reward processing
    float clipReward(float reward, float minReward = -1.0f, float maxReward = 1.0f);
    float scaleReward(float reward, float targetRange = 1.0f);
    float shapeReward(float reward, const LearningState& state,
                     std::function<float(const LearningState&)> shapingFunction);
    
    // Experience generation
    LearningState createExperience(const std::vector<float>& state,
                                   ActionType action,
                                   float reward,
                                   const std::vector<float>& nextState,
                                   bool done,
                                   const std::string& episodeId = "",
                                   size_t stepInEpisode = 0);
    
    // Statistics calculation
    float calculateStdDev(const std::vector<float>& values);
    float calculateMSE(const std::vector<float>& predicted, const std::vector<float>& actual);
    std::vector<float> runningAverage(const std::vector<float>& values, size_t windowSize = 100);
    
    // Randomness
    void setRandomSeed(unsigned int seed);
    std::mt19937& getRandomGenerator();
    
    // Progress tracking
    class LearningProgressTracker {
    public:
        LearningProgressTracker();
        ~LearningProgressTracker();
        
        void recordStep(float reward, float valueEstimate = 0.0f);
        float getAverageReward(size_t windowSize = 100) const;
        float getProgress() const; // 0.0 to 1.0
        bool isConverged(float threshold = 0.95f) const;
        
        std::vector<float> getRewardHistory() const;
        size_t getTotalSteps() const;
        
        void reset();
        
        struct ProgressMetrics {
            float currentReward = 0.0f;
            float averageReward = 0.0f;
            float progress = 0.0f;
            bool converged = false;
            size_t steps = 0;
        };
        
        ProgressMetrics getMetrics() const;
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

} // namespace nlm
