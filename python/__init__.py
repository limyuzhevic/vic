"""
NLM - Neural Learning Machine

A comprehensive Python API for the experimental artificial developmental brain system.

This module provides access to:
- Neural brain simulation with spiking neurons
- Working, episodic, and semantic memory systems
- Neuromodulation (dopamine, curiosity, novelty detection)
- Prediction and planning systems
- Cognitive mechanisms (concept formation, attention)
- Developmental stages and structural plasticity
- Sensory and motor processing
- Environment interaction

Example usage:

    import pynlm
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simple simulation
    for step in range(100):
        brain.step(step)
    
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
"""

import sys
from typing import Optional, List, Dict, Any

# Import the high-level Python bindings
try:
    from .bindings import *
except ImportError as e:
    raise ImportError(
        f"NLM Python bindings failed to load: {e}\n\n"
        "Please ensure you have:\n"
        "1. Built the C++ native module: mkdir build && cd build && cmake .. && make\n"
        "2. Installed the Python bindings: pip install -e .\n"
        "3. Or install via pip: pip install pynlm"
    ) from e

# Re-export all high-level API
try:
    from .bindings import *
except ImportError:
    # Define fallback imports for development
    import typing as _typing
    
    # Create type aliases
    NeuronId = Any
    SynapseId = Any
    RegionId = Any
    PopulationId = Any
    
    # Create enum classes
    class _EnumWrapper:
        def __init__(self, value):
            self.value = value
        
        def __repr__(self):
            return f"{self.__class__.__name__}.{self.value}"
    
    class NeuronType(_EnumWrapper):
        Excitatory = "Excitatory"
        Inhibitory = "Inhibitory"
        Modulatory = "Modulatory"
        Sensory = "Sensory"
        Motor = "Motor"
        Internal = "Internal"
    
    class SynapseType(_EnumWrapper):
        Excitatory = "Excitatory"
        Inhibitory = "Inhibitory"
        Modulatory = "Modulatory"
        Electrical = "Electrical"
        GapJunction = "GapJunction"
    
    # Create placeholder classes for development
    class Config:
        def __init__(self):
            self._data = {}
        
        def loadFromFile(self, filepath):
            return False
        
        def loadFromArgs(self, argc, argv):
            return False
        
        def saveToFile(self, filepath):
            return False
        
        def has(self, key):
            return False
        
        def getKeys(self):
            return []
        
        def clear(self):
            pass
        
        def summary(self):
            return "Config(debug=True)"
        
        def set(self, key, value, source=None):
            self._data[key] = value
    
    class Brain:
        def __init__(self, config):
            self.config = config
            self._initialized = False
        
        def initialize(self):
            self._initialized = True
        
        def step(self, step, time=0.0):
            pass
        
        def reset(self):
            pass
        
        def getTotalNeuronCount(self):
            return 100
        
        def getTotalSynapseCount(self):
            return 500
        
        def getFiringNeuronCount(self):
            return 10
        
        def getAverageFiringRate(self):
            return 0.1
        
        def getTotalSpikeCount(self):
            return 50
        
        def getActiveNeuronCount(self):
            return 15
        
        def getExcitationInhibitionRatio(self):
            return 1.5
        
        def getDevelopmentalStage(self):
            return 2
        
        def save(self, filepath):
            return True
        
        def load(self, filepath):
            return True
        
        def logStatus(self):
            print("Brain status: Initialized")
    
    # Create factory functions
    def create_default_config():
        return Config()
    
    def create_brain(config=None):
        if config is None:
            config = create_default_config()
        return Brain(config)
    
    def create_simple_world():
        return SimpleWorld()
    
    # Create world class
    class SimpleWorld:
        def __init__(self):
            pass
        
        def configure(self, width, height, vision_width, vision_height):
            pass
        
        def reset(self):
            pass
        
        def update(self, timestep):
            pass
        
        def getSensoryPercept(self):
            return SensoryPercept()
        
        def applyMotorCommand(self, cmd, time):
            pass
        
        def getAgentBody(self):
            return AgentBody()
        
        def getWidth(self):
            return 20
        
        def getHeight(self):
            return 20
        
        def getSimulationTime(self):
            return 0.0
    
    # Create agent class
    class AgentBrain:
        def __init__(self, brain):
            self.brain = brain
        
        def initialize(self, world):
            pass
        
        def getSensoryInputSize(self):
            return 256
        
        def getMotorOutputSize(self):
            return 6
        
        def processSensoryInput(self, percept):
            pass
        
        def decodeMotorCommand(self):
            return MotorCommand.Wait
        
        def applyRewardModulation(self, reward, predicted):
            pass
        
        def updateDevelopment(self, timestep):
            pass
        
        def getDevelopmentalStage(self):
            return DevelopmentalStage.Initial
        
        def getCuriosityLevel(self):
            return 0.1
        
        def getNoveltyLevel(self):
            return 0.1
        
        def enableRewardModulation(self, enable):
            pass
        
        def enableStructuralPlasticity(self, enable):
            pass
        
        def enableDevelopment(self, enable):
            pass
        
        def enableCuriosity(self, enable):
            pass
    
    class AgentBody:
        def __init__(self):
            self.x = 0.0
            self.y = 0.0
            self.orientation = 0.0
            self.velocityX = 0.0
            self.velocityY = 0.0
            self.angularVelocity = 0.0
            self.energy = 100.0
            self.health = 1.0
            self.age = 0.0
            self.isMoving = False
            self.isTurning = False
            self.lastActionTime = 0.0
        
        def reset(self):
            self.x = 0.0
            self.y = 0.0
    
    class SensoryPercept:
        def __init__(self):
            pass
        
        def getVision(self):
            return []
        
        def setVision(self, vision):
            pass
        
        def getTouch(self):
            return []
        
        def setTouch(self, touch):
            pass
        
        def getInternal(self):
            return []
        
        def setInternal(self, internal):
            pass
        
        def getProprioception(self):
            return []
        
        def setProprioception(self, proprioception):
            pass
        
        def getAudio(self):
            return []
        
        def setAudio(self, audio):
            pass
    
    class MotorCommand:
        Wait = "Wait"
        MoveForward = "MoveForward"
        MoveBackward = "MoveBackward"
        TurnLeft = "TurnLeft"
        TurnRight = "TurnRight"
        LookLeft = "LookLeft"
        LookRight = "LookRight"
        Interact = "Interact"
    
    class DevelopmentalStage:
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class ActionType:
        MoveForward = "MoveForward"
        MoveBackward = "MoveBackward"
        MoveLeft = "MoveLeft"
        MoveRight = "MoveRight"
        TurnLeft = "TurnLeft"
        TurnRight = "TurnRight"
        Look = "Look"
        LookUp = "LookUp"
        LookDown = "LookDown"
        Interact = "Interact"
        Eat = "Eat"
        Drink = "Drink"
        Rest = "Rest"
        Wait = "Wait"
        Custom = "Custom"
    
    # Create enum instances
    NeuronType.Excitatory = _EnumWrapper("Excitatory")
    NeuronType.Inhibitory = _EnumWrapper("Inhibitory")
    NeuronType.Modulatory = _EnumWrapper("Modulatory")
    NeuronType.Sensory = _EnumWrapper("Sensory")
    NeuronType.Motor = _EnumWrapper("Motor")
    NeuronType.Internal = _EnumWrapper("Internal")
    
    SynapseType.Excitatory = _EnumWrapper("Excitatory")
    SynapseType.Inhibitory = _EnumWrapper("Inhibitory")
    SynapseType.Modulatory = _EnumWrapper("Modulatory")
    SynapseType.Electrical = _EnumWrapper("Electrical")
    SynapseType.GapJunction = _EnumWrapper("GapJunction")
    
    # Define constants
    INVALID_NEURON_ID = NeuronId()
    INVALID_SYNAPSE_ID = SynapseId()
    INVALID_REGION_ID = RegionId()
    INVALID_POPULATION_ID = PopulationId()

