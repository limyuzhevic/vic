pynlm/__init__.py

"""
NLM (Neural Learning Machine) Python Bindings

A Python interface to the NLM C++ neural simulation framework.
Provides access to neural computation, learning, and adaptive behavior
capabilities through a high-level, intuitive API.

Key Features:
- Brain simulation with spiking neurons
- Working memory and episodic memory systems
- Neuromodulation (dopamine, curiosity, novelty)
- Sensory processing and motor control
- Reinforcement learning
- Checkpoint persistence
- Real-time event handling

Example Usage:
    >>> from pynlm import create_brain, create_simple_world
    >>> 
    >>> # Create a neural brain
    >>> config = create_default_config()
    >>> brain = create_brain(config)
    >>> brain.initialize()
    >>>
    >>> # Create environment
    >>> world = create_simple_world(64, 64, 32, 32)
    >>> 
    >>> # Run simulation loop
    >>> for step in range(1000):
    ...     percept = world.get_sensory_percept()
    ...     brain.receive_sensory_input(percept)
    ...     brain.step(step, step * 0.001)
    ...     action = brain.produce_action()
    ...     world.apply_motor_command(action)
    ...
    >>>
    >>> print(f"Final reward: {world.get_total_reward()}")

For advanced usage, see examples/ directory.
"""

# Core classes
from pynlm.core import Config, Brain, AgentBrain, SimpleWorld
from pynlm.sensory import Vision, Audio, InternalSignals, SensoryInput
from pynlm.motor import Action
from pynlm.environment import WorldObject

# Types and enums
from pynlm.types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand, WorldObjectType
)

# Factory functions
from pynlm.factory import (
    create_default_config,
    create_brain,
    create_simple_world,
    create_agent_brain
)

# Constants
from pynlm.constants import (
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID
)

# Version
__version__ = "0.1.0"
__author__ = "NLM Project"
__email__ = "nlm@example.com"
__license__ = "MIT"

# Python version check
import sys
if sys.version_info < (3, 8):
    raise RuntimeError("pynlm requires Python 3.8 or higher")

# Optional dependencies
OPTIONAL_DEPENDENCIES = {
    "numpy": "numpy",
    "matplotlib": "matplotlib",
    "scipy": "scipy"
}

# Event system
import threading
from typing import Callable, Dict, Any

class EventSystem:
    """Event-driven simulation system for callbacks and notifications."""
    
    def __init__(self):
        self._callbacks: Dict[str, list] = {}
        self._lock = threading.RLock()
    
    def on(self, event_type: str, callback: Callable):
        """Register callback for event type."""
        with self._lock:
            if event_type not in self._callbacks:
                self._callbacks[event_type] = []
            self._callbacks[event_type].append(callback)
    
    def emit(self, event_type: str, data: Any = None):
        """Emit event to all registered callbacks."""
        with self._lock:
            callbacks = self._callbacks.get(event_type, []).copy()
        
        for callback in callbacks:
            try:
                callback(data)
            except Exception as e:
                print(f"Error in event callback: {e}")
    
    def remove(self, event_type: str, callback: Callable):
        """Remove callback from event type."""
        with self._lock:
            if event_type in self._callbacks:
                try:
                    self._callbacks[event_type].remove(callback)
                except ValueError:
                    pass

# Global event system instance
_events = EventSystem()

# Utility functions
def get_events() -> EventSystem:
    """Get the global event system instance."""
    return _events

__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'WorldObject',
    
    # Types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Factory functions
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Version info
    '__version__',
    
    # Event system
    'EventSystem',
    'get_events'
]
