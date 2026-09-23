#!/usr/bin/env python3
"""
Main NLM Python package entry point.

This module provides the high-level Python API for the NLM (熙然) 
neural learning machine framework, integrating all the core components
into a single, user-friendly interface.

The NLM Python package includes:
- Configuration management
- Brain simulation
- Agent-environment interaction
- World simulation
- Sensory processing
- Motor control

All classes and functions are organized into submodules under the
`nlm.python` package.
"""

__version__ = "0.1.0"
__author__ = "NLM Project Authors"
__description__ = "Neural Learning Machine - Python bindings for Phase 6 integration"

# Core classes and functions are imported from their respective modules
from .config import Config, create_config_from_dict, create_config_from_json
from .brain import Brain, create_brain, demo_brain_simulation
from .agent import AgentBrain, create_agent, demo_agent_behavior
from .world import SimpleWorld, WorldObject, create_world, demo_world_environment
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, MotorCommand, create_action

# Make all classes and functions available at package level
__all__ = [
    # Classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld', 'WorldObject',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput', 'Action', 'MotorCommand',
    
    # Factory functions
    'create_config_from_dict', 'create_config_from_json',
    'create_brain', 'demo_brain_simulation',
    'create_agent', 'demo_agent_behavior',
    'create_world', 'demo_world_environment',
    'create_action',
]

def get_version():
    """Get the version of the NLM Python package."""
    return __version__

def get_info():
    """Get information about the NLM Python package."""
    return {
        'version': __version__,
        'author': __author__,
        'description': __description__,
        'status': 'Phase 6 - Complete Integration',
        'components': [
            'Configuration Management',
            'Brain Simulation (Phase 6)',
            'Agent-Environment Interface',
            'World Simulation',
            'Sensory Processing',
            'Motor Control',
        ],
        'features': [
            'Real LIF neuron dynamics',
            'Event-driven spike propagation',
            'STDP and Hebbian plasticity',
            'Structural plasticity',
            'Memory systems (working, episodic, associative)',
            'Neuromodulation (dopamine, curiosity, novelty)',
            'Prediction systems',
            'Cognition (planning, concept formation, attention)',
            'Development stages',
            'Checkpoint save/load',
            'Memory replay and consolidation',
        ]
    }
class _InternalImportHelper:
    """Internal class to help with imports."""
    pass

# Type aliases for convenience
import typing
if typing.TYPE_CHECKING:
    ConfigType = Config
    BrainType = Brain
    AgentBrainType = AgentBrain
    SimpleWorldType = SimpleWorld
    WorldObjectType = WorldObject
    VisionType = Vision
    AudioType = Audio
    InternalSignalsType = InternalSignals
    SensoryInputType = SensoryInput
    ActionType = Action
    MotorCommandType = MotorCommand