from .examples import *
from .benchmark import NLMBenchmark, BenchmarkResult, BenchmarkStage, benchmark_all
from .cli import main

__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) - experimental artificial developmental brain simulation"

# Global settings
GLOBAL_SETTINGS = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "log_level": "INFO",
    "enable_visualization": False,
    "enable_checkpointing": False,
}

# Configuration presets
DEFAULT_CONFIG_PRESETS = {
    "minimal": {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.05,
    },
    "standard": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
    },
    "large": {
        "neuron_count": 5000,
        "region_count": 4,
        "connection_probability": 0.05,
    },
}

# Module constants
INVALID_NEURON_ID = None
INVALID_SYNAPSE_ID = None
INVALID_REGION_ID = None
INVALID_POPULATION_ID = None

# Performance settings
DEFAULT_SIMULATION_STEPS = 1000
DEFAULT_BRAIN_SIZE = 1000
DEFAULT_WORLD_SIZE = 20
DEFAULT_VISION_SIZE = 8

# Export all important classes and functions
__all__ = [
    # Core functionality
    'ConfigManager',
    'ConfigSchema',
    'ConfigValidationError',
    'ConfigPresets',
    'create_default_config',
    'get_global_config',
    'set_global_config',
    'load_preset',
    'validate_config',
    'ConfigSource',
    
    # Benchmark functionality
    'NLMBenchmark',
    'BenchmarkResult',
    'BenchmarkStage',
    'benchmark_all',
    
    # CLI functionality
    'main',
    
    # Examples functionality
    'run_all_examples',
    'get_example_list',
    'run_example',
    'ExamplesDirectory',
    
    # Global constants and settings
    '__version__',
    '__author__',
    '__description__',
    'GLOBAL_SETTINGS',
    'DEFAULT_CONFIG_PRESETS',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    'DEFAULT_SIMULATION_STEPS',
    'DEFAULT_BRAIN_SIZE',
    'DEFAULT_WORLD_SIZE',
    'DEFAULT_VISION_SIZE',
]