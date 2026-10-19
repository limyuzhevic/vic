"""
Factory functions for NLM Python bindings.
"""

from typing import Optional
from .config import Config
from .brain.brain import Brain
from .world.simple_world import SimpleWorld
from .agent.agent_brain import AgentBrain

# Factory functions
def create_default_config() -> Config:
    """
    Create a default configuration for NLM.
    
    Returns:
        Default configuration
    """
    return Config.create_default()

def create_brain(config: Config) -> Brain:
    """
    Create a brain with configuration.
    
    Args:
        config: Brain configuration
        
    Returns:
        New brain instance
    """
    brain = Brain(config)
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    return brain

def create_simple_world() -> SimpleWorld:
    """
    Create a simple world for NLM simulation.
    
    Returns:
        New simple world instance
    """
    return SimpleWorld()

def create_agent_brain(brain: Brain) -> AgentBrain:
    """
    Create an agent brain interface.
    
    Args:
        brain: Underlying brain
        
    Returns:
        New agent brain instance
    """
    return AgentBrain(brain)

__all__ = [
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
]
