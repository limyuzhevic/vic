"""
Motor and action management for NLM Python bindings.

This module provides enhanced motor functionality for NLM,
including action management, motor decoding, and behavior control.
"""

import enum
from typing import Optional, Dict, Any, List
from enum import Enum as PythonEnum

from .exceptions import ValidationError

# Import C++ types
try:
    from ..bindings import Action, ActionType, MotorCommand
except ImportError as e:
    raise ImportError(f"Failed to import NLM motor bindings: {e}")


class ActionStatus(PythonEnum):
    """Enumeration of action execution status."""
    PENDING = "pending"
    EXECUTING = "executing"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"
class ActionEffect:
    """
    Action effects and outcomes.
    
    Represents the consequences of executing an action,
    including rewards, penalties, and state changes.
    """
    
    def __init__(self):
        """Initialize action effect."""
        self.reward = 0.0
        self.success = False
        self.message = ""
        self.additional_effects = {}
    
    def set_reward(self, reward: float) -> 'ActionEffect':
        """Set reward value."""
        self.reward = reward
        return self
    
    def set_success(self, success: bool) -> 'ActionEffect':
        """Set success status."""
        self.success = success
        return self
    
    def set_message(self, message: str) -> 'ActionEffect':
        """Set message."""
        self.message = message
        return self
    
    def add_effect(self, key: str, value: Any) -> 'ActionEffect':
        """Add additional effect."""
        self.additional_effects[key] = value
        return self
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get effect value."""
        return self.additional_effects.get(key, default)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert effect to dictionary."""
        return {
            'reward': self.reward,
            'success': self.success,
            'message': self.message,
            'additional_effects': self.additional_effects.copy(),
        }
    
    def __repr__(self) -> str:
        status = "✓" if self.success else "✗"
        return f"ActionEffect(reward={self.reward}, {status})"
