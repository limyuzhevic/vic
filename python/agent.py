"""
Agent NLM functionality.

This module contains agent-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import Brain, createAgentBrain as create_agent_brain_native
from .core import ConfigurationError, InitializationError

def create_advanced_agent(brain: Brain, world: SimpleWorld, 
                          config: Optional[Dict[str, Any]] = None) -> AgentBrain:
    """
    Create an advanced agent with enhanced capabilities.
    
    Args:
        brain: Brain instance to control.
        world: World instance to interact with.
        config: Optional configuration dictionary.
        
    Returns:
        Advanced AgentBrain instance.
    """
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    if config:
        # Apply agent-specific configuration
        if config.get('enable_reward_modulation', True):
            agent.enableRewardModulation(True)
        
        if config.get('enable_structural_plasticity', True):
            agent.enableStructuralPlasticity(True)
        
        if config.get('enable_development', True):
            agent.enableDevelopment(True)
        
        if config.get('enable_curiosity', True):
            agent.enableCuriosity(True)
    
    return agent

def create_learning_agent(brain: Brain, world: SimpleWorld,
                          learning_config: Optional[Dict[str, Any]] = None) -> AgentBrain:
    """
    Create a learning agent with specific learning configuration.
    
    Args:
        brain: Brain instance to control.
        world: World instance to interact with.
        learning_config: Learning configuration dictionary.
        
    Returns:
        Learning agent instance.
    """
    agent = create_advanced_agent(brain, world)
    
    if learning_config:
        # Configure learning parameters
        if 'reward_mode' in learning_config:
            agent.applyRewardModulation(learning_config['reward_mode'], 0.0)
        
        if 'learning_rate' in learning_config:
            # Set learning rate (placeholder - actual implementation depends on NLM API)
            pass
    
    return agent

def run_learning_episode(agent: AgentBrain, world: SimpleWorld,
                         max_steps: int = 1000) -> Dict[str, Any]:
    """
    Run a learning episode with an agent.
    
    Args:
        agent: Agent brain instance.
        world: World instance.
        max_steps: Maximum number of steps to run.
        
    Returns:
        Dictionary containing episode results.
    """
    results = {
        'steps': 0,
        'rewards': [],
        'actions': [],
        'success': True,
        'error': None,
    }
    
    try:
        for step in range(max_steps):
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record results
            results['steps'] = step + 1
            results['actions'].append(action)
            
            # Get reward if available
            if hasattr(percept, 'getInternal') and percept.getInternal():
                results['rewards'].append(percept.getInternal()[0])
    
    except Exception as e:
        results['success'] = False
        results['error'] = str(e)
    
    return results

def analyze_agent_performance(agent: AgentBrain) -> Dict[str, Any]:
    """
    Analyze agent performance metrics.
    
    Args:
        agent: Agent brain instance to analyze.
        
    Returns:
        Dictionary containing performance metrics.
    """
    performance = {}
    
    # Get neuromodulation levels
    performance['neuromodulation'] = {
        'level': agent.getNeuromodulationLevel(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
    }
    
    # Get developmental stage
    performance['development'] = {
        'stage': agent.getDevelopmentalStage(),
    }
    
    # Get learning status
    performance['learning'] = {
        'reward_enabled': agent.isRewardModulationEnabled(),
        'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled(),
        'development_enabled': agent.isDevelopmentEnabled(),
        'curiosity_enabled': agent.isCuriosityEnabled(),
    }
    
    return performance

def create_reinforcement_agent(brain: Brain, world: SimpleWorld,
                               reward_config: Optional[Dict[str, Any]] = None) -> AgentBrain:
    """
    Create a reinforcement learning agent.
    
    Args:
        brain: Brain instance to control.
        world: World instance to interact with.
        reward_config: Reward configuration dictionary.
        
    Returns:
        Reinforcement agent instance.
    """
    agent = create_advanced_agent(brain, world)
    
    # Enable reward-based learning
    agent.enableRewardModulation(True)
    
    if reward_config:
        # Configure reward parameters
        if 'reward_scale' in reward_config:
            # Set reward scale (placeholder)
            pass
    
    return agent

def create_curiosity_agent(brain: Brain, world: SimpleWorld,
                           curiosity_config: Optional[Dict[str, Any]] = None) -> AgentBrain:
    """
    Create a curiosity-driven agent.
    
    Args:
        brain: Brain instance to control.
        world: World instance to interact with.
        curiosity_config: Curiosity configuration dictionary.
        
    Returns:
        Curiosity agent instance.
    """
    agent = create_advanced_agent(brain, world)
    
    # Enable curiosity
    agent.enableCuriosity(True)
    
    if curiosity_config:
        # Configure curiosity parameters
        if 'exploration_rate' in curiosity_config:
            # Set exploration rate (placeholder)
            pass
    
    return agent