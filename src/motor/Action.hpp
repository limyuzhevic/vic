#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Action types for motor output
enum class ActionType {
    MoveForward,      // Move forward with speed parameter (0.0-1.0)
    MoveBackward,     // Move backward with speed parameter (0.0-1.0)
    MoveLeft,         // Move left with speed parameter (0.0-1.0)
    MoveRight,        // Move right with speed parameter (0.0-1.0)
    TurnLeft,         // Rotate left with speed parameter (0.0-1.0, radians/sec)
    TurnRight,        // Rotate right with speed parameter (0.0-1.0, radians/sec)
    Look,             // Look in direction with direction parameter (0.0-360 degrees)
    LookUp,           // Look up with angle parameter (0.0-90 degrees)
    LookDown,         // Look down with angle parameter (0.0-90 degrees)
    Interact,         // Interact with object, with strength parameter (0.0-1.0)
    Eat,              // Eat object with quantity parameter (0.0-1.0)
    Drink,            // Drink object with quantity parameter (0.0-1.0)
    Rest,             // Rest, with duration parameter (0.0-1.0, as fraction of max rest)
    Wait,             // No action, with probability parameter (0.0-1.0)
    Custom            // Custom action, with arbitrary parameters
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
