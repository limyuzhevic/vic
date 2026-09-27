#!/usr/bin/env python3
"""
NLM Core Python Bindings
==========================

This is the main Python interface to the NLM (Neural Learning Machine) framework.
It provides the core functionality for creating and simulating artificial brains.

This module is automatically generated from the C++ bindings and provides
Pythonic access to NLM's neural simulation capabilities.

Key Features:
- Brain creation and simulation
- World and environment management
- Agent brain interface
- Sensory input/output handling
- Configuration management
- Resource management utilities

Author: NLM Development Team
License: MIT
Version: 1.0.0
Release: 2026-09-27
"""

import numpy as np
from typing import Optional, List, Dict, Any, Callable, Union, Tuple
from dataclasses import dataclass
from enum import Enum
import time
import json
import logging
from contextlib import contextmanager
import warnings

# Configure logging
logging.basicConfig(level=logging.WARNING)
logger = logging.getLogger(__name__)

print(f"NLM Core Python Bindings v1.0.0 loading...")

# Try to import the actual C++ bindings
try:
    # This simulates the pybind11 bindings
    # In a real implementation, this would come from the compiled module
    import sys
    import importlib.util
    
    # Check if pynlm module exists (from C++ bindings)
    if 'pynlm' not in sys.modules:
        # Create mock implementations for demonstration
        _create_mock_bindings()
    else:
        # Use actual bindings
        from pynlm import (
            Config, Brain, AgentBrain, SimpleWorld,
            Vision, Audio, InternalSignals, Action,
            SensoryInput, SensoryPercept, WorldObject,
            AgentBody, ActionResult, NeuronId, SynapseId,
            RegionId, PopulationId, NeuronType, SynapseType,
            DevelopmentalStage, ActionType, MotorCommand,
            WorldObjectType, FiringState, SimulationStep,
            Timestamp, MembranePotential, Delay
        )
        PYNLM_AVAILABLE = True
    
except ImportError:
    print("Warning: pynlm module not found. Creating mock bindings for demonstration.")
    _create_mock_bindings()
    PYNLM_AVAILABLE = False

# Mock binding implementations for demonstration
_mock_bindings_created = False


