"""Motor action implementation with Pythonic interface"""

from typing import List, Dict, Any, Optional
from dataclasses import dataclass
from enum import Enum
import numpy as np
class ActionType(Enum):
    """Action types for motor control."""
    
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"
@dataclass
class ActionParameters:
    """Parameters for action execution."""
    
    magnitude: float = 1.0
    direction: float = 0.0
    duration: float = 0.1
    intensity: float = 1.0
    custom_data: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.custom_data is None:
            self.custom_data = {}
class MotorCommand:
    """Motor command with Pythonic interface."""
    
    def __init__(self, action_type: ActionType = ActionType.WAIT, 
                 parameters: ActionParameters = None):
        self._action_type = action_type
        self._parameters = parameters or ActionParameters()
        self._timestamp = 0.0
        self._executed = False
    
    def set_action_type(self, action_type: ActionType):
        """Set action type."""
        self._action_type = action_type
    
    def get_action_type(self) -> ActionType:
        """Get action type."""
        return self._action_type
    
    def set_parameters(self, parameters: ActionParameters):
        """Set action parameters."""
        self._parameters = parameters
    
    def get_parameters(self) -> ActionParameters:
        """Get action parameters."""
        return self._parameters
    
    def set_magnitude(self, magnitude: float):
        """Set action magnitude."""
        self._parameters.magnitude = magnitude
    
    def get_magnitude(self) -> float:
        """Get action magnitude."""
        return self._parameters.magnitude
    
    def set_direction(self, direction: float):
        """Set action direction (radians)."""
        self._parameters.direction = direction
    
    def get_direction(self) -> float:
        """Get action direction."""
        return self._parameters.direction
    
    def set_duration(self, duration: float):
        """Set action duration."""
        self._parameters.duration = duration
    
    def get_duration(self) -> float:
        """Get action duration."""
        return self._parameters.duration
    
    def set_intensity(self, intensity: float):
        """Set action intensity."""
        self._parameters.intensity = intensity
    
    def get_intensity(self) -> float:
        """Get action intensity."""
        return self._parameters.intensity
    
    def add_custom_parameter(self, key: str, value: Any):
        """Add custom parameter."""
        self._parameters.custom_data[key] = value
    
    def get_custom_parameter(self, key: str, default=None):
        """Get custom parameter."""
        return self._parameters.custom_data.get(key, default)
    
    def get_custom_parameters(self) -> Dict[str, Any]:
        """Get all custom parameters."""
        return self._parameters.custom_data.copy()
    
    def set_timestamp(self, timestamp: float):
        """Set execution timestamp."""
        self._timestamp = timestamp
    
    def get_timestamp(self) -> float:
        """Get execution timestamp."""
        return self._timestamp
    
    def mark_executed(self):
        """Mark action as executed."""
        self._executed = True
    
    def is_executed(self) -> bool:
        """Check if action has been executed."""
        return self._executed
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert motor command to dictionary."""
        return {
            'action_type': self._action_type.value,
            'magnitude': self._parameters.magnitude,
            'direction': self._parameters.direction,
            'duration': self._parameters.duration,
            'intensity': self._parameters.intensity,
            'custom_parameters': self._parameters.custom_data,
            'timestamp': self._timestamp,
            'executed': self._executed
        }
    
    def __repr__(self):
        return f"<MotorCommand({self._action_type.value}, magnitude={self._parameters.magnitude:.2f})>"


# Legacy Action class for backward compatibility

class Action:
    """Legacy action class for backward compatibility."""
    
    def __init__(self, action_type: ActionType = ActionType.WAIT,
                 parameters: Optional[List[float]] = None):
        self._action_type = action_type
        self._parameters = parameters or []
        self._timestamp = 0.0
        self._executed = False
    
    def getType(self) -> ActionType:
        """Get action type (legacy method)."""
        return self._action_type
    
    def setType(self, action_type: ActionType):
        """Set action type (legacy method)."""
        self._action_type = action_type
    
    def getParameters(self) -> List[float]:
        """Get action parameters (legacy method)."""
        return self._parameters.copy()
    
    def setParameters(self, parameters: List[float]):
        """Set action parameters (legacy method)."""
        self._parameters = parameters.copy()
    
    def getName(self) -> str:
        """Get action name (legacy method)."""
        return self._action_type.value
    
    def clone(self) -> 'Action':
        """Clone action (legacy method)."""
        return Action(self._action_type, self._parameters.copy())
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert action to dictionary (legacy method)."""
        return {
            'action_type': self._action_type.value,
            'parameters': self._parameters,
            'timestamp': self._timestamp,
            'executed': self._executed
        }
    
    def __repr__(self):
        return f"<Action({self._action_type.value}, params={len(self._parameters)})>"


