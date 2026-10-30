"""
Python constants and configuration module for NLM

This module provides Python-friendly constants and configuration
options for NLM, including enhanced error handling and validation.
"""

import sys
from typing import Dict, Any, Union
from enum import Enum

# Import from C++ bindings if available
try:
    from .bindings import (
        CHECKPOINT_VERSION_MAJOR,
        CHECKPOINT_VERSION_MINOR,
        CHECKPOINT_VERSION_PATCH,
        CHECKPOINT_MAGIC,
    )
except ImportError:
    # Define constants as fallback
    CHECKPOINT_VERSION_MAJOR = 1
    CHECKPOINT_VERSION_MINOR = 0
    CHECKPOINT_VERSION_PATCH = 0
    CHECKPOINT_MAGIC = 0x4E4C4D434B545430  # "NLMCKT0"

# Compression levels for checkpointing
class CompressionLevel(Enum):
    """Compression levels for checkpointing operations"""
    
    NONE = 0
    """No compression"""
    
    FAST = 1
    """Fast compression (smaller CPU cost, larger files)"""
    
    BALANCED = 2
    """Balanced compression (good trade-off)"""
    
    BEST = 3
    """Best compression (smaller files, higher CPU cost)"""
    
    @classmethod
    def get_level_name(cls, level: 'CompressionLevel') -> str:
        """Get human-readable name for compression level"""
        names = {
            cls.NONE: "None",
            cls.FAST: "Fast",
            cls.BALANCED: "Balanced", 
            cls.BEST: "Best"
        }
        return names.get(level, str(level))
    
    @classmethod
    def from_string(cls, name: str) -> 'CompressionLevel':
        """Create compression level from string"""
        try:
            return cls[name.upper()]
        except KeyError:
            # Try case-insensitive match
            for member in cls:
                if member.name.lower() == name.lower():
                    return member
            raise ValueError(f"Unknown compression level: {name}")

# Checkpoint sections for file organization
class CheckpointSection(Enum):
    """Sections in checkpoint files"""
    
    HEADER = 0
    """Checkpoint header"""
    
    CONFIG = 1
    """Configuration data"""
    
    NEURONS = 2
    """Neuron states"""
    
    SYNAPSES = 3
    """Synapse states"""
    
    CONNECTIVITY = 4
    """Connectivity information"""
    
    PLASTICITY = 5
    """Plasticity state"""
    
    DEVELOPMENT = 6
    """Development state"""
    
    NEUROMODULATION = 7
    """Neuromodulation state"""
    
    MEMORY = 8
    """Memory system state"""
    
    SPIKE_HISTORY = 9
    """Spike history"""
    
    RANDOM_STATE = 10
    """Random number generator state"""
    
    SIMULATION_STATE = 11
    """General simulation state"""
    
    ENVIRONMENT = 12
    """Environment state"""
    
    CHECKSUM = 13
    """Checksum for validation"""
    
    END = 14
    """End of file marker"""
    
    @classmethod
    def get_section_name(cls, section: 'CheckpointSection') -> str:
        """Get human-readable name for checkpoint section"""
        names = {
            cls.HEADER: "Header",
            cls.CONFIG: "Configuration",
            cls.NEURONS: "Neurons",
            cls.SYNAPSES: "Synapses",
            cls.CONNECTIVITY: "Connectivity",
            cls.PLASTICITY: "Plasticity",
            cls.DEVELOPMENT: "Development",
            cls.NEUROMODULATION: "Neuromodulation",
            cls.MEMORY: "Memory",
            cls.SPIKE_HISTORY: "Spike History",
            cls.RANDOM_STATE: "Random State",
            cls.SIMULATION_STATE: "Simulation State",
            cls.ENVIRONMENT: "Environment",
            cls.CHECKSUM: "Checksum",
            cls.END: "End"
        }
        return names.get(section, str(section))

# Invalid ID sentinel values
class InvalidID:
    """Invalid ID sentinel values for safety"""
    
    @property
    def NEURON_ID(self) -> int:
        """Invalid neuron ID sentinel value"""
        return 0xFFFFFFFFFFFFFFFF
    
    @property
    def SYNAPSE_ID(self) -> int:
        """Invalid synapse ID sentinel value"""
        return 0xFFFFFFFFFFFFFFFF
    
    @property
    def REGION_ID(self) -> int:
        """Invalid region ID sentinel value"""
        return 0xFFFFFFFFFFFFFFFF
    
    @property
    def POPULATION_ID(self) -> int:
        """Invalid population ID sentinel value"""
        return 0xFFFFFFFFFFFFFFFF

