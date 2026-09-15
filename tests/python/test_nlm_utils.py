"""
Test utilities for NLM Python bindings.

This module provides utility functions for testing the NLM Python bindings,
including setup, teardown, and common test patterns.
"""

import sys
import os
import tempfile
import json
import time
from typing import Any, Dict, List, Optional, Union
import numpy as np

# Try to import pynlm, but handle the case where it's not available
try:
    # Import the NLM Python bindings
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    # Create a mock module for testing when pynlm is not available
    class MockPynlm:
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
        
        class NeuronId:
            def __init__(self, value=0):
                self.value = value
            def index(self):
                return self.value
            def __eq__(self, other):
                return isinstance(other, self.__class__) and self.value == other.value
            def __ne__(self, other):
                return not self == other
            def __hash__(self):
                return hash(self.value)
            def __repr__(self):
                return f"<NeuronId: {self.value}>"
        
        class SynapseId:
            def __init__(self, value=0):
                self.value = value
            def index(self):
                return self.value
            def __eq__(self, other):
                return isinstance(other, self.__class__) and self.value == other.value
            def __ne__(self, other):
                return not self == other
            def __hash__(self):
                return hash(self.value)
            def __repr__(self):
                return f"<SynapseId: {self.value}>"
        
        class RegionId:
            def __init__(self, value=0):
                self.value = value
            def index(self):
                return self.value
            def __eq__(self, other):
                return isinstance(other, self.__class__) and self.value == other.value
            def __ne__(self, other):
                return not self == other
            def __hash__(self):
                return hash(self.value)
            def __repr__(self):
                return f"<RegionId: {self.value}>"
        
        class PopulationId:
            def __init__(self, value=0):
                self.value = value
            def index(self):
                return self.value
            def __eq__(self, other):
                return isinstance(other, self.__class__) and self.value == other.value
            def __ne__(self, other):
                return not self == other
        
        class Config:
            def __init__(self):
                self._data = {}
            
            def has(self, key):
                return key in self._data
            
            def set(self, key, value, source=None):
                self._data[key] = value
            
            def get(self, key):
                return OptionalValue(self._data.get(key))
            
            def getOr(self, key, default):
                return self._data.get(key, default)
            
            def remove(self, key):
                if key in self._data:
                    del self._data[key]
            
            def getKeys(self):
                return list(self._data.keys())
            
            def clear(self):
                self._data.clear()
            
            def summary(self):
                return f"Config with {len(self._data)} entries"
            
            def saveToFile(self, filepath):
                # Simple JSON serialization for testing
                with open(filepath, 'w') as f:
                    json.dump(self._data, f, indent=2)
                return True
            
            def loadFromFile(self, filepath):
                try:
                    with open(filepath, 'r') as f:
                        self._data = json.load(f)
                    return True
                except:
                    return False
            
            def loadFromArgs(self, argc, argv):
                # Simple argument parsing
                i = 1
                while i < argc:
                    if argv[i].startswith('--'):
                        if '=' in argv[i]:
                            key, value = argv[i][2:].split('=', 1)
                            self.set(key, value)
                        i += 1
                    elif argv[i].startswith('-'):
                        key = argv[i][1:]
                        if i + 1 < argc:
                            self.set(key, argv[i + 1])
                            i += 2
                    else:
                        i += 1
                return True
        
        def createDefaultConfig():
            return Config()
        
        class Vision:
            def __init__(self, width=0, height=0, channels=0):
                self._width = width
                self._height = height
                self._channels = channels
                self._data = []
            
            def getWidth(self):
                return self._width
            
            def getHeight(self):
                return self._height
            
            def getChannels(self):
                return self._channels
            
            def setData(self, data):
                self._data = data
            
            def getData(self):
                return self._data
        
        class Audio:
            def __init__(self, sampleRate=0, numSamples=0):
                self._sampleRate = sampleRate
                self._numSamples = numSamples
                self._data = []
            
            def setData(self, data):
                self._data = data
            
            def setSampleRate(self, rate):
                self._sampleRate = rate
            
            def getSampleRate(self):
                return self._sampleRate
            
            def getNumSamples(self):
                return self._numSamples
        
        class InternalSignals:
            def __init__(self):
                self._signals = []
            
            def addSignal(self, value):
                self._signals.append(value)
            
            def clearSignals(self):
                self._signals.clear()
        
        class Action:
            def __init__(self, action_type=None, parameters=None):
                self._type = action_type
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
                if self._type:
                    return str(self._type).split('.')[-1]
                return "Custom"
            
            def clone(self):
                return Action(self._type, self._parameters.copy())
        
        class WorldObject:
            def __init__(self, x=0.0, y=0.0, obj_type=None, value=0.0, radius=0.5):
                self.x = x
                self.y = y
                self.type = obj_type
                self.value = value
                self.radius = radius
                self.active = True
        
        class AgentBody:
            def __init__(self):
                self.x = 0.0
                self.y = 0.0
                self.orientation = 0.0
                self.velocityX = 0.0
                self.velocityY = 0.0
                self.angularVelocity = 0.0
                self.energy = 100.0
                self.health = 100.0
                self.age = 0
                self.isMoving = False
                self.isTurning = False
                self.lastActionTime = 0.0
            
            def reset(self):
                self.x = 0.0
                self.y = 0.0
                self.orientation = 0.0
                self.velocityX = 0.0
                self.velocityY = 0.0
                self.angularVelocity = 0.0
                self.energy = 100.0
                self.health = 100.0
                self.age = 0
                self.isMoving = False
                self.isTurning = False
                self.lastActionTime = 0.0
        
        class ActionResult:
            def __init__(self, reward=0.0, success=False, message=""):
                self.reward = reward
                self.success = success
                self.message = message
        
        class SensoryPercept:
            def __init__(self):
                self._vision = []
                self._touch = []
                self._internal = []
                self._proprioception = []
                self._audio = []
                self._timestamp = 0
            
            def getVision(self):
                return self._vision
            
            def setVision(self, vision):
                self._vision = vision
            
            def getVisionWidth(self):
                return len(self._vision[0]) if self._vision else 0
            
            def getVisionHeight(self):
                return len(self._vision) if self._vision else 0
            
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
                return {
                    'vision': self._vision,
                    'touch': self._touch,
                    'internal': self._internal,
                    'proprioception': self._proprioception,
                    'audio': self._audio,
                    'timestamp': self._timestamp
                }
            
            def getTimestamp(self):
                return self._timestamp
            
            def setTimestamp(self, timestamp):
                self._timestamp = timestamp
        
        class SimpleWorld:
            def __init__(self):
                self._width = 0
                self._height = 0
                self._visionWidth = 0
                self._visionHeight = 0
                self._agentX = 0.0
                self._agentY = 0.0
                self._agentBody = AgentBody()
                self._percept = SensoryPercept()
                self._objects = []
                self._simulationTime = 0.0
                self._maxEnergy = 100.0
                self._energyDecayRate = 0.01
                self._randomSeed = 0
            
            def configure(self, width=0, height=0, visionWidth=0, visionHeight=0):
                self._width = width
                self._height = height
                self._visionWidth = visionWidth
                self._visionHeight = visionHeight
            
            def reset(self):
                self._agentX = 0.0
                self._agentY = 0.0
                self._agentBody = AgentBody()
                self._percept = SensoryPercept()
                self._objects = []
                self._simulationTime = 0.0
            
            def setAgentStart(self, x, y):
                self._agentX = x
                self._agentY = y
                self._agentBody.x = x
                self._agentBody.y = y
            
            def update(self, timestep):
                self._simulationTime += timestep
            
            def applyMotorCommand(self, cmd, currentTime):
                # Simple movement
                if cmd == MockPynlm.MotorCommand.MoveForward:
                    self._agentX += 1.0 * timestep
                elif cmd == MockPynlm.MotorCommand.MoveBackward:
                    self._agentX -= 1.0 * timestep
                elif cmd == MockPynlm.MotorCommand.TurnLeft:
                    self._agentBody.orientation += 45.0 * timestep
                elif cmd == MockPynlm.MotorCommand.TurnRight:
                    self._agentBody.orientation -= 45.0 * timestep
                
                self._agentBody.x = self._agentX
                self._agentBody.y = self._agentY
                self._agentBody.isMoving = True
                self._agentBody.lastActionTime = currentTime
            
            def getSensoryPercept(self):
                return self._percept
            
            def getAgentBody(self):
                return self._agentBody
            
            def addObject(self, obj):
                self._objects.append(obj)
            
            def removeObject(self, x, y):
                self._objects = [o for o in self._objects if not (abs(o.x - x) < 0.1 and abs(o.y - y) < 0.1)]
            
            def isValidPosition(self, x, y):
                # Simple bounds checking
                return 0 <= x < self._width and 0 <= y < self._height
            
            def getWidth(self):
                return self._width
            
            def getHeight(self):
                return self._height
            
            def getMaxEnergy(self):
                return self._maxEnergy
            
            def setMaxEnergy(self, e):
                self._maxEnergy = e
            
            def getEnergyDecayRate(self):
                return self._energyDecayRate
            
            def setEnergyDecayRate(self, r):
                self._energyDecayRate = r
            
            def getSimulationTime(self):
                return self._simulationTime
            
            def setRandomSeed(self, seed):
                self._randomSeed = seed
            
            def getRandomSeed(self):
                return self._randomSeed
        
        # Global instances for functions
        _default_config = Config()
        
        def createDefaultConfig():
            return Config()
        
        def createBrain(config):
            class MockBrain:
                def __init__(self, config):
                    self._config = config
                    self._initialized = False
                    self._neuron_count = config.getOr("neuron_count", 100)
                    self._region_count = config.getOr("region_count", 1)
                
                def initialize(self):
                    self._initialized = True
                    return True
                
                def step(self, currentStep):
                    pass
                
                def step_with_time(self, currentStep, currentTime):
                    pass
                
                def getTotalNeuronCount(self):
                    return self._neuron_count
                
                def getTotalSynapseCount(self):
                    return self._neuron_count * 5  # Approximate
                
                def getActiveNeuronCount(self):
                    return 0
                
                def getFiringNeuronCount(self):
                    return 0
                
                def getAverageFiringRate(self):
                    return 0.0
                
                def getExcitationInhibitionRatio(self):
                    return 1.0
                
                def getTotalSpikeCount(self):
                    return 0
                
                def getDevelopmentalStage(self):
                    return MockPynlm.DevelopmentalStage.Adult
                
                def setDevelopmentalStage(self, stage):
                    pass
                
                def getConfig(self):
                    return self._config
                
                def getRegionCount(self):
                    return self._region_count
                
                def getRegionIds(self):
                    return [RegionId(i) for i in range(self._region_count)]
                
                def getRegions(self):
                    return []
                
                def addRegion(self, name=""):
                    return RegionId(self._region_count)
                
                def getRegion(self, rid):
                    return None
                
                def reset(self):
                    pass
                
                def save(self, filepath):
                    return True
                
                def load(self, filepath):
                    return True
                
                def logStatus(self):
                    pass
            
            return MockBrain(config)
        
        def createSimpleWorld():
            return SimpleWorld()
        
        def createAgentBrain(brain):
            class MockAgentBrain:
                def __init__(self, brain):
                    self._brain = brain
                
                def initialize(self, world):
                    pass
                
                def getSensoryInputSize(self):
                    return 8
                
                def getMotorOutputSize(self):
                    return 1
                
                def processSensoryInput(self, percept):
                    pass
                
                def decodeMotorCommand(self):
                    return MockPynlm.MotorCommand.Wait
                
                def applyRewardModulation(self, reward, predictedReward):
                    pass
                
                def updateDevelopment(self, timestep):
                    pass
                
                def getDevelopmentalStage(self):
                    return MockPynlm.DevelopmentalStage.Adult
                
                def getNeuromodulationLevel(self):
                    return 0.5
                
                def getCuriosityLevel(self):
                    return 0.1
                
                def getNoveltyLevel(self):
                    return 0.2
                
                def getPredictionError(self):
                    return 0.0
                
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
            
            return MockAgentBrain(brain)
        
        # Constants
        INVALID_NEURON_ID = NeuronId(-1)
        INVALID_SYNAPSE_ID = SynapseId(-1)
        INVALID_REGION_ID = RegionId(-1)
        INVALID_POPULATION_ID = PopulationId(-1)
        
        class OptionalValue:
            def __init__(self, value):
                self._value = value
            
            def get(self):
                return self._value
            
            def is_none(self):
                return self._value is None
    
    # Use the mock module if pynlm is not available
    if not PYNLM_AVAILABLE:
        pynlm = MockPynlm()


