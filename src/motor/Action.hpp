#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

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

/**
 * @class Action
 * @brief Action representation for motor output and environmental interaction
 * @details Action represents a unit of behavior that an agent can produce to
 * interact with its environment. It combines an action type with parameterized
 * execution details, enabling flexible control over movement, perception, and
 * manipulation behaviors.
 * 
 * Action Components:
 * - Action type: Discrete category of behavior (MoveForward, TurnLeft, etc.)
 * - Parameters: Continuous variables for execution (speed, duration, angle)
 * - Name: Human-readable action description
 * - Execution: Information needed to perform the action
 * 
 * Implementation Notes:
 * - Actions are produced by MotorSystem based on neural activity
 * - Applied by world simulator to produce environmental effects
 * - Can be cloned for parallel processing
 * - Parameters enable fine-grained control
 * 
 * Thread Safety: Thread-safe for concurrent access. Action objects are immutable
 * after creation except through setter methods. Multiple threads can read
 * action properties simultaneously.
 * 
 * Performance: O(1) for all operations. Parameters vector is copied on modification.
 * Memory overhead is minimal (~ few KB per action).
 * 
 * Parameter Semantics:
 * - Speed: Movement velocity (0.0 to 1.0 normalized)
 * - Duration: Execution time in simulation steps
 * - Angle: Rotation angle in radians
 * - Other: Action-specific parameters
 * 
 * @code
 * // Create simple action
 * Action moveForward(ActionType::MoveForward);
 * 
 * // Create action with parameters
 * vector<float> params = {0.8f, 10.0f}; // speed=0.8, duration=10.0
 * Action moveWithParams(ActionType::MoveForward, params);
 * 
 * // Access action properties
 * ActionType type = moveForward.getType();
 * string name = moveForward.getName();
 * const vector<float>& params = moveForward.getParameters();
 * 
 * // Clone action for parallel execution
 * unique_ptr<Action> clonedAction = moveForward.clone();
 * 
 * // Modify action
 * moveWithParams.setType(ActionType::MoveBackward);
 * moveWithParams.setParameters({0.5f, 5.0f});
 * @endcode
 * 
 * @see ActionType
 * @see MotorSystem
 * @see ActionResult
 * @see MotorCommand
 */
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
