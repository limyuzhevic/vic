#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

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

// Action consequences for planning and prediction
struct ActionConsequence {
    // State changes resulting from action execution
    std::vector<float> stateChanges;
    
    // Expected reward from this consequence
    float expectedReward;
    
    // Probability of this consequence occurring
    float probability;
    
    // Execution time (in simulation steps)
    double executionTime;
    
    // Resource cost (energy, etc.)
    float resourceCost;
    
    // Risk/uncertainty associated with this consequence
    float uncertainty;
    
    ActionConsequence()
        : expectedReward(0.0f), probability(1.0f), executionTime(1.0), resourceCost(0.0f), uncertainty(0.0f) {}
};

// Enhanced Action representation with advanced features
class Action {
public:
    Action();
    Action(ActionType type);
    Action(ActionType type, const std::vector<float>& parameters);
    ~Action();
    
    // Action type
    ActionType getType() const;
    void setType(ActionType type);
    
    // Action parameters
    const std::vector<float>& getParameters() const;
    void setParameters(const std::vector<float>& params);
    
    // Get action name
    std::string getName() const;
    
    // Clone
    std::unique_ptr<Action> clone() const;
    
    // Advanced features
    
    // Add consequence for this action
    void addConsequence(const ActionConsequence& consequence);
    
    // Get all consequences
    const std::vector<ActionConsequence>& getConsequences() const;
    
    // Get expected total reward for action
    float getExpectedReward() const;
    
    // Get average execution time
    double getExecutionTime() const;
    
    // Get total resource cost
    float getResourceCost() const;
    
    // Get maximum uncertainty among consequences
    float getUncertainty() const;
    
    // Set action execution state
    void setExecutionState(bool executing) { isExecuting_ = executing; }
    
    // Check if action is currently executing
    bool isExecuting() const { return isExecuting_; }
    
    // Set action completion time
    void setCompletionTime(double completionTime) { completionTime_ = completionTime; }
    
    // Get action completion time
    double getCompletionTime() const { return completionTime_; }
    
    // Get action preference/score
    float getPreference() const { return preference_; }
    
    // Set action preference
    void setPreference(float preference) { preference_ = std::clamp(preference, 0.0f, 1.0f); }
    
    // Check if action is valid for current state
    virtual bool isValid(const std::vector<float>& currentState) const;
    
    // Get action metadata
    const std::string& getMetadata(const std::string& key) const;
    void setMetadata(const std::string& key, const std::string& value);
    
    // Clone with parameters
    std::unique_ptr<Action> cloneWithParameters(const std::vector<float>& newParams) const;

private:
    ActionType type_;
    std::vector<float> parameters_;
    std::vector<ActionConsequence> consequences_;
    bool isExecuting_ = false;
    double completionTime_ = 0.0;
    float preference_ = 0.5f;
    std::unordered_map<std::string, std::string> metadata_;
    static const char* actionTypeToString(ActionType type);
};

} // namespace nlm
