#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Action types for motor output
enum class ActionType {
    // Movement actions
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    TurnLeft,
    TurnRight,
    
    // Gaze/observation actions
    Look,
    LookLeft,
    LookRight,
    LookUp,
    LookDown,
    
    // Dynamic movement actions
    Jump,
    Climb,
    Fall,
    Roll,
    
    // Object interaction actions
    Grab,
    Release,
    Push,
    Pull,
    Lift,
    Drop,
    
    // Social actions
    Communicate,
    Greet,
    Share,
    Compete,
    Collaborate,
    
    // Physiological actions
    Eat,
    Drink,
    Sleep,
    Groom,
    Heal,
    
    // Environmental actions
    Build,
    Destroy,
    Explore,
    Dig,
    Swim,
    Fly,
    
    // Basic actions
    Interact,
    Rest,
    Wait,
    Custom
};

// Action representation
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
    
private:
    ActionType type_;
    std::vector<float> parameters_;
    static const char* actionTypeToString(ActionType type);
};

} // namespace nlm
