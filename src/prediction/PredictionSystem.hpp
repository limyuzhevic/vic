#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class PolicyNetwork;
class ValueNetwork;
class ForwardModel;
class InverseModel;
class CuriosityDrivenExploration;
class RewardPredictor;

// Prediction system: predicts future sensory states and consequences
// Integrates neural prediction systems with RL algorithms

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize prediction system with brain reference
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep using forward model
    std::vector<float> predictNextState(const std::vector<float>& currentState,
                                      ActionType action);
    
    // Update predictions based on actual observation
    float updatePredictions(const std::vector<float>& predicted,
                          const std::vector<float>& actual,
                          float reward);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const std::vector<float>& state,
               ActionType action,
               const std::vector<float>& nextState,
               float reward);
    
    // Select action using policy network with RL
    ActionType selectAction(const std::vector<float>& state,
                          float explorationRate = 0.1f);
    
    // Evaluate state using value network
    float evaluateState(const std::vector<float>& state);
    
    // Compute exploration bonus
    float computeExplorationBonus(const std::vector<float>& state,
                                ActionType action,
                                float predictionError,
                                float novelty);
    
    // Get forward model for external access
    ForwardModel* getForwardModel() { return forwardModel_.get(); }
    
    // Get inverse model for external access
    InverseModel* getInverseModel() { return inverseModel_.get(); }
    
    // Get policy network for external access
    PolicyNetwork* getPolicyNetwork() { return policyNetwork_.get(); }
    
    // Get value network for external access
    ValueNetwork* getValueNetwork() { return valueNetwork_.get(); }
    
    // Get curiosity explorer for external access
    CuriosityDrivenExploration* getCuriosityExplorer() { return curiosityExplorer_.get(); }
    
    // Get reward predictor for external access
    RewardPredictor* getRewardPredictor() { return rewardPredictor_.get(); }
    
    // Enable/disable prediction mechanisms
    void enableForwardModel(bool enable) { forwardModelEnabled_ = enable; }
    void enableInverseModel(bool enable) { inverseModelEnabled_ = enable; }
    void enablePolicyNetwork(bool enable) { policyNetworkEnabled_ = enable; }
    void enableValueNetwork(bool enable) { valueNetworkEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableRewardPrediction(bool enable) { rewardPredictionEnabled_ = enable; }
    
    // Configuration
    void setStateSize(size_t size) { stateSize_ = size; }
    void setActionSize(size_t size) { actionSize_ = size; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setPredictionHorizon(size_t horizon) { predictionHorizon_ = horizon; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural components
    std::unique_ptr<PolicyNetwork> policyNetwork_;
    std::unique_ptr<ValueNetwork> valueNetwork_;
    std::unique_ptr<ForwardModel> forwardModel_;
    std::unique_ptr<InverseModel> inverseModel_;
    std::unique_ptr<CuriosityDrivenExploration> curiosityExplorer_;
    std::unique_ptr<RewardPredictor> rewardPredictor_;
    
    // Configuration
    size_t stateSize_;
    size_t actionSize_;
    float learningRate_;
    size_t predictionHorizon_;
    bool forwardModelEnabled_;
    bool inverseModelEnabled_;
    bool policyNetworkEnabled_;
    bool valueNetworkEnabled_;
    bool curiosityEnabled_;
    bool rewardPredictionEnabled_;
    
    // Brain reference
    class Brain* brain_;
};

} // namespace nlm
