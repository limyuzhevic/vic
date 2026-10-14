# World environment wrappers

from typing import List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum

from .exceptions import NLMErrors
from .brain import BrainSession

class WorldObjectType(Enum):
    """World object types."""
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4

@dataclass
class WorldObject:
    """World object representation."""
    x: float
    y: float
    radius: float
    type: WorldObjectType
    value: float = 0.0
    active: bool = True
    
    def __post_init__(self):
        if self.radius <= 0:
            raise ValueError("Radius must be positive")

@dataclass
class ActionResult:
    """Result of an action taken in the world."""
    reward: float
    success: bool
    message: str = ""

@dataclass
class AgentBody:
    """Agent body state."""
    x: float
    y: float
    orientation: float
    velocity_x: float
    velocity_y: float
    angular_velocity: float
    energy: float
    health: float
    age: float
    is_moving: bool
    is_turning: bool
    last_action_time: float
    
    def __post_init__(self):
        self.is_moving = (self.velocity_x != 0 or self.velocity_y != 0)
        self.is_turning = (abs(self.angular_velocity) > 0.001)

class Vision:
    """Vision/sensory perception wrapper."""
    
    def __init__(self, width: int = 8, height: int = 8, channels: int = 3):
        self.width = width
        self.height = height
        self.channels = channels
        self.data = [[0.0 for _ in range(channels)] for _ in range(height)]
        
    def set_data(self, data: List[List[float]]):
        """Set vision data."""
        if len(data) != self.height:
            raise ValueError(f"Expected {self.height} rows, got {len(data)}")
        for row in data:
            if len(row) != self.channels:
                raise ValueError(f"Expected {self.channels} channels, got {len(row)}")
        self.data = data
        
    def get_data(self) -> List[List[float]]:
        """Get vision data."""
        return self.data
        
    def get_flat_data(self) -> List[float]:
        """Get flattened vision data."""
        return [val for row in self.data for val in row]
        
    def clear(self):
        """Clear vision data."""
        self.data = [[0.0 for _ in range(self.channels)] for _ in range(self.height)]

class SensoryPercept:
    """Combined sensory perception data."""
    
    def __init__(self):
        self.vision = Vision()
        self.touch = Vision()
        self.internal = Vision()
        self.proprioception = Vision()
        self.audio = Vision()
        self.timestamp = 0.0
        
    def set_vision(self, vision: Vision):
        """Set vision perception."""
        self.vision = vision
        
    def set_touch(self, touch: Vision):
        """Set touch perception."""
        self.touch = touch
        
    def set_internal(self, internal: Vision):
        """Set internal perception."""
        self.internal = internal
        
    def set_proprioception(self, proprioception: Vision):
        """Set proprioception."""
        self.proprioception = proprioception
        
    def set_audio(self, audio: Vision):
        """Set audio perception."""
        self.audio = audio
        
    def set_timestamp(self, timestamp: float):
        """Set timestamp."""
        self.timestamp = timestamp
        
    def get_all_signals(self) -> Dict[str, Vision]:
        """Get all sensory signals."""
        return {
            'vision': self.vision,
            'touch': self.touch,
            'internal': self.internal,
            'proprioception': self.proprioception,
            'audio': self.audio,
        }
class SimpleWorld:
    """Enhanced simple world simulation with Pythonic API."""
    
    def __init__(self):
        self._world = None
        self._width = 16.0
        self._height = 16.0
        self._vision_width = 8
        self._vision_height = 8
        
    def configure(self, width: float, height: float, vision_width: int = 8,
                  vision_height: int = 8):
        """Configure world parameters."""
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        
    def get_width(self) -> float:
        """Get world width."""
        return self._width
        
    def get_height(self) -> float:
        """Get world height."""
        return self._height
        
    def get_vision_width(self) -> int:
        """Get vision width."""
        return self._vision_width
        
    def get_vision_height(self) -> int:
        """Get vision height."""
        return self._vision_height
        
    def get_agent_body(self) -> AgentBody:
        """Get agent body state."""
        # This would call the C++ world implementation
        # For now, return a default state
        return AgentBody(
            x=8.0, y=8.0, orientation=0.0, velocity_x=0.0, velocity_y=0.0,
            angular_velocity=0.0, energy=100.0, health=100.0, age=0.0,
            is_moving=False, is_turning=False, last_action_time=0.0
        )
        
    def get_sensory_percept(self) -> SensoryPercept:
        """Get current sensory perception."""
        # This would call the C++ world implementation
        # For now, return a default percept
        percept = SensoryPercept()
        # Generate some dummy vision data
        for y in range(self._vision_height):
            for x in range(self._vision_width):
                for c in range(3):
                    percept.vision.data[y][c] = (x + y) % 255 / 255.0
        return percept
        
    def add_object(self, obj: WorldObject):
        """Add object to world."""
        if not (0 <= obj.x <= self._width and 0 <= obj.y <= self._height):
            raise NLMErrors(f"Object position ({obj.x}, {obj.y}) outside world bounds")
            
        if obj.type in (WorldObjectType.WALL, WorldObjectType.HAZARD):
            if obj.radius > 0.5:
                raise NLMErrors(f"Hazard/wall radius should not exceed 0.5, got {obj.radius}")
                
    def remove_object(self, x: float, y: float):
        """Remove object at position."""
        # This would call the C++ world implementation
        pass
        
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid (not wall)."""
        # This would call the C++ world implementation
        return (0 <= x <= self._width and 0 <= y <= self._height)
        
    def get_object_at(self, x: float, y: float) -> Optional[WorldObject]:
        """Get object at position."""
        # This would call the C++ world implementation
        return None
        
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position."""
        if not self.is_valid_position(x, y):
            raise NLMErrors(f"Invalid start position ({x}, {y})")
            
    def reset(self):
        """Reset world to initial state."""
        # This would call the C++ world implementation
        pass
        
    def update(self, timestep: float):
        """Update world state."""
        # This would call the C++ world implementation
        pass
        
    def apply_motor_command(self, cmd, current_time: float) -> ActionResult:
        """Apply motor command to agent."""
        # This would call the C++ world implementation
        # For now, return a default successful action
        return ActionResult(reward=0.0, success=True, message="Action completed")
        
    def compute_reward(self, brain) -> float:
        """Compute reward based on brain activity."""
        # This would call the C++ world implementation
        # For now, return a default reward
        return 0.0
        
    def set_random_seed(self, seed: int):
        """Set random seed for reproducibility."""
        # This would call the C++ world implementation
        pass
        
    def get_random_seed(self) -> int:
        """Get current random seed."""
        # This would call the C++ world implementation
        return 42

# Convenience functions

def create_simple_world():
    """Create a simple world instance."""
    return SimpleWorld()
        

def run_world_simulation(config=None, duration: float = 10.0,
                         timestep: float = 0.1) -> SimpleWorld:
    """Run a world simulation with the given configuration."""
    world = create_simple_world()
    
    for i in range(int(duration / timestep)):
        world.update(timestep)
        time.sleep(timestep)
        
    return world