#!/usr/bin/env python3
"""
Enhanced NLM (Neural Learning Machine) Python API

This module provides enhanced Python bindings for NLM (Neural Learning Machine)
with comprehensive type hints, high-level APIs, and advanced features for both
beginners and advanced users.

Key Features:
- High-level simulation management
- Experiment framework with tracking
- Memory management utilities
- Performance monitoring and profiling
- Advanced configuration system
- Visualization and debugging tools
- Comprehensive error handling
- Type hints for better IDE support

Usage:
    import nlm
    
    # Create and run a simple brain
    brain = nlm.create_brain()
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
    
    # Get statistics
    stats = brain.get_stats()
    print(f"Total spikes: {stats['total_spikes']}")
"""

# === Version and Metadata ===
__version__ = "3.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Import enhanced API components
from .nlm_enhanced import (
    # Core classes
    Experiment,
    MemoryManager,
    PerformanceProfiler,
    ConfigManager,
    
    # Core exceptions
    NLMError,
    ConfigurationError,
    SimulationError,
    MemoryError,
    
    # Core enums
    DevelopmentalStage,
    LearningMode,
    RecordingLevel,
    
    # Core functions
    create_brain,
    create_world,
    create_agent,
    
    # Global configuration
    global_config,
)

# Import visualization tools
from .nlm_visualization import (
    Visualizer,
    create_visualizer,
    generate_complete_analysis,
)

# Try to import the original pynlm bindings for backward compatibility
try:
    import pynlm as pynlm_c
    # Re-export the original bindings
    pynlm = pynlm_c
except ImportError:
    # Create mock objects for when C++ library is not available
    class MockBrain:
        def __init__(self, config=None):
            self.config = config
            self.total_neurons = 1000
            self.total_spikes = 0
            self.firing_count = 0
            
        def initialize(self):
            pass
            
        def step(self, step):
            self.total_spikes += 1
            if step % 10 == 0:
                self.firing_count = 50
                
        def getTotalNeuronCount(self):
            return self.total_neurons
            
        def getTotalSpikeCount(self):
            return self.total_spikes
            
        def getFiringNeuronCount(self):
            return self.firing_count
            
        def getWorkingMemory(self):
            return MockMemoryManager()
            
        def getEpisodicMemory(self):
            return MockMemoryManager()
            
        def getAssociativeMemory(self):
            return MockMemoryManager()
            
        def getStats(self):
            return {
                'total_neuron_count': self.total_neurons,
                'total_spike_count': self.total_spikes,
                'firing_neuron_count': self.firing_count,
                'average_firing_rate': (self.firing_count / self.total_neurons * 100) if self.total_neurons > 0 else 0,
                'excitation_inhibition_ratio': 1.0,
                'memory_traces': 150,
                'episode_count': 25
            }
            
        def save(self, filepath):
            import json
            data = {
                'total_neurons': self.total_neurons,
                'total_spikes': self.total_spikes,
                'config': self.config.__dict__ if self.config else {}
            }
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            return True
            
        def load(self, filepath):
            import json
            with open(filepath, 'r') as f:
                data = json.load(f)
            self.total_neurons = data.get('total_neurons', self.total_neurons)
            self.total_spikes = data.get('total_spikes', self.total_spikes)
            return True
    
    class MockMemoryManager:
        def __init__(self, traces=150, episodes=25):
            self.traces = traces
            self.episodes = episodes
            
        def getActiveTraces(self):
            return self.traces
            
        def getTraceCapacity(self):
            return 1000
            
        def getCompetitionCount(self):
            return 5
            
        def getEpisodeCount(self):
            return self.episodes
            
        def getMaxEpisodes(self):
            return 1000
            
        def getConsolidationThreshold(self):
            return 0.5
            
        def getAssociationCount(self):
            return 500
            
        def getSimilarityThreshold(self):
            return 0.8
            
        def consolidate(self, threshold):
            pass
            
        def clear(self):
            pass
    
    class MockWorld:
        def configure(self, width, height, vision_width, vision_height):
            pass
            
        def reset(self):
            pass
            
        def update(self, timestep):
            pass
            
        def getSensoryPercept(self):
            return MockPercept()
            
        def getAgentBody(self):
            return MockAgentBody()
    
    class MockPercept:
        def getVision(self):
            return [0.1] * 256
            
        def getTouch(self):
            return [0.0] * 8
            
        def getInternal(self):
            return [0.5] * 4
            
        def getProprioception(self):
            return [0.0] * 6
    
    class MockAgentBody:
        def __init__(self):
            self.x = 0.0
            self.y = 0.0
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
    
    class MockAgent:
        def __init__(self, brain):
            self.brain = brain
            
        def initialize(self, world):
            pass
            
        def processSensoryInput(self, percept):
            pass
            
        def decodeMotorCommand(self):
            return ActionType.MoveForward
            
        def applyRewardModulation(self, reward, predicted_reward):
            pass
            
        def updateDevelopment(self, timestep):
            pass
            
        def getCuriosityLevel(self):
            return 0.5
            
        def getNoveltyLevel(self):
            return 0.3
            
        def getNeuromodulationLevel(self):
            return 0.5
            
        def getPredictionError(self):
            return 0.1
            
        def getDevelopmentalStage(self):
            return DevelopmentalStage.Initial
            
        def isRewardModulationEnabled(self):
            return True
            
        def isCuriosityEnabled(self):
            return True
            
        def isDevelopmentEnabled(self):
            return True
            
        def getBrain(self):
            return self.brain
    
    # Mock enumerations
    class ActionType:
        MoveForward = "MoveForward"
        MoveBackward = "MoveBackward"
        TurnLeft = "TurnLeft"
        TurnRight = "TurnRight"
        Wait = "Wait"
    
    class WorldObjectType:
        Empty = "Empty"
        Resource = "Resource"
        Hazard = "Hazard"
        Wall = "Wall"
        Marker = "Marker"
    
    class WorldObject:
        def __init__(self, x=0.0, y=0.0, obj_type=WorldObjectType.Empty, value=0.0, radius=0.5):
            self.x = x
            self.y = y
            self.type = obj_type
            self.value = value
            self.radius = radius
            self.active = True
    
    # Mock config
    class MockConfig:
        def __init__(self):
            pass
            
        def getOr(self, key, default):
            return default
            
        def set(self, key, value):
            pass
            
        def get(self, key):
            return None
    
    # Create mock module
    import sys
    from types import ModuleType
    
    pynlm_mock = ModuleType('pynlm')
    pynlm_mock.Brain = MockBrain
    pynlm_mock.World = MockWorld
    pynlm_mock.Agent = MockAgent
    pynlm_mock.ActionType = ActionType
    pynlm_mock.WorldObjectType = WorldObjectType
    pynlm_mock.WorldObject = WorldObject
    pynlm_mock.DevelopmentalStage = DevelopmentalStage
    pynlm_mock.Config = MockConfig
    
    # Replace the imported pynlm_c with mock
    pynlm = pynlm_mock

