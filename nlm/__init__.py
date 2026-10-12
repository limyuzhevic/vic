import sys
from typing import Optional, Dict, Any
from .brain import Brain
from .world import World
from .agent import Agent
from .config import Config

__version__ = "1.0.0"


def create_brain(config: Optional[Config] = None, **kwargs) -> Brain:
    """Create a new Brain instance.
    
    Args:
        config: Configuration object (optional)
        **kwargs: Additional configuration parameters
        
    Returns:
        Brain instance
    """
    return Brain(config, **kwargs)


def run_simulation(brain: Brain, steps: int = 100, **kwargs) -> None:
    """Run a simulation with the given brain.
    
    Args:
        brain: Brain instance to run
        steps: Number of simulation steps
        **kwargs: Additional simulation parameters
    """
    brain.run_simulation(steps, **kwargs)


def create_agent(name: str, brain: Optional[Brain] = None, **kwargs) -> Agent:
    """Create a new Agent instance.
    
    Args:
        name: Agent name
        brain: Brain instance (optional)
        **kwargs: Additional configuration parameters
        
    Returns:
        Agent instance
    """
    return Agent(name, brain, **kwargs)


def get_version() -> str:
    """Get the NLM package version.
    
    Returns:
        Version string
    """
    return __version__

# For backward compatibility
from pynlm import Brain as OldBrain, World as OldWorld, Agent as OldAgent

# Re-export for backward compatibility
Brain = OldBrain or Brain
World = OldWorld or World
Agent = OldAgent or Agent