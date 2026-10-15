#!/usr/bin/env python3
"""
NLM Python API - Enhanced Python interface for NLM (Neural Learning Machine)

This module provides a comprehensive Python interface to the NLM framework,
allowing users to create and simulate artificial developmental brains.
"""

import sys
import os
import json
import logging
from pathlib import Path
from typing import Optional, List, Dict, Any, Union, Callable
from enum import Enum
import ctypes
import ctypes.util
from dataclasses import dataclass, field
from contextlib import contextmanager

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class NLMVersionError(NLMError):
    """Raised when there's a version compatibility issue."""
    pass

class ConfigError(NLMError):
    """Raised when there's a configuration error."""
    pass

class BrainInitializationError(NLMError):
    """Raised when brain initialization fails."""
    pass

class StepError(NLMError):
    """Raised when simulation step fails."""
    pass

# Version information
__version__ = "0.1.0"
__author__ = "NLM Research Team"
__email__ = "contact@nlm.org"

# Import NLM C++ extensions if available
try:
    import _pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    logger.warning("NLM C++ extensions not available. Using fallback implementation.")

# Type definitions for better Python API
@dataclass
class NLMConfig:
    """Configuration settings for NLM brain simulation."""
    
    # Core parameters
    neuron_count: int = 1000
    region_count: int = 2
    connection_probability: float = 0.1
    random_seed: int = 42
    simulation_timestep: float = 0.001
    
    # Plasticity settings
    plasticity_learning_rate: float = 0.01
    stdp_ltp_weight: float = 0.02
    stdp_ltd_weight: float = 0.015
    stdp_time_constant: float = 20.0
    
    # Development settings
    development_synaptogenesis_rate: float = 0.0001
    development_pruning_rate: float = 0.00001
    
    # Neuromodulation settings
    dopamine_baseline: float = 0.1
    
    # Reward settings
    reward_discount_factor: float = 0.99
    
    # Environment settings
    environment_name: str = "GridWorld"
    environment_width: int = 20
    environment_height: int = 20
    
    # Simulation settings
    max_simulation_steps: int = 10000
    simulation_time_limit: float = 0.0
    
    # Visualization settings
    visualization_enabled: bool = False
    visualization_update_rate: float = 30.0
    
    # Additional configuration
    vision_width: int = 16
    vision_height: int = 16
    stdp_tau: float = 20.0
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'NLMConfig':
        """Create NLMConfig from dictionary."""
        # Create instance with all default values
        instance = cls()
        
        # Update with provided values
        for key, value in config_dict.items():
            if hasattr(instance, key):
                setattr(instance, key, value)
        
        return instance
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert NLMConfig to dictionary."""
        return {
            attr: getattr(self, attr)
            for attr in self.__dataclass_fields__
        }
    
    def save(self, filepath: str) -> None:
        """Save configuration to JSON file."""
        with open(filepath, 'w') as f:
            json.dump(self.to_dict(), f, indent=2)
    
    @classmethod
    def load(cls, filepath: str) -> 'NLMConfig':
        """Load configuration from JSON file."""
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        return cls.from_dict(config_dict)

class SimulationStatus:
    """Status information for simulation state."""
    
    def __init__(self):
        self.step: int = 0
        self.time: float = 0.0
        self.neuron_count: int = 0
        self.synapse_count: int = 0
        self.total_spikes: int = 0
        self.firing_neuron_count: int = 0
        self.avg_firing_rate: float = 0.0
        self.memory_episodes: int = 0
        self.dopamine_level: float = 0.0
        self.curiosity_level: float = 0.0
        self.novelty_level: float = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'step': self.step,
            'time': self.time,
            'neuron_count': self.neuron_count,
            'synapse_count': self.synapse_count,
            'total_spikes': self.total_spikes,
            'firing_neuron_count': self.firing_neuron_count,
            'avg_firing_rate': self.avg_firing_rate,
            'memory_episodes': self.memory_episodes,
            'dopamine_level': self.dopamine_level,
            'curiosity_level': self.curiosity_level,
            'novelty_level': self.novelty_level,
        }

class MotorCommand(Enum):
    """Motor commands for agent brain."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    INTERACT = 4
    WAIT = 5
    LOOK_LEFT = 6
    LOOK_RIGHT = 7
    
    def to_string(self) -> str:
        """Convert to string representation."""
        return self.name.replace('_', ' ')

