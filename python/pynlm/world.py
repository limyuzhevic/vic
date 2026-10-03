"""
World management for NLM Python bindings.

This module provides enhanced world functionality for NLM,
including environment management, object handling, and
world simulation utilities.
"""

import random
import time
from typing import Optional, Dict, Any, List, Tuple
from enum import Enum

from .exceptions import (
    SimulationError, ValidationError, WorldError
)
from .types import (
    NeuronType, WorldObjectType, ActionType, MotorCommand
)

# Import C++ types
try:
    from ..bindings import (
        createSimpleWorld, SimpleWorld, WorldObject, AgentBody,
        ActionResult, ActionType as CPPActionType
    )
except ImportError as e:
    raise ImportError(f"Failed to import NLM world bindings: {e}")


class WorldObject3D:
    """
    3D world object with enhanced functionality.
    
    Provides Pythonic interface for world objects with
    physics simulation and interaction capabilities.
    """
    
    def __init__(self, x: float, y: float, obj_type: WorldObjectType,
                 value: float = 0.0, radius: float = 0.5):
        """
        Initialize world object.
        
        Args:
            x: X position
            y: Y position
            obj_type: Object type
            value: Object value (e.g., energy, health)
            radius: Object radius
            
        Raises:
            ValidationError: If parameters are invalid
        """
        if radius <= 0:
            raise ValidationError(f"Radius must be positive, got {radius}")
        
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
        self.id = f"obj_{int(time.time() * 1000)}_{random.randint(0, 10000)}"
    
    def distance_to(self, other: 'WorldObject3D') -> float:
        """
        Calculate distance to another object.
        
        Args:
            other: Other world object
            
        Returns:
            Distance between objects
        """
        return ((self.x - other.x) ** 2 + (self.y - other.y) ** 2) ** 0.5
    
    def is_colliding(self, other: 'WorldObject3D') -> bool:
        """
        Check if colliding with another object.
        
        Args:
            other: Other world object
            
        Returns:
            True if objects are colliding
        """
        return (self.distance_to(other) <= (self.radius + other.radius) and 
                self.active and other.active)
    
    def move(self, dx: float, dy: float) -> None:
        """
        Move object by relative amounts.
        
        Args:
            dx: Change in X
            dy: Change in Y
        """
        self.x += dx
        self.y += dy
    
    def set_position(self, x: float, y: float) -> None:
        """Set object position."""
        self.x = x
        self.y = y
    
    def get_position(self) -> Tuple[float, float]:
        """Get object position."""
        return (self.x, self.y)
    
    def update(self, dt: float, world_width: float, world_height: float) -> None:
        """
        Update object state (physics).
n        
        Args:
            dt: Time step
            world_width: World width for boundary checking
            world_height: World height for boundary checking
        """
        # Basic physics: update position based on velocity if implemented
        # Boundary conditions
        self.x = max(self.radius, min(world_width - self.radius, self.x))
        self.y = max(self.radius, min(world_height - self.radius, self.y))
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert object to dictionary."""
        return {
            'id': self.id,
            'x': self.x,
            'y': self.y,
            'type': self.type.value,
            'value': self.value,
            'radius': self.radius,
            'active': self.active,
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'WorldObject3D':
        """
        Create object from dictionary.
        
        Args:
            data: Object data dictionary
            
        Returns:
            Created object
        """
        obj = cls(
            data['x'],
            data['y'],
            WorldObjectType(data['type']),
            data['value'],
            data['radius']
        )
        obj.id = data['id']
        obj.active = data['active']
        return obj
    
    def __repr__(self) -> str:
        return f"WorldObject3D({self.type.value} at ({self.x:.1f}, {self.y:.1f}))"


class NMLSimpleWorld:
    """
    Enhanced NLM simple world with Pythonic interface.
    
    Provides world simulation with object management, physics,
    and agent interaction capabilities.
    """
    
    def __init__(self):
        """Initialize NLM world."""
        self._world = createSimpleWorld()
        self._is_configured = False
        self._objects: Dict[str, WorldObject3D] = {}
        self._agent_position = (0.0, 0.0)
        self._simulation_time = 0.0
    
    def configure(self, width: int, height: int,
                 vision_width: int = 8, vision_height: int = 8) -> None:
        """
        Configure world dimensions.
