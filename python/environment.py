"""
NLM Python Bindings - Environment Module

This module provides Python bindings for NLM environment components like
world simulation, sensory input, and motor commands.
"""

import numpy as np
from typing import Optional, List, Tuple

# Core environment components
class WorldState:
    """Represents the state of the simulated world."""
    
    def __init__(self, width: int, height: int):
        self.width = width
        self.height = height
        self.agent_position = np.array([0, 0], dtype=np.float32)
        self.grid = np.zeros((height, width), dtype=np.float32)
        self.observations = []
        
    def update(self, action: 'Action') -> None:
        """Update world state based on action."""
        # Simple grid movement
        x, y = self.agent_position
        if action.get_type() == 'move_forward':
            if action.get_direction() == 'north':
                y = max(0, y - 1)
            elif action.get_direction() == 'south':
                y = min(self.height - 1, y + 1)
            elif action.get_direction() == 'east':
                x = min(self.width - 1, x + 1)
            elif action.get_direction() == 'west':
                x = max(0, x - 1)
                
        self.agent_position = np.array([x, y], dtype=np.float32)
        
    def get_observation(self) -> np.ndarray:
        """Get observation of current world state."""
        # Return a feature vector representing the world
        observation = np.concatenate([
            self.agent_position / max(self.width, self.height),
            np.array([np.mean(self.grid)], dtype=np.float32),
            np.array([self.width, self.height], dtype=np.float32),
        ])
        return observation


class SimpleWorld:
    """Simple grid-based world for NLM agent simulation."""
    
    def __init__(self, width: int = 16, height: int = 16):
        self.width = width
        self.height = height
        self.state = WorldState(width, height)
        self.reward_history = []
        
    def initialize(self, width: int, height: int) -> None:
        """Initialize the world with given dimensions."""
        self.width = width
        self.height = height
        self.state = WorldState(width, height)
        
    def observe(self, brain: Optional['Brain'] = None) -> np.ndarray:
        """Observe current world state."""
        return self.state.get_observation()
        
    def apply_action(self, brain: Optional['Brain'], action: 'Action') -> None:
        """Apply action to world and update state."""
        self.state.update(action)
        
        # Update grid based on agent activity
        x, y = self.state.agent_position.astype(int)
        if 0 <= x < self.width and 0 <= y < self.height:
            self.state.grid[y, x] = 1.0
            
        # Add some noise to make it more interesting
        self.state.grid += np.random.normal(0, 0.01, (self.height, self.width))
        self.state.grid = np.clip(self.state.grid, 0, 1)
        
    def compute_reward(self, brain: Optional['Brain']) -> float:
        """Compute reward based on current world state."""
        # Simple reward: getting closer to center
        center = np.array([self.width / 2, self.height / 2], dtype=np.float32)
        distance = np.linalg.norm(self.state.agent_position - center)
        max_distance = np.sqrt((self.width/2)**2 + (self.height/2)**2)
        
        # Reward is higher for being closer to center
        reward = 1.0 - (distance / max_distance)
        
        # Add some noise
        reward += np.random.normal(0, 0.05)
        
        self.reward_history.append(reward)
        return max(0.0, reward)  # Ensure non-negative reward


class SensoryInput:
    """Sensory input from the environment."""
    
    def __init__(self, data: Optional[np.ndarray] = None):
        if data is None:
            self._data = np.zeros(10, dtype=np.float32)
        else:
            self._data = np.array(data, dtype=np.float32)
            
    def get_data(self) -> np.ndarray:
        """Get sensory input data."""
        return self._data.copy()
        
    def get_vector(self) -> np.ndarray:
        """Get sensory input as a vector."""
        return self._data
        
    def get_magnitude(self) -> float:
        """Get magnitude of sensory input."""
        return np.linalg.norm(self._data)
        
    @classmethod
    def from_array(cls, array: np.ndarray) -> 'SensoryInput':
        """Create SensoryInput from numpy array."""
        return cls(array)
        
    @classmethod
    def from_features(cls, features: Dict[str, float]) -> 'SensoryInput':
        """Create SensoryInput from feature dictionary."""
        data = np.array(list(features.values()), dtype=np.float32)
        return cls(data)


class MotorCommand:
    """Motor command for agent actions."""
    
    def __init__(self, command_type: str, parameters: Optional[Dict[str, float]] = None):
        self._type = command_type
        self._parameters = parameters or {}
        
    def get_type(self) -> str:
        """Get command type."""
        return self._type
        
    def get_parameter(self, name: str, default: float = 0.0) -> float:
        """Get parameter value."""
        return self._parameters.get(name, default)
        
    def get_parameters(self) -> Dict[str, float]:
        """Get all parameters."""
        return self._parameters.copy()
        
    def get_action(self) -> 'Action':
        """Convert to Action object."""
        action_type = None
        direction = None
        
        if self._type == 'move':
            action_type = 'move_forward'
            direction = self._parameters.get('direction', 'north')
        elif self._type == 'turn':
            action_type = 'turn'
            direction = self._parameters.get('direction', 'left')
        elif self._type == 'wait':
            action_type = 'wait'
        
        return Action(action_type, direction)
        
    @classmethod
    def move_forward(cls, direction: str = 'north') -> 'MotorCommand':
        """Create a move forward command."""
        return cls('move', {'direction': direction})
        
    @classmethod
    def turn(cls, direction: str = 'left') -> 'MotorCommand':
        """Create a turn command."""
        return cls('turn', {'direction': direction})
        
    @classmethod
    def wait(cls) -> 'MotorCommand':
        """Create a wait command."""
        return cls('wait', {})


class Action:
    """Agent action result."""
    
    def __init__(self, action_type: str, direction: Optional[str] = None):
        self._type = action_type
        self._direction = direction
        
    def get_type(self) -> str:
        """Get action type."""
        return self._type
        
    def get_direction(self) -> Optional[str]:
        """Get action direction."""
        return self._direction
        
    def __str__(self) -> str:
        if self._direction:
            return f"Action({self._type}, {self._direction})"
        return f"Action({self._type})"
        
    def __repr__(self) -> str:
        return self.__str__()
