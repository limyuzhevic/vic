#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <chrono>
#include <functional>
#include <cmath>

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "TrainingLoop.hpp" // For TrainingExperience

namespace nlm {

// Reinforcement learning algorithms
enum class RLAlgorithm {
    Q_LEARNING,
    POLICY_GRADIENT,
    DEEP_Q_NETWORK,
    PPO, // Proximal Policy Optimization
    A2C, // Advantage Actor-Critic
    DDPG, // Deep Deterministic Policy Gradient
    SAC, // Soft Actor-Critic
    TABULAR_Q_LEARNING,
    SARSA,
    EXPECTATION_MAXIMIZATION
};

// Exploration strategies
enum class ExplorationStrategy {
    EPSILON_GREEDY,
    UCB1, // Upper Confidence Bound
    THERMAL, // Temperature-based
    NOISE, // Action noise
    COUNT, // Count-based
    ADAPTIVE
};

// Experience replay buffer
class ExperienceReplay {
public:
    ExperienceReplay(size_t capacity = 10000);
    
    void add(const TrainingExperience& experience);
    TrainingExperience sample();
    TrainingExperience sampleUniform();
    bool isReady(size_t minSize) const;
    void clear();
    size_t size() const { return experiences_.size(); }
    size_t capacity() const { return capacity_; }
    
private:
    std::vector<TrainingExperience> experiences_;
    size_t capacity_;
    size_t head_;
    std::mutex mutex_;
};

// Q-table for tabular Q-learning
class QTable {
public:
    QTable(int stateSpaceSize, int actionSpaceSize);
    
    float getValue(const std::vector<float>& state, int action) const;
    void setValue(const std::vector<float>& state, int action, float value);
    int getBestAction(const std::vector<float>& state) const;
    void update(const std::vector<float>& state, int action, 
                float reward, const std::vector<float>& nextState, 
                int nextAction, float alpha, float gamma);
    void incrementVisitCount(const std::vector<float>& state, int action);
    float getVisitCount(const std::vector<float>& state, int action) const;
    
private:
    // Simple hash function for state vector
    size_t hashState(const std::vector<float>& state) const;
    
    std::vector<std::vector<float>> stateTable_;
    std::vector<std::vector<float>> actionTable_;
    std::vector<std::vector<int>> visitCountTable_;
    int stateSpaceSize_;
    int actionSpaceSize_;
    size_t seed_;
};

// Neural network utility functions
class NeuralNetworkUtils {
public:
    static std::vector<float> softmax(const std::vector<float>& logits);
    static float entropy(const std::vector<float>& probabilities);
    static float clippedGradient(float gradient, float clipNorm);
    static std::vector<float> oneHotEncode(int action, int actionSpaceSize);
    static std::vector<float> muSigmaToDistribution(float mu, float sigma);
};

// ============= REINFORCEMENT LEARNER CLASS IMPLEMENTATION =============

class ReinforcementLearner {
public:
    ReinforcementLearner(std::shared_ptr<Brain> brain,
                        std::shared_ptr<AgentBrain> agent,
                        std::shared_ptr<SimpleWorld> world,
                        std::shared_ptr<Config> config);
    
    virtual ~ReinforcementLearner() = default;
    
    // Main training interface
    virtual bool train(size_t numEpisodes) = 0;
    virtual bool predict(std::shared_ptr<Brain> brain, const std::vector<float>& state, int& action) = 0;
    virtual bool learn() = 0;
    
    // Exploration vs exploitation
    virtual bool explore(const std::vector<float>& state, int& action) = 0;
    virtual bool exploit(const std::vector<float>& state, int& action) = 0;
    
    // Model persistence
    virtual bool saveModel(const std::string& filepath) = 0;
    virtual bool loadModel(const std::string& filepath) = 0;
    
    // Experience replay
    virtual bool storeExperience(const TrainingExperience& experience) = 0;
    virtual bool canLearn() const = 0;
    virtual void clearMemory() = 0;
    
    // Evaluation
    virtual bool evaluate(std::shared_ptr<Brain> brain, size_t evalEpisodes) = 0;
    virtual float getEvaluationScore() const { return evaluationScore_; }
    
    // Configuration and utilities
    RLAlgorithm getAlgorithm() const { return algorithm_; }
    ExplorationStrategy getExplorationStrategy() const { return explorationStrategy_; }
    int getActionSpaceSize() const { return actionSpaceSize_; }
    int getStateSpaceSize() const { return stateSpaceSize_; }
    
    // Hyperparameters
    float getAlpha() const { return alpha_; }
    float getGamma() const { return gamma_; }
    float getEpsilon() const { return epsilon_; }
    float getTemperature() const { return temperature_; }
    
    void setHyperparameters(float alpha, float gamma, float epsilon, float temperature);
    
    // Statistics
    size_t getExperienceCount() const { return experienceCount_; }
    size_t getEpisodeCount() const { return episodeCount_; }
    double getTotalReward() const { return totalReward_; }
    std::string getStatsSummary() const;
    
protected:
    // Common utilities
    std::vector<float> discretizeState(const std::vector<float>& state) const;
    float computeTDTarget(float reward, float nextValue, float done, float gamma) const;
    float computeLoss(float predicted, float target) const;
    