n        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            
        Raises:
            ValidationError: If parameters are invalid
        """
        if width <= 0 or height <= 0:
            raise ValidationError(f"World dimensions must be positive: {width}x{height}")
        
        if vision_width <= 0 or vision_height <= 0:
            raise ValidationError(f"Vision dimensions must be positive: {vision_width}x{vision_height}")
        
        self._world.configure(width, height, vision_width, vision_height)
        self._is_configured = True
    
    def is_configured(self) -> bool:
        """Check if world is configured."""
        return self._is_configured
    
    def reset(self) -> None:
        """Reset world to initial state."""
        if self._is_configured:
            self._world.reset()
        
        # Clear objects
        self._objects.clear()
        self._agent_position = (0.0, 0.0)
        self._simulation_time = 0.0
    
    def update(self, timestep: float) -> None:
        """
        Update world state.
        
        Args:
            timestep: Time step to update
            
        Raises:
            SimulationError: If world is not configured
        """
        if not self._is_configured:
            raise SimulationError("World must be configured before updating")
        
        self._world.update(timestep)
        self._simulation_time += timestep
        
        # Update objects
        for obj in self._objects.values():
            if obj.active:
                obj.update(timestep, self._world.getWidth(), self._world.getHeight())
    
    # World information methods
    def get_width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    def get_vision_width(self) -> int:
        """Get vision width."""
        return self._world.getVisionWidth()
    
    def get_vision_height(self) -> int:
        """Get vision height."""
        return self._world.getVisionHeight()
    
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._world.getEnergyDecayRate()
    
    # Object management methods
    def add_object(self, obj: WorldObject3D) -> str:
        """
        Add object to world.
        
        Args:
            obj: Object to add
            
        Returns:
            Object ID
        """
        self._objects[obj.id] = obj
        return obj.id
    
    def add_resource(self, x: float, y: float, value: float = 1.0) -> str:
        """
        Add resource object to world.
        
        Args:
            x: X position
            y: Y position
            value: Resource value
            
        Returns:
            Object ID
        """
        obj = WorldObject3D(x, y, WorldObjectType.Resource, value, 0.3)
        return self.add_object(obj)
    
    def add_hazard(self, x: float, y: float, radius: float = 0.3) -> str:
        """
        Add hazard object to world.
        
        Args:
            x: X position
            y: Y position
            radius: Hazard radius
            
        Returns:
            Object ID
        """
        obj = WorldObject3D(x, y, WorldObjectType.Hazard, 0.0, radius)
        return self.add_object(obj)
    
    def add_wall(self, x: float, y: float, radius: float = 0.4) -> str:
        """
        Add wall object to world.
