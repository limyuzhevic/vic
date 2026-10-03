# NLM Python Bindings - High-Level API

"""
NLM (Neural Learning Machine) Python Bindings
===========================================

This module provides a simplified, Pythonic interface to the NLM neural simulation framework.

Key Features:
- Easy brain creation and configuration
- Simplified neuron and synapse identification
- High-level simulation control
- Built-in experiment support
- Memory and performance monitoring

Usage Examples:
```python
from nlm import NLM, Config, Brain

# Create a simple brain configuration
config = Config()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

# Create and initialize brain
brain = Brain(config)
brain.initialize()

# Run simulation
brain.step(100)

# Get statistics
print(f"Neurons: {brain.get_total_neuron_count()}")
print(f"Spikes: {brain.get_total_spike_count()}")
```

For more advanced usage, import from nlm.advanced:
```python
from nlm.advanced import AgentBrain, SimpleWorld
```
"""

import sys
from typing import List, Optional, Dict, Any, Union
import numpy as np

# Try to import core bindings
try:
    from .pynlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage,
        ActionType, MotorCommand, WorldObjectType,
        
        # Main classes
        Brain, Config, AgentBrain, SimpleWorld,
        SensoryInput, Vision, Audio, InternalSignals,
        Action, WorldObject, AgentBody, ActionResult,
        SensoryPercept,
        
        # Factory functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
        INVALID_POPULATION_ID
    )
    
    CORE_AVAILABLE = True
    ADVANCED_CONFIG_AVAILABLE = True
    
except ImportError as e:
    CORE_AVAILABLE = False
    ADVANCED_CONFIG_AVAILABLE = False
    
    # Define placeholder classes for when bindings aren't available
    class Config:
        def __init__(self):
            self._config = {}
        
        def set(self, key: str, value: Any) -> None:
            self._config[key] = value
        
        def get(self, key: str, default: Any = None) -> Any:
            return self._config.get(key, default)
        
        def loadFromFile(self, filepath: str) -> bool:
            # Implementation would load from JSON
            return False
        
        def saveToFile(self, filepath: str) -> bool:
            # Implementation would save to JSON
            return False
        
        def has(self, key: str) -> bool:
            return key in self._config
        
        def getKeys(self) -> List[str]:
            return list(self._config.keys())
    
    class Brain:
        def __init__(self, config: Optional[Config] = None):
            self._brain = None
            self.config = config or Config()
        
        def initialize(self) -> bool:
            return False
        
        def step(self, step: int, time: Optional[float] = None) -> None:
            pass
        
        def reset(self) -> None:
            pass
        
        # Convenience methods
        def configure(self, **kwargs) -> "Brain":
            for key, value in kwargs.items():
                self.config.set(key, value)
            return self
        
        def get_stats(self) -> Dict[str, Any]:
            return {
                "neurons": 0,
                "synapses": 0,
                "spikes": 0,
                "active": 0,
                "firing": 0
            }

# High-level facade functions

def create_simple_brain(
    neuron_count: int = 1000,
    region_count: int = 1,
    connection_probability: float = 0.1,
    timestep: float = 0.001,
    random_seed: int = 42
) -> Brain:
    """
    Create a simple, configured brain with sensible defaults.
    
    Args:
        neuron_count: Total number of neurons
        region_count: Number of brain regions
        connection_probability: Probability of synaptic connections
        timestep: Simulation timestep in seconds
        random_seed: Random seed for reproducibility
    
    Returns:
        Configured and initialized Brain instance
    """
    config = Config()
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connection_probability)
    config.set("simulation_timestep", timestep)
    config.set("random_seed", random_seed)
    
    brain = Brain(config)
    brain.initialize()
    return brain


