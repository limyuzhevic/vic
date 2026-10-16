"""Enhanced world simulation for NLM.

This module provides enhanced world simulation for the NLM (Neural Learning Machine)
framework with Pythonic interfaces, object history tracking, and enhanced
configuration support. It builds upon the original SimpleWorld class to
provide more intuitive and robust world simulation interfaces.

Key features:
- EnhancedWorld with Pythonic interfaces
- Object history tracking
- Enhanced configuration helpers
- Simulation run methods
- World object management
- Position validation and utilities

Usage examples:

    from pynlm.enhanced.world import EnhancedWorld

    # Create enhanced world
    world = EnhancedWorld()
    world.configure_with_defaults(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    # Add object with history
    obj = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0)
    world.add_object_with_history(obj)
    
    # Run simulation
    states = world.run_simulation(agent, duration=10.0, timestep=0.1)
    
    # Get world statistics
    print(f"Width: {world.getWidth()}, Height: {world.getHeight()}")
    print(f"Objects in history: {len(world.object_history)}")

The world simulation module includes:

1. EnhancedWorld class - Extended world with enhanced features
2. WorldObject with enhanced interfaces
3. World configuration helpers
4. Simulation run methods
5. Object history tracking
6. Position validation and utilities
7. Backward compatibility with original SimpleWorld
"""

from typing import Dict, Any, Optional, List, Iterator
from contextlib import contextmanager
from .. import SimpleWorld as OriginalSimpleWorld

# Import from enhanced_api
from ..enhanced_api import (
    EnhancedWorld as EnhancedWorldClass,
    ValidationError,
    NeuralNetworkError,
    ErrorHandler
)

# Re-export class
EnhancedWorld = EnhancedWorldClass

class WorldStats:
    """Statistics for world simulation.
    
    The WorldStats class collects and summarizes statistics for world simulation,
    including object counts, agent positions, and simulation metrics.
    
    Attributes:
        steps_completed: Number of steps completed
        objects_added: Number of objects added
        objects_removed: Number of objects removed
        agent_positions: List of agent positions over time
        simulation_time: Current simulation time
        valid_positions: Number of valid positions checked
    """
    
    def __init__(self):
        self.steps_completed = 0
        self.objects_added = 0
        self.objects_removed = 0
        self.agent_positions: List[Dict[str, float]] = []
        self.simulation_time = 0.0
        self.valid_positions = 0
    
    def record_step(self, agent_x: float, agent_y: float, time: float) -> None:
        """Record a simulation step.
        
        Args:
            agent_x: Agent x position
            agent_y: Agent y position
            time: Simulation time
        """
        self.steps_completed += 1
        self.simulation_time = time
        self.agent_positions.append({'x': agent_x, 'y': agent_y})
    
    def record_object_added(self) -> None:
        """Record that an object was added."""
        self.objects_added += 1
    
    def record_object_removed(self) -> None:
        """Record that an object was removed."""
        self.objects_removed += 1
    
    def record_valid_position(self) -> None:
        """Record that a position was checked and found valid."""
        self.valid_positions += 1
    
    def __repr__(self) -> str:
        """String representation of world stats.
        
        Args:
            
        Returns:
            String representation
        """
        return (f"WorldStats(steps={self.steps_completed}, "
                f"objects_added={self.objects_added}, "
                f"objects_removed={self.objects_removed}, "
                f"simulation_time={self.simulation_time:.1f})")

# Factory function for creating enhanced worlds
def create_enhanced_world() -> EnhancedWorld:
    """Create an enhanced world.
    
    Args:
        
    Returns:
        Enhanced world instance
    """
    return EnhancedWorld()

# World configuration helper functions
def configure_world(world: EnhancedWorld, width: int, height: int, 
                  vision_width: int, vision_height: int) -> None:
    """Configure world with specified parameters.
    
    Args:
        world: World to configure
        width: World width
        height: World height
        vision_width: Vision sensor width
        vision_height: Vision sensor height
    """
    world.configure(width, height, vision_width, vision_height)

def configure_world_with_defaults(world: EnhancedWorld, 
                                  width: int = 20, height: int = 20,
                                  vision_width: int = 8, vision_height: int = 8) -> None:
    """Configure world with default parameters.
    
    Args:
        world: World to configure
        width: World width (default: 20)
        height: World height (default: 20)
        vision_width: Vision sensor width (default: 8)
        vision_height: Vision sensor height (default: 8)
    """
    world.configure_with_defaults(width, height, vision_width, vision_height)

# World simulation runner function
def run_world_simulation(world: EnhancedWorld, agent, duration: float, 
                        timestep: float = 0.1) -> List[Dict[str, Any]]:
    """Run world simulation.
    
    Args:
        world: World to simulate
        agent: Agent to control
        duration: Simulation duration
        timestep: Time step size (default: 0.1)
        
    Returns:
        List of simulation states
    """
    return world.run_simulation(agent, duration, timestep)

# Export all public classes and functions
__all__ = [
    'EnhancedWorld',
    'WorldStats',
    'create_enhanced_world',
    'configure_world',
    'configure_world_with_defaults',
    'run_world_simulation',
]