"""Type definitions and enumerations for NLM framework"""

from typing import TypeVar, Generic, List, Dict, Any, Optional
from dataclasses import dataclass
import math

# Type aliases for better readability
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int
SimulationStep = int
Timestamp = float
MembranePotential = float

# Core enumeration types
class NeuronType:
    """Types of neurons in the NLM framework"""
    
    # Cell types
    Excitatory = 0    # Excitatory neurons (positive current)
    Inhibitory = 1     # Inhibitory neurons (negative current)
    Modulatory = 2     # Modulatory neurons (neuromodulatory signals)
    
    # Functional types
    Sensory = 3       # Sensory processing neurons
    Motor = 4         # Motor output neurons
    Internal = 5      # Internal processing neurons
    
    @classmethod
    def get_type_name(cls, neuron_type: int) -> str:
        """Get human-readable name for neuron type"""
        type_names = {
            cls.Excitatory: "Excitatory",
            cls.Inhibitory: "Inhibitory",
            cls.Modulatory: "Modulatory",
            cls.Sensory: "Sensory",
            cls.Motor: "Motor",
            cls.Internal: "Internal",
        }
        return type_names.get(neuron_type, f"Unknown({neuron_type})")
class SynapseType:
    """Types of synapses in the NLM framework"""
    
    # Classical synapse types
    Excitatory = 0     # Glutamatergic excitatory synapses
    Inhibitory = 1     # GABAergic inhibitory synapses
    Modulatory = 2     # Neuromodulatory synapses
    
    # Electrical synapses
    Electrical = 3     # Gap junctions (direct electrical coupling)
    GapJunction = 4    # Alternative name for electrical synapses
    
    @classmethod
    def get_type_name(cls, synapse_type: int) -> str:
        """Get human-readable name for synapse type"""
        type_names = {
            cls.Excitatory: "Excitatory",
            cls.Inhibitory: "Inhibitory",
            cls.Modulatory: "Modulatory",
            cls.Electrical: "Electrical",
            cls.GapJunction: "GapJunction",
        }
        return type_names.get(synapse_type, f"Unknown({synapse_type})")
class ActionType:
    """Types of actions that can be performed"""
    
    # Movement actions
    MoveForward = 0    # Move forward
    MoveBackward = 1   # Move backward
    MoveLeft = 2       # Move left
    MoveRight = 3      # Move right
    TurnLeft = 4       # Rotate left
    TurnRight = 5      # Rotate right
    
    # Gaze actions
    Look = 6           # General look action
    LookUp = 7         # Look up
    LookDown = 8       # Look down
    
    # Interaction actions
    Interact = 9       # Interact with nearby object
    Eat = 10           # Eat/consume object
    Drink = 11         # Drink/consume liquid
    Rest = 12          # Rest/heal
    
    # Behavioral actions
    Wait = 13          # Do nothing
    Custom = 14        # Custom action with parameters
    
    @classmethod
    def get_action_name(cls, action_type: int) -> str:
        """Get human-readable name for action type"""
        type_names = {
            cls.MoveForward: "MoveForward",
            cls.MoveBackward: "MoveBackward",
            cls.MoveLeft: "MoveLeft",
            cls.MoveRight: "MoveRight",
            cls.TurnLeft: "TurnLeft",
            cls.TurnRight: "TurnRight",
            cls.Look: "Look",
            cls.LookUp: "LookUp",
            cls.LookDown: "LookDown",
            cls.Interact: "Interact",
            cls.Eat: "Eat",
            cls.Drink: "Drink",
            cls.Rest: "Rest",
            cls.Wait: "Wait",
            cls.Custom: "Custom",
        }
        return type_names.get(action_type, f"Unknown({action_type})")
class MotorCommand:
    """Low-level motor commands that the brain can produce"""
    
    MoveForward = 0    # Propel forward
    MoveBackward = 1   # Propel backward
    TurnLeft = 2       # Rotate left
    TurnRight = 3      # Rotate right
    LookLeft = 4       # Pan sensor left
    LookRight = 5      # Pan sensor right
    Interact = 6       # Interact with nearby object
    Wait = 7           # No movement
    
    @classmethod
    def get_command_name(cls, command: int) -> str:
        """Get human-readable name for motor command"""
        type_names = {
            cls.MoveForward: "MoveForward",
            cls.MoveBackward: "MoveBackward",
            cls.TurnLeft: "TurnLeft",
            cls.TurnRight: "TurnRight",
            cls.LookLeft: "LookLeft",
            cls.LookRight: "LookRight",
            cls.Interact: "Interact",
            cls.Wait: "Wait",
        }
        return type_names.get(command, f"Unknown({command})")
