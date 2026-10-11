"""
World wrapper for NLM simulation world

This module provides a Pythonic interface to the C++ SimpleWorld class with
convenience methods for simulation management and sensory input handling.
"""

from typing import Optional, List, Any
from enum import Enum

# Import the C++ bindings
from .pynlm import (
    SimpleWorld as CppSimpleWorld,
    createSimpleWorld as create_cpp_simple_world,
    Action,
    ActionType,
    MotorCommand,
    SensoryPercept,
    Vision,
    Audio,
    InternalSignals,
    AgentBody,
    WorldObject,
    WorldObjectType,
)

class SimpleWorld:
    """Simple 2D world for NLM simulation."""
    
    def __init__(self):
        """Create a new simple world."""
        self._cpp_world = create_cpp_simple_world()
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions and sensory capabilities.
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Vision width in cells
            vision_height: Vision height in cells
        """
        self._cpp_world.configure(width, height, vision_width, vision_height)
    
    def reset(self):
        """Reset the world to initial state."""
        self._cpp_world.reset()
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._cpp_world.setAgentStart(x, y)
    
    def update(self, timestep: float):
        """Update the world by timestep seconds.
        
        Args:
            timestep: Time step duration in seconds
        """
        self._cpp_world.update(timestep)
    
    def apply_motor_command(self, command: Action, current_time: float):
        """Apply motor command to the world.
        
        Args:
            command: Motor command to apply
            current_time: Current simulation time
        """
        self._cpp_world.applyMotorCommand(command, current_time)
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get the current sensory percept.
        
        Returns:
            Sensory percept object
        """
        return self._cpp_world.getSensoryPercept()
    
    def get_agent_body(self) -> AgentBody:
        """Get the agent body state.
        
        Returns:
            Agent body state
        """
        return self._cpp_world.getAgentBody()
    
    def add_object(self, obj: WorldObject):
        """Add a world object.
        
        Args:
            obj: World object to add
        """
        self._cpp_world.addObject(obj)
    
    def remove_object(self, x: float, y: float):
        """Remove world object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._cpp_world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid
        """
        return self._cpp_world.isValidPosition(x, y)
    
    def get_width(self) -> int:
        """Get world width.
        
        Returns:
            World width in cells
        """
        return self._cpp_world.getWidth()
    
    def get_height(self) -> int:
        """Get world height.
        
        Returns:
            World height in cells
        """
        return self._cpp_world.getHeight()
    
    def get_max_energy(self) -> float:
        """Get maximum energy capacity.
        
        Returns:
            Maximum energy
        """
        return self._cpp_world.getMaxEnergy()
    
    def set_max_energy(self, energy: float):
        """Set maximum energy capacity.
        
        Args:
            energy: Maximum energy
        """
        self._cpp_world.setMaxEnergy(energy)
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            Energy decay rate
        """
        return self._cpp_world.getEnergyDecayRate()
    
    def set_energy_decay_rate(self, rate: float):
        """Set energy decay rate.
        
        Args:
            rate: Energy decay rate
        """
        self._cpp_world.setEnergyDecayRate(rate)
    
    def get_simulation_time(self) -> float:
        """Get current simulation time.
        
        Returns:
            Current simulation time
        """
        return self._cpp_world.getSimulationTime()
    
    def set_random_seed(self, seed: int):
        """Set random seed.
        
        Args:
            seed: Random seed
        """
        self._cpp_world.setRandomSeed(seed)
    
    def get_random_seed(self) -> int:
        """Get random seed.
        
        Returns:
            Random seed
        """
        return self._cpp_world.getRandomSeed()
    
    def __repr__(self) -> str:
        """Get string representation of world.
        
        Returns:
            String representation
        """
        return f"SimpleWorld({self.get_width()}x{self.get_height()} cells)"

# Convenience functions

def create_world() -> SimpleWorld:
    """Create a new simple world.
    
    Returns:
        SimpleWorld instance
    """
    return SimpleWorld()

# Export types and classes
__all__ = [
    'SimpleWorld',
    'WorldObject',
    'WorldObjectType',
    'create_world',
    'Action',
    'ActionType',
    'MotorCommand',
    'SensoryPercept',
    'Vision',
    'Audio',
    'InternalSignals',
    'AgentBody',
]