# Utility functions for testing
class ConfigTestHelper:
    """Helper class for testing Config functionality."""
    
    @staticmethod
    def create_test_config() -> 'pynlm.Config':
        """Create a config with test data."""
        config = pynlm.createDefaultConfig()
        
        # Add various data types for testing
        config.set("string_value", "test_string")
        config.set("int_value", 42)
        config.set("float_value", 3.14159)
        config.set("bool_value", True)
        config.set("int_list", [1, 2, 3, 4, 5])
        config.set("float_list", [1.1, 2.2, 3.3])
        config.set("string_list", ["a", "b", "c"])
        
        return config
    
    @staticmethod
    def test_config_operations(config: 'pynlm.Config') -> bool:
        """Test basic config operations."""
        try:
            # Test has
            assert not config.has("nonexistent")
            assert config.has("string_value")
            
            # Test get
            assert config.get("string_value").get() == "test_string"
            assert config.get("int_value").get() == 42
            assert abs(config.get("float_value").get() - 3.14159) < 0.0001
            assert config.get("bool_value").get() == True
            assert config.get("int_list").get() == [1, 2, 3, 4, 5]
            
            # Test getOr
            assert config.getOr("nonexistent", "default") == "default"
            assert config.getOr("string_value", "default") == "test_string"
            
            # Test remove
            config.set("temp", 123)
            assert config.has("temp")
            config.remove("temp")
            assert not config.has("temp")
            
            # Test clear
            config.clear()
            assert not config.has("string_value")
            
            # Test summary
            assert "Configuration" in config.summary()
            
            return True
        except Exception:
            return False
    
    @staticmethod
    def test_config_serialization(config: 'pynlm.Config') -> bool:
        """Test config serialization to/from file."""
        try:
            # Create temp file
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                filepath = f.name
                
                # Save config
                config.saveToFile(filepath)
                
                # Create new config and load
                new_config = pynlm.createDefaultConfig()
                success = new_config.loadFromFile(filepath)
                
                if success:
                    # Verify data
                    assert new_config.get("string_value").get() == "test_string"
                    assert new_config.get("int_value").get() == 42
                    assert abs(new_config.get("float_value").get() - 3.14159) < 0.0001
                
                return success
        except Exception:
            return False
        finally:
            if 'filepath' in locals():
                try:
                    os.unlink(filepath)
                except:
                    pass
    
    @staticmethod
    def test_config_from_args(config: 'pynlm.Config') -> bool:
        """Test config loading from command line arguments."""
        try:
            # Create args
            args = ['program', '--string=test', '--int=42', '-float=3.14', '--bool=true']
            
            # Load from args
            success = config.loadFromArgs(len(args), args)
            
            if success:
                # Verify data was loaded
                assert config.get("string").get() == "test"
                assert config.get("int").get() == 42
                assert abs(config.get("float").get() - 3.14) < 0.01
                # Note: bool handling might be different in implementation
            
            return success
        except Exception:
            return False


class TestScorer:
    """Utility class for scoring test performance."""
    
    @staticmethod
    def time_operation(func, *args, **kwargs) -> float:
        """Time an operation and return the elapsed time in seconds."""
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        return end_time - start_time, result
    
    @staticmethod
    def benchmark_operation(func, iterations: int, *args, **kwargs) -> Dict[str, float]:
        """Benchmark an operation over multiple iterations."""
        times = []
        
        for _ in range(iterations):
            start_time = time.time()
            result = func(*args, **kwargs)
            end_time = time.time()
            times.append(end_time - start_time)
        
        return {
            'min': min(times),
            'max': max(times),
            'mean': sum(times) / len(times),
            'std': (sum((t - sum(times)/len(times))**2 for t in times) / len(times))**0.5
        }


# Test result types for compatibility with different test frameworks
class TestResult:
    """Container for test results."""
    
    def __init__(self, passed: bool, message: str = "", duration: float = 0.0):
        self.passed = passed
        self.message = message
        self.duration = duration
        self.timestamp = time.time()
    
    def __bool__(self):
        return self.passed
    
    def __str__(self):
        status = "PASSED" if self.passed else "FAILED"
        return f"{status}: {self.message} (in {self.duration:.3f}s)"


# Export common test utilities
__all__ = [
    'PYNLM_AVAILABLE',
    'ConfigTestHelper',
    'TestScorer',
    'TestResult',
    'pynlm'
]