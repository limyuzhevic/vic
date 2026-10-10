"""
Pythonic interface for NLM world simulation.

This module provides interfaces for the simulated environment in which the NLM
brain operates. The world handles agent positioning, object management,
sensory rendering, and world state management.
"""

from typing import Optional, List, Any, Union
from dataclasses import dataclass
from pathlib import Path

# Import the C++ extension module directly
import pynlm as _c_extension
from .agent import ActionResultData

if _c_extension is None:
    raise ImportError(
        "NLM C++ extension not found. Please build the extension first:\n"
        "mkdir build && cd build && cmake .. && make"
    )
class WorldError(Exception):
    """Raised when world operations fail."""
    pass
@dataclass
class WorldObject:
    """World object representation.
    
    Attributes:
        x: X position in world coordinates
        y: Y position in world coordinates
        type: Type of world object
        value: Additional value (e.g., resource amount)
        radius: Object radius for collision detection
        active: Whether object is currently active
    """
    x: float
    y: float
    type: Any
    value: float = 0.0
    radius: float = 0.5
    active: bool = True
class SimpleWorld:
    """Simple 2D world for NLM simulation.
    
    The SimpleWorld class provides a basic 2D simulation environment where
    agents can move, interact with objects, and receive sensory input.
    
    Examples:
        >>> from pynlm import SimpleWorld
        >>> world = SimpleWorld(64, 64, 32, 32)  # width, height, vision_w, vision_h
        >>> world.setAgentStart(32.0, 32.0)
        >>> 
        >>> # Run simulation loop
        >>> for step in range(100):
        ...     percept = world.getSensoryPercept()
        ...     action = brain.produceAction()
        ...     world.applyMotorCommand(action, step)
        ...     
        >>> # Get agent state
        >>> body = world.getAgentBody()
        >>> print(f"Position: ({body.x}, {body.y})")
    """
    
    def __init__(self) -> None:
        """Create an empty world."""
        self._world = _c_extension.SimpleWorld()
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None:
        """Configure world dimensions and agent sensing.
        
        Args:
            width: World width in cells/units.
            height: World height in cells/units.
            vision_width: Agent vision width in cells.
            vision_height: Agent vision height in cells.
            
        Examples:
            >>> world.configure(64, 64, 32, 32)
        """
        self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self) -> None:
        """Reset world to initial state.
        
        Examples:
            >>> world.reset()
        """
        self._world.reset()
    
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position.
        
        Args:
            x: X coordinate for agent start.
            y: Y coordinate for agent start.
            
        Examples:
            >>> world.setAgentStart(32.0, 32.0)
        """
        self._world.setAgentStart(x, y)
    
    def update(self, timestep: float) -> None:
        """Update world state.
        
        Args:
            timestep: Time step for simulation update.
            
        Examples:
            >>> world.update(0.1)
        """
        self._world.update(timestep)
    
    def apply_motor_command(self, action: Any, current_time: float) -> Optional['ActionResultData']:
        """Apply motor command to world.
        
        Args:
            action: Action to apply.
            current_time: Current simulation time.
            
        Returns:
            ActionResultData with reward and success status, or None if failed.
            
        Examples:
            >>> result = world.applyMotorCommand(action, current_time)
        """
        try:
            result = self._world.applyMotorCommand(action, current_time)
            if result is not None:
                return ActionResultData(
                    reward=result.reward,
                    success=result.success,
                    message=result.message
                )
            return None
        except Exception as e:
            raise WorldError(f"Failed to apply motor command: {e}")
    
    def get_sensory_percept(self) -> Any:
        """Get sensory percept from world.
        
        Returns:
            SensoryPercept object containing all sensory modalities.
            
        Examples:
            >>> percept = world.getSensoryPercept()
        """
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> Any:
        """Get agent body state.
        
        Returns:
            AgentBody object representing current agent state.
            
        Examples:
            >>> body = world.getAgentBody()
        """
        return self._world.getAgentBody()
    
    def add_object(self, obj: Union[WorldObject, Any]) -> None:
        """Add object to world.
        
        Args:
            obj: World object to add.
            
        Examples:
            >>> world_object = WorldObject(x=10, y=10, type=WorldObjectType.Resource)
            >>> world.add_object(world_object)
        """
        self._world.addObject(obj)
    
    def remove_object(self, x: float, y: float) -> bool:
        """Remove object from world.
        
        Args:
            x: X coordinate of object to remove.
            y: Y coordinate of object to remove.
            
        Returns:
            True if object was removed, False otherwise.
            
        Examples:
            >>> removed = world.removeObject(10.0, 10.0)
        """
        return self._world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid (not colliding with obstacles).
        
        Args:
            x: X coordinate to check.
            y: Y coordinate to check.
            
        Returns:
            True if position is valid, False otherwise.
            
        Examples:
            >>> valid = world.isValidPosition(32.0, 32.0)
        """
        return self._world.isValidPosition(x, y)
    
    def get_width(self) -> int:
        """Get world width.
        
        Returns:
            World width in cells/units.
            
        Examples:
            >>> width = world.getWidth()
        """
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height.
        
        Returns:
            World height in cells/units.
            
        Examples:
            >>> height = world.getHeight()
        """
        return self._world.getHeight()
    
    def get_max_energy(self) -> float:
        """Get maximum energy level.
        
        Returns:
            Maximum energy value.
            
        Examples:
            >>> max_energy = world.getMaxEnergy()
        """
        return self._world.getMaxEnergy()
    
    def set_max_energy(self, energy: float) -> None:
        """Set maximum energy level.
        
        Args:
            energy: Maximum energy value.
            
        Examples:
            >>> world.setMaxEnergy(100.0)
        """
        self._world.setMaxEnergy(energy)
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            Energy decay rate per timestep.
            
        Examples:
            >>> decay_rate = world.getEnergyDecayRate()
        """
        return self._world.getEnergyDecayRate()
    
    def set_energy_decay_rate(self, rate: float) -> None:
        """Set energy decay rate.
        
        Args:
            rate: Energy decay rate per timestep.
            
        Examples:
            >>> world.setEnergyDecayRate(0.01)
        """
        self._world.setEnergyDecayRate(rate)
    
    def get_simulation_time(self) -> float:
        """Get current simulation time.
        
        Returns:
            Current simulation time.
            
        Examples:
            >>> time = world.getSimulationTime()
        """
        return self._world.getSimulationTime()
    
    def set_random_seed(self, seed: int) -> None:
        """Set random seed for reproducible simulations.
        
        Args:
            seed: Random seed value.
            
        Examples:
            >>> world.setRandomSeed(12345)
        """
        self._world.setRandomSeed(seed)
    
    def get_random_seed(self) -> int:
        """Get current random seed.
        
        Returns:
            Current random seed value.
            
        Examples:
            >>> seed = world.getRandomSeed()
        """
        return self._world.getRandomSeed()
    
    def __repr__(self) -> str:
        """String representation of world."""
        return f"<SimpleWorld: {self.get_width()}x{self.get_height()}>"
    
    def __str__(self) -> str:
        """String representation of world."""
        return f"SimpleWorld(width={self.get_width()}, height={self.get_height()})"
# Convenience function
def create_simple_world() -> SimpleWorld:
    """Create a new simple world.
    
    Returns:
        New SimpleWorld instance.
        
    Examples:
        >>> world = create_simple_world()
        >>> world.configure(64, 64, 32, 32)
    """
    return SimpleWorld()