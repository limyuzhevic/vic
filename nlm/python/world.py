#!/usr/bin/env python3
"""
NLM World Module

Provides world simulation capabilities for NLM (Neural Learning Machine).
"""

from typing import List

class WorldObjectType:
    """Enumeration of world object types."""
    Resource = "resource"
    Marker = "marker"
    Agent = "agent"
    Obstacle = "obstacle"
    Food = "food"
    Water = "water"

class WorldObject:
    """
    Represents an object in the NLM world.
    
    This class represents entities that exist in the NLM simulation world,
    such as resources, markers, and other environmental objects.
    """
    
    class WorldObjectType:
        """Enumeration of world object types (nested class)."""
        Resource = "resource"
        Marker = "marker"
        Agent = "agent"
        Obstacle = "obstacle"
        Food = "food"
        Water = "water"
    
    def __init__(self, x: float, y: float, obj_type: str, value: float):
        """Create a new world object at the given position."""
        self._x = x
        self._y = y
        self._type = obj_type
        self._value = value
    
    def getX(self) -> float:
        """Get the x-coordinate of the object."""
        return self._x
    
    def getY(self) -> float:
        """Get the y-coordinate of the object."""
        return self._y
    
    def getType(self) -> str:
        """Get the type of the object."""
        return self._type
    
    def getValue(self) -> float:
        """Get the value of the object."""
        return self._value
    
    def __str__(self):
        return f"WorldObject(x={self._x}, y={self._y}, type={self._type}, value={self._value})"

class SimpleWorld:
    """
    Simple world simulation for NLM agents.
    
    This class provides a basic 2D world environment where NLM agents
    can interact with objects and learn from their experiences.
    """
    
    def __init__(self):
        """Create a new world."""
        self._width = 100
        self._height = 100
        self._vision_width = 20
        self._vision_height = 20
        self._agent_x = 0
        self._agent_y = 0
        self._objects: List[WorldObject] = []
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure the world dimensions and vision range."""
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
    
    def setAgentStart(self, x: int, y: int):
        """Set the agent's starting position."""
        self._agent_x = x
        self._agent_y = y
    
    def set_agent_start(self, x: int, y: int):
        """Set the agent's starting position."""
        self.setAgentStart(x, y)
    
    def addObject(self, obj: WorldObject):
        """Add an object to the world."""
        self._objects.append(obj)
    
    def add_object(self, obj: WorldObject):
        """Add an object to the world (underscore alias)."""
        self.addObject(obj)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if a position is valid within the world boundaries."""
        return 0 <= x < self._width and 0 <= y < self._height
    
    def getAgentX(self) -> float:
        """Get the agent's x-coordinate."""
        return self._agent_x
    
    def getAgentY(self) -> float:
        """Get the agent's y-coordinate."""
        return self._agent_y
    
    def getObjects(self) -> List[WorldObject]:
        """Get all objects in the world."""
        return self._objects.copy()
    
    def getObjectCount(self) -> int:
        """Get the number of objects in the world."""
        return len(self._objects)
    
    def __str__(self):
        return f"SimpleWorld(width={self._width}, height={self._height}, objects={len(self._objects)})"

def create_world() -> SimpleWorld:
    """Create a new world instance."""
    return SimpleWorld()

def demo_world_environment():
    """Run a demo of world environment capabilities."""
    print("=== NLM World Environment Demo ===")
    
    world = create_world()
    
    print(f"World created: {world}")
    
    # Configure world
    world.configure(width=100, height=100, vision_width=20, vision_height=20)
    print("World configured")
    
    # Set agent start position
    world.setAgentStart(10, 10)
    print(f"Agent placed at ({world.getAgentX()}, {world.getAgentY()})")
    
    # Add objects
    world.addObject(WorldObject(50, 50, WorldObject.WorldObjectType.Resource, 10.0))
    world.addObject(WorldObject(25, 25, WorldObject.WorldObjectType.Marker, 0.0))
    world.addObject(WorldObject(75, 75, WorldObject.WorldObjectType.Food, 5.0))
    
    print(f"Added {world.getObjectCount()} objects to world")
    
    # Test position validation
    print("Position validation tests:")
    print(f"  - Valid position (50, 50): {world.is_valid_position(50, 50)}")
    print(f"  - Invalid position (150, 50): {world.is_valid_position(150, 50)}")
    print(f"  - Invalid position (50, -10): {world.is_valid_position(50, -10)}")
    
    # Show object details
    for i, obj in enumerate(world.getObjects()):
        print(f"  - Object {i+1}: {obj}")
    
    print("World environment demo completed!")
    return world
