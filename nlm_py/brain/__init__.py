"""
NLM Brain Python Module

This module contains the brain core classes for the NLM Python bindings.
"""

import ctypes
import numpy as np
from typing import Optional, List, Dict, Any, Tuple, Union
from .exceptions import NLMError, BrainError, NeuronError, SynapseError
from .core import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SynapseType
from .core import DevelopmentalStage, FiringState, ActionType, MotorCommand
from .core import WorldObjectType, SimulationStep, Timestamp, TimestepDuration
from .core import NeuronIndex, SynapseIndex, PopulationIndex, RegionIndex
from .core import SynapticWeight, MembranePotential, FiringRate, Delay

# Import the actual bindings if available
try:
    from ..bindings import pynlm
    _bindings_available = True
except ImportError:
    _bindings_available = False
    pynlm = None

class Config:
    """Configuration for NLM brain simulation."""
    
    def __init__(self):
        self._config_ptr = None
        if _bindings_available:
            try:
                self._config_ptr = pynlm.create_default_config()
            except Exception:
                self._config_ptr = None
        else:
            # Python-only implementation
            self._data = {}
    
    def load_from_file(self, filepath: str):
        """Load configuration from a JSON file."""
        if _bindings_available and self._config_ptr:
            self._config_ptr.loadFromFile(filepath)
        else:
            import json
            with open(filepath, 'r') as f:
                self._data.update(json.load(f))
    
    def load_from_args(self, argc: int, argv: List[str]):
        """Load configuration from command line arguments."""
        if _bindings_available and self._config_ptr:
            # Convert to C-style argv
            c_argv = [ctypes.c_char_p(arg.encode('utf-8')) for arg in argv]
            self._config_ptr.loadFromArgs(argc, c_argv)
        else:
            # Simple argument parsing
            for i in range(0, len(argv), 2):
                if i + 1 < len(argv):
                    key = argv[i]
                    value = argv[i + 1]
                    self._data[key] = value
    
    def save_to_file(self, filepath: str):
        """Save configuration to a JSON file."""
        if _bindings_available and self._config_ptr:
            self._config_ptr.saveToFile(filepath)
        else:
            import json
            with open(filepath, 'w') as f:
                json.dump(self._data, f, indent=2)
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        if _bindings_available and self._config_ptr:
            return self._config_ptr.has(key)
        else:
            return key in self._data
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        if _bindings_available and self._config_ptr:
            return self._config_ptr.getKeys()
        else:
            return list(self._data.keys())
    
    def clear(self):
        """Clear all configuration entries."""
        if _bindings_available and self._config_ptr:
            self._config_ptr.clear()
        else:
            self._data.clear()
    
    def summary(self) -> str:
        """Get a summary string of the configuration."""
        if _bindings_available and self._config_ptr:
            return self._config_ptr.summary()
        else:
            return f"Config with {len(self._data)} keys: {list(self._data.keys())}"
    
    def __repr__(self):
        return f"<Config: {self.summary()}>"


class SensoryInput:
    """Base class for sensory input."""
    
    def __init__(self):
        self._sensory_ptr = None
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                self._sensory_ptr = None
    
    def get_type(self):
        """Get the type of sensory input."""
        if _bindings_available and self._sensory_ptr:
            return self._sensory_ptr.getType()
        return "Unknown"
    
    def get_data(self) -> List[float]:
        """Get the raw data as a list."""
        if _bindings_available and self._sensory_ptr:
            return self._sensory_ptr.getData()
        return []
    
    def get_dimensions(self) -> int:
        """Get the dimensionality."""
        if _bindings_available and self._sensory_ptr:
            return self._sensory_ptr.getDimensions()
        return 0
    
    def get_timestamp(self) -> Timestamp:
        """Get the timestamp."""
        if _bindings_available and self._sensory_ptr:
            return self._sensory_ptr.getTimestamp()
        return 0.0
    
    def set_timestamp(self, timestamp: Timestamp):
        """Set the timestamp."""
        if _bindings_available and self._sensory_ptr:
            self._sensory_ptr.setTimestamp(timestamp)