def _create_mock_bindings():
    """Create mock bindings for demonstration purposes."""
    global _mock_bindings_created
    
    if _mock_bindings_created:
        return
    
    _mock_bindings_created = True
    
    # Mock classes for demonstration
    class MockNeuronId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"NeuronId({self.value})"
    
    class MockSynapseId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"SynapseId({self.value})"
    
    class MockRegionId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"RegionId({self.value})"
    
    class MockPopulationId:
        def __init__(self, value=0):
            self.value = value
        def __repr__(self):
            return f"PopulationId({self.value})"
    
    # Enums
    class NeuronType(Enum):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Sensory = 3
        Motor = 4
        Internal = 5
    
    class SynapseType(Enum):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Electrical = 3
        GapJunction = 4
    
    class DevelopmentalStage(Enum):
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class ActionType(Enum):
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
    
    class MotorCommand(Enum):
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        LookLeft = 4
        LookRight = 5
        Interact = 6
        Wait = 7
    
    class WorldObjectType(Enum):
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    class FiringState(Enum):
        Resting = 0
        Active = 1
        Refractory = 2
        Inhibited = 3
    
    # Type definitions
    NeuronId = MockNeuronId
    SynapseId = MockSynapseId
    RegionId = MockRegionId
    PopulationId = MockPopulationId
    
    # Mock Config class
    class MockConfig:
        def __init__(self):
            self._data = {}
            self._defaults = {
                "brain.neuron_count": 1000,
                "brain.region_count": 1,
                "brain.connection_probability": 0.1,
                "brain.initial_weight_mean": 0.5,
                "brain.initial_weight_std": 0.1,
                "brain.v_thresh": -50.0,
                "brain.v_rest": -70.0,
                "brain.v_reset": -75.0,
                "brain.tau_mem": 20.0,
                "brain.tau_ref": 2.0,
                "plasticity.stdp.enable": True,
                "plasticity.stdp.learning_rate": 0.001,
                "plasticity.stdp.tau_plus": 20.0,
                "plasticity.stdp.tau_minus": 20.0,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True
            }
        
        def loadFromFile(self, filepath):
            """Load configuration from JSON file."""
            try:
                with open(filepath, 'r') as f:
                    data = json.load(f)
                self._data.update(data)
                return True
            except Exception as e:
                logger.error(f"Failed to load config from {filepath}: {e}")
                return False
        
        def saveToFile(self, filepath):
            """Save configuration to JSON file."""
            try:
                with open(filepath, 'w') as f:
                    json.dump(self._data, f, indent=2)
                return True
            except Exception as e:
                logger.error(f"Failed to save config to {filepath}: {e}")
                return False
        
        def has(self, key):
            """Check if configuration key exists."""
            return key in self._data or key in self._defaults
        
        def get(self, key, default=None):
            """Get configuration value."""
            return self._data.get(key, self._defaults.get(key, default))
        
        def set(self, key, value, source="user"):
            """Set configuration value."""
            self._data[key] = value
        
        def getKeys(self):
            """Get all configuration keys."""
            return list(self._data.keys()) + list(self._defaults.keys())
        
        def clear(self):
            """Clear all configuration entries."""
            self._data.clear()
        
        def summary(self):
            """Get configuration summary."""
            return f"Config with {len(self._data)} entries"
        
        def __repr__(self):
            return f"MockConfig({len(self._data)} entries)"
    
    # Mock SensoryInput class
    class MockSensoryInput:
        def __init__(self):
            self._type = "Vision"
            self._data = np.zeros(64, dtype=np.float32)  # Default vision data
            self._timestamp = time.time()
        
        def getType(self):
            return self._type
        
        def getData(self):
            return self._data.tolist()
        
        def getDimensions(self):
            return len(self._data)
        
        def getTimestamp(self):
            return self._timestamp
        
        def setTimestamp(self, timestamp):
            self._timestamp = timestamp
    
    # Mock Vision class
    class MockVision(MockSensoryInput):
        def __init__(self, width=8, height=8, channels=3):
            super().__init__()
            self._width = width
            self._height = height
            self._channels = channels
            self._data = np.random.rand(width * height * channels).astype(np.float32)
        
        def getWidth(self):
            return self._width
        
        def getHeight(self):
            return self._height
        
        def getChannels(self):
            return self._channels
    
    # Mock Audio class
    class MockAudio(MockSensoryInput):
        def __init__(self, sampleRate=44100, numSamples=1024):
            super().__init__()
            self._sampleRate = sampleRate
            self._numSamples = numSamples
            self._data = np.random.randn(numSamples).astype(np.float32)
        
        def getSampleRate(self):
            return self._sampleRate
        
        def getNumSamples(self):
            return self._numSamples
    
    # Mock InternalSignals class
    class MockInternalSignals(MockSensoryInput):
        def __init__(self):
            super().__init__()
            self._signals = []
        
        def addSignal(self, value):
            self._signals.append(value)
        
        def clearSignals(self):
            self._signals.clear()
    
    # Mock Action class
    class MockAction:
        def __init__(self, action_type=None, parameters=None):
            self._type = action_type or ActionType.Wait
            self._parameters = parameters or []
        
        def getType(self):
            return self._type
        
        def setType(self, action_type):
            self._type = action_type
        
        def getParameters(self):
            return self._parameters
        
        def setParameters(self, parameters):
            self._parameters = parameters
        
        def getName(self):
            return self._type.name
        
        def clone(self):
            return MockAction(self._type, self._parameters.copy())
        
        def __repr__(self):
            return f"Action({self._type.name})"
    
    # Mock WorldObject class
    class MockWorldObject:
        def __init__(self, x=0.0, y=0.0, obj_type=WorldObjectType.Empty, value=0.0, radius=0.5):
            self.x = x
            self.y = y
            self.type = obj_type
            self.value = value
            self.radius = radius
            self.active = True
        
        def __repr__(self):
            return f"WorldObject({self.type.name} at ({self.x:.1f}, {self.y:.1f}))"
    
    # Mock AgentBody class
    class MockAgentBody:
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
            self.reset()
        
        def reset(self):
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
            self.lastActionTime = time.time()
    
    # Mock SensoryPercept class
    class MockSensoryPercept:
        def __init__(self):
            self._vision = MockVision()
            self._touch = []
            self._internal = []
            self._proprioception = []
            self._audio = MockAudio()
            self._timestamp = time.time()
        
        def getVision(self):
            return self._vision
        
        def setVision(self, vision):
            self._vision = vision
        
        def getVisionWidth(self):
            return self._vision.getWidth()
        
        def getVisionHeight(self):
            return self._vision.getHeight()
        
        def getTouch(self):
            return self._touch
        
        def setTouch(self, touch):
            self._touch = touch
        
        def getInternal(self):
            return self._internal
        
        def setInternal(self, internal):
            self._internal = internal
        
        def getProprioception(self):
            return self._proprioception
        
        def setProprioception(self, proprioception):
            self._proprioception = proprioception
        
        def getAudio(self):
            return self._audio
        
        def setAudio(self, audio):
            self._audio = audio
        
        def getAllSignals(self):
            signals = []
            signals.extend(self._touch)
            signals.extend(self._internal)
            signals.extend(self._proprioception)
            return signals
        
        def getTimestamp(self):
            return self._timestamp
        
        def setTimestamp(self, timestamp):
            self._timestamp = timestamp
    
    # Mock SimpleWorld class
    class MockSimpleWorld:
        def __init__(self):
            self._width = 20
            self._height = 20
            self._visionWidth = 8
            self._visionHeight = 8
            self._agent_x = 10.0
            self._agent_y = 10.0
            self._simulation_time = 0.0
            self._max_energy = 100.0
            self._energy_decay_rate = 0.01
            self._random_seed = 42
            self._agent_body = MockAgentBody()
            self._sensory_percept = MockSensoryPercept()
            self._objects = []
            self._initialized = False
        
        def configure(self, width=20, height=20, visionWidth=8, visionHeight=8):
            self._width = width
            self._height = height
            self._visionWidth = visionWidth
            self._visionHeight = visionHeight
            self._sensory_percept = MockSensoryPercept()
            self._sensory_percept.setVision(MockVision(width, height))
        
        def reset(self):
            self._agent_body.reset()
            self._simulation_time = 0.0
            self._objects.clear()
            self._sensory_percept = MockSensoryPercept()
            self._sensory_percept.setVision(MockVision(self._width, self._height))
            self._initialized = True
        
        def setAgentStart(self, x, y):
            self._agent_x = x
            self._agent_y = y
            self._agent_body.x = x
            self._agent_body.y = y
        
        def update(self, timestep):
            self._simulation_time += timestep
            self._agent_body.energy *= (1.0 - self._energy_decay_rate)
            self._agent_body.age += timestep
            self._agent_body.lastActionTime = self._simulation_time
        
        def applyMotorCommand(self, command, current_time):
            if command == MotorCommand.MoveForward:
                self._agent_body.velocityX += 0.5
                self._agent_body.isMoving = True
            elif command == MotorCommand.MoveBackward:
                self._agent_body.velocityX -= 0.5
                self._agent_body.isMoving = True
            elif command == MotorCommand.TurnLeft:
                self._agent_body.angularVelocity -= 0.1
                self._agent_body.isTurning = True
            elif command == MotorCommand.TurnRight:
                self._agent_body.angularVelocity += 0.1
                self._agent_body.isTurning = True
                
            self._agent_body.x += self._agent_body.velocityX * 0.1
            self._agent_body.y += self._agent_body.velocityY * 0.1
            self._agent_body.orientation += self._agent_body.angularVelocity * 0.1
            
            # Update agent body
            self._agent_body.velocityX *= 0.95  # Friction
            self._agent_body.velocityY *= 0.95
            self._agent_body.angularVelocity *= 0.95
            
        def getSensoryPercept(self):
            # Simulate vision - get what's in front of agent
            vision_data = np.zeros(self._visionWidth * self._visionHeight * 3)
            for i, obj in enumerate(self._objects):
                # Simple distance-based vision
                dx = obj.x - self._agent_body.x
                dy = obj.y - self._agent_body.y
                dist = (dx**2 + dy**2)**0.5
                if dist < 20.0:  # Vision range
                    # Create visual representation
                    for x in range(self._visionWidth):
                        for y in range(self._visionHeight):
                            # Very simple vision
                            pass
            return self._sensory_percept
        
        def getAgentBody(self):
            return self._agent_body
        
        def addObject(self, obj):
            self._objects.append(obj)
        
        def removeObject(self, x, y):
            self._objects = [obj for obj in self._objects if abs(obj.x - x) > 0.1 or abs(obj.y - y) > 0.1]
        
        def isValidPosition(self, x, y):
            return 0 <= x < self._width and 0 <= y < self._height
        
        def getWidth(self):
            return self._width
        
        def getHeight(self):
            return self._height
        
        def getMaxEnergy(self):
            return self._max_energy
        
        def setMaxEnergy(self, e):
            self._max_energy = e
        
        def getEnergyDecayRate(self):
            return self._energy_decay_rate
        
        def setEnergyDecayRate(self, r):
            self._energy_decay_rate = r
        
        def getSimulationTime(self):
            return self._simulation_time
        
        def setRandomSeed(self, seed):
            self._random_seed = seed
            np.random.seed(seed)
        
        def getRandomSeed(self):
            return self._random_seed
    
    # Mock Brain class
    class MockBrain:
        def __init__(self, config):
            self._config = config
            self._neurons = []
            self._synapses = []
            self._regions = []
            self._total_neurons = config.get("brain.neuron_count", 1000)
            self._total_synapses = int(self._total_neurons * 0.1)
            self._firing_neurons = 0
            self._total_spikes = 0
            self._average_firing_rate = 0.0
            self._initialized = False
            self._developmental_stage = DevelopmentalStage.Initial
            self._simulation_step = 0
        
        def initialize(self):
            """Initialize the brain."""
            self._initialized = True
            self._neurons = [MockNeuron() for _ in range(self._total_neurons)]
            self._total_spikes = 0
            self._firing_neurons = 0
            print(f"Brain initialized with {self._total_neurons} neurons and {self._total_synapses} synapses")
        
        def step(self, step_number, time=None):
            """Perform a simulation step."""
            if not self._initialized:
                self.initialize()
                
            self._simulation_step = step_number
            current_time = time if time is not None else step_number * 0.1
            
            # Simulate neural activity
            self._update_neural_dynamics()
            self._propagate_spikes()
            self._update_plasticity()
            self._update_developmental_stage()
            
        def _update_neural_dynamics(self):
            """Update neural dynamics."""
            self._firing_neurons = 0
            for neuron in self._neurons:
                if neuron.update_dynamics():
                    self._firing_neurons += 1
                    self._total_spikes += 1
        
        def _propagate_spikes(self):
            """Propagate spikes through synapses."""
            # Simplified spike propagation
            pass
        
        def _update_plasticity(self):
            """Update synaptic plasticity."""
            # Simplified plasticity update
            pass
        
        def _update_developmental_stage(self):
            """Update developmental stage."""
            age = self._simulation_step * 0.1
            if age >= 100:
                self._developmental_stage = DevelopmentalStage.Adult
            elif age >= 50:
                self._developmental_stage = DevelopmentalStage.Maturation
            elif age >= 20:
                self._developmental_stage = DevelopmentalStage.CriticalPeriod
        
        def reset(self):
            """Reset brain state."""
            self._total_spikes = 0
            self._firing_neurons = 0
            self._simulation_step = 0
            self._developmental_stage = DevelopmentalStage.Initial
            print("Brain reset")
        
        def save(self, filepath):
            """Save brain state to file."""
            try:
                data = {
                    'total_neurons': self._total_neurons,
                    'total_synapses': self._total_synapses,
                    'firing_neurons': self._firing_neurons,
                    'total_spikes': self._total_spikes,
                    'developmental_stage': self._developmental_stage.name,
                    'simulation_step': self._simulation_step
                }
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)
                return True
            except Exception as e:
                logger.error(f"Failed to save brain to {filepath}: {e}")
                return False
        
        def load(self, filepath):
            """Load brain state from file."""
            try:
                with open(filepath, 'r') as f:
                    data = json.load(f)
                self._total_neurons = data['total_neurons']
                self._total_synapses = data['total_synapses']
                self._firing_neurons = data['firing_neurons']
                self._total_spikes = data['total_spikes']
                self._developmental_stage = DevelopmentalStage[data['developmental_stage']]
                self._simulation_step = data['simulation_step']
                self._initialized = True
                return True
            except Exception as e:
                logger.error(f"Failed to load brain from {filepath}: {e}")
                return False
        
        def addRegion(self, name=""):
            """Add a neural region."""
            region_id = len(self._regions)
            self._regions.append(f"Region_{region_id}")
            return region_id
        
        def getRegion(self, id):
            """Get region by ID."""
            if 0 <= id < len(self._regions):
                return self._regions[id]
            return None
        
        def getRegionCount(self):
            """Get number of regions."""
            return len(self._regions)
        
        def getRegionIds(self):
            """Get all region IDs."""
            return list(range(len(self._regions)))
        
        def getRegions(self):
            """Get all regions."""
            return self._regions
        
        def getTotalNeuronCount(self):
            """Get total neuron count."""
            return self._total_neurons
        
        def getTotalSynapseCount(self):
            """Get total synapse count."""
            return self._total_synapses
        
        def getActiveNeuronCount(self):
            """Get active neuron count."""
            return self._firing_neurons
        
        def getFiringNeuronCount(self):
            """Get firing neuron count."""
            return self._firing_neurons
        
        def getAverageFiringRate(self):
            """Get average firing rate."""
            return self._average_firing_rate
        
        def getExcitationInhibitionRatio(self):
            """Get excitation/inhibition ratio."""
            return 1.5  # Default value
        
        def getTotalSpikeCount(self):
            """Get total spike count."""
            return self._total_spikes
        
        def getDevelopmentalStage(self):
            """Get developmental stage."""
            return self._developmental_stage
        
        def setDevelopmentalStage(self, stage):
            """Set developmental stage."""
            self._developmental_stage = stage
        
        def getConfig(self):
            """Get configuration."""
            return self._config
        
        def logStatus(self):
            """Log brain status."""
            print(f"Brain Status: {self._total_neurons} neurons, {self._firing_neurons} firing, "
                  f"{self._total_spikes} total spikes, Stage: {self._developmental_stage.name}")
        
        def receiveSensoryInput(self, input_data):
            """Receive sensory input."""
            pass
        
        def injectCurrent(self, neuron_id, current):
            """Inject current into specific neuron."""
            pass
        
        def injectCurrentToNeurons(self, neuron_type, current):
            """Inject current into all neurons of a specific type."""
            pass
        
        def produceAction(self):
            """Produce motor action."""
            return MockAction(ActionType.Wait)
    
    # Mock Neuron class for Brain
    class MockNeuron:
        def __init__(self):
            self._voltage = -70.0
            self._refractory = 0.0
            self._firing = False
        
        def update_dynamics(self):
            """Update neuron dynamics."""
            if self._refractory > 0:
                self._refractory -= 0.1
                self._firing = False
            else:
                # Simple integration
                self._voltage += 0.1  # Input current
                if self._voltage > -50:  # Threshold
                    self._firing = True
                    self._voltage = -75.0  # Reset
                    self._refractory = 5.0  # Refractory period
                else:
                    self._firing = False
            return self._firing
    
    # Mock AgentBrain class
    class MockAgentBrain:
        def __init__(self, brain):
            self._brain = brain
            self._world = None
            self._reward_modulation_enabled = False
            self._structural_plasticity_enabled = False
            self._development_enabled = False
            self._curiosity_enabled = False
        
        def initialize(self, world):
            """Initialize agent brain."""
            self._world = world
        
        def getSensoryInputSize(self):
            """Get expected sensory input size."""
            return 64  # Default vision size
        
        def getMotorOutputSize(self):
            """Get expected motor output size."""
            return 8  # Default motor commands
        
        def processSensoryInput(self, percept):
            """Process sensory input."""
            pass
        
        def decodeMotorCommand(self):
            """Decode motor command."""
            return MotorCommand.Wait
        
        def applyRewardModulation(self, reward, predicted_reward):
            """Apply reward modulation."""
            if self._reward_modulation_enabled:
                pass
        
        def updateDevelopment(self, timestep):
            """Update development."""
            if self._development_enabled:
                # Update brain development
                stage = self._brain.getDevelopmentalStage()
                if stage == DevelopmentalStage.CriticalPeriod:
                    # Enhanced learning during critical period
                    pass
        
        def getDevelopmentalStage(self):
            """Get developmental stage."""
            return self._brain.getDevelopmentalStage()
        
        def getNeuromodulationLevel(self):
            """Get neuromodulation level."""
            return 0.5
        
        def getCuriosityLevel(self):
            """Get curiosity level."""
            return 0.3
        
        def getNoveltyLevel(self):
            """Get novelty level."""
            return 0.2
        
        def getPredictionError(self):
            """Get prediction error."""
            return 0.1
        
        def reset(self):
            """Reset agent brain."""
            pass
        
        def getBrain(self):
            """Get underlying brain."""
            return self._brain
        
        def enableRewardModulation(self, enable):
            """Enable/disable reward modulation."""
            self._reward_modulation_enabled = enable
        
        def enableStructuralPlasticity(self, enable):
            """Enable/disable structural plasticity."""
            self._structural_plasticity_enabled = enable
        
        def enableDevelopment(self, enable):
            """Enable/disable development."""
            self._development_enabled = enable
        
        def enableCuriosity(self, enable):
            """Enable/disable curiosity."""
            self._curiosity_enabled = enable
        
        def isRewardModulationEnabled(self):
            """Check if reward modulation enabled."""
            return self._reward_modulation_enabled
        
        def isStructuralPlasticityEnabled(self):
            """Check if structural plasticity enabled."""
            return self._structural_plasticity_enabled
        
        def isDevelopmentEnabled(self):
            """Check if development enabled."""
            return self._development_enabled
        
        def isCuriosityEnabled(self):
            """Check if curiosity enabled."""
            return self._curiosity_enabled
    
    # Mock action result class
    class MockActionResult:
        def __init__(self, reward=0.0, success=False, message=""):
            self.reward = reward
            self.success = success
            self.message = message
    
    # Mock constants
    INVALID_NEURON_ID = -1
    INVALID_SYNAPSE_ID = -1
    INVALID_REGION_ID = -1
    INVALID_POPULATION_ID = -1
    
    SimulationStep = int
    Timestamp = float
    MembranePotential = float
    Delay = int
    
    # Register all classes in module namespace
    globals().update({
        'Config': MockConfig,
        'Brain': MockBrain,
        'AgentBrain': MockAgentBrain,
        'SimpleWorld': MockSimpleWorld,
        'Vision': MockVision,
        'Audio': MockAudio,
        'InternalSignals': MockInternalSignals,
        'Action': MockAction,
        'SensoryInput': MockSensoryInput,
        'SensoryPercept': MockSensoryPercept,
        'WorldObject': MockWorldObject,
        'AgentBody': MockAgentBody,
        'ActionResult': MockActionResult,
        'NeuronId': MockNeuronId,
        'SynapseId': MockSynapseId,
        'RegionId': MockRegionId,
        'PopulationId': MockPopulationId,
        'NeuronType': NeuronType,
        'SynapseType': SynapseType,
        'DevelopmentalStage': DevelopmentalStage,
        'ActionType': ActionType,
        'MotorCommand': MotorCommand,
        'WorldObjectType': WorldObjectType,
        'FiringState': FiringState,
        'SimulationStep': SimulationStep,
        'Timestamp': Timestamp,
        'MembranePotential': MembranePotential,
        'Delay': Delay,
        'INVALID_NEURON_ID': INVALID_NEURON_ID,
        'INVALID_SYNAPSE_ID': INVALID_SYNAPSE_ID,
        'INVALID_REGION_ID': INVALID_REGION_ID,
        'INVALID_POPULATION_ID': INVALID_POPULATION_ID,
    })

