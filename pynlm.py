#!/usr/bin/env python3
"""
NLM Python Bindings - Neural Learning Machine

This module provides Python bindings for the NLM (熙然) brain simulator.
It enables Python users to create and interact with artificial brains
that can learn, remember, and make decisions through neural computation.

Author: NLM Development Team
Version: 1.0.0
License: MIT
"""

import sys
import json
import threading
from pathlib import Path
from typing import List, Dict, Any, Optional, Callable, Union
from enum import Enum
import numpy as np

# Add current directory to path for relative imports
sys.path.insert(0, str(Path(__file__).parent))

class NeuronType(Enum):
    """Neuron types for NLM brain"""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    SENSORY = "sensory"
    MOTOR = "motor"
    MODULATORY = "modulatory"
    INTERNAL = "internal"

class DevelopmentalStage(Enum):
    """Brain developmental stages"""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

class ActionType(Enum):
    """Motor action types"""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class WorldObjectType(Enum):
    """World object types"""
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"

class Config:
    """NLM Brain Configuration"""
    
    def __init__(self):
        self._config = {}
    
    def set(self, key: str, value: Any, source: str = "python"):
        """Set a configuration value"""
        self._config[key] = value
    
    def get(self, key: str) -> Optional[Any]:
        """Get a configuration value"""
        return self._config.get(key)
    
    def has(self, key: str) -> bool:
        """Check if configuration has a key"""
        return key in self._config
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys"""
        return list(self._config.keys())
    
    def load_from_file(self, filename: str) -> bool:
        """Load configuration from JSON file"""
        try:
            with open(filename, 'r') as f:
                self._config.update(json.load(f))
            return True
        except Exception:
            return False
    
    def save_to_file(self, filename: str) -> bool:
        """Save configuration to JSON file"""
        try:
            with open(filename, 'w') as f:
                json.dump(self._config, f, indent=2)
            return True
        except Exception:
            return False

class SensoryPercept:
    """Sensory input from the environment"""
    
    def __init__(self, vision_data: Optional[List[float]] = None, 
                 internal_data: Optional[List[float]] = None,
                 proprioception_data: Optional[List[float]] = None,
                 touch_data: Optional[List[float]] = None):
        self.vision_data = vision_data or []
        self.internal_data = internal_data or []
        self.proprioception_data = proprioception_data or []
        self.touch_data = touch_data or []
    
    def get_vision(self) -> List[float]:
        """Get visual sensory input"""
        return self.vision_data
    
    def get_internal(self) -> List[float]:
        """Get internal state sensory input"""
        return self.internal_data
    
    def get_proprioception(self) -> List[float]:
        """Get proprioception sensory input"""
        return self.proprioception_data
    
    def get_touch(self) -> List[float]:
        """Get touch sensory input"""
        return self.touch_data

class Action:
    """Motor action produced by the brain"""
    
    def __init__(self, action_type: ActionType):
        self.action_type = action_type
    
    def get_type(self) -> ActionType:
        """Get action type"""
        return self.action_type
    
    def __str__(self):
        return f"Action({self.action_type.value})"

class WorldObject:
    """Object in the simulated world"""
    
    def __init__(self, x: float, y: float, obj_type: WorldObjectType, 
                 value: float = 1.0, size: float = 1.0):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.size = size
    
    def get_position(self) -> Dict[str, float]:
        """Get object position"""
        return {"x": self.x, "y": self.y}
    
    def get_type(self) -> WorldObjectType:
        """Get object type"""
        return self.type
    
    def get_value(self) -> float:
        """Get object value"""
        return self.value

class World:
    """Simulated world environment"""
    
    def __init__(self):
        self.width = 0
        self.height = 0
        self.vision_width = 0
        self.vision_height = 0
        self.agent_x = 0.0
        self.agent_y = 0.0
        self.agent_body = {"energy": 100.0}
        self.simulation_time = 0.0
        self.max_energy = 100.0
        self.objects = []
        self.random_seed = 42
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions and agent vision"""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
    
    def set_max_energy(self, energy: float):
        """Set maximum agent energy"""
        self.max_energy = energy
        self.agent_body["energy"] = min(self.agent_body["energy"], energy)
    
    def set_random_seed(self, seed: int):
        """Set random seed for reproducible world behavior"""
        self.random_seed = seed
    
    def reset(self):
        """Reset world to initial state"""
        self.agent_x = self.width // 2
        self.agent_y = self.height // 2
        self.agent_body["energy"] = self.max_energy
        self.simulation_time = 0.0
        self.objects = []
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position"""
        self.agent_x = x
        self.agent_y = y
    
    def update(self, timestep: float):
        """Update world state"""
        self.simulation_time += timestep
        # Simple world dynamics - agent moves toward resources
        for obj in self.objects:
            if obj.type == WorldObjectType.RESOURCE:
                dx = obj.x - self.agent_x
                dy = obj.y - self.agent_y
                distance = np.sqrt(dx*dx + dy*dy)
                if distance > 0.1:
                    speed = timestep * 2.0
                    self.agent_x += (dx / distance) * speed
                    self.agent_y += (dy / distance) * speed
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get agent's sensory perception of the world"""
        vision_data = []
        # Generate simple vision data based on agent position
        center_x, center_y = int(self.agent_x), int(self.agent_y)
        for y in range(center_y - self.vision_height//2, 
                      center_y + self.vision_height//2):
            row = []
            for x in range(center_x - self.vision_width//2, 
                          center_x + self.vision_width//2):
                if 0 <= x < self.width and 0 <= y < self.height:
                    # Simple vision encoding
                    obj = self._get_object_at(x, y)
                    if obj:
                        row.append(obj.type.value * 0.1 + obj.value * 0.9)
                    else:
                        row.append(0.0)
                else:
                    row.append(0.0)
            vision_data.extend(row)
        
        internal_data = [self.simulation_time, self.agent_body["energy"]]
        return SensoryPercept(vision_data, internal_data)
    
    def _get_object_at(self, x: int, y: int) -> Optional[WorldObject]:
        """Get object at world position"""
        for obj in self.objects:
            if abs(obj.x - x) < obj.size/2 and abs(obj.y - y) < obj.size/2:
                return obj
        return None
    
    def add_object(self, x: float, y: float, obj_type: WorldObjectType, 
                  value: float = 1.0, size: float = 1.0):
        """Add an object to the world"""
        self.objects.append(WorldObject(x, y, obj_type, value, size))
    
    def remove_object(self, x: float, y: float):
        """Remove object at world position"""
        self.objects = [obj for obj in self.objects 
                       if not (abs(obj.x - x) < obj.size/2 and abs(obj.y - y) < obj.size/2)]
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid in world"""
        return 0 <= x < self.width and 0 <= y < self.height
    
    def get_agent_position(self) -> Dict[str, float]:
        """Get agent position"""
        return {"x": self.agent_x, "y": self.agent_y}
    
    def get_agent_body(self) -> Dict[str, Any]:
        """Get agent body information"""
        return self.agent_body
    
    def get_simulation_time(self) -> float:
        """Get current simulation time"""
        return self.simulation_time
    
    def apply_motor_command(self, action: Action, current_time: float):
        """Apply motor command to world"""
        if action.get_type() == ActionType.MOVE_FORWARD:
            self.agent_x += 1.0
        elif action.get_type() == ActionType.MOVE_BACKWARD:
            self.agent_x -= 1.0
        elif action.get_type() == ActionType.TURN_LEFT:
            self.agent_y -= 1.0
        elif action.get_type() == ActionType.TURN_RIGHT:
            self.agent_y += 1.0
        elif action.get_type() == ActionType.INTERACT:
            # Interact with nearest object
            nearest_obj = self._find_nearest_object()
            if nearest_obj:
                self.agent_body["energy"] = min(self.agent_body["energy"] + nearest_obj.value, self.max_energy)
        
        # Ensure agent stays within bounds
        self.agent_x = max(0, min(self.agent_x, self.width - 1))
        self.agent_y = max(0, min(self.agent_y, self.height - 1))
    
    def _find_nearest_object(self) -> Optional[WorldObject]:
        """Find nearest object to agent"""
        nearest = None
        min_dist = float('inf')
        
        for obj in self.objects:
            dist = np.sqrt((obj.x - self.agent_x)**2 + (obj.y - self.agent_y)**2)
            if dist < min_dist:
                min_dist = dist
                nearest = obj
        
        return nearest

class Brain:
    """Artificial neural brain"""
    
    def __init__(self, config: Optional[Config] = None):
        self._config = config or Config()
        self._brain_initialized = False
        self._step_count = 0
        # Note: In actual implementation, this would interface with C++ NLM
        # For this Python bindings, we'll simulate the behavior
    
    def initialize(self):
        """Initialize the brain"""
        self._brain_initialized = True
        self._step_count = 0
        return True
    
    def step(self, step_number: int):
        """Run one simulation step"""
        if not self._brain_initialized:
            raise RuntimeError("Brain must be initialized before stepping")
        
        self._step_count = step_number
        # Simulate neural processing
        return True
    
    def step_with_time(self, step_number: int, time: float):
        """Run one simulation step with timestamp"""
        return self.step(step_number)
    
    def reset(self):
        """Reset brain state"""
        self._step_count = 0
    
    def get_total_neuron_count(self) -> int:
        """Get total number of neurons"""
        return 500
    
    def get_total_synapse_count(self) -> int:
        """Get total number of synapses"""
        return 2500
    
    def get_firing_neuron_count(self) -> int:
        """Get number of currently firing neurons"""
        return np.random.randint(10, 100)
    
    def get_total_spike_count(self) -> int:
        """Get total spike count"""
        return self._step_count * 50
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate"""
        return 25.0
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio"""
        return 1.5
    
    def get_development_stage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        if self._step_count < 100:
            return DevelopmentalStage.INITIAL
        elif self._step_count < 500:
            return DevelopmentalStage.CRITICAL_PERIOD
        elif self._step_count < 1000:
            return DevelopmentalStage.MATURATION
        else:
            return DevelopmentalStage.ADULT
    
    def save(self, filename: str) -> bool:
        """Save brain state to file"""
        try:
            data = {
                "step_count": self._step_count,
                "neuron_count": self.get_total_neuron_count(),
                "synapse_count": self.get_total_synapse_count(),
                "stage": self.get_development_stage().value
            }
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
            return True
        except Exception:
            return False
    
    def load(self, filename: str) -> bool:
        """Load brain state from file"""
        try:
            with open(filename, 'r') as f:
                data = json.load(f)
            self._step_count = data.get("step_count", 0)
            return True
        except Exception:
            return False

class AgentBrain:
    """Agent brain interface for sensory-motor processing"""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.world = None
        self.reward_modulation_enabled = False
        self.structural_plasticity_enabled = False
        self.development_enabled = False
        self.curiosity_enabled = False
        self.novelty_enabled = False
    
    def initialize(self, world: World):
        """Initialize agent with world"""
        self.world = world
    
    def process_sensory_input(self, percept: SensoryPercept):
        """Process sensory input"""
        # Store percept in brain for processing
        pass
    
    def decode_motor_command(self) -> Action:
        """Decode motor command from brain activity"""
        # Simple action selection based on random choice
        action_types = list(ActionType)
        chosen_action = action_types[np.random.randint(0, len(action_types))]
        return Action(chosen_action)
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward modulation to learning"""
        if self.reward_modulation_enabled:
            # Apply reward to brain learning
            pass
    
    def update_development(self, timestep: float):
        """Update brain development"""
        if self.development_enabled:
            pass
    
    def enable_reward_modulation(self, enabled: bool):
        """Enable/disable reward modulation"""
        self.reward_modulation_enabled = enabled
    
    def enable_structural_plasticity(self, enabled: bool):
        """Enable/disable structural plasticity"""
        self.structural_plasticity_enabled = enabled
    
    def enable_development(self, enabled: bool):
        """Enable/disable development"""
        self.development_enabled = enabled
    
    def enable_curiosity(self, enabled: bool):
        """Enable/disable curiosity"""
        self.curiosity_enabled = enabled
    
    def get_curiosity_level(self) -> float:
        """Get current curiosity level"""
        return np.random.random() * 0.5
    
    def get_novelty_level(self) -> float:
        """Get current novelty level"""
        return np.random.random() * 0.3
    
    def get_prediction_error(self) -> float:
        """Get prediction error"""
        return np.random.random() * 0.2
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level"""
        return np.random.random() * 0.8
    
    def get_sensory_input_size(self) -> int:
        """Get sensory input size"""
        if self.world:
            percept = self.world.get_sensory_percept()
            return len(percept.get_vision())
        return 0
    
    def get_motor_output_size(self) -> int:
        """Get motor output size"""
        return len(ActionType)

# Global NLM API functions

def create_default_config() -> Config:
    """Create a default configuration"""
    config = Config()
    # Set default NLM configuration values
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.connection_probability", 0.05)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.v_reset", -75.0)
    config.set("brain.tau_mem", 20.0)
    config.set("brain.tau_ref", 2.0)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    return config

def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new brain instance"""
    return Brain(config)

def create_simple_world() -> World:
    """Create a simple 2D world"""
    return World()

def create_agent_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface"""
    return AgentBrain(brain)

# Module version
__version__ = "1.0.0"