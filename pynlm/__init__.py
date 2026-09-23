#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine

A Python interface to the NLM C++ neural simulation framework.

This module provides access to brain, world, agent, and sensory/motor interfaces
for creating and simulating artificial brains.

Example usage:
    import pynlm
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Neurons: {brain.getTotalNeuronCount()}")
"""

import sys
from typing import List, Optional, Any, Union, Dict

# Try to import the core bindings
try:
    from pynlm._pynlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Main classes
        Config, Brain, AgentBrain, SimpleWorld,
        Vision, Audio, InternalSignals, Action, WorldObject, AgentBody,
        ActionResult, SensoryPercept,
        
        # Factory functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID,
    )
    
    # Python-friendly wrappers and convenience classes
    
    class DefaultConfig:
        """Easy-to-use configuration with sensible defaults."""
        
        def __init__(self):
            self._config = createDefaultConfig()
        
        def set(self, key: str, value: Any, source: str = "runtime") -> None:
            """Set a configuration value."""
            self._config.set(key, value, source)
        
        def has(self, key: str) -> bool:
            """Check if key exists."""
            return self._config.has(key)
        
        def load_from_file(self, filepath: str) -> bool:
            """Load configuration from JSON file."""
            return self._config.loadFromFile(filepath)
        
        def save_to_file(self, filepath: str) -> bool:
            """Save configuration to JSON file."""
            return self._config.saveToFile(filepath)
        
        def get_keys(self) -> List[str]:
            """Get all configuration keys."""
            return self._config.getKeys()
        
        def clear(self) -> None:
            """Clear all configuration entries."""
            self._config.clear()
        
        def summary(self) -> str:
            """Get configuration summary."""
            return self._config.summary()
        
        def __repr__(self) -> str:
            return f"<DefaultConfig: {self.summary()}>"
    
    class Simulation:
        """Helper class for running complete agent simulations."""
        
        def __init__(self, brain: Any, world: Any, agent: Any):
            self.brain = brain
            self.world = world
            self.agent = agent
        
        def run_episode(self, num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
            """Run a complete simulation episode."""
            stats = {
                'steps': 0,
                'spikes': 0,
                'firing_neurons': 0,
                'curiosity': 0.0,
                'novelty': 0.0,
                'prediction_error': 0.0,
                'development_stage': 0,
            }
            
            initial_spikes = self.brain.getTotalSpikeCount()
            
            for step in range(num_steps):
                # Update world
                self.world.update(timestep)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Brain thinks
                self.brain.step(step)
                
                # Get motor command
                action = self.agent.decodeMotorCommand()
                
                # Apply action to world
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward if available
                if percept.getInternal():
                    reward = percept.getInternal()[0]
                    self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(timestep)
                
                # Collect statistics
                stats['spikes'] = self.brain.getTotalSpikeCount() - initial_spikes
                stats['firing_neurons'] = self.brain.getFiringNeuronCount()
                stats['curiosity'] = self.agent.getCuriosityLevel()
                stats['novelty'] = self.agent.getNoveltyLevel()
                stats['prediction_error'] = self.agent.getPredictionError()
                stats['development_stage'] = self.brain.getDevelopmentalStage()
            
            stats['steps'] = num_steps
            return stats
        
        def run_multiple_episodes(self, num_episodes: int, steps_per_episode: int, 
                                timestep: float = 0.1) -> List[Dict[str, Any]]:
            """Run multiple episodes and return results."""
            results = []
            
            for episode in range(num_episodes):
                self.world.reset()
                stats = self.run_episode(steps_per_episode, timestep)
                stats['episode'] = episode
                results.append(stats)
            
            return results
        
        def save_checkpoint(self, filepath: str) -> bool:
            """Save simulation checkpoint."""
            return self.brain.save(filepath)
        
        def load_checkpoint(self, filepath: str) -> bool:
            """Load simulation checkpoint."""
            return self.brain.load(filepath)
    
    # Version information
    __version__ = "0.1.0"
    
    # Make important symbols available at module level
    __all__ = [
        # Types
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        
        # Classes
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'Vision', 'Audio', 'InternalSignals', 'Action', 'WorldObject',
        'AgentBody', 'ActionResult', 'SensoryPercept',
        'DefaultConfig', 'Simulation',
        
        # Factory functions
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        
        # Constants
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
        
        # Version
        '__version__',
    ]

except ImportError as e:
    print(f"Warning: Could not import pynlm bindings: {e}")
    print("Please build the Python extensions first by running: pip install -e .")
    
    # Create stub classes for development
    class _StubClass:
        def __init__(self, *args, **kwargs):
            pass
        
        def __getattr__(self, name):
            return lambda *args, **kwargs: None
    
    # Define stub classes
    Config = _StubClass
    Brain = _StubClass
    AgentBrain = _StubClass
    SimpleWorld = _StubClass
    Vision = _StubClass
    Audio = _StubClass
    InternalSignals = _StubClass
    Action = _StubClass
    WorldObject = _StubClass
    AgentBody = _StubClass
    ActionResult = _StubClass
    SensoryPercept = _StubClass
    
    # Stub factory functions
    def createDefaultConfig(): return _StubClass()
    def createBrain(config): return _StubClass()
    def createSimpleWorld(): return _StubClass()
    def createAgentBrain(brain): return _StubClass()
    
    # Stub constants
    INVALID_NEURON_ID = _StubClass()
    INVALID_SYNAPSE_ID = _StubClass()
    INVALID_REGION_ID = _StubClass()
    INVALID_POPULATION_ID = _StubClass()
    
    # Enum stubs
    class NeuronType(_StubClass):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Sensory = 3
        Motor = 4
        Internal = 5
    
    class SynapseType(_StubClass):
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Electrical = 3
        GapJunction = 4
    
    class DevelopmentalStage(_StubClass):
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class FiringState(_StubClass):
        Resting = 0
        Active = 1
        Refractory = 2
        Inhibited = 3
    
    class ActionType(_StubClass):
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
    
    class MotorCommand(_StubClass):
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        LookLeft = 4
        LookRight = 5
        Interact = 6
        Wait = 7
    
    class WorldObjectType(_StubClass):
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    DefaultConfig = _StubClass
    Simulation = _StubClass
    
    __version__ = "0.1.0"
    
    __all__ = [
        # Types
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        
        # Classes
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'Vision', 'Audio', 'InternalSignals', 'Action', 'WorldObject',
        'AgentBody', 'ActionResult', 'SensoryPercept',
        'DefaultConfig', 'Simulation',
        
        # Factory functions
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        
        # Constants
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
        
        # Version
        '__version__',
    ]

print(f"NLM Python bindings version {__version__} loaded.")