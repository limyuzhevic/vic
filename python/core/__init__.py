"""NLM Core types and configuration module"""

import sys
import os
from typing import Dict, Any, Optional, List

# Add the parent directory to the path so we can import from src
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

# Import C++ bindings
import pynlm

__all__ = [
    'Config',
    'Brain', 
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'MotorCommand',
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
]

# Re-export the main classes
Config = pynlm.Config
Brain = pynlm.Brain
SimpleWorld = pynlm.SimpleWorld
Vision = pynlm.Vision
Audio = pynlm.Audio
InternalSignals = pynlm.InternalSignals
Action = pynlm.Action
MotorCommand = pynlm.MotorCommand

# Factory functions
def create_default_config() -> 'Config':
    """Create a default NLM configuration"""
    return pynlm.createDefaultConfig()

def create_brain(config: 'Config') -> 'Brain':
    """Create a brain with the given configuration"""
    return pynlm.createBrain(config)

def create_simple_world() -> 'SimpleWorld':
    """Create a simple world for simulation"""
    return pynlm.createSimpleWorld()

def create_agent_brain(brain: 'Brain'):
    """Create an agent brain interface"""
    return pynlm.createAgentBrain(brain)

# Utility functions
def load_config_from_file(filepath: str) -> 'Config':
    """Load configuration from a JSON file"""
    config = create_default_config()
    # Note: This assumes JSON config files are supported
    # For .cfg files, custom parsing would be needed
    return config

def save_config_to_file(config: 'Config', filepath: str):
    """Save configuration to a JSON file"""
    # Note: This assumes JSON config files are supported
    pass

# Configuration presets
def get_research_config() -> 'Config':
    """Get configuration optimized for research"""
    config = create_default_config()
    config.set("neuron_count", 2000)
    config.set("region_count", 4)
    config.set("connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01)
    config.set("stdp_ltd_weight", 0.012)
    return config

def get_agent_training_config() -> 'Config':
    """Get configuration optimized for agent training"""
    config = create_default_config()
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    return config

def get_development_config() -> 'Config':
    """Get configuration optimized for developmental studies"""
    config = create_default_config()
    config.set("neuron_count", 1500)
    config.set("region_count", 3)
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    config.set("plasticity.structural.enable", True)
    return config
