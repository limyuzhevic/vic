"""
World interface for NLM simulation.

Provides high-level Python API for world simulation and environment management.
"""

import math
from typing import List, Optional, Dict, Any, Tuple
from dataclasses import dataclass

from .exceptions import NLMValidationError

@dataclass
class WorldObject:
    """World object representation.
    
    Represents an object in the NLM simulation world.
    """
    x: float
    y: float
    radius: float = 0.5
    type: str = "empty"
    value: float = 0.0
    active: bool = True
    
    def __post_init__(self):
        if self.radius <= 0:
            raise NLMValidationError("Object radius must be positive")
    
    @property
    def position(self) -> Tuple[float, float]:
        """Get object position."""
        return (self.x, self.y)
    
    @property
    def is_resource(self) -> bool:
        """Check if object is a resource."""
        return self.type == "resource"
    
    @property
    def is_hazard(self) -> bool:
        """Check if object is a hazard."""
        return self.type == "hazard"
    
    @property
    def is_wall(self) -> bool:
        """Check if object is a wall."""
        return self.type == "wall"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'x': self.x,
            'y': self.y,
            'radius': self.radius,
            'type': self.type,
            'value': self.value,
            'active': self.active
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'WorldObject':
        """Create from dictionary."""
        return cls(**data)
class World:
    """Base world interface.
    
    Abstract base class for simulation worlds.
    """
    
    def __init__(self, width: float, height: float):
        """Initialize world.
        
        Args:
            width: World width
            height: World height
        """
        if width <= 0 or height <= 0:
            raise NLMValidationError("World dimensions must be positive")
            
        self.width = width
        self.height = height
        self.simulation_time = 0.0
        self.objects: List[WorldObject] = []
    
    def reset(self):
        """Reset world to initial state."""
        self.simulation_time = 0.0
        self._reset_objects()
    
    def update(self, timestep: float) -> Dict[str, Any]:
        """Update world state.
        
        Args:
            timestep: Time step to advance
            
        Returns:
            Dictionary with update results
        """
        self.simulation_time += timestep
        return self._update_world(timestep)
    
    def add_object(self, obj: WorldObject):
        """Add object to world.
        
        Args:
            obj: Object to add
        """
        self.objects.append(obj)
        self._on_object_added(obj)
    
    def remove_object(self, x: float, y: float, radius: float = 0.0):
        """Remove object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            radius: Object radius for matching
        """
        to_remove = []
        for i, obj in enumerate(self.objects):
            if radius == 0.0:
                if abs(obj.x - x) < 0.001 and abs(obj.y - y) < 0.001:
                    to_remove.append(i)
            else:
                distance = ((obj.x - x) ** 2 + (obj.y - y) ** 2) ** 0.5
                if abs(distance - radius) < 0.001:
                    to_remove.append(i)
        
        # Remove in reverse order to preserve indices
        for i in reversed(to_remove):
            removed_obj = self.objects.pop(i)
            self._on_object_removed(removed_obj)
    
    def get_object_at(self, x: float, y: float) -> Optional[WorldObject]:
        """Get object at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Object at position or None
        """
        for obj in self.objects:
            distance = ((obj.x - x) ** 2 + (obj.y - y) ** 2) ** 0.5
            if distance <= obj.radius:
                return obj
        return None
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid (not inside walls).
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid
        """
        # Check bounds
        if x < 0 or x > self.width or y < 0 or y > self.height:
            return False
            
        # Check walls
        for obj in self.objects:
            if obj.is_wall:
                distance = ((obj.x - x) ** 2 + (obj.y - y) ** 2) ** 0.5
                if distance <= obj.radius:
                    return False
                    
        return True
    
    def get_nearest_object(self, x: float, y: float) -> Optional[WorldObject]:
        """Get nearest object to position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Nearest object or None
        """
        nearest_obj = None
        min_distance = float('inf')
        
        for obj in self.objects:
            distance = ((obj.x - x) ** 2 + (obj.y - y) ** 2) ** 0.5
            if distance < min_distance:
                min_distance = distance
                nearest_obj = obj
                
        return nearest_obj if min_distance != float('inf') else None
    
    def get_objects_by_type(self, obj_type: str) -> List[WorldObject]:
        """Get all objects of specific type.
        
        Args:
            obj_type: Object type
            
        Returns:
            List of matching objects
        """
        return [obj for obj in self.objects if obj.type == obj_type]
    
    def get_objects_in_radius(self, x: float, y: float, radius: float) -> List[WorldObject]:
        """Get all objects within radius of position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            radius: Search radius
            
        Returns:
            List of objects within radius
        """
        nearby = []
        for obj in self.objects:
            distance = ((obj.x - x) ** 2 + (obj.y - y) ** 2) ** 0.5
            if distance <= radius:
                nearby.append(obj)
        return nearby
    
    def get_stats(self) -> Dict[str, Any]:
        """Get world statistics.
        
        Returns:
            Dictionary with world statistics
        """
        return {
            "width": self.width,
            "height": self.height,
            "simulation_time": self.simulation_time,
            "object_count": len(self.objects),
            "object_types": list(set(obj.type for obj in self.objects)),
        }
    
    def _reset_objects(self):
        """Reset objects (to be implemented by subclasses)."""
        pass
    
    def _update_world(self, timestep: float) -> Dict[str, Any]:
        """Update world state (to be implemented by subclasses).
        
        Args:
            timestep: Time step
            
        Returns:
            Dictionary with update results
        """
        return {}
    
    def _on_object_added(self, obj: WorldObject):
        """Called when object is added (to be overridden)."""
        pass
    
    def _on_object_removed(self, obj: WorldObject):
        """Called when object is removed (to be overridden)."""
        pass
class SimpleWorld(World):
    """Simple 2D world for NLM simulation.
    
    Implements a basic 2D simulation world with agent and objects.
    """
    
    def __init__(self, width: float = 20.0, height: float = 20.0,
                 vision_width: int = 8, vision_height: int = 8):
        """Initialize simple world.
        
        Args:
            width: World width
            height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        super().__init__(width, height)
        
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        # Agent state
        self.agent_x = width / 2.0
        self.agent_y = height / 2.0
        self.agent_orientation = 0.0
        self.agent_velocity_x = 0.0
        self.agent_velocity_y = 0.0
        self.agent_angular_velocity = 0.0
        self.agent_energy = 1.0
        self.agent_max_energy = 100.0
        self.agent_energy_decay_rate = 0.01
        self.agent_resource_energy_gain = 10.0
        
        # Objects
        self.boundary_objects = self._create_boundary_walls()
        self.objects.extend(self.boundary_objects)
        
        # Vision
        self.vision_data = [[0.0 for _ in range(vision_height)] for _ in range(vision_width)]
        
        # Sensors
        self.touch_sensors = [0.0] * 8
        self.internal_sensors = [0.0] * 4
        self.proprioception_sensors = [0.0] * 6
    
    def configure(self, width: float, height: float,
                  vision_width: int, vision_height: int):
        """Configure world parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self.vision_data = [[0.0 for _ in range(vision_height)] for _ in range(vision_width)]
        self._on_configuration_changed()
    
    def reset(self):
        """Reset world to initial state."""
        super().reset()
        self.agent_x = self.width / 2.0
        self.agent_y = self.height / 2.0
        self.agent_orientation = 0.0
        self.agent_velocity_x = 0.0
        self.agent_velocity_y = 0.0
        self.agent_angular_velocity = 0.0
        self.agent_energy = self.agent_max_energy
        self.vision_data = [[0.0 for _ in range(self.vision_height)] for _ in range(self.vision_width)]
        self._reset_sensors()
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        if not self.is_valid_position(x, y):
            raise NLMValidationError(f"Invalid agent start position: ({x}, {y})")
            
        self.agent_x = x
        self.agent_y = y
    
    def update(self, timestep: float) -> Dict[str, Any]:
        """Update world state.
        
        Args:
            timestep: Time step to advance
            
        Returns:
            Dictionary with update results
        """
        # Update simulation time
        self.simulation_time += timestep
        
        # Decay energy
        self.agent_energy = max(0.0, self.agent_energy - 
                               self.agent_energy_decay_rate * timestep)
        
        # Generate vision
        self.generate_vision()
        
        # Check collisions
        self.check_collisions()
        
        # Interact with nearby objects
        interaction_result = self.interact()
        
        return {
            "simulation_time": self.simulation_time,
            "agent_energy": self.agent_energy,
            "interaction": interaction_result,
            "vision_generated": True,
        }
    
    def apply_motor_command(self, motor_command: int, current_time: float) -> Dict[str, Any]:
        """Apply motor command to agent.
        
        Args:
            motor_command: Motor command (0-7)
            current_time: Current simulation time
            
        Returns:
            Dictionary with action result
        """
        # Convert motor command to movement
        dx, dy = 0.0, 0.0
        angular_velocity = 0.0
        
        if motor_command == 0:  # Move forward
            angle_rad = self.agent_orientation
            dx = self.agent_velocity_x + 0.5 * timestep * math.cos(angle_rad)
            dy = self.agent_velocity_y + 0.5 * timestep * math.sin(angle_rad)
        elif motor_command == 1:  # Move backward
            angle_rad = self.agent_orientation
            dx = self.agent_velocity_x - 0.5 * timestep * math.cos(angle_rad)
            dy = self.agent_velocity_y - 0.5 * timestep * math.sin(angle_rad)
        elif motor_command == 2:  # Turn left
            angular_velocity = -0.5 * timestep
        elif motor_command == 3:  # Turn right
            angular_velocity = 0.5 * timestep
        elif motor_command == 4:  # Look left
            self.agent_orientation = (self.agent_orientation - math.pi / 8.0) % (2 * math.pi)
        elif motor_command == 5:  # Look right
            self.agent_orientation = (self.agent_orientation + math.pi / 8.0) % (2 * math.pi)
        elif motor_command == 6:  # Interact
            interaction_result = self.interact()
            return {
                "reward": interaction_result.reward,
                "success": interaction_result.success,
                "message": interaction_result.message,
            }
        # motor_command == 7: Wait (no movement)
        
        # Update agent state
        self.agent_velocity_x = dx
        self.agent_velocity_y = dy
        self.agent_angular_velocity = angular_velocity
        
        # Update position
        new_x = self.agent_x + self.agent_velocity_x * timestep
        new_y = self.agent_y + self.agent_velocity_y * timestep
        
        # Check boundaries
        if not self.is_valid_position(new_x, new_y):
            new_x = self.agent_x
            new_y = self.agent_y
            
        self.agent_x = new_x
        self.agent_y = new_y
        self.agent_orientation = (self.agent_orientation + angular_velocity) % (2 * math.pi)
        
        # Update sensors
        self._update_sensors()
        
        return {
            "reward": 0.0,
            "success": True,
            "message": "",
            "position": (self.agent_x, self.agent_y),
            "orientation": self.agent_orientation,
        }
    
    def get_sensory_percept(self) -> Dict[str, Any]:
        """Get sensory percept for the agent.
        
        Returns:
            Dictionary with sensory data
        """
        return {
            "vision": self.vision_data,
            "touch": self.touch_sensors,
            "internal": self.internal_sensors,
            "proprioception": self.proprioception_sensors,
        }
    
    def get_agent_body(self) -> Dict[str, Any]:
        """Get agent body state.
        
        Returns:
            Dictionary with agent state
        """
        return {
            "x": self.agent_x,
            "y": self.agent_y,
            "orientation": self.agent_orientation,
            "velocity_x": self.agent_velocity_x,
            "velocity_y": self.agent_velocity_y,
            "angular_velocity": self.agent_angular_velocity,
            "energy": self.agent_energy,
            "max_energy": self.agent_max_energy,
            "is_moving": self.agent_velocity_x != 0 or self.agent_velocity_y != 0 or self.agent_angular_velocity != 0,
        }
    
    def add_object(self, obj: WorldObject):
        """Add object to world.
        
        Args:
            obj: Object to add
        """
        if not self.is_valid_position(obj.x, obj.y):
            raise NLMValidationError(f"Cannot add object at invalid position: ({obj.x}, {obj.y})")
            
        super().add_object(obj)
    
    def get_width(self) -> float:
        """Get world width."""
        return self.width
    
    def get_height(self) -> float:
        """Get world height."""
        return self.height
    
    def get_max_energy(self) -> float:
        """Get maximum agent energy."""
        return self.agent_max_energy
    
    def set_max_energy(self, energy: float):
        """Set maximum agent energy."""
        self.agent_max_energy = energy
    
    def get_energy_decay_rate(self) -> float:
        """Get agent energy decay rate."""
        return self.agent_energy_decay_rate
    
    def set_energy_decay_rate(self, rate: float):
        """Set agent energy decay rate."""
        self.agent_energy_decay_rate = rate
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        return self.simulation_time
    
    def set_random_seed(self, seed: int):
        """Set random seed for reproducibility.
        
        Args:
            seed: Random seed
        """
        import random
        random.seed(seed)
    
    def _create_boundary_walls(self) -> List[WorldObject]:
        """Create boundary walls."""
        walls = []
        wall_thickness = 0.5
        
        # Create walls around perimeter
        for i in range(4):
            x = 0.0 if i == 0 or i == 3 else self.width - wall_thickness
            y = 0.0 if i == 0 or i == 1 else self.height - wall_thickness
            
            walls.append(WorldObject(
                x, y, "wall", radius=wall_thickness,
                width=wall_thickness if i % 2 == 0 else self.width,
                height=wall_thickness if i % 2 == 1 else self.height
            ))
            
        return walls
    
    def generate_vision(self):
        """Generate vision data based on agent position and orientation."""
        # Clear vision
        self.vision_data = [[0.0 for _ in range(self.vision_height)] for _ in range(self.vision_width)]
        
        # Get vision cone (simplified)
        vision_angle = math.pi / 4.0  # 45 degrees field of view
        
        for i in range(self.vision_width):
            for j in range(self.vision_height):
                # Calculate vision ray
                angle_offset = (i / (self.vision_width - 1) - 0.5) * vision_angle
                ray_angle = self.agent_orientation + angle_offset
                
                ray_distance = 10.0  # Max vision distance
                
                # Cast ray
                for distance in range(1, int(ray_distance)):
                    x = self.agent_x + distance * math.cos(ray_angle)
                    y = self.agent_y + distance * math.sin(ray_angle)
                    
                    if not (0 <= x <= self.width and 0 <= y <= self.height):
                        break
                        
                    # Check for objects
                    obj = self.get_object_at(x, y)
                    if obj:
                        # Mark vision with object type
                        if obj.is_hazard:
                            self.vision_data[i][j] = 0.1
                        elif obj.is_resource:
                            self.vision_data[i][j] = 0.5
                        elif obj.is_wall:
                            self.vision_data[i][j] = 0.3
                        else:
                            self.vision_data[i][j] = 0.2
                        break
                    else:
                        # Empty space
                        self.vision_data[i][j] = 0.0
    
    def check_collisions(self):
        """Check for agent collisions with objects."""
        for obj in self.objects:
            if obj.type == "wall":
                distance = ((obj.x - self.agent_x) ** 2 + (obj.y - self.agent_y) ** 2) ** 0.5
                if distance < obj.radius:
                    # Collide with wall - bounce back
                    overlap = obj.radius - distance
                    if obj.x > self.agent_x:
                        self.agent_x += overlap
                    else:
                        self.agent_x -= overlap
                    if obj.y > self.agent_y:
                        self.agent_y += overlap
                    else:
                        self.agent_y -= overlap
                    
                    # Reduce velocity
                    self.agent_velocity_x *= 0.5
                    self.agent_velocity_y *= 0.5
    
    def interact(self) -> Dict[str, Any]:
        """Interact with nearby objects.
        
        Returns:
            Dictionary with interaction result
        """
        # Find nearest object
        nearest_obj = self.get_nearest_object(self.agent_x, self.agent_y)
        
        if not nearest_obj:
            return {"reward": 0.0, "success": False, "message": "No object to interact with"}
        
        # Process interaction based on object type
        if nearest_obj.is_resource:
            # Collect resource
            self.agent_energy = min(self.agent_max_energy, 
                                   self.agent_energy + nearest_obj.value * self.agent_resource_energy_gain)
            nearest_obj.active = False
            return {"reward": nearest_obj.value, "success": True, "message": "Collected resource"}
            
        elif nearest_obj.is_hazard:
            # Take damage
            damage = nearest_obj.value
            self.agent_energy = max(0.0, self.agent_energy - damage)
            return {"reward": -damage, "success": True, "message": "Took damage from hazard"}
            
        elif nearest_obj.is_wall:
            # Try to open door or something
            return {"reward": 0.0, "success": False, "message": "Cannot interact with wall"}
            
        else:
            return {"reward": 0.0, "success": False, "message": "No action available"}
    
    def _update_sensors(self):
        """Update internal sensors."""
        # Touch sensors (simplified)
        self.touch_sensors = [0.0] * 8
        
        # Internal sensors (homeostatic)
        self.internal_sensors = [
            0.5 + 0.1 * math.sin(self.simulation_time / 10.0),  # Energy need
            0.3 + 0.1 * math.cos(self.simulation_time / 5.0),   # Fatigue
            0.7 + 0.05 * math.sin(self.simulation_time / 7.0),  # Curiosity
            0.4 + 0.05 * math.cos(self.simulation_time / 3.0),  # Hunger
        ]
        
        # Proprioception sensors (body position/velocity)
        self.proprioception_sensors = [
            self.agent_x / self.width,  # Normalized x position
            self.agent_y / self.height,  # Normalized y position
            self.agent_velocity_x / 5.0,  # Normalized velocity x
            self.agent_velocity_y / 5.0,  # Normalized velocity y
            math.cos(self.agent_orientation),  # Orientation x
            math.sin(self.agent_orientation),  # Orientation y
        ]
    
    def _reset_sensors(self):
        """Reset sensors."""
        self.touch_sensors = [0.0] * 8
        self.internal_sensors = [0.5, 0.3, 0.7, 0.4]
        self.proprioception_sensors = [0.0] * 6
    
    def _on_configuration_changed(self):
        """Called when configuration changes."""
        self.vision_data = [[0.0 for _ in range(self.vision_height)] for _ in range(self.vision_width)]
        self._reset_sensors()