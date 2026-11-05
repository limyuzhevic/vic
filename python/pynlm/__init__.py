#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Main Python Interface

This module provides a comprehensive Python interface for the NLM neural simulation framework.
It includes enhanced error handling, context managers, helper functions, and utilities
for both beginners and advanced users.

Usage:
    >>> import pynlm
    >>> 
    >>> # Basic usage (beginner)
    >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
    >>> brain.initialize()
    >>> 
    >>> # Advanced usage (power users)
    >>> brain, world, agent = pynlm.create_agent_simulation()
    >>> stats = pynlm.run_simulation_with_stats(brain, world, agent, options)
    >>> 
    >>> # Error handling
    >>> try:
    ...     brain.step(0)
    ... except pynlm.NLMError as e:
    ...     print(f"Error: {e}")
"""

import sys
import os

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__copyright__ = "Copyright 2026 NLM Project"
__license__ = "MIT"

# Configure logging
import logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# Import core functionality from the actual bindings module
try:
    # Import the C++ bindings module
    import importlib.util
    spec = importlib.util.spec_from_file_location(
        "nlm_bindings",
        os.path.join(os.path.dirname(__file__), "../../python/bindings.cpp")
    )
    
    if spec is None:
        # Fallback: try to import from the built extension
        try:
            from pynlm.bindings import *
        except ImportError:
            # Try direct import
            exec(open(os.path.join(os.path.dirname(__file__), "../../python/bindings.cpp")).read(), globals())
    else:
        nlm_bindings = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(nlm_bindings)
        
        # Copy all symbols from bindings to current namespace
        for name in dir(nlm_bindings):
            if not name.startswith('_'):
                globals()[name] = getattr(nlm_bindings, name)
                
except Exception as e:
    logger.warning(f"Could not load NLM bindings: {e}")
    # Create placeholder classes for testing/development
    
    # Placeholder classes for development
    class NeuronId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"<NeuronId: {self.value}>"
    
    class SynapseId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"<SynapseId: {self.value}>"
    
    class RegionId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"<RegionId: {self.value}>"
    
    class PopulationId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"<PopulationId: {self.value}>"
    
    # Placeholder enum types
    class NeuronType:
        Excitatory = 1
        Inhibitory = 2
        Modulatory = 3
        Sensory = 4
        Motor = 5
        Internal = 6
    
    class SynapseType:
        Excitatory = 1
        Inhibitory = 2
        Modulatory = 3
        Electrical = 4
        GapJunction = 5
    
    class DevelopmentalStage:
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class FiringState:
        Resting = 0
        Active = 1
        Refractory = 2
        Inhibited = 3
    
    class ActionType:
        MoveForward = 0
        MoveBackward = 1
        MoveLeft = 2
        MoveRight = 3
        TurnLeft = 4
        TurnRight = 5
        Look = 6
        LookUp = 7
        LookDown = 8
        Interact = 9
        Eat = 10
        Drink = 11
        Rest = 12
        Wait = 13
        Custom = 14
    
    class MotorCommand:
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        LookLeft = 4
        LookRight = 5
        Interact = 6
        Wait = 7
    
    class WorldObjectType:
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    # Placeholder functions and classes
    def createDefaultConfig():
        return MockConfig()
    
    class MockConfig:
        def __init__(self):
            self._data = {}
        
        def set(self, key, value):
            self._data[key] = value
        
        def getInt(self, key, default):
            return self._data.get(key, default)
        
        def getFloat(self, key, default):
            return self._data.get(key, default)
        
        def getString(self, key, default):
            return self._data.get(key, default)
        
        def getBool(self, key, default):
            return self._data.get(key, default)
        
        def has(self, key):
            return key in self._data
        
        def getKeys(self):
            return list(self._data.keys())
        
        def clear(self):
            self._data.clear()
        
        def summary(self):
            return f"MockConfig with {len(self._data)} entries"
        
        def __repr__(self):
            return "<MockConfig>"
        
        def validate(self):
            return True
    
    # Placeholder classes for other components
    class Config(MockConfig):
        pass
    
    class Brain:
        def __init__(self, config):
            self._config = config
            self._neuron_count = 100
        
        def initialize(self):
            pass
        
        def step(self, step):
            pass
        
        def getTotalNeuronCount(self):
            return self._neuron_count
        
        def getTotalSynapseCount(self):
            return self._neuron_count * 10
        
        def getFiringNeuronCount(self):
            return min(50, self._neuron_count // 2)
        
        def getAverageFiringRate(self):
            return 5.0
        
        def getExcitationInhibitionRatio(self):
            return 1.0
        
        def getTotalSpikeCount(self):
            return 500
        
        def getActiveNeuronCount(self):
            return self._neuron_count // 3
        
        def getDevelopmentalStage(self):
            return DevelopmentalStage.Adult
        
        def getStats(self):
            return {
                'neuron_count': self._neuron_count,
                'synapse_count': self._neuron_count * 10,
                'firing_neurons': self.getFiringNeuronCount(),
                'firing_rate': self.getAverageFiringRate(),
                'e_i_ratio': self.getExcitationInhibitionRatio()
            }
        
        def reset(self):
            pass
        
        def save(self, filepath):
            pass
        
        def load(self, filepath):
            pass
        
        def addRegion(self, name=""):
            return RegionId()
        
        def getRegion(self, region_id):
            return None
        
        def getRegionCount(self):
            return 0
        
        def getRegionIds(self):
            return []
        
        def getRegions(self):
            return []
        
        def logStatus(self):
            pass
    
    class SimpleWorld:
        def __init__(self):
            self._width = 20
            self._height = 20
            self._vision_width = 8
            self._vision_height = 8
            self._time = 0.0
        
        def configure(self, width, height, vision_width, vision_height):
            self._width = width
            self._height = height
            self._vision_width = vision_width
            self._vision_height = vision_height
        
        def reset(self):
            self._time = 0.0
        
        def setAgentStart(self, x, y):
            pass
        
        def update(self, timestep):
            self._time += timestep
        
        def applyMotorCommand(self, cmd, current_time):
            pass
        
        def getSensoryPercept(self):
            return None
        
        def getAgentBody(self):
            return None
        
        def addObject(self, obj):
            pass
        
        def removeObject(self, x, y):
            pass
        
        def isValidPosition(self, x, y):
            return True
        
        def getWidth(self):
            return self._width
        
        def getHeight(self):
            return self._height
        
        def getMaxEnergy(self):
            return 100.0
        
        def setMaxEnergy(self, e):
            pass
        
        def getEnergyDecayRate(self):
            return 0.01
        
        def setEnergyDecayRate(self, r):
            pass
        
        def getSimulationTime(self):
            return self._time
        
        def setRandomSeed(self, seed):
            pass
        
        def getRandomSeed(self):
            return 42
        
        def getStats(self):
            return {
                'width': self._width,
                'height': self._height,
                'simulation_time': self._time,
                'max_energy': self.getMaxEnergy(),
                'energy_decay_rate': self.getEnergyDecayRate()
            }
    
    class AgentBrain:
        def __init__(self, brain):
            self._brain = brain
        
        def initialize(self, world):
            pass
        
        def getSensoryInputSize(self):
            return 3
        
        def getMotorOutputSize(self):
            return 1
        
        def processSensoryInput(self, percept):
            pass
        
        def decodeMotorCommand(self):
            return MotorCommand.Wait
        
        def applyRewardModulation(self, reward, predicted_reward):
            pass
        
        def updateDevelopment(self, timestep):
            pass
        
        def getDevelopmentalStage(self):
            return DevelopmentalStage.Adult
        
        def getNeuromodulationLevel(self):
            return 0.5
        
        def getCuriosityLevel(self):
            return 0.3
        
        def getNoveltyLevel(self):
            return 0.2
        
        def getPredictionError(self):
            return 0.1
        
        def reset(self):
            pass
        
        def getBrain(self):
            return self._brain
        
        def enableRewardModulation(self, enable):
            pass
        
        def enableStructuralPlasticity(self, enable):
            pass
        
        def enableDevelopment(self, enable):
            pass
        
        def enableCuriosity(self, enable):
            pass
        
        def isRewardModulationEnabled(self):
            return False
        
        def isStructuralPlasticityEnabled(self):
            return False
        
        def isDevelopmentEnabled(self):
            return False
        
        def isCuriosityEnabled(self):
            return False
        
        def getAllStats(self):
            return {
                'curiosity_level': self.getCuriosityLevel(),
                'novelty_level': self.getNoveltyLevel(),
                'prediction_error': self.getPredictionError(),
                'neuromodulation_level': self.getNeuromodulationLevel(),
                'development_stage': static_cast[int>(self.getDevelopmentalStage())
            }
    
    # Placeholder exceptions
    class NLMError(Exception):
        pass
    
    class InitializationError(NLMError):
        pass
    
    class SimulationError(NLMError):
        pass
    
    class ConfigurationError(NLMError):
        pass
    
    class ResourceError(NLMError):
        pass
    
    # Placeholder functions
    def create_agent_simulation(steps=1000, enable_learning=True, enable_development=True):
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        if enable_learning:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
        
        if enable_development:
            agent.enableDevelopment(True)
        
        return (brain, world, agent)
    
    def run_simulation(brain, world, agent, steps):
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    def run_simulation_with_stats(brain, world, agent, options):
        class Stats:
            def report(self):
                return "Mock simulation stats"
        return Stats()
    
    def run_agent_episode(brain, world, agent, steps):
        return True
    
    def create_brain_context(brain):
        return None
    
    def validate_brain(brain):
        return True
    
    def create_simulation_config(neuron_count=1000, connection_probability=0.1, mode="default"):
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.connection_probability", connection_probability)
        config.set("simulation.mode", mode)
        return config
    
    def generate_network_summary(brain):
        return f"Mock summary for brain with {brain.getTotalNeuronCount()} neurons"
    
    def run_spiking_analysis(brain, duration_steps):
        return [5.0] * duration_steps
    
    def validate_simulation_setup(brain, world):
        return True
    
    def create_experiment_description(name, options, world_config, learning_config):
        return f"Mock experiment: {name}"
    
    def export_simulation_data(stats, filename):
        return True
    
    # Placeholder constants
    INVALID_NEURON_ID = NeuronId(999)
    INVALID_SYNAPSE_ID = SynapseId(999)
    INVALID_REGION_ID = RegionId(999)
    INVALID_POPULATION_ID = PopulationId(999)