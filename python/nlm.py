"""
NLM (Neural Learning Machine) - Pythonic API

This module provides a Pythonic interface to the NLM neural simulation framework.
It wraps the C++ bindings to provide a more intuitive Python experience with
type hints, better naming conventions, and common Python patterns.
"""

import sys
from typing import List, Optional, Dict, Any, Union, Tuple
import numpy as np

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = None

# Import the C++ bindings directly
if PYNLM_AVAILABLE:
    from pynlm import (
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        Config, Brain, AgentBrain, SimpleWorld, 
        SensoryInput, Vision, Audio, InternalSignals,
        SensoryPercept, Action, WorldObject,
        createDefaultConfig, createBrain, createSimpleWorld,
        createAgentBrain
    )

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Raised when there's an error with configuration."""
    pass

class SimulationError(NLMError):
    """Raised when there's an error during simulation."""
    pass

class MemoryError(NLMError):
    """Raised when there's an error with memory systems."""
    pass

class NLMBrain:
    """
    A Pythonic wrapper around the NLM Brain class.
    
    This class provides a more intuitive Python interface for interacting
    with the neural brain simulation, with better error handling, type hints,
    and convenience methods.
    """
    
    def __init__(self, config: Optional[Config] = None):
        """
        Initialize a new NLM brain.
        
        Args:
            config: Configuration object. If None, creates a default config.
        """
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm module not available")
            
        if config is None:
            config = createDefaultConfig()
            
        self._brain = createBrain(config)
        self._brain.initialize()
        self._config = config
        
    def step(self, step_number: int, timestep: float = 0.001) -> Dict[str, Any]:
        """
        Advance the simulation by one time step.
        
        Args:
            step_number: The step number (used for tracking)
            timestep: Time step size in seconds
            
        Returns:
            Dictionary containing simulation statistics
        """
        self._brain.step(step_number, timestep)
        return self.get_stats()
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get current brain statistics.
        
        Returns:
            Dictionary containing brain statistics
        """
        return {
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'pending_spikes': self._brain.getPendingSpikeEventCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'development_stage': self._brain.getDevelopmentalStage(),
            'working_memory_traces': (
                self._brain.getWorkingMemory().getActiveTraces() if self._brain.getWorkingMemory() else 0
            ),
            'episodic_memory_episodes': (
                self._brain.getEpisodicMemory().getEpisodeCount() if self._brain.getEpisodicMemory() else 0
            ),
            'dopamine_level': (
                self._brain.getDopamine().getLevel() if self._brain.getDopamine() else 0.0
            ),
            'curiosity_level': (
                self._brain.getCuriosity().getLevel() if self._brain.getCuriosity() else 0.0
            ),
            'novelty_level': (
                self._brain.getNovelty().getLevel() if self._brain.getNovelty() else 0.0
            ),
        }
    
    def add_region(self, name: str = "") -> RegionId:
        """
        Add a new brain region.
        
        Args:
            name: Name for the region (optional)
            
        Returns:
            RegionId of the newly created region
        """
        return self._brain.addRegion(name)
    
    def get_region(self, region_id: RegionId):
        """
        Get a brain region by ID.
        
        Args:
            region_id: The RegionId to look up
            
        Returns:
            NeuralRegion object or None if not found
        """
        return self._brain.getRegion(region_id)
    
    def get_all_regions(self) -> List:
        """
        Get all brain regions.
        
        Returns:
            List of all NeuralRegion objects
        """
        return self._brain.getRegions()
    
    def reset(self):
        """Reset the brain to initial state."""
        self._brain.reset()
        self._brain.initialize()
    
    def save(self, filepath: str) -> bool:
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save the checkpoint file
            
        Returns:
            True if save was successful, False otherwise
        """
        return self._brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to the checkpoint file
            
        Returns:
            True if load was successful, False otherwise
        """
        return self._brain.load(filepath)

class NLMWorld:
    """
    A Pythonic wrapper around the SimpleWorld class.
    
    Provides a more intuitive interface for working with the simulated environment.
    """
    
    def __init__(self):
        """Initialize a new world."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm module not available")
            
        self._world = createSimpleWorld()
        
    def configure(self, width: int, height: int, 
                  vision_width: Optional[int] = None, 
                  vision_height: Optional[int] = None):
        """
        Configure the world dimensions.
        
        Args:
            width: World width in units
            height: World height in units
            vision_width: Agent vision width (defaults to world width)
            vision_height: Agent vision height (defaults to world height)
        """
        if vision_width is None:
            vision_width = width
        if vision_height is None:
            vision_height = height
            
        self._world.configure(width, height, vision_width, vision_height)
        
    def reset(self):
        """Reset the world to initial state."""
        self._world.reset()
        
    def set_agent_start(self, x: float, y: float):
        """
        Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.setAgentStart(x, y)
        
    def update(self, timestep: float) -> SensoryPercept:
        """
        Update the world simulation.
        
        Args:
            timestep: Time step to advance
            
        Returns:
            SensoryPercept object containing sensor data
        """
        self._world.update(timestep)
        return self._world.getSensoryPercept()
        
    def get_sensory_input(self) -> Dict[str, Any]:
        """
        Get current sensory input from the world.
        
        Returns:
            Dictionary containing vision, touch, internal, and proprioception data
        """
        percept = self._world.getSensoryPercept()
        
        return {
            'vision': percept.getVision() if percept.getVision() else None,
            'touch': percept.getTouch() if percept.getTouch() else None,
            'internal': percept.getInternal() if percept.getInternal() else None,
            'proprioception': percept.getProprioception() if percept.getProprioception() else None,
            'audio': percept.getAudio() if percept.getAudio() else None,
        }
        
    def apply_action(self, action: Action, timestep: float):
        """
        Apply an action to the world.
        
        Args:
            action: Action to apply
            timestep: Current simulation time
        """
        self._world.applyMotorCommand(action, timestep)
        
    def get_agent_body(self) -> Dict[str, Any]:
        """
        Get agent body state.
        
        Returns:
            Dictionary containing agent state
        """
        body = self._world.getAgentBody()
        return {
            'x': body.x,
            'y': body.y,
            'orientation': body.orientation,
            'velocity_x': body.velocityX,
            'velocity_y': body.velocityY,
            'angular_velocity': body.angularVelocity,
            'energy': body.energy,
            'health': body.health,
            'age': body.age,
            'is_moving': body.isMoving,
            'is_turning': body.isTurning,
            'last_action_time': body.lastActionTime,
        }

class NLMEnvironment:
    """
    High-level interface for complete agent-environment interaction.
    
    This class provides a complete framework for running agent simulations
    with brain, world, and agent integration.
    """
    
    def __init__(self, config: Optional[Config] = None):
        """
        Initialize the environment.
        
        Args:
            config: Configuration object (optional)
        """
        self.brain = NLMBrain(config)
        self.world = NLMWorld()
        self.agent = None
        self._step_count = 0
        
    def setup(self, world_width: int = 20, world_height: int = 20,
              vision_width: int = 8, vision_height: int = 8,
              agent_start_x: float = 10.0, agent_start_y: float = 10.0):
        """
        Set up the complete environment.
        
        Args:
            world_width: World width
            world_height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
            agent_start_x: Initial agent X position
            agent_start_y: Initial agent Y position
        """
        # Create world
        self.world.configure(world_width, world_height, vision_width, vision_height)
        self.world.reset()
        self.world.set_agent_start(agent_start_x, agent_start_y)
        
        # Create agent
        self.agent = createAgentBrain(self.brain._brain)
        self.agent.initialize(self.world._world)
        
        # Enable subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
    def run_episode(self, max_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """
        Run a complete simulation episode.
        
        Args:
            max_steps: Maximum number of steps to run
            timestep: Simulation time step
            
        Returns:
            Dictionary containing episode statistics
        """
        if self.agent is None:
            raise RuntimeError("Environment not set up. Call setup() first.")
            
        self._step_count = 0
        episode_stats = {
            'steps_completed': 0,
            'total_reward': 0.0,
            'avg_firing_rate': 0.0,
            'curiosity_levels': [],
            'novelty_levels': [],
            'development_stages': [],
        }
        
        for step in range(max_steps):
            # Update world
            percept = self.world.update(timestep)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain._brain.step(step, step * timestep)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            self.world.apply_action(action, step * timestep)
            
            # Apply reward modulation
            reward = 0.0
            internal_signals = percept.getInternal()
            if internal_signals and not internal_signals.empty():
                reward = internal_signals[0]
                
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            # Collect statistics
            episode_stats['steps_completed'] += 1
            episode_stats['total_reward'] += reward
            
            if step % 10 == 0:  # Sample every 10 steps
                episode_stats['curiosity_levels'].append(self.agent.getCuriosityLevel())
                episode_stats['novelty_levels'].append(self.agent.getNoveltyLevel())
                episode_stats['development_stages'].append(self.brain._brain.getDevelopmentalStage())
            
            self._step_count += 1
            
        # Calculate averages
        if episode_stats['steps_completed'] > 0:
            episode_stats['avg_firing_rate'] = (
                self.brain._brain.getAverageFiringRate()
            )
            episode_stats['avg_curiosity'] = (
                sum(episode_stats['curiosity_levels']) / len(episode_stats['curiosity_levels'])
                if episode_stats['curiosity_levels'] else 0.0
            )
            episode_stats['avg_novelty'] = (
                sum(episode_stats['novelty_levels']) / len(episode_stats['novelty_levels'])
                if episode_stats['novelty_levels'] else 0.0
            )
            
        return episode_stats

# Convenience functions for common use cases
def create_simple_brain(neuron_count: int = 1000, region_count: int = 1,
                         connection_probability: float = 0.1) -> NLMBrain:
    """
    Create a simple brain with default configuration.
    
    Args:
        neuron_count: Number of neurons
        region_count: Number of regions
        connection_probability: Connection probability between neurons
        
    Returns:
        NLMBrain instance
    """
    config = createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", connection_probability)
    
    return NLMBrain(config)

def run_simple_simulation(brain: NLMBrain, world: NLMWorld, 
                          agent: AgentBrain, max_steps: int, 
                          timestep: float = 0.1) -> Dict[str, Any]:
    """
    Run a simple simulation with brain, world, and agent.
    
    Args:
        brain: NLMBrain instance
        world: NLMWorld instance
        agent: AgentBrain instance
        max_steps: Maximum number of steps
        timestep: Time step size
        
    Returns:
        Dictionary containing simulation results
    """
    # Setup world
    world.configure(20, 20, 8, 8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world._world)
    
    # Run simulation
    results = {
        'steps': [],
        'reward_history': [],
        'firing_history': [],
    }
    
    for step in range(max_steps):
        # Update world
        percept = world.update(timestep)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain._brain.step(step, step * timestep)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.apply_action(action, step * timestep)
        
        # Get reward and apply neuromodulation
        reward = 0.0
        internal = percept.getInternal()
        if internal and not internal.empty():
            reward = internal[0]
            
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect results
        results['steps'].append(step)
        results['reward_history'].append(reward)
        results['firing_history'].append(brain._brain.getFiringNeuronCount())
    
    return results

# Make the main classes available at module level
__all__ = [
    'NLMError', 'ConfigurationError', 'SimulationError', 'MemoryError',
    'NLMBrain', 'NLMWorld', 'NLMEnvironment',
    'create_simple_brain', 'run_simple_simulation',
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
]

# Version
__version__ = "0.1.0"

# Check if pynlm is available
if not PYNLM_AVAILABLE:
    raise ImportError(
        "pynlm module not available. Please install it first:\n"
        "pip install pynlm\n"
        "or build from source:\n"
        "cd python && pip install -e ."
    )