class Vision(SensoryInput):
    """Vision sensory input."""
    
    def __init__(self, width: size_t = 0, height: size_t = 0, channels: size_t = 3):
        super().__init__()
        self.width = width
        self.height = height
        self.channels = channels
        self.data = np.zeros((height, width, channels), dtype=np.float32)
    
    def set_data(self, data: np.ndarray):
        """Set the data as a numpy array."""
        if data.shape == (self.height, self.width, self.channels):
            self.data = data
        else:
            raise ValueError("Data shape doesn't match vision dimensions")
    
    def get_width(self) -> size_t:
        """Get the width."""
        return self.width
    
    def get_height(self) -> size_t:
        """Get the height."""
        return self.height
    
    def get_channels(self) -> size_t:
        """Get the number of channels."""
        return self.channels


class Audio(SensoryInput):
    """Audio sensory input."""
    
    def __init__(self, sample_rate: size_t = 0, num_samples: size_t = 0):
        super().__init__()
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self.data = np.zeros(num_samples, dtype=np.float32)
    
    def set_data(self, data: np.ndarray):
        """Set the data as a numpy array."""
        if data.shape == (self.num_samples,):
            self.data = data
        else:
            raise ValueError("Data shape doesn't match audio dimensions")
    
    def set_sample_rate(self, sample_rate: size_t):
        """Set the sample rate."""
        self.sample_rate = sample_rate
    
    def get_sample_rate(self) -> size_t:
        """Get the sample rate."""
        return self.sample_rate
    
    def get_num_samples(self) -> size_t:
        """Get the number of samples."""
        return self.num_samples


class Action:
    """Action representation for motor output."""
    
    def __init__(self, action_type: ActionType = ActionType.Wait, parameters: Optional[List[float]] = None):
        self.action_type = action_type
        self.parameters = parameters or []
    
    def get_type(self) -> ActionType:
        """Get the action type."""
        return self.action_type
    
    def set_type(self, action_type: ActionType):
        """Set the action type."""
        self.action_type = action_type
    
    def get_parameters(self) -> List[float]:
        """Get the parameters."""
        return self.parameters
    
    def set_parameters(self, parameters: List[float]):
        """Set the parameters."""
        self.parameters = parameters
    
    def get_name(self) -> str:
        """Get the action name."""
        return self.action_type.name if hasattr(self.action_type, 'name') else str(self.action_type)
    
    def clone(self) -> 'Action':
        """Clone the action."""
        return Action(self.action_type, self.parameters.copy())


class WorldObject:
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: WorldObjectType = WorldObjectType.Empty,
                 value: float = 0.0, radius: float = 0.5):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True


class AgentBody:
    """Agent body state."""
    
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 1.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
    
    def reset(self):
        """Reset the agent body to initial state."""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 1.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0


