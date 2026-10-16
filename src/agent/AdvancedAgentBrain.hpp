#ifndef ADVANCED_AGENT_BRAIN_HPP
#define ADVANCED_AGENT_BRAIN_HPP

#include "AgentBrain.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

// Advanced agent states for sophisticated behavior
enum class AgentState {
    IDLE,
    EXPLORATION,
    EXPLOITATION,
    LEARNING,
    CONSOLIDATING,
    RESTING,
    TRANSITIONING
};

// Advanced action selection strategies
enum class ActionSelectionStrategy {
    EPSILON_GREEDY,
    SOFTMAX,
    UCB1, 
    BAYESIAN_OPTIMIZATION,
    POLICY_GRADIENT,
    ACTOR_CRITIC,
    TRPO,
    PPO
};

// Experience replay buffer for continual learning
struct Experience {
    std::vector<float> state;           // Sensory input
    MotorCommand action;                // Action taken
    float reward;                       // Reward received
    std::vector<float> nextState;       // Next sensory input
    bool done;                          // Episode terminated
    double timestamp;                   // Time of experience
    float predictionError;              // TD error for learning
    
    Experience() : action(MotorCommand::Wait), reward(0.0f), done(false), 
                   timestamp(0.0), predictionError(0.0f) {}
    
    Experience(const std::vector<float>& s, MotorCommand a, float r,
               const std::vector<float>& ns, bool d, double t)
        : state(s), action(a), reward(r), nextState(ns), done(d), 
          timestamp(t), predictionError(0.0f) {}
};

// Policy network (simplified neural network for policy representation)
class PolicyNetwork {
public:
    PolicyNetwork(size_t inputSize, size_t hiddenSize, size_t outputSize);
    
    std::vector<float> forward(const std::vector<float>& input);
    void update(const std::vector<float>& input, const std::vector<float>& target, float learningRate);
    void addExperience(const std::vector<float>& state, MotorCommand action, float reward, 
                      const std::vector<float>& nextState, bool done);
    void trainExperienceReplay(size_t batchSize);
    void reset();
    
    void setStrategy(ActionSelectionStrategy strategy) { strategy_ = strategy; }
    ActionSelectionStrategy getStrategy() const { return strategy_; }
    
    void enableAdvancedLearning(bool enable) { advancedLearningEnabled_ = enable; }
    bool isAdvancedLearningEnabled() const { return advancedLearningEnabled_; }
    
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    float getExplorationRate() const { return explorationRate_; }
    
    void setEntropyRegularization(float factor) { entropyFactor_ = factor; }
    float getEntropyRegularization() const { return entropyFactor_; }
    
private:
    size_t inputSize_;
    size_t hiddenSize_;
    size_t outputSize_;
    ActionSelectionStrategy strategy_;
    bool advancedLearningEnabled_;
    float explorationRate_;
    float entropyFactor_;
    
    // Simple neural network parameters (in practice, would use proper neural network library)
    std::vector<float> policyWeights1_;
    std::vector<float> policyWeights2_;
    std::vector<float> policyBiases1_;
    std::vector<float> policyBiases2_;
    
    // Experience replay buffer
    std::vector<Experience> experienceBuffer_;
    size_t maxBufferSize_;
    double replayProbability_;
    
    // Learning statistics
    size_t learningSteps_;
    float totalReward_;
    double lastUpdateTime_;
    
    // Helper methods
    float sigmoid(float x) const { return 1.0f / (1.0f + std::exp(-x)); }
    float softmax(const std::vector<float>& logits) const;
    float entropy(const std::vector<float>& probabilities) const;
};

// Advanced agent brain with sophisticated learning capabilities
class AdvancedAgentBrain {
public:
    AdvancedAgentBrain(std::shared_ptr<Brain> brain);
    ~AdvancedAgentBrain();
    
    // Initialize with world and configuration
    void initialize(const SimpleWorld& world, std::shared_ptr<Config> config);
    
    // Process sensory input and update internal state
    void processSensoryInput(const SensoryPercept& percept);
    
    // Select action using advanced policy
    MotorCommand selectAction(const std::vector<float>& state);
    
    // Update learning from experience
    void learnFromExperience(const std::vector<float>& state, MotorCommand action,
                           float reward, const std::vector<float>& nextState, bool done);
    
    // Apply reward modulation with advanced algorithms
    void applyAdvancedRewardModulation(float reward, float predictedReward);
    
    // Update development with sophisticated life stages
    void updateAdvancedDevelopment(double timestep);
    