class WorldObjectType:
    """Types of objects in the world"""
    
    Empty = 0          # Empty space
    Resource = 1       # Positive reward (food, water, etc.)
    Hazard = 2         # Negative reward (danger, damage)
    Wall = 3           # Boundary/obstacle
    Marker = 4         # Navigation aid (goal)
    
    @classmethod
    def get_object_name(cls, obj_type: int) -> str:
        """Get human-readable name for world object type"""
        type_names = {
            cls.Empty: "Empty",
            cls.Resource: "Resource",
            cls.Hazard: "Hazard",
            cls.Wall: "Wall",
            cls.Marker: "Marker",
        }
        return type_names.get(obj_type, f"Unknown({obj_type})")
class DevelopmentalStage:
    """Developmental stages of the brain"""
    
    Initial = 0        # Initial state, high plasticity
    CriticalPeriod = 1 # Sensitive period for learning
    Maturation = 2     # Maturation of cognitive functions
    Adult = 3          # Full adult capabilities
    Aging = 4          # Age-related decline
    
    @classmethod
    def get_stage_name(cls, stage: int) -> str:
        """Get human-readable name for developmental stage"""
        stage_names = {
            cls.Initial: "Initial",
            cls.CriticalPeriod: "CriticalPeriod",
            cls.Maturation: "Maturation",
            cls.Adult: "Adult",
            cls.Aging: "Aging",
        }
        return stage_names.get(stage, f"Unknown({stage})")
class FiringState:
    """States of neuron firing"""
    
    Resting = 0         # Not firing, below threshold
    Active = 1          # Currently firing action potential
    Refractory = 2      # Just fired, cannot fire again
    Inhibited = 3       # Suppressed by inhibition
    
    @classmethod
    def get_state_name(cls, state: int) -> str:
        """Get human-readable name for firing state"""
        state_names = {
            cls.Resting: "Resting",
            cls.Active: "Active",
            cls.Refractory: "Refractory",
            cls.Inhibited: "Inhibited",
        }
        return state_names.get(state, f"Unknown({state})")
class PlasticityFlag:
    """Flags for enabling/disabling plasticity rules"""
    
    # Plasticity rules
    STDP = 0            # Spike-timing dependent plasticity
    Hebbian = 1         # Hebbian co-activation learning
    Anomalous = 2      # Alternative plasticity rule
    
    # Modifiers
    LTP = 3             # Long-term potentiation bias
    LTD = 4             # Long-term depression bias
    
    # Special rules
    Structural = 5      # Structural plasticity (new synapses)
    Homeostatic = 6     # Homeostatic plasticity
    
    @classmethod
    def get_flag_name(cls, flag: int) -> str:
        """Get human-readable name for plasticity flag"""
        flag_names = {
            cls.STDP: "STDP",
            cls.Hebbian: "Hebbian",
            cls.Anomalous: "Anomalous",
            cls.LTP: "LTP",
            cls.LTD: "LTD",
            cls.Structural: "Structural",
            cls.Homeostatic: "Homeostatic",
        }
        return flag_names.get(flag, f"Unknown({flag})")
class PlasticityFlags:
    """Combination of plasticity flags"""
    
    def __init__(self):
        self.STDP = False      # Enable STDP
        self.Hebbian = False   # Enable Hebbian
        self.Anomalous = False  # Enable anomalous
        self.Structural = False # Enable structural
        self.Homeostatic = False # Enable homeostatic
        
    def __str__(self):
        flags = []
        if self.STDP: flags.append("STDP")
        if self.Hebbian: flags.append("Hebbian")
        if self.Anomalous: flags.append("Anomalous")
        if self.Structural: flags.append("Structural")
        if self.Homeostatic: flags.append("Homeostatic")
        return "PlasticityFlags(" + ", ".join(flags) + ")"

# Identity types for neurons, synapses, regions, populations
@dataclass
class NeuronId:
    """Unique identifier for a neuron"""
    index: int
    
    def __hash__(self):
        return hash(self.index)
    
    def __eq__(self, other):
        return isinstance(other, NeuronId) and self.index == other.index
    
    def __repr__(self):
        return f"NeuronId({self.index})"
@dataclass
class SynapseId:
    """Unique identifier for a synapse"""
    index: int
    
    def __hash__(self):
        return hash(self.index)
    
    def __eq__(self, other):
        return isinstance(other, SynapseId) and self.index == other.index
    
    def __repr__(self):
        return f"SynapseId({self.index})"
