# NLM Python World Implementation

from typing import Dict, Any, Optional
from enum import Enum
from .neural_types import ActionType

class World:
    """World environment for the agent."""
    
    def __init__(self):
        self._world = {
            'width': 10,
            'height': 10,
            'vision_width': 8,
            'vision_height': 8,
            'agent_x': 5.0,
            'agent_y': 5.0,
            'time': 0.0,
            'vision': [],
            'internal': [],
            'sound': []
        }
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """Configure world dimensions."""
        self._world.update({
            'width': width,
            'height': height,
            'vision_width': vision_width,
            'vision_height': vision_height
        })
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self._world['time'] = 0.0
        self._world['agent_x'] = self._world['width'] / 2.0
        self._world['agent_y'] = self._world['height'] / 2.0
        self._world['vision'] = [[0.0 for _ in range(self._world['vision_height'])] 
                                 for _ in range(self._world['vision_width'])]
        self._world['internal'] = [0.0]
        self._world['sound'] = [[0.0 for _ in range(self._world['vision_height'])] 
                                for _ in range(self._world['vision_width'])]
    
    def update(self, timestep: float) -> None:
        """Update world by timestep seconds."""
        self._world['time'] += timestep
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position."""
        self._world['agent_x'] = x
        self._world['agent_y'] = y
    
    def get_sensory_percept(self) -> Dict[str, Any]:
        """Get sensory perception of current world state."""
        return self._world.copy()
    
    def apply_motor_command(self, action: ActionType, time: float) -> bool:
        """Apply motor command to world."""
        # Simple movement implementation
        dx, dy = 0.0, 0.0
        
        if action == ActionType.MOVE_FORWARD:
            dx, dy = 1.0, 0.0
        elif action == ActionType.TURN_LEFT:
            # Turn left (just change angle for now)
            pass
        elif action == ActionType.TURN_RIGHT:
            # Turn right (just change angle for now)
            pass
        elif action == ActionType.INTERACT:
            # Interaction with world
            pass
        elif action == ActionType.EAT:
            # Eat action
            pass
        elif action == ActionType.SLEEP:
            # Sleep action
            pass
        elif action == ActionType.EXPLORE:
            # Explore action
            pass
        elif action == ActionType.STAY:
            # Stay action
            pass
        
        # Update agent position (simple implementation)
        self._world['agent_x'] += dx
        self._world['agent_y'] += dy
        
        # Keep agent within bounds
        self._world['agent_x'] = max(0, min(self._world['width'], self._world['agent_x']))
        self._world['agent_y'] = max(0, min(self._world['height'], self._world['agent_y']))
        
        return True
    
    @property
    def width(self) -> int:
        return self._world['width']
    
    @property
    def height(self) -> int:
        return self._world['height']
    
    @property
    def simulation_time(self) -> float:
        return self._world['time']

class SensoryPercept:
    """Sensory perception from the world."""
    
    def __init__(self, percept_data: Optional[Dict[str, Any]] = None):
        if percept_data is None:
            percept_data = {
                'vision': [],
                'internal': [],
                'sound': []
            }
        self._percept = percept_data
    
    @property
    def vision(self) -> list:
        """Get visual input."""
        return self._percept.get('vision', [])
    
    @property
    def internal(self) -> list:
        """Get internal signals."""
        return self._percept.get('internal', [])
    
    @property
    def sound(self) -> list:
        """Get audio input."""
        return self._percept.get('sound', [])

# Module-level functions for compatibility
def createSimpleWorld() -> World:
    """Create a simple world."""
    return World()