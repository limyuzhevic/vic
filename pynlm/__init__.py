"""
NLM (熙然) - Neural Learning Machine

A brain-inspired spiking neural network simulator for Python.

This package provides Python bindings for the NLM C++ neural simulation framework,
allowing you to create, train, and simulate artificial brains with neurons, synapses,
memory systems, neuromodulation, and learning.

Key Features:
- Real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, reward-modulated)
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Cognitive systems (planning, concept formation, attention)
- Developmental stages and structural plasticity
- Checkpoint/saving and loading
- Comprehensive debugging and visualization tools
"""

import sys
import json
import time
from typing import List, Optional, Dict, Any, Union, Callable
from enum import Enum

# Try to import pybind11 features
try:
    import pybind11
    from pybind11 import std_vector, std_string, std_map, class_
    PYBIND11_AVAILABLE = True
except ImportError:
    PYBIND11_AVAILABLE = False

# Version information
__version__ = "2.0.0"
__author__ = "NLM Authors"
__license__ = "MIT"

if PYBIND11_AVAILABLE:
    import pynlm._pynlm
    from pynlm._pynlm import (
        # Core classes
        Brain,
        Neuron,
        Synapse,
        NeuralRegion,
        
        # Memory systems
        WorkingMemory,
        EpisodicMemory,
        AssociativeMemory,
        
        # Neuromodulation
        Dopamine,
        Curiosity,
        Novelty,
        PredictionError,
        
        # Plasticity
        STDP,
        Hebbian,
        StructuralPlasticity,
        
        # Cognition
        NeuralPlanner,
        ConceptFormation,
        AttentionalSelection,
        
        # Development
        DevelopmentSystem,
        
        # Sensory/Motor
        SimpleWorld,
        SensoryPercept,
        AgentBrain,
        AgentBody,
        
        # Environment
        Environment,
        Observation,
        
        # Utilities
        SimulationClock,
        RandomGenerator,
        Config,
        Logger,
        
        # Enums
        NeuronType,
        FiringState,
        ActionType,
        DevelopmentalStage,
        PlasticityFlags,
        LogLevel,
        SynapseType,
        WorldObjectType,
        
        # Type aliases
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        Timestamp,
        TimestepDuration,
        SimulationStep,
        
        # Configuration
        createDefaultConfig,
        saveConfig,
        loadConfig,
        
        # Factory functions
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        
        # Main simulation loop
        runSimulation,
        runAgentSimulation,
        
        # Utility functions
        getVersion,
        getBuildInfo,
    )
    
    # Create Python-friendly classes that wrap the C++ classes
    # with additional Pythonic features
    
    class PyBrain:
        """Python-friendly wrapper for Brain with enhanced features."""
        
        def __init__(self, config=None):
            if config is None:
                config = createDefaultConfig()
            self._brain = createBrain(config)
            self._brain.initialize()
        
        def step(self, step_number: int = None, time: float = None) -> None:
            """Run one simulation step.
            
            Args:
                step_number: Step number (optional)
                time: Timestamp (optional)
            """
            if time is not None and step_number is not None:
                self._brain.step(step_number, time)
            elif step_number is not None:
                self._brain.step(step_number)
            else:
                self._brain.step(0)
        
        def run_steps(self, num_steps: int) -> None:
            """Run multiple simulation steps."""
            for i in range(num_steps):
                self.step(i)
        
        def inject_current(self, neuron_id: NeuronId, current: float) -> None:
            """Inject current into a specific neuron."""
            self._brain.injectCurrent(neuron_id, current)
        
        def inject_current_by_type(self, neuron_type: NeuronType, current: float) -> None:
            """Inject current into all neurons of a specific type."""
            self._brain.injectCurrentToNeurons(neuron_type, current)
        
        # Statistics properties
        @property
        def total_neurons(self) -> int:
            return self._brain.getTotalNeuronCount()
        
        @property
        def total_synapses(self) -> int:
            return self._brain.getTotalSynapseCount()
        
        @property
        def firing_neurons(self) -> int:
            return self._brain.getFiringNeuronCount()
        
        @property
        def total_spikes(self) -> int:
            return self._brain.getTotalSpikeCount()
        
        @property
        def average_firing_rate(self) -> float:
            return self._brain.getAverageFiringRate()
        
        @property
        def e_i_ratio(self) -> float:
            return self._brain.getExcitationInhibitionRatio()
        
        @property
        def developmental_stage(self) -> DevelopmentalStage:
            return self._brain.getDevelopmentalStage()
        
        def save(self, filepath: str) -> bool:
            """Save brain state to file."""
            return self._brain.save(filepath)
        
        def load(self, filepath: str) -> bool:
            """Load brain state from file."""
            return self._brain.load(filepath)
        
        def reset(self) -> None:
            """Reset brain to initial state."""
            self._brain.reset()
        
        def log_status(self) -> None:
            """Print current brain status."""
            self._brain.logStatus()
        
        def __enter__(self):
            return self
        
        def __exit__(self, exc_type, exc_val, exc_tb):
            self.reset()
    
    class PyConfig:
        """Python-friendly configuration wrapper."""
        
        def __init__(self, config_file: str = None):
            if config_file:
                config = loadConfig(config_file)
            else:
                config = createDefaultConfig()
            self._config = config
        
        def set(self, key: str, value: Any) -> None:
            """Set a configuration value."""
            if isinstance(value, bool):
                self._config.set(key, value)
            elif isinstance(value, int):
                self._config.set(key, value)
            elif isinstance(value, float):
                self._config.set(key, value)
            elif isinstance(value, str):
                self._config.set(key, value)
            elif isinstance(value, list):
                # For lists, we need to convert to the appropriate C++ type
                if all(isinstance(x, int) for x in value):
                    self._config.set(key, value)
                elif all(isinstance(x, float) for x in value):
                    self._config.set(key, value)
                elif all(isinstance(x, str) for x in value):
                    self._config.set(key, value)
                else:
                    raise TypeError(f"Unsupported list type for key '{key}'")
            else:
                raise TypeError(f"Unsupported type for key '{key}': {type(value)}")
        
        def get(self, key: str, default: Any = None) -> Any:
            """Get a configuration value."""
            value = self._config.getOr(key, default)
            return value
        
        def has(self, key: str) -> bool:
            """Check if a key exists."""
            return self._config.has(key)
        
        def keys(self) -> List[str]:
            """Get all configuration keys."""
            return self._config.getKeys()
        
        def save(self, filepath: str) -> bool:
            """Save configuration to file."""
            return saveConfig(self._config, filepath)
        
        def __getitem__(self, key: str) -> Any:
            """Get item using [] operator."""
            return self.get(key)
        
        def __setitem__(self, key: str, value: Any) -> None:
            """Set item using [] operator."""
            self.set(key, value)
        
        def __contains__(self, key: str) -> bool:
            """Check if key exists using 'in' operator."""
            return self.has(key)
    
    class PySimpleWorld:
        """Python-friendly world simulation wrapper."""
        
        def __init__(self, width: int = 20, height: int = 20):
            self._world = createSimpleWorld()
            self._world.configure(width, height, 8, 8)
            self._world.reset()
        
        def set_agent_start(self, x: float, y: float) -> None:
            """Set agent starting position."""
            self._world.setAgentStart(x, y)
        
        def update(self, timestep: float = 0.1) -> None:
            """Update world state."""
            self._world.update(timestep)
        
        def get_sensory_percept(self) -> Dict[str, Any]:
            """Get sensory percept from world."""
            percept = self._world.getSensoryPercept()
            return self._percept_to_dict(percept)
        
        def apply_motor_command(self, action: Any, time: float = None) -> None:
            """Apply motor command to world."""
            if time is None:
                time = self._world.getSimulationTime()
            self._world.applyMotorCommand(action, time)
        
        @property
        def width(self) -> int:
            return self._world.getWidth()
        
        @property
        def height(self) -> int:
            return self._world.getHeight()
        
        @property
        def simulation_time(self) -> float:
            return self._world.getSimulationTime()
        
        def reset(self) -> None:
            """Reset world to initial state."""
            self._world.reset()
        
        def _percept_to_dict(self, percept) -> Dict[str, Any]:
            """Convert C++ percept to Python dictionary."""
            # This is a placeholder - actual implementation would depend on the C++ API
            return {
                "vision": percept.getVision(),
                "touch": percept.getTouch(),
                "internal": percept.getInternal(),
                "proprioception": percept.getProprioception(),
            }
    
    # Create convenience functions for common use cases
    def quick_brain_simulation(neuron_count: int = 100, steps: int = 1000) -> PyBrain:
        """Create a simple brain and run a quick simulation."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = PyBrain(config)
        brain.run_steps(steps)
        return brain
    
    def agent_world_simulation(brain: PyBrain, world: PySimpleWorld, steps: int = 500) -> PyBrain:
        """Run an agent in a world simulation."""
        agent = createAgentBrain(brain._brain)
        agent.initialize(world._world)
        
        for step in range(steps):
            world.update(0.1)
            percept = world.get_sensory_percept()
            # Convert back to C++ percept (placeholder)
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.apply_motor_command(action)
        
        return brain
    
    def benchmark_simulation(neuron_counts: List[int], steps: int) -> Dict[int, float]:
        """Benchmark simulation performance across different neuron counts."""
        results = {}
        for count in neuron_counts:
            config = createDefaultConfig()
            config.set("brain.neuron_count", count)
            
            brain = PyBrain(config)
            start_time = time.time()
            brain.run_steps(steps)
            end_time = time.time()
            
            results[count] = end_time - start_time
        return results
    
    def get_example_configurations() -> Dict[str, PyConfig]:
        """Get example configurations for common use cases."""
        examples = {
            "simple": PyConfig(),
            "large": PyConfig(),
            "sparse": PyConfig(),
            "dense": PyConfig(),
        }
        
        # Configure examples
        examples["simple"].set("brain.neuron_count", 100)
        examples["simple"].set("brain.connection_probability", 0.05)
        
        examples["large"].set("brain.neuron_count", 5000)
        examples["large"].set("brain.connection_probability", 0.1)
        
        examples["sparse"].set("brain.neuron_count", 500)
        examples["sparse"].set("brain.connection_probability", 0.01)
        
        examples["dense"].set("brain.neuron_count", 1000)
        examples["dense"].set("brain.connection_probability", 0.3)
        
        return examples
    
    def print_version_info() -> None:
        """Print version information."""
        print(f"NLM Python Bindings v{__version__}")
        print(f"Author: {__author__}")
        print(f"License: {__license__}")
        print(f"Build: {getBuildInfo()}")
        
        # Print available subsystems
        print("\nAvailable subsystems:")
        print("  - Brain and Neural Computation")
        print("  - Memory Systems (Working, Episodic, Associative)")
        print("  - Neuromodulation (Dopamine, Curiosity, Novelty)")
        print("  - Plasticity (STDP, Hebbian, Structural)")
        print("  - Cognition (Planning, Concepts, Attention)")
        print("  - Development")
        print("  - Sensory/Motor")
        print("  - Environment Simulation")
        print("  - Utilities (Config, Logger, Random)")

# Provide a comprehensive module documentation
__all__ = [
    # Core classes
    "Brain", "Neuron", "Synapse", "NeuralRegion",
    
    # Memory systems
    "WorkingMemory", "EpisodicMemory", "AssociativeMemory",
    
    # Neuromodulation
    "Dopamine", "Curiosity", "Novelty", "PredictionError",
    
    # Plasticity
    "STDP", "Hebbian", "StructuralPlasticity",
    
    # Cognition
    "NeuralPlanner", "ConceptFormation", "AttentionalSelection",
    
    # Development
    "DevelopmentSystem",
    
    # Sensory/Motor
    "SimpleWorld", "SensoryPercept", "AgentBrain", "AgentBody",
    
    # Environment
    "Environment", "Observation",
    
    # Utilities
    "SimulationClock", "RandomGenerator", "Config", "Logger",
    
    # Enums
    "NeuronType", "FiringState", "ActionType", "DevelopmentalStage",
    "PlasticityFlags", "LogLevel", "SynapseType", "WorldObjectType",
    
    # Type aliases
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "Timestamp", "TimestepDuration", "SimulationStep",
    
    # Configuration
    "createDefaultConfig", "saveConfig", "loadConfig",
    
    # Factory functions
    "createBrain", "createSimpleWorld", "createAgentBrain",
    
    # Main simulation loop
    "runSimulation", "runAgentSimulation",
    
    # Utility functions
    "getVersion", "getBuildInfo",
    
    # Python-friendly classes
    "PyBrain", "PyConfig", "PySimpleWorld",
    
    # Convenience functions
    "quick_brain_simulation", "agent_world_simulation",
    "benchmark_simulation", "get_example_configurations",
    
    # Module info
    "print_version_info", "__version__", "__author__", "__license__"
]

if PYBIND11_AVAILABLE:
    # If pybind11 is available, try to override the classes with the actual C++ bindings
    import importlib.util
    import sys
    
    # Load the C++ extension module
    spec = importlib.util.find_spec("pynlm._pynlm")
    if spec is not None:
        # Replace our placeholder classes with the actual C++ bindings
        globals().update({
            'Brain': pynlm._pynlm.Brain,
            'Neuron': pynlm._pynlm.Neuron,
            'Synapse': pynlm._pynlm.Synapse,
            'NeuralRegion': pynlm._pynlm.NeuralRegion,
            'WorkingMemory': pynlm._pynlm.WorkingMemory,
            'EpisodicMemory': pynlm._pynlm.EpisodicMemory,
            'AssociativeMemory': pynlm._pynlm.AssociativeMemory,
            'Dopamine': pynlm._pynlm.Dopamine,
            'Curiosity': pynlm._pynlm.Curiosity,
            'Novelty': pynlm._pynlm.Novelty,
            'PredictionError': pynlm._pynlm.PredictionError,
            'STDP': pynlm._pynlm.STDP,
            'Hebbian': pynlm._pynlm.Hebbian,
            'StructuralPlasticity': pynlm._pynlm.StructuralPlasticity,
            'NeuralPlanner': pynlm._pynlm.NeuralPlanner,
            'ConceptFormation': pynlm._pynlm.ConceptFormation,
            'AttentionalSelection': pynlm._pynlm.AttentionalSelection,
            'DevelopmentSystem': pynlm._pynlm.DevelopmentSystem,
            'SimpleWorld': pynlm._pynlm.SimpleWorld,
            'SensoryPercept': pynlm._pynlm.SensoryPercept,
            'AgentBrain': pynlm._pynlm.AgentBrain,
            'AgentBody': pynlm._pynlm.AgentBody,
            'Environment': pynlm._pynlm.Environment,
            'Observation': pynlm._pynlm.Observation,
            'SimulationClock': pynlm._pynlm.SimulationClock,
            'RandomGenerator': pynlm._pynlm.RandomGenerator,
            'Config': pynlm._pynlm.Config,
            'Logger': pynlm._pynlm.Logger,
            
            'NeuronType': pynlm._pynlm.NeuronType,
            'FiringState': pynlm._pynlm.FiringState,
            'ActionType': pynlm._pynlm.ActionType,
            'DevelopmentalStage': pynlm._pynlm.DevelopmentalStage,
            'PlasticityFlags': pynlm._pynlm.PlasticityFlags,
            'LogLevel': pynlm._pynlm.LogLevel,
            'SynapseType': pynlm._pynlm.SynapseType,
            'WorldObjectType': pynlm._pynlm.WorldObjectType,
            
            'NeuronId': pynlm._pynlm.NeuronId,
            'SynapseId': pynlm._pynlm.SynapseId,
            'RegionId': pynlm._pynlm.RegionId,
            'PopulationId': pynlm._pynlm.PopulationId,
            'Timestamp': pynlm._pynlm.Timestamp,
            'TimestepDuration': pynlm._pynlm.TimestepDuration,
            'SimulationStep': pynlm._pynlm.SimulationStep,
            
            'createDefaultConfig': pynlm._pynlm.createDefaultConfig,
            'saveConfig': pynlm._pynlm.saveConfig,
            'loadConfig': pynlm._pynlm.loadConfig,
            
            'createBrain': pynlm._pynlm.createBrain,
            'createSimpleWorld': pynlm._pynlm.createSimpleWorld,
            'createAgentBrain': pynlm._pynlm.createAgentBrain,
            
            'runSimulation': pynlm._pynlm.runSimulation,
            'runAgentSimulation': pynlm._pynlm.runAgentSimulation,
            
            'getVersion': pynlm._pynlm.getVersion,
            'getBuildInfo': pynlm._pynlm.getBuildInfo,
        })

# For backwards compatibility, create the old-style imports
class _LegacyImport:
    """Legacy import compatibility."""
    
    def __getattr__(self, name):
        # Try to import from the actual module
        try:
            from pynlm._pynlm import __all__ as pynlm_all
            if name in pynlm_all:
                from pynlm._pynlm import *
                return globals()[name]
        except ImportError:
            pass
        
        # Return our Python-friendly wrapper
        if name == "PyBrain":
            return PyBrain
        elif name == "PyConfig":
            return PyConfig
        elif name == "PySimpleWorld":
            return PySimpleWorld
        
        raise ImportError(f"module '{__name__}' has no attribute '{name}'")

# Create legacy compatibility module
sys.modules[__name__] = _LegacyImport()
