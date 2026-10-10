#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

/**
 * @file Action.hpp
 * 
 * Motor action system for NLM
 * 
 * This file provides the motor action system for NLM, implementing:
 * - Action types for motor output
 * - Action representation
 * - Action management
 * - Action cloning
 * - Action naming
 * 
 * The motor action system in NLM:
 * - **ActionType**: Action types for motor output
 * - **Action**: Action representation
 * - **Action management**: Manages actions
 * - **Action cloning**: Clones actions
 * - **Action naming**: Provides action names
 * 
 * Key features:
 * - **Action types**: Action types for motor output
 * - **Action representation**: Action representation
 * - **Action management**: Manages actions
 * - **Action cloning**: Clones actions
 * - **Action naming**: Provides action names
 * 
 * Action characteristics in NLM:
 * - **Action types**: Action types for motor output
 * - **Action representation**: Action representation
 * - **Action management**: Manages actions
 * - **Action cloning**: Clones actions
 * - **Action naming**: Provides action names
 * 
 * @note Critical for motor output
 * @note Used extensively in motor systems
 * @note Essential for action execution
 * @see Action.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @enum ActionType
 * 
 * Neural implementation of action types.
 * 
 * This enumeration implements the action types for NLM motor output.
 * It is essential for:
 * - **Action types**: Action types for motor output
 * - **Action representation**: Action representation
 * - **Action management**: Manages actions
 * - **Action cloning**: Clones actions
 * - **Action naming**: Provides action names
 * 
 * Action characteristics in NLM:
 * - **Action types**: Action types for motor output
 * - **Action representation**: Action representation
 * - **Action management**: Manages actions
 * - **Action cloning**: Clones actions
 * - **Action naming**: Provides action names
 * 
 * @note Action types for motor output
 * 
 * @author NLM Development Team
 */
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
 * 
 * Neural implementation of action representation.
 * 
 * This class implements the action representation for NLM, providing
 * action representation for motor output. It is essential for:
 * - **Action types**: Action types for motor output
 * - **Action representation**: Action representation
 * - **Action management**: Manages actions
 * - **Action cloning**: Clones actions
 * - **Action naming**: Provides action names
 * 
 * Action characteristics in NLM:
 * - **Action types**: Action types for motor output
 * - **Action representation**: Action representation
 * - **Action management**: Manages actions
 * - **Action cloning**: Clones actions
 * - **Action naming**: Provides action names
 * 
 * Key components:
 * - **Action type**: Returns action type
 * - **Action parameters**: Manages action parameters
 * - **Action name**: Provides action name
 * - **Cloning**: Clones action
 * 
 * @note Critical for motor output
 * @see Action.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class Action {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default action instance with default parameters.
     * The action is initialized with default values for
     * all action fields.
     */
    Action();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates an action instance with specified action type.
     * The action is initialized with specified action type.
     */
    Action(ActionType type);
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates an action instance with specified action type and parameters.
     * The action is initialized with specified action type and parameters.
     */
    Action(ActionType type, const std::vector<float>& parameters);
    
    /**
     * @brief Destructor
     * 
     * Cleans up action resources and performs any necessary
     * cleanup operations. The action is left in a valid but
     * non-operational state after destruction.
     */
    ~Action();
    
    /**
     * @brief Get action type
     * 
     * Returns the action type. This is used for:
     * - Action type access
     * - Action type management
     * - Action type processing
     * - Action type control
     */
    ActionType getType() const;
    
    /**
     * @brief Set action type
     * 
     * Sets the action type. This is used for:
     * - Action type setting
     * - Action type management
     * - Action type processing
     * - Action type control
     */
    void setType(ActionType type);
    
    /**
     * @brief Get action parameters
     * 
     * Returns the action parameters. This is used for:
     * - Action parameter access
     * - Action parameter management
     * - Action parameter processing
     * - Action parameter control
     */
    const std::vector<float>& getParameters() const;
    
    /**
     * @brief Set action parameters
     * 
     * Sets the action parameters. This is used for:
     * - Action parameter setting
     * - Action parameter management
     * - Action parameter processing
     * - Action parameter control
     */
    void setParameters(const std::vector<float>& params);
    
    /**
     * @brief Get action name
     * 
     * Returns the action name. This is used for:
     * - Action name access
     * - Action name management
     * - Action name processing
     * - Action name control
     */
    std::string getName() const;
    
    /**
     * @brief Clone
     * 
     * Clones the action. This is used for:
     * - Action cloning
     * - Action duplication
     * - Action copying
     * - Action replication
     */
    std::unique_ptr<Action> clone() const;
    
private:
    /** Action type */
    ActionType type_;
    
    /** Action parameters */
    std::vector<float> parameters_;
    
    /** Convert action type to string */
    static const char* actionTypeToString(ActionType type);
};

} // namespace nlm