    // Experience replay for continual learning
    void performExperienceReplay();
    
    // Meta-learning and adaptation
    void adaptPolicyBasedOnPerformance(float performanceMetric);
    void updateExplorationStrategy();
    
    // Advanced statistics and monitoring
    float getExpectedValue(const std::vector<float>& state) const;
    float getPolicyEntropy() const;
    float getLearningProgress() const;
    float getCuriosityScore() const;
    float getNoveltyScore() const;
    
    // Get current agent state
    AgentState getAgentState() const { return currentState_; }
    ActionSelectionStrategy getActionSelectionStrategy() const { return policyNetwork_.getStrategy(); }
    
    // Configuration and tuning
    void configurePolicyNetwork(size_t inputSize, size_t hiddenSize, size_t outputSize);
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setDiscountFactor(float gamma) { discountFactor_ = gamma; }
    void setTau(float tau) { polyakAvgTau_ = tau; }
    void enableMetaLearning(bool enable) { metaLearningEnabled_ = enable; }
    void setExperienceReplaySize(size_t size) { policyNetwork_.setExperienceReplaySize(size); }
    
    // Advanced action selection strategies
    void setActionSelectionStrategy(ActionSelectionStrategy strategy);
    
    // Enhanced curiosity and exploration
    void enableAdaptiveExploration(bool enable);
    void setExplorationSchedule(const std::vector<float>& schedule);
    
    // Performance monitoring
    struct PerformanceMetrics {
        float recentReturn;
        float episodeLength;
        float explorationEfficiency;
        float learningRate;
        float policyEntropy;
        size_t episodesCompleted;
        double lastUpdateTime;
        
        PerformanceMetrics() : recentReturn(0.0f), episodeLength(0.0f),
                               explorationEfficiency(0.0f), learningRate(0.0f),
                               policyEntropy(0.0f), episodesCompleted(0),
                               lastUpdateTime(0.0) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const { return metrics_; }
    
    // Policy export/import for advanced usage
    void savePolicy(const std::string& filepath) const;
    bool loadPolicy(const std::string& filepath);
    
    // Statistics and debugging
    size_t getExperienceBufferSize() const { return policyNetwork_.getExperienceBufferSize(); }
    size_t getTotalLearningSteps() const { return policyNetwork_.getTotalLearningSteps(); }
    float getTotalReward() const { return policyNetwork_.getTotalReward(); }
    
    // Context manager for safe resource management
    class ScopedTraining {
    public:
        ScopedTraining(AdvancedAgentBrain& agent) : agent_(agent) {
            agent_.enableAdvancedLearning(true);
        }
        ~ScopedTraining() {
            agent_.enableAdvancedLearning(false);
        }
    private:
        AdvancedAgentBrain& agent_;
    };
    
    ScopedTraining scopedTraining() { return ScopedTraining(*this); }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    
    // Current state
    AgentState currentState_;
    std::vector<float> currentStateVector_;
    MotorCommand lastAction_;
    float lastReward_;
    double lastTimestamp_;
    
    // Advanced learning components
    PolicyNetwork policyNetwork_;
    float learningRate_;
    float discountFactor_;
    float polyakAvgTau_;
    bool metaLearningEnabled_;
    
    // Experience management
    std::vector<Experience> episodeBuffer_;
    double episodeStartTime_;
    
    // Neuromodulation with multiple signals
    float dopamineLevel_;
    float serotoninLevel_;
    float noradrenalineLevel_;
    float acetylcholineLevel_;
    
    // Advanced curiosity and exploration
    float intrinsicMotivation_;
    float extrinsicCuriosity_;
    float adaptiveExplorationRate_;
    
    // Development and plasticity
    double developmentalAge_;
    float plasticityModifier_;
    float learningCapacity_;
    
    // Performance monitoring
    PerformanceMetrics metrics_;
    std::vector<float> performanceHistory_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool adaptiveExplorationEnabled_;
    
    // Helper methods
    void updatePerformanceMetrics(float reward, double timestamp);
    void updateIntrinsicMotivation(const std::vector<float>& state);
    void updateNeuromodulationSignals(float reward, float predictionError);
    std::vector<float> extractStateFeatures(const SensoryPercept& percept) const;
    float calculateTDTarget(float reward, float nextValue, bool done) const;
    void softUpdatePolicy(float tau);
};

} // namespace nlm

#endif // ADVANCED_AGENT_BRAIN_HPP