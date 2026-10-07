# Motor system implementation for NLM Python API

"""
Motor system for NLM Python API

Provides Pythonic interfaces to motor action systems:
Action, ActionType, and MotorCommand classes.
"""

from typing import List, Any, Optional, Union, Dict
from pathlib import Path

# Import the C++ binding and type definitions
from pynlm.types import ActionType as CppActionType, MotorCommand as CppMotorCommand

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class MotorError(NLMError):
    """Raised when motor operations fail"""
    pass

class ActionType:
    """
    Action type enumeration
    
    Enumeration of possible action types for NLM agents.
    
    Attributes:
        MoveForward: Move forward
        MoveBackward: Move backward
        TurnLeft: Turn left
        TurnRight: Turn right
        Look: Look in current direction
        LookUp: Look up
        LookDown: Look down
        Interact: Interact with objects
        Eat: Eat/eat food
        Drink: Drink water
        Rest: Rest (do nothing)
        Wait: Wait for next timestep
        Custom: Custom action
    """
    
    # Enum values
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    Look = 4
    LookUp = 5
    LookDown = 6
    Interact = 7
    Eat = 8
    Drink = 9
    Rest = 10
    Wait = 11
    Custom = 12
    
    # Mapping for friendly names
    _names = {
        MoveForward: "MoveForward",
        MoveBackward: "MoveBackward",
        TurnLeft: "TurnLeft",
        TurnRight: "TurnRight",
        Look: "Look",
        LookUp: "LookUp",
        LookDown: "LookDown",
        Interact: "Interact",
        Eat: "Eat",
        Drink: "Drink",
        Rest: "Rest",
        Wait: "Wait",
        Custom: "Custom"
    }
    
    @classmethod
    def get_name(cls, value: int) -> str:
        """
        Get friendly name for action type.
        
        Args:
            value: Action type value
            
        Returns:
            Friendly name
        """
        return cls._names.get(value, f"Unknown({value})")
    
    @classmethod
    def is_movement(cls, value: int) -> bool:
        """
        Check if action is a movement action.
        
        Args:
            value: Action type value
            
        Returns:
            True if action is movement
        """
        return value in (cls.MoveForward, cls.MoveBackward, cls.TurnLeft, cls.TurnRight)
    
    @classmethod
    def is_looking(cls, value: int) -> bool:
        """
        Check if action is a looking action.
        
        Args:
            value: Action type value
            
        Returns:
            True if action is looking
        """
        return value in (cls.Look, cls.LookUp, cls.LookDown)
    
    @classmethod
    def is_interaction(cls, value: int) -> bool:
        """
        Check if action is an interaction action.
        
        Args:
            value: Action type value
            
        Returns:
            True if action is interaction
        """
        return value in (cls.Interact, cls.Eat, cls.Drink)

class MotorCommand:
    """
    Low-level motor command enumeration
    
    Enumeration of low-level motor commands for NLM agents.
    
    Attributes:
        MoveForward: Move forward
        MoveBackward: Move backward
        TurnLeft: Turn left
        TurnRight: Turn right
        LookLeft: Look left
        LookRight: Look right
        Interact: Interact with objects
        Wait: Wait
    """
    
    # Enum values
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7
    
    # Mapping for friendly names
    _names = {
        MoveForward: "MoveForward",
        MoveBackward: "MoveBackward",
        TurnLeft: "TurnLeft",
        TurnRight: "TurnRight",
        LookLeft: "LookLeft",
        LookRight: "LookRight",
        Interact: "Interact",
        Wait: "Wait"
    }
    
    @classmethod
    def get_name(cls, value: int) -> str:
        """
        Get friendly name for motor command.
        
        Args:
            value: Motor command value
            
        Returns:
            Friendly name
        """
        return cls._names.get(value, f"Unknown({value})")