class ActionResult:
    """Action result from world."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        self.reward = reward
        self.success = success
        self.message = message


class SensoryPercept:
    """Sensory percept data."""
    
    def __init__(self):
        self.vision = Vision()
        self.touch = np.zeros(10, dtype=np.float32)  # Placeholder for touch sensors
        self.internal = np.zeros(5, dtype=np.float32)  # Placeholder for internal signals
        self.proprioception = np.zeros(5, dtype=np.float32)  # Placeholder for proprioception
        self.audio = Audio()
        self.timestamp = 0.0
    
    def get_vision(self) -> Vision:
        """Get vision data."""
        return self.vision
    
    def set_vision(self, vision: Vision):
        """Set vision data."""
        self.vision = vision
    
    def get_vision_width(self) -> size_t:
        """Get vision width."""
        return self.vision.get_width()
    
    def get_vision_height(self) -> size_t:
        """Get vision height."""
        return self.vision.get_height()
    
    def get_touch(self) -> np.ndarray:
        """Get touch data."""
        return self.touch
    
    def set_touch(self, touch: np.ndarray):
        """Set touch data."""
        if touch.shape == (10,):
            self.touch = touch
        else:
            raise ValueError("Touch data must have shape (10,)")
    
    def get_internal(self) -> np.ndarray:
        """Get internal data."""
        return self.internal
    
    def set_internal(self, internal: np.ndarray):
        """Set internal data."""
        if internal.shape == (5,):
            self.internal = internal
        else:
            raise ValueError("Internal data must have shape (5,)")
    
    def get_proprioception(self) -> np.ndarray:
        """Get proprioception data."""
        return self.proprioception
    
    def set_proprioception(self, proprioception: np.ndarray):
        """Set proprioception data."""
        if proprioception.shape == (5,):
            self.proprioception = proprioception
        else:
            raise ValueError("Proprioception data must have shape (5,)")
    
    def get_audio(self) -> Audio:
        """Get audio data."""
        return self.audio
    
    def set_audio(self, audio: Audio):
        """Set audio data."""
        self.audio = audio
    
    def get_all_signals(self) -> Dict[str, Any]:
        """Get all signals as a dictionary."""
        return {
            'vision': self.vision,
            'touch': self.touch,
            'internal': self.internal,
            'proprioception': self.proprioception,
            'audio': self.audio,
            'timestamp': self.timestamp
        }
    
    def get_timestamp(self) -> Timestamp:
        """Get the timestamp."""
        return self.timestamp
    
    def set_timestamp(self, timestamp: Timestamp):
        """Set the timestamp."""
        self.timestamp = timestamp


class SimpleWorld:
    """Simple 2D world for NLM simulation."""
    
    def __init__(self):
        self.width = 100
        self.height = 100
        self.vision_width = 8
        self.vision_height = 8
        self.max_energy = 100.0
        self.energy_decay_rate = 0.01
        self.simulation_time = 0.0
        self.random_seed = 42
        
        # Agent state
        self.agent_body = AgentBody()
        
        # World objects
        self.objects = []
        
        # Sensory percept
        self.percept = SensoryPercept()
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure the world dimensions."""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        self.percept.vision = Vision(vision_width, vision_height)
    
    def reset(self):
        """Reset the world to initial state."""
        self.agent_body = AgentBody()
        self.objects = []
        self.percept = SensoryPercept()
        self.percept.vision = Vision(self.vision_width, self.vision_height)
        self.simulation_time = 0.0
    
    def set_agent_start(self, x: float, y: float):
        """Set the agent starting position."""
        self.agent_body.x = x
        self.agent_body.y = y
    
    def update(self, timestep: float):
        """Update the world by a timestep."""
        self.simulation_time += timestep
        
        # Simple world physics
        self.agent_body.energy *= (1 - self.energy_decay_rate)
        self.agent_body.age += int(timestep)
        
        # Update agent body position (placeholder for actual physics)
        self.agent_body.x += self.agent_body.velocityX * timestep
        self.agent_body.y += self.agent_body.velocityY * timestep
        self.agent_body.orientation += self.agent_body.angularVelocity * timestep
        
        # Wrap around world boundaries
        self.agent_body.x %= self.width
        self.agent_body.y %= self.height
    
    def apply_motor_command(self, action: Action, current_time: Timestamp):
        """Apply a motor command to the world."""
        self.agent_body.lastActionTime = current_time
        self.agent_body.isMoving = (action.action_type != ActionType.Wait and 
                                   action.action_type != ActionType.Rest)
        self.agent_body.isTurning = (action.action_type in [ActionType.TurnLeft, ActionType.TurnRight])
        
        # Convert action to movement (simplified)
        if action.action_type == ActionType.MoveForward:
            angle = self.agent_body.orientation
            self.agent_body.velocityX += np.cos(angle) * 0.5
            self.agent_body.velocityY += np.sin(angle) * 0.5
        elif action.action_type == ActionType.MoveBackward:
            angle = self.agent_body.orientation
            self.agent_body.velocityX -= np.cos(angle) * 0.5
            self.agent_body.velocityY -= np.sin(angle) * 0.5
        elif action.action_type == ActionType.TurnLeft:
            self.agent_body.angularVelocity -= 0.1
        elif action.action_type == ActionType.TurnRight:
            self.agent_body.angularVelocity += 0.1
        elif action.action_type == ActionType.Look:
            # Look action - update vision direction (placeholder)
            pass
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get the sensory percept."""
        return self.percept
    
    def get_agent_body(self) -> AgentBody:
        """Get the agent body."""
        return self.agent_body
    
    def add_object(self, obj: WorldObject):
        """Add a world object."""
        self.objects.append(obj)
    
    def remove_object(self, x: float, y: float):
        """Remove a world object at position (x, y)."""
        self.objects = [obj for obj in self.objects if not (abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1)]
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if a position is valid."""
        for obj in self.objects:
            if abs(obj.x - x) < obj.radius and abs(obj.y - y) < obj.radius:
                return False
        return 0 <= x < self.width and 0 <= y < self.height
    
    def get_width(self) -> int:
        """Get the world width."""
        return self.width
    
    def get_height(self) -> int:
        """Get the world height."""
        return self.height
    
    def get_max_energy(self) -> float:
        """Get the maximum energy."""
        return self.max_energy
    
    def set_max_energy(self, e: float):
        """Set the maximum energy."""
        self.max_energy = e
    
    def get_energy_decay_rate(self) -> float:
        """Get the energy decay rate."""
        return self.energy_decay_rate
    
    def set_energy_decay_rate(self, r: float):
        """Set the energy decay rate."""
        self.energy_decay_rate = r
    
    def get_simulation_time(self) -> float:
        """Get the simulation time."""
        return self.simulation_time
    
    def set_random_seed(self, seed: int):
        """Set the random seed."""
        self.random_seed = seed
        # In a real implementation, this would set the RNG seed
    
    def get_random_seed(self) -> int:
        """Get the random seed."""
        return self.random_seed


