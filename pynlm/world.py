"""
Pythonic world simulation class for NLM.

This module provides enhanced Pythonic APIs for world simulation,
featuring simplified configuration, comprehensive state management,
and improved object interaction.
"""

import sys
from typing import List, Optional, Tuple

# Add parent directory to path for imports
if len(sys.path) == 1:
    sys.path.append("..")

from nlm import SimpleWorld, WorldObject, WorldObjectType, 
    Action, ActionResult, AgentBody, SensoryPercept

class WorldError(Exception):
    """Exception raised for world simulation errors."""
    pass

class WorldWrapper:
    """
    Enhanced Pythonic wrapper for NLM simple world simulation.
    
    Provides simplified configuration, comprehensive state management,
    and improved object interaction for neural environment simulation.
    """
    
    def __init__(self, world: Optional[SimpleWorld] = None):
        """
        Initialize world wrapper.
        
        Args:
            world: Existing NLM SimpleWorld object to wrap
        """
        if world is None:
            self.world = SimpleWorld()
        else:
            self.world = world
    
    def configure(self, width: float, height: float, 
                  vision_width: int, vision_height: int) -> None:
        """
        Configure world dimensions and vision parameters.
        
        Args:
            width: World width in meters
            height: World height in meters
            vision_width: Vision sensor width
            vision_height: Vision sensor height
        """
        self.world.configure(width, height, vision_width, vision_height)
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self.world.reset()
    
    def set_agent_start(self, x: float, y: float) -> None:
        """
        Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self.world.setAgentStart(x, y)
    
    def update(self, timestep: float) -> None:
        """
        Update world state.
        
        Args:
            timestep: Time step in seconds
        """
        self.world.update(timestep)
    
    def apply_motor_command(self, action: Action, 
                           current_time: float) -> ActionResult:
        """
        Apply motor command to agent.
        
        Args:
            action: Motor action to apply
            current_time: Current simulation time
        
        Returns:
            Action result
        
        Raises:
            WorldError: If action application fails
        """
        result = self.world.applyMotorCommand(action, current_time)
        if not result:
            raise WorldError("Action application failed")
        return result
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get current sensory percept for agent."""
        return self.world.getSensoryPercept()
    
    def get_vision(self) -> List[List[float]]:
        """Get vision sensory data as array."""
        percept = self.getSensoryPercept()
        vision = percept.getVision()
        
        # Convert to Python list
        return vision
    
    def get_agent_body(self) -> AgentBody:
        """Get current agent body state."""
        return self.world.getAgentBody()
    
    def get_agent_position(self) -> Tuple[float, float]:
        """Get agent position."""
        body = self.get_agent_body()
        return (body.x, body.y)
    
    def get_agent_orientation(self) -> float:
        """Get agent orientation in radians."""
        body = self.get_agent_body()
        return body.orientation
    
    def get_agent_velocity(self) -> Tuple[float, float]:
        """Get agent velocity."""
        body = self.get_agent_body()
        return (body.velocityX, body.velocityY)
    
    def get_agent_energy(self) -> float:
        """Get current agent energy."""
        body = self.get_agent_body()
        return body.energy
    
    def get_agent_health(self) -> float:
        """Get current agent health."""
        body = self.get_agent_body()
        return body.health
    
    def get_agent_age(self) -> float:
        """Get current agent age."""
        body = self.get_agent_body()
        return body.age
    
    def get_agent_is_moving(self) -> bool:
        """Check if agent is currently moving."""
        body = self.get_agent_body()
        return body.isMoving
    
    def get_agent_is_turning(self) -> bool:
        """Check if agent is currently turning."""
        body = self.get_agent_body()
        return body.isTurning
    
    # Object management
    def add_object(self, obj: WorldObject) -> None:
        """
        Add object to world.
        
        Args:
            obj: World object to add
        """
        self.world.addObject(obj)
    
    def create_object(self, x: float, y: float, obj_type: WorldObjectType,
                     value: float = 0.0, radius: float = 0.5) -> WorldObject:
        """
        Create and add a new world object.
        
        Args:
            x: X coordinate
            y: Y coordinate
            obj_type: Object type
            value: Reward value or damage (for resources/hazards)
            radius: Object radius
        
        Returns:
            Created WorldObject
        """
        obj = WorldObject(x, y, obj_type, value, radius)
        self.add_object(obj)
        return obj
    
    def remove_object(self, x: float, y: float) -> None:
        """
        Remove object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self.world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if position is valid (not wall).
        
        Args:
            x: X coordinate
            y: Y coordinate
        
        Returns:
            True if position is valid, False otherwise
        """
        return self.world.isValidPosition(x, y)
    
    def get_object_at(self, x: float, y: float) -> Optional[WorldObject]:
        """
        Get object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        
        Returns:
            WorldObject at position or None if no object
        """
        obj = self.world.getObjectAt(x, y)
        return obj
    
    # Dimensions and settings
    def get_width(self) -> float:
        """Get world width."""
        return self.world.getWidth()
    
    def get_height(self) -> float:
        """Get world height."""
        return self.world.getHeight()
    
    def get_vision_width(self) -> int:
        """Get vision width."""
        return self.world.getVisionWidth()
    
    def get_vision_height(self) -> int:
        """Get vision height."""
        return self.world.getVisionHeight()
    
    # Energy settings
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return self.world.getMaxEnergy()
    
    def set_max_energy(self, energy: float) -> None:
        """Set maximum energy."""
        self.world.setMaxEnergy(energy)
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self.world.getEnergyDecayRate()
    
    def set_energy_decay_rate(self, rate: float) -> None:
        """Set energy decay rate."""
 dicion
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self.world.getSimulationTime()
    
    # Random seed
    def set_random_seed(self, seed: int) -> None:
        """Set random seed for reproducibility."""
        self.world.setRandomSeed(seed)
    
    def get_random_seed(self) -> int:
        """Get random seed."""
        return self.world.getRandomSeed()
    
    # Clone method
    def clone(self) -> 'WorldWrapper':
        """Create a copy of the world with same configuration."""
        new_world = SimpleWorld()
        new_world.configure(self.get_width(), self.get_height(),
                           self.get_vision_width(), self.get_vision_height())
        new_world.setAgentStart(self.get_agent_position()[0], 
                               self.get_agent_position()[1])
        new_world.setMaxEnergy(self.get_max_energy())
        new_world.setEnergyDecayRate(self.get_energy_decay_rate())
        new_world.setRandomSeed(self.get_random_seed())
        
        return WorldWrapper(new_world)
    
    def __repr__(self) -> str:
        """String representation."""
        width = self.get_width()
        height = self.get_height()
        return f"<WorldWrapper: {width}x{height} meters>"
    
    def __str__(self) -> str:
        """String representation."""
        width = self.get_width()
        height = self.get_height()
        x, y = self.get_agent_position()
        return f"World({width}x{height}), Agent({x:.1f}, {y:.1f})"
    
    def get_status_summary(self) -> str:
        """Get comprehensive status summary."""
        width = self.get_width()
        height = self.get_height()
        x, y = self.get_agent_position()
        energy = self.get_agent_energy()
        health = self.get_agent_health()
        sim_time = self.get_simulation_time()
        
        return (f"World: {width}x{height} meters, "
                f"Agent at ({x:.1f}, {y:.1f}), "
                f"Energy: {energy:.1f}/{self.get_max_energy()}, "
                f"Health: {health:.1f}, "
                f"Time: {sim_time:.2f}s")