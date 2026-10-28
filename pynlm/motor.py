# Motor system interfaces for NLM
from typing import List, Optional

class Action:
    """Action representation for motor output in NLM."""
    
    def __init__(self, action_type: 'ActionType' = None, parameters: List[float] = None):
        """Initialize action."""
        self._type = action_type if action_type is not None else ActionType.WAIT
        self._parameters = parameters if parameters is not None else []
    
    def getType(self) -> 'ActionType':
        """Get the action type."""
        return self._type
    
    def setType(self, action_type: 'ActionType') -> None:
        """Set the action type."""
        self._type = action_type
    
    def getParameters(self) -> List[float]:
        """Get action parameters."""
        return self._parameters.copy()
    
    def setParameters(self, parameters: List[float]) -> None:
        """Set action parameters."""
        self._parameters = parameters.copy()
    
    def getName(self) -> str:
        """Get human-readable action name."""
        return self._type.name.replace('_', ' ').title()
    
    def clone(self) -> 'Action':
        """Create a copy of this action."""
        return Action(self._type, self._parameters.copy())
    
    def __repr__(self) -> str:
        return f"<Action type={self._type.name} params={self._parameters}>"

    def __str__(self) -> str:
        return f"{self.getName()} {self._parameters}"

    def __eq__(self, other) -> bool:
        if isinstance(other, Action):
            return self._type == other._type and self._parameters == other._parameters
        return False

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)
