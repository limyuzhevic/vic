#pragma once

#include "../core/Types/Types.hpp"
#include "../environment/Observation.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <vector>
#include <memory>
#include <random>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Complete Reward system for reinforcement learning and brain emulation
// Implements state-based, action-based, and model-based reward computation
// with intrinsic/extrinsic reward integration and uncertainty modeling

class Reward {
public:
    // Reward modality types
    enum class Modality {
        Extrinsic,    // Environment-based rewards
        Intrinsic,    // Internal state rewards (curiosity, novelty)
        Social,       // Social interaction rewards
        Structural    // Development/pruning rewards
    };

    // Reward computation strategies
    enum class Strategy {
        Tabular,       // Q-table based
        FunctionApproximation,  // Neural network based
        ModelBased,    // Full model prediction
        Hybrid         // Combination of methods
    };

    // Reward statistics for analysis
    struct RewardStats {
        float totalRewards;              // Sum of all rewards received
        float extrinsicTotal;            // Sum of extrinsic rewards
        float intrinsicTotal;            // Sum of intrinsic rewards
        float socialTotal;              // Sum of social rewards
        float structuralTotal;          // Sum of structural rewards
        float meanReward;               // Average reward
        float rewardStdDev;             // Standard deviation
        float rewardVariance;           // Variance
        float discountFactor;           // Effective discount factor
        float explorationBonus;         // Exploration bonus accumulated
        float exploitationValue;        // Exploitation value accumulated
        float predictionAccuracy;       // Prediction accuracy
        float uncertainty;              // Reward uncertainty
        Timestamp lastUpdateTime;       // Last update timestamp
        SimulationStep totalSteps;      // Total steps processed
    };

    // Reward prediction with uncertainty
    struct RewardPrediction {
        float predictedValue;            // Predicted reward value
        float predictionError;          // Prediction error (RPE)
        float uncertainty;              // Uncertainty/confidence
        float explorationValue;         // Value of exploring this state
        float exploitationValue;        // Value of exploiting this action
        std::vector<float> featureWeights; // Feature-based weights
        Timestamp predictionTime;       // When prediction was made
    };

    // Reward history entry with temporal context
    struct RewardEntry {
        Timestamp timestamp;            // When reward was received
        Modality modality;              // Reward type
        float value;                    // Reward value
        float discountedValue;          // Discounted value
        std::string source;             // Reward source description
        SimulationStep step;           // Simulation step
        float predictionError;          // Associated prediction error
    };

public:
    // Constructor
    Reward();
    
    // Destructor
    ~Reward();

    // Core reward computation methods
    
    // Compute state-based reward from observation
    float computeStateReward(const Observation& observation) const;
    
    // Compute action-based reward for policy gradient methods
    float computeActionReward(const Action& action, const Observation& observation) const;
    
    // Compute model-based reward prediction
    float computeModelBasedReward(const Observation& observation, 
                                 const Action& action,
                                 const std::vector<float>& futureState) const;
    
    // Integrate multiple reward modalities
    float integrateRewards(const std::vector<std::pair<Modality, float>>& modalityRewards) const;
    
    // Apply reward shaping for exploration vs exploitation
    float shapeReward(float reward, const Action& action, 
                     const Observation& observation, bool isExploration) const;

    // Reward prediction and learning
    
    // Update reward prediction using temporal difference learning
    void updatePrediction(float actualReward, float predictedReward);
    
    // Compute reward prediction error (RPE) signal
    float computePredictionError(float actualReward, float predictedReward) const;
    
    // Update model with new reward observations
    void updateModel(const Observation& observation, const Action& action, float reward);
    
    // Predict future rewards using learned model
    RewardPrediction predictFutureReward(const Observation& observation,
                                        const Action& action,
                                        float horizon) const;

    // Neuromodulation integration
    
    // Send reward signal to dopamine system
    void signalToDopamine(nlm::Dopamine& dopamine, float reward, float predictedReward);
    
    // Integrate with neuromodulation systems
    void integrateWithNeuromodulation(nlm::Dopamine& dopamine, const Observation& observation);

    // Learning algorithm implementations
    
    // Q-learning reward update
    void qLearningUpdate(const std::vector<float>& stateFeatures,
                        const Action& action,
                        float reward,
                        const std::vector<float>& nextStateFeatures,
                        float learningRate);
    
