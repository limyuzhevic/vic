"""Motor system and action implementations."""

from typing import List
from .brain.neuron import NeuronId

class Action:
    """Action representation for motor output."""
    
    def __init__(self, action_type: int = 13):  # Wait type
        self._type = action_type
        self._parameters: List[float] = []
    
    def getType(self) -> int:
        return self._type
    
    def setType(self, action_type: int):
        self._type = action_type
    
    def getParameters(self) -> List[float]:
        return self._parameters
    
    def setParameters(self, parameters: List[float]):
        self._parameters = parameters
    
    def getName(self) -> str:
        return f"Action_{self._type}"
    
    def clone(self):
        new_action = Action(self._type)
        new_action.setParameters(self._parameters.copy())
        return new_action

class MotorSystem:
    """Motor system for action production."""
    
    def __init__(self):
        self._actions: List[Action] = []
    
    def addAction(self, action: Action):
        self._actions.append(action)
    
    def getActions(self) -> List[Action]:
        return self._actions.copy()
    
    def clearActions(self):
        self._actions.clear()

__all__ = ['Action', 'MotorSystem']