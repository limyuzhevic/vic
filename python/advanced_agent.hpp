#pragma once

// Advanced agent features for NLM
// Provides reinforcement learning integration, experience replay and learning,
// custom reward functions, and advanced perception processing

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <deque>
#include <chrono>
#include <random>

#include "../src/agent/AgentBrain.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/Action.hpp"

namespace nlm {
namespace python {

// Experience replay structure
struct Experience {
    double timestamp;
    std::vector<float> state;
    int action;
    double reward;
    std::vector<float> nextState;
    bool done;
    std::map<std::string, double> metadata;
    
    Experience() : timestamp(0.0), action(-1), reward(0.0), done(false) {}
    
    Experience(double ts, const std::vector<float>& s, int a, double r,
               const std::vector<float>& ns, bool d)
        : timestamp(ts), state(s), action(a), reward(r), nextState(ns), done(d) {}
};

// Reinforcement learning algorithm types
enum class RLAlgorithm {
    Q_LEARNING,
    SARSA,
    DEEP_Q_LEARNING,
    POLICY_GRADIENT,
    ACOUSTIC_SEMI_GRADIENT,
    EXPECTATION_MAXIMIZATION,
    HORIZON_BASED
};

// Reward function signature
using RewardFunction = std::function<double(const std::vector<float>&, int, 
                                            const std::vector<float>&)>;

// Neural network structure for reinforcement learning
struct NeuralNetwork {
    std::string name;
    std::vector<size_t> layerSizes;
    std::vector<std::string> activationFunctions;
    std::vector<std::vector<float>> weights;
    std::vector<std::vector<float>> biases;
    std::map<std::string, double> hyperparameters;
    std::string architecture;
};

// Custom reward function metadata
struct RewardFunctionInfo {
    std::string name;
    std::string description;
    std::vector<std::string> parameters;
    std::string formula;
    bool isBuiltIn;
    std::chrono::system_clock::timePoint createdAt;
    std::map<std::string, std::string> metadata;
};

// Advanced agent class
class AdvancedAgent {
public:
    // Reinforcement learning integration
    struct RLSimulationParameters {
        RLAlgorithm algorithm;
        double learningRate;
        double discountFactor;
        double explorationRate;
        double explorationDecayRate;
        size_t replayBufferSize;
        size_t batchSize;
        double targetNetworkUpdateRate;
        std::string optimizer;
        double epsilon;
        double alpha;
        double beta;
        std::string policyType;
        double temperature;
        size_t horizonSteps;
        double lambda;
        double gamma;
        size_t updateFrequency;
        size_t gradientSteps;
    };
    
    // Experience replay configuration
    struct ExperienceReplayConfig {
        size_t maxBufferSize;
        size_t batchSize;
        double importanceSamplingExponent;
        bool prioritizedReplay;
        double alpha;
        double beta;
        std::string samplingStrategy;
        bool storeTransitions;
        bool useIsWeights;
    };
    
    // Constructor
    AdvancedAgent(std::shared_ptr<AgentBrain> agentBrain, 
                  const RLSimulationParameters& rlParams);
    
    // Destructor
    ~AdvancedAgent();
    
    // Disable copying, enable moving
    AdvancedAgent(const AdvancedAgent&) = delete;
    AdvancedAgent& operator=(const AdvancedAgent&) = delete;
    AdvancedAgent(AdvancedAgent&&) noexcept;
    AdvancedAgent& operator=(AdvancedAgent&&) noexcept;
    
    // Core reinforcement learning functionality
    void startLearning();
    void stopLearning();
    bool isLearning() const;
    
    // Experience collection and replay
    void recordExperience(const Experience& experience);
    bool addExperience(const std::vector<float>& state, int action, 
                      double reward, const std::vector<float>& nextState, bool done);
    Experience getExperience(size_t index) const;
    std::vector<Experience> getExperienceBatch(size_t batchSize = 32) const;
    std::deque<Experience> getExperienceBuffer() const;
    size_t getExperienceCount() const;
    void clearExperienceBuffer();
    
    // Learning algorithms
    void performQLearningStep();
    void performSARSAStep();
    void performDeepQLearningStep();
    void performPolicyGradientStep();
    void performAcousticSeminGradientStep();
    void performExpectationMaximizationStep();
    void performHorizonBasedStep();
    void performRLStep(); // Dispatches to appropriate algorithm
    
    // Reward functions
    bool registerRewardFunction(const std::string& name, const RewardFunction& rewardFunc);
    bool registerBuiltInRewardFunction(const RewardFunctionInfo& rewardInfo);
    bool setRewardFunction(const std::string& name);
    double computeReward(const std::vector<float>& state, int action,
                       const std::vector<float>& nextState);
    std::string getCurrentRewardFunctionName() const;
    std::vector<RewardFunctionInfo> getAvailableRewardFunctions() const;
    bool removeRewardFunction(const std::string& name);
    
