"""
Environment and world modules for NLM.

This module contains classes for environment interaction, world objects,
observations, and actions.
"""

from enum import Enum
from typing import List, Any, Optional
import numpy as np

# Import from C++ bindings
import pynlm._bindings as bindings
class WorldObjectType(Enum):
    """Enumeration of world object types."""
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

    def __str__(self) -> str:
        return self.name
class WorldObject:
    """World object representation.
    
    Objects in the world can be resources (food, water), hazards (poison, traps),
    walls (boundaries), or markers (goals, waypoints).
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, 
                 obj_type: WorldObjectType = WorldObjectType.Empty,
                 value: float = 0.0, radius: float = 0.5):
        self._world_object = bindings.create_world_object(x, y, obj_type, value, radius)
    
    @property
    def x(self) -> float:
        """Get X coordinate."""
        return self._world_object.x
    
    @x.setter
    def x(self, value: float) -> None:
        """Set X coordinate."""
        self._world_object.x = value
    
    @property
    def y(self) -> float:
        """Get Y coordinate."""
        return self._world_object.y
    
    @y.setter
    def y(self, value: float) -> None:
        """Set Y coordinate."""
        self._world_object.y = value
    
    @property
    def radius(self) -> float:
        """Get radius."""
        return self._world_object.radius
    
    @radius.setter
    def radius(self, value: float) -> None:
        """Set radius."""
        self._world_object.radius = value
    
    @property
    def obj_type(self) -> WorldObjectType:
        """Get object type."""
        return self._world_object.type
    
    @obj_type.setter
    def obj_type(self, value: WorldObjectType) -> None:
        """Set object type."""
        self._world_object.type = value
    
    @property
    def value(self) -> float:
        """Get object value."""
        return self._world_object.value
    
    @value.setter
    def value(self, value: float) -> None:
        """Set object value."""
        self._world_object.value = value
    
    @property
    def active(self) -> bool:
        """Get active status."""
        return self._world_object.active
    
    @active.setter
    def active(self, value: bool) -> None:
        """Set active status."""
        self._world_object.active = value
    
    def distance_to(self, other: 'WorldObject') -> float:
        """Calculate distance to another object."""
        dx = self.x - other.x
        dy = self.y - other.y
        return np.sqrt(dx * dx + dy * dy)
    
    def is_colliding(self, other: 'WorldObject') -> bool:
        """Check if colliding with another object."""
        return self.distance_to(other) < (self.radius + other.radius)
    
    def contains_point(self, x: float, y: float) -> bool:
        """Check if point is inside this object."""
        dx = self.x - x
        dy = self.y - y
        return dx * dx + dy * dy <= self.radius * self.radius
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert object to dictionary."""
        return {
            'x': self.x,
            'y': self.y,
            'type': self.obj_type.value,
            'value': self.value,
            'radius': self.radius,
            'active': self.active
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'WorldObject':
        """Create object from dictionary."""
        return cls(
            x=data['x'],
            y=data['y'],
            obj_type=WorldObjectType(data['type']),
            value=data['value'],
            radius=data['radius']
        )
    
    def __repr__(self) -> str:
        return f"<WorldObject: {self.obj_type.name} at ({self.x:.1f}, {self.y:.1f}), radius={self.radius}>"

# Convenience functions for world object creation
def create_resource(x: float, y: float, value: float = 1.0, radius: float = 0.5) -> WorldObject:
    """Create a resource object."""
    obj = WorldObject(x, y, WorldObjectType.Resource, value, radius)
    return obj

def create_hazard(x: float, y: float, value: float = -1.0, radius: float = 0.5) -> WorldObject:
    """Create a hazard object."""
    obj = WorldObject(x, y, WorldObjectType.Hazard, value, radius)
    return obj

def create_wall(x: float, y: float, radius: float = 0.5) -> WorldObject:
    """Create a wall object."""
    obj = WorldObject(x, y, WorldObjectType.Wall, radius=radius)
    return obj

def create_marker(x: float, y: float, radius: float = 0.3) -> WorldObject:
    """Create a marker object."""
    obj = WorldObject(x, y, WorldObjectType.Marker, radius=radius)
    return obj