class NMLAction:
    """
    Enhanced action representation with Pythonic interface.
    
    Provides action functionality with parameters, execution,
    and effect management.
    """
    
    def __init__(self, action_type: ActionType, parameters: List[float] = None):
        """
        Initialize action.
        
        Args:
            action_type: Type of action
            parameters: Action parameters (optional)
        """
        self.type = action_type
        self.parameters = parameters or []
        self.name = self._get_action_name(action_type)
        self.description = self._get_action_description(action_type)
        self.status = ActionStatus.PENDING
        self.effect = ActionEffect()
        self.timestamp = 0.0
        self.identifier = f"action_{int(self.timestamp * 1000)}_{random.randint(0, 10000)}"
    
    def _get_action_name(self, action_type: ActionType) -> str:
        """Get action name from type."""
        names = {
            ActionType.MoveForward: "Move Forward",
            ActionType.MoveBackward: "Move Backward",
            ActionType.MoveLeft: "Move Left",
            ActionType.MoveRight: "Move Right",
            ActionType.TurnLeft: "Turn Left",
            ActionType.TurnRight: "Turn Right",
            ActionType.Look: "Look",
            ActionType.LookUp: "Look Up",
            ActionType.LookDown: "Look Down",
            ActionType.Interact: "Interact",
            ActionType.Eat: "Eat",
            ActionType.Drink: "Drink",
            ActionType.Rest: "Rest",
            ActionType.Wait: "Wait",
            ActionType.Custom: "Custom",
        }
        return names.get(action_type, "Unknown")
    
    def _get_action_description(self, action_type: ActionType) -> str:
        """Get action description from type."""
        descriptions = {
            ActionType.MoveForward: "Move forward one unit",
            ActionType.MoveBackward: "Move backward one unit",
            ActionType.MoveLeft: "Move left one unit",
            ActionType.MoveRight: "Move right one unit",
            ActionType.TurnLeft: "Turn left 90 degrees",
            ActionType.TurnRight: "Turn right 90 degrees",
            ActionType.Look: "Look forward",
            ActionType.LookUp: "Look up",
            ActionType.LookDown: "Look down",
            ActionType.Interact: "Interact with object",
            ActionType.Eat: "Eat food",
            ActionType.Drink: "Drink water",
            ActionType.Rest: "Rest and recover",
            ActionType.Wait: "Wait for next turn",
            ActionType.Custom: "Custom action",
        }
        return descriptions.get(action_type, "No description")
    
    def set_parameter(self, index: int, value: float) -> 'NMLAction':
        """
        Set action parameter.
n        
        Args:
            index: Parameter index
            value: Parameter value
            
        Returns:
            Self for method chaining
        """
        if index < 0:
            raise ValidationError(f"Parameter index must be non-negative, got {index}")
        
        # Extend parameters list if needed
        while len(self.parameters) <= index:
            self.parameters.append(0.0)
        
        self.parameters[index] = value
        return self
    
    def get_parameter(self, index: int, default: float = 0.0) -> float:
        """Get action parameter."""
        if index < len(self.parameters):
            return self.parameters[index]
        return default
    
    def add_parameter(self, value: float) -> 'NMLAction':
        """Add parameter to end of parameters list."""
        self.parameters.append(value)
        return self
    
    def clear_parameters(self) -> 'NMLAction':
        """Clear all parameters."""
        self.parameters.clear()
        return self
    
    def set_status(self, status: ActionStatus) -> 'NMLAction':
        """Set action status."""
        self.status = status
        return self
    
    def execute(self, world) -> ActionEffect:
        """
        Execute action in world.
        
        Args:
            world: World object
            
        Returns:
            Action effect
        """
        self.status = ActionStatus.EXECUTING
        self.timestamp = time.time()
        
        try:
            # In real implementation, this would call world.applyMotorCommand
            # For now, return a mock effect
            effect = ActionEffect()
            
            # Simple effect based on action type
            if self.type == ActionType.MoveForward:
                effect.set_reward(1.0).set_success(True).set_message("Moved forward")
            elif self.type == ActionType.Rest:
                effect.set_reward(0.5).set_success(True).set_message("Restored energy")
            elif self.type == ActionType.Eat:
                effect.set_reward(2.0).set_success(True).set_message("Gained energy")
            else:
                effect.set_reward(0.0).set_success(True).set_message(f"Executed {self.name}")
            
            self.effect = effect
            self.status = ActionStatus.COMPLETED
            
            return effect
            
        except Exception as e:
            self.status = ActionStatus.FAILED
            error_effect = ActionEffect().set_success(False).set_message(str(e))
            self.effect = error_effect
            raise
    
    def is_completed(self) -> bool:
        """Check if action is completed."""
        return self.status == ActionStatus.COMPLETED
    
    def is_failed(self) -> bool:
        """Check if action failed."""
        return self.status == ActionStatus.FAILED
    
    def get_name(self) -> str:
        """Get action name."""
        return self.name
    
    def get_description(self) -> str:
        """Get action description."""
        return self.description
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert action to dictionary."""
        return {
            'id': self.identifier,
            'type': self.type.value,
            'parameters': self.parameters.copy(),
            'name': self.name,
            'description': self.description,
            'status': self.status.value,
            'timestamp': self.timestamp,
            'effect': self.effect.to_dict(),
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'NMLAction':
        """
        Create action from dictionary.