    // Advanced perception processing
    void enableAdvancedPerception(bool enablePredictions = true,
                                 bool enableNovelty = true,
                                 bool enableCuriosity = true,
                                 bool enableAttention = true);
    bool isAdvancedPerceptionEnabled() const;
    std::vector<float> extractFeatures(const SensoryPercept& percept);
    std::map<std::string, double> analyzePerception(const SensoryPercept& percept);
    void updatePerceptionModel(const SensoryPercept& percept);
    std::vector<std::vector<float>> getFeatureVectorHistory(size_t historySize = 100) const;
    
    // Learning statistics and monitoring
    struct LearningStatistics {
        double totalRewards;
        double averageReward;
        double maxReward;
        double minReward;
        size_t stepsLearned;
        double learningRate;
        double explorationRate;
        double discountFactor;
        double qValueAverage;
        double qValueStdDev;
        std::map<std::string, double> algorithmSpecificStats;
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    LearningStatistics getLearningStatistics() const;
    std::vector<double> getRewardHistory(size_t maxHistory = 1000) const;
    std::vector<double> getQValueHistory(size_t maxHistory = 1000) const;
    double calculateExplorationExploitationRatio() const;
    double calculateLearningProgress() const;
    
    // Policy and value functions
    std::map<int, double> getQValues(const std::vector<float>& state) const;
    std::map<int, double> getPolicy(const std::vector<float>& state) const;
    double getValueFunction(const std::vector<float>& state) const;
    void updatePolicy(const std::vector<float>& state, int action, double reward,
                     const std::vector<float>& nextState);
    void updateValueFunction(const std::vector<float>& state, double targetValue);
    
    // Neural network management (for deep RL)
    bool createNeuralNetwork(const NeuralNetwork& network);
    bool updateNeuralNetwork(const std::string& networkName,
                           const std::vector<std::vector<float>>& newWeights,
                           const std::vector<std::vector<float>>& newBiases);
    NeuralNetwork getNeuralNetwork(const std::string& name) const;
    std::vector<NeuralNetwork> getAllNeuralNetworks() const;
    bool removeNeuralNetwork(const std::string& name);
    void setTargetNetwork(const std::string& sourceNetwork, const std::string& targetNetwork);
    void updateTargetNetworks();
    
    // Advanced feature control
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    void enableNoveltyDetection(bool enable);
    void enablePredictionErrorLearning(bool enable);
    
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    bool isNoveltyDetectionEnabled() const;
    bool isPredictionErrorLearningEnabled() const;
    
    // Interaction with brain
    void setBrain(std::shared_ptr<Brain> brain);
    std::shared_ptr<Brain> getBrain() const;
    void setAgentBrain(std::shared_ptr<AgentBrain> agentBrain);
    std::shared_ptr<AgentBrain> getAgentBrain() const;
    
    // Configuration
    void configure(const RLSimulationParameters& params);
    RLSimulationParameters getConfiguration() const;
    void setExperienceReplayConfig(const ExperienceReplayConfig& config);
    ExperienceReplayConfig getExperienceReplayConfig() const;
    
    // Utility functions
    int selectAction(const std::vector<float>& state, bool useExploration = true);
    double computeTDTarget(double reward, double nextValue, bool done, double discountFactor);
    std::vector<float> preprocessState(const std::vector<float>& rawState);
    std::vector<float> postprocessAction(int action);
    void saveAgent(const std::string& filepath);
    bool loadAgent(const std::string& filepath);
    std::string getAgentID() const;
    
    // Callbacks and events
    using ExperienceCallback = std::function<void(const Experience&)>;
    using RewardCallback = std::function<double(const std::vector<float>&, int, const std::vector<float>&)>;
    using LearningCallback = std::function<void(double, const std::vector<float>&, int)>;
    
    void addExperienceCallback(const ExperienceCallback& callback);
    void addRewardCallback(const RewardCallback& callback);
    void addLearningCallback(const LearningCallback& callback);
    void removeExperienceCallback(const ExperienceCallback& callback);
    void removeRewardCallback(const RewardCallback& callback);
    void removeLearningCallback(const LearningCallback& callback);
    
    // Statistics
    size_t getStepsSinceLastReset() const;
    double getTotalRewardsAccumulated() const;
    double getAverageReward() const;
    double getMaxReward() const;
    std::chrono::system_clock::time_point getLastLearningTime() const;
    
    // Lifecycle management
    void resetLearningStatistics();
    void resetExperienceBuffer();
    void pauseLearning();
    void resumeLearning();
    bool isPaused() const;
    
private:
    struct Impl;
    Impl* pImpl;
};