#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Action types for motor output
enum class ActionType {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    TurnLeft,
    TurnRight,
    Look,
    LookUp,
    LookDown,
    Interact,
    Eat,
    Drink,
    Rest,
    Wait,
    Custom
};

// Motor dynamics parameters for biological realism
struct MotorDynamics {
    // Primary motor command (the intended action)
    MotorCommand motorCommand;
    
    // Execution dynamics
    float executionTime;           // Time to complete action (ms)
    float energyCost;              // Metabolic energy required (J)
    float accuracy;                // Motor precision (0-1)
    float stability;               // Motor stability (0-1)
    
    // Biological dynamics
    float force;                   // Motor force output (N)
    float torque;                  // Rotational torque (Nm)
    float velocity;                // Desired movement velocity (m/s)
    
    // Neural integration
    float neuralDrive;             // Neural activation level (0-1)
    float arousal;                // Arousal modulation (0-1)
    float neuromodulation;         // Neuromodulatory influence (-1 to 1)
    
    // State-dependent dynamics
    float fatigueFactor;           // Muscle fatigue (0-1)
    float adaptationState;         // Motor adaptation state (-1 to 1)
    
    // Cost-benefit analysis
    float expectedValue;           // Expected reward prediction
    float risk;                    // Action uncertainty/variance
    float opportunityCost;         // Cost of not doing alternatives
    
    // History and learning
    float successRate;             // Historical success rate (0-1)
    float learningRate;            // Adaptation learning rate
    float explorationBonus;       // Exploration incentive
    
    // Action sequence context
    size_t sequencePosition;       // Position in current action sequence
    size_t sequenceLength;         // Total length of action sequence
    bool isFirstInSequence;        // First action in sequence
    bool isLastInSequence;         // Last action in sequence
    
    MotorDynamics()
        : motorCommand(MotorCommand::Wait)
        , executionTime(100.0f)
        , energyCost(0.5f)
        , accuracy(0.8f)
        , stability(1.0f)
        , force(0.0f)
        , torque(0.0f)
        , velocity(0.0f)
        , neuralDrive(0.0f)
        , arousal(0.5f)
        , neuromodulation(0.0f)
        , fatigueFactor(0.0f)
        , adaptationState(0.0f)
        , expectedValue(0.0f)
        , risk(0.0f)
        , opportunityCost(0.0f)
        , successRate(0.5f)
        , learningRate(0.01f)
        , explorationBonus(0.0f)
        , sequencePosition(0)
        , sequenceLength(1)
        , isFirstInSequence(true)
        , isLastInSequence(true) {}
};

// Action representation with real dynamics and neural integration
class Action {
public:
    Action();
    Action(ActionType type);
    Action(ActionType type, const std::vector<float>& parameters);
    Action(ActionType type, const MotorDynamics& dynamics);
    ~Action();
    
    // Action type
    ActionType getType() const;
    void setType(ActionType type);
    
    // Motor command extraction
    MotorCommand getMotorCommand() const;
    void setMotorCommand(MotorCommand cmd);
    
    // Action parameters
    const std::vector<float>& getParameters() const;
    void setParameters(const std::vector<float>& params);
    
    // Motor dynamics
    const MotorDynamics& getDynamics() const;
    MotorDynamics& getDynamics();
    void setDynamics(const MotorDynamics& dynamics);
    
    // Biological realism integration
    void updateNeuralDrive(float neuralActivation, float arousalLevel, float neuromodulator);
    void updateFatigue(float fatigueRate, double dt);
    void updateAdaptation(float adaptationDelta);
    
    // Cost-benefit analysis
    void updateExpectedValue(float reward, float uncertainty);
    void updateSuccessRate(bool success);
    void applyExploration(float explorationStrength);
    
    // Action sequence support
    void setSequenceContext(size_t position, size_t length);
    void setSequenceFirst(bool isFirst);
    void setSequenceLast(bool isLast);
    
    // Dynamics computation
    float computeCost() const;
    float computeBenefit() const;
    float computeNetValue() const;
    float computeExecutionTime() const;
    
    // Motor execution
    bool isExecutable() const;
    float getExecutionProbability() const;
    float getMotorOutputLevel() const;
    
    // Learning and adaptation
    void learnFromOutcome(float reward, float predictionError);
    void updateMotorPattern(const std::vector<float>& pattern);
    
    // Get action name
    std::string getName() const;
    
    // Clone
    std::unique_ptr<Action> clone() const;
    
    // Statistics and monitoring
    float getCurrentNeuralDrive() const { return dynamics_.neuralDrive; }
    float getCurrentEnergyCost() const { return dynamics_.energyCost; }
    float getCurrentExpectedValue() const { return dynamics_.expectedValue; }
    size_t getExecutionCount() const { return executionCount_; }
    void incrementExecutionCount() { executionCount_++; }
    
private:
    ActionType type_;
    std::vector<float> parameters_;
    MotorDynamics dynamics_;
    
    // Execution history for learning
    size_t executionCount_;
    size_t successfulExecutions_;
    float averageExecutionTime_;
    float totalEnergyConsumed_;
    
    static const char* actionTypeToString(ActionType type);
    static const char* motorCommandToString(MotorCommand cmd);
};

} // namespace nlm
