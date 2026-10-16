"""Enhanced agent interfaces for NLM.

This module provides enhanced agent interfaces for the NLM (Neural Learning Machine)
framework with Pythonic interfaces, episode context managers, and enhanced
neuromodulation support. It builds upon the original AgentBrain class to
provide more intuitive and robust agent interfaces.

Key features:
- EnhancedAgent with Pythonic interfaces
- Episode context managers for structured episodes
- Enhanced neuromodulation statistics
- Automatic reward tracking
- Pythonic iteration and property interfaces
- Backward compatibility with original AgentBrain API

Usage examples:

    from pynlm.enhanced.agent import EnhancedAgent

    # Create enhanced agent
    agent = EnhancedAgent(brain, name="MyAgent")
    agent.initialize(world)
    
    # Use episode context manager
    with agent.episode_context(max_steps=1000) as episode:
        world.update(0.1)
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
    
    # Get enhanced statistics
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")

The agent interfaces include:

1. EnhancedAgent class - Extended agent with enhanced features
2. Episode context managers for structured episodes
3. Automatic reward and action tracking
4. Enhanced neuromodulation statistics
5. Pythonic property interfaces
6. Backward compatibility with original AgentBrain
"""

from typing import Dict, Any, Optional, List, Iterator
from contextlib import contextmanager
from .. import AgentBrain as OriginalAgentBrain

# Import from enhanced_api
from ..enhanced_api import (
    EnhancedAgent as EnhancedAgentClass,
    ValidationError,
    NeuralNetworkError,
    ErrorHandler
)

# Re-export class
EnhancedAgent = EnhancedAgentClass

class EpisodeStats:
    """Statistics for an agent episode.
    
    The EpisodeStats class collects and summarizes statistics for an agent
    episode, including actions taken, rewards received, and performance metrics.
    
    Attributes:
        steps_completed: Number of steps completed
        actions_taken: Number of actions taken
        total_reward: Total reward received
        episode_success: Whether episode was successful
        average_reward: Average reward per step
        action_distribution: Distribution of actions taken
    """
    
    def __init__(self):
        self.steps_completed = 0
        self.actions_taken = 0
        self.total_reward = 0.0
        self.episode_success = False
        self.average_reward = 0.0
        self.action_distribution: Dict[str, int] = {}
    
    def record_action(self, action: Any, reward: float) -> None:
        """Record an action and reward.
        
        Args:
            action: Action taken
            reward: Reward received
        """
        self.actions_taken += 1
        self.total_reward += reward
        
        # Track action distribution
        action_key = str(action)
        self.action_distribution[action_key] = self.action_distribution.get(action_key, 0) + 1
    
    def finalize(self) -> None:
        """Finalize episode statistics."""
        if self.steps_completed > 0:
            self.average_reward = self.total_reward / self.steps_completed
        else:
            self.average_reward = 0.0
    
    def __repr__(self) -> str:
        """String representation of episode stats.
        
        Args:
            
        Returns:
            String representation
        """
        return (f"EpisodeStats(steps={self.steps_completed}, "
                f"actions={self.actions_taken}, "
                f"reward={self.total_reward:.2f}, "
                f"avg_reward={self.average_reward:.3f})")

# Factory function for creating enhanced agents
def create_enhanced_agent(brain: OriginalAgentBrain, name: Optional[str] = None) -> EnhancedAgent:
    """Create an enhanced agent.
    
    Args:
        brain: Brain to control
        name: Optional agent name
        
    Returns:
        Enhanced agent instance
    """
    return EnhancedAgent(brain, name)

# Episode context manager
def episode_context(agent: EnhancedAgent, max_steps: int = 1000):
    """Context manager for agent episodes.
    
    Args:
        agent: Agent to run episode for
        max_steps: Maximum episode steps
        
    Returns:
        Episode context manager
    """
    return agent.episode_context(max_steps)

# Episode runner function
def run_episode(agent: EnhancedAgent, world, max_steps: int = 1000) -> Dict[str, Any]:
    """Run a complete agent episode.
    
    Args:
        agent: Agent to run
        world: World to interact with
        max_steps: Maximum episode steps
        
    Returns:
        Episode statistics
    """
    return agent.run_episode(world, max_steps)

# Export all public classes and functions
__all__ = [
    'EnhancedAgent',
    'EpisodeStats',
    'create_enhanced_agent',
    'episode_context',
    'run_episode',
]