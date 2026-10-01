# NLM Agent Python Module
# Python bindings for NLM agent brain interface

from .core import NeuronId
from typing import Optional, List, Dict, Any

class AgentBrain:
    def __init__(self, brain):
        self._brain = brain
        self._world = None
    
    def initialize(self, world):
        """Initialize agent with world"""
        self._world = world
        return True
    
    def get_sensory_input_size(self):
        """Get expected sensory input size"""
        return 0
    
    def get_motor_output_size(self):
        """Get expected motor output size"""
        return 0
    
    def process_sensory_input(self, percept):
        """Process sensory percept and inject into brain"""
        if self._brain:
            self._brain.receive_sensory_input(percept)
        return True
    
    def decode_motor_command(self):
        """Decode brain motor activity into motor command"""
        if self._brain:
            action = self._brain.produce_action()
            # Convert action to motor command
            class MotorCommand:
                def __init__(self):
                    self.action_type = getattr(action, 'type', 'Wait')
                    self.parameters = getattr(action, 'parameters', {})
            return MotorCommand()
        return None
    
    def apply_reward_modulation(self, reward, predicted_reward):
        """Apply reward-based neuromodulation"""
        # Stub implementation
        return True
    
    def update_development(self, timestep):
        """Update development system"""
        # Stub implementation
        return True
    
    def get_developmental_stage(self):
        """Get current developmental stage"""
        from .core import DevelopmentalStage
        return DevelopmentalStage.Initial
    
    def get_neuromodulation_level(self):
        """Get current neuromodulation level"""
        return 0.0
    
    def get_curiosity_level(self):
        """Get curiosity level"""
        return 0.0
    
    def get_novelty_level(self):
        """Get novelty level"""
        return 0.0
    
    def get_prediction_error(self):
        """Get prediction error"""
        return 0.0
    
    def reset(self):
        """Reset agent for new episode"""
        return True
    
    def get_brain(self):
        """Get the underlying brain"""
        return self._brain
    
    def enable_reward_modulation(self, enable):
        """Enable/disable reward modulation"""
        return True
    
    def enable_structural_plasticity(self, enable):
        """Enable/disable structural plasticity"""
        return True
    
    def enable_development(self, enable):
        """Enable/disable development"""
        return True
    
    def enable_curiosity(self, enable):
        """Enable/disable curiosity"""
        return True
    
    def is_reward_modulation_enabled(self):
        """Check if reward modulation is enabled"""
        return False
    
    def is_structural_plasticity_enabled(self):
        """Check if structural plasticity is enabled"""
        return False
    
    def is_development_enabled(self):
        """Check if development is enabled"""
        return False
    
    def is_curiosity_enabled(self):
        """Check if curiosity is enabled"""
        return False

# Supporting classes
class SensoryPercept:
    def __init__(self):
        self.vision = None
        self.touch = None
        self.internal = None
        self.proprioception = None
        self.audio = None
        self.timestamp = 0.0
    
    def get_vision(self):
        return self.vision
    
    def set_vision(self, vision):
        self.vision = vision
    
    def get_vision_width(self):
        return 0 if self.vision is None else 0
    
    def get_vision_height(self):
        return 0 if self.vision is None else 0
    
    def get_touch(self):
        return self.touch
    
    def set_touch(self, touch):
        self.touch = touch
    
    def get_internal(self):
        return self.internal
    
    def set_internal(self, internal):
        self.internal = internal
    
    def get_proprioception(self):
        return self.proprioception
    
    def set_proprioception(self, proprioception):
        self.proprioception = proprioception
    
    def get_audio(self):
        return self.audio
    
    def set_audio(self, audio):
        self.audio = audio
    
    def get_all_signals(self):
        return []
    
    def get_timestamp(self):
        return self.timestamp
    
    def set_timestamp(self, timestamp):
        self.timestamp = timestamp

class AgentBody:
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
        self.angular_velocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0
    
    def reset(self):
        """Reset agent body state"""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
        self.angular_velocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0

class WorldObject:
    def __init__(self, x=0.0, y=0.0, obj_type=None, value=0.0, radius=0.5):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True

# Action classes
class Action:
    def __init__(self, action_type="Wait", parameters=None):
        self.type = action_type
        self.parameters = parameters or {}

class ActionResult:
    def __init__(self, reward=0.0, success=True, message=""):
        self.reward = reward
        self.success = success
        self.message = message

# Re-export types
from .core import ActionType

__all__ = [
    'AgentBrain', 'SensoryPercept', 'AgentBody', 'WorldObject', 
    'Action', 'ActionResult', 'ActionType'
]
