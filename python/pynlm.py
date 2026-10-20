# NLM Python module - High-level Python API for NLM
# Provides Pythonic wrappers around the low-level C++ bindings

import ctypes
import os
import sys
from typing import Optional, List, Dict, Any, Tuple, Union
import warnings

# Try to import the C++ bindings
try:
    from pynlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Classes
        Config, SensoryInput, Vision, Audio, InternalSignals,
        Action, WorldObject, AgentBody, ActionResult, SensoryPercept,
        SimpleWorld, Brain, AgentBrain,
        
        # Factory functions
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
    )
    
    C_BINDINGS_AVAILABLE = True
except ImportError as e:
    C_BINDINGS_AVAILABLE = False
    print(f"Warning: C++ bindings not available: {e}")
    
    # Create stub classes for when C++ bindings are not available
    class NeuronId:
        def __init__(self, value: int = 0):
            self.value = value
        
        def index(self) -> int:
            return self.value
        
        def __eq__(self, other):
            return isinstance(other, NeuronId) and self.value == other.value
        
        def __hash__(self):
            return hash(self.value)
        
        def __repr__(self):
            return f"NeuronId({self.value})"
    
    # Create other stub classes as needed...
    # For brevity, I'll just show the main ones and the user should
    # install the C++ bindings to get full functionality

class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigError(NLMError):
    """Configuration error"""
    pass

class BrainError(NLMError):
    """Brain simulation error"""
    pass

class ValidationError(NLMError):
    """Validation error"""
    pass

# High-level helper functions
def validate_config(config: Config) -> bool:
    """
    Validate a configuration for basic sanity checks.
    
    Args:
        config: The configuration to validate
        
    Returns:
        True if configuration is valid, False otherwise
        
    Raises:
        ValidationError: If configuration is invalid
    """
    if not config.has("neuron_count"):
        raise ValidationError("Configuration must specify neuron_count")
    
    neuron_count = config.getOr[size_t]("neuron_count", 1000)
    if neuron_count == 0:
        raise ValidationError("neuron_count must be positive")
    
    if not config.has("region_count"):
        raise ValidationError("Configuration must specify region_count")
    
    region_count = config.getOr[size_t]("region_count", 1)
    if region_count == 0:
        raise ValidationError("region_count must be positive")
    
    return True