n        
        Args:
            x: X position
            y: Y position
            radius: Wall radius
            
        Returns:
            Object ID
        """
        obj = WorldObject3D(x, y, WorldObjectType.Wall, 0.0, radius)
        return self.add_object(obj)
    
    def remove_object(self, obj_id: str) -> bool:
        """
        Remove object from world.
        
        Args:
            obj_id: Object ID
            
        Returns:
            True if object was removed
        """
        if obj_id in self._objects:
            del self._objects[obj_id]
            return True
        return False
    
    def get_object(self, obj_id: str) -> Optional[WorldObject3D]:
        """Get object by ID."""
        return self._objects.get(obj_id)
    
    def get_objects(self) -> List[WorldObject3D]:
        """Get all objects."""
        return list(self._objects.values())
    
    def get_objects_by_type(self, obj_type: WorldObjectType) -> List[WorldObject3D]:
        """
        Get objects by type.
        
        Args:
            obj_type: Object type
            
        Returns:
            List of matching objects
        """
        return [obj for obj in self._objects.values() if obj.type == obj_type]
    
    def find_nearest_object(self, x: float, y: float,
                           obj_type: Optional[WorldObjectType] = None) -> Optional[WorldObject3D]:
        """
        Find nearest object to position.
        
        Args:
            x: X position
            y: Y position
            obj_type: Object type to search for (optional)
            
        Returns:
            Nearest object or None
        """
        nearest = None
        min_distance = float('inf')
        
        for obj in self._objects.values():
            if not obj.active:
                continue
            
            if obj_type is not None and obj.type != obj_type:
                continue
            
            distance = ((x - obj.x) ** 2 + (y - obj.y) ** 2) ** 0.5
            if distance < min_distance:
                min_distance = distance
                nearest = obj
        
        return nearest
    
    # Agent methods
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position."""
        self._agent_position = (x, y)
        if self._is_configured:
            self._world.setAgentStart(x, y)
    
    def get_agent_position(self) -> Tuple[float, float]:
        """Get agent position."""
        return self._agent_position
    
    def get_agent_body(self):
        """Get agent body."""
        if self._is_configured:
            return self._world.getAgentBody()
        return None
    
    # Simulation methods
    def apply_motor_command(self, action, timestep: float) -> ActionResult:
        """
        Apply motor command to world.
n        
        Args:
            action: Motor action
            timestep: Current time step
            
        Returns:
            Action result
        """
        if not self._is_configured:
            raise SimulationError("World must be configured")
        
        return self._world.applyMotorCommand(action, timestep)
    
    def get_sensory_percept(self):
        """
        Get sensory percept from world.
n        
        Returns:
            Sensory percept
        """
        if not self._is_configured:
            raise SimulationError("World must be configured")
        
        return self._world.getSensoryPercept()
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if position is valid in world.
n        
        Args:
            x: X position
            y: Y position
            
        Returns:
            True if position is valid
        """
        if not self._is_configured:
            return False
        
        return self._world.isValidPosition(x, y)
    
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._simulation_time
    
    # Object interaction methods
    def check_collisions(self) -> List[Tuple[WorldObject3D, WorldObject3D]]:
        """
        Check for object collisions.
        
        Returns:
            List of colliding object pairs
        """
        collisions = []
        obj_ids = list(self._objects.keys())
        
        for i, obj_id1 in enumerate(obj_ids):
            obj1 = self._objects[obj_id1]
            if not obj1.active:
                continue
            
            for obj_id2 in obj_ids[i + 1:]:
                obj2 = self._objects[obj_id2]
                if not obj2.active:
                    continue
                
                if obj1.is_colliding(obj2):
                    collisions.append((obj1, obj2))
        
        return collisions
    
    def resolve_collisions(self) -> Dict[str, Any]:
        """
        Resolve object collisions.
        
        Returns:
            Collision resolution statistics
        """
        collisions = self.check_collisions()
        resolution_stats = {
            'total_collisions': len(collisions),
            'objects_affected': set(),
            'collision_types': {},
        }
        
        for obj1, obj2 in collisions:
            resolution_stats['objects_affected'].add(obj1.id)
            resolution_stats['objects_affected'].add(obj2.id)
            
            collision_type = f"{obj1.type.value}_to_{obj2.type.value}"
            resolution_stats['collision_types'][collision_type] = \
                resolution_stats['collision_types'].get(collision_type, 0) + 1
        
        # Simple collision resolution: bounce objects apart
        for obj1, obj2 in collisions:
            # Calculate separation vector
            dx = obj2.x - obj1.x
            dy = obj2.y - obj1.y
            distance = ((dx ** 2) + (dy ** 2)) ** 0.5
            
            if distance > 0:
                # Normalize and apply separation
                separation_x = (dx / distance) * (obj1.radius + obj2.radius - distance) * 0.5
                separation_y = (dy / distance) * (obj1.radius + obj2.radius - distance) * 0.5
                
                obj1.x -= separation_x
                obj1.y -= separation_y
                obj2.x += separation_x
                obj2.y += separation_y
        
        return resolution_stats
    
    def update_object_states(self) -> None:
        """Update all object states (physics, AI, etc.)."""
        # Update objects based on their type and behavior
        for obj in self._objects.values():
            if not obj.active:
                continue
            
            # Type-specific behavior
            if obj.type == WorldObjectType.Resource:
                # Resources could deplete or move
                pass
            elif obj.type == WorldObjectType.Hazard:
                # Hazards could be static or moving
                pass
            elif obj.type == WorldObjectType.Wall:
                # Walls are static
                pass
    
    # Statistics methods
    def get_statistics(self) -> Dict[str, Any]:
        """Get world statistics."""
        return {
            'width': self.get_width(),
            'height': self.get_height(),
            'vision_width': self.get_vision_width(),
            'vision_height': self.get_vision_height(),
            'total_objects': len(self._objects),
            'active_objects': len([obj for obj in self._objects.values() if obj.active]),
            'resource_count': len(self.get_objects_by_type(WorldObjectType.Resource)),
            'hazard_count': len(self.get_objects_by_type(WorldObjectType.Hazard)),
            'wall_count': len(self.get_objects_by_type(WorldObjectType.Wall)),
            'simulation_time': self.get_simulation_time(),
        }
    
    def __repr__(self) -> str:
        if self._is_configured:
            return f"NMLSimpleWorld({self.get_width()}x{self.get_height()})"
        return "NMLSimpleWorld(not configured)"
# Alias for backward compatibility
SimpleWorld = NMLSimpleWorld


# Convenience constructors
def create_simple_world() -> NMLSimpleWorld:
    """Create NLM simple world."""
    return NMLSimpleWorld()
def create_world_object(x: float, y: float, obj_type: WorldObjectType,
                        value: float = 0.0, radius: float = 0.5) -> WorldObject3D:
    """Create world object."""
    return WorldObject3D(x, y, obj_type, value, radius)


# Type aliases for convenience
WorldObjectTypeType = WorldObjectType
WorldObject3DType = WorldObject3D

# Export all main classes
__all__.extend([
    'WorldObject3D', 'NMLSimpleWorld', 'create_simple_world',
    'create_world_object', 'WorldObjectTypeType', 'WorldObject3DType'
])