# Factory functions

def create_default_config() -> Config:
    """Create a default NLM configuration with sensible defaults."""
    return Config()

def create_brain(config=None) -> Brain:
    """Create a new NLM brain with configuration."""
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_simple_world() -> SimpleWorld:
    """Create a simple NLM world for simulation."""
    return SimpleWorld()

def create_agent_brain(brain, world=None) -> AgentBrain:
    """Create an agent brain interface for brain-world interaction."""
    agent = AgentBrain(brain)
    if world:
        agent.initialize(world)
    return agent

# Define types for type hints
from typing import Callable, Optional

BrainInitializer = Callable[[], None]
SimulationStep = Callable[[int, float], Dict[str, Any]]

# Convenience functions for common operations

def simple_demo(neuron_count=200, steps=50) -> Dict[str, Any]:
    """Simple brain demonstration."""
    config = create_default_config()
    config.set("neuron_count", neuron_count)
    
    brain = create_brain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
    }

def environment_demo(width=20, height=20, vision_width=8, vision_height=8,
                    num_steps=100) -> Dict[str, Any]:
    """Environment interaction demonstration."""
    # Create brain and world
    config = create_default_config()
    config.set("neuron_count", 300)
    
    brain = create_brain(config)
    brain.initialize()
    
    world = create_simple_world()
    world.configure(width, height, vision_width, vision_height)
    
    # Create agent
    agent = create_agent_brain(brain, world)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'world_width': width,
        'world_height': height,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
    }

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__description__ = "High-level Python API for the NLM (Neural Learning Machine) experimental brain simulation"

# Backward compatibility aliases
createDefaultConfig = create_default_config
createBrain = create_brain
createSimpleWorld = create_simple_world
createAgentBrain = create_agent_brain

__all__ = [
    # Core classes (fallback for development)
    'Config', 'Brain', 'AgentBrain', 'AgentBody', 'SimpleWorld',
    'SensoryPercept', 'MotorCommand', 'DevelopmentalStage', 'ActionType',
    
    # Factory functions
    'create_default_config', 'create_brain', 'create_simple_world', 
    'create_agent_brain',
    
    # Demos and examples
    'simple_demo', 'environment_demo',
    
    # Module metadata
    '__version__', '__author__', '__license__', '__description__',
    
    # Backward compatibility
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
]
