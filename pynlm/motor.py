"""
Pythonic interface for NLM motor system.

This module provides interfaces for motor control and action execution in the NLM
system, including action types, motor commands, and movement controls.
"""

from typing import Any

# Import the C++ extension module directly
import pynlm as _c_extension

if _c_extension is None:
    raise ImportError(
        "NLM C++ extension not found. Please build the extension first:\n"
        "mkdir build && cd build && cmake .. && make"
    )
# Re-export all enum types from C++ extension for Python access
NeuronType = _c_extension.NeuronType
SynapseType = _c_extension.SynapseType
DevelopmentalStage = _c_extension.DevelopmentalStage
FiringState = _c_extension.FiringState
ActionType = _c_extension.ActionType
MotorCommand = _c_extension.MotorCommand
WorldObjectType = _c_extension.WorldObjectType

# Convenience functions for common action types
ACTION_TYPES = {
    "move_forward": ActionType.MoveForward,
    "move_backward": ActionType.MoveBackward,
    "move_left": ActionType.MoveLeft,
    "move_right": ActionType.MoveRight,
    "turn_left": ActionType.TurnLeft,
    "turn_right": ActionType.TurnRight,
    "look": ActionType.Look,
    "look_up": ActionType.LookUp,
    "look_down": ActionType.LookDown,
    "interact": ActionType.Interact,
    "eat": ActionType.Eat,
    "drink": ActionType.Drink,
    "rest": ActionType.Rest,
    "wait": ActionType.Wait,
    "custom": ActionType.Custom,
}

MOTOR_COMMANDS = {
    "move_forward": MotorCommand.MoveForward,
    "move_backward": MotorCommand.MoveBackward,
    "turn_left": MotorCommand.TurnLeft,
    "turn_right": MotorCommand.TurnRight,
    "look_left": MotorCommand.LookLeft,
    "look_right": MotorCommand.LookRight,
    "interact": MotorCommand.Interact,
    "wait": MotorCommand.Wait,
}

# Helper functions for common actions
def create_movement_action(direction: str, magnitude: float = 1.0) -> Any:
    """Create a movement action.
    
    Args:
        direction: Movement direction ("forward", "backward", "left", "right").
        magnitude: Movement magnitude (default: 1.0).
        
    Returns:
        Action object.
        
    Examples:
        >>> from pynlm.motor import create_movement_action
        >>> action = create_movement_action("forward", 2.0)
    """
    from .agent import Action
    action_type_map = {
        "forward": ActionType.MoveForward,
        "backward": ActionType.MoveBackward,
        "left": ActionType.MoveLeft,
        "right": ActionType.MoveRight,
    }
    
    action = Action()
    action.set_type(action_type_map[direction])
    action.set_parameters([magnitude])
    return action
def create_rotation_action(direction: str, angle: float) -> Any:
    """Create a rotation action.
    
    Args:
        direction: Rotation direction ("left", "right").
        angle: Rotation angle in radians.
        
    Returns:
        Action object.
        
    Examples:
        >>> from pynlm.motor import create_rotation_action
        >>> action = create_rotation_action("left", 0.5)
    """
    from .agent import Action
    action_type_map = {
        "left": ActionType.TurnLeft,
        "right": ActionType.TurnRight,
    }
    
    action = Action()
    action.set_type(action_type_map[direction])
    action.set_parameters([angle])
    return action
def is_valid_motor_command(command: Any) -> bool:
    """Check if motor command is valid.
    
    Args:
        command: Motor command to validate.
        
    Returns:
        True if valid, False otherwise.
        
    Examples:
        >>> from pynlm.motor import is_valid_motor_command
        >>> from pynlm import MotorCommand
        >>> valid = is_valid_motor_command(MotorCommand.MoveForward)
    """
    return command in MOTOR_COMMANDS.values()