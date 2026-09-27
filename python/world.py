"""
NLM SimpleWorld implementation.

This module provides the SimpleWorld class that implements a simple 2D world
for NLM simulation. The world manages the agent, objects, and simulation
environment, providing sensory percepts and handling motor commands.
"""

import time
import math
from typing import List, Any, Optional, Union

from .brain import Brain, EnhancedBrain
from .sensory import SensoryPercept

# Import C++ bindings
from .bindings import (
    SimpleWorld as _SimpleWorld,
    WorldObject as _WorldObject,
    ActionResult as _ActionResult,
    AgentBody as _AgentBody,
    ActionType,
    WorldObjectType,
)
class EnhancedSimpleWorld:
    """Enhanced SimpleWorld class with better error handling.
    
    This class provides a Pythonic interface to the NLM simple world,
    making it easier to work with the world simulation through properties
    and methods that follow Python conventions.
    
    The enhanced simple world provides:
    - Better error handling with custom exceptions
    - Convenience methods for common operations
    - World object management
    - Agent position and state tracking
    - Simulation statistics and monitoring
    
    Example:
        >>> world = EnhancedSimpleWorld()
        >>> world.configure(width=20, height=20, vision_width=8, vision_height=8)
        >>> world.reset()
        >>> world.setAgentStart(10.0, 10.0)
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     action = agent.processSensoryInput(percept)
        ...     world.applyMotorCommand(action, world.simulation_time)
        >>> 
        >>> # Access world statistics
        >>> print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
        >>> print(f"Energy: {world.getAgentBody().energy}")
    """
    
    def __init__(self):
        """Initialize a new simple world.
        
        Creates a default world instance with standard parameters.
        """
        try:
            self._world = _SimpleWorld()
            self._initialized = False
            
        except Exception as e:
            raise SimulationError(f"Failed to create simple world: {e}")
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """Configure world dimensions and vision.
        
        Args:
            width: World width in units.
            height: World height in units.
            vision_width: Width of agent's vision (pixels).
            vision_height: Height of agent's vision (pixels).
            
        Raises:
            SimulationError: If world configuration fails.
        """
        try:
            self._world.configure(width, height, vision_width, vision_height)
            self._initialized = True
        except Exception as e:
            raise SimulationError(f"Failed to configure world: {e}")
    
    def reset(self) -> None:
        """Reset world to initial state.
        
        Raises:
            SimulationError: If world reset fails.
        """
        try:
            self._world.reset()
            self._initialized = True
        except Exception as e:
            raise SimulationError(f"Failed to reset world: {e}")
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position.
        
        Args:
            x: X coordinate for agent start position.
            y: Y coordinate for agent start position.
            
        Raises:
            SimulationError: If agent start position setting fails.
        """
        try:
            self._world.setAgentStart(x, y)
        except Exception as e:
            raise SimulationError(f"Failed to set agent start position: {e}")
    
    def update(self, timestep: float) -> None:
        """Update world simulation.
        
        Args:
            timestep: Time step for simulation update.
            
        Raises:
            SimulationError: If world update fails.
        """
        try:
            self._world.update(timestep)
        except Exception as e:
            raise SimulationError(f"Failed to update world: {e}")
    
    def applyMotorCommand(self, action: Any, current_time: float) -> Any:
        """Apply motor command to world.
        
        Args:
            action: Motor command to apply.
            current_time: Current simulation time.
            
        Returns:
            Action result from world application.
            
        Raises:
            SimulationError: If motor command application fails.
        """
        try:
            return self._world.applyMotorCommand(action, current_time)
        except Exception as e:
            raise SimulationError(f"Failed to apply motor command: {e}")
    
    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept from world.
        
        Returns:
            Sensory percept containing all sensory inputs.
            
        Raises:
            SimulationError: If percept retrieval fails.
        """
        try:
            percept_obj = self._world.getSensoryPercept()
            return SensoryPercept(percept_obj)
        except Exception as e:
            raise SimulationError(f"Failed to get sensory percept: {e}")
    
    def getAgentBody(self) -> Any:
        """Get agent body state.
        
        Returns:
            Agent body state object.
            
        Raises:
            SimulationError: If agent body retrieval fails.
        """
        try:
            return self._world.getAgentBody()
        except Exception as e:
            raise SimulationError(f"Failed to get agent body: {e}")
    
    def addObject(self, obj: Any) -> None:
        """Add object to world.
        
        Args:
            obj: World object to add.
            
        Raises:
            SimulationError: If object addition fails.
        """
        try:
            self._world.addObject(obj)
        except Exception as e:
            raise SimulationError(f"Failed to add object to world: {e}")
    
    def removeObject(self, x: float, y: float) -> None:
        """Remove object from world at position.
        
        Args:
            x: X coordinate of object to remove.
            y: Y coordinate of object to remove.
            
        Raises:
            SimulationError: If object removal fails.
        """
        try:
            self._world.removeObject(x, y)
        except Exception as e:
            raise SimulationError(f"Failed to remove object from world: {e}")
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid in world.
        
        Args:
            x: X coordinate to check.
            y: Y coordinate to check.
            
        Returns:
            True if position is valid, False otherwise.
            
        Raises:
            SimulationError: If position validation fails.
        """
        try:
            return self._world.isValidPosition(x, y)
        except Exception as e:
            raise SimulationError(f"Failed to validate position: {e}")
    
    def getWidth(self) -> int:
        """Get world width.
        
        Returns:
            World width in units.
            
        Raises:
            SimulationError: If width retrieval fails.
        """
        try:
            return self._world.getWidth()
        except Exception as e:
            raise SimulationError(f"Failed to get world width: {e}")
    
    def getHeight(self) -> int:
        """Get world height.
        
        Returns:
            World height in units.
            
        Raises:
            SimulationError: If height retrieval fails.
        """
        try:
            return self._world.getHeight()
        except Exception as e:
            raise SimulationError(f"Failed to get world height: {e}")
    
    def getMaxEnergy(self) -> float:
        """Get maximum energy capacity.
        
        Returns:
            Maximum energy value.
            
        Raises:
            SimulationError: If max energy retrieval fails.
        """
        try:
            return self._world.getMaxEnergy()
        except Exception as e:
            raise SimulationError(f"Failed to get max energy: {e}")
    
    def setMaxEnergy(self, energy: float) -> None:
        """Set maximum energy capacity.
        
        Args:
            energy: Maximum energy value.
            
        Raises:
            SimulationError: If max energy setting fails.
        """
        try:
            self._world.setMaxEnergy(energy)
        except Exception as e:
            raise SimulationError(f"Failed to set max energy: {e}")
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            Energy decay rate.
            
        Raises:
            SimulationError: If energy decay rate retrieval fails.
        """
        try:
            return self._world.getEnergyDecayRate()
        except Exception as e:
            raise SimulationError(f"Failed to get energy decay rate: {e}")
    
    def setEnergyDecayRate(self, rate: float) -> None:
        """Set energy decay rate.
        
        Args:
            rate: Energy decay rate.
            
        Raises:
            SimulationError: If energy decay rate setting fails.
        """
        try:
            self._world.setEnergyDecayRate(rate)
        except Exception as e:
            raise SimulationError(f"Failed to set energy decay rate: {e}")
    
    def getSimulationTime(self) -> float:
        """Get current simulation time.
        
        Returns:
            Current simulation time.
            
        Raises:
            SimulationError: If simulation time retrieval fails.
        """
        try:
            return self._world.getSimulationTime()
        except Exception as e:
            raise SimulationError(f"Failed to get simulation time: {e}")
    
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed for world.
        
        Args:
            seed: Random seed value.
            
        Raises:
            SimulationError: If random seed setting fails.
        """
        try:
            self._world.setRandomSeed(seed)
        except Exception as e:
            raise SimulationError(f"Failed to set random seed: {e}")
    
    def getRandomSeed(self) -> int:
        """Get random seed for world.
        
        Returns:
            Current random seed value.
            
        Raises:
            SimulationError: If random seed retrieval fails.
        """
        try:
            return self._world.getRandomSeed()
        except Exception as e:
            raise SimulationError(f"Failed to get random seed: {e}")
    
    # Pythonic properties for easier access
    
    @property
    def width(self) -> int:
        """World width."""
        return self.getWidth()
    
    @property
    def height(self) -> int:
        """World height."""
        return self.getHeight()
    
    @property
    def max_energy(self) -> float:
        """Maximum energy capacity."""
        return self.getMaxEnergy()
    
    @property
    def energy_decay_rate(self) -> float:
        """Energy decay rate."""
        return self.getEnergyDecayRate()
    
    @property
    def simulation_time(self) -> float:
        """Current simulation time."""
        return self.getSimulationTime()
    
    @property
    def random_seed(self) -> int:
        """Current random seed."""
        return self.getRandomSeed()
    
    @property
    def initialized(self) -> bool:
        """Whether the world has been configured."""
        return self._initialized
    
    def __repr__(self) -> str:
        return f"EnhancedSimpleWorld(width={self.width}, height={self.height}, initialized={self.initialized})"
class SimpleWorld(EnhancedSimpleWorld):
    """SimpleWorld class for NLM simulation.
    
    This class implements a simple 2D world for NLM simulation. The world
    manages the agent, objects, and simulation environment, providing
    sensory percepts and handling motor commands.
    
    The class provides a complete world interface for the NLM framework,
    including working memory, episodic memory, prediction systems, cognition,
    neuromodulation, and development systems.
    
    Example:
        >>> world = SimpleWorld()
        >>> world.configure(width=20, height=20, vision_width=8, vision_height=8)
        >>> world.reset()
        >>> world.setAgentStart(10.0, 10.0)
        >>> 
        >>> # Access world properties
        >>> print(f"World size: {world.getWidth()}x{world.getHeight()}")
        >>> print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    """
    
    def __init__(self):
        """Initialize a new simple world."""
        super().__init__()
    
    # The enhanced world provides all the functionality through inheritance
    # This class is kept for backward compatibility and clarity


# Type aliases for better documentation
WorldWidth = int
WorldHeight = int
VisionSize = int
Timestep = float

__all__ = ["SimpleWorld", "EnhancedSimpleWorld", "WorldWidth", "WorldHeight", "VisionSize", "Timestep"]