@dataclass
class RegionId:
    """Unique identifier for a brain region"""
    index: int
    
    def __hash__(self):
        return hash(self.index)
    
    def __eq__(self, other):
        return isinstance(other, RegionId) and self.index == other.index
    
    def __repr__(self):
        return f"RegionId({self.index})"
@dataclass
class PopulationId:
    """Unique identifier for a neuron population"""
    index: int
    
    def __hash__(self):
        return hash(self.index)
    
    def __eq__(self, other):
        return isinstance(other, PopulationId) and self.index == other.index
    
    def __repr__(self):
        return f"PopulationId({self.index})"

# Invalid ID constants for error handling
INVALID_NEURON_ID = NeuronId(-1)
INVALID_SYNAPSE_ID = SynapseId(-1)
INVALID_REGION_ID = RegionId(-1)
INVALID_POPULATION_ID = PopulationId(-1)

# Physics and dynamics constants
@dataclass
class PhysicsConstants:
    """Physical constants for neural simulation"""
    
    # Neuron membrane properties
    resting_potential: float = -65.0    # mV
    threshold_potential: float = -50.0   # mV
    reset_potential: float = -65.0      # mV
    membrane_capacitance: float = 1.0    # nF
    leak_conductance: float = 0.1        # nS
    
    # Synaptic properties
    synaptic_delay: float = 1.0          # ms
    max_synaptic_weight: float = 1.0
    min_synaptic_weight: float = 0.0
    weight_normalization: float = 1.0
    
    # Spike properties
    refractory_period: float = 2.0        # ms
    spike_duration: float = 1.0           # ms
    spike_amplitude: float = 20.0         # mV
    
    # Noise and variability
    noise_std: float = 0.1                # Standard deviation for noise
    variability: float = 0.05             # Level of intrinsic variability
    
    @classmethod
    def get_default_constants(cls) -> 'PhysicsConstants':
        """Get default physics constants"""
        return cls()

# Performance and optimization constants
@dataclass
class PerformanceConstants:
    """Performance optimization constants"""
    
    # SIMD configuration
    enable_simd: bool = True
    simd_width: int = 4                     # Vector width in floats
    
    # Parallel processing
    enable_parallel: bool = True
    max_threads: int = 8
    work_queue_size: int = 1000
    
    # Memory management
    memory_pool_size: int = 10000          # For neuron/synapse allocation
    enable_memory_pools: bool = True
    cache_line_size: int = 64
    
    # Event system
    event_queue_size: int = 10000
    enable_event_driven: bool = True
    
    # I/O and checkpointing
    checkpoint_buffer_size: int = 1024 * 1024  # 1MB
    compression_level: int = 6
    max_history_entries: int = 10000
    
    @classmethod
    def get_default_constants(cls) -> 'PerformanceConstants':
        """Get default performance constants"""
        return cls()

# Simulation constants
@dataclass
class SimulationConstants:
    """Simulation timing and control constants"""
    
    # Time stepping
    default_timestep: float = 0.001         # 1 ms
    min_timestep: float = 0.0001
    max_timestep: float = 0.1
    
    # Integration methods
    integration_method: str = "forward_euler"  # "forward_euler", "runge_kutta"
    
    # Stability
    max_voltage_change: float = 50.0        # mV per timestep
    max_current_injection: float = 100.0   # nA
    
    # Output and logging
    log_level: str = "info"                # "debug", "info", "warning", "error"
    log_interval: int = 100                # Log every N steps
    status_interval: int = 1000             # Status every N steps
    
    # Statistics
    spike_history_size: int = 10000        # Number of spikes to keep
    activity_window: int = 100             # Window for activity calculation
    
    @classmethod
    def get_default_constants(cls) -> 'SimulationConstants':
        """Get default simulation constants"""
        return cls()

# Type variables for generic functions
T = TypeVar('T')
BrainType = TypeVar('BrainType')
WorldType = TypeVar('WorldType')

# Export commonly used types and constants
__all__ = [
    # Type aliases
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'SimulationStep', 'Timestamp', 'MembranePotential',
    
    # Enumeration classes
    'NeuronType', 'SynapseType', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'DevelopmentalStage', 'FiringState',
    'PlasticityFlag', 'PlasticityFlags',
    
    # Invalid ID constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Constants classes
    'PhysicsConstants', 'PerformanceConstants', 'SimulationConstants',
    
    # Type variables
    'T', 'BrainType', 'WorldType',
    
    # Utility functions
    'get_neuron_type_name', 'get_synapse_type_name',
    'get_action_name', 'get_motor_command_name',
    'get_object_type_name', 'get_developmental_stage_name',
    'get_firing_state_name', 'get_plasticity_flag_name',
]

