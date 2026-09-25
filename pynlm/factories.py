# Factory functions module
# ========================

from .core import Config, Brain, SimpleWorld, AgentBrain

__all__ = [
    'createConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
]

def createConfig():
    """Create a new NLM configuration.
    
    Returns:
        Config: A new configuration object
    """
    return Config()

def createBrain(config=None):
    """Create a new NLM brain.
    
    Args:
        config (Config, optional): Configuration for the brain. If None, a default config is used.
        
    Returns:
        Brain: A new brain instance
    """
    if config is None:
        config = createConfig()
    return Brain(config)

def createSimpleWorld():
    """Create a new simple world for NLM simulation.
    
    Returns:
        SimpleWorld: A new world instance
    """
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface.
    
    Args:
        brain (Brain): The underlying NLM brain
        
    Returns:
        AgentBrain: An agent brain interface
    """
    return AgentBrain(brain)