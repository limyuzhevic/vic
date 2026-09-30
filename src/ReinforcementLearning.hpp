#pragma once

#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../environment/Observation.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <array>

namespace nlm {

/**
 * @brief Advanced reinforcement learning algorithms for NLM
 * 
 * This namespace contains comprehensive reinforcement learning implementations
 * including Q-learning, Deep Q-Network, actor-critic methods, and meta-learning algorithms.
 * All algorithms are designed to work with the spiking neural framework and leverage
 * the integrated neuromodulation systems for reward processing.
 */
namespace ReinforcementLearning {

/**
 * @brief RL agent state for learning and decision making
 */
struct RLAgentState {
    // State representation (can be neural population activity, working memory, etc.)
    std::vector<float> stateVector;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Action history for eligibility traces
    std::vector<std::pair<size_t, ActionType>> actionHistory;
    std::vector<double> rewardHistory;
    
    // Memory for temporal difference learning
    std::vector<float> valueMemory;
    std::vector<float> policyMemory;
    
    // Neural representation of learned values
    std::map<NeuronId, float> valueFunction;
    std::map<std::pair<NeuronId, NeuronId>, float> actionValues;
    
    RLAgentState() = default;
    RLAgentState(size_t stateSize) : stateVector(stateSize, 0.0f) {}
};

/**
 * @brief Q-table for discrete action spaces
 */
class QTable {
public:
    struct QEntry {
        std::vector<float> state;
        ActionType action;
        float value;
        double timestamp;
        size_t visitCount;
        
        QEntry() : value(0.0f), timestamp(0.0), visitCount(0) {}
        QEntry(const std::vector<float>& s, ActionType a) : state(s), action(a), value(0.0f), timestamp(0.0), visitCount(0) {}
    };

private:
    std::vector<QEntry> qEntries_;
    float learningRate_;
    float discountFactor_;
    float explorationRate_;
    float epsilonDecay_;
    size_t maxHistorySize_;
    
