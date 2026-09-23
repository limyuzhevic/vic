#!/usr/bin/env python3
"""
NLM Motor Module

Provides action generation capabilities for NLM (Neural Learning Machine).
"""

from typing import List, Optional, Union

class ActionType:
    """Enumeration of action types."""
    MoveForward = "move_forward"
    MoveBackward = "move_backward"
    TurnLeft = "turn_left"
    TurnRight = "turn_right"
    Look = "look"
    Interact = "interact"
    Eat = "eat"
    Drink = "drink"
    Explore = "explore"

class Action:
    """
    Represents an action that an NLM agent can perform.
    
    This class represents the actions that NLM agents can generate and
    execute in response to their sensory inputs.
    """
    
    def __init__(self, action_type: str, parameters: Optional[List[float]] = None):
        """Create a new action with the given type and parameters."""
        self._type = action_type
        self._parameters = parameters or []
    
    def getType(self) -> str:
        """Get the type of the action."""
        return self._type
    
    def get_name(self) -> str:
        """Get a human-readable name for the action."""
        names = {
            ActionType.MoveForward: "Move Forward",
            ActionType.MoveBackward: "Move Backward",
            ActionType.TurnLeft: "Turn Left",
            ActionType.TurnRight: "Turn Right",
            ActionType.Look: "Look",
            ActionType.Interact: "Interact",
            ActionType.Eat: "Eat",
            ActionType.Drink: "Drink",
            ActionType.Explore: "Explore",
        }
        return names.get(self._type, self._type)
    
    def getParameters(self) -> List[float]:
        """Get the action parameters."""
        return self._parameters.copy()
    
    def get_parameters(self) -> List[float]:
        """Get the action parameters (underscore alias)."""
        return self.getParameters()
    
    def getParameter(self, index: int) -> float:
        """Get a specific parameter value."""
        if 0 <= index < len(self._parameters):
            return self._parameters[index]
        return 0.0
    
    def setParameter(self, index: int, value: float):
        """Set a specific parameter value."""
        if 0 <= index < len(self._parameters):
            self._parameters[index] = value
    
    def get_type(self) -> str:
        """Get the type of the action (alias for getType)."""
        return self.getType()
    
    def clone(self) -> 'Action':
        """Create a copy of this action."""
        return Action(self._type, self._parameters.copy())
    
    def __str__(self):
        return f"Action(type={self._type}, parameters={self._parameters})"

class MotorCommand:
    """
    Represents a motor command to be sent to an NLM agent's actuators.
    
    This class represents the low-level motor commands that control
    an NLM agent's physical movements.
    """
    
    def __init__(self, action: Action, strength: float = 1.0, duration: float = 1.0):
        """Create a new motor command with the given action."""
        self._action = action
        self._strength = strength
        self._duration = duration
    
    def getAction(self) -> Action:
        """Get the action component of the motor command."""
        return self._action
    
    def getStrength(self) -> float:
        """Get the strength of the motor command."""
        return self._strength
    
    def getDuration(self) -> float:
        """Get the duration of the motor command."""
        return self._duration
    
    def setStrength(self, strength: float):
        """Set the strength of the motor command."""
        self._strength = strength
    
    def setDuration(self, duration: float):
        """Set the duration of the motor command."""
        self._duration = duration
    
    def __str__(self):
        return f"MotorCommand(action={self._action}, strength={self._strength}, duration={self._duration})"

def create_action(action_type: str, parameters: Optional[List[float]] = None) -> Action:
    """Create a new action with the given type and parameters."""
    return Action(action_type, parameters or [])

def demo_action_system():
    """Run a demo of the action system."""
    print("=== NLM Action System Demo ===")
    
    # Create various actions
    move_forward = create_action(ActionType.MoveForward)
    look_right = create_action(ActionType.Look, [0.5, 0.0])
    interact = create_action(ActionType.Interact)
    
    actions = [move_forward, look_right, interact]
    
    print("Available actions:")
    for action in actions:
        print(f"  - {action.getType()} -> {action.get_name()}")
    
    # Test action cloning
    original = create_action(ActionType.MoveForward, [0.8])
    cloned = original.clone()
    
    print(f"\nAction cloning test:")
    print(f"  - Original: {original.getType()} with parameters {original.getParameters()}")
    print(f"  - Cloned: {cloned.getType()} with parameters {cloned.getParameters()}")
    
    # Create motor commands
    cmd1 = MotorCommand(move_forward, strength=0.8, duration=1.0)
    cmd2 = MotorCommand(look_right, strength=0.5, duration=0.5)
    
    print(f"\nMotor commands:")
    print(f"  - {cmd1}")
    print(f"  - {cmd2}")
    
    print("\nAction system demo completed!")
    return [move_forward, look_right, interact]
