#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Action types for motor output
// 
// Enumerates the possible motor actions that an agent can perform.
// These represent semantic actions (what to do) as opposed to low-level
// motor commands (how to do it). Actions are produced by the brain and
// interpreted by the world to produce actual movement.
// 
// Action types are categorized:
// - Movement: Change agent position
// - Interaction: Modify world state
// - Perception: Change sensory focus
// - State changes: Modify internal state
// - Waiting: Do nothing
// - Custom: User-defined actions
// 
// Each action type can have parameters for fine-grained control
// (e.g., how far to move, at what angle, etc.).
// 
// Example usage:
//   Action moveForward(1.0f);           // Move forward 1 unit
//   Action turnLeft(0.5f);              // Turn left 0.5 radians
//   Action look(ActionType::LookUp);    // Look up
//   Action eat();                       // Eat nearby resource
//   
// Actions are created by the brain and decoded by AgentBrain to
// low-level MotorCommand values for world execution.

enum class ActionType {
    MoveForward,    // Move forward in current direction
    MoveBackward,   // Move backward
    MoveLeft,       // Strafe left
    MoveRight,      // Strafe right
    TurnLeft,       // Rotate counterclockwise
    TurnRight,      // Rotate clockwise
    Look,           // Look straight ahead
    LookUp,         // Look upward
    LookDown,       // Look downward
    Interact,       // Interact with nearby object
    Eat,            // Consume edible resource
    Drink,          // Consume drinkable resource
    Rest,           // Stop moving and rest
    Wait,           // Do nothing this step
    Custom          // User-defined action type
};

// Action representation
// 
// Represents a motor action with a type and optional parameters.
// Actions are produced by the brain and transmitted to the world
// for execution. Each action has:
// - A type from the ActionType enumeration
// - A list of floating-point parameters for fine control
// - A name for human-readable display
// 
// Actions are lightweight and can be cloned for multiple uses.
// They are used in the brain's action production system and are
// decoded by the AgentBrain into low-level motor commands.
// 
// Key characteristics:
// - Type determines the basic action (Move, Turn, Look, Interact, etc.)
// - Parameters vary by type (e.g., distance for Move, angle for Turn)
// - Empty parameters use reasonable defaults
// - Can be cloned for thread-safe usage
// - Human-readable names for debugging
// 
// Example usage:
//   // Create simple actions
//   Action moveForward;
//   Action moveForward(1.0f);
//   Action turnLeft(0.5f);
//   Action lookUp;
//   
//   // Modify actions
//   action.setType(ActionType::MoveForward);
//   action.setParameters({1.5f, 0.0f});  // Move forward 1.5 units
//   
//   // Get action info
//   std::string name = action.getName();
//   ActionType type = action.getType();
//   auto params = action.getParameters();
//   
//   // Clone actions
//   auto copy = action.clone();

class Action {
public:
    //! @brief Default constructor
    //! 
    //! Creates a default Wait action with empty parameters.
    //! 
    //! Example:
    //!   Action action;  // Creates Wait action

    Action();
    
    //! @brief Constructor with type
    //! 
    //! Creates an action with the specified type. Parameters use defaults
    //! based on the action type.
    //! 
    //! Parameters:
    //!   type: Action type to create
    //!   
    //! Example:
    //!   Action moveForward(ActionType::MoveForward);
    //!   Action turnLeft(ActionType::TurnLeft);

    Action(ActionType type);
    
    //! @brief Constructor with type and parameters
    //! 
    //! Creates an action with type and parameters for fine control.
    //! 
    //! Parameters:
    //!   type: Action type
    //!   parameters: Vector of float parameters specific to the action type
    //!   
    //! Example:
    //!   Action moveForward(ActionType::MoveForward, std::vector<float>{1.5f});
    //!   Action turnLeft(ActionType::TurnLeft, std::vector<float>{0.5f});

    Action(ActionType type, const std::vector<float>& parameters);
    
    //! @brief Destructor
    //! 
    //! Cleans up action resources.

    ~Action();
    
    //! @brief Get action type
    //! 
    //! Returns the type of this action.
    //! 
    //! Returns:
    //!   ActionType The action type
    //!   
    //! Example:
    //!   ActionType type = action->getType();
    //!   if (type == ActionType::MoveForward) {
    //!       // Handle movement
    //!   }

    ActionType getType() const;

    //! @brief Set action type
    //! 
    //! Changes the action type. Parameters will be reset to defaults
    //! or adjusted based on the new type.
    //! 
    //! Parameters:
    //!   type: New action type
    //!   
    //! Example:
    //!   action->setType(ActionType::TurnLeft);

    void setType(ActionType type);

    //! @brief Get action parameters
    //! 
    //! Returns the parameters vector for this action.
    //! Parameters are action-specific:
    //! - Move actions: distance, optional strafe
    //! - Turn actions: angle
    //! - Look actions: pitch, yaw
    //! - Interact actions: optional interaction parameters
    //! - Eat/Drink: optional quantity
    //! 
    //! Returns:
    //!   const std::vector<float>& Reference to parameters vector
    //!   
    //! Example:
    //!   auto params = action->getParameters();
    //!   if (!params.empty()) {
    //!       float distance = params[0];
    //!   }

    const std::vector<float>& getParameters() const;

    //! @brief Set action parameters
    //! 
    //! Sets the parameters for this action. The number and meaning
    //! of parameters depends on the action type.
    //! 
    //! Parameters:
    //!   params: Vector of float parameters
    //!   
    //! Example:
    //!   action->setParameters(std::vector<float>{1.5f, 0.0f});

    void setParameters(const std::vector<float>& params);

    //! @brief Get action name
    //! 
    //! Returns a human-readable name for this action type.
    //! Useful for debugging, logging, and user interfaces.
    //! 
    //! Returns:
    //!   std::string Human-readable action name
    //!   
    //! Example:
    //!   std::string name = action->getName();
    //!   std::cout << "Action: " << name << std::endl;
    //!   // Output: "Move Forward" or "Turn Left"

    std::string getName() const;

    //! @brief Clone
    //! 
    //! Creates a deep copy of this action.
    //! 
    //! Returns:
    //!   std::unique_ptr<Action> Copy of this action
    //!   
    //! Example:
    //!   auto copy = original->clone();
    //!   // Use copy independently

    std::unique_ptr<Action> clone() const;
    
private:
    ActionType type_;
    std::vector<float> parameters_;
    static const char* actionTypeToString(ActionType type);
};

} // namespace nlm
