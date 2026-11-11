# pynlm.py - Python wrapper for NLM C++ bindings

"""
NLM (熙然) - Neural Learning Machine Python Bindings

An experimental artificial developmental brain framework that creates
virtual brains capable of learning, remembering, and making decisions.

This module provides Python bindings to the NLM C++ neural simulation framework.
It wraps the core functionality including Brain creation, configuration,
Agent brains for world interaction, and neural computation.

Example usage:
    import pynlm
    
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Make the brain think for 100 steps
    for i in range(100):
        brain.step(i)
    
    print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
"""

import sys
import os
import struct
import ctypes
from pathlib import Path
from typing import List, Optional, Union, Dict, Any, Callable

# Try to import the compiled module
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Forward declarations for type hints
class NeuronId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value & 0xFFFFFFFF
    
    def __eq__(self, other):
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<NeuronId: {self.value}>"

class SynapseId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value & 0xFFFFFFFF
    
    def __eq__(self, other):
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<SynapseId: {self.value}>"

class RegionId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value & 0xFFFFFFFF
    
    def __eq__(self, other):
        return isinstance(other, RegionId) and self.value == other.value
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __repr__(self):
        return f"<RegionId: {self.value}>"

class PopulationId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value & 0xFFFFFFFF

# Enum types
class NeuronType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5
    
    @classmethod
    def to_string(cls, value: int) -> str:
        for name, val in cls.__dict__.items():
            if name.isupper() and val == value:
                return name
        return f"Unknown({value})"

class SynapseType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4
    
    @classmethod
    def to_string(cls, value: int) -> str:
        for name, val in cls.__dict__.items():
            if name.isupper() and val == value:
                return name
        return f"Unknown({value})"

class DevelopmentalStage:
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4
    
    @classmethod
    def to_string(cls, value: int) -> str:
        for name, val in cls.__dict__.items():
            if name.isupper() and val == value:
                return name
        return f"Unknown({value})"

# Try to import the C++ module
try:
    import pynlm as pynlm_cpp
    _CPP_AVAILABLE = True
except ImportError:
    _CPP_AVAILABLE = False
    print("Warning: C++ module not found. Some functionality will be limited.")
    print("Try running 'python setup.py build_ext --inplace' to build the module.")

# Mock classes for when C++ module is not available
def _create_mock_class(name: str):
    class MockClass:
        def __init__(self, *args, **kwargs):
            pass
        def __repr__(self):
            return f"<{name} mock>"
    MockClass.__name__ = name
    return MockClass

if not _CPP_AVAILABLE:
    # Create mock classes for all C++ types
    NeuronId = _create_mock_class("NeuronId")
    SynapseId = _create_mock_class("SynapseId")
    RegionId = _create_mock_class("RegionId")
    PopulationId = _create_mock_class("PopulationId")
    
    # Mock other classes
    Config = _create_mock_class("Config")
    Brain = _create_mock_class("Brain")
    AgentBrain = _create_mock_class("AgentBrain")
    SimpleWorld = _create_mock_class("SimpleWorld")
    Reward = _create_mock_class("Reward")

# Pythonic wrapper functions
def createDefaultConfig() -> 'Config':
    """
    Create a default configuration for the brain.
    
    Returns:
        Config: A configuration object with reasonable defaults
    """
    if _CPP_AVAILABLE:
        return pynlm_cpp.createDefaultConfig()
    else:
        return Config()

def createBrain(config: 'Config') -> 'Brain':
    """
    Create a brain with the given configuration.
    
    Args:
        config: Brain configuration
        
    Returns:
        Brain: A new brain instance
    """
    if _CPP_AVAILABLE:
        return pynlm_cpp.createBrain(config)
    else:
        return Brain()

def createAgentBrain(brain: 'Brain') -> 'AgentBrain':
    """
    Create an agent brain that connects a brain to a world.
    
    Args:
        brain: The brain to connect
        
    Returns:
        AgentBrain: An agent that can process sensory input and produce actions
    """
    if _CPP_AVAILABLE:
        return pynlm_cpp.createAgentBrain(brain)
    else:
        return AgentBrain(brain)

def createSimpleWorld() -> 'SimpleWorld':
    """
    Create a simple world for the agent to interact with.
    
    Returns:
        SimpleWorld: A simple world instance
    """
    if _CPP_AVAILABLE:
        return pynlm_cpp.createSimpleWorld()
    else:
        return SimpleWorld()