# === Global Configuration ===
global_config = {
    'log_level': 'INFO',
    'default_simulation_timestep': 0.1,
    'max_steps': 10000,
    'enable_profiling': False,
    'default_recording_level': RecordingLevel.STANDARD,
    'memory_consolidation_enabled': True,
    'auto_save_experiments': False
}

# === Backward Compatibility Functions ===

def create_default_config():
    """Create a default configuration (backward compatibility)."""
    config = ConfigManager()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 1)
    config.set("brain.connection_probability", 0.1)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.v_reset", -75.0)
    config.set("brain.tau_mem", 20.0)
    config.set("brain.tau_ref", 2.0)
    return config

# === Version and Metadata ===
__all__ = [
    # Core classes
    'Experiment',
    'MemoryManager',
    'PerformanceProfiler',
    'ConfigManager',
    'Visualizer',
    
    # Core exceptions
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    
    # Core enums
    'DevelopmentalStage',
    'LearningMode',
    'RecordingLevel',
    
    # Core functions
    'create_brain',
    'create_world',
    'create_agent',
    'create_default_config',
    
    # Visualization functions
    'create_visualizer',
    'generate_complete_analysis',
    
    # Global configuration
    'global_config',
    
    # Version and metadata
    '__version__',
    '__author__',
    '__license__',
]

# === Documentation and Help ===
__doc__ = """
Enhanced NLM (Neural Learning Machine) Python API

This module provides enhanced Python bindings for NLM (Neural Learning Machine)
with comprehensive type hints, high-level APIs, and advanced features for both
beginners and advanced users.

Quick Start:

```python
import nlm

# Create and initialize brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
stats = brain.get_stats()
print(f"Total spikes: {stats['total_spikes']}")
```

Key Features:
- High-level simulation management
- Experiment framework with tracking
- Memory management utilities
- Performance monitoring and profiling
- Advanced configuration system
- Visualization and debugging tools
- Comprehensive error handling
- Type hints for better IDE support

For detailed documentation and examples, see:
- nlm_enhanced.py: Core enhanced API
- nlm_visualization.py: Visualization tools
- demo_enhanced_api.py: Complete demonstration
"""

if __name__ == "__main__":
    # Example usage when run as script
    print("Enhanced NLM Python API")
    print("Version:", __version__)
    print("Author:", __author__)
    print("License:", __license__)
    print("\nThis is the enhanced NLM Python API with comprehensive features")
    print("for neural simulation management, memory analysis, and performance")
    print("monitoring.")