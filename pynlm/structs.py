"""
Data structures for NLM Python bindings.
"""

class WorldObject:
    """
    Represents an object in the world.
    
    World objects can be resources, hazards, walls, markers, etc.
    """
    
    def __init__(self, x, y, obj_type, value=0.0):
        """
        Initialize a world object.
        
        Args:
            x (float): X coordinate
            y (float): Y coordinate  
            obj_type (str): Type of object (see WorldObjectType)
            value (float): Value of the object (e.g., food value)
        """
        self.x = x
        self.y = y
        self.obj_type = obj_type
        self.value = value
        self.id = f"obj_{x}_{y}_{obj_type}"  # Simple ID
    
    def getX(self):
        """Get X coordinate."""
        return self.x
    
    def getY(self):
        """Get Y coordinate."""
        return self.y
    
    def getType(self):
        """Get object type."""
        return self.obj_type
    
    def getValue(self):
        """Get object value."""
        return self.value
    
    def setValue(self, value):
        """Set object value."""
        self.value = value
    
    def __repr__(self):
        return f"WorldObject(x={self.x}, y={self.y}, type={self.obj_type}, value={self.value})"
    
    def __eq__(self, other):
        if not isinstance(other, WorldObject):
            return False
        return (self.x == other.x and self.y == other.y and 
                self.obj_type == other.obj_type and self.value == other.value)


class Vector2D:
    """2D vector for position, direction, etc."""
    
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __repr__(self):
        return f"Vector2D(x={self.x}, y={self.y})"


class Size2D:
    """2D size for dimensions."""
    
    def __init__(self, width, height):
        self.width = width
        self.height = height
    
    def __repr__(self):
        return f"Size2D(width={self.width}, height={self.height})"


class Color:
    """RGB color."""
    
    def __init__(self, r, g, b, a=1.0):
        self.r = r
        self.g = g
        self.b = b
        self.a = a
    
    def __repr__(self):
        return f"Color(r={self.r}, g={self.g}, b={self.b}, a={self.a})"


__all__ = [
    'WorldObject',
    'Vector2D',
    'Size2D',
    'Color'
]