# Convenience utility functions
def get_neuron_type_name(neuron_type: int) -> str:
    """Get human-readable name for neuron type"""
    return NeuronType.get_type_name(neuron_type)


def get_synapse_type_name(synapse_type: int) -> str:
    """Get human-readable name for synapse type"""
    return SynapseType.get_type_name(synapse_type)


def get_action_name(action_type: int) -> str:
    """Get human-readable name for action type"""
    return ActionType.get_action_name(action_type)


def get_motor_command_name(command: int) -> str:
    """Get human-readable name for motor command"""
    return MotorCommand.get_command_name(command)


def get_object_type_name(obj_type: int) -> str:
    """Get human-readable name for world object type"""
    return WorldObjectType.get_object_name(obj_type)


def get_developmental_stage_name(stage: int) -> str:
    """Get human-readable name for developmental stage"""
    return DevelopmentalStage.get_stage_name(stage)


def get_firing_state_name(state: int) -> str:
    """Get human-readable name for firing state"""
    return FiringState.get_state_name(state)


def get_plasticity_flag_name(flag: int) -> str:
    """Get human-readable name for plasticity flag"""
    return PlasticityFlag.get_flag_name(flag)


# Mathematical utility functions
def clamp(value: float, min_val: float, max_val: float) -> float:
    """Clamp value between min and max"""
    return max(min_val, min(max_val, value))


def normalize(value: float, min_val: float, max_val: float) -> float:
    """Normalize value to [0, 1] range"""
    if max_val == min_val:
        return 0.5
    return (value - min_val) / (max_val - min_val)


def sigmoid(x: float) -> float:
    """Sigmoid activation function"""
    return 1.0 / (1.0 + math.exp(-x))


def gaussian(x: float, mean: float, std: float) -> float:
    """Gaussian function"""
    return math.exp(-0.5 * ((x - mean) / std) ** 2) / (std * math.sqrt(2 * math.pi))


def exponential_decay(value: float, decay_rate: float, timestep: float) -> float:
    """Exponential decay function"""
    return value * math.exp(-decay_rate * timestep)


# Vector operations (simplified)
def dot_product(a: List[float], b: List[float]) -> float:
    """Dot product of two vectors"""
    if len(a) != len(b):
        raise ValueError("Vectors must have same length")
    return sum(ai * bi for ai, bi in zip(a, b))


def magnitude(vector: List[float]) -> float:
    """Magnitude of a vector"""
    return math.sqrt(sum(x * x for x in vector))


def normalize_vector(vector: List[float]) -> List[float]:
    """Normalize vector to unit length"""
    mag = magnitude(vector)
    if mag == 0:
        return [0.0] * len(vector)
    return [x / mag for x in vector]


def distance(p1: tuple, p2: tuple) -> float:
    """Distance between two points"""
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)


# Time and simulation utilities
def format_time(seconds: float) -> str:
    """Format time in seconds to human-readable string"""
    if seconds < 1.0:
        return f"{seconds * 1000:.1f}ms"
    elif seconds < 60.0:
        return f"{seconds:.2f}s"
    elif seconds < 3600.0:
        minutes = seconds / 60.0
        return f"{minutes:.1f}m"
    else:
        hours = seconds / 3600.0
        return f"{hours:.2f}h"


def format_step_count(steps: int) -> str:
    """Format step count with thousands separators"""
    return f"{steps:,}"


# Random number generation utilities
class SimpleRNG:
    """Simple random number generator for NLM"""
    
    def __init__(self, seed: int = 42):
        self.seed = seed
        self.state = seed
        
    def next_int(self, max_val: int) -> int:
        """Generate random integer in [0, max_val)"""
        self.state = (1664525 * self.state + 1013904223) % 2**32
        return self.state % max_val
        
    def next_float(self) -> float:
        """Generate random float in [0, 1)"""
        self.state = (1664525 * self.state + 1013904223) % 2**32
        return self.state / 2**32
        
    def next_gaussian(self, mean: float = 0.0, std: float = 1.0) -> float:
        """Generate random Gaussian number"""
        # Box-Muller transform
        u1 = self.next_float()
        u2 = self.next_float()
        
        z0 = math.sqrt(-2.0 * math.log(u1)) * math.cos(2.0 * math.pi * u2)
        z1 = math.sqrt(-2.0 * math.log(u1)) * math.sin(2.0 * math.pi * u2)
        
        return mean + std * z0
        
    def reset(self, seed: Optional[int] = None) -> None:
        """Reset random number generator"""
        if seed is not None:
            self.seed = seed
        self.state = self.seed
        
    def shuffle(self, items: List[Any]) -> List[Any]:
        """Shuffle list randomly"""
        result = items.copy()
        for i in range(len(result) - 1, 0, -1):
            j = self.next_int(i + 1)
            result[i], result[j] = result[j], result[i]
        return result


# Performance timing utilities
class Timer:
    """Simple timer for performance measurement"""
    
    def __init__(self):
        self.start_time = 0.0
        self.end_time = 0.0
        self.duration = 0.0
        
    def start(self) -> None:
        """Start timer"""
        self.start_time = time.time()
        
    def stop(self) -> float:
        """
        Stop timer and return duration.
        
        Returns:
            Duration in seconds.
        """
        self.end_time = time.time()
        self.duration = self.end_time - self.start_time
        return self.duration
        
    def reset(self) -> None:
        """Reset timer"""
        self.start_time = 0.0
        self.end_time = 0.0
        self.duration = 0.0
        
    def get_duration(self) -> float:
        """Get duration in seconds"""
        return self.duration
        
    def get_duration_ms(self) -> float:
        """Get duration in milliseconds"""
        return self.duration * 1000.0
        
    def __enter__(self):
        """Context manager entry"""
        self.start()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.stop()


# Memory utilities
class MemoryTracker:
    """Track memory usage for debugging"""
    
    @staticmethod
    def get_memory_usage() -> Dict[str, Any]:
        """Get current memory usage"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        return {
            'rss': memory_info.rss,  # Resident set size
            'vms': memory_info.vms,  # Virtual memory size
            'shared': memory_info.shared,
            'text': memory_info.text,
            'data': memory_info.data,
            'dirty': memory_info.dirty,
        }
        
    @staticmethod
    def get_memory_usage_mb() -> Dict[str, float]:
        """Get memory usage in megabytes"""
        memory_info = MemoryTracker.get_memory_usage()
        return {k: v / (1024 * 1024) for k, v in memory_info.items()}


# File utilities
class FileUtils:
    """File I/O utilities for NLM"""
    
    @staticmethod
    def ensure_directory(path: str) -> None:
        """Ensure directory exists"""
        import os
        os.makedirs(path, exist_ok=True)
        
    @staticmethod
    def safe_write(filepath: str, content: str, encoding: str = 'utf-8') -> bool:
        """
        Safely write content to file.
        
        Args:
            filepath: Path to file.
            content: Content to write.
            encoding: File encoding.
            
        Returns:
            True if successful, False otherwise.
        """
        try:
            FileUtils.ensure_directory(os.path.dirname(filepath))
            with open(filepath, 'w', encoding=encoding) as f:
                f.write(content)
            return True
        except Exception:
            return False
            
    @staticmethod
    def safe_read(filepath: str, encoding: str = 'utf-8') -> Optional[str]:
        """
        Safely read content from file.
        
        Args:
            filepath: Path to file.
            encoding: File encoding.
            
        Returns:
            File content or None if read failed.
        """
        try:
            with open(filepath, 'r', encoding=encoding) as f:
                return f.read()
        except Exception:
            return None
            
    @staticmethod
    def file_exists(filepath: str) -> bool:
        """Check if file exists"""
        import os
        return os.path.exists(filepath)
        
    @staticmethod
    def get_file_extension(filepath: str) -> str:
        """Get file extension"""
        import os
        return os.path.splitext(filepath)[1].lower()
        
    @staticmethod
    def is_text_file(filepath: str) -> bool:
        """Check if file is likely a text file"""
        ext = FileUtils.get_file_extension(filepath)
        text_extensions = ['.txt', '.md', '.json', '.xml', '.yaml', '.yml', '.py', '.cpp', '.hpp', '.h']
        return ext in text_extensions


# Export type conversion functions
__all__.extend([
    'clamp', 'normalize', 'sigmoid', 'gaussian', 'exponential_decay',
    'dot_product', 'magnitude', 'normalize_vector', 'distance',
    'format_time', 'format_step_count',
    'SimpleRNG', 'Timer', 'MemoryTracker', 'FileUtils',
])