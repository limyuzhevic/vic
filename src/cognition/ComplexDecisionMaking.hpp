#ifndef COMPLEX_DECISION_MAKING_HPP
#define COMPLEX_DECISION_MAKING_HPP

#include <vector>
#include <string>

namespace nlm {

class Brain;

// Action types for decision making
enum class ActionType {
    Wait,
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact
};

class ComplexDecisionMaking {
public:
    ComplexDecisionMaking();
    ~ComplexDecisionMaking();
    
    void initialize(Brain* brain);
    void update(const std::vector<std::vector<float>>& options,
                const std::vector<float>& objectives,
                const std::vector<float>& constraints,
                ActionType& selectedAction,
                float& confidence);
    void updateActionQuality(ActionType action,
                             const std::vector<float>& outcome,
                             float reward);
    
    // Configuration methods
    void setConfidenceThreshold(float threshold);
    void setUncertaintyThreshold(float threshold);
    void setRiskAversion(float aversion);
    void setHeuristicEnabled(bool enabled);
    void setRiskAware(bool aware);
    
    // Status and control
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Decision configuration
    float confidenceThreshold_;
    float uncertaintyThreshold_;
    float riskAversion_;
    bool heuristicEnabled_;
    bool riskAware_;
    
    // Decision quality tracking
    std::unordered_map<ActionType, float> decisionQuality_;
};

// Internal model classes (for implementation)
class MultiObjectiveOptimizer;
class BoundedRationalityModel;
class HeuristicProcessor;
class RiskAssessor;

} // namespace nlm

#endif // COMPLEX_DECISION_MAKING_HPP