n        
        Args:
            data: Action data dictionary
            
        Returns:
            Created action
        """
        action = cls(
            ActionType(data['type']),
            data['parameters']
        )
        action.identifier = data['id']
        action.status = ActionStatus(data['status'])
        action.timestamp = data['timestamp']
        
        # Reconstruct effect
        effect_data = data['effect']
        action.effect = ActionEffect()
        action.effect.reward = effect_data['reward']
        action.effect.success = effect_data['success']
        action.effect.message = effect_data['message']
        action.effect.additional_effects = effect_data['additional_effects']
        
        return action
    
    def __repr__(self) -> str:
        return f"NMLAction({self.name}, status={self.status.value})"
# Alias for backward compatibility
Action = NMLAction


class NMLMotorCommand:
    """
    Enhanced motor command with Pythonic interface.
    
    Provides motor command functionality with type safety,
    parameter validation, and execution tracking.
    """
    
    def __init__(self, command_type: ActionType, parameters: List[float] = None):
        """
        Initialize motor command.
        
        Args:
            command_type: Type of motor command
            parameters: Command parameters (optional)
        """
        self.type = command_type
        self.parameters = parameters or []
        self.name = self._get_command_name(command_type)
        self.timestamp = 0.0
        self.identifier = f"cmd_{int(self.timestamp * 1000)}_{random.randint(0, 10000)}"
    
    def _get_command_name(self, command_type: ActionType) -> str:
        """Get command name from type."""
        names = {
            ActionType.MoveForward: "MoveForward",
            ActionType.MoveBackward: "MoveBackward",
            ActionType.TurnLeft: "TurnLeft",
            ActionType.TurnRight: "TurnRight",
            ActionType.LookLeft: "LookLeft",
            ActionType.LookRight: "LookRight",
            ActionType.Interact: "Interact",
            ActionType.Wait: "Wait",
        }
        return names.get(command_type, "Unknown")
    
    def set_parameter(self, index: int, value: float) -> 'NMLMotorCommand':
        """
        Set command parameter.
n        
        Args:
            index: Parameter index
            value: Parameter value
            
        Returns:
            Self for method chaining
        """
        if index < 0:
            raise ValidationError(f"Parameter index must be non-negative, got {index}")
        
        # Extend parameters list if needed
        while len(self.parameters) <= index:
            self.parameters.append(0.0)
        
        self.parameters[index] = value
        return self
    
    def get_parameter(self, index: int, default: float = 0.0) -> float:
        """Get command parameter."""
        if index < len(self.parameters):
            return self.parameters[index]
        return default
    
    def add_parameter(self, value: float) -> 'NMLMotorCommand':
        """Add parameter to end of parameters list."""
        self.parameters.append(value)
        return self
    
    def clear_parameters(self) -> 'NMLMotorCommand':
        """Clear all parameters."""
        self.parameters.clear()
        return self
    
    def get_name(self) -> str:
        """Get command name."""
        return self.name
    
    def is_valid(self) -> bool:
        """Check if command is valid."""
        # Basic validation
        return self.type is not None and len(self.parameters) >= 0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert command to dictionary."""
        return {
            'id': self.identifier,
            'type': self.type.value,
            'parameters': self.parameters.copy(),
            'name': self.name,
            'timestamp': self.timestamp,
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'NMLMotorCommand':
        """
        Create command from dictionary.
n        
        Args:
            data: Command data dictionary
            
        Returns:
            Created command
        """
        cmd = cls(
            ActionType(data['type']),
            data['parameters']
        )
        cmd.identifier = data['id']
        cmd.timestamp = data['timestamp']
        
        return cmd
    
    def __repr__(self) -> str:
        return f"NMLMotorCommand({self.name})"
# Alias for backward compatibility
MotorCommand = NMLMotorCommand


# Convenience constructors
def create_action(action_type: ActionType, parameters: List[float] = None) -> NMLAction:
    """
    Create action.
n    
    Args:
        action_type: Type of action
        parameters: Action parameters
        
    Returns:
        NMLAction object
    """
    return NMLAction(action_type, parameters)
def create_motor_command(command_type: ActionType, parameters: List[float] = None) -> NMLMotorCommand:
    """
    Create motor command.
n    
    Args:
        command_type: Type of command
        parameters: Command parameters
        
    Returns:
        NMLMotorCommand object
    """
    return NMLMotorCommand(command_type, parameters)

# Type aliases for convenience
ActionTypeType = ActionType
ActionStatusType = ActionStatus
ActionEffectType = ActionEffect
NMLActionType = NMLAction
NMLMotorCommandType = NMLMotorCommand

# Export all main classes
__all__.extend([
    'ActionType', 'ActionStatus', 'ActionEffect', 'NMLAction', 'NMLMotorCommand',
    'create_action', 'create_motor_command',
    'ActionTypeType', 'ActionStatusType', 'ActionEffectType', 
    'NMLActionType', 'NMLMotorCommandType'
])
