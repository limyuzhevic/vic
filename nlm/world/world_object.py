"""
WorldObject class for NLM neural simulation framework.

This module provides a Python interface to the C++ WorldObject class with enhanced
Python features including automatic numpy conversion, error formatting, and
time measurement utilities.
"""

import time
import numpy as np
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass
import traceback

# Python utilities
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
)

# Forward references
WorldObjectType = Any

@dataclass
class WorldObjectState:
    """State information for world object"""
    x: float = 0.0
    y: float = 0.0
    radius: float = 0.5
    type: WorldObjectType = WorldObjectType.Empty
    value: float = 0.0
    active: bool = True

class WorldObject:
    """
    WorldObject class for NLM neural simulation framework.
    
    This class provides a Python interface to the underlying C++ WorldObject class
    with enhanced Python features including numpy integration, error formatting,
    and performance timing utilities.
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = None, 
                 radius: float = 0.5, value: float = 0.0):
        """
        Initialize world object.
        
        Args:
            x: X-coordinate (default: 0.0)
            y: Y-coordinate (default: 0.0)
            obj_type: Object type (default: Empty)
            radius: Object radius (default: 0.5)
            value: Object value (default: 0.0)
        """
        self._logger = PythonSessionLogger()
        self._state = WorldObjectState()
        
        # Set state
        self._state.x = x
        self._state.y = y
        self._state.type = obj_type if obj_type is not None else WorldObjectType.Empty
        self._state.radius = radius
        self._state.value = value
        
        # Log initialization
        self._logger.logEntry(f"WorldObject initialized: type={self._state.type.name if hasattr(self._state.type, 'name') else str(self._state.type)}, position=({x}, {y})")
    
    # Position properties
    
    @property
    def x(self) -> float:
        """Get x-coordinate."""
        return self._state.x
    
    @x.setter
    def x(self, value: float) -> None:
        """
        Set x-coordinate.
        
        Args:
            value: X-coordinate
        """
        self._state.x = value
    
    @property
    def y(self) -> float:
        """Get y-coordinate."""
        return self._state.y
    
    @y.setter
    def y(self, value: float) -> None:
        """
        Set y-coordinate.
        
        Args:
            value: Y-coordinate
        """
        self._state.y = value
    
    @property
    def radius(self) -> float:
        """Get object radius."""
        return self._state.radius
    
    @radius.setter
    def radius(self, value: float) -> None:
        """
        Set object radius.
        
        Args:
            value: Object radius
        """
        self._state.radius = max(0.0, value)
    
    @property
    def type(self) -> WorldObjectType:
        """Get object type."""
        return self._state.type
    
    @type.setter
    def type(self, value: WorldObjectType) -> None:
        """
        Set object type.
        
        Args:
            value: Object type
        """
        self._state.type = value
    
    @property
    def value(self) -> float:
        """Get object value."""
        return self._state.value
    
    @value.setter
    def value(self, value: float) -> None:
        """
        Set object value.
        
        Args:
            value: Object value
        """
        self._state.value = value
    
    @property
    def active(self) -> bool:
        """Check if object is active."""
        return self._state.active
    
    @active.setter
    def active(self, value: bool) -> None:
        """
        Set object active state.
        
        Args:
            value: Active state
        """
        self._state.active = value
    
    # Object operations
    
    def move(self, dx: float, dy: float) -> None:
        """
        Move object by specified offsets.
        
        Args:
            dx: X offset
            dy: Y offset
        """
        self.x += dx
        self.y += dy
        self._logger.logEntry(f"WorldObject moved: dx={dx}, dy={dy}, new_position=({self.x}, {self.y})")
    
    def rotate(self, angle: float) -> None:
        """
        Rotate object by specified angle.
        
        Args:
            angle: Angle in degrees
        """
        # This is a placeholder - WorldObject doesn't have orientation in C++
        self._logger.logEntry(f"WorldObject rotated: angle={angle}")
    
    def scale(self, factor: float) -> None:
        """
        Scale object by specified factor.
        
        Args:
            factor: Scaling factor
        """
        self.radius *= factor
        self._logger.logEntry(f"WorldObject scaled: factor={factor}, new_radius={self.radius}")
    
    def setProperties(self, radius: float = None, value: float = None, active: bool = None) -> None:
        """
        Set object properties.
        
        Args:
            radius: Object radius (optional)
            value: Object value (optional)
            active: Active state (optional)
        """
        if radius is not None:
            self.radius = radius
        if value is not None:
            self.value = value
        if active is not None:
            self.active = active
        
        self._logger.logEntry(f"WorldObject properties set: radius={self.radius}, value={self.value}, active={self.active}")
    
    def getDistanceTo(self, other: 'WorldObject') -> float:
        """
        Get distance to another object.
        
        Args:
            other: Other world object
            
        Returns:
            Distance between objects
        """
        dx = self.x - other.x
        dy = self.y - other.y
        return np.sqrt(dx * dx + dy * dy)
    
    def isColliding(self, other: 'WorldObject') -> bool:
        """
        Check if object is colliding with another object.
        
        Args:
            other: Other world object
            
        Returns:
            True if objects are colliding, False otherwise
        """
        distance = self.getDistanceTo(other)
        return distance <= (self.radius + other.radius)
    
    def containsPoint(self, px: float, py: float) -> bool:
        """
        Check if object contains a point.
        
        Args:
            px: X-coordinate of point
            py: Y-coordinate of point
            
        Returns:
            True if object contains point, False otherwise
        """
        dx = self.x - px
        dy = self.y - py
        return (dx * dx + dy * dy) <= (self.radius * self.radius)
    
    def getArea(self) -> float:
        """
        Get object area.
        
        Returns:
            Object area
        """
        return np.pi * self.radius * self.radius
    
    def getPerimeter(self) -> float:
        """
        Get object perimeter.
        
        Returns:
            Object perimeter
        """
        return 2 * np.pi * self.radius
    
    def reset(self) -> None:
        """Reset object state."""
        self.active = True
        self._logger.logEntry(f"WorldObject reset: type={self._state.type.name if hasattr(self._state.type, 'name') else str(self._state.type)}")
    
    def __repr__(self) -> str:
        """String representation of world object."""
        return f"<WorldObject: type={self._state.type.name if hasattr(self._state.type, 'name') else str(self._state.type)}, position=({self.x}, {self.y}), radius={self.radius}>"
    
    def __str__(self) -> str:
        """String representation of world object."""
        return f"NLM WorldObject {self._state.type.name if hasattr(self._state.type, 'name') else str(self._state.type)} at ({self.x}, {self.y})"

__all__ = ['WorldObject']