    // State compression for efficiency
    struct StateHash {
        size_t operator()(const std::vector<float>& state) const {
            std::hash<float> hasher;
            size_t seed = 0;
            for (float val : state) {
                seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
    
    std::unordered_map<size_t, QEntry*> stateActionMap_;
    
public:
    QTable() : learningRate_(0.1f), discountFactor_(0.99f), explorationRate_(0.1f),
               epsilonDecay_(0.995f), maxHistorySize_(10000) {}
    
    QTable(float alpha, float gamma, float epsilon, float decay, size_t maxHistory)
        : learningRate_(alpha), discountFactor_(gamma), explorationRate_(epsilon),
          epsilonDecay_(decay), maxHistorySize_(maxHistory) {}
    
    // Core Q-learning update
    void update(const std::vector<float>& state, ActionType action, 
                float reward, const std::vector<float>& nextState, 
                bool done, double timestamp);
    
    // Action selection
    ActionType selectAction(const std::vector<float>& state, double timestamp);
    
    // Value estimation
    float getQValue(const std::vector<float>& state, ActionType action) const;
    float getMaxQValue(const std::vector<float>& state) const;
    
    // Policy extraction
    ActionType getGreedyAction(const std::vector<float>& state) const;
    
    // Statistics and analysis
    float getAverageQValue() const;
    float getQValueVariance() const;
    std::vector<float> getQValueDistribution() const;
    
    // Exploration strategies
    ActionType epsilonGreedyAction(const std::vector<float>& state, double timestamp);
    ActionType softmaxAction(const std::vector<float>& state) const;
    ActionType UCBAction(const std::vector<float>& state, double timestamp) const;
    
    // State management
    void clearOldEntries(double currentTimestamp);
    void compressState(const std::vector<float>& state, float compressionFactor);
    
    // Persistence
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
    
    // Configuration
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setDiscountFactor(float gamma) { discountFactor_ = gamma; }
    void setExplorationRate(float epsilon) { explorationRate_ = epsilon; }
    void setEpsilonDecay(float decay) { epsilonDecay_ = decay; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    
    float getLearningRate() const { return learningRate_; }
    float getDiscountFactor() const { return discountFactor_; }
    float getExplorationRate(double timestamp) const { return explorationRate_ * std::pow(epsilonDecay_, timestamp); }
    size_t getSize() const { return qEntries_.size(); }
};

/**
 * @brief Actor-Critic learning algorithms for continuous action spaces
 */
class ActorCritic {
public:
    struct ActorNetwork {
        std::vector<size_t> layerSizes;
        std::vector<std::vector<float>> weights;
        std::vector<float> biases;
        std::string activationFunction;  // "sigmoid", "tanh", "relu", "softmax"
        
        ActorNetwork() = default;
        ActorNetwork(const std::vector<size_t>& sizes) : layerSizes(sizes) {
            initializeNetwork();
        }
        
    private:
        void initializeNetwork() {
            weights.clear();
            biases.clear();
            
            for (size_t i = 0; i < layerSizes.size() - 1; ++i) {
                size_t inputSize = layerSizes[i];
                size_t outputSize = layerSizes[i + 1];
                
                std::vector<float> layerWeights(inputSize * outputSize);
                std::generate(layerWeights.begin(), layerWeights.end(), 
                             [this]() { return (rand() / (RAND_MAX + 1.0f)) * 2.0f - 1.0f; });
                weights.push_back(layerWeights);
                biases.push_back(std::vector<float>(outputSize, 0.0f));
            }
        }
        
        float activate(float x, const std::string& func) const {
            if (func == "sigmoid") return 1.0f / (1.0f + std::exp(-x));
            else if (func == "tanh") return std::tanh(x);
            else if (func == "relu") return std::max(0.0f, x);
            else if (func == "softmax") return std::exp(x); // Simplified
            return x;
        }
        
        std::vector<float> forward(const std::vector<float>& input) const {
            std::vector<float> current = input;
            
            for (size_t i = 0; i < weights.size(); ++i) {
                std::vector<float> nextLayer(biases[i].size(), 0.0f);
                
                for (size_t j = 0; j < nextLayer.size(); ++j) {
                    float sum = biases[i][j];
                    for (size_t k = 0; k < current.size(); ++k) {
                        sum += weights[i][k * nextLayer.size() + j] * current[k];
                    }
                    nextLayer[j] = activate(sum, activationFunction);
                }
                current = nextLayer;
            }
            
            return current;
        }
        
        friend class ActorCritic;
    };

    struct CriticNetwork {
        std::vector<size_t> layerSizes;
        std::vector<std::vector<float>> weights;
        std::vector<float> biases;
        
        CriticNetwork() = default;
        CriticNetwork(const std::vector<size_t>& sizes) : layerSizes(sizes) {
            initializeNetwork();
        }
        
    private:
        void initializeNetwork() {
            weights.clear();
            biases.clear();
            
            for (size_t i = 0; i < layerSizes.size() - 1; ++i) {
                size_t inputSize = layerSizes[i];
                size_t outputSize = layerSizes[i + 1];
                
                std::vector<float> layerWeights(inputSize * outputSize);
                std::generate(layerWeights.begin(), layerWeights.end(), 
                             [this]() { return (rand() / (RAND_MAX + 1.0f)) * 2.0f - 1.0f; });
                weights.push_back(layerWeights);
                biases.push_back(std::vector<float>(outputSize, 0.0f));
            }
        }
        
        float forward(const std::vector<float>& input) const {
            std::vector<float> current = input;
            
            for (size_t i = 0; i < weights.size(); ++i) {
                std::vector<float> nextLayer(biases[i].size(), 0.0f);
                
                for (size_t j = 0; j < nextLayer.size(); ++j) {
                    float sum = biases[i][j];
                    for (size_t k = 0; k < current.size(); ++k) {
                        sum += weights[i][k * nextLayer.size() + j] * current[k];
                    }
                    nextLayer[j] = sum; // Linear output for critic
                }
                current = nextLayer;
            }
            
            return current[0]; // Value function output
        }
        
        friend class ActorCritic;
    };

private:
    ActorNetwork actor_;
    CriticNetwork critic_;
    float actorLearningRate_;
    float criticLearningRate_;
    float entropyCoefficient_;
    float rewardScale_;
    float targetNetworkUpdateFreq_;
    size_t replayBufferSize_;
    
    // Experience replay buffer
    struct Experience {
        std::vector<float> state;
        ActionType action;
        float reward;
        std::vector<float> nextState;
        bool done;
        double timestamp;
    };
    
    std::vector<Experience> replayBuffer_;
    size_t currentIndex_;
    
    // Target networks for stable learning
    ActorNetwork targetActor_;
    CriticNetwork targetCritic_;
    
public:
    ActorCritic() : actorLearningRate_(0.001f), criticLearningRate_(0.01f),
                    entropyCoefficient_(0.01f), rewardScale_(1.0f), targetNetworkUpdateFreq_(100),
                    replayBufferSize_(10000), currentIndex_(0) {}
    
    ActorCritic(const std::vector<size_t>& actorLayers, const std::vector<size_t>& criticLayers,
                float actorLR, float criticLR, float entropy, float rewardScale)
        : actorLearningRate_(actorLR), criticLearningRate_(criticLR),
          entropyCoefficient_(entropy), rewardScale_(rewardScale), targetNetworkUpdateFreq_(100),
          replayBufferSize_(10000), currentIndex_(0) {
        actor_.layerSizes = actorLayers;
        critic_.layerSizes = criticLayers;
        actor_.activationFunction = "relu";
        initializeTargetNetworks();
    }
    
    // Core learning updates
    void learn(const Experience& experience);
    void updateTargetNetworks();
    void trainFromReplayBuffer(size_t batchSize);
    
    // Action selection
    ActionType selectAction(const std::vector<float>& state, float epsilon = 0.0f);
    std::vector<float> getActionProbabilities(const std::vector<float>& state) const;
    
    // Value estimation
    float evaluateState(const std::vector<float>& state) const;
    float getAdvantage(const std::vector<float>& state, ActionType action) const;
    
    // Experience replay management
    void storeExperience(const Experience& exp);
    void clearReplayBuffer() { replayBuffer_.clear(); currentIndex_ = 0; }
    size_t getReplayBufferSize() const { return replayBuffer_.size(); }
    
    // Network management
    void initializeTargetNetworks() {
        targetActor_ = actor_;
        targetCritic_ = critic_;
    }
    
    void setActorLearningRate(float rate) { actorLearningRate_ = rate; }
    void setCriticLearningRate(float rate) { criticLearningRate_ = rate; }
    void setEntropyCoefficient(float coeff) { entropyCoefficient_ = coeff; }
    void setRewardScale(float scale) { rewardScale_ = scale; }
    
    // Neural network access
    const ActorNetwork& getActor() const { return actor_; }
    const CriticNetwork& getCritic() const { return critic_; }
};

/**
 * @brief Deep Q-Network (DQN) implementation
 */
class DeepQNetwork {
public:
    DeepQNetwork() = default;
    
    // DQN learning and inference
    void trainEpisode(Brain* brain, const Observation& observation, 
                     ActionType action, float reward, const Observation& nextObservation,
                     bool done, double timestamp);
    
    ActionType selectAction(const std::vector<float>& state, double timestamp, bool explore = true);
    void updateTargetNetwork();
    void optimizeReplayBuffer(size_t batchSize = 32);
    
    void setNetwork Architecture(const std::vector<size_t>& layers) { networkArchitecture_ = layers; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setDiscountFactor(float gamma) { discountFactor_ = gamma; }
    void setReplayBufferSize(size_t size) { replayBufferSize_ = size; }
    void setTargetUpdateFreq(size_t freq) { targetUpdateFreq_ = freq; }
    
private:
    std::vector<size_t> networkArchitecture_;
    float learningRate_;
    float discountFactor_;
    size_t replayBufferSize_;
    size_t targetUpdateFreq_;
    size_t targetUpdateCounter_;
    
    // Simple neural network for Q-function approximation
    struct DQNetwork {
        std::vector<size_t> layers;
        std::vector<std::vector<float>> weights;
        std::vector<float> biases;
        
        DQNetwork(const std::vector<size_t>& l) : layers(l) {
            initializeWeights();
        }
        
    private:
        void initializeWeights() {
            weights.clear();
            biases.clear();
            
            for (size_t i = 0; i < layers.size() - 1; ++i) {
                size_t inputSize = layers[i];
                size_t outputSize = layers[i + 1];
                
                std::vector<float> layerWeights(inputSize * outputSize);
                std::generate(layerWeights.begin(), layerWeights.end(), 
                             []() { return (rand() / (RAND_MAX + 1.0f)) * 2.0f - 1.0f; });
                weights.push_back(layerWeights);
                biases.push_back(std::vector<float>(outputSize, 0.0f));
            }
        }
        
        std::vector<float> forward(const std::vector<float>& input) const {
            std::vector<float> current = input;
            
            for (size_t i = 0; i < weights.size(); ++i) {
                std::vector<float> nextLayer(biases[i].size(), 0.0f);
                
                for (size_t j = 0; j < nextLayer.size(); ++j) {
                    float sum = biases[i][j];
                    for (size_t k = 0; k < current.size(); ++k) {
                        sum += weights[i][k * nextLayer.size() + j] * current[k];
                    }
                    nextLayer[j] = 1.0f / (1.0f + std::exp(-sum)); // Sigmoid
                }
                current = nextLayer;
            }
            
            return current;
        }
        
        friend class DeepQNetwork;
    };
    
    DQNetwork onlineNetwork_;
    DQNetwork targetNetwork_;
    
    // Experience replay
    struct DQExperience {
        std::vector<float> state;
        ActionType action;
        float reward;
        std::vector<float> nextState;
        bool done;
        double timestamp;
    };
    
    std::vector<DQExperience> replayBuffer_;
};

/**
 * @brief Meta-learning algorithms for fast adaptation
 */
class MetaLearning {
public:
    MetaLearning() = default;
    
    // Model-agnostic meta-learning (MAML)
    void metaLearn(const Brain* metaBrain, const Observation& taskContext,
                   const std::vector<float>& taskParameters);
    
    // Prototypical networks for few-shot learning
    void learnPrototypes(const Brain* supportBrain, const std::vector<Observation>& supportSet,
                        const std::vector<ActionType>& supportActions);
    
    // Learning to learn with gradient-based meta-learning
    void learnLearningRate(const Brain* fastLearnerBrain, const Observation& context,
                          const Experience& fastExperience);
    
    // Transfer learning between tasks
    void transferKnowledge(const Brain* sourceBrain, Brain* targetBrain,
                           const std::string& transferType);
    
    // Continual learning with regularization
    void continualLearn(Brain* brain, const Observation& observation,
                        ActionType action, float reward, const Observation& nextObservation);
    
    // Elastic weight consolidation (EWC)
    void applyElasticWeightConsolidation(Brain* brain, const std::map<NeuronId, float>& FisherInformation);
    
private:
    // Meta-learning specific state and parameters
    std::map<std::string, float> metaParameters_;
    std::vector<float> metaGradients_;
    std::map<NeuronId, float> taskSpecificFisherInfo_;
};

/**
 * @brief Advanced RL agent for NLM integration
 */
class AdvancedRLAgent {
public:
    AdvancedRLAgent() = default;
    
    // Initialization
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Core RL loop
    void step(const Observation& observation, ActionType& selectedAction, float& reward);
    void learn(const Observation& observation, ActionType action, float reward,
               const Observation& nextObservation, bool done);
    
    // Learning algorithms selection
    void setLearningAlgorithm(const std::string& algorithm) {
        learningAlgorithm_ = algorithm;
    }
    
    void setQTable(std::unique_ptr<QTable> qtable) { qTable_ = std::move(qtable); }
    void setActorCritic(std::unique_ptr<ActorCritic> actorCritic) { actorCritic_ = std::move(actorCritic); }
    void setDeepQNetwork(std::unique_ptr<DeepQNetwork> dqn) { deepQNetwork_ = std::move(dqn); }
    void setMetaLearning(std::unique_ptr<MetaLearning> metaLearning) { metaLearning_ = std::move(metaLearning); }
    
    // State management
    void reset() {
        currentState_.clear();
        currentState_.assign(stateSize_, 0.0f);
        episodeReward_ = 0.0;
        episodeSteps_ = 0;
        totalSteps_ = 0;
    }
    
    // Statistics and monitoring
    float getEpisodeReward() const { return episodeReward_; }
    size_t getEpisodeSteps() const { return episodeSteps_; }
    size_t getTotalSteps() const { return totalSteps_; }
    double getLastLearningTime() const { return lastLearningTime_; }
    
    // Configuration
    void setStateSize(size_t size) { stateSize_ = size; }
    void setActionSpace(const std::vector<ActionType>& actions) { actionSpace_ = actions; }
    void setRewardShaping(const std::function<float(const Observation&, ActionType, const Observation&)>& shaping) {
        rewardShapingFunction_ = shaping;
    }
    
    // Neural integration utilities
    void extractFeaturesFromBrain(const Brain* brain, std::vector<float>& features) const;
    void injectActionIntoBrain(Brain* brain, ActionType action) const;
    void applyRewardToBrain(Brain* brain, float reward) const;
    
    // Policy and value functions
    ActionType getGreedyAction() const;
    float getStateValue() const { return currentStateValue_; }
    float getPolicyEntropy() const { return policyEntropy_; }
    
private:
    // RL components
    std::string learningAlgorithm_;
    std::unique_ptr<QTable> qTable_;
    std::unique_ptr<ActorCritic> actorCritic_;
    std::unique_ptr<DeepQNetwork> deepQNetwork_;
    std::unique_ptr<MetaLearning> metaLearning_;
    
    // Agent state
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    // Current episode state
    std::vector<float> currentState_;
    size_t stateSize_;
    std::vector<ActionType> actionSpace_;
    float episodeReward_;
    size_t episodeSteps_;
    size_t totalSteps_;
    double lastLearningTime_;
    
    // Learning functions
    std::function<float(const Observation&, ActionType, const Observation&)> rewardShapingFunction_;
    std::function<ActionType(const std::vector<float>&)> policyFunction_;
    std::function<float(const std::vector<float>&)> valueFunction_;
    
    // Neural integration
    float currentStateValue_;
    float policyEntropy_;
};

} // namespace ReinforcementLearning

/**
 * @brief Comprehensive RL system for NLM
 * 
 * This class integrates all reinforcement learning capabilities into the NLM framework,
 * providing a unified interface for advanced RL experimentation and deployment.
 */
class AdvancedRLSystem {
public:
    AdvancedRLSystem() = default;
    
    // System initialization
    void initialize(std::shared_ptr<Config> config);
    
    // Agent management
    std::shared_ptr<AdvancedRLAgent> createAgent(const std::string& name, 
                                                 const std::vector<size_t>& stateSize,
                                                 const std::vector<ActionType>& actionSpace,
                                                 const std::string& learningAlgorithm = "q_learning");
    
    bool removeAgent(const std::string& name);
    std::shared_ptr<AdvancedRLAgent> getAgent(const std::string& name);
    std::vector<std::string> getAgentNames() const;
    
    // Training and learning
    void trainStep(const std::string& agentName, const Observation& observation,
                   ActionType& selectedAction, float& reward);
    
    void agentLearn(const std::string& agentName, const Observation& observation,
                    ActionType action, float reward, const Observation& nextObservation,
                    bool done);
    
    void trainAllAgents(const std::vector<Observation>& observations);
    void resetAllAgents() { for (auto& pair : agents_) { pair.second->reset(); } }
    
    // Advanced learning features
    void setCurriculumLearning(const std::string& agentName, 
                               const std::vector<std::pair<size_t, size_t>>& curriculum);
    void setTransferLearning(const std::string& sourceAgent, const std::string& targetAgent);
    void setMultiAgentLearning(const std::vector<std::string>& agentNames);
    
    // Monitoring and analysis
    void recordTrainingMetrics(const std::string& agentName, const std::string& metric, float value);
    void analyzeLearningProgress(const std::string& agentName) const;
    float getPerformanceMetric(const std::string& agentName, const std::string& metric) const;
    
    // Configuration and optimization
    void configureHyperparameters(const std::string& agentName,
                                   const std::map<std::string, float>& hyperparameters);
    void optimizeParameters(const std::string& agentName, size_t iterations = 100);
    
    // State management
    void saveSystemState(const std::string& filename) const;
    void loadSystemState(const std::string& filename);
    
    // Neural integration utilities
    void synchronizeWithNeuralDynamics(const std::string& agentName, Brain* brain);
    void integrateNeuromodulation(const std::string& agentName, 
                                   const Neuromodulator& signal);
    
    // Performance monitoring
    void monitorPerformance() const;
    void logTrainingProgress() const;
    
private:
    std::map<std::string, std::shared_ptr<AdvancedRLAgent>> agents_;
    std::shared_ptr<Config> config_;
    
    // Multi-agent learning parameters
    bool multiAgentLearningEnabled_;
    std::vector<std::string> cooperatingAgents_;
    std::vector<std::string> competingAgents_;
    
    // Curriculum learning
    std::map<std::string, std::vector<std::pair<size_t, size_t>>> curriculumSchedules_;
    
    // Performance metrics
    struct TrainingMetrics {
        std::map<std::string, std::vector<float>> metricHistory;
        std::map<std::string, float> currentMetrics;
        double lastUpdateTime;
        
        TrainingMetrics() : lastUpdateTime(0.0) {}
    };
    
    std::map<std::string, TrainingMetrics> performanceMetrics_;
};

} // namespace nlm