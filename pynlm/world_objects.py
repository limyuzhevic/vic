# World objects implementation for NLM Python API

"""
World objects for NLM Python API

Provides Pythonic interfaces to world objects:
WorldObject and WorldObjectType classes.
"""

from typing import Any, Optional
from enum import Enum

# Import the C++ binding and type definitions
from pynlm.types import WorldObjectType as CppWorldObjectType

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class WorldObjectError(NLMError):
    """Raised when world object operations fail"""
    pass

class WorldObjectType(Enum):
    """
    World object type enumeration
    
    Enumeration of possible object types in the NLM world.
    
    Attributes:
        Empty: Empty/invalid cell
        Resource: Resource object
        Hazard: Hazard object  
        Wall: Wall/object boundary
        Marker: Special marker object
    """
    
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4
    
    @classmethod
    def get_name(cls, value: int) -> str:
        """
        Get friendly name for world object type.
        
        Args:
            value: Object type value
            
        Returns:
            Friendly name
        """
        try:
            return cls(value).name
        except ValueError:
            return f"Unknown({value})"
    
    @classmethod
    def is_solid(cls, value: int) -> bool:
        """
        Check if object type is solid (blocks movement).
        
        Args:
            value: Object type value
            
        Returns:
            True if solid
        """
        try:
            return cls(value) in (cls.Wall, cls.Hazard)
        except ValueError:
            return False
    
    @classmethod
    def is_passable(cls, value: int) -> bool:
        """
        Check if object type is passable (allows movement).
        
        Args:
            value: Object type value
            
        Returns:
            True if passable
        """
        return not cls.is_solid(value)
    
    @classmethod
    def is_collideable(cls, value: int) -> bool:
        """
        Check if object type can collide with agent.
        
        Args:
            value: Object type value
            
        Returns:
            True if collideable
        """
        return cls(value) in (cls.Wall, cls.Hazard, cls.Resource)
    
    @classmethod
    def get_value_range(cls, value: int) -> tuple[float, float]:
        """
        Get value range for object type.
        
        Args:
            value: Object type value
            
        Returns:
            (min_value, max_value) for object type
        """
        try:
            stage = cls(value)
            if stage == cls.Resource:
                return (1.0, 10.0)  # Resource value range
            elif stage == cls.Hazard:
                return (1.0, 5.0)   # Hazard value range
            else:
                return (0.0, 1.0)   # Default range
        except ValueError:
            return (0.0, 1.0)

