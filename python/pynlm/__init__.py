"""
pynlm - Python API for NLM (Neural Learning Machine)

A comprehensive Python interface for the NLM neural simulation framework.
Provides high-level abstractions, type safety, and Pythonic APIs for working
with neural brains, agents, worlds, and simulations.
"""

__version__ = "0.1.0"

# Core exceptions
from .exceptions import (
    NLMError,
    ConfigurationError,
    SimulationError,
    MemoryError,
    ValidationError,
)

# Core classes
from .core import Brain, Config

# Agent interface
from .agent import AgentBrain

# World simulation
from .world import SimpleWorld

# Sensory inputs
from .sensory import Vision, Audio

# Performance monitoring
from .profiler import Profiler, ProfileSimulation

# Simulation lifecycle
from .simulation import Simulation

# Configuration management
from .config import Config

# Factory functions and convenience methods
def createBrain(config=None):
    """Create a new Brain instance.
    
    Args:
        config: Optional configuration. If None, uses default config.
        
    Returns:
        Brain instance
        
    Example:
        >>> brain = pynlm.createBrain()
        >>> brain.initialize()
    """
    if config is None:
        config = createDefaultConfig()
    return Brain(config)


def createSimpleWorld():
    """Create a new SimpleWorld instance.
    
    Returns:
        SimpleWorld instance
        
    Example:
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=10, height=10)
    """
    return SimpleWorld()


def createAgentBrain(brain):
    """Create a new AgentBrain instance.
    
    Args:
        brain: Brain instance to wrap
        
    Returns:
        AgentBrain instance
        
    Example:
        >>> brain = pynlm.createBrain()
        >>> agent = pynlm.createAgentBrain(brain)
    """
    return AgentBrain(brain)


def createDefaultConfig():
    """Create a default configuration.
    
    Returns:
        Config instance with sensible defaults
        
    Example:
        >>> config = pynlm.createDefaultConfig()
    """
    return Config()


# Learning feature convenience methods
def enableLearning(agent, features=None):
    """Enable learning features on an agent in a Pythonic way.
    
    Args:
        agent: AgentBrain instance
        features: List of features to enable. If None, enables all.
        
    Example:
        >>> pynlm.enableLearning(agent, ["reward", "structural", "development"])
    """
    if features is None:
        features = ["reward", "structural", "development", "curiosity"]
    
    feature_map = {
        "reward": agent.enableRewardModulation,
        "structural": agent.enableStructuralPlasticity,
        "development": agent.enableDevelopment,
        "curiosity": agent.enableCuriosity,
    }
    
    for feature in features:
        if feature in feature_map:
            feature_map[feature](True)


# Batch operations helper
def run_batch(brain, steps=1000, save_interval=None):
    """Run a batch of brain steps efficiently.
    
    Args:
        brain: Brain instance
        steps: Number of steps to simulate
        save_interval: Save brain state every this many steps (None = never)
        
    Returns:
        Batch execution statistics
        
    Example:
        >>> stats = pynlm.run_batch(brain, steps=1000, save_interval=100)
    """
    import time
    from .profiler import ProfileSimulation
    
    start_time = time.time()
    profiler = ProfileSimulation(brain)
    
    for i in range(steps):
        brain.step(i)
        if save_interval and (i + 1) % save_interval == 0:
            brain.save(f"brain_step_{i+1}.pkl")
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    return {
        "steps": steps,
        "elapsed_time": elapsed,
        "steps_per_second": steps / elapsed if elapsed > 0 else 0,
        "profiler": profiler,
    }


def run_episode(agent, world, steps=500):
    """Run a complete agent-world episode.
    
    Args:
        agent: AgentBrain instance
        world: SimpleWorld instance
        steps: Number of steps in episode
        
    Returns:
        Episode statistics
        
    Example:
        >>> stats = pynlm.run_episode(agent, world, steps=500)
    """
    from .profiler import ProfileSimulation
    
    profiler = ProfileSimulation(agent.getBrain(), world)
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        "steps": steps,
        "final_firing_rate": agent.getBrain().getAverageFiringRate(),
        "profiler": profiler,
    }


# Pythonic API helper for multi-feature learning
def configure_agent(agent, **kwargs):
    """Configure agent learning features using Pythonic kwargs.
    
    Args:
        agent: AgentBrain instance
        **kwargs: Feature name (reward, structural, development, curiosity) -> bool
        
    Example:
        >>> pynlm.configureAgent(agent, reward=True, structural=True, development=False)
    """
    feature_map = {
        "reward": agent.enableRewardModulation,
        "structural": agent.enableStructuralPlasticity,
        "development": agent.enableDevelopment,
        "curiosity": agent.enableCuriosity,
    }
    
    for feature, enabled in kwargs.items():
        if feature in feature_map:
            feature_map[feature](enabled)