class DevelopmentalStage(Enum):
    """Developmental stages of the brain."""
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4

class World:
    """2D simulation world for agent-brain interaction."""
    
    def __init__(self, width: float, height: float, vision_width: int = 16, 
                 vision_height: int = 16, random_seed: int = 42):
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self.random_seed = random_seed
        self.agent_body = AgentBody()
        self.simulation_time = 0.0
        self._reset()
    
    def _reset(self) -> None:
        """Reset world state."""
        self.agent_body.x = self.width // 2
        self.agent_body.y = self.height // 2
        self.agent_body.energy = 100.0
        self.agent_body.hp = 100.0
        self.simulation_time = 0.0
    
    def reset(self) -> None:
        """Reset the entire world."""
        self._reset()
    
    def update(self, dt: float) -> None:
        """Update world simulation."""
        self.simulation_time += dt
        # Update agent body position based on movement
        if hasattr(self, '_pending_movement'):
            self.agent_body.x += self._pending_movement.get('dx', 0)
            self.agent_body.y += self._pending_movement.get('dy', 0)
            # Keep agent in bounds
            self.agent_body.x = max(0, min(self.width - 1, self.agent_body.x))
            self.agent_body.y = max(0, min(self.height - 1, self.agent_body.y))
            delattr(self, '_pending_movement')
    
    def apply_motor_command(self, command: MotorCommand) -> 'ActionResult':
        """Apply a motor command to the world."""
        result = ActionResult()
        
        # Default behavior - move in direction
        dx, dy = 0, 0
        energy_cost = 1.0
        
        if command == MotorCommand.MOVE_FORWARD:
            dy = -1
        elif command == MotorCommand.MOVE_BACKWARD:
            dy = 1
        elif command == MotorCommand.TURN_LEFT:
            dx = -1
        elif command == MotorCommand.TURN_RIGHT:
            dx = 1
        
        if dx != 0 or dy != 0:
            # Check bounds
            new_x = self.agent_body.x + dx
            new_y = self.agent_body.y + dy
            
            if 0 <= new_x < self.width and 0 <= new_y < self.height:
                self._pending_movement = {'dx': dx, 'dy': dy}
                self.update(0.01)  # Small time step
        
        # Calculate reward based on action
        result.reward = self._calculate_reward(command, result)
        result.success = True
        
        return result
    
    def get_sensory_percept(self) -> 'SensoryPercept':
        """Get sensory percept from world."""
        percept = SensoryPercept()
        
        # Vision - simplified for now
        percept.vision = [[0.5 for _ in range(self.vision_width)] 
                         for _ in range(self.vision_height)]
        
        # Position
        percept.x = self.agent_body.x
        percept.y = self.agent_body.y
        
        # Energy
        percept.energy = self.agent_body.energy
        
        # Time
        percept.time = self.simulation_time
        
        return percept
    
    def _calculate_reward(self, command: MotorCommand, result: 'ActionResult') -> float:
        """Calculate reward for action."""
        base_reward = 0.0
        
        # Different commands have different rewards
        if command == MotorCommand.INTERACT:
            base_reward = 10.0
        elif command == MotorCommand.MOVE_FORWARD:
            base_reward = 1.0
        elif command == MotorCommand.WAIT:
            base_reward = -1.0  # Waiting costs energy
        
        # Subtract energy cost
        base_reward -= 1.0
        
        return base_reward

@dataclass
class ActionResult:
    """Result of applying a motor command."""
    reward: float = 0.0
    success: bool = False
    energy_cost: float = 0.0
    damage: float = 0.0
    distance_traveled: float = 0.0

@dataclass
class AgentBody:
    """Agent body in the world."""
    x: float = 0.0
    y: float = 0.0
    energy: float = 100.0
    hp: float = 100.0

