"""World implementation with Pythonic interface"""

from typing import List, Dict, Any, Optional, Tuple
from dataclasses import dataclass
from enum import Enum
import numpy as np
import time

class WorldObjectType(Enum):
    """World object types."""
    
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
    AGENT_START = "agent_start"
@dataclass
class WorldObject:
    """World object with Pythonic interface."""
    
    x: float = 0.0
    y: float = 0.0
    obj_type: WorldObjectType = WorldObjectType.EMPTY
    value: float = 0.0
    radius: float = 0.5
    active: bool = True
    object_id: str = ""
    
    def position(self) -> Tuple[float, float]:
        """Get object position."""
        return (self.x, self.y)
    
    def set_position(self, x: float, y: float):
        """Set object position."""
        self.x = x
        self.y = y
    
    def distance_to(self, other: 'WorldObject') -> float:
        """Calculate distance to another object."""
        dx = self.x - other.x
        dy = self.y - other.y
        return np.sqrt(dx * dx + dy * dy)
    
    def is_near(self, x: float, y: float, threshold: float = 1.0) -> bool:
        """Check if object is near given position."""
        distance = np.sqrt((self.x - x) ** 2 + (self.y - y) ** 2)
        return distance <= threshold
    
    def contains_point(self, x: float, y: float) -> bool:
        """Check if point is inside object (for circular objects)."""
        distance = np.sqrt((self.x - x) ** 2 + (self.y - y) ** 2)
        return distance <= self.radius
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert world object to dictionary."""
        return {
            'x': self.x,
            'y': self.y,
            'type': self.obj_type.value,
            'value': self.value,
            'radius': self.radius,
            'active': self.active,
            'object_id': self.object_id
        }
    
    def __repr__(self):
        return f"<WorldObject({self.obj_type.value} at ({self.x:.1f}, {self.y:.1f}), value={self.value:.2f})>"


class SimpleWorld:
    """Simple 2D world with Pythonic interface."""
    
    def __init__(self):
        self._width = 20.0
        self._height = 20.0
        self._vision_width = 8
        self._vision_height = 8
        self._vision_channels = 3
        
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_orientation = 0.0
        
        self._objects: List[WorldObject] = []
        self._world_objects: Dict[str, WorldObject] = {}  # ID to object mapping
        
        self._simulation_time = 0.0
        self._max_energy = 100.0
        self._energy_decay_rate = 0.1
        
        self._sensory_percept = None
        self._agent_body = None
        
        self._random_seed = None
        self._rng = np.random.RandomState()
    
    def configure(self, width: float = 20.0, height: float = 20.0,
                  vision_width: int = 8, vision_height: int = 8,
                  vision_channels: int = 3):
        """Configure world dimensions and sensory parameters."""
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        self._vision_channels = vision_channels
        
        # Regenerate world with new dimensions
        self._regenerate_world()
    
    def _regenerate_world(self):
        """Regenerate world with current dimensions."""
        # Keep existing objects if possible, otherwise recreate empty world
        existing_objects = self._objects.copy()
        self._objects.clear()
        self._world_objects.clear()
        
        # Re-add objects (they will be re-processed below)
        for obj in existing_objects:
            self._add_object_internal(obj)
    
    def reset(self):
        """Reset world to initial state."""
        self._simulation_time = 0.0
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_orientation = 0.0
        
        # Clear objects except static ones
        objects_to_keep = []
        for obj in self._objects:
            if obj.obj_type in [WorldObjectType.WALL, WorldObjectType.RESOURCE, WorldObjectType.HAZARD]:
                objects_to_keep.append(obj)
        
        self._objects = objects_to_keep
        self._world_objects.clear()
        for obj in self._objects:
            self._world_objects[obj.object_id] = obj
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position."""
        self._agent_x = max(0.0, min(x, self._width - 1.0))
        self._agent_y = max(0.0, min(y, self._height - 1.0))
    
    def update(self, timestep: float = 0.1):
        """Update world simulation."""
        self._simulation_time += timestep
        
        # Update agent body
        if self._agent_body:
            self._agent_body.x = self._agent_x
            self._agent_body.y = self._agent_y
            self._agent_body.orientation = self._agent_orientation
        
        # Update world state
        self._update_world_objects(timestep)
        
        # Generate sensory percept
        self._generate_sensory_percept()
    
    def _update_world_objects(self, timestep: float):
        """Update world objects (placeholder for dynamic behavior)."""
        # This would contain world dynamics (decay, regeneration, etc.)
        # For now, it's a placeholder
        pass
    
    def _generate_sensory_percept(self):
        """Generate sensory percept for agent."""
        # Create vision data (simplified)
        vision_width = self._vision_width
        vision_height = self._vision_height
        vision_channels = self._vision_channels
        
        vision_data = []
        
        for y in range(vision_height):
            for x in range(vision_width):
                # Map vision coordinates to world coordinates
                world_x = self._agent_x + (x - vision_width // 2) * 2.0
                world_y = self._agent_y + (y - vision_height // 2) * 2.0
                
                # Check if within bounds
                if 0 <= world_x < self._width and 0 <= world_y < self._height:
                    # Sample world at this position
                    vision_value = self._sample_world(world_x, world_y)
                    vision_data.append(vision_value)
                else:
                    vision_data.append(0.0)  # Outside bounds
        
        # Create vision input (simplified - using our sensory module if available)
        try:
            from .sensory import Vision
            vision_input = Vision(vision_width, vision_height, vision_channels)
            vision_input.set_data(vision_data)
            self._sensory_percept = vision_input
        except ImportError:
            # Fallback: create simple list-based percept
            class SimplePercept:
                def getVision(self):
                    return vision_data
                def getWidth(self):
                    return vision_width
                def getHeight(self):
                    return vision_height
            
            self._sensory_percept = SimplePercept()
    
    def _sample_world(self, x: float, y: float) -> float:
        """Sample world value at position (simplified)."""
        # Check for collisions with objects
        for obj in self._objects:
            distance = np.sqrt((obj.x - x) ** 2 + (obj.y - y) ** 2)
            if distance <= obj.radius:
                # Object present
                if obj.obj_type == WorldObjectType.RESOURCE:
                    return 1.0
                elif obj.obj_type == WorldObjectType.HAZARD:
                    return -1.0
                elif obj.obj_type == WorldObjectType.WALL:
                    return -0.5
                elif obj.obj_type == WorldObjectType.MARKER:
                    return 0.5
        
        return 0.0  # Empty space
    
    def applyMotorCommand(self, motor_cmd, current_time: float):
        """Apply motor command from agent."""
        # Extract movement from motor command
        move_speed = 0.2
        turn_speed = 0.1
        
        # This would need to interface with actual motor command parsing
        # For now, implement simple movement
        
        # Simple movement based on current time and agent's position
        angle = current_time * 0.5  # Oscillating movement
        self._agent_x += move_speed * np.cos(angle)
        self._agent_y += move_speed * np.sin(angle)
        
        # Update orientation
        self._agent_orientation += angle * 0.1
        
        # Clamp to bounds
        self._agent_x = max(0.0, min(self._agent_x, self._width - 1.0))
        self._agent_y = max(0.0, min(self._agent_y, self._height - 1.0))
        
        # Update agent body
        if self._agent_body:
            self._agent_body.x = self._agent_x
            self._agent_body.y = self._agent_y
            self._agent_body.orientation = self._agent_orientation
            self._agent_body.isMoving = True
            self._agent_body.isTurning = True
            self._agent_body.velocityX = move_speed * np.cos(angle)
            self._agent_body.velocityY = move_speed * np.sin(angle)
    
    def getSensoryPercept(self):
        """Get current sensory percept."""
        return self._sensory_percept
    
    def getAgentBody(self):
        """Get current agent body state."""
        return self._agent_body
    
    def addObject(self, obj: WorldObject):
        """Add world object."""
        self._add_object_internal(obj)
    
    def _add_object_internal(self, obj: WorldObject):
        """Internal method to add object."""
        if not obj.object_id:
            obj.object_id = f"obj_{len(self._objects)}_{time.time()}"
        
        self._objects.append(obj)
        self._world_objects[obj.object_id] = obj
    
    def removeObject(self, x: float, y: float, tolerance: float = 0.5):
        """Remove object at position."""
        objects_to_remove = []
        
        for obj in self._objects:
            distance = np.sqrt((obj.x - x) ** 2 + (obj.y - y) ** 2)
            if distance <= tolerance:
                objects_to_remove.append(obj.object_id)
        
        for obj_id in objects_to_remove:
            if obj_id in self._world_objects:
                del self._world_objects[obj_id]
            
            self._objects = [obj for obj in self._objects if obj.object_id != obj_id]
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid in world."""
        # Check bounds
        if not (0 <= x < self._width and 0 <= y < self._height):
            return False
        
        # Check for collisions with obstacles
        for obj in self._objects:
            if obj.obj_type in [WorldObjectType.WALL, WorldObjectType.HAZARD]:
                distance = np.sqrt((obj.x - x) ** 2 + (obj.y - y) ** 2)
                if distance <= obj.radius:
                    return False
        
        return True
    
    def setRandomSeed(self, seed: int):
        """Set random seed for reproducibility."""
        self._random_seed = seed
        self._rng = np.random.RandomState(seed)
    
    def getRandomSeed(self) -> int:
        """Get random seed."""
        return self._random_seed
    
    # Properties with Pythonic interface
    @property
    def width(self) -> float:
        """Get world width."""
        return self._width
    
    @property
    def height(self) -> float:
        """Get world height."""
        return self._height
    
    @property
    def vision_width(self) -> int:
        """Get vision width."""
        return self._vision_width
    
    @property
    def vision_height(self) -> int:
        """Get vision height."""
        return self._vision_height
    
    @property
    def vision_channels(self) -> int:
        """Get vision channels."""
        return self._vision_channels
    
    @property
    def simulation_time(self) -> float:
        """Get simulation time."""
        return self._simulation_time
    
    @property
    def agent_position(self) -> Tuple[float, float]:
        """Get agent position."""
        return (self._agent_x, self._agent_y)
    
    @agent_position.setter
    def agent_position(self, value: Tuple[float, float]):
        """Set agent position."""
        self._agent_x, self._agent_y = value
    
    @property
    def agent_orientation(self) -> float:
        """Get agent orientation."""
        return self._agent_orientation
    
    @agent_orientation.setter
    def agent_orientation(self, value: float):
        """Set agent orientation."""
        self._agent_orientation = value
    
    @property
    def max_energy(self) -> float:
        """Get maximum energy."""
        return self._max_energy
    
    @max_energy.setter
    def max_energy(self, value: float):
        """Set maximum energy."""
        self._max_energy = max(0.0, value)
    
    @property
    def energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._energy_decay_rate
    
    @energy_decay_rate.setter
    def energy_decay_rate(self, value: float):
        """Set energy decay rate."""
        self._energy_decay_rate = max(0.0, min(1.0, value))
    
    def get_objects(self, obj_type: Optional[WorldObjectType] = None) -> List[WorldObject]:
        """Get objects, optionally filtered by type."""
        if obj_type is None:
            return self._objects.copy()
        
        return [obj for obj in self._objects if obj.obj_type == obj_type]
    
    def get_object_count(self, obj_type: Optional[WorldObjectType] = None) -> int:
        """Get number of objects."""
        return len(self.get_objects(obj_type))
    
    def find_nearest_object(self, x: float, y: float, 
                           obj_type: Optional[WorldObjectType] = None) -> Optional[WorldObject]:
        """Find nearest object to given position."""
        objects = self.get_objects(obj_type)
        if not objects:
            return None
        
        nearest = min(objects, key=lambda obj: np.sqrt((obj.x - x) ** 2 + (obj.y - y) ** 2))
        return nearest
    
    def generate_resource(self, x: float, y: float, value: float = 1.0):
        """Generate resource at position."""
        if self.isValidPosition(x, y):
            obj = WorldObject(x, y, WorldObjectType.RESOURCE, value, radius=0.5)
            self.addObject(obj)
            return obj
        return None
    
    def generate_hazard(self, x: float, y: float, hazard_level: float = 0.5):
        """Generate hazard at position."""
        if self.isValidPosition(x, y):
            obj = WorldObject(x, y, WorldObjectType.HAZARD, hazard_level, radius=0.8)
            self.addObject(obj)
            return obj
        return None
    
    def generate_wall(self, x: float, y: float, length: float = 1.0):
        """Generate wall segment."""
        # Simple wall generation (placeholder for more complex wall systems)
        return None
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get world statistics."""
        return {
            'width': self._width,
            'height': self._height,
            'simulation_time': self._simulation_time,
            'agent_position': self.agent_position,
            'agent_orientation': self.agent_orientation,
            'object_count': len(self._objects),
            'object_types': {
                obj_type.value: self.get_object_count(obj_type)
                for obj_type in WorldObjectType
            }
        }
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert world to dictionary."""
        return {
            'width': self._width,
            'height': self._height,
            'vision_width': self._vision_width,
            'vision_height': self._vision_height,
            'vision_channels': self._vision_channels,
            'simulation_time': self._simulation_time,
            'agent_position': self.agent_position,
            'agent_orientation': self._agent_orientation,
            'objects': [obj.to_dict() for obj in self._objects],
            'statistics': self.get_statistics()
        }
    
    def __repr__(self):
        return f"<SimpleWorld({self._width}x{self._height}, {len(self._objects)} objects, t={self._simulation_time:.1f})>"


# Factory functions

def create_simple_world() -> SimpleWorld:
    """Create simple world."""
    return SimpleWorld()


def create_world_object(x: float, y: float, obj_type: WorldObjectType,
                       value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """Create world object."""
    return WorldObject(x, y, obj_type, value, radius)


# Export for backward compatibility
WorldObjectType = WorldObjectType
WorldObject = WorldObject
SimpleWorld = SimpleWorld