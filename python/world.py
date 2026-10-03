# -*- coding: utf-8 -*-
"""
High-level World abstraction for NLM Python package.

This module provides a high-level interface for working with NLM worlds,
abstracting away some of the low-level details and providing more Pythonic APIs.
"""

from typing import Optional, List, Dict, Any

from .bindings import (
    SimpleWorld as _SimpleWorld,
    WorldObject as _WorldObject,
    WorldObjectType,
    AgentBody as _AgentBody,
    MotorCommand,
    SensoryPercept as _SensoryPercept,
)
class World:
    """
    High-level interface for NLM world simulation.
    
    This class wraps the low-level C++ SimpleWorld class and provides a more
    Pythonic interface for common operations.
    """
    
    def __init__(self):
        """Initialize a new world."""
        self._world = _SimpleWorld()
    
    def configure(self, width: int, height: int, 
                  vision_width: Optional[int] = None,
                  vision_height: Optional[int] = None,
                  max_energy: Optional[float] = None,
                  energy_decay_rate: Optional[float] = None):
        """
        Configure the world.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width for agent perception
            vision_height: Vision height for agent perception
            max_energy: Maximum energy for agent
            energy_decay_rate: Energy decay rate
        """
        self._world.configure(width, height)
        if vision_width is not None and vision_height is not None:
            # This would need to be added to the C++ API
            pass
        if max_energy is not None:
            self._world.setMaxEnergy(max_energy)
        if energy_decay_rate is not None:
            self._world.setEnergyDecayRate(energy_decay_rate)
        return self
    
    def reset(self):
        """Reset the world to its initial state."""
        self._world.reset()
        return self
    
    def update(self, timestep: float):
        """
        Update the world for one timestep.
        
        Args:
            timestep: Time step duration
        """
        self._world.update(timestep)
        return self
    
    def set_agent_start(self, x: float, y: float, 
                        orientation: Optional[float] = None):
        """
        Set the agent's starting position.
        
        Args:
            x: Starting X coordinate
            y: Starting Y coordinate
            orientation: Starting orientation (optional)
        """
        self._world.setAgentStart(x, y)
        # Note: orientation would need to be added to C++ API
        return self
    
    def apply_motor_command(self, action, timestep: float):
        """
        Apply a motor command to the agent in the world.
        
        Args:
            action: Motor command or action
            timestep: Current simulation time
        """
        self._world.applyMotorCommand(action, timestep)
        return self
    
    def get_sensory_percept(self) -> _SensoryPercept:
        """
        Get the current sensory percept from the agent.
        
        Returns:
            Sensory percept object
        """
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> _AgentBody:
        """
        Get the agent's body state.
        
        Returns:
            Agent body object
        """
        return self._world.getAgentBody()
    
    def add_object(self, obj: _WorldObject):
        """
        Add an object to the world.
        
        Args:
            obj: World object to add
        """
        self._world.addObject(obj)
        return self
    
    def remove_object(self, x: float, y: float):
        """
        Remove an object from the world.
        
        Args:
            x: X coordinate of object
            y: Y coordinate of object
        """
        self._world.removeObject(x, y)
        return self
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if a position is valid in the world.
        
        Args:
            x: X coordinate
            y: Y coordinate
        
        Returns:
            True if position is valid, False otherwise
        """
        return self._world.isValidPosition(x, y)
    
    def get_width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    def get_max_energy(self) -> float:
        """Get maximum energy for agent."""
        return self._world.getMaxEnergy()
    
    def set_max_energy(self, e: float):
        """Set maximum energy for agent."""
        self._world.setMaxEnergy(e)
        return self
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._world.getEnergyDecayRate()
    
    def set_energy_decay_rate(self, r: float):
        """Set energy decay rate."""
        self._world.setEnergyDecayRate(r)
        return self
    
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    def set_random_seed(self, seed: int):
        """
        Set random seed for reproducible simulations.
        
        Args:
            seed: Random seed
        """
        self._world.setRandomSeed(seed)
        return self
    
    def get_random_seed(self) -> int:
        """Get current random seed."""
        return self._world.getRandomSeed()
    
    def __repr__(self):
        return f"<World {self._world.getWidth()}x{self._world.getHeight()}>"


def createSimpleWorld() -> World:
    """
    Create a new simple world.
    
    Returns:
        New World instance
    """
    return World()


# For backward compatibility
World = createSimpleWorld
