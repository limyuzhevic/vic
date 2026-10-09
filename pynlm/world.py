"""
Enhanced Python interface for NLM world and environment classes
"""

from typing import List, Dict, Any, Optional, Tuple
from enum import Enum
from .exceptions import NLMError

class ActionType(Enum):
    """Action type enumeration"""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"

class WorldObjectType(Enum):
    """World object type enumeration"""
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"

class WorldObject:
    """World object with enhanced Python interface"""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, 
                 obj_type: WorldObjectType = WorldObjectType.EMPTY, 
                 value: float = 0.0, radius: float = 0.5, 
                 active: bool = True):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = active
        
    def get_position(self) -> Tuple[float, float]:
        """Get object position"""
        return (self.x, self.y)
        
    def set_position(self, x: float, y: float) -> None:
        """Set object position"""
        self.x = x
        self.y = y
        
    def get_type(self) -> WorldObjectType:
        """Get object type"""
        return self.type
        
    def set_type(self, obj_type: WorldObjectType) -> None:
        """Set object type"""
        self.type = obj_type
        
    def get_value(self) -> float:
        """Get object value"""
        return self.value
        
    def set_value(self, value: float) -> None:
        """Set object value"""
        self.value = value
        
    def get_radius(self) -> float:
        """Get object radius"""
        return self.radius
        
    def set_radius(self, radius: float) -> None:
        """Set object radius"""
        self.radius = radius
        
    def is_active(self) -> bool:
        """Check if object is active"""
        return self.active
        
    def set_active(self, active: bool) -> None:
        """Set object active state"""
        self.active = active
        
    def distance_to(self, other: 'WorldObject') -> float:
        """Calculate distance to another object"""
        dx = self.x - other.x
        dy = self.y - other.y
        return (dx * dx + dy * dy) ** 0.5
        
    def is_colliding(self, other: 'WorldObject') -> bool:
        """Check if colliding with another object"""
        distance = self.distance_to(other)
        return distance < (self.radius + other.radius)
        
    def __repr__(self):
        return f"<WorldObject: {self.type.value} at ({self.x:.1f}, {self.y:.1f}), radius={self.radius:.1f}>"

class ActionResult:
    """Result from applying a motor command in the world"""
    
    def __init__(self, reward: float = 0.0, success: bool = False, 
                 message: str = ""):
        self.reward = reward
        self.success = success
        self.message = message
        
    def get_reward(self) -> float:
        """Get reward value"""
        return self.reward
        
    def set_reward(self, reward: float) -> None:
        """Set reward value"""
        self.reward = reward
        
    def was_successful(self) -> bool:
        """Check if action was successful"""
        return self.success
        
    def set_successful(self, success: bool) -> None:
        """Set success status"""
        self.success = success
        
    def get_message(self) -> str:
        """Get result message"""
        return self.message
        
    def set_message(self, message: str) -> None:
        """Set result message"""
        self.message = message
        
    def __repr__(self):
        return f"<ActionResult: reward={self.reward:.2f}, success={self.success}, message='{self.message}'>"

class SimpleWorld:
    """Enhanced Python interface for SimpleWorld"""
    
    def __init__(self):
        self.width = 0
        self.height = 0
        self.vision_width = 0
        self.vision_height = 0
        self.max_energy = 100.0
        self.energy_decay_rate = 0.01
        self.simulation_time = 0.0
        self.random_seed = 42
        
        # Agent position and state
        self.agent_x = 0.0
        self.agent_y = 0.0
        self.agent_orientation = 0.0
        
        # World objects
        self.objects = []
        
        # Sensory percept
        self.current_percept = None
        
        # Body state
        self.body = None
        
    def configure(self, width: int, height: int, 
                  vision_width: int, vision_height: int) -> None:
        """Configure world dimensions"""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
    def reset(self) -> None:
        """Reset world to initial state"""
        self.simulation_time = 0.0
        self.agent_x = self.width // 2
        self.agent_y = self.height // 2
        self.agent_orientation = 0.0
        self.objects.clear()
        self.body = None
        
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position"""
        self.agent_x = x
        self.agent_y = y
        
    def update(self, timestep: float) -> None:
        """Update world simulation by timestep"""
        self.simulation_time += timestep
        
    def apply_motor_command(self, action_type: ActionType, 
                           current_time: float) -> ActionResult:
        """Apply motor command to world"""
        
    def get_sensory_percept(self) -> 'SensoryPercept':
        """Get sensory percept from world"""
        
    def get_agent_body(self) -> 'AgentBody':
        """Get agent body state"""
        return self.body
        
    def add_object(self, obj: WorldObject) -> None:
        """Add object to world"""
        self.objects.append(obj)
        
    def remove_object(self, x: float, y: float) -> bool:
        """Remove object at position"""
        for i, obj in enumerate(self.objects):
            if abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1:
                del self.objects[i]
                return True
        return False
        
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid (not inside walls/objects)"""
        
    def get_width(self) -> int:
        """Get world width"""
        return self.width
        
    def get_height(self) -> int:
        """Get world height"""
        return self.height
        
    def get_max_energy(self) -> float:
        """Get maximum energy"""
        return self.max_energy
        
    def set_max_energy(self, energy: float) -> None:
        """Set maximum energy"""
        self.max_energy = energy
        
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate"""
        return self.energy_decay_rate
        
    def set_energy_decay_rate(self, rate: float) -> None:
        """Set energy decay rate"""
        self.energy_decay_rate = rate
        
    def get_simulation_time(self) -> float:
        """Get current simulation time"""
        return self.simulation_time
        
    def set_random_seed(self, seed: int) -> None:
        """Set random seed"""
        self.random_seed = seed
        
    def get_random_seed(self) -> int:
        """Get random seed"""
        return self.random_seed
        
    def get_object_count(self) -> int:
        """Get number of objects in world"""
        return len(self.objects)
        
    def __repr__(self):
        return f"<SimpleWorld: {self.width}x{self.height}, vision={self.vision_width}x{self.vision_height}, objects={len(self.objects)}>"