# Brain class will be defined separately due to its complexity
# For now, create a placeholder that can be properly implemented later
class Brain:
    """Central neural simulation brain class."""
    
    def __init__(self, config: Optional[Config] = None):
        if config is None:
            config = Config()
        self.config = config
        self._brain_ptr = None
        
        # Python fallback implementation
        self.regions = []
        self.total_spike_count = 0
        self.active_neuron_count = 0
        self.firing_neuron_count = 0
        self.developmental_stage = DevelopmentalStage.Initial
        
        # Initialize with default values
        self._initialize_default()
    
    def _initialize_default(self):
        """Initialize brain with default values."""
        # Create a default region
        region_id = RegionId(0)
        region = NeuralRegion(region_id, "Default Region")
        
        # Create a population
        pop_id = PopulationId(0)
        population = NeuralPopulation(pop_id, 100)
        
        # Add to region
        region.add_population(population)
        self.regions.append(region)
    
    def initialize(self):
        """Initialize the brain with configuration."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.initialize()
        
        # Python fallback initialization
        for region in self.regions:
            region.initialize_random(self.config.get('random_seed', 42), 0.1, 0.0, 0.1)
        
        return True
    
    def step(self, current_step: SimulationStep, current_time: Timestamp = 0.0):
        """Perform a simulation step."""
        if _bindings_available and self._brain_ptr:
            if current_time > 0:
                return self._brain_ptr.step(current_step, current_time)
            else:
                return self._brain_ptr.step(current_step)
        
        # Python fallback implementation
        for region in self.regions:
            region.step(current_time)
        
        # Update statistics
        self.total_spike_count = sum(
            neuron.getState().lastSpikeTime >= 0 for region in self.regions 
            for population in region.getPopulations() 
            for neuron in population.getNeurons()
        )
        
        self.active_neuron_count = sum(
            1 for region in self.regions 
            for population in region.getPopulations() 
            for neuron in population.getNeurons()
            if neuron.isFiring()
        )
        
        self.firing_neuron_count = sum(
            1 for region in self.regions 
            for population in region.getPopulations() 
            for neuron in population.getNeurons()
            if neuron.isFiring()
        )
    
    def receive_sensory_input(self, input_data):
        """Inject sensory input into the brain."""
        if _bindings_available and self._brain_ptr:
            # Convert to appropriate C++ type
            if hasattr(input_data, 'getData'):
                data = input_data.getData()
                # This would need proper type conversion
                pass
    
    def inject_current(self, neuron_id: NeuronId, current: float):
        """Inject current into a specific neuron."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.injectCurrent(neuron_id, current)
    
    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float):
        """Inject current into all neurons of a specific type."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.injectCurrentToNeurons(neuron_type, current)
    
    def produce_action(self) -> Action:
        """Produce motor action based on neural activity."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.produceAction()
        
        # Python fallback - simple action based on firing rate
        firing_rate = self.get_average_firing_rate()
        if firing_rate > 10:
            return Action(ActionType.MoveForward)
        elif firing_rate > 5:
            return Action(ActionType.Wait)
        else:
            return Action(ActionType.Rest)
    
    def reset(self):
        """Reset brain state."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.reset()
        
        # Python fallback
        for region in self.regions:
            region.reset()
        self.total_spike_count = 0
        self.active_neuron_count = 0
        self.firing_neuron_count = 0
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.save(filepath)
        
        # Python fallback - save as JSON
        import json
        data = {
            'total_spike_count': self.total_spike_count,
            'active_neuron_count': self.active_neuron_count,
            'firing_neuron_count': self.firing_neuron_count,
            'developmental_stage': self.developmental_stage.value,
            'regions': []
        }
        
        for region in self.regions:
            region_data = {
                'id': region.getId().value,
                'name': region.getName(),
                'populations': []
            }
            
            for population in region.getPopulations():
                pop_data = {
                    'id': population.getId().value,
                    'neuron_type': population.getNeuronType().value,
                    'size': population.getSize()
                }
                region_data['populations'].append(pop_data)
            
            data['regions'].append(region_data)
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        return True
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.load(filepath)
        
        # Python fallback - load from JSON
        import json
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.total_spike_count = data.get('total_spike_count', 0)
        self.active_neuron_count = data.get('active_neuron_count', 0)
        self.firing_neuron_count = data.get('firing_neuron_count', 0)
        self.developmental_stage = DevelopmentalStage(data.get('developmental_stage', 0))
        
        # Clear existing regions
        self.regions = []
        
        # Load regions
        for region_data in data.get('regions', []):
            region_id = RegionId(region_data['id'])
            region = NeuralRegion(region_id, region_data['name'])
            
            # In a full implementation, we would reconstruct populations and neurons
            # For now, create empty regions
            self.regions.append(region)
        
        return True
    
    def add_region(self, name: str = "") -> RegionId:
        """Add a new neural region."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.addRegion(name)
        
        region_id = RegionId(len(self.regions))
        region = NeuralRegion(region_id, name)
        self.regions.append(region)
        return region_id
    
    def get_region(self, region_id: RegionId) -> 'NeuralRegion':
        """Get a region by ID."""
        if _bindings_available and self._brain_ptr:
            region_ptr = self._brain_ptr.getRegion(region_id)
            # Convert to Python object (would need proper wrapper)
            pass
        
        for region in self.regions:
            if region.getId() == region_id:
                return region
        
        raise BrainError(f"Region not found: {region_id}")
    
    def get_region_count(self) -> int:
        """Get the number of regions."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getRegionCount()
        return len(self.regions)
    
    def get_region_ids(self) -> List[RegionId]:
        """Get all region IDs."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getRegionIds()
        return [region.getId() for region in self.regions]
    
    def get_regions(self) -> List['NeuralRegion']:
        """Get all regions."""
        if _bindings_available and self._brain_ptr:
            region_ptrs = self._brain_ptr.getRegions()
            # Convert to Python objects (would need proper wrapper)
            pass
        return self.regions
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count across all regions."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getTotalNeuronCount()
        return sum(region.getTotalNeuronCount() for region in self.regions)
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count across all regions."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getTotalSynapseCount()
        return sum(
            sum(pop.getNeuronCount() for pop in region.getPopulations()) * 5
            for region in self.regions
        )
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getActiveNeuronCount()
        return self.active_neuron_count
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getFiringNeuronCount()
        return self.firing_neuron_count
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getAverageFiringRate()
        
        total_spikes = self.total_spike_count
        neurons = self.get_total_neuron_count()
        if neurons == 0:
            return 0.0
        return total_spikes / neurons
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getExcitationInhibitionRatio()
        return 1.0  # Default balanced
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getTotalSpikeCount()
        return self.total_spike_count
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.getDevelopmentalStage()
        return self.developmental_stage
    
    def set_developmental_stage(self, stage: DevelopmentalStage):
        """Set developmental stage."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.setDevelopmentalStage(stage)
        self.developmental_stage = stage
    
    def get_config(self) -> Config:
        """Get the configuration."""
        if _bindings_available and self._brain_ptr:
            config_ptr = self._brain_ptr.getConfig()
            # Convert to Python object (would need proper wrapper)
            pass
        return self.config
    
    def log_status(self):
        """Log brain status."""
        if _bindings_available and self._brain_ptr:
            return self._brain_ptr.logStatus()
        
        # Python fallback logging
        import logging
        logger = logging.getLogger(__name__)
        logger.info(f"Brain Status: {self.get_total_neuron_count()} neurons, "
                   f"{self.get_total_synapse_count()} synapses, "
                   f"{self.get_active_neuron_count()} active, "
                   f"{self.get_firing_neuron_count()} firing, "
                   f"rate: {self.get_average_firing_rate():.2f} Hz, "
                   f"stage: {self.developmental_stage}")


class AgentBrain:
    """Agent brain interface connecting NLM brain to world."""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self._agent_brain_ptr = None
        
        if _bindings_available:
            try:
                self._agent_brain_ptr = pynlm.createAgentBrain(brain._brain_ptr if brain._brain_ptr else None)
            except Exception:
                self._agent_brain_ptr = None
    
    def initialize(self, world: SimpleWorld):
        """Initialize with world."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.initialize(world._world_ptr if hasattr(world, '_world_ptr') else None)
        
        # Python fallback initialization
        self.world = world
        return True
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.getSensoryInputSize()
        
        # Python fallback - based on world dimensions
        return self.world.get_vision_width() * self.world.get_vision_height() * 3
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.getMotorOutputSize()
        
        return 10  # Default motor output size
    
    def process_sensory_input(self, percept: SensoryPercept):
        """Process sensory percept and inject into brain."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.processSensoryInput(percept._percept_ptr if hasattr(percept, '_percept_ptr') else None)
        
        # Python fallback - convert percept to brain input
        self.brain.receive_sensory_input(percept)
    
    def decode_motor_command(self) -> Action:
        """Decode brain motor activity into motor command."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.decodeMotorCommand()
        
        # Python fallback - simple decoding
        return self.brain.produce_action()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.applyRewardModulation(reward, predicted_reward)
        
        # Python fallback - modify brain behavior based on reward
        pass
    
    def update_development(self, timestep: float):
        """Update development system."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.updateDevelopment(timestep)
        
        # Python fallback - update developmental stage
        stage = self.brain.get_developmental_stage()
        if stage == DevelopmentalStage.Initial and timestep > 100:
            self.brain.set_developmental_stage(DevelopmentalStage.CriticalPeriod)
        elif stage == DevelopmentalStage.CriticalPeriod and timestep > 200:
            self.brain.set_developmental_stage(DevelopmentalStage.Maturation)
        elif stage == DevelopmentalStage.Maturation and timestep > 300:
            self.brain.set_developmental_stage(DevelopmentalStage.Adult)
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.getDevelopmentalStage()
        return self.brain.get_developmental_stage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.getNeuromodulationLevel()
        return 0.5  # Default
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.getCuriosityLevel()
        return 0.5  # Default
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.getNoveltyLevel()
        return 0.5  # Default
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.getPredictionError()
        return 0.0  # Default
    
    def reset(self):
        """Reset agent for new episode."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.reset()
        
        self.brain.reset()
    
    def get_brain(self) -> Brain:
        """Get the underlying brain."""
        return self.brain
    
    def enable_reward_modulation(self, enable: bool):
        """Enable reward modulation."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool):
        """Enable structural plasticity."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool):
        """Enable development."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool):
        """Enable curiosity."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.enableCuriosity(enable)
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.isRewardModulationEnabled()
        return False
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.isStructuralPlasticityEnabled()
        return False
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.isDevelopmentEnabled()
        return False
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        if _bindings_available and self._agent_brain_ptr:
            return self._agent_brain_ptr.isCuriosityEnabled()
        return False