@dataclass
class SensoryPercept:
    """Sensory percept from the world."""
    
    # Vision
    vision: List[List[float]] = field(default_factory=lambda: 
                                     [[0.5 for _ in range(16)] for _ in range(16)])
    
    # Position
    x: float = 0.0
    y: float = 0.0
    
    # Energy
    energy: float = 100.0
    
    # Time
    time: float = 0.0
    
    # Novelty
    novelty: float = 0.0

class Brain:
    """Main NLM brain simulation."""
    
    def __init__(self, config: Optional[NLMConfig] = None):
        self.config = config or NLMConfig()
        self.status = SimulationStatus()
        self._is_initialized = False
        self._is_running = False
        
        # Python-C++ interface
        self._cpp_bridge = None
        if PYNLM_AVAILABLE:
            try:
                self._cpp_bridge = _pynlm.NLMBrainBridge(self.config)
            except Exception as e:
                logger.warning(f"Failed to create C++ bridge: {e}")
                PYNLM_AVAILABLE = False
        
        # Fallback Python implementation
        self._init_fallback()
    
    def _init_fallback(self) -> None:
        """Initialize fallback Python implementation."""
        self.neurons = []
        self.synapses = []
        self.regions = []
        self.spike_system = None
        self.memory_system = None
        self.neuromodulation = None
    
    def initialize(self) -> bool:
        """Initialize the brain."""
        try:
            if PYNLM_AVAILABLE and self._cpp_bridge:
                success = self._cpp_bridge.initialize()
                if success:
                    self._update_status_from_cpp()
                    self._is_initialized = True
                    return True
            
            # Fallback Python implementation
            self._initialize_fallback()
            self._is_initialized = True
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize brain: {e}")
            raise BrainInitializationError(f"Brain initialization failed: {e}")
    
    def _initialize_fallback(self) -> None:
        """Initialize fallback Python implementation."""
        # Create neurons
        for i in range(self.config.neuron_count):
            neuron = self._create_neuron(i)
            self.neurons.append(neuron)
        
        # Create regions
        neurons_per_region = self.config.neuron_count // self.config.region_count
        for region_id in range(self.config.region_count):
            region_neurons = self.neurons[region_id * neurons_per_region:
                                        (region_id + 1) * neurons_per_region]
            region = NeuralRegion(region_id, region_neurons)
            self.regions.append(region)
        
        # Create spike system
        self.spike_system = SpikeSystem()
        
        # Create memory system
        self.memory_system = MemorySystem()
        
        # Create neuromodulation
        self.neuromodulation = NeuromodulationSystem()
    
    def _create_neuron(self, neuron_id: int) -> 'Neuron':
        """Create a single neuron."""
        return Neuron(
            neuron_id,
            neuron_type=NeuronType.SENSORIAL if neuron_id % 10 == 0 
                       else NeuronType.EXCITATORY if neuron_id % 3 == 0
                       else NeuronType.INHIBITORY
        )
    
    def step(self, step_num: int, time: float) -> bool:
        """Perform one simulation step."""
        if not self._is_initialized:
            raise StepError("Brain not initialized. Call initialize() first.")
        
        try:
            if PYNLM_AVAILABLE and self._cpp_bridge:
                success = self._cpp_bridge.step(step_num, time)
                if success:
                    self._update_status_from_cpp()
                    return True
            
            # Fallback Python implementation
            self._step_fallback(step_num, time)
            self._is_running = True
            return True
            
        except Exception as e:
            logger.error(f"Step failed: {e}")
            raise StepError(f"Simulation step failed: {e}")
    
    def _step_fallback(self, step_num: int, time: float) -> None:
        """Perform one simulation step in fallback implementation."""
        # Inject random input
        for neuron in self.neurons:
            if neuron.neuron_type == NeuronType.SENSORIAL:
                # Random input
                if random.random() < 0.1:
                    neuron.inject_current(30.0 + random.uniform(-5, 5))
        
        # Update neuron dynamics
        for neuron in self.neurons:
            neuron.update_dynamics(time)
            
            if neuron.is_spiking():
                neuron.clear_spike()
                # Propagate spikes
                self._propagate_spikes(neuron)
        
        # Update memory
        if self.memory_system:
            self.memory_system.update()
        
        # Update neuromodulation
        if self.neuromodulation:
            self.neuromodulation.update()
        
        # Update status
        self.status.step = step_num
        self.status.time = time
        self.status.neuron_count = len(self.neurons)
        self.status.total_spikes = sum(neuron.spike_count for neuron in self.neurons)
        self.status.firing_neuron_count = sum(1 for neuron in self.neurons if neuron.is_firing)
        self.status.avg_firing_rate = (self.status.total_spikes / 
                                      max(1, self.config.max_simulation_steps))
    
    def _propagate_spikes(self, neuron: 'Neuron') -> None:
        """Propagate spikes to connected neurons."""
        for synapse in neuron.get_output_synapses():
            target_neuron = synapse.target_neuron
            if target_neuron:
                target_neuron.receive_spike(synapse.delay, synapse.weight)
    
    def _update_status_from_cpp(self) -> None:
        """Update status from C++ bridge."""
        if self._cpp_bridge:
            status_dict = self._cpp_bridge.get_status()
            for key, value in status_dict.items():
                if hasattr(self.status, key):
                    setattr(self.status, key, value)
    
    def reset(self) -> None:
        """Reset the brain to initial state."""
        if PYNLM_AVAILABLE and self._cpp_bridge:
            self._cpp_bridge.reset()
        
        self._reset_fallback()
        self._is_running = False
    
    def _reset_fallback(self) -> None:
        """Reset fallback implementation."""
        for neuron in self.neurons:
            neuron.reset()
        
        if self.spike_system:
            self.spike_system.reset()
        
        self.status.step = 0
        self.status.time = 0.0
        self.status.total_spikes = 0
    
    def log_status(self) -> None:
        """Log current brain status."""
        logger.info(f"Brain Status - Step: {self.status.step}, "
                   f"Time: {self.status.time:.3f}s, "
                   f"Neurons: {self.status.neuron_count}, "
                   f"Synapses: {self.status.synapse_count}, "
                   f"Spikes: {self.status.total_spikes}, "
                   f"Firing: {self.status.firing_neuron_count}")
    
    @property
    def is_initialized(self) -> bool:
        """Check if brain is initialized."""
        return self._is_initialized
    
    @property
    def is_running(self) -> bool:
        """Check if brain is running."""
        return self._is_running
    
    def get_region(self, region_id: int) -> Optional['NeuralRegion']:
        """Get a specific brain region."""
        for region in self.regions:
            if region.region_id == region_id:
                return region
        return None
    
    def get_total_neuron_count(self) -> int:
        """Get total number of neurons."""
        return self.status.neuron_count
    
    def get_total_synapse_count(self) -> int:
        """Get total number of synapses."""
        return self.status.synapse_count
    
    def get_total_spike_count(self) -> int:
        """Get total number of spikes."""
        return self.status.total_spikes
    
    def get_firing_neuron_count(self) -> int:
        """Get number of firing neurons."""
        return self.status.firing_neuron_count
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self.status.avg_firing_rate

