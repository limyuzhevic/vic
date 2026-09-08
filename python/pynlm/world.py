"""
World simulation module for agent-environment interaction.

This module provides classes for world simulation, including simple 2D worlds,
agent body management, sensory perception, and world object interactions.
"""

from typing import Dict, Any, Optional, List, Tuple
from enum import Enum
import json
import math

from .exceptions import (
    NLMError,
    ConfigurationError,
    InitializationError,
    SimulationError,
    ResourceError,
)

class WorldObjectType(Enum):
    """Enumeration of world object types."""
    Empty = "empty"
    Resource = "resource"
    Hazard = "hazard"
    Wall = "wall"
    Marker = "marker"


class WorldObject:
    """
    Represents an object in the simulated world.
    
    This class models objects in the simulated environment, including their
    position, type, and associated properties.
    """

    def __init__(self, x: float, y: float, obj_type: WorldObjectType, value: float = 1.0):
        """
        Initialize a world object.
        
        Args:
            x: X coordinate
            y: Y coordinate
            obj_type: Type of object
            value: Value or energy of the object
        """
        self._x = x
        self._y = y
        self._type = obj_type
        self._value = value
        self._active = True

    def __repr__(self) -> str:
        """String representation."""
        return f"WorldObject(x={self._x}, y={self._y}, type={self._type}, value={self._value})"

    # Properties for Pythonic interface

    @property
    def x(self) -> float:
        """Get X coordinate."""
        return self._x

    @property
    def y(self) -> float:
        """Get Y coordinate."""
        return self._y

    @property
    def type(self) -> WorldObjectType:
        """Get object type."""
        return self._type

    @property
    def value(self) -> float:
        """Get object value."""
        return self._value

    @property
    def active(self) -> bool:
        """Check if object is active."""
        return self._active


class SimpleWorld:
    """
    Simple 2D world simulation for agent-environment interaction.
    
    This class provides a simple 2D simulation world for testing and
    demonstration purposes. It includes agent body management, sensory
    perception, and world object interactions.
    """

    def __init__(self):
        """Initialize a new SimpleWorld instance."""
        self._width = 20
        self._height = 20
        self._vision_width = 8
        self._vision_height = 8
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_angle = 0.0
        self._simulation_time = 0.0
        self._max_energy = 100.0
        self._energy = self._max_energy
        self._objects: List[WorldObject] = []
        self._sensory_percept = None
        self._initialized = False

    def __enter__(self):
        """Context manager entry point."""
        self.reset()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit point."""
        self.reset()

    def configure(
        self,
        width: Optional[int] = None,
        height: Optional[int] = None,
        vision_width: Optional[int] = None,
        vision_height: Optional[int] = None,
    ) -> None:
        """
        Configure world parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        if width is not None:
            self._width = width
        if height is not None:
            self._height = height
        if vision_width is not None:
            self._vision_width = vision_width
        if vision_height is not None:
            self._vision_height = vision_height

    def reset(self) -> None:
        """Reset world to initial state."""
        self._simulation_time = 0.0
        self._energy = self._max_energy
        self._objects = []
        self._sensory_percept = None
        self._initialized = True

    def update(self, timestep: float) -> Dict[str, Any]:
        """
        Update world state for a time step.
        
        Args:
            timestep: Time step for update
            
        Returns:
            Dictionary with update results
            """
        self._simulation_time += timestep
        self._energy -= 0.1 * timestep  # Energy consumption

        # Update world objects
        self._update_objects(timestep)

        # Update sensory percept
        self._update_sensory_percept()

        return {
            "time": self._simulation_time,
            "energy": self._energy,
            "objects_updated": len(self._objects),
        }

    def set_agent_start(self, x: float, y: float) -> None:
        """
        Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._agent_x = x
        self._agent_y = y

    def apply_motor_command(self, motor_cmd: Dict[str, Any], current_time: float) -> Dict[str, Any]:
        """
        Apply motor command to agent.
        
        Args:
            motor_cmd: Motor command dictionary
            current_time: Current simulation time
            
        Returns:
            Dictionary with application results
        """
        # Extract command parameters
        action_type = motor_cmd.get("action_type", "move_forward")
        magnitude = motor_cmd.get("magnitude", 1.0)

        # Apply movement
        if action_type == "move_forward":
            angle = self._agent_angle
            self._agent_x += magnitude * math.cos(math.radians(angle))
            self._agent_y += magnitude * math.sin(math.radians(angle))

        # Keep agent in bounds
        self._agent_x = max(0, min(self._agent_x, self._width))
        self._agent_y = max(0, min(self._agent_y, self._height))

        return {
            "position": (self._agent_x, self._agent_y),
            "energy": self._energy,
            "time": current_time,
        }

    def get_sensory_percept(self) -> "SensoryPercept":
        """
        Get current sensory percept.
        
        Returns:
            Sensory percept object
        """
        if self._sensory_percept is None:
            self._update_sensory_percept()

        return self._sensory_percept

    def get_agent_body(self) -> Dict[str, Any]:
        """Get agent body information."""
        return {
            "x": self._agent_x,
            "y": self._agent_y,
            "angle": self._agent_angle,
            "energy": self._energy,
            "vision_range": self._vision_width,
        }

    def add_object(self, obj: WorldObject) -> None:
        """
        Add object to world.
        
        Args:
            obj: Object to add
        """
        self._objects.append(obj)

    def remove_object(self, x: float, y: float) -> bool:
        """
        Remove object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if object was removed, False otherwise
        """
        for obj in self._objects:
            if obj.x == x and obj.y == y:
                self._objects.remove(obj)
                return True
        return False

    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if position is valid.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid, False otherwise
        """
        return 0 <= x <= self._width and 0 <= y <= self._height

    # Properties for Pythonic interface

    @property
    def width(self) -> int:
        """Get world width."""
        return self._width

    @property
    def height(self) -> int:
        """Get world height."""
        return self._height

    @property
    def simulation_time(self) -> float:
        """Get simulation time."""
        return self._simulation_time

    @property
    def max_energy(self) -> float:
        """Get maximum energy."""
        return self._max_energy

    @property
    def is_initialized(self) -> bool:
        """Check if world is initialized."""
        return self._initialized

    # Configuration

    def set_max_energy(self, value: float) -> None:
        """Set maximum energy."""
        self._max_energy = value

    def set_random_seed(self, seed: int) -> None:
        """Set random seed."""
        import random
        random.seed(seed)

    def _update_objects(self, timestep: float) -> None:
        """Update world objects."""
        pass

    def _update_sensory_percept(self) -> None:
        """Update sensory percept."""
        from .sensory import SensoryPercept

        self._sensory_percept = SensoryPercept(self._agent_x, self._agent_y, self._width, self._height, self._objects)


def create_simple_world() -> SimpleWorld:
    """
    Create a new SimpleWorld instance.
    
    Returns:
        SimpleWorld instance
    """
    return SimpleWorld()