class Action:
    """
    Action representation for motor output
    
    Represents a motor action that an agent can execute in the world.
    Contains action type and optional parameters.
    
    Examples:
        >>> action = Action(ActionType.MoveForward)
        >>> action.setType(ActionType.TurnRight)
        >>> params = action.getParameters()
        >>> action.setParameters([0.5, 0.5])  # Direction
        >>> name = action.getName()
    """
    
    def __init__(self, action_type: ActionType = ActionType.Wait, parameters: Optional[List[float]] = None):
        """
        Initialize action.
        
        Args:
            action_type: Action type
            parameters: Optional action parameters
            
        Raises:
            MotorError: If action cannot be created
        """
        # Note: In actual implementation, this would wrap C++ Action object
        self._action_type = action_type
        self._parameters = parameters or []
        self._initialized = True
    
    def get_type(self) -> ActionType:
        """
        Get action type.
        
        Returns:
            Action type
            
        Raises:
            MotorError: If action type cannot be retrieved
        """
        return self._action_type
    
    def set_type(self, action_type: ActionType) -> None:
        """
        Set action type.
        
        Args:
            action_type: Action type
            
        Raises:
            MotorError: If action type cannot be set
        """
        self._action_type = action_type
    
    def get_parameters(self) -> List[float]:
        """
        Get action parameters.
        
        Returns:
            Action parameters
            
        Raises:
            MotorError: If parameters cannot be retrieved
        """
        return self._parameters.copy()
    
    def set_parameters(self, parameters: List[float]) -> None:
        """
        Set action parameters.
        
        Args:
            parameters: Action parameters
            
        Raises:
            MotorError: If parameters cannot be set
        """
        self._parameters = parameters.copy()
    
    def add_parameter(self, parameter: float) -> None:
        """
        Add a parameter to action.
        
        Args:
            parameter: Parameter to add
            
        Raises:
            MotorError: If parameter cannot be added
        """
        self._parameters.append(parameter)
    
    def get_name(self) -> str:
        """
        Get human-readable action name.
        
        Returns:
            Action name
            
        Raises:
            MotorError: If name cannot be retrieved
        """
        return ActionType.get_name(self._action_type)
    
    def is_movement(self) -> bool:
        """
        Check if action is a movement action.
        
        Returns:
            True if action is movement
            
        Raises:
            MotorError: If check cannot be performed
        """
        return ActionType.is_movement(self._action_type)
    
    def is_looking(self) -> bool:
        """
        Check if action is a looking action.
        
        Returns:
            True if action is looking
            
        Raises:
            MotorError: If check cannot be performed
        """
        return ActionType.is_looking(self._action_type)
    
    def is_interaction(self) -> bool:
        """
        Check if action is an interaction action.
        
        Returns:
            True if action is interaction
            
        Raises:
            MotorError: If check cannot be performed
        """
        return ActionType.is_interaction(self._action_type)
    
    def clone(self) -> 'Action':
        """
        Create a copy of this action.
        
        Returns:
            Cloned action
            
        Raises:
            MotorError: If cloning fails
        """
        return Action(self._action_type, self._parameters.copy())
    
    def __repr__(self) -> str:
        """String representation of action"""
        name = self.get_name()
        params_str = f", params={self._parameters}" if self._parameters else ""
        return f"<Action {name}{params_str}>"
    
    def __str__(self) -> str:
        """String representation of action"""
        return self.__repr__()

# Factory functions for motor actions
def create_action(action_type: ActionType = ActionType.Wait, parameters: Optional[List[float]] = None) -> Action:
    """
    Create an action.
    
    Args:
        action_type: Action type
        parameters: Optional action parameters
        
    Returns:
        New Action instance
        
    Examples:
        >>> action = createAction(ActionType.MoveForward)
        >>> action = createAction(ActionType.Custom, [0.5, 0.5])
    """
    return Action(action_type, parameters)

# Movement convenience functions
def move_forward() -> Action:
    """Create a move forward action."""
    return Action(ActionType.MoveForward)

def move_backward() -> Action:
    """Create a move backward action."""
    return Action(ActionType.MoveBackward)

def turn_left() -> Action:
    """Create a turn left action."""
    return Action(ActionType.TurnLeft)

def turn_right() -> Action:
    """Create a turn right action."""
    return Action(ActionType.TurnRight)

def look() -> Action:
    """Create a look action."""
    return Action(ActionType.Look)

def look_up() -> Action:
    """Create a look up action."""
    return Action(ActionType.LookUp)

def look_down() -> Action:
    """Create a look down action."""
    return Action(ActionType.LookDown)

def interact() -> Action:
    """Create an interact action."""
    return Action(ActionType.Interact)

def eat() -> Action:
    """Create an eat action."""
    return Action(ActionType.Eat)

def drink() -> Action:
    """Create a drink action."""
    return Action(ActionType.Drink)

def rest() -> Action:
    """Create a rest action."""
    return Action(ActionType.Rest)

def wait() -> Action:
    """Create a wait action."""
    return Action(ActionType.Wait)

def create_custom_action(parameters: List[float]) -> Action:
    """
    Create a custom action.
    
    Args:
        parameters: Action parameters
        
    Returns:
        Custom action
        
    Examples:
        >>> custom_action = createCustomAction([0.5, 0.8, 0.3])
    """
    return Action(ActionType.Custom, parameters)