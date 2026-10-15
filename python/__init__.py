from pynlm import (
    # Core types
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Classes
    Config, Vision, Audio, InternalSignals, Action, WorldObject,
    AgentBody, ActionResult, SensoryPercept,
    SimpleWorld, Brain, AgentBrain,
    
    # Helper functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    
    # Enhanced classes
    NeuralSystem, SensorData, BrainMetrics, DataManager,
    PerformanceProfiler, Visualizer,
    
    # Example modules
    LearningExamples, DevelopmentExamples, NeuromodulationExamples,
    EnvironmentExamples, PerformanceExamples, CheckpointExamples,
    
    # Type aliases
    tensor, epoch, step, neuron_id, synapse_id, population_id, region_id,
    
    # Exception classes
    NLMError, ValidationError, ConfigurationError, DeviceError,
    
    # Context managers
    Timer, BrainSession, WorldSession,
    
    # Utility functions
    configure_logging, set_config, get_config,
    raise_validation_error, raise_configuration_error, raise_device_error
)

# Module metadata
__version__ = "1.0.0"
__author__ = "NLM Development Team"
__description__ = "Enhanced Python bindings for NLM neural simulation framework"
__license__ = "MIT"

# Platform-specific imports
import sys
import platform

if platform.system() == "Linux":
    # Linux-specific configuration
    import subprocess
    __platform__ = "Linux"
elif platform.system() == "Windows":
    __platform__ = "Windows"
elif platform.system() == "Darwin":
    __platform__ = "macOS"
else:
    __platform__ = "Unknown"

# Module-level configuration
_default_config = {
    'backend': 'numpy',
    'device': 'cpu',
    'precision': 'float32',
    'matplotlib_backend': 'Agg',
    'enable_profiling': False,
    'checkpoint_dir': './checkpoints',
    'log_level': 'INFO',
    'random_seed': 42
}

# Initialize module
configure_logging(get_config('log_level', _default_config['log_level']))

# Check Python version
if sys.version_info < (3, 8):
    import warnings
    warnings.warn("NLM requires Python 3.8 or higher", DeprecationWarning)

# Check NumPy version
import numpy as np
if np.__version__ < "1.20.0":
    import warnings
    warnings.warn("NLM recommends NumPy 1.20.0 or higher", DeprecationWarning)

# Check Matplotlib version
import matplotlib
if matplotlib.__version__ < "3.0.0":
    import warnings
    warnings.warn("NLM recommends Matplotlib 3.0.0 or higher", DeprecationWarning)

print(f"NLM Python bindings v{__version__} initialized")
print(f"Platform: {__platform__}")
print(f"Python: {sys.version}")
print(f"NumPy: {np.__version__}")
print(f"Matplotlib: {matplotlib.__version__}")