class AgentBrain:
    """Bridge between brain and world interaction."""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.world = None
        self._is_initialized = False
        
        # Neuromodulation flags
        self.reward_modulation_enabled = False
        self.structural_plasticity_enabled = False
        self.development_enabled = False
        self.curiosity_enabled = False
        
        # Development tracking
        self.developmental_stage = DevelopmentalStage.INITIAL
        self.developmental_age = 0.0
    
    def initialize(self, world: World) -> None:
        """Initialize agent brain with world."""
        self.world = world
        self._is_initialized = True
    
    def process_sensory_input(self, percept: SensoryPercept) -> None:
        """Process sensory input from world."""
        if not self._is_initialized:
            raise NLMError("AgentBrain not initialized. Call initialize() first.")
        
        # Inject sensory input into brain
        if percept:
            # Convert percept to brain input
            for i, row in enumerate(percept.vision):
                for j, value in enumerate(row):
                    if value > 0.5:  # Only significant vision
                        # Inject current into sensory neurons
                        if self.brain:
                            self.brain.inject_current_to_neurons(
                                NeuronType.SENSORIAL, value * 20.0)
    
    def decode_motor_command(self) -> MotorCommand:
        """Decode motor command from brain activity."""
        if not self._is_initialized:
            raise NLMError("AgentBrain not initialized. Call initialize() first.")
        
        # Simple decision making based on brain state
        if self.brain:
            firing_count = self.brain.get_firing_neuron_count()
            total_neurons = self.brain.get_total_neuron_count()
            
            if firing_count > total_neurons * 0.3:
                # High activity - move forward
                return MotorCommand.MOVE_FORWARD
            elif firing_count < total_neurons * 0.1:
                # Low activity - wait
                return MotorCommand.WAIT
            else:
                # Medium activity - turn
                return MotorCommand.TURN_LEFT if (int(self.brain.get_total_spike_count()) % 2) == 0 else MotorCommand.TURN_RIGHT
        
        return MotorCommand.WAIT
    
    def apply_reward_modulation(self, reward: float, learning_rate: float = 0.01) -> None:
        """Apply reward modulation to brain."""
        if not self._is_initialized:
            raise NLMError("AgentBrain not initialized. Call initialize() first.")
        
        # Update neuromodulation
        if self.brain and hasattr(self.brain, 'neuromodulation'):
            self.brain.neuromodulation.apply_reward(reward, learning_rate)
    
    def update_development(self, dt: float) -> None:
        """Update developmental state."""
        if not self._is_initialized:
            raise NLMError("AgentBrain not initialized. Call initialize() first.")
        
        self.developmental_age += dt
        
        # Update developmental stage based on age
        if self.developmental_age < 10.0:
            self.developmental_stage = DevelopmentalStage.INITIAL
        elif self.developmental_age < 50.0:
            self.developmental_stage = DevelopmentalStage.CRITICAL_PERIOD
        elif self.developmental_age < 100.0:
            self.developmental_stage = DevelopmentalStage.MATURATION
        elif self.developmental_age < 200.0:
            self.developmental_stage = DevelopmentalStage.ADULT
        else:
            self.developmental_stage = DevelopmentalStage.AGING
        
        # Enable development if requested
        if self.development_enabled:
            if self.brain and hasattr(self.brain, 'neuromodulation'):
                self.brain.neuromodulation.update_development(self.developmental_stage)
    
    def reset(self) -> None:
        """Reset agent brain."""
        if self.world:
            self.world.reset()
        if self.brain:
            self.brain.reset()
        
        self.developmental_age = 0.0
        self.developmental_stage = DevelopmentalStage.INITIAL
    
    def get_sensory_input_size(self) -> int:
        """Get size of sensory input."""
        if self.world:
            return self.world.vision_width * self.world.vision_height
        return 0
    
    def get_motor_output_size(self) -> int:
        """Get size of motor output."""
        return len(MotorCommand)
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        if self.brain and hasattr(self.brain, 'neuromodulation'):
            return self.brain.neuromodulation.get_dopamine_level()
        return 0.0
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        if self.brain and hasattr(self.brain, 'neuromodulation'):
            return self.brain.neuromodulation.get_novelty_level()
        return 0.0
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        if self.brain and hasattr(self.brain, 'neuromodulation'):
            return self.brain.neuromodulation.get_curiosity_level()
        return 0.0
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
        if self.brain and hasattr(self.brain, 'neuromodulation'):
            return self.brain.neuromodulation.get_prediction_error()
        return 0.0
    
    def getBrain(self) -> Brain:
        """Get the brain instance."""
        return self.brain

