#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <unordered_set>
#include <limits>
#include <functional>
#include <thread>
#include <mutex>
#include <numeric>

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Reward.hpp"

namespace nlm {

// Learning state structure
struct LearningState {
    std::vector<float> state;
    float reward;
    bool done;
    float predictedReward;
    std::vector<float> actionProbabilities;
    float curiosity;
    float entropy;
    float learningRate;
    std::chrono::steady_clock::time_point timestamp;
    
    LearningState(size_t stateDim) : state(stateDim), done(false), predictedReward(0.0f),
                                      curiosity(0.0f), entropy(0.0f), learningRate(0.001f) {
        timestamp = std::chrono::steady_clock::now();
    }
};

// Action structure
struct Action {
    int actionId;
    std::vector<float> parameters;
    float probability;
    float value;
    float entropy;
    float logProbability;
    
    Action(int id = 0) : actionId(id), probability(0.0f), value(0.0f),
                         entropy(0.0f), logProbability(0.0f) {}
};

// Transition structure for experience replay
struct Transition {
    LearningState state;
    Action action;
    LearningState nextState;
    float reward;
    float discountFactor;
    size_t timestamp;
    float priority;
    
    Transition(const LearningState& s, const Action& a, const LearningState& ns, float r, float gamma)
        : state(s), action(a), nextState(ns), reward(r), discountFactor(gamma),
          timestamp(0), priority(1.0f) {
        auto now = std::chrono::steady_clock::now();
        timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
    }
};

// Neural network for learning
class LearningNetwork {
public:
    LearningNetwork(size_t inputSize, size_t hiddenSize, size_t outputSize);
    ~LearningNetwork();
    
    // Forward pass
    std::vector<float> forward(const std::vector<float>& input);
    
    // Backward pass for training
    void backward(const std::vector<float>& target, float learningRate, float lambda = 0.01f);
    
    // Update weights
    void updateWeights(const std::vector<float>& gradients, float learningRate);
    
    // Get output probabilities (for stochastic actions)
    std::vector<float> getOutputProbabilities(const std::vector<float>& input);
    
    // Get gradients
    std::vector<float> getGradients() const { return gradients_; }
    
    // Get parameters
    const std::vector<std::vector<float>>& getWeights() const { return weights_; }
    const std::vector<std::vector<float>>& getBiases() const { return biases_; }
    
    // Initialize with normal distribution
    void initializeWeights(float mean = 0.0f, float stddev = 0.01f);
    
    // Save and load (serialization)
    void save(const std::string& filename);
    void load(const std::string& filename);
    
    // Get network size info
    size_t getInputSize() const { return inputSize_; }
    size_t getHiddenSize() const { return hiddenSize_; }
    size_t getOutputSize() const { return outputSize_; }
    
private:
    size_t inputSize_;
    size_t hiddenSize_;
    size_t outputSize_;
    
    std::vector<std::vector<float>> weights1_;
    std::vector<std::vector<float>> weights2_;
    std::vector<float> biases1_;
    std::vector<float> biases2_;
    
    std::vector<float> activations1_;
    std::vector<float> activations2_;
    std::vector<float> preActivations1_;
    std::vector<float> preActivations2_;
    
    std::vector<float> inputGradient1_;
    std::vector<float> outputGradient1_;
    std::vector<float> gradients1_;
    std::vector<float> gradients2_;
    std::vector<float> biasesGradients1_;
    std::vector<float> biasesGradients2_;
    
    std::mt19937 rng_;
    std::normal_distribution<float> normalDist_;
};

// AdvancedAgentLearning: Core learning system with multiple algorithms
class AdvancedAgentLearning {
public:
    AdvancedAgentLearning(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain);
    ~AdvancedAgentLearning();
    
    // Initialize learning system
    void initialize(size_t stateDim, size_t actionDim, size_t hiddenSize = 256);
    
    // Process transition
    void processTransition(const LearningState& state, const Action& action,
                          const LearningState& nextState, float reward, bool done);
    
    // Select action using current policy
    Action selectAction(const std::vector<float>& state, float epsilon = 0.0f,
                       bool useExploration = true);
    
    // Learn from experience
    void learn(float batchSize = 1.0f, float discountFactor = 0.99f);
    
    // Update learning parameters
    void updateLearningParameters(float learningRate, float explorationRate,
                                 float entropyCoefficient = 0.01f);
    
    // Reset for new episode
    void reset();
    
    // Get current policy value
    float getValue(const std::vector<float>& state);
    
    // Get policy entropy (for exploration)
    float getPolicyEntropy(const std::vector<float>& state);
    
    // Get experience statistics
    size_t getExperienceCount() const { return experiences_.size(); }
    
    // Enable/disable specific algorithms
    void enableAlgorithm(const std::string& algorithm, bool enable);
    void enableExplorationStrategy(const std::string& strategy, bool enable);
    
    // Set curiosity mechanism
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    void setTransferLearningEnabled(bool enabled) { transferLearningEnabled_ = enabled; }
    
    // Get learning statistics
    struct Statistics {
        float totalReward;
        float averageReward;
        size_t episodeCount;
        size_t stepCount;
        float explorationRate;
        float learningRate;
        float valueEstimate;
        float policyEntropy;
        