class WorldObject:
    """
    World object representation
    
    Represents objects in the NLM world simulation.
    Objects can be resources, hazards, walls, markers, etc.
    
    Examples:
        >>> obj = WorldObject(x=3, y=3, type=WorldObjectType.Resource, value=5.0)
        >>> print(f"Object at ({obj.x}, {obj.y})")
        >>> print(f"Type: {obj.type}")
        >>> print(f"Value: {obj.value}")
        >>> 
        >>> wall = WorldObject(x=1, y=1, type=WorldObjectType.Wall, radius=0.8)
        >>> hazard = WorldObject(x=5, y=5, type=WorldObjectType.Hazard, value=2.0)
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty, 
                 value: float = 0.0, radius: float = 0.5):
        """
        Initialize world object.
        
        Args:
            x: X coordinate
            y: Y coordinate
            obj_type: Object type
            value: Object value (e.g., resource amount, hazard severity)
            radius: Object radius (for collision detection)
            
        Raises:
            WorldObjectError: If object cannot be created
        """
        # Note: In actual implementation, this would wrap C++ WorldObject object
        self.x = float(x)
        self.y = float(y)
        self.type = obj_type
        self.value = float(value)
        self.radius = float(radius)
        self.active = True
        
        # Validate parameters
        if radius <= 0:
            raise WorldObjectError(f"Invalid radius: {radius}")
        
        if obj_type in (WorldObjectType.Resource, WorldObjectType.Hazard):
            if value < 0:
                raise WorldObjectError(f"Invalid value for {obj_type}: {value}")
    
    def is_at(self, x: float, y: float) -> bool:
        """
        Check if object is at given position.
        
        Args:
            x: X coordinate to check
            y: Y coordinate to check
            
        Returns:
            True if object is at position
            
        Examples:
            >>> obj = WorldObject(x=3.0, y=3.0, radius=0.5)
            >>> if obj.isAt(3.0, 3.0):
            ...     print("Object is at position")
        """
        distance = ((self.x - x) ** 2 + (self.y - y) ** 2) ** 0.5
        return distance <= self.radius
    
    def get_value_range(self) -> tuple[float, float]:
        """
        Get value range for object type.
        
        Returns:
            (min_value, max_value) for object type
            
        Examples:
            >>> obj = WorldObject(type=WorldObjectType.Resource)
            >>> min_val, max_val = obj.getValueRange()
            >>> print(f"Value range: [{min_val}, {max_val}]")
        """
        return WorldObjectType.get_value_range(self.type.value)
    
    def is_passable(self) -> bool:
        """
        Check if object is passable (allows movement).
        
        Returns:
            True if passable
            
        Examples:
            >>> obj = WorldObject(type=WorldObjectType.Wall)
            >>> if not obj.isPassable():
            ...     print("Wall blocks movement")
        """
        return WorldObjectType.is_passable(self.type.value)
    
    def is_solid(self) -> bool:
        """
        Check if object is solid (blocks movement).
        
        Returns:
            True if solid
            
        Examples:
            >>> obj = WorldObject(type=WorldObjectType.Wall)
            >>> if obj.isSolid():
            ...     print("Wall is solid")
        """
        return WorldObjectType.is_solid(self.type.value)
    
    def is_collideable(self) -> bool:
        """
        Check if object can collide with agent.
        
        Returns:
            True if collideable
            
        Examples:
            >>> obj = WorldObject(type=WorldObjectType.Resource)
            >>> if obj.isCollideable():
            ...     print("Object can be collected")
        """
        return WorldObjectType.is_collideable(self.type.value)
    
    def distance_to(self, other_x: float, other_y: float) -> float:
        """
        Get distance to other position.
        
        Args:
            other_x: Other X coordinate
            other_y: Other Y coordinate
            
        Returns:
            Distance to other position
            
        Examples:
            >>> obj = WorldObject(x=1.0, y=2.0, radius=0.5)
            >>> distance = obj.distanceTo(4.0, 6.0)
            >>> print(f"Distance: {distance}")
        """
        return ((self.x - other_x) ** 2 + (self.y - other_y) ** 2) ** 0.5
    
    def center_distance_to(self, other_x: float, other_y: float) -> float:
        """
        Get distance from object center to other position.
        
        Args:
            other_x: Other X coordinate
            other_y: Other Y coordinate
            
        Returns:
            Distance from center
            
        Examples:
            >>> obj = WorldObject(x=5.0, y=5.0, radius=0.5)
            >>> distance = obj.centerDistanceTo(5.5, 5.5)
            >>> print(f"Center distance: {distance}")
        """
        return self.distance_to(other_x, other_y)
    
    def overlaps_with(self, other_x: float, other_y: float, other_radius: float) -> bool:
        """
        Check if object overlaps with other position.
        
        Args:
            other_x: Other X coordinate
            other_y: Other Y coordinate
            other_radius: Other object radius
            
        Returns:
            True if objects overlap
            
        Examples:
            >>> obj = WorldObject(x=3.0, y=3.0, radius=0.5)
            >>> if obj.overlapsWith(3.4, 3.4, 0.3):
            ...     print("Objects overlap")
        """
        distance = self.distance_to(other_x, other_y)
        return distance < (self.radius + other_radius)
    
    def contains_point(self, x: float, y: float) -> bool:
        """
        Check if object contains given point.
        
        Args:
            x: X coordinate to check
            y: Y coordinate to check
            
        Returns:
            True if point is inside object
            
        Examples:
            >>> obj = WorldObject(x=3.0, y=3.0, radius=0.5)
            >>> if obj.containsPoint(3.2, 3.2):
            ...     print("Point is inside object")
        """
        return self.is_at(x, y)
    
    def get_bounding_box(self) -> dict:
        """
        Get object bounding box.
        
        Returns:
            Dictionary with x, y, width, height
            
        Examples:
            >>> obj = WorldObject(x=3.0, y=3.0, radius=0.5)
            >>> bbox = obj.getBoundingBox()
            >>> print(f"Bounding box: {bbox}")
        """
        return {
            'x': self.x - self.radius,
            'y': self.y - self.radius,
            'width': 2 * self.radius,
            'height': 2 * self.radius
        }
    
    def update(self, x: Optional[float] = None, y: Optional[float] = None, 
               value: Optional[float] = None, active: Optional[bool] = None) -> None:
        """
        Update object properties.
        
        Args:
            x: New X coordinate (if None, unchanged)
            y: New Y coordinate (if None, unchanged)
            value: New value (if None, unchanged)
            active: New active state (if None, unchanged)
            
        Examples:
            >>> obj = WorldObject(x=1.0, y=2.0)
            >>> obj.update(x=1.5, value=7.0)  # Update x and value
        """
        if x is not None:
            self.x = float(x)
        if y is not None:
            self.y = float(y)
        if value is not None:
            self.value = float(value)
        if active is not None:
            self.active = bool(active)
    
    def __repr__(self) -> str:
        """String representation of world object"""
        return f"<WorldObject type={self.type.name} at ({self.x:.1f}, {self.y:.1f}), value={self.value:.1f}, radius={self.radius:.1f}>"
    
    def __str__(self) -> str:
        """String representation of world object"""
        return self.__repr__()

# Factory functions for world objects
def create_world_object(x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty,
                        value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """
    Create a world object.
    
    Args:
        x: X coordinate
        y: Y coordinate
        obj_type: Object type
        value: Object value
        radius: Object radius
        
    Returns:
        New WorldObject instance
        
    Examples:
        >>> obj = createWorldObject(3, 3, WorldObjectType.Resource, 5.0)
        >>> wall = createWorldObject(1, 1, WorldObjectType.Wall, radius=0.8)
    """
    return WorldObject(x, y, obj_type, value, radius)

# Common world object creation functions
def create_wall(x: float, y: float, radius: float = 0.5) -> WorldObject:
    """
    Create a wall object.
    
    Args:
        x: X coordinate
        y: Y coordinate
        radius: Wall radius
        
    Returns:
        Wall object
        
    Examples:
        >>> wall = createWall(5, 5, 0.5)
    """
    return create_world_object(x, y, WorldObjectType.Wall, radius=radius)

def create_resource(x: float, y: float, value: float = 1.0, radius: float = 0.3) -> WorldObject:
    """
    Create a resource object.
    
    Args:
        x: X coordinate
        y: Y coordinate
        value: Resource value
        radius: Resource radius
        
    Returns:
        Resource object
        
    Examples:
        >>> food = createResource(3, 3, 5.0)
    """
    return create_world_object(x, y, WorldObjectType.Resource, value, radius)

def create_hazard(x: float, y: float, value: float = 1.0, radius: float = 0.3) -> WorldObject:
    """
    Create a hazard object.
    
    Args:
        x: X coordinate
        y: Y coordinate
        value: Hazard value (severity)
        radius: Hazard radius
        
    Returns:
        Hazard object
        
    Examples:
        >>> lava = createHazard(7, 7, 2.0)
    """
    return create_world_object(x, y, WorldObjectType.Hazard, value, radius)

def create_marker(x: float, y: float, radius: float = 0.2) -> WorldObject:
    """
    Create a marker object.
    
    Args:
        x: X coordinate
        y: Y coordinate
        radius: Marker radius
        
    Returns:
        Marker object
        
    Examples:
        >>> start_marker = createMarker(10, 10)
    """
    return create_world_object(x, y, WorldObjectType.Marker, radius=radius)