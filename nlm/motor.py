"""
Motor system classes and actions
"""

from typing import List, Optional, Dict, Any
from enum import Enum

from .types import ActionType

class MotorCommand(Enum):
    """Low-level motor command enumeration"""
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class Action:
    """Action representation for motor output"""
    
    def __init__(self, action_type: Optional[ActionType] = None, parameters: Optional[List[float]] = None):
        self.action_type = action_type or ActionType.Wait
        self.parameters = parameters or []
        self.timestamp = None
    
    def getType(self) -> ActionType:
        """Get action type"""
        return self.action_type
    
    def setType(self, action_type: ActionType):
        """Set action type"""
        self.action_type = action_type
    
    def getParameters(self) -> List[float]:
        """Get action parameters"""
        return self.parameters
    
    def setParameters(self, params: List[float]):
        """Set action parameters"""
        self.parameters = params
    
    def getName(self) -> str:
        """Get action name"""
        return self.action_type.name.replace('_', ' ').title()
    
    def clone(self) -> 'Action':
        """Clone action"""
        new_action = Action(self.action_type, self.parameters.copy())
        new_action.timestamp = self.timestamp
        return new_action
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            "type": self.action_type,
            "type_name": self.getName(),
            "parameters": self.parameters,
            "timestamp": self.timestamp,
        }
    
    def __repr__(self):
        return f"<Action: {self.getName()} {self.parameters}>"
