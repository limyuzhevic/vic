#pragma once

#include "AgentBrainImproved.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

// Motor decoding algorithms with advanced features
class MotorDecoder {
public:
    MotorDecoder(const AgentBrainConfig& config) : config_(config) {}
    
    // Decode motor command from neural activity with improved algorithms
    MotorCommand decodeFromNeuralActivity(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait) const;
    
    // Calculate neural activity with noise and signal processing
    std::vector<float> calculateNeuralActivity(const std::vector<Neuron*>& neurons) const;
    
    // Advanced activity combination using weighted averaging
    float calculateWeightedActivity(const std::vector<Neuron*>& neurons, 
                                   const std::vector<float>& weights) const;
    
    // Competitive selection with softmax for better exploration
    size_t softmaxSelection(const std::vector<float>& activities) const;
    
    // Gated attention mechanism for motor selection
    size_t gatedAttentionSelection(const std::vector<float>& activities,
                                  const std::vector<float>& attentionWeights) const;
    
    // Dynamic threshold adjustment based on context
    float calculateDynamicThreshold(const std::vector<float>& activities) const;
    
    // Smooth motor command transitions
    MotorCommand smoothTransition(const MotorCommand& previous,
                                 const MotorCommand& current,
                                 double timestep) const;
    
private:
    AgentBrainConfig config_;
};

// Enhanced curiosity-based exploration system
class CuriositySystem {
public:
    CuriositySystem(const AgentBrainConfig& config) : config_(config) {}
    
    // Calculate curiosity level with multiple components
    float calculateCuriosity(float novelty, float predictionError,
                            float explorationState, float timestamp) const;
    
    // Generate exploration probability with temperature scaling
    float calculateExplorationProbability(float curiosity, float temperature) const;
    
    // Select action with enhanced exploration-exploitation balance
    MotorCommand selectAction(const MotorCommand& defaultAction,
                            const std::vector<MotorCommand>& allActions,
                            float curiosity, float explorationProb,
                            std::shared_ptr<RandomGenerator> rng) const;
    
    // Update exploration state with configurable decay and growth
    void updateExplorationState(float& explorationState, double timestep) const;
    
    // Adaptive curiosity threshold based on developmental stage
    float getAdaptiveCuriosityThreshold(DevelopmentalStage stage) const;
    
private:
    AgentBrainConfig config_;
};

// Advanced reward modulation system
class RewardModulation {
public:
    RewardModulation(const AgentBrainConfig& config) : config_(config) {}
    
    // Apply reward-modulated learning with eligibility traces
    void applyModulatedLearning(float reward, float predictedReward,
                               std::vector<Synapse*>& synapses,
                               float eligibilityDecay = 0.1f) const;
    
    // Calculate prediction error with adaptive learning rate
    float calculatePredictionError(float reward, float predictedReward,
                                  float eligibilityTrace) const;
    
    // Update expected reward with exponential moving average
    void updateExpectedReward(float& expectedReward, float reward,
                            float alpha = 0.05f) const;
    
    // Calculate dopamine signal with configurable gain
    float calculateDopamineSignal(float predictionError, float gain) const;
    
    // Modulate plasticity based on reward history
    float calculatePlasticityModulator(float dopamine, float baseline = 0.5f,
                                       float sensitivity = 0.5f) const;
    
    // Reward normalization for better learning
    float normalizeReward(float reward, float minReward = -1.0f, float maxReward = 1.0f) const;
    
private:
    AgentBrainConfig config_;
};

// Developmental progression system
class DevelopmentSystem {
public:
    DevelopmentSystem(const AgentBrainConfig& config) : config_(config) {}
    
    // Update developmental stage with configurable timing
    DevelopmentalStage updateDevelopmentalStage(double age, double timestep) const;
    
    // Calculate developmental plasticity modifier
    float calculatePlasticityModifier(DevelopmentalStage stage) const;
    
    // Check if critical period is active
    bool isCriticalPeriod(DevelopmentalStage stage) const;
    
    // Get developmental rewards for learning
    float getDevelopmentalReward(DevelopmentalStage stage) const;
    
    // Calculate maturational changes in behavior
    float calculateMaturationalChange(DevelopmentalStage from, DevelopmentalStage to) const;
    
    // Check for developmental milestones
    bool checkMilestone(double age, DevelopmentalStage stage) const;
    
private:
    AgentBrainConfig config_;
};

// Performance monitoring and optimization
class PerformanceMonitor {
public:
    PerformanceMonitor() : totalSteps_(0), activeNeurons_(0), firingRate_(0.0f) {}
    
    // Record performance metrics
    void recordStep(size_t activeNeurons, float firingRate);
    
    // Calculate performance statistics
    float getAverageFiringRate() const;
    float getActivityLevel() const;
    size_t getTotalSteps() const { return totalSteps_; }
    
    // Check for performance issues
    bool detectActivityDrop() const;
    bool detectExcessiveActivity() const;
    
