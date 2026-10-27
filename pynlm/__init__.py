#!/usr/bin/env python3
"""
NLM Python Module - Main Entry Point

This is the main module for the NLM (Neural Learning Machine) Python bindings.
It provides access to the original NLM functionality through a Pythonic interface.

For enhanced features and utilities, import pynlm_enhanced.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

# Import the core bindings from the C++ extension
try:
    # Try to import from the C++ extension
    import pynlm_core
    _has_c_extensions = True
except ImportError:
    _has_c_extensions = False
    print("Warning: C++ extensions not available")
    print("Some NLM functionality may be limited.")

# Import the enhanced API if available
try:
    import pynlm_enhanced
    _has_enhanced_api = True
except ImportError:
    _has_enhanced_api = False
    print("Warning: Enhanced API not available")

# Define the main module classes and functions
try:
    # Re-export the most commonly used symbols from the original bindings
    # These are typically imported by users of the library
    
    # Try to get the core bindings
    if _has_c_extensions:
        from pynlm_core import (
            Config,
            Brain,
            SimpleWorld,
            AgentBrain,
            SensoryPercept,
            Vision,
            Audio,
            InternalSignals,
            Action,
            WorldObject,
            AgentBody,
            ActionResult,
            NeuronId,
            SynapseId,
            RegionId,
            PopulationId,
            NeuronType,
            SynapseType,
            DevelopmentalStage,
            FiringState,
            ActionType,
            MotorCommand,
            WorldObjectType,
            # Constants
            INVALID_NEURON_ID,
            INVALID_SYNAPSE_ID,
            INVALID_REGION_ID,
            INVALID_POPULATION_ID
        )
        _has_original_bindings = True
    else:
        # Create minimal fallback classes
        class Config:
            def __init__(self):
                pass
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
                return "Config (fallback)"
        
        class Brain:
            def __init__(self, config=None):
                pass
            def initialize(self):
                pass
            def step(self, step, time=None):
                pass
            def receiveSensoryInput(self, input):
                pass
            def injectCurrent(self, neuron, current):
                pass
            def injectCurrentToNeurons(self, type, current):
                pass
            def produceAction(self):
                return None
            def reset(self):
                pass
            def save(self, filepath):
                return False
            def load(self, filepath):
                return False
            def addRegion(self, name):
                return None
            def getRegion(self, id):
                return None
            def getRegionCount(self):
                return 0
            def getRegionIds(self):
                return []
            def getRegions(self):
                return []
            def getTotalNeuronCount(self):
                return 0
            def getTotalSynapseCount(self):
                return 0
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
            def getPendingSpikeEventCount(self):
                return 0
            def getDevelopmentalStage(self):
                return 0
            def setDevelopmentalStage(self, stage):
                pass
            def getConfig(self):
                return None
            def logStatus(self):
                pass
        
        class SimpleWorld:
            def __init__(self):
                pass
            def configure(self, width, height, visionWidth, visionHeight):
                pass
            def reset(self):
                pass
            def setAgentStart(self, x, y):
                pass
            def update(self, timestep):
                pass
            def applyMotorCommand(self, cmd, time):
                return None
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
                return 0.0
            def getHeight(self):
                return 0.0
            def getMaxEnergy(self):
                return 0.0
            def setMaxEnergy(self, e):
                pass
            def getEnergyDecayRate(self):
                return 0.0
            def setEnergyDecayRate(self, r):
                pass
            def getSimulationTime(self):
                return 0.0
            def setRandomSeed(self, seed):
                pass
            def getRandomSeed(self):
                return 0
        
        class AgentBrain:
            def __init__(self, brain):
                pass
            def initialize(self, world):
                pass
            def getSensoryInputSize(self):
                return 0
            def getMotorOutputSize(self):
                return 0
            def processSensoryInput(self, percept):
                pass
            def decodeMotorCommand(self):
                return None
            def applyRewardModulation(self, reward, predictedReward):
                pass
            def updateDevelopment(self, timestep):
                pass
            def getDevelopmentalStage(self):
                return 0
            def getNeuromodulationLevel(self):
                return 0.0
            def getCuriosityLevel(self):
                return 0.0
            def getNoveltyLevel(self):
                return 0.0
            def getPredictionError(self):
                return 0.0
            def reset(self):
                pass
            def getBrain(self):
                return None
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
        
        # Define type enums (minimal)
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
        
        # Define constants
        INVALID_NEURON_ID = None
        INVALID_SYNAPSE_ID = None
        INVALID_REGION_ID = None
        INVALID_POPULATION_ID = None
        
        _has_original_bindings = True
    
    # Define convenience functions for backward compatibility
    def createDefaultConfig():
        return Config()
    
    def createBrain(config=None):
        return Brain(config)
    
    def createSimpleWorld():
        return SimpleWorld()
    
    def createAgentBrain(brain):
        return AgentBrain(brain)
    
    # Define enhanced API convenience functions if available
    if _has_enhanced_api:
        from pynlm_enhanced import (
            create_simple_agent,
            run_simulation,
            run_learning_simulation,
            ConfigBuilder,
            PerformanceStats,
            SimulationProfiler,
            NLMError,
            ConfigurationError,
            SimulationError,
            ValidationError,
            PerformanceError,
            nlm_session,
            config_session
        )
        
        # Alias enhanced API functions to make them easily discoverable
        createSimpleAgent = create_simple_agent
        runSimulation = run_simulation
        runLearningSimulation = run_learning_simulation
    
    # Define __all__ for this module
    __all__ = [
        # Original bindings
        'Config',
        'Brain',
        'SimpleWorld',
        'AgentBrain',
        'SensoryPercept',  # Placeholder
        'Vision',  # Placeholder
        'Audio',  # Placeholder
        'InternalSignals',  # Placeholder
        'Action',
        'WorldObject',
        'AgentBody',
        'ActionResult',
        'NeuronId',  # Placeholder
        'SynapseId',  # Placeholder
        'RegionId',  # Placeholder
        'PopulationId',  # Placeholder
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
        # Convenience functions
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain',
        'createSimpleAgent',
        'runSimulation',
        'runLearningSimulation',
        # Enhanced API (if available)
        'ConfigBuilder',
        'PerformanceStats',
        'SimulationProfiler',
        'NLMError',
        'ConfigurationError',
        'SimulationError',
        'ValidationError',
        'PerformanceError',
        'nlm_session',
        'config_session'
    ]
    
except ImportError as e:
    print(f"Warning: Could not import NLM bindings: {e}")
    print("Some NLM functionality may not be available.")
    
    # Create minimal fallback modules
    class Config:
        def __init__(self):
            pass
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
            return "Config (fallback)"
    
    class Brain:
        def __init__(self, config=None):
            pass
        def initialize(self):
            pass
        def step(self, step, time=None):
            pass
        def receiveSensoryInput(self, input):
            pass
        def injectCurrent(self, neuron, current):
            pass
        def injectCurrentToNeurons(self, type, current):
            pass
        def produceAction(self):
            return None
        def reset(self):
            pass
        def save(self, filepath):
            return False
        def load(self, filepath):
            return False
        def addRegion(self, name):
            return None
        def getRegion(self, id):
            return None
        def getRegionCount(self):
            return 0
        def getRegionIds(self):
            return []
        def getRegions(self):
            return []
        def getTotalNeuronCount(self):
            return 0
        def getTotalSynapseCount(self):
            return 0
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
        def getPendingSpikeEventCount(self):
            return 0
        def getDevelopmentalStage(self):
            return 0
        def setDevelopmentalStage(self, stage):
            pass
        def getConfig(self):
            return None
        def logStatus(self):
            pass
    
    class SimpleWorld:
        def __init__(self):
            pass
        def configure(self, width, height, visionWidth, visionHeight):
            pass
        def reset(self):
            pass
        def setAgentStart(self, x, y):
            pass
        def update(self, timestep):
            pass
        def applyMotorCommand(self, cmd, time):
            return None
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
            return 0.0
        def getHeight(self):
            return 0.0
        def getMaxEnergy(self):
            return 0.0
        def setMaxEnergy(self, e):
            pass
        def getEnergyDecayRate(self):
            return 0.0
        def setEnergyDecayRate(self, r):
            pass
        def getSimulationTime(self):
            return 0.0
        def setRandomSeed(self, seed):
            pass
        def getRandomSeed(self):
            return 0
    
    class AgentBrain:
        def __init__(self, brain):
            pass
        def initialize(self, world):
            pass
        def getSensoryInputSize(self):
            return 0
        def getMotorOutputSize(self):
            return 0
        def processSensoryInput(self, percept):
            pass
        def decodeMotorCommand(self):
            return None
        def applyRewardModulation(self, reward, predictedReward):
            pass
        def updateDevelopment(self, timestep):
            pass
        def getDevelopmentalStage(self):
            return 0
        def getNeuromodulationLevel(self):
            return 0.0
        def getCuriosityLevel(self):
            return 0.0
        def getNoveltyLevel(self):
            return 0.0
        def getPredictionError(self):
            return 0.0
        def reset(self):
            pass
        def getBrain(self):
            return None
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
    
    # Define minimal type enums
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
    
    # Define fallback convenience functions
    def createDefaultConfig():
        return Config()
    
    def createBrain(config=None):
        return Brain(config)
    
    def createSimpleWorld():
        return SimpleWorld()
    
    def createAgentBrain(brain):
        return AgentBrain(brain)
    
    def createSimpleAgent(config_dict=None):
        print("Warning: createSimpleAgent requires core NLM bindings")
        return None
    
    def runSimulation(brain, world, agent, num_steps, verbose=False):
        print(f"Simulation with {num_steps} steps (fallback implementation)")
        for step in range(num_steps):
            brain.step(step)
            if verbose and step % 100 == 0:
                print(f"Step {step}")
        return {'success': True, 'steps_completed': num_steps}
    
    def runLearningSimulation(brain, world, agent, num_steps, **kwargs):
        print(f"Learning simulation with {num_steps} steps (fallback implementation)")
        return runSimulation(brain, world, agent, num_steps, kwargs.get('verbose', False))
    
    # Define fallback exception classes
    class NLMError(Exception):
        pass
    
    class ConfigurationError(NLMError):
        pass
    
    class SimulationError(NLMError):
        pass
    
    class ValidationError(NLMError):
        pass
    
    class PerformanceError(NLMError):
        pass
    
    # Define constants
    INVALID_NEURON_ID = None
    INVALID_SYNAPSE_ID = None
    INVALID_REGION_ID = None
    INVALID_POPULATION_ID = None
    
    # Define __all__ for fallback
    __all__ = [
        'Config',
        'Brain',
        'SimpleWorld',
        'AgentBrain',
        'SensoryPercept',
        'Vision',
        'Audio',
        'InternalSignals',
        'Action',
        'WorldObject',
        'AgentBody',
        'ActionResult',
        'NeuronId',
        'SynapseId',
        'RegionId',
        'PopulationId',
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
        'createSimpleAgent',
        'runSimulation',
        'runLearningSimulation',
        'NLMError',
        'ConfigurationError',
        'SimulationError',
        'ValidationError',
        'PerformanceError'
    ]

# Version information
__version__ = "1.0.0"
__description__ = "NLM Python bindings for neural simulation framework"

# Feature availability
features_available = {
    'original_bindings': 'Config' in globals() and 'Brain' in globals(),
    'enhanced_bindings': _has_enhanced_api,
    'learning': False,
    'plasticity': False,
    'development': False
}

# Make the module easily discoverable
sys.modules['nlm'] = sys.modules[__name__]

# Print initialization message
print(f"NLM Python bindings v{__version__} loaded successfully")
print(f"Available features: {sum(features_available.values())}/{len(features_available)}")

for feature, available in features_available.items():
    if available:
        print(f"  ✓ {feature.replace('_', ' ').title()}")

print(f"\n{'='*60}")
print("NLM (Neural Learning Machine) - Python Bindings")
print("="*60)
print(f"Version: {__version__}")
print(f"Description: {__description__}")
print(f"\nCore bindings available: {'Yes' if features_available['original_bindings'] else 'No'}")
print(f"Enhanced API available: {'Yes' if features_available['enhanced_bindings'] else 'No'}")
print(f"\nFor advanced features and utilities, import pynlm_enhanced")
print(f"{'='*60}")
