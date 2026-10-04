"""
NLM Motor classes
"""

from typing import List, Optional, Any

class MotorError(Exception):
    """Raised when motor operations fail."""
    pass
class ActionType:
    """Action type enumeration."""
    
    MOVEFORWARD = "MoveForward"
    MOVEBACKWARD = "MoveBackward"
    MOVELEFT = "MoveLeft"
    MOVERIGHT = "MoveRight"
    TURNLEFT = "TurnLeft"
    TURNRIGHT = "TurnRight"
    LOOK = "Look"
    LOOKUP = "LookUp"
    LOOKDOWN = "LookDown"
    INTERACT = "Interact"
    EAT = "Eat"
    DRINK = "Drink"
    REST = "Rest"
    WAIT = "Wait"
    CUSTOM = "Custom"
class Action:
    """Action representation for motor output."""
    
    def __init__(self, action_type: str = ActionType.WAIT,
                 parameters: Optional[List[float]] = None):
        """Create an action.
        
        Args:
            action_type: Type of action
            parameters: Action parameters (optional)
        """
        self.type = action_type
        self.parameters = parameters or []
    
    def get_type(self) -> str:
        """Get action type.
        
        Returns:
            Action type
        """
        return self.type
    
    def set_type(self, action_type: str) -> None:
        """Set action type.
        
        Args:
            action_type: Action type
        """
        self.type = action_type
    
    def get_parameters(self) -> List[float]:
        """Get action parameters.
        
        Returns:
            Action parameters
        """
        return self.parameters
    
    def set_parameters(self, parameters: List[float]) -> None:
        """Set action parameters.
        
        Args:
            parameters: Action parameters
        """
        self.parameters = parameters
    
    def get_name(self) -> str:
        """Get action name.
        
        Returns:
            Action name
        """
        return self.type.replace("_", " ").title()
    
    def clone(self) -> 'Action':
        """Create a copy of this action.
        
        Returns:
            Cloned action
        """
        return Action(self.type, self.parameters.copy())
    
    def __repr__(self) -> str:
        return f"Action(type={self.type}, parameters={self.parameters})"
class MotorCommand:
    """Low-level motor command enumeration."""
    
    MOVEFORWARD = "MoveForward"
    MOVEBACKWARD = "MoveBackward"
    TURNLEFT = "TurnLeft"
    TURNRIGHT = "TurnRight"
    LOOKLEFT = "LookLeft"
    LOOKRIGHT = "LookRight"
    INTERACT = "Interact"
    WAIT = "Wait"