    // Members
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<Config> config_;
    
    RLAlgorithm algorithm_;
    ExplorationStrategy explorationStrategy_;
    
    // Hyperparameters
    float alpha_; // Learning rate
    float gamma_; // Discount factor
    float epsilon_; // Exploration rate
    float temperature_; // For thermal exploration
    
    // State and action spaces
    int stateSpaceSize_;
    int actionSpaceSize_;
    
    // Statistics
    size_t experienceCount_;
    size_t episodeCount_;
    double totalReward_;
    float evaluationScore_;
    
    // Random number generator
    std::mt19937 rng_;
    
    // Experience replay
    std::unique_ptr<ExperienceReplay> experienceReplay_;
    
    // State discretization
    std::vector<int> stateBins_;
    std::vector<float> stateMinValues_;
    std::vector<float> stateMaxValues_;
};

// ============= TABULAR Q-LEARNING IMPLEMENTATION =============

class TabularQLearning : public ReinforcementLearner {
public:
    TabularQLearning(std::shared_ptr<Brain> brain,
                    std::shared_ptr<AgentBrain> agent,
                    std::shared_ptr<SimpleWorld> world,
                    std::shared_ptr<Config> config);
    
    bool train(size_t numEpisodes) override;
    bool predict(std::shared_ptr<Brain> brain, const std::vector<float>& state, int& action) override;
    bool learn() override;
    bool explore(const std::vector<float>& state, int& action) override;
    bool exploit(const std::vector<float>& state, int& action) override;
    bool saveModel(const std::string& filepath) override;
    bool loadModel(const std::string& filepath) override;
    bool storeExperience(const TrainingExperience& experience) override;
    bool canLearn() const override;
    void clearMemory() override;
    bool evaluate(std::shared_ptr<Brain> brain, size_t evalEpisodes) override;
    
private:
    std::unique_ptr<QTable> qTable_;
    std::vector<TrainingExperience> episodicMemory_;
    std::unordered_map<size_t, int> stateActionCounts_;
};

// ============= DEEP Q-NETWORK IMPLEMENTATION =============

class DeepQLearning : public ReinforcementLearner {
public:
    DeepQLearning(std::shared_ptr<Brain> brain,
                 std::shared_ptr<AgentBrain> agent,
                 std::shared_ptr<SimpleWorld> world,
                 std::shared_ptr<Config> config);
    
    bool train(size_t numEpisodes) override;
    bool predict(std::shared_ptr<Brain> brain, const std::vector<float>& state, int& action) override;
    bool learn() override;
    bool explore(const std::vector<float>& state, int& action) override;
    bool exploit(const std::vector<float>& state, int& action) override;
    bool saveModel(const std::string& filepath) override;
    bool loadModel(const std::string& filepath) override;
    bool storeExperience(const TrainingExperience& experience) override;
    bool canLearn() const override;
    void clearMemory() override;
    bool evaluate(std::shared_ptr<Brain> brain, size_t evalEpisodes) override;
    
private:
    // Neural network implementation would go here
    // For now, placeholder implementation
    std::vector<std::vector<float>> neuralNetworkWeights_;
    std::vector<std::vector<float>> neuralNetworkBiases_;
    
    std::vector<float> computeQValues(const std::vector<float>& state) const;
    void updateNeuralNetwork(const std::vector<TrainingExperience>& batch);
};

// ============= POLICY GRADIENT IMPLEMENTATION =============

class PolicyGradientLearner : public ReinforcementLearner {
public:
    PolicyGradientLearner(std::shared_ptr<Brain> brain,
                         std::shared_ptr<AgentBrain> agent,
                         std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<Config> config);
    
    bool train(size_t numEpisodes) override;
    bool predict(std::shared_ptr<Brain> brain, const std::vector<float>& state, int& action) override;
    bool learn() override;
    bool explore(const std::vector<float>& state, int& action) override;
    bool exploit(const std::vector<float>& state, int& action) override;
    bool saveModel(const std::string& filepath) override;
    bool loadModel(const std::string& filepath) override;
    bool storeExperience(const TrainingExperience& experience) override;
    bool canLearn() const override;
    void clearMemory() override;
    bool evaluate(std::shared_ptr<Brain> brain, size_t evalEpisodes) override;
    
private:
    // Policy gradient specific implementation
    std::vector<float> policyWeights_;
    std::vector<float> valueWeights_;
    
    std::vector<float> computePolicyGradient(const TrainingExperience& experience) const;
    std::vector<float> computeAdvantage(const std::vector<float>& rewards, float gamma) const;
};

// ============= ADVANCED REINFORCEMENT LEARNER FACTORY =============

class ReinforcementLearnerFactory {
public:
    static std::unique_ptr<ReinforcementLearner> createLearner(
        RLAlgorithm algorithm,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<Config> config);
    
    static RLAlgorithm stringToAlgorithm(const std::string& algorithmName);
    static std::string algorithmToString(RLAlgorithm algorithm);
    static std::vector<std::string> getSupportedAlgorithms();
};

} // namespace nlm
