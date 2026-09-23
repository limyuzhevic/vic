#!/usr/bin/env python3
"""
NLM Agent Module

Provides agent behavior capabilities for NLM (Neural Learning Machine).
"""

from typing import Optional
from .brain import Brain

class AgentBrain:
    """
    High-level interface to an NLM agent that interacts with the environment.
    
    This class provides a simplified interface for creating and controlling
    agents that can learn through experience.
    """
    
    def __init__(self, brain: Brain):
        """Create a new agent with the given brain."""
        self._brain = brain
        self._enabled = False
        self._reward_modulation = False
        self._structural_plasticity = False
    
    def enableRewardModulation(self, enabled: bool):
        """Enable or disable reward-based modulation."""
        self._reward_modulation = enabled
    
    def enableStructuralPlasticity(self, enabled: bool):
        """Enable or disable structural plasticity."""
        self._structural_plasticity = enabled
    
    def isEnabled(self) -> bool:
        """Check if the agent is enabled."""
        return self._enabled
    
    def getBrain(self) -> Brain:
        """Get the agent's brain."""
        return self._brain
    
    def step(self, sensory_input, action):
        """Process sensory input and generate an action."""
        pass
    
    def __str__(self):
        return f"AgentBrain(brain={self._brain})"

def create_agent(brain: Optional[Brain] = None) -> AgentBrain:
    """Create a new Agent instance with optional brain."""
    if brain is None:
        from .brain import create_brain
        brain = create_brain()
    return AgentBrain(brain)

def demo_agent_behavior():
    """Run a demo of agent behavior."""
    print("=== NLM Agent Behavior Demo ===")
    
    brain = create_brain()
    agent = create_agent(brain)
    
    print(f"Agent created: {agent}")
    print(f"Agent enabled: {agent.isEnabled()}")
    
    # Enable learning mechanisms
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    print("Agent learning mechanisms enabled:")
    print(f"  - Reward modulation: {agent._reward_modulation}")
    print(f"  - Structural plasticity: {agent._structural_plasticity}")
    
    # Simulate some agent activity
    brain.initialize()
    for step in range(10):
        brain.step(step)
    
    print(f"Agent brain state after 10 steps:")
    print(f"  - Neurons: {brain.get_total_neuron_count()}")
    print(f"  - Developmental stage: {brain.get_developmental_stage()}")
    
    print("Agent behavior demo completed!")
    return brain, agent