    // Get performance recommendations
    std::string getPerformanceRecommendations() const;
    
    // Reset performance metrics
    void reset();
    
private:
    size_t totalSteps_;
    std::vector<size_t> activeNeuronsHistory_;
    std::vector<float> firingRateHistory_;
};

// Integration helper class
class AgentBrainIntegrator {
public:
    AgentBrainIntegrator(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
        : brain_(brain), config_(config), motorDecoder_(config), 
          curiositySystem_(config), rewardModulation_(config), 
          developmentSystem_(config), performanceMonitor_() {}
    
    // Complete sensory processing pipeline
    std::vector<float> processSensoryInput(const SensoryPercept& percept);
    
    // Complete motor command pipeline
    MotorCommand processMotorCommand(double timestep);
    
    // Complete reward processing pipeline
    void processReward(float reward, float predictedReward);
    
    // Complete development pipeline
    void processDevelopment(double timestep);
    
    // Get integrated agent state
    struct AgentState {
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float dopamineLevel;
        DevelopmentalStage developmentalStage;
        float explorationState;
        float plasticityModifier;
        size_t sensoryInputSize;
        size_t motorOutputSize;
    };
    
    AgentState getState() const;
    
    // Performance optimization
    std::string getOptimizationRecommendations() const;
    
private:
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    MotorDecoder motorDecoder_;
    CuriositySystem curiositySystem_;
    RewardModulation rewardModulation_;
    DevelopmentSystem developmentSystem_;
    PerformanceMonitor performanceMonitor_;
    
    // Internal state
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float explorationState_;
    DevelopmentalStage developmentalStage_;
    double developmentalAge_;
    float plasticityModifier_;
    MotorCommand previousMotorCommand_;
    double lastMotorCommandTime_;
};

// Integration of advanced systems into AgentBrain
class AdvancedAgentBrain : public AgentBrain {
public:
    AdvancedAgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig::getDefaultConfig())
        : AgentBrain(brain, config), integrator_(brain, config) {}
    
    // Override methods with advanced integration
    void processSensoryInput(const SensoryPercept& percept) override;
    MotorCommand decodeMotorCommand() override;
    void applyRewardModulation(float reward, float predictedReward) override;
    void updateDevelopment(double timestep) override;
    
    // Advanced methods
    std::string getPerformanceAnalysis() const;
    std::string getOptimizationSuggestions() const;
    
    // Configuration from external sources
    bool loadConfigFromFile(const std::string& filepath);
    bool loadConfigFromCommandLine(int argc, char** argv);
    
private:
    AgentBrainIntegrator integrator_;
};

// Utility functions
namespace MotorUtils {
    // Calculate Euclidean distance between two motor command vectors
    float calculateDistance(MotorCommand cmd1, MotorCommand cmd2);
    
    // Smooth interpolation between motor commands
    MotorCommand interpolateCommands(MotorCommand cmd1, MotorCommand cmd2, float t);
    
    // Convert motor command to vector representation
    std::vector<float> commandToVector(MotorCommand cmd);
    
    // Convert vector to motor command
    MotorCommand vectorToCommand(const std::vector<float>& vec);
    
    // Get all available motor commands
    std::vector<MotorCommand> getAllMotorCommands();
    
    // Check if motor command is valid
    bool isValidMotorCommand(MotorCommand cmd);
}

namespace CuriosityUtils {
    // Calculate Shannon entropy of curiosity distribution
    float calculateCuriosityEntropy(float curiosityLevel);
    
    // Calculate curiosity gradient
    float calculateCuriosityGradient(float novelty, float predictionError);
    
    // Apply curiosity noise
    float applyCuriosityNoise(float baseCuriosity, float noiseLevel);
    
    // Temperature-scaled softmax for curiosity
    float temperatureSoftmax(float value, float temperature);
    
    // Normalize curiosity across multiple dimensions
    float normalizeCuriosity(float novelty, float predictionError, float exploration);
}

namespace RewardUtils {
    // Calculate discounted future reward
    float calculateDiscountedReward(float currentReward, float futureRewards, float gamma = 0.9f);
    
    // Calculate reward prediction error with temporal difference
    float calculateTDError(float reward, float valueNext, float valueCurrent, float discount = 0.99f);
    
    // Reward normalization strategies
    float normalizeRewards(const std::vector<float>& rewards);
    float scaleRewards(const std::vector<float>& rewards, float targetMean = 0.0f, float targetStddev = 1.0f);
}

namespace DevelopmentUtils {
    // Calculate developmental rate based on age
    float calculateDevelopmentalRate(double age);
    
    // Get developmental reward for learning
    float getDevelopmentalLearningReward(double age);
    
    // Check if development phase is complete
    bool isDevelopmentComplete(double age);
    
    // Calculate maturity coefficient
    float calculateMaturationCoefficient(double age);
}

} // namespace nlm