# Convenience functions
def run_simulation(brain: Brain, world: SimpleWorld, agent: AgentBrain, num_steps: int):
    """Run a simulation loop."""
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.get_sensory_percept()
        
        # Tell the brain
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decode_motor_command()
        
        # Do action in world
        world.apply_motor_command(action, world.get_simulation_time())


def format_spike_statistics(brain: Brain) -> str:
    """Format spike statistics for display."""
    return (f"Spikes: {brain.get_total_spike_count()}, "
            f"Active: {brain.get_active_neuron_count()}, "
            f"Firing: {brain.get_firing_neuron_count()}, "
            f"Rate: {brain.get_average_firing_rate():.2f} Hz")


def format_memory_stats(brain: Brain) -> str:
    """Format memory statistics for display."""
    # This would need actual memory system access
    return "Memory statistics not available in Python fallback"


def format_prediction_stats(brain: Brain) -> str:
    """Format prediction statistics for display."""
    # This would need actual prediction system access
    return "Prediction statistics not available in Python fallback"


def create_custom_config() -> Config:
    """Create a custom configuration object."""
    return Config()


def create_neural_config() -> Config:
    """Create a neural network configuration."""
    config = Config()
    # Set some default neural parameters
    return config


def create_agent_config() -> Config:
    """Create an agent configuration."""
    config = Config()
    # Set some default agent parameters
    return config


# Factory functions
def create_default_config() -> Config:
    """Create a default configuration object."""
    return Config()


def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with the given configuration."""
    return Brain(config)


def create_simple_world() -> SimpleWorld:
    """Create a new SimpleWorld simulation environment."""
    return SimpleWorld()


def create_agent_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface for a brain object."""
    return AgentBrain(brain)


# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Export public interface
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain',
    
    # Sensory classes
    'SensoryInput', 'Vision', 'Audio',
    
    # Motor classes
    'Action',
    
    # World classes
    'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept', 'SimpleWorld',
    
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Numeric types
    'SimulationStep', 'Timestamp', 'TimestepDuration',
    'NeuronIndex', 'SynapseIndex', 'PopulationIndex', 'RegionIndex',
    'SynapticWeight', 'MembranePotential', 'FiringRate', 'Delay',
    
    # Convenience functions
    'run_simulation', 'format_spike_statistics', 'format_memory_stats',
    'format_prediction_stats', 'create_custom_config', 'create_neural_config',
    'create_agent_config', 'create_default_config', 'create_brain',
    'create_simple_world', 'create_agent_brain',
    
    # Version
    '__version__',
]