    // SARSA learning update
    void sarsaUpdate(const std::vector<float>& state,
                    const Action& action,
                    float reward,
                    const std::vector<float>& nextState,
                    const Action& nextAction,
                    float learningRate,
                    float discountFactor);
    
    // Policy gradient reward computation
    float policyGradientReward(const Action& action,
                              const Observation& observation,
                              float advantage) const;
    
    // Actor-critic style learning
    void actorCriticUpdate(const Observation& observation,
                          const Action& action,
                          float reward,
                          const Observation& nextObservation,
                          float criticLearningRate,
                          float actorLearningRate,
                          float discountFactor);

    // Reward uncertainty and noise modeling
    
    // Add Gaussian noise to rewards
    float addNoise(float reward, float noiseLevel) const;
    
    // Compute reward uncertainty based on history
    float computeUncertainty(const Observation& observation) const;
    
    // Update uncertainty estimates
    void updateUncertainty(float reward, float predictionError);

    // Statistical analysis and history management
    
    // Get current reward statistics
    const RewardStats& getStats() const;
    
    // Get reward history
    const std::vector<RewardEntry>& getHistory() const;
    
    // Add reward entry to history
    void addRewardEntry(Modality modality, float value, const std::string& source = "");
    
    // Clear reward history
    void clearHistory();
    
    // Compute reward statistics
    void computeStats();

    // Configuration and parameter management
    
    // Set reward computation strategy
    void setStrategy(Strategy strategy);
    
    // Set exploration-exploitation balance
    void setExplorationRate(float rate);
    
    // Set discount factor for temporal integration
    void setDiscountFactor(float factor);
    
    // Set reward scaling parameters
    void setRewardScaling(float extrinsicScale, float intrinsicScale, 
                         float socialScale, float structuralScale);
    
    // Set noise parameters
    void setNoiseParameters(float mean, float stddev);
    
    // Set prediction horizon for model-based methods
    void setPredictionHorizon(float horizon);
    
    // Get current configuration parameters
    const std::unordered_map<std::string, float>& getParameters() const;
    
    // Set parameter value
    void setParameter(const std::string& name, float value);

    // Utility methods
    
    // Get current reward value
    float getCurrentReward() const;
    
    // Set current reward value
    void setCurrentReward(float value);
    
    // Get immediate reward value
    float getImmediateReward() const;
    
    // Update reward with temporal integration
    void update(float reward, TimestepDuration dt);
    
    // Reset reward system
    void reset();
    
    // Clone reward system
    std::unique_ptr<Reward> clone() const;

    // Advanced methods for brain emulation
    
    // Implement reward-modulated plasticity
    void applyRewardModulatedPlasticity(nlm::Dopamine& dopamine,
                                       const std::vector<std::shared_ptr<Synapse>>& synapses,
                                       const std::vector<float>& eligibilityTraces);
    
    // Model-based credit assignment for delayed rewards
    void creditAssignmentForDelayedRewards(const Observation& observation,
                                           const Action& action,
                                           float reward,
                                           float delay);
    
    // Social reward computation
    float computeSocialReward(const std::vector<float>& socialFeatures) const;
    
    // Intrinsic motivation (curiosity, novelty)
    float computeIntrinsicReward(const Observation& observation,
                                const Action& action,
                                const std::vector<float>& modelPrediction) const;
    
    // Structural plasticity rewards (development)
    float computeStructuralReward(DevelopmentalStage stage, 
                                 float plasticityPotential) const;

    // Learning rate adaptation
    void adaptLearningRate(float environmentComplexity, 
                          float predictionAccuracy);
    
    // Exploration-exploitation strategies
    Action selectAction(const std::vector<Action>& availableActions,
                       const Observation& observation,
                       const std::vector<float>& stateFeatures) const;
    
    // Statistical analysis of reward learning
    struct RewardLearningAnalysis {
        float learningCurve[100];        // Learning over time (last 100 steps)
        float predictionErrorVariance;   // Variance in prediction errors
        float rewardHistoryEntropy;      // Entropy of reward distribution
        float explorationExploitationRatio; // Ratio of exploration to exploitation
        float intrinsicExtrinsicRatio;   // Ratio of intrinsic to extrinsic rewards
        float creditAssignmentAccuracy;  // Accuracy of delayed reward assignment
        float modelPredictionError;      // Error in model predictions
        std::vector<float> rewardModalityDistribution; // Distribution by modality
    };
    
    RewardLearningAnalysis analyzeRewardLearning() const;

private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm