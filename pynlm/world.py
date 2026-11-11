# World module - Enhanced Python interface for world simulation
"""
Enhanced Python interface for the SimpleWorld class with additional functionality
and better integration with Python.
"""

from typing import List, Optional, Any, Dict
import numpy as np

# Try to import C++ bindings
try:
    from ._pynlm import createSimpleWorld as _createSimpleWorld
    CPLUSPLUS_AVAILABLE = True
except ImportError:
    CPLUSPLUS_AVAILABLE = False

class SimpleWorld:
    """Enhanced Python interface for SimpleWorld simulation.
    
    This class provides a more Pythonic interface for interacting with the world,
    including better visualization support and enhanced functionality for
    common world operations.
    """
    
    def __init__(self):
        """Initialize world."""
        if CPLUSPLUS_AVAILABLE and _createSimpleWorld is not None:
            self._world = _createSimpleWorld()
        else:
            # Create mock world for testing
            self._world = MockWorld()
    
    def configure(self, width, height, vision_width, vision_height):
        """Configure world dimensions.
        
        Args:
            width: World width.
            height: World height.
            vision_width: Vision sensor width.
            vision_height: Vision sensor height.
        """
        return self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self):
        """Reset world to initial state."""
        return self._world.reset()
    
    def set_agent_start(self, x, y):
        """Set agent starting position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
        """
        return self._world.setAgentStart(x, y)
    
    def update(self, timestep):
        """Update world state.
        
        Args:
            timestep: Current timestep.
        """
        return self._world.update(timestep)
    
    def apply_motor_command(self, cmd, current_time=None):
        """Apply motor command to world.
        
        Args:
            cmd: Motor command to apply.
            current_time: Current time (optional).
        """
        return self._world.applyMotorCommand(cmd, current_time)
    
    def get_sensory_percept(self):
        """Get current sensory percept from world.
        
        Returns:
            SensoryPercept object.
        """
        return self._world.getSensoryPercept()
    
    def get_agent_body(self):
        """Get current agent body state.
        
        Returns:
            AgentBody object.
        """
        return self._world.getAgentBody()
    
    def add_object(self, obj):
        """Add object to world.
        
        Args:
            obj: World object to add.
        """
        return self._world.addObject(obj)
    
    def remove_object(self, x, y):
        """Remove object from world.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
        """
        return self._world.removeObject(x, y)
    
    def is_valid_position(self, x, y):
        """Check if position is valid.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            
        Returns:
            True if position is valid, False otherwise.
        """
        return self._world.isValidPosition(x, y)
    
    # Convenience properties
    
    @property
    def width(self):
        """Get world width."""
        return self._world.getWidth()
    
    @property
    def height(self):
        """Get world height."""
        return self._world.getHeight()
    
    @property
    def max_energy(self):
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    @property
    def energy_decay_rate(self):
        """Get energy decay rate."""
        return self._world.getEnergyDecayRate()
    
    @property
    def simulation_time(self):
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    @property
    def random_seed(self):
        """Get random seed."""
        return self._world.getRandomSeed()
    
    @property
    def vision_width(self):
        """Get vision sensor width."""
        return self._world.getVisionWidth()
    
    @property
    def vision_height(self):
        """Get vision sensor height."""
        return self._world.getVisionHeight()
    
    @width.setter
    def width(self, value):
        """Set world width (requires reconfiguration)."""
        raise NotImplementedError("Width requires world reconfiguration")
    
    @height.setter
    def height(self, value):
        """Set world height (requires reconfiguration)."""
        raise NotImplementedError("Height requires world reconfiguration")
    
    @max_energy.setter
    def max_energy(self, value):
        """Set maximum energy."""
        return self._world.setMaxEnergy(value)
    
    @energy_decay_rate.setter
    def energy_decay_rate(self, value):
        """Set energy decay rate."""
        return self._world.setEnergyDecayRate(value)
    
    @random_seed.setter
    def random_seed(self, value):
        """Set random seed."""
        return self._world.setRandomSeed(value)
    
    # Additional world functionality
    
    def get_objects(self, object_type=None):
        """Get all objects in world (or specific type).
        
        Args:
            object_type: Type of objects to filter by (optional).
            
        Returns:
            List of world objects.
        """
        # This would need to be implemented based on world's API
        raise NotImplementedError("Object listing not yet implemented")
    
    def get_distance_to_nearest(self, x, y, object_type=None):
        """Get distance to nearest object of specified type.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            object_type: Type of objects to search for (optional).
            
        Returns:
            Distance to nearest object, or infinity if none found.
        """
        # This would need to be implemented based on world's API
        raise NotImplementedError("Distance calculation not yet implemented")
    
    def is_collision(self, x, y, radius=0.5):
        """Check for collision with world objects.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            radius: Collision radius.
            
        Returns:
            True if collision detected, False otherwise.
        """
        return not self.is_valid_position(x, y)
    
    def visualize(self, agent=None, resolution=1):
        """Visualize world state.
        
        Args:
            agent: Agent position (optional, shows agent on map).
            resolution: Visualization resolution (for downsampling).
        """
        # This would generate a text-based visualization
        width = self.width
        height = self.height
        
        print(f"World Visualization ({width}x{height}):")
        print("=" * (width + 2))
        
        for y in range(height - 1, -1, -1):
            row = "|"
            for x in range(width):
                if self.is_valid_position(x, y):
                    # Find object type
                    obj_type = "  "
                    if agent and abs(agent.x - x) <= resolution and abs(agent.y - y) <= resolution:
                        obj_type = "A "
                    row += obj_type
                else:
                    row += "# "
            row += "|"
            print(row)
        
        print("=" * (width + 2))
        print(f"Time: {self.simulation_time:.2f}s, Energy: {agent.energy if agent else 'N/A'}")
    
    def get_world_status(self):
        """Get comprehensive world status.
        
        Returns:
            Dictionary with world status information.
        """
        agent = self.get_agent_body()
        return {
            'dimensions': {'width': self.width, 'height': self.height},
            'vision': {'width': self.vision_width, 'height': self.vision_height},
            'simulation': {'time': self.simulation_time, 'seed': self.random_seed},
            'energy': {'max': self.max_energy, 'decay_rate': self.energy_decay_rate},
            'agent': {
                'x': agent.x,
                'y': agent.y,
                'orientation': agent.orientation,
                'energy': agent.energy,
                'health': agent.health,
                'age': agent.age,
                'is_moving': agent.isMoving,
                'is_turning': agent.isTurning,
            } if agent else None,
            'valid_positions': f"{self.width * self.height} cells",
        }
    
    def reset_with_start(self, x, y, timestep=0):
        """Reset world with specific agent start position.
        
        Args:
            x: X coordinate for agent start.
            y: Y coordinate for agent start.
            timestep: Starting timestep.
        """
        self.reset()
        self.set_agent_start(x, y)
        if timestep > 0:
            self.update(timestep)
    
    def simulate_step(self, action, timestep):
        """Perform one simulation step with action.
        
        Args:
            action: Motor command to apply.
            timestep: Current timestep.
            
        Returns:
            Dictionary with step results.
        """
        # Get current percept before action
        percept = self.get_sensory_percept()
        
        # Apply action
        self.apply_motor_command(action, timestep)
        
        # Update world
        self.update(timestep)
        
        # Get new percept
        new_percept = self.get_sensory_percept()
        
        return {
            'timestep': timestep,
            'action': action,
            'percept_before': percept,
            'percept_after': new_percept,
            'agent': self.get_agent_body(),
        }
    
    def __repr__(self):
        """String representation of the world."""
        return f"<SimpleWorld: {self.width}x{self.height}, time={self.simulation_time:.2f}s>"


# Mock world class for testing when C++ bindings are not available
class MockWorld:
    def __init__(self):
        self.width = 100
        self.height = 100
        self.agent_x = 50
        self.agent_y = 50
        self.energy = 100
        
    def configure(self, width, height, vision_width, vision_height):
        self.width = width
        self.height = height
        
    def reset(self):
        pass
        
    def setAgentStart(self, x, y):
        self.agent_x = x
        self.agent_y = y
        
    def update(self, timestep):
        self.energy -= 1
        
    def applyMotorCommand(self, cmd, current_time=None):
        if cmd == "MoveForward":
            self.agent_y = min(self.height - 1, self.agent_y + 1)
            
    def getSensoryPercept(self):
        return MockVision()
        
    def getAgentBody(self):
        return MockAgentBody()
        
    def addObject(self, obj):
        pass
        
    def removeObject(self, x, y):
        pass
        
    def isValidPosition(self, x, y):
        return 0 <= x < self.width and 0 <= y < self.height
        
    def getWidth(self):
        return self.width
        
    def getHeight(self):
        return self.height
        
    def getMaxEnergy(self):
        return self.energy
        
    def getEnergyDecayRate(self):
        return 0.1
        
    def getSimulationTime(self):
        return 0.0
        
    def getRandomSeed(self):
        return 42
        
    def getVisionWidth(self):
        return 64
        
    def getVisionHeight(self):
        return 64


# Mock classes for testing
class MockVision:
    def __init__(self):
        self.width = 64
        self.height = 64
        self.channels = 3
        
    def getWidth(self):
        return self.width
        
    def getHeight(self):
        return self.height
        
    def getChannels(self):
        return self.channels

class MockAgentBody:
    def __init__(self):
        self.x = 50
        self.y = 50
        self.orientation = 0
        self.velocityX = 0
        self.velocityY = 0
        self.angularVelocity = 0
        self.energy = 100
        self.health = 100
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = time.time()
        
    def reset(self):
        self.energy = 100
        self.health = 100


# Re-export for convenience
from . import _pynlm
SimpleWorld.__base__ = _pynlm.SimpleWorld