# Fallback implementation classes
class NeuronType(Enum):
    SENSORIAL = 0
    EXCITATORY = 1
    INHIBITORY = 2

class Neuron:
    """Simple neuron for fallback implementation."""
    
    def __init__(self, neuron_id: int, neuron_type: NeuronType = NeuronType.EXCITATORY):
        self.neuron_id = neuron_id
        self.neuron_type = neuron_type
        self.membrane_potential = 0.0
        self.threshold = 20.0
        self.rest_potential = 0.0
        self.spike_count = 0
        self.is_firing = False
        self.synaptic_inputs = []
        self.synaptic_outputs = []
        self.time_constant = 0.02
    
    def inject_current(self, current: float) -> None:
        """Inject current into neuron."""
        self.membrane_potential += current * self.time_constant
    
    def receive_spike(self, delay: float, weight: float) -> None:
        """Receive spike from connected neuron."""
        self.inject_current(weight)
    
    def update_dynamics(self, dt: float) -> None:
        """Update neuron dynamics."""
        # Simple LIF dynamics
        self.membrane_potential *= (1.0 - dt / self.time_constant)
        
        if self.membrane_potential >= self.threshold:
            self.spike_count += 1
            self.is_firing = True
            self.membrane_potential = self.rest_potential
    
    def is_spiking(self) -> bool:
        """Check if neuron is spiking."""
        return self.is_firing
    
    def clear_spike(self) -> None:
        """Clear spike state."""
        self.is_firing = False
    
    def get_output_synapses(self) -> List['Synapse']:
        """Get output synapses."""
        return self.synaptic_outputs
    
    def reset(self) -> None:
        """Reset neuron."""
        self.membrane_potential = self.rest_potential
        self.spike_count = 0
        self.is_firing = False

