"""
Enhanced world class for NLM with Pythonic interface.

This module provides an enhanced Python wrapper for the NLM world classes,
adding convenience methods, validation, and helper functions.
"""

import time
from typing import Dict, Any, Optional, Tuple, List
from .exceptions import WorldError
from .brain import BrainWrapper, create_brain

# Import the original C++ classes
import pynlm

def create_simple_world() -> pynlm.SimpleWorld:
    """Create a new simple world."""
    return pynlm.createSimpleWorld()

def create_world_with_config(width: int, height: int, vision_width: int = 8, vision_height: int = 8) -> pynlm.SimpleWorld:
    """
    Create a simple world with configuration.
    
    Args:
        width: World width
        height: World height
        vision_width: Vision width
        vision_height: Vision height
        
    Returns:
        SimpleWorld instance
    """
    world = create_simple_world()
    world.configure(width, height, vision_width, vision_height)
    return world

class WorldWrapper:
    """
    Enhanced Python wrapper for NLM world with convenience methods.
    
    This class provides a more Pythonic interface to world simulation,
    including validation, helper methods, and convenient property access.
    """
    
    def __init__(self, world: pynlm.SimpleWorld):
        """
        Initialize the world wrapper.
        
        Args:
            world: The underlying C++ SimpleWorld object
        """
        self._world = world
    
    def configure(self, width: int, height: int, vision_width: int = 8, vision_height: int = 8) -> None:
        """
        Configure the world.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        """
        self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self._world.reset()
    
    def set_agent_start(self, x: float, y: float) -> None:
        """
        Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.setAgentStart(x, y)
    
    def update(self, timestep: float) -> None:
        """
        Update world simulation.
        
        Args:
            timestep: Time step size
        """
        self._world.update(timestep)
    
    def apply_motor_command(self, action: Any, current_time: float) -> None:
        """
        Apply motor command to world.
        
        Args:
            action: Motor command
            current_time: Current simulation time
        """
        self._world.applyMotorCommand(action, current_time)
    
    def get_sensory_percept(self) -> Any:
        """
        Get sensory percept from world.
        
        Returns:
            Sensory percept object
        """
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> Any:
        """
        Get agent body state.
        
        Returns:
            Agent body object
        """
        return self._world.getAgentBody()
    
    def add_object(self, obj: Any) -> None:
        """
        Add object to world.
        
        Args:
            obj: World object
        """
        self._world.addObject(obj)
    
    def remove_object(self, x: float, y: float) -> None:
        """
        Remove object from world.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if position is valid in world.
        
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
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    def set_max_energy(self, energy: float) -> None:
        """
        Set maximum energy.
        
        Args:
            energy: Maximum energy value
        """
        self._world.setMaxEnergy(energy)
    
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    def set_random_seed(self, seed: int) -> None:
        """
        Set random seed.
        
        Args:
            seed: Random seed
        """
        self._world.setRandomSeed(seed)
    
    def get_random_seed(self) -> int:
        """Get random seed."""
        return self._world.getRandomSeed()
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get world statistics as a dictionary.
        
        Returns:
            Dictionary of world statistics
        """
        agent_body = self.get_agent_body()
        return {
            'width': self.get_width(),
            'height': self.get_height(),
            'agent_position': (agent_body.x, agent_body.y),
            'agent_orientation': agent_body.orientation,
            'agent_energy': agent_body.energy,
            'agent_health': agent_body.health,
            'simulation_time': self.get_simulation_time(),
            'random_seed': self.get_random_seed(),
        }
    
    def step(self, timestep: float) -> None:
        """
        Convenience method for world update step.
        
        Args:
            timestep: Time step size
        """
        self.update(timestep)
    
    def move_agent(self, dx: float, dy: float) -> bool:
        """
        Move agent by relative amounts.
        
        Args:
            dx: Change in X coordinate
            dy: Change in Y coordinate
            
        Returns:
            True if move succeeded, False if position is invalid
        """
        agent_body = self.get_agent_body()
        new_x = agent_body.x + dx
        new_y = agent_body.y + dy
        
        if self.is_valid_position(new_x, new_y):
            # This is a simplified move - real implementation would need motor commands
            self.set_agent_start(new_x, new_y)
            return True
        return False
    
    def distance_to_target(self, target_x: float, target_y: float) -> float:
        """
        Calculate distance from agent to target.
        
        Args:
            target_x: Target X coordinate
            target_y: Target Y coordinate
            
        Returns:
            Distance to target
        """
        agent_body = self.get_agent_body()
        dx = target_x - agent_body.x
        dy = target_y - agent_body.y
        return (dx * dx + dy * dy) ** 0.5
    
    def is_agent_moving(self) -> bool:
        """Check if agent is currently moving."""
        agent_body = self.get_agent_body()
        return agent_body.isMoving

# Context manager for World
from contextlib import contextmanager

@contextmanager
def world_context(world: WorldWrapper, auto_reset: bool = True) -> Any:
    """
    Context manager for world operations.
    
    Args:
        world: World wrapper instance
        auto_reset: Whether to automatically reset the world
        
    Yields:
        World wrapper
        
    Example:
        world = pynlm.create_simple_world()
        wrapper = WorldWrapper(world)
        wrapper.configure(100, 100, 20, 20)
        
        with world_context(wrapper) as world:
            for step in range(100):
                world.update(0.1)
    """
    try:
        yield world
    finally:
        if auto_reset:
            world.reset()

# Convenience functions
def run_world_simulation(
    width: int = 100,
    height: int = 100,
    vision_width: int = 20,
    vision_height: int = 20,
    steps: int = 100,
    timestep: float = 0.1,
    initial_position: Optional[Tuple[float, float]] = None,
    random_seed: Optional[int] = None
) -> WorldWrapper:
    """
    Run a complete world simulation with convenient API.
    
    Args:
        width: World width
        height: World height
        vision_width: Vision width
        vision_height: Vision height
        steps: Number of simulation steps
        timestep: Time step size
        initial_position: Initial agent position (x, y)
        random_seed: Random seed
        
    Returns:
        World wrapper instance
    """
    world = create_world_with_config(width, height, vision_width, vision_height)
    wrapper = WorldWrapper(world)
    
    if initial_position:
        wrapper.set_agent_start(initial_position[0], initial_position[1])
    
    if random_seed is not None:
        wrapper.set_random_seed(random_seed)
    
    with world_context(wrapper):
        for step in range(steps):
            wrapper.update(timestep)
    
    return wrapper
def create_agent_simulation(
    brain: pynlm.Brain,
    world_config: Dict[str, Any] = None,
    steps: int = 100,
    timestep: float = 0.1
) -> Tuple[WorldWrapper, BrainWrapper]:
    """
    Create an agent simulation with brain and world.
    
    Args:
        brain: Brain instance
        world_config: World configuration dictionary
        steps: Number of simulation steps
        timestep: Time step size
        
    Returns:
        Tuple of (WorldWrapper, BrainWrapper)
    """
    world = create_simple_world()
    
    if world_config:
        world.configure(
            world_config.get('width', 100),
            world_config.get('height', 100),
            world_config.get('vision_width', 20),
            world_config.get('vision_height', 20)
        )
    
    wrapper = WorldWrapper(world)
    brain_wrapper = BrainWrapper(brain)
    
    # Create agent brain interface
    agent_brain = pynlm.createAgentBrain(brain)
    
    with world_context(wrapper):
        for step in range(steps):
            wrapper.update(timestep)
            
            percept = wrapper.get_sensory_percept()
            agent_brain.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent_brain.decodeMotorCommand()
            wrapper.apply_motor_command(action, wrapper.get_simulation_time())
    
    return wrapper, brain_wrapper

# Re-export enhanced functions and classes
__all__ = [
    'create_simple_world',
    'create_world_with_config',
    'WorldWrapper',
    'world_context',
    'run_world_simulation',
    'create_agent_simulation',
]