class NLMSimulator:
    """
    High-level NLM simulator that provides a more Pythonic interface
    to brain simulation with common patterns and utilities.
    """
    
    def __init__(self, config: Optional[Config] = None):
        """
        Initialize an NLM simulator.
        
        Args:
            config: Optional configuration. If None, creates a default config.
        """
        if config is None:
            config = createDefaultConfig()
        
        self.config = config
        self.brain: Optional[Brain] = None
        self.world: Optional[SimpleWorld] = None
        self.agent: Optional[AgentBrain] = None
        
        # Default configuration for Phase 6 integration
        self._configure_default()
        
        # Validate configuration
        validate_config(self.config)
    
    def _configure_default(self):
        """Set default configuration values for Phase 6."""
        defaults = {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.15,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_tau": 20.0,
            "development_synaptogenesis_rate": 0.0001,
            "development_pruning_rate": 0.00001,
            "dopamine_baseline": 0.1,
            "reward_discount_factor": 0.99,
            "environment_name": "GridWorld",
            "environment_width": 10,
            "environment_height": 10,
            "log_level": "INFO",
            "max_simulation_steps": 10000,
            "visualization_enabled": False,
        }
        
        for key, value in defaults.items():
            if not self.config.has(key):
                self.config.set(key, value)
    
    def create_brain(self) -> Brain:
        """
        Create and initialize a brain.
        
        Returns:
            The initialized Brain object
        """
        if self.brain is not None:
            raise BrainError("Brain already created. Call reset() first.")
        
        self.brain = createBrain(self.config)
        
        if not self.brain.initialize():
            raise BrainError("Failed to initialize brain")
        
        return self.brain
    
    def reset(self):
        """Reset the simulator and all its components."""
        if self.brain is not None:
            self.brain.reset()
        
        if self.world is not None:
            self.world.reset()
        
        if self.agent is not None:
            self.agent.reset()
        
        # Re-initialize brain if it was created
        if self.brain is not None:
            self.brain.initialize()
    
    def run_simulation(self, num_steps: int, world: Optional[SimpleWorld] = None,
                      agent: Optional[AgentBrain] = None, 
                      auto_sensory: bool = True) -> Dict[str, Any]:
        """
        Run a simulation for the specified number of steps.
        
        Args:
            num_steps: Number of simulation steps to run
            world: Optional world to use. If None, creates a default world.
            agent: Optional agent to use. If None, creates an agent brain.
            auto_sensory: If True, automatically generate simple sensory input.
            
        Returns:
            Dictionary with simulation statistics
        """
        # Create world and agent if not provided
        if world is not None:
            self.world = world
        elif self.world is None:
            self.world = createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
        
        if agent is not None:
            self.agent = agent
        elif self.agent is None:
            if self.brain is None:
                self.create_brain()
            self.agent = createAgentBrain(self.brain)
            self.agent.initialize(self.world)
        
        # Run simulation
        statistics = {
            'steps': num_steps,
            'total_spikes': 0,
            'firing_count': 0,
            'avg_firing_rate': 0.0,
            'curiosity_level': 0.0,
            'novelty_level': 0.0,
            'dopamine_level': 0.0,
            'episodes_stored': 0,
            'working_memory_traces': 0,
            'development_stage': None
        }
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Add simple sensory input if requested
            if auto_sensory and step % 10 == 0:
                # Simple visual pattern
                vision = Vision(8, 8, 3)
                data = [0.0] * 8 * 8 * 3
                
                # Create a simple pattern (white cross on black background)
                for i in range(8):
                    for j in range(8):
                        if i == j or i + j == 7:  # Diagonal cross
                            idx = (i * 8 + j) * 3
                            data[idx] = 0.5    # Red
                            data[idx + 1] = 0.5  # Green
                            data[idx + 2] = 0.5  # Blue
                
                vision.setData(data)
                percept.setVision(vision)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Collect statistics
            if self.brain.getFiringNeuronCount() > 0:
                statistics['firing_count'] += 1
            
            statistics['total_spikes'] += self.brain.getTotalSpikeCount()
            
            # Get agent statistics
            if self.agent.getCuriosity():
                statistics['curiosity_level'] = self.agent.getCuriosityLevel()
            
            if self.agent.getNovelty():
                statistics['novelty_level'] = self.agent.getNoveltyLevel()
            
            if self.agent.getNeuromodulationLevel():
                statistics['dopamine_level'] = self.agent.getNeuromodulationLevel()
            
            statistics['development_stage'] = self.brain.getDevelopmentalStage()
            
            if self.brain.getWorkingMemory():
                statistics['working_memory_traces'] = self.brain.getWorkingMemory().getActiveTraces()
            
            if self.brain.getEpisodicMemory():
                statistics['episodes_stored'] = self.brain.getEpisodicMemory().getEpisodeCount()
            
            statistics['avg_firing_rate'] = (statistics['avg_firing_rate'] * step + 
                                           self.brain.getAverageFiringRate()) / (step + 1)
            
            # Print progress every 100 steps
            if step % 100 == 0 and step > 0:
                print(f"Step {step}/{num_steps}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Avg rate: {self.brain.getAverageFiringRate():.3f}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
        
        # Calculate final statistics
        statistics['avg_firing_rate'] = (statistics['avg_firing_rate'] * num_steps - 
                                       (num_steps * (num_steps - 1) / 2)) / num_steps
        
        return statistics
    
    def enable_all_subsystems(self):
        """Enable all optional subsystems for maximum functionality."""
        if self.agent is not None:
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)
        
        # Set development configuration
        self.config.set("development_enabled", True)
        self.config.set("development_age_rate", 0.1)
        
        # Set neuromodulation configuration
        self.config.set("curiosity_enabled", True)
        self.config.set("novelty_enabled", True)
        self.config.set("dopamine_scale", 1.0)
        
        # Set cognition configuration
        self.config.set("planning_enabled", True)
        self.config.set("concept_formation_enabled", True)
        self.config.set("attention_enabled", True)
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """
        Get comprehensive brain statistics.
        
        Returns:
            Dictionary with brain statistics
        """
        if self.brain is None:
            raise BrainError("Brain not initialized. Call create_brain() first.")
        
        stats = {
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'development_stage': self.brain.getDevelopmentalStage(),
        }
        
        # Add memory statistics if available
        if self.brain.getWorkingMemory():
            stats['working_memory_capacity'] = self.brain.getWorkingMemory().getCapacity()
            stats['working_memory_traces'] = self.brain.getWorkingMemory().getActiveTraces()
        
        if self.brain.getEpisodicMemory():
            stats['episodic_memory_episodes'] = self.brain.getEpisodicMemory().getEpisodeCount()
            stats['episodic_memory_max_episodes'] = self.brain.getEpisodicMemory().getMaxEpisodes()
        
        if self.brain.getAssociativeMemory():
            stats['associative_memory_patterns'] = self.brain.getAssociativeMemory().getPatternCount()
        
        # Add neuromodulation statistics if available
        if self.brain.getDopamine():
            stats['dopamine_level'] = self.brain.getDopamine().getLevel()
            stats['dopamine_plasticity_factor'] = self.brain.getDopamine().getPlasticityFactor()
        
        if self.brain.getCuriosity():
            stats['curiosity_level'] = self.brain.getCuriosity().getLevel()
        
        if self.brain.getNovelty():
            stats['novelty_level'] = self.brain.getNovelty().getLevel()
        
        # Add cognition statistics if available
        if self.brain.getPlanner():
            stats['planner_depth'] = self.brain.getPlanner().getPlanningDepth()
        
        if self.brain.getAttention():
            stats['attention_inhibition'] = self.brain.getAttention().getInhibitionStrength()
            stats['attention_excitation'] = self.brain.getAttention().getExcitationStrength()
        
        return stats

# Backwards compatibility - create module-level functions that wrap the simulator

def create_default_config() -> Config:
    """Create a default configuration."""
    return createDefaultConfig()

def create_brain(config: Config) -> Brain:
    """Create a brain with the given configuration."""
    return Brain(config)

def create_simple_world() -> SimpleWorld:
    """Create a simple world."""
    return SimpleWorld()

def create_agent_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface."""
    return AgentBrain(brain)

# Version and metadata
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Classes
    'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # High-level classes
    'NLMSimulator', 'NLMError', 'ConfigError', 'BrainError', 'ValidationError',
    
    # Helper functions
    'validate_config',
    
    # Module-level functions for backwards compatibility
    'create_default_config', 'create_brain', 'create_simple_world', 'create_agent_brain',
]