class Synapse:
    """Simple synapse for fallback implementation."""
    
    def __init__(self, source_neuron: Neuron, target_neuron: Neuron, weight: float = 1.0, delay: float = 0.0):
        self.source_neuron = source_neuron
        self.target_neuron = target_neuron
        self.weight = weight
        self.delay = delay
        
        source_neuron.synaptic_outputs.append(self)
        target_neuron.synaptic_inputs.append(self)

class NeuralRegion:
    """Neural region for fallback implementation."""
    
    def __init__(self, region_id: int, neurons: List[Neuron]):
        self.region_id = region_id
        self.neurons = neurons
    
    def getSynapses(self) -> List[Synapse]:
        """Get synapses in region."""
        synapses = []
        for neuron in self.neurons:
            synapses.extend(neuron.synaptic_outputs)
            synapses.extend(neuron.synaptic_inputs)
        return list(set(synapses))  # Remove duplicates
    
    def getAllNeurons(self) -> List[Neuron]:
        """Get all neurons in region."""
        return self.neurons

class SpikeSystem:
    """Spike system for fallback implementation."""
    
    def __init__(self):
        self.spike_count = 0
    
    def update(self, neurons: List[Neuron], dt: float) -> None:
        """Update spike system."""
        for neuron in neurons:
            if neuron.is_spiking():
                self.spike_count += 1
    
    def reset(self) -> None:
        """Reset spike system."""
        self.spike_count = 0

class MemorySystem:
    """Memory system for fallback implementation."""
    
    def __init__(self):
        self.working_memory = {}
        self.episodic_memory = []
    
    def update(self) -> None:
        """Update memory system."""
        # Simple update - could be enhanced
        pass
    
    def store(self, key: str, value: Any) -> None:
        """Store value in working memory."""
        self.working_memory[key] = value
    
    def recall(self, key: str) -> Any:
        """Recall value from working memory."""
        return self.working_memory.get(key)

class NeuromodulationSystem:
    """Neuromodulation system for fallback implementation."""
    
    def __init__(self):
        self.dopamine_level = 0.0
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        self.prediction_error = 0.0
    
    def update(self) -> None:
        """Update neuromodulation system."""
        # Simple update - could be enhanced
        self.dopamine_level *= 0.9
        self.curiosity_level *= 0.9
        self.novelty_level *= 0.9
    
    def apply_reward(self, reward: float, learning_rate: float) -> None:
        """Apply reward to neuromodulation."""
        self.dopamine_level += reward * learning_rate
        self.dopamine_level = max(0.0, min(1.0, self.dopamine_level))
    
    def update_development(self, stage: DevelopmentalStage) -> None:
        """Update development."""
        if stage == DevelopmentalStage.CRITICAL_PERIOD:
            # Increase plasticity during critical period
            pass
    
    def get_dopamine_level(self) -> float:
        """Get dopamine level."""
        return self.dopamine_level
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self.curiosity_level
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self.novelty_level
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self.prediction_error

