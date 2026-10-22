# NLM Python Bindings
# This file provides the Python interface to the C++ NLM bindings

import sys
import os
from typing import Dict, List, Optional, Any, Union

# Add the src directory to the path so we can import the C++ headers
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

# Import the C++ bindings using pybind11
# The actual bindings are created by pybind11 in bindings.cpp
try:
    # Try to import from the compiled extension module
    from . import pynlm_ext as _nlm_ext
    
    # If that fails, try to import the Python interface
    try:
        from . import pynlm as _pynlm_interface
        # Use the interface as fallback
        _nlm_ext = _pynlm_interface
    except ImportError:
        # Create a minimal interface for basic functionality
        print("Warning: NLM C++ extension not found. Creating minimal Python interface.")
        
        # Create a mock interface with basic functionality
        class MockConfig:
            def __init__(self):
                self._data = {}
            
            def set(self, key, value, source=None):
                self._data[key] = value
            
            def get(self, key):
                return self._data.get(key)
            
            def has(self, key):
                return key in self._data
            
            def getKeys(self):
                return list(self._data.keys())
        
        class MockBrain:
            def __init__(self, config):
                self.config = config
                self._step_count = 0
                self._neuron_count = 1000
                self._synapse_count = 10000
                self._total_spikes = 0
                self._firing_neurons = 100
            
            def initialize(self):
                pass
            
            def step(self, step, time=None):
                self._step_count = step
                self._total_spikes += 100
                self._firing_neurons = 100
            
            def reset(self):
                self._step_count = 0
                self._total_spikes = 0
            
            def getTotalNeuronCount(self):
                return self._neuron_count
            
            def getTotalSynapseCount(self):
                return self._synapse_count
            
            def getFiringNeuronCount(self):
                return self._firing_neurons
            
            def getTotalSpikeCount(self):
                return self._total_spikes
            
            def getAverageFiringRate(self):
                return 0.1
            
            def getExcitationInhibitionRatio(self):
                return 1.0
            
            def getDevelopmentalStage(self):
                return "Adult"
        
        class MockSimpleWorld:
            def __init__(self):
                self._agent_x = 0
                self._agent_y = 0
                self._simulation_time = 0
            
            def configure(self, width, height, vision_width, vision_height):
                pass
            
            def reset(self):
                pass
            
            def update(self, timestep):
                self._simulation_time += timestep
            
            def getSensoryPercept(self):
                class MockPercept:
                    def getVision(self):
                        return []
                    def getTouch(self):
                        return []
                    def getInternal(self):
                        return []
                    def getProprioception(self):
                        return []
                    def getAudio(self):
                        return []
                return MockPercept()
            
            def getAgentBody(self):
                class MockBody:
                    x = 0.0
                    y = 0.0
                    orientation = 0.0
                    velocityX = 0.0
                    velocityY = 0.0
                return MockBody()
        
        # Create the mock extension module
        class MockNLMExtension:
            Config = MockConfig
            Brain = MockBrain
            SimpleWorld = MockSimpleWorld
            
            # Enums
            class NeuronType:
                Excitatory = 0
                Inhibitory = 1
                Modulatory = 2
                Sensory = 3
                Motor = 4
                Internal = 5
            
            class SynapseType:
                Excitatory = 0
                Inhibitory = 1
                Modulatory = 2
                Electrical = 3
                GapJunction = 4
            
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
            
            # Constants
            INVALID_NEURON_ID = -1
            INVALID_SYNAPSE_ID = -1
            INVALID_REGION_ID = -1
            INVALID_POPULATION_ID = -1
            
            # Factory functions
            def createDefaultConfig():
                return MockConfig()
            
            def createBrain(config):
                return MockBrain(config)
            
            def createSimpleWorld():
                return MockSimpleWorld()
        
        _nlm_ext = MockNLMExtension()
    
    # Export all symbols from the extension
    from .pynlm import (
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        Vision,
        Audio,
        InternalSignals,
        SensoryInput,
        Action,
        WorldObject,
        AgentBody,
        ActionResult,
        SensoryPercept,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        NLMError,
        ConfigurationError,
        SimulationError,
        brain_context,
        run_simulation,
        create_agent_world,
        run_experiment,
        NeuralNetworkAnalyzer,
        validate_configuration,
        save_config_to_json,
        load_config_from_json,
        NLMInterface,
        create_brain_with_config,
    )
    
except ImportError as e:
    print(f"Warning: Could not import NLM bindings: {e}")
    
    # Create minimal fallback implementation
    class Config:
        def __init__(self):
            self._data = {}
        
        def set(self, key, value, source=None):
            self._data[key] = value
        
        def get(self, key):
            return self._data.get(key)
        
        def has(self, key):
            return key in self._data
        
        def getKeys(self):
            return list(self._data.keys())
        
        def getOr(self, key, default_value):
            return self._data.get(key, default_value)
        
        def clear(self):
            self._data.clear()
        
        def summary(self):
            return f"Config with {len(self._data)} entries"
    
    class Brain:
        def __init__(self, config):
            self.config = config
        
        def initialize(self):
            pass
        
        def step(self, step, time=None):
            pass
        
        def reset(self):
            pass
        
        def getTotalNeuronCount(self):
            return 1000
        
        def getTotalSynapseCount(self):
            return 10000
        
        def getFiringNeuronCount(self):
            return 100
        
        def getTotalSpikeCount(self):
            return 1000
        
        def getAverageFiringRate(self):
            return 0.1
        
        def getExcitationInhibitionRatio(self):
            return 1.0
        
        def getDevelopmentalStage(self):
            return "Adult"
    
    # Create the main extension object
    _nlm_ext = type('NLMExtension', (), {
        'Config': Config,
        'Brain': Brain,
        'SimpleWorld': None,
        'createDefaultConfig': lambda: Config(),
        'createBrain': lambda config: Brain(config),
        'createSimpleWorld': lambda: None,
        'createAgentBrain': lambda brain: None,
    })()

# Re-export all symbols
__all__ = [
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'brain_context',
    'run_simulation',
    'create_agent_world',
    'run_experiment',
    'NeuralNetworkAnalyzer',
    'validate_configuration',
    'save_config_to_json',
    'load_config_from_json',
    'NLMInterface',
    'create_brain_with_config',
]