# Simulation constants
class SimulationConstants:
    """Constants for simulation configuration"""
    
    # Default simulation parameters
    DEFAULT_SIMULATION_STEP = 0.001  # seconds
    DEFAULT_SIMULATION_TIME = 0.0  # seconds
    DEFAULT_MAX_STEPS = 10000
    
    # Brain parameters
    DEFAULT_NEURON_COUNT = 1000
    DEFAULT_REGION_COUNT = 2
    DEFAULT_CONNECTION_PROBABILITY = 0.1
    
    # Plasticity parameters
    DEFAULT_LEARNING_RATE = 0.01
    DEFAULT_STDP_LTP_WEIGHT = 0.01
    DEFAULT_STDP_LTD_WEIGHT = 0.012
    DEFAULT_STDP_TIME_CONSTANT = 20.0
    
    # Development parameters
    DEFAULT_DEVELOPMENT_SYNAPTONGENESIS_RATE = 0.001
    DEFAULT_DEVELOPMENT_PRUNING_RATE = 0.0001
    
    # Neuromodulation parameters
    DEFAULT_DOPAMINE_BASELINE = 0.1
    DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99
    
    # Environment parameters
    DEFAULT_ENVIRONMENT_WIDTH = 10
    DEFAULT_ENVIRONMENT_HEIGHT = 10
    
    @classmethod
    def get_default(cls, param_name: str) -> Any:
        """
        Get default value for parameter.
        
        Args:
            param_name: Parameter name
            
        Returns:
            Default value
            
        Raises:
            KeyError: If parameter not found
        """
        params = {
            'simulation_step': cls.DEFAULT_SIMULATION_STEP,
            'simulation_time': cls.DEFAULT_SIMULATION_TIME,
            'max_steps': cls.DEFAULT_MAX_STEPS,
            'neuron_count': cls.DEFAULT_NEURON_COUNT,
            'region_count': cls.DEFAULT_REGION_COUNT,
            'connection_probability': cls.DEFAULT_CONNECTION_PROBABILITY,
            'learning_rate': cls.DEFAULT_LEARNING_RATE,
            'stdp_ltp_weight': cls.DEFAULT_STDP_LTP_WEIGHT,
            'stdp_ltd_weight': cls.DEFAULT_STDP_LTD_WEIGHT,
            'stdp_time_constant': cls.DEFAULT_STDP_TIME_CONSTANT,
            'development_synaptogenesis_rate': cls.DEFAULT_DEVELOPMENT_SYNAPTONGENESIS_RATE,
            'development_pruning_rate': cls.DEFAULT_DEVELOPMENT_PRUNING_RATE,
            'dopamine_baseline': cls.DEFAULT_DOPAMINE_BASELINE,
            'reward_discount_factor': cls.DEFAULT_REWARD_DISCOUNT_FACTOR,
            'environment_width': cls.DEFAULT_ENVIRONMENT_WIDTH,
            'environment_height': cls.DEFAULT_ENVIRONMENT_HEIGHT,
        }
        
        if param_name not in params:
            raise KeyError(f"Unknown parameter: {param_name}")
        
        return params[param_name]

# Error constants
class ErrorConstants:
    """Constants for error handling and messaging"""
    
    # Error codes
    SUCCESS = 0
    GENERAL_ERROR = 1
    INVALID_PARAMETER = 2
    FILE_NOT_FOUND = 3
    INVALID_FORMAT = 4
    MEMORY_ERROR = 5
    VALIDATION_ERROR = 6
    
    # Error messages
    ERROR_MESSAGES = {
        GENERAL_ERROR: "General error occurred",
        INVALID_PARAMETER: "Invalid parameter provided",
        FILE_NOT_FOUND: "File not found",
        INVALID_FORMAT: "Invalid file format",
        MEMORY_ERROR: "Memory allocation error",
        VALIDATION_ERROR: "Validation error",
    }
    
    @classmethod
    def get_error_message(cls, error_code: int) -> str:
        """Get error message for error code"""
        return cls.ERROR_MESSAGES.get(error_code, cls.ERROR_MESSAGES[GENERAL_ERROR])

# Configuration constants
class ConfigConstants:
    """Constants for configuration management"""
    
    # Configuration sources
    SOURCE_DEFAULT = "default"
    SOURCE_FILE = "file"
    SOURCE_COMMAND_LINE = "command_line"
    SOURCE_RUNTIME = "runtime"
    
    # Configuration key prefixes
    BRAIN_PREFIX = "brain."
    PLASTICITY_PREFIX = "plasticity."
    NEUROMODULATION_PREFIX = "neuromod."
    DEVELOPMENT_PREFIX = "development."
    ENVIRONMENT_PREFIX = "environment."
    LOGGING_PREFIX = "logging."
    
    # Default configuration file paths
    DEFAULT_CONFIG_FILE = "default.cfg"
    DEFAULT_CHECKPOINT_DIR = "checkpoints"
    
    @classmethod
    def get_config_prefix(cls, component: str) -> str:
        """Get configuration prefix for component"""
        prefixes = {
            'brain': cls.BRAIN_PREFIX,
            'plasticity': cls.PLASTICITY_PREFIX,
            'neuromod': cls.NEUROMODULATION_PREFIX,
            'development': cls.DEVELOPMENT_PREFIX,
            'environment': cls.ENVIRONMENT_PREFIX,
            'logging': cls.LOGGING_PREFIX,
        }
        
        if component not in prefixes:
            raise ValueError(f"Unknown component: {component}")
        
        return prefixes[component]

# System information
class SystemInfo:
    """System information and metadata"""
    
    @staticmethod
    def get_version() -> str:
        """Get NLM version"""
        return "0.1.0"
    
    @staticmethod
    def get_python_version() -> str:
        """Get Python version"""
        return f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}"
    
    @staticmethod
    def get_platform() -> str:
        """Get platform information"""
        import platform
        return platform.system()
    
    @staticmethod
    def get_available_memory() -> int:
        """Get available memory in MB (placeholder)"""
        # This would need platform-specific implementation
        return 0  # Placeholder

# Export all constants
__all__ = [
    # Core constants
    'CHECKPOINT_VERSION_MAJOR',
    'CHECKPOINT_VERSION_MINOR',
    'CHECKPOINT_VERSION_PATCH',
    'CHECKPOINT_MAGIC',
    
    # Enum classes
    'CompressionLevel',
    'CheckpointSection',
    
    # Invalid ID class
    'InvalidID',
    
    # Constants classes
    'SimulationConstants',
    'ErrorConstants',
    'ConfigConstants',
    'SystemInfo',
    
    # Common values
    'DEFAULT_SIMULATION_STEP',
    'DEFAULT_NEURON_COUNT',
    'DEFAULT_CONNECTION_PROBABILITY',
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python constants for NLM neural simulation"

print(f"NLM constants module loaded (version {__version__})")