# Convenience functions for common patterns
def run_simple_simulation(config: 'Config', world: 'SimpleWorld', 
                         agent: 'AgentBrain', num_steps: int = 1000) -> Dict[str, Any]:
    """
    Run a simple simulation with brain, world, and agent.
    
    Args:
        config: Brain configuration
        world: The world to simulate
        agent: The agent interacting with the world
        num_steps: Number of simulation steps
        
    Returns:
        Dict: Summary of simulation results
    """
    brain = createBrain(config)
    brain.initialize()
    
    total_reward = 0.0
    total_firing_rate = 0.0
    max_firing_rate = 0.0
    
    for step in range(num_steps):
        # Update world
        world.update(0.001)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        action_result = world.applyMotorCommand(action, step * 0.001)
        
        # Accumulate rewards
        if action_result.reward != 0.0:
            agent.applyRewardModulation(action_result.reward, 0.0)
        
        total_firing_rate += brain.getAverageFiringRate()
        max_firing_rate = max(max_firing_rate, brain.getAverageFiringRate())
        
        if step % 100 == 0:
            print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f} Hz")
    
    return {
        'final_firing_rate': brain.getAverageFiringRate(),
        'max_firing_rate': max_firing_rate,
        'total_spikes': brain.getTotalSpikeCount(),
        'reward_system': agent.rewardSystem if hasattr(agent, 'rewardSystem') else None,
        'brain_stats': {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
    }

def create_experimental_brain(neuron_count: int = 500, 
                               connection_probability: float = 0.1) -> 'Brain':
    """
    Create an experimental brain with specific parameters.
    
    Args:
        neuron_count: Number of neurons in the brain
        connection_probability: Probability of connections between neurons
        
    Returns:
        Brain: An experimental brain instance
    """
    config = createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("connection_probability", connection_probability)
    return createBrain(config)

def create_experimental_world(width: int = 10, height: int = 10,
                              vision_width: int = 8, vision_height: int = 8) -> 'SimpleWorld':
    """
    Create an experimental world for testing.
    
    Args:
        width: World width
        height: World height
        vision_width: Agent's vision width
        vision_height: Agent's vision height
        
    Returns:
        SimpleWorld: An experimental world instance
    """
    world = createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    return world

def create_complete_agent(brain_config: Optional['Config'] = None,
                          world_config: Optional[Dict[str, Any]] = None) -> 'AgentBrain':
    """
    Create a complete agent with brain and world.
    
    Args:
        brain_config: Brain configuration (uses default if None)
        world_config: World configuration (uses defaults if None)
        
    Returns:
        AgentBrain: A complete agent ready for simulation
    """
    if brain_config is None:
        brain_config = createDefaultConfig()
    
    brain = createBrain(brain_config)
    brain.initialize()
    
    if world_config is None:
        world = createSimpleWorld()
        world.configure(16, 16, 16, 16)  # Default world size
    else:
        world = createSimpleWorld()
        world.configure(
            world_config.get('width', 16),
            world_config.get('height', 16),
            world_config.get('vision_width', 16),
            world_config.get('vision_height', 16)
        )
    
    world.reset()
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return agent

# Utility functions
def get_version() -> str:
    """Get the NLM version."""
    return "0.1.0 (Phase 6 integration)"

def list_available_features() -> List[str]:
    """List available features of the NLM Python module."""
    features = [
        "Brain creation and initialization",
        "Neuron and synapse management",
        "Memory systems (working, episodic, associative)",
        "Neuromodulation (dopamine, curiosity, novelty)",
        "Prediction and planning systems",
        "Development stages and plasticity",
        "Checkpoint save/load",
        "Replay and consolidation",
        "Agent world interaction",
        "Reward-based learning",
        "Text processing capabilities",
        "Self-model and social learning"
    ]
    return features

# Error handling
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Raised for configuration errors."""
    pass

class SimulationError(NLMError):
    """Raised for simulation errors."""
    pass

# Add the module to __all__
__all__ = [
    'createDefaultConfig',
    'createBrain',
    'createAgentBrain',
    'createSimpleWorld',
    'create_experimental_brain',
    'create_experimental_world',
    'create_complete_agent',
    'run_simple_simulation',
    'get_version',
    'list_available_features',
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage'
]