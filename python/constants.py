"""Core constants and type definitions for NLM Python bindings"""

from enum import Enum
from dataclasses import dataclass
from typing import Dict, Any

# Constants
SIMULATION_TIMESTEP = 0.001
DEFAULT_VISION_WIDTH = 16
DEFAULT_VISION_HEIGHT = 16
DEFAULT_VISION_CHANNELS = 3
DEFAULT_AUDIO_SAMPLE_RATE = 44100
DEFAULT_AUDIO_DURATION = 1.0  # seconds

# Invalid ID sentinels (these would be cast from C++ types)
INVALID_NEURON_ID = -1
INVALID_SYNAPSE_ID = -1
INVALID_REGION_ID = -1
INVALID_POPULATION_ID = -1

# Default configuration
DEFAULT_CONFIG = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0,
    },
    "plasticity": {
        "stdp": {"enable": True, "learning_rate": 0.001, "tau_plus": 20.0, "tau_minus": 20.0},
        "hebbian": {"enable": True},
        "structural": {"enable": True},
    },
    "neuromod": {
        "dopamine": {"scale": 1.0},
        "curiosity": {"enable": True},
        "novelty": {"enable": True},
    }
}

# Type aliases
NeuronIndex = int
SynapseIndex = int
PopulationIndex = int
RegionIndex = int

# Unit types
class UnitTypes:
    """Common unit types for NLM quantities."""
    
    class Voltage:
        """Membrane potential units (mV)."""
        def __init__(self, mV):
            self.mV = mV
        
        def __repr__(self):
            return f"{self.mV} mV"
    
    class Time:
        """Time units (seconds)."""
        def __init__(self, s):
            self.s = s
        
        def __repr__(self):
            return f"{self.s} s"
    
    class Frequency:
        """Frequency units (Hz)."""
        def __init__(self, hz):
            self.hz = hz
        
        def __repr__(self):
            return f"{self.hz} Hz"
    
    class Current:
        """Current units (nA)."""
        def __init__(self, na):
            self.na = na
        
        def __repr__(self):
            return f"{self.na} nA"

# Factory functions
def create_config(neuron_count=1000, synapse_density=0.1, **kwargs):
    """Create a configuration dictionary with sensible defaults."""
    config = DEFAULT_CONFIG.copy()
    
    # Override with user parameters
    if "brain" in kwargs:
        config["brain"].update(kwargs["brain"])
    
    if "neuron_count" in kwargs:
        config["brain"]["neuron_count"] = kwargs["neuron_count"]
    
    if "synapse_density" in kwargs:
        config["brain"]["synapse_density"] = kwargs["synapse_density"]
    
    # Set the specified neuron count
    config["brain"]["neuron_count"] = neuron_count
    
    return config


def validate_config(config):
    """Validate configuration dictionary."""
    errors = []
    
    if "brain" not in config:
        errors.append("Missing 'brain' section in configuration")
        return errors
    
    brain = config["brain"]
    
    # Check required brain parameters
    required_params = ["neuron_count", "synapse_density", "connection_probability"]
    for param in required_params:
        if param not in brain:
            errors.append(f"Missing required brain parameter: {param}")
    
    # Check parameter ranges
    if brain.get("neuron_count", 0) <= 0:
        errors.append("neuron_count must be positive")
    
    if not (0.0 <= brain.get("synapse_density", 0) <= 1.0):
        errors.append("synapse_density must be between 0.0 and 1.0")
    
    if not (0.0 <= brain.get("connection_probability", 0) <= 1.0):
        errors.append("connection_probability must be between 0.0 and 1.0")
    
    return errors


# Neural unit helpers
def mV(voltage):
    """Create membrane potential in mV."""
    return UnitTypes.Voltage(voltage)


def ms(time):
    """Create time in milliseconds."""
    return UnitTypes.Time(time / 1000.0)  # Convert to seconds


def Hz(frequency):
    """Create frequency in Hz."""
    return UnitTypes.Frequency(frequency)


def nA(current):
    """Create current in nA."""
    return UnitTypes.Current(current)


# Comparison helpers
def is_near(a, b, tolerance=1.0):
    """Check if two values are near each other within tolerance."""
    return abs(a - b) <= tolerance


def clamp(value, min_val, max_val):
    """Clamp value between min and max."""
    return max(min_val, min(max_val, value))


# Distance calculation helpers
def euclidean_distance(x1, y1, x2, y2):
    """Calculate Euclidean distance between two points."""
    return ((x2 - x1) ** 2 + (y2 - y1) ** 2) ** 0.5


def manhattan_distance(x1, y1, x2, y2):
    """Calculate Manhattan distance between two points."""
    return abs(x2 - x1) + abs(y2 - y1)


# Angle helpers
def normalize_angle(angle):
    """Normalize angle to [-π, π]."""
    import math
    while angle > math.pi:
        angle -= 2 * math.pi
    while angle < -math.pi:
        angle += 2 * math.pi
    return angle


def angle_difference(angle1, angle2):
    """Calculate smallest difference between two angles."""
    import math
    diff = abs(angle1 - angle2)
    return min(diff, 2 * math.pi - diff)


# Color helpers (for visualization)
COLORS = {
    'excitatory': '#FF0000',  # Red
    'inhibitory': '#0000FF',  # Blue
    'sensory': '#00FF00',     # Green
    'motor': '#FFA500',       # Orange
    'internal': '#800080',    # Purple
    'modulatory': '#00FFFF',  # Cyan
    'active_neuron': '#FF6B6B',
    'inactive_neuron': '#CCCCCC',
    'synapse': '#666666',
    'background': '#FFFFFF',
    'text': '#000000'
}