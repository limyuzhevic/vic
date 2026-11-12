import sys
from pathlib import Path

# Add utility modules to Python path
sys.path.append(str(Path(__file__).parent / "pynlm"))

# Try to import core modules
try:
    from utils import BrainAnalyzer, ConfigManager, WorldUtils, ExperimentRunner
    UTILS_AVAILABLE = True
except ImportError:
    UTILS_AVAILABLE = False

# Try to import visualization modules
try:
    from viz import Visualization
    VIZ_AVAILABLE = True
except ImportError:
    VIZ_AVAILABLE = False

# Try to import configuration modules
try:
    from config import BrainConfigurator, ParameterSweep, ExperimentManager
    CONFIG_AVAILABLE = True
except ImportError:
    CONFIG_AVAILABLE = False

# Core NLM bindings
try:
    from ctypes import cdll
    pynlm = cdll.LoadLibrary("libpynlm.so")
except OSError:
    print("Warning: libpynlm.so not found")

# Provide basic Python bindings for common functionality
import json
import numpy as np
from pathlib import Path
from typing import Dict, Any, Optional

def create_brain():
    """Create a basic NLM brain."""
    # This would normally load from the C++ extension
    # For demonstration, we'll create a simple mock
    class SimpleBrain:
        def __init__(self):
            self.neuron_count = 1000
            self.synapse_count = 5000
            self.total_spikes = 0
            self.firing_neurons = 0
        
        def initialize(self):
            pass
        
        def step(self, step_num):
            self.total_spikes += np.random.randint(10, 20)
            self.firing_neurons += np.random.randint(5, 10)
        
        def getTotalNeuronCount(self):
            return self.neuron_count
        
        def getTotalSynapseCount(self):
            return self.synapse_count
        
        def getTotalSpikeCount(self):
            return self.total_spikes
        
        def getFiringNeuronCount(self):
            return self.firing_neurons
    
    return SimpleBrain()

def create_default_config():
    """Create default configuration."""
    class SimpleConfig:
        def __init__(self):
            self.settings = {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1,
                'random_seed': 42,
                'simulation_timestep': 0.001
            }
        
        def set(self, key, value):
            self.settings[key] = value
        
        def get(self, key):
            return self.settings.get(key)
        
        def has(self, key):
            return key in self.settings
        
        def getKeys(self):
            return list(self.settings.keys())
    
    return SimpleConfig()

def create_brain_from_config(config):
    """Create brain from configuration."""
    return create_brain()

def create_simple_world():
    """Create simple world."""
    class SimpleWorld:
        def __init__(self):
            self.width = 20.0
            self.height = 20.0
            self.vision_width = 16
            self.vision_height = 16
            self.simulation_time = 0.0
            self.agent_x = 10.0
            self.agent_y = 10.0
        
        def configure(self, width, height, vision_width, vision_height):
            self.width = width
            self.height = height
            self.vision_width = vision_width
            self.vision_height = vision_height
        
        def reset(self):
            self.simulation_time = 0.0
            self.agent_x = self.width / 2
            self.agent_y = self.height / 2
        
        def update(self, timestep):
            self.simulation_time += timestep
        
        def getSensoryPercept(self):
            # Return mock sensory percept
            class SimplePercept:
                def __init__(self):
                    self.vision = [0.1] * (self.vision_width * self.vision_height)
                    self.internal = [0.5, 0.5, 0.0, 0.0]
                
                def getVision(self):
                    return self.vision
                
                def getInternal(self):
                    return self.internal
            
            return SimplePercept()
        
        def getAgentBody(self):
            class SimpleBody:
                def __init__(self):
                    self.x = self.agent_x
                    self.y = self.agent_y
                    self.orientation = 0.0
                    self.velocityX = 0.0
                    self.velocityY = 0.0
                    self.angularVelocity = 0.0
                    self.energy = 100.0
                    self.health = 100.0
                    self.age = 0.0
                    self.isMoving = False
                    self.isTurning = False
                    self.lastActionTime = 0.0
            
            return SimpleBody()
        
        def applyMotorCommand(self, command, current_time):
            # Apply motor command
            pass
        
        def getWidth(self):
            return self.width
        
        def getHeight(self):
            return self.height
        
        def getVisionWidth(self):
            return self.vision_width
        
        def getVisionHeight(self):
            return self.vision_height
        
        def getSimulationTime(self):
            return self.simulation_time
    
    return SimpleWorld()

def create_agent_brain(brain):
    """Create agent brain interface."""
    class SimpleAgent:
        def __init__(self, brain):
            self.brain = brain
        
        def initialize(self, world):
            pass
        
        def processSensoryInput(self, percept):
            pass
        
        def decodeMotorCommand(self):
            # Return a mock motor command
            class SimpleCommand:
                def __init__(self):
                    self.type = 0  # Wait
            
            return SimpleCommand()
        
        def applyRewardModulation(self, reward, predicted):
            pass
        
        def updateDevelopment(self, timestep):
            pass
        
        def enableRewardModulation(self, enable):
            pass
        
        def enableCuriosity(self, enable):
            pass
        
        def enableDevelopment(self, enable):
            pass
        
        def getCuriosityLevel(self):
            return 0.5
        
        def getNoveltyLevel(self):
            return 0.3
        
        def getPredictionError(self):
            return 0.1
    
    return SimpleAgent(brain)

# Export public API
__all__ = [
    'create_brain',
    'create_default_config',
    'create_brain_from_config',
    'create_simple_world',
    'create_agent_brain',
]

# Version information
__version__ = '2.0.0'
__author__ = 'NLM Project'
__description__ = 'Neural Learning Machine - Advanced Brain Simulation Framework'
