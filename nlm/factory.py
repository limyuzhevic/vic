"""
Factory functions for creating common NLM objects
"""

from typing import Optional, Dict, Any
from .core import Brain, Config, AgentBrain, SimpleWorld
from .wrapper import Config as EnhancedConfig
class Factory:
    """Factory class for creating common NLM objects"""
    
    @staticmethod
    def create_default_config() -> EnhancedConfig:
        """Create default configuration"""
        return EnhancedConfig()
    
    @staticmethod
    def create_brain(config: Optional[EnhancedConfig] = None) -> Brain:
        """Create brain with optional configuration"""
        return Brain(config) if config else Brain()
    
    @staticmethod
    def create_world(width: int = 800, height: int = 600,
                    vision_width: int = 80, vision_height: int = 80) -> SimpleWorld:
        """Create simple world with configuration"""
        world = SimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        return world
    
    @staticmethod
    def create_agent_brain(brain: Brain) -> AgentBrain:
        """Create agent brain interface"""
        return AgentBrain(brain)
# Legacy function names for backward compatibility
create_default_config = Factory.create_default_config
create_brain = Factory.create_brain
create_world = Factory.create_world
create_agent_brain = Factory.create_agent_brain