        Statistics() : totalReward(0.0f), averageReward(0.0f), episodeCount(0),
                      stepCount(0), explorationRate(0.0f), learningRate(0.0f),
                      valueEstimate(0.0f), policyEntropy(0.0f) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
    // Set learning algorithms
    void setPrimaryAlgorithm(const std::string& algorithm) { primaryAlgorithm_ = algorithm; }
    void setBackupAlgorithm(const std::string& algorithm) { backupAlgorithm_ = algorithm; }
    
    // Experience management
    void enableExperienceReplay(bool enabled) { experienceReplayEnabled_ = enabled; }
    void setReplayBufferSize(size_t size) { replayBufferSize_ = size; }
    
    // Curiosity and intrinsic motivation
    void setCuriositySource(const std::string& source, float weight);
    void enableMetaLearning(bool enabled) { metaLearningEnabled_ = enabled; }
    void enableSocialLearning(bool enabled) { socialLearningEnabled_ = enabled; }
    
private:
    // Learning algorithms
    void qLearning(const LearningState& state, const Action& action,
                  const LearningState& nextState, float reward, bool done);
    void actorCritic(const LearningState& state, const Action& action,
                    const LearningState& nextState, float reward, bool done);
    void reinforcementLearning(const LearningState& state, const Action& action,
                              const LearningState& nextState, float reward, bool done);
    void metaLearningUpdate(const LearningState& state, const Action& action,
                           const LearningState& nextState, float reward, bool done);
    
    // Exploration strategies
    Action epsilonGreedyExploration(const std::vector<float>& state, float epsilon);
    Action thompsonSamplingExploration(const std::vector<float>& state);
    Action ucbExploration(const std::vector<float>& state);
    Action entropyRegularizedExploration(const std::vector<float>& state, float temperature);
    
    // Learning rate adaptation
    float adaptiveLearningRate(const std::vector<float>& gradients, size_t step);
    float learningRateAnnealing(size_t step, float initialRate, float finalRate, size_t totalSteps);
    
    // Experience replay
    void storeExperience(const Transition& transition);
    std::vector<Transition> sampleExperience(float batchSize, bool prioritize = false);
    
    // Curiosity mechanisms
    void updateCuriosityDrivenExploration(LearningState& state);
    float computeIntrinsicReward(const std::vector<float>& state,
                                const std::vector<float>& nextState);
    
    // Transfer learning
    void updateTransferKnowledge(const LearningState& state, const Action& action);
    float transferKnowledge(const std::string& sourceDomain);
    
    // Social learning
    void updateSocialKnowledge(const LearningState& state, const Action& action);
    void imitationLearning(const Action& expertAction, const LearningState& state);
    
    // Neural attention mechanisms
    void computeAttentionWeights(const std::vector<float>& state,
                                std::vector<float>& attentionWeights);
    void computeRepresentation(const std::vector<float>& state,
                               std::vector<float>& representation);
    
    // Helper functions
    float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
    
    float softmax(const std::vector<float>& inputs, std::vector<float>& output);
    
    float computeEntropy(const std::vector<float>& probabilities);
    
    float computeValueEstimate(const std::vector<float>& state);
    
    std::vector<float> computeFeatureRepresentation(const std::vector<float>& state);
    
    float computeSurprise(const std::vector<float>& state, const std::vector<float>& nextState);
    
    // Core components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Curiosity> curiosity_;
    std::shared_ptr<Novelty> novelty_;
    std::shared_ptr<Neuromodulator> neuromodulator_;
    
    // Learning network
    std::unique_ptr<LearningNetwork> valueNetwork_;
    std::unique_ptr<LearningNetwork> policyNetwork_;
    
    // Experience buffer
    std::vector<Transition> experiences_;
    size_t replayBufferSize_;
    
    // Learning algorithms
    std::string primaryAlgorithm_;
    std::string backupAlgorithm_;
    std::unordered_map<std::string, bool> algorithmEnabled_;
    std::unordered_map<std::string, bool> explorationStrategyEnabled_;
    
    // Learning parameters
    float learningRate_;
    float discountFactor_;
    float explorationRate_;
    float entropyCoefficient_;
    float curiosityWeight_;
    
    // State tracking
    size_t actionDimension_;
    size_t stateDimension_;
    size_t hiddenLayerSize_;
    
    std::vector<float> currentState_;
    Action lastAction_;
    float lastReward_;
    bool episodeDone_;
    size_t episodeStep_;
    
    // Statistics
    Statistics stats_;
    
    // Curiosity and meta-learning
    bool curiosityEnabled_;
    bool transferLearningEnabled_;
    bool metaLearningEnabled_;
    bool socialLearningEnabled_;
    bool experienceReplayEnabled_;
    
    // Transfer learning knowledge
    std::unordered_map<std::string, std::vector<float>> transferableKnowledge_;
    
    // Social learning knowledge
    std::vector<Action> expertDemonstrations_;
    std::unordered_map<std::vector<float>, Action> socialKnowledge_;
    
    // Random number generation
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformDist_;
    std::normal_distribution<float> normalDist_;
};