# NLM World Python Module
# Python bindings for NLM world simulation

from typing import Optional, List, Dict, Any
from enum import Enum

class WorldObjectType(Enum):
    Empty = "Empty"
    Resource = "Resource"
    Hazard = "Hazard"
    Wall = "Wall"
    Marker = "Marker"

class WorldObject:
    def __init__(self, x=0.0, y=0.0, obj_type=WorldObjectType.Empty, value=0.0, radius=0.5):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def __repr__(self):
        return f"WorldObject(x={self.x}, y={self.y}, type={self.type.name}, value={self.value})"

class SimpleWorld:
    def __init__(self):
        self.width = 0
        self.height = 0
        self.vision_width = 0
        self.vision_height = 0
        self.max_energy = 100.0
        self.energy_decay_rate = 0.01
        self.simulation_time = 0.0
        self.agent_x = 0.0
        self.agent_y = 0.0
        self.agent_orientation = 0.0
        self.agent_velocity_x = 0.0
        self.agent_velocity_y = 0.0
        self.agent_energy = 100.0
        self.agent_health = 100.0
        self.agent_age = 0
        self.objects = []
        self.random_seed = 42
    
    def configure(self, width, height, vision_width, vision_height):
        """Configure world dimensions"""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        return True
    
    def reset(self):
        """Reset world to initial state"""
        self.simulation_time = 0.0
        self.agent_x = self.width // 2
        self.agent_y = self.height // 2
        self.agent_orientation = 0.0
        self.agent_velocity_x = 0.0
        self.agent_velocity_y = 0.0
        self.agent_energy = self.max_energy
        self.agent_health = self.max_energy
        self.agent_age = 0
        self.objects = []
        return True
    
    def set_agent_start(self, x, y):
        """Set agent starting position"""
        self.agent_x = x
        self.agent_y = y
        return True
    
    def update(self, timestep):
        """Update world state"""
        self.simulation_time += timestep
        self.agent_age += 1
        
        # Decay agent energy
        self.agent_energy *= (1.0 - self.energy_decay_rate * timestep)
        
        # Update agent position
        self.agent_x += self.agent_velocity_x * timestep
        self.agent_y += self.agent_velocity_y * timestep
        
        # Apply boundary conditions
        self.agent_x = max(0, min(self.agent_x, self.width - 1))
        self.agent_y = max(0, min(self.agent_y, self.height - 1))
        
        # Apply friction to velocity
        self.agent_velocity_x *= (1.0 - self.energy_decay_rate * timestep)
        self.agent_velocity_y *= (1.0 - self.energy_decay_rate * timestep)
        
        return True
    
    def apply_motor_command(self, command, current_time):
        """Apply motor command to world"""
        # Parse command and apply forces to agent
        action_type = getattr(command, 'action_type', 'Wait')
        parameters = getattr(command, 'parameters', {})
        
        if action_type == "MoveForward":
            speed = parameters.get('speed', 1.0)
            self.agent_velocity_x += speed * timestep * 0.1
            self.agent_velocity_y += speed * timestep * 0.1
        elif action_type == "MoveBackward":
            speed = parameters.get('speed', 1.0)
            self.agent_velocity_x -= speed * timestep * 0.1
            self.agent_velocity_y -= speed * timestep * 0.1
        elif action_type == "TurnLeft":
            angular_speed = parameters.get('angular_speed', 1.0)
            self.agent_orientation += angular_speed * timestep
        elif action_type == "TurnRight":
            angular_speed = parameters.get('angular_speed', 1.0)
            self.agent_orientation -= angular_speed * timestep
        elif action_type == "Look":
            # Look action doesn't move the agent
            pass
        
        return True
    
    def get_sensory_percept(self):
        """Get sensory percept from world"""
        percept = SensoryPercept()
        
        # Vision: simple gradient based on agent position
        vision_data = []
        for y in range(self.vision_height):
            row = []
            for x in range(self.vision_width):
                # Calculate relative position
                world_x = self.agent_x - self.vision_width // 2 + x
                world_y = self.agent_y - self.vision_height // 2 + y
                
                if 0 <= world_x < self.width and 0 <= world_y < self.height:
                    # Sample world at this position
                    value = self._sample_world(world_x, world_y)
                    row.append(value)
                else:
                    row.append(0.0)
            vision_data.extend(row)
        
        percept.set_vision(Vision(self.vision_width, self.vision_height, vision_data))
        
        # Touch: simple distance-based sensing
        touch_data = []
        for obj in self.objects:
            if obj.active:
                distance = ((self.agent_x - obj.x) ** 2 + (self.agent_y - obj.y) ** 2) ** 0.5
                touch_data.append(distance)
        
        percept.set_touch(Touch(touch_data))
        
        # Internal signals
        internal_signals = InternalSignals()
        internal_signals.add_signal(self.agent_energy / self.max_energy)
        internal_signals.add_signal(self.agent_health / self.max_energy)
        internal_signals.add_signal(self.agent_age / 1000.0)
        internal_signals.add_signal(self.simulation_time / 10000.0)
        percept.set_internal(internal_signals)
        
        # Proprioception
        proprioception = Proprioception()
        proprioception.set_position(self.agent_x, self.agent_y)
        proprioception.set_orientation(self.agent_orientation)
        proprioception.set_velocity(self.agent_velocity_x, self.agent_velocity_y)
        percept.set_proprioception(proprioception)
        
        # Audio: simple spatial sound
        audio_data = Audio()
        audio_data.set_sample_rate(44100)
        audio_data.set_num_samples(0)
        percept.set_audio(audio_data)
        
        return percept
    
    def get_agent_body(self):
        """Get agent body state"""
        body = AgentBody()
        body.x = self.agent_x
        body.y = self.agent_y
        body.orientation = self.agent_orientation
        body.velocity_x = self.agent_velocity_x
        body.velocity_y = self.agent_velocity_y
        body.energy = self.agent_energy
        body.health = self.agent_health
        body.age = self.agent_age
        body.is_moving = (self.agent_velocity_x != 0 or self.agent_velocity_y != 0)
        body.is_turning = (abs(self.agent_orientation) > 0.01)
        body.last_action_time = self.simulation_time
        return body
    
    def add_object(self, obj):
        """Add object to world"""
        self.objects.append(obj)
        return True
    
    def remove_object(self, x, y):
        """Remove object at position"""
        self.objects = [obj for obj in self.objects if not (obj.active and abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1)]
        return True
    
    def is_valid_position(self, x, y):
        """Check if position is valid"""
        return (0 <= x < self.width and 0 <= y < self.height and
                not any(obj.active and abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1 for obj in self.objects))
    
    def get_width(self):
        """Get world width"""
        return self.width
    
    def get_height(self):
        """Get world height"""
        return self.height
    
    def get_max_energy(self):
        """Get maximum energy"""
        return self.max_energy
    
    def set_max_energy(self, e):
        """Set maximum energy"""
        self.max_energy = e
        return True
    
    def get_energy_decay_rate(self):
        """Get energy decay rate"""
        return self.energy_decay_rate
    
    def set_energy_decay_rate(self, r):
        """Set energy decay rate"""
        self.energy_decay_rate = r
        return True
    
    def get_simulation_time(self):
        """Get simulation time"""
        return self.simulation_time
    
    def set_random_seed(self, seed):
        """Set random seed"""
        self.random_seed = seed
        return True
    
    def get_random_seed(self):
        """Get random seed"""
        return self.random_seed
    
    def _sample_world(self, x, y):
        """Sample world at position (stub implementation)"""
        # Simple implementation: return distance from center
        center_x = self.width / 2
        center_y = self.height / 2
        distance = ((x - center_x) ** 2 + (y - center_y) ** 2) ** 0.5
        max_distance = ((self.width / 2) ** 2 + (self.height / 2) ** 2) ** 0.5
        return 1.0 - min(1.0, distance / max_distance)

# Supporting classes
class Vision:
    def __init__(self, width, height, channels=3):
        self.width = width
        self.height = height
        self.channels = channels
        self.data = []
    
    def set_data(self, data):
        self.data = data
    
    def get_data(self):
        return self.data
    
    def get_dimensions(self):
        return [self.width, self.height]

class Audio:
    def __init__(self, sample_rate=44100, num_samples=0):
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self.data = []
    
    def set_data(self, data):
        self.data = data
        self.num_samples = len(data)
    
    def get_data(self):
        return self.data
    
    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate
    
    def get_sample_rate(self):
        return self.sample_rate
    
    def get_num_samples(self):
        return self.num_samples

class InternalSignals:
    def __init__(self):
        self.signals = []
    
    def add_signal(self, value):
        self.signals.append(value)
    
    def clear_signals(self):
        self.signals = []
    
    def get_signals(self):
        return self.signals

class Touch:
    def __init__(self, data=None):
        self.data = data or []
    
    def set_data(self, data):
        self.data = data
    
    def get_data(self):
        return self.data

class Proprioception:
    def __init__(self):
        self.position_x = 0.0
        self.position_y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
    
    def set_position(self, x, y):
        self.position_x = x
        self.position_y = y
    
    def set_orientation(self, orientation):
        self.orientation = orientation
    
    def set_velocity(self, vx, vy):
        self.velocity_x = vx
        self.velocity_y = vy

# Re-export types
from .core import WorldObjectType

__all__ = [
    'SimpleWorld', 'WorldObject', 'Vision', 'Audio', 
    'InternalSignals', 'Touch', 'Proprioception', 'WorldObjectType'
]