# Action Result class

class ActionResult:
    """Action result with Pythonic interface."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        self._reward = reward
        self._success = success
        self._message = message
        self._timestamp = 0.0
        self._action_executed = False
    
    def getReward(self) -> float:
        """Get reward."""
        return self._reward
    
    def setReward(self, reward: float):
        """Set reward."""
        self._reward = reward
    
    def getSuccess(self) -> bool:
        """Check if action was successful."""
        return self._success
    
    def setSuccess(self, success: bool):
        """Set success status."""
        self._success = success
    
    def getMessage(self) -> str:
        """Get result message."""
        return self._message
    
    def setMessage(self, message: str):
        """Set result message."""
        self._message = message
    
    def setTimestamp(self, timestamp: float):
        """Set result timestamp."""
        self._timestamp = timestamp
    
    def getTimestamp(self) -> float:
        """Get result timestamp."""
        return self._timestamp
    
    def markExecuted(self):
        """Mark result as from executed action."""
        self._action_executed = True
    
    def isFromExecutedAction(self) -> bool:
        """Check if result is from executed action."""
        return self._action_executed
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert action result to dictionary."""
        return {
            'reward': self._reward,
            'success': self._success,
            'message': self._message,
            'timestamp': self._timestamp,
            'from_executed_action': self._action_executed
        }
    
    def __repr__(self):
        return f"<ActionResult(reward={self._reward:.3f}, success={self._success}, message='{self._message}')>"


# Factory functions

def create_motor_command(action_type: ActionType = ActionType.WAIT,
                        magnitude: float = 1.0,
                        direction: float = 0.0,
                        duration: float = 0.1,
                        intensity: float = 1.0) -> MotorCommand:
    """Create motor command with parameters."""
    params = ActionParameters(
        magnitude=magnitude,
        direction=direction,
        duration=duration,
        intensity=intensity
    )
    return MotorCommand(action_type, params)


def create_action(action_type: ActionType = ActionType.WAIT,
                 parameters: Optional[List[float]] = None) -> Action:
    """Create action (legacy factory function)."""
    return Action(action_type, parameters)


def create_action_result(reward: float = 0.0, success: bool = False, 
                         message: str = "") -> ActionResult:
    """Create action result."""
    return ActionResult(reward, success, message)


# Convenience functions

def get_action_type_names() -> List[str]:
    """Get all action type names."""
    return [action_type.value for action_type in ActionType]


def get_movement_actions() -> List[ActionType]:
    """Get movement-related action types."""
    return [ActionType.MOVE_FORWARD, ActionType.MOVE_BACKWARD,
            ActionType.MOVE_LEFT, ActionType.MOVE_RIGHT,
            ActionType.TURN_LEFT, ActionType.TURN_RIGHT]


def get_interaction_actions() -> List[ActionType]:
    """Get interaction-related action types."""
    return [ActionType.INTERACT, ActionType.EAT, ActionType.DRINK]


# Export for backward compatibility
ActionType = ActionType
ActionParameters = ActionParameters
MotorCommand = MotorCommand
ActionResult = ActionResult