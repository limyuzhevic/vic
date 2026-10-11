# Motor module for NLM
from typing import Optional, List
import time
from enum import Enum

class MotorCommand(Enum):
    """Low-level motor command enumeration."""
    
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

class Action:
    """
    Action representation for motor output.
    
    Represents an action that the agent can take in the environment,
    such as movement, looking, interacting, etc.
    """
    
    def __init__(self, action_type: int = 0, parameters: Optional[List[float]] = None):
        """
        Initialize action.
        
        Args:
            action_type: Action type (integer enum)
            parameters: Action parameters (e.g., movement velocity, duration)
        """
        self._type = action_type
        self._parameters = parameters or []
        self._name = self._get_action_name(action_type)
        self._timestamp = time.time()
    
    def getType(self) -> int:
        """Get action type."""
        return self._type
    
    def setType(self, action_type: int) -> None:
        """
        Set action type.
        
        Args:
            action_type: Action type
        """
        self._type = action_type
        self._name = self._get_action_name(action_type)
    
    def getParameters(self) -> List[float]:
        """Get action parameters."""
        return self._parameters
    
    def setParameters(self, parameters: List[float]) -> None:
        """
        Set action parameters.
        
        Args:
            parameters: Action parameters
        """
        self._parameters = parameters
    
    def getName(self) -> str:
        """Get action name."""
        return self._name
    
    def setName(self, name: str) -> None:
        """
        Set action name.
        
        Args:
            name: Action name
        """
        self._name = name
    
    def clone(self):
        """Clone action."""
        new_action = Action(self._type, self._parameters.copy())
        new_action.setName(self._name)
        new_action.setTimestamp(self._timestamp)
        return new_action
    
    def getTimestamp(self) -> float:
        """Get action timestamp."""
        return self._timestamp
    
    def setTimestamp(self, timestamp: float) -> None:
        """Set action timestamp."""
        self._timestamp = timestamp
    
    def __repr__(self):
        return f"Action(type={self._type}, name='{self._name}', params={self._parameters})"
    
    # Helper method to get action names
    @staticmethod
    def _get_action_name(action_type: int) -> str:
        """Get action name from type."""
        action_names = {
            0: "MoveForward",
            1: "MoveBackward",
            2: "MoveLeft",
            3: "MoveRight",
            4: "TurnLeft",
            5: "TurnRight",
            6: "Look",
            7: "LookUp",
            8: "LookDown",
            9: "Interact",
            10: "Eat",
            11: "Drink",
            12: "Rest",
            13: "Wait",
            14: "Custom"
        }
        return action_names.get(action_type, f"Action_{action_type}")

# Factory functions for backward compatibility
def createAction() -> Action:
    """Create an empty action."""
    return Action()

def createActionFromType(action_type: int) -> Action:
    """Create action from type.
    
    Args:
        action_type: Action type
        
    Returns:
        Action: New action object
    """
    return Action(action_type)

def createActionWithParameters(action_type: int, parameters: List[float]) -> Action:
    """Create action with parameters.
    
    Args:
        action_type: Action type
        parameters: Action parameters
        
    Returns:
        Action: New action object
    """
    return Action(action_type, parameters)