# Additional NLM components for completeness
class Network:
    """Complete NLM network interface."""
    
    def __init__(self, config: Optional[NLMConfig] = None):
        self.config = config or NLMConfig()
        self.brain = Brain(self.config)
        self.world = None
        self.agent_brain = None
    
    def create_brain(self) -> Brain:
        """Create a brain with current configuration."""
        if self.brain:
            return self.brain
        self.brain = Brain(self.config)
        return self.brain
    
    def create_world(self, width: float, height: float) -> World:
        """Create a world with given dimensions."""
        self.world = World(width, height)
        return self.world
    
    def create_agent_brain(self) -> AgentBrain:
        """Create an agent brain."""
        if not self.brain:
            self.create_brain()
        self.agent_brain = AgentBrain(self.brain)
        return self.agent_brain
    
    def run(self, steps: int) -> Dict[str, Any]:
        """Run the complete NLM simulation."""
        if not self.brain or not self.agent_brain or not self.world:
            raise NLMError("Network not fully configured. Create brain, world, and agent_brain first.")
        
        results = {
            'steps_completed': 0,
            'total_reward': 0.0,
            'final_brain_status': self.brain.status.to_dict()
        }
        
        for step in range(steps):
            try:
                # Get sensory input
                percept = self.world.get_sensory_percept()
                
                # Process sensory input
                self.agent_brain.process_sensory_input(percept)
                
                # Step brain
                self.brain.step(step, step * self.config.simulation_timestep)
                
                # Decode and apply motor command
                command = self.agent_brain.decode_motor_command()
                action_result = self.world.apply_motor_command(command)
                
                # Apply reward modulation
                self.agent_brain.apply_reward_modulation(action_result.reward)
                
                # Update world and development
                self.world.update(self.config.simulation_timestep)
                self.agent_brain.update_development(self.config.simulation_timestep)
                
                # Track results
                results['steps_completed'] += 1
                results['total_reward'] += action_result.reward
                
                # Log progress periodically
                if step % 100 == 0:
                    logger.info(f"Step {step}: Reward = {action_result.reward:.2f}, "
                               f"Total = {results['total_reward']:.2f}")
                
            except Exception as e:
                logger.error(f"Error at step {step}: {e}")
                break
        
        return results

# Main API functions
def create_brain(config: Optional[NLMConfig] = None) -> Brain:
    """Create a new NLM brain."""
    return Brain(config)

def create_world(width: float, height: float, random_seed: int = 42) -> World:
    """Create a new world."""
    return World(width, height, random_seed=random_seed)

def create_agent_brain(brain: Brain, world: World) -> AgentBrain:
    """Create an agent brain interface."""
    agent_brain = AgentBrain(brain)
    agent_brain.initialize(world)
    return agent_brain

def run_simulation(config: Optional[NLMConfig] = None, 
                  world_width: float = 20.0,
                  world_height: float = 20.0,
                  steps: int = 1000) -> Dict[str, Any]:
    """Run a complete NLM simulation."""
    brain = create_brain(config)
    world = create_world(world_width, world_height)
    agent_brain = create_agent_brain(brain, world)
    
    # Initialize
    brain.initialize()
    
    # Run simulation
    return {
        'brain': brain,
        'world': world,
        'agent_brain': agent_brain,
        'results': brain.run_simulation(steps)
    }

# Module information
def get_version() -> str:
    """Get NLM Python API version."""
    return __version__

def get_info() -> Dict[str, Any]:
    """Get NLM Python API information."""
    return {
        'version': __version__,
        'author': __author__,
        'email': __email__,
        'description': 'NLM Python API - Enhanced Python interface for NLM',
        'features': [
            'Brain simulation',
            'World interaction',
            'Agent-brain interface',
            'Neuromodulation',
            'Memory systems',
            'Cognition',
            'Development',
            'Configuration management'
        ]
    }

# Convenience aliases
BrainNetwork = Network  # Alias for backward compatibility
PynlmBrain = Brain     # Legacy alias
PynlmWorld = World     # Legacy alias
PynlmAgentBrain = AgentBrain  # Legacy alias

# Print module information when imported
if __name__ != '__main__':
    print(f"NLM Python API version {__version__} loaded successfully")
    print("For documentation, visit: https://github.com/nlm-project/pynlm")