def run_simulation(
    steps: int,
    config: Optional[Config] = None,
    verbose: bool = False
) -> Dict[str, Any]:
    """
    Run a simple simulation for the specified number of steps.
    
    Args:
        steps: Number of simulation steps to run
        config: Configuration (uses defaults if None)
        verbose: Whether to print progress information
    
    Returns:
        Dictionary with simulation results and statistics
    """
    brain = create_simple_brain() if config is None else Brain(config)
    
    if verbose:
        print(f"Starting simulation with {steps} steps...")
        print(f"Brain initialized with {brain.config.get('neuron_count', 0)} neurons")
    
    stats = {
        "steps_run": 0,
        "initial_spikes": 0,
        "final_spikes": 0,
        "max_spike_rate": 0.0,
        "neuron_count": brain.config.get("neuron_count", 0),
        "connection_probability": brain.config.get("connection_probability", 0.0),
        "simulation_timestep": brain.config.get("simulation_timestep", 0.0)
    }
    
    # Record initial state
    initial_stats = brain.get_stats()
    stats["initial_spikes"] = initial_stats.get("spikes", 0)
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        if verbose and (step + 1) % 100 == 0:
            current_stats = brain.get_stats()
            print(f"Step {step + 1}/{steps}, Spikes: {current_stats.get('spikes', 0)}")
    
    # Record final state
    final_stats = brain.get_stats()
    stats["steps_run"] = steps
    stats["final_spikes"] = final_stats.get("spikes", 0)
    
    # Calculate statistics
    spike_increase = stats["final_spikes"] - stats["initial_spikes"]
    if steps > 0:
        stats["max_spike_rate"] = spike_increase / steps
    
    if verbose:
        print(f"Simulation complete!")
        print(f"Steps run: {stats['steps_run']}")
        print(f"Spikes generated: {spike_increase}")
        print(f"Max spike rate: {stats['max_spike_rate']:.4f} spikes/step")
    
    return stats


def experiment_learning(
    duration: int = 1000,
    input_strength: float = 30.0,
    config: Optional[Config] = None
) -> Dict[str, Any]:
    """
    Run a learning experiment demonstrating synaptic plasticity.
    
    This creates a scenario where neurons receive repeated input to demonstrate
    learning through spike-timing dependent plasticity (STDP) and Hebbian learning.
    
    Args:
        duration: Number of simulation steps
        input_strength: Strength of sensory input injected
        config: Brain configuration
    
    Returns:
        Dictionary with learning metrics and statistics
    """
    brain = create_simple_brain() if config is None else Brain(config)
    
    # Enable plasticity
    if hasattr(brain, 'enable_plasticity'):
        brain.enable_plasticity(True)
    
    # Record initial state
    initial_stats = brain.get_stats()
    initial_weights = []
    
    # Run learning experiment
    for step in range(duration):
        # Inject sensory input pattern
        if hasattr(brain, 'inject_current_to_neurons'):
            brain.inject_current_to_neurons(NeuronType.Sensory if CORE_AVAILABLE else "Sensory", input_strength)
        
        brain.step(step)
        
        # Log progress every 100 steps
        if (step + 1) % 100 == 0:
            current_stats = brain.get_stats()
            print(f"Learning step {step + 1}/{duration}, Spikes: {current_stats.get('spikes', 0)}")
    
    # Record final state
    final_stats = brain.get_stats()
    
    # Calculate learning metrics
    learning_result = {
        "steps_run": duration,
        "initial_spikes": initial_stats.get("spikes", 0),
        "final_spikes": final_stats.get("spikes", 0),
        "spike_increase": final_stats.get("spikes", 0) - initial_stats.get("spikes", 0),
        "learning_detected": False,
        "config": brain.config.getKeys() if hasattr(brain.config, 'getKeys') else []
    }
    
    # Determine if learning occurred
    spike_increase = learning_result["spike_increase"]
    if spike_increase > 10:  # Threshold for detecting learning
        learning_result["learning_detected"] = True
        learning_result["significance"] = "strong"
    elif spike_increase > 0:
        learning_result["learning_detected"] = True
        learning_result["significance"] = "weak"
    else:
        learning_result["significance"] = "none"
    
    return learning_result


# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Export public API
__all__ = [
    # Core classes (when available)
    "Brain", "Config", "AgentBrain", "SimpleWorld",
    "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept",
    
    # Types and constants
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
    "create_simple_brain", "run_simulation", "experiment_learning",
    
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Module info
    "__version__", "__author__", "__license__",
    
    # Status flags
    "CORE_AVAILABLE", "ADVANCED_CONFIG_AVAILABLE"
]

# Aliases for backward compatibility
if CORE_AVAILABLE:
    # If core bindings are available, use them directly
    from .pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
else:
    # Provide placeholder implementations
    def createDefaultConfig():
        return Config()
    
    def createBrain(config: Optional[Config] = None):
        return Brain(config)
    
    def createSimpleWorld():
        return Brain()  # Placeholder
    
    def createAgentBrain(brain: Brain):
        return brain  # Placeholder