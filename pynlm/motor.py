"""
Enhanced Python interface for NLM motor system classes
"""

from typing import List, Dict, Any, Optional, Tuple
from enum import Enum
from .exceptions import NLMError

class ActionType(Enum):
    """Action type enumeration"""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
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

class MotorCommand(Enum):
    """Motor command enumeration"""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class Action:
    """Action representation with enhanced Python interface"""
    
    def __init__(self, action_type: ActionType = ActionType.WAIT, 
                 parameters: Optional[List[float]] = None):
        self.action_type = action_type
        self.parameters = parameters or []
        
    def get_type(self) -> ActionType:
        """Get action type"""
        return self.action_type
        
    def set_type(self, action_type: ActionType) -> None:
        """Set action type"""
        self.action_type = action_type
        
    def get_parameters(self) -> List[float]:
        """Get action parameters"""
        return self.parameters
        
    def set_parameters(self, parameters: List[float]) -> None:
        """Set action parameters"""
        self.parameters = parameters
        
    def get_name(self) -> str:
        """Get action name"""
        return str(self.action_type).split('.')[-1]
        
    def get_description(self) -> str:
        """Get action description"""
        descriptions = {
            ActionType.MOVE_FORWARD: "Move forward in the direction agent is facing",
            ActionType.MOVE_BACKWARD: "Move backward",
            ActionType.TURN_LEFT: "Turn left (typically 90 degrees)",
            ActionType.TURN_RIGHT: "Turn right (typically 90 degrees)",
            ActionType.LOOK: "Look in current direction",
            ActionType.LOOK_UP: "Look upward",
            ActionType.LOOK_DOWN: "Look downward",
            ActionType.INTERACT: "Interact with nearby object",
            ActionType.EAT: "Eat available food resource",
            ActionType.DRINK: "Drink water resource",
            ActionType.REST: "Rest and recover energy",
            ActionType.WAIT: "Do nothing this step",
            ActionType.CUSTOM: "Custom action (requires parameters)"
        }
        return descriptions.get(self.action_type, "Unknown action")
        
    def clone(self) -> 'Action':
        """Create a copy of this action"""
        return Action(self.action_type, self.parameters.copy())
        
    def __repr__(self):
        return f"<Action: {self.get_name()}>"
        
    def __eq__(self, other):
        if not isinstance(other, Action):
            return False
        return (self.action_type == other.action_type and 
                self.parameters == other.parameters)
        
    def __hash__(self):
        return hash((self.action_type, tuple(self.parameters)))