# Create configuration function
def createDefaultConfig():
    """Create default configuration."""
    return Config()

# Create brain function
def createBrain(config):
    """Create brain with configuration."""
    return Brain(config)

# Create simple world function
def createSimpleWorld():
    """Create simple world."""
    return SimpleWorld()

# Create agent brain function
def createAgentBrain(brain):
    """Create agent brain interface."""
    return AgentBrain(brain)

# Version information
__version__ = "1.0.0"
__author__ = "NLM Development Team"
__license__ = "MIT"
__copyright__ = "Copyright (c) 2026 NLM Development Team"

# Print startup message
if PYNLM_AVAILABLE:
    print(f"NLM Core Python Bindings v{__version__} loaded successfully.")
else:
    print("NLM Core Python Bindings v{__version__} loaded with mock implementations for demonstration.")
    print("Note: This is a mock implementation for demonstration purposes only.")
    print("For actual functionality, install the compiled pynlm module.")

# Export key functions and classes
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'Vision', 'Audio', 'InternalSignals', 'Action',
    'SensoryInput', 'SensoryPercept', 'WorldObject',
    'AgentBody', 'ActionResult', 'NeuronId', 'SynapseId',
    'RegionId', 'PopulationId', 'NeuronType', 'SynapseType',
    'DevelopmentalStage', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'FiringState', 'SimulationStep',
    'Timestamp', 'MembranePotential', 'Delay',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain',
    
    # Version info
    '__version__', '__author__', '__license__',
    '__copyright__'
]