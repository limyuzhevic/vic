# Objects module for NLM
from typing import Optional, List
import time
from enum import Enum

# Action result from world
class ActionResult:
    """
    Action result from world.
    
    Contains the result of an action applied to the world,
    including reward, success status, and any messages.
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        """
        Initialize action result.
        
        Args:
            reward: Reward received from action
            success: Whether action succeeded
            message: Optional message describing result
        """
        self.reward = reward
        self.success = success
        self.message = message
        self.timestamp = time.time()
    
    def __repr__(self):
        return f"ActionResult(reward={self.reward}, success={self.success}, message='{self.message}')"

# World object types
class WorldObjectType(Enum):
    """World object type enumeration."""
    
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

# World object representation
class WorldObject:
    """
    World object representation.
    
    Represents an object in the 2D world with position, type, and value.
    Objects can be resources (positive value), hazards (negative value),
    walls (impassable), or markers.
    """
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty,
                 value: float = 0.0, radius: float = 0.5):
        """
        Initialize world object.
        
        Args:
            x: X coordinate
            y: Y coordinate
            obj_type: Object type
            value: Object value (reward for resources, damage for hazards)
            radius: Object radius
        """
        self.x = x
        self.y = y
        self.radius = radius
        self.type = obj_type
        self.value = value  # Reward value (for resources) or damage (for hazards)
        self.active = True
    
    def __repr__(self):
        return f"WorldObject(x={self.x}, y={self.y}, type={self.type.name}, value={self.value})"

# Action type enumeration
class ActionType(Enum):
    """Action type enumeration."""
    
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14

# Factory functions

def createWorldObject(x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty,
                      value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """Create a world object.
    
    Args:
        x: X coordinate
        y: Y coordinate
        obj_type: Object type
        value: Object value
        radius: Object radius
        
    Returns:
        WorldObject: New world object
    """
    return WorldObject(x, y, obj_type, value, radius)

def createActionResult(reward: float = 0.0, success: bool = False, message: str = "") -> ActionResult:
    """Create an action result.
    
    Args:
        reward: Reward received
        success: Whether action succeeded
        message: Optional message
        
    Returns:
        ActionResult: New action result
    """
    return ActionResult(reward, success, message)
