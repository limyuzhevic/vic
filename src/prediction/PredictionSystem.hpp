#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Enhanced PredictionSystem with comprehensive integration
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Configuration
    void configure(float learningRate = 0.01f, float confidenceThreshold = 0.7f);
    
    // Training and learning
    void train(const std::vector<float>& sensoryInput, ActionType action);
    void updatePrediction(const std::vector<float>& predictedState, 
                         const std::vector<float>& actualState, float error);
    
    // Prediction and action selection
    std::vector<ActionType> predictBestActions(const std::vector<float>& currentState,
                                             size_t maxSteps = 5);
    std::vector<float> predictNextState(ActionType action,
                                       const std::vector<float>& currentState);
    
    // Get predicted actions
    const std::vector<ActionType>& getPredictedActions() const { return predictedActions_; }
    float getPredictionConfidence() const { return confidence_; }
    float getPredictionError() const { return predictionError_; }
    
    // Statistics and management
    float getLearningProgress() const { return learningProgress_; }
    size_t getExperienceCount() const { return experienceCount_; }
    size_t getHistorySize() const { return stateHistory_.size(); }
    
    // Reset and maintenance
    void clearHistory();
    void reset();
    
    // Advanced prediction features
    float computeActionValue(ActionType action, const std::vector<float>& state) const;
    std::vector<float> learnTemporalPattern(const std::vector<float>& sequence);
    float getTemporalCoherence() const { return temporalCoherence_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    float learningRate_;
    float confidenceThreshold_;
    
    // Prediction state
    std::vector<ActionType> predictedActions_;
    float confidence_;
    float predictionError_;
    
    // Learning state
    float learningProgress_;
    size_t experienceCount_;
    
    // History tracking
    std::vector<std::vector<float>> stateHistory_;
    std::vector<ActionType> actionHistory_;
    std::vector<float> rewardHistory_;
    
    // Temporal patterns
    std::vector<std::vector<float>> temporalPatterns_;
    float temporalCoherence_;
    
    // Internal methods
    void addExperience(const std::vector<float>& state, ActionType action, float reward);
    float computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) const;
    float computePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const;
    std::vector<ActionType> extractActionSequence(const std::vector<float>& state);
    float evaluateActionSequence(const std::vector<ActionType>& sequence,
                                const std::vector<float>& initialState) const;
    
    // Temporal pattern learning
    void updateTemporalPatterns(const std::vector<float>& currentState);
    std::vector<float> interpolatePattern(const std::vector<float>& pattern1,
                                          const std::vector<float>& pattern2, float weight) const;
    
    // Advanced prediction methods
    void learnForwardModel(const std::vector<float>& state, ActionType action, const std::vector<float>& nextState);
    std::vector<ActionType> monteCarloSearch(const std::vector<float>& currentState, size_t searchDepth) const;
    float estimateActionValue(ActionType action, const std::vector<float>& state) const;
    
    // Confidence and error computation
    float computePredictionConfidence(const std::vector<float>& state, ActionType action) const;
    void updatePredictionError(float newError);
    
    // Pattern matching
    std::vector<std::vector<float>> findSimilarStates(const std::vector<float>& queryState,
                                                     float threshold = 0.8f) const;
    std::vector<ActionType> transferActions(const std::vector<float>& sourceState,
                                           const std::vector<float>& targetState) const;
};

} // namespace nlm

// Helper function to convert ActionType to MotorCommand
inline MotorCommand ActionTypeToMotorCommand(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::LookLeft: return MotorCommand::LookLeft;
        case ActionType::LookRight: return MotorCommand::LookRight;
        case ActionType::Eat: return MotorCommand::Interact;
        case ActionType::Wait: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}