"""
Motor system for NLM - action representation and output.

This module provides the motor system that generates actions and
interfaces the NLM brain with the environment.
"""

import sys
from typing import List, Dict, Any

__all__ = [
    'Action',
    'ActionType',
    'MotorCommand',
    'ActionResult',
]


class Action:
    """Action representation for motor output.
    
    This class represents an action that the agent can take in the
    environment. It includes action type, parameters, and name for
    human-readable description.
    
    Attributes:
        action_type: Type of action (e.g., "MoveForward", "Look").
        parameters: Action parameters (e.g., speed, direction).
    """
    
    def __init__(self, action_type: str = "Wait", parameters: List[float] = None):
        self._action_type = action_type
        self._parameters = parameters or []
    
    def getType(self) -> str:
        """Get action type.
        
        Returns:
            str: Action type.
        """
        return self._action_type
    
    def setType(self, action_type: str) -> bool:
        """Set action type.
        
        Args:
            action_type: New action type.
            
        Returns:
            bool: True if action type set successfully.
        """
        self._action_type = action_type
        return True
    
    def getParameters(self) -> List[float]:
        """Get action parameters.
        
        Returns:
            List[float]: Action parameters.
        """
        return self._parameters
    
    def setParameters(self, parameters: List[float]) -> bool:
        """Set action parameters.
        
        Args:
            parameters: New action parameters.
            
        Returns:
            bool: True if parameters set successfully.
        """
        self._parameters = parameters
        return True
    
    def getName(self) -> str:
        """Get human-readable action name.
        
        Returns:
            str: Action name.
        """
        return self._action_type.replace("_", " ").title()
    
    def clone(self) -> 'Action':
        """Create a copy of this action.
        
        Returns:
            Action: Copy of this action.
        """
        return Action(self._action_type, self._parameters.copy())
    
    def __repr__(self) -> str:
        return f"<Action: type={self._action_type}, params={self._parameters}>"


class ActionType:
    """Enumeration of action types.
    
    This class provides the possible action types that the agent can
    take in the environment.
    
    Common actions include movement (forward, backward, turn), perception
    (look), interaction (interact, eat, drink), and rest (wait).
    """
    
    MoveForward = "MoveForward"
    MoveBackward = "MoveBackward"
    MoveLeft = "MoveLeft"
    MoveRight = "MoveRight"
    TurnLeft = "TurnLeft"
    TurnRight = "TurnRight"
    Look = "Look"
    LookUp = "LookUp"
    LookDown = "LookDown"
    Interact = "Interact"
    Eat = "Eat"
    Drink = "Drink"
    Rest = "Rest"
    Wait = "Wait"
    Custom = "Custom"


class MotorCommand:
    """Enumeration of motor commands.
    
    This class provides the low-level motor commands that the brain
    can generate to control the agent's body.
    
    Motor commands are typically simpler than actions and represent
    direct neural outputs that drive movement.
    """
    
    MoveForward = "MoveForward"
    MoveBackward = "MoveBackward"
    TurnLeft = "TurnLeft"
    TurnRight = "TurnRight"
    LookLeft = "LookLeft"
    LookRight = "LookRight"
    Interact = "Interact"
    Wait = "Wait"


class ActionResult:
    """Result of applying an action to the environment.
    
    This class represents the outcome of an action, including any
    rewards received, success status, and descriptive messages.
    
    Attributes:
        reward: Reward received (positive or negative).
        success: Whether action was successful.
        message: Descriptive message about action outcome.
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False, 
                 message: str = ""):
        self._reward = reward
        self._success = success
        self._message = message
    
    def getReward(self) -> float:
        """Get action reward.
        
        Returns:
            float: Action reward.
        """
        return self._reward
    
    def setReward(self, reward: float) -> bool:
        """Set action reward.
        
        Args:
            reward: New action reward.
            
        Returns:
            bool: True if reward set successfully.
        """
        self._reward = reward
        return True
    
    def getSuccess(self) -> bool:
        """Get action success status.
        
        Returns:
            bool: Whether action was successful.
        """
        return self._success
    
    def setSuccess(self, success: bool) -> bool:
        """Set action success status.
        
        Args:
            success: New success status.
            
        Returns:
            bool: True if success status set successfully.
        """
        self._success = success
        return True
    
    def getMessage(self) -> str:
        """Get action result message.
        
        Returns:
            str: Action result message.
        """
        return self._message
    
    def setMessage(self, message: str) -> bool:
        """Set action result message.
        
        Args:
            message: New action result message.
            
        Returns:
            bool: True if message set successfully.
        """
        self._message = message
        return True
    
    def __repr__(self) -> str:
        return f"<ActionResult: reward={self._reward:.2f}, success={self._success}, message='{self._message}'>"
