"""
Enhanced NLM Python Bindings
===========================

Complete Python interface for the Neural Learning Machine (NLM) framework.

This module provides enhanced Python bindings that expose ALL the C++ core APIs,
including memory systems, prediction systems, neuromodulation systems, cognitive
systems, development systems, and comprehensive performance monitoring.

Key Features:
-------------
- **Memory Systems**: WorkingMemory, EpisodicMemory, AssociativeMemory
- **Prediction System**: Real-time prediction and error computation
- **Neuromodulation**: Dopamine, Curiosity, Novelty, PredictionError
- **Cognitive Systems**: NeuralPlanner, ConceptFormation, AttentionalSelection, SelfModel
- **Development**: Full developmental stage and plasticity control
- **Performance Monitoring**: System health, memory usage, cognitive load metrics
- **Utilities**: Simulation control, memory management, debugging tools

Usage Example:
-------------
```python
import pynlm as nlm

# Create brain with configuration
config = nlm.Config()
brain = nlm.createBrain(config)

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

# Record and retrieve experiences
pattern = [0.1, 0.5, 0.9, 0.2]
working_memory.store_pattern(pattern, strength=1.0)

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()

# Plan actions
planner = brain.getPlanner()
action = planner.planAction(pattern)

# Run simulation
world = nlm.createSimpleWorld()
results = nlm.run_simulation(brain, world, 100)

# Monitor system health
perf_stats = brain.get_system_performance()
```

API Reference:
--------------
"""

from .bindings import (
    # Basic types
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Core objects
    Config, SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, AgentBody, ActionResult, SensoryPercept,
    SimpleWorld,
    
    # Brain systems
    Brain, AgentBrain,
    
    # Memory systems
    WorkingMemory, EpisodicMemory, AssociativeMemory,
    
    # Prediction system
    PredictionSystem,
    
    # Neuromodulation systems
    Dopamine, Curiosity, Novelty, PredictionError,
    
    # Cognitive systems
    NeuralPlanner, ConceptFormation, AttentionalSelection, SelfModel,
    
    # Development system
    DevelopmentSystem,
    
    # Global constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    INVALID_REGION_ID, INVALID_POPULATION_ID,
)

# Factory functions
from .bindings import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    run_simulation, train_agent,
)

# Utility functions
from .bindings import (
    backup_brain_state, restore_brain_state, clear_all_memory,
    debug_brain_state, trace_neural_activity, get_system_performance,
    version,
)

# High-level convenience classes
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Core objects
    'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld',
    
    # Brain systems
    'Brain', 'AgentBrain',
    
    # Memory systems
    'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    
    # Prediction system
    'PredictionSystem',
    
    # Neuromodulation systems
    'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    
    # Cognitive systems
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection', 'SelfModel',
    
    # Development system
    'DevelopmentSystem',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'run_simulation', 'train_agent',
    
    # Utility functions
    'backup_brain_state', 'restore_brain_state', 'clear_all_memory',
    'debug_brain_state', 'trace_neural_activity', 'get_system_performance',
    'version',
    
    # High-level classes
    'LearningAgent', 'MemoryProfiler', 'PredictionAnalyzer',
    'NeuromodulationController', 'DevelopmentManager', 'CognitiveProfiler',
]
