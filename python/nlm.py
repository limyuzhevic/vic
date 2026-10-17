#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Python Interface
================================================

A Pythonic interface to the NLM (Neural Learning Machine) neural simulation framework.
Provides high-level APIs for creating and running neural brain simulations with
automatic error handling, convenience functions, and comprehensive documentation.

Key Features:
- Simple, Pythonic API design
- Automatic error handling with informative exceptions
- Convenience factory methods and presets
- Comprehensive documentation and examples
- Full coverage of important C++ APIs
"""

import json
import os
import warnings
from typing import Any, Dict, List, Optional, Union, Callable
from enum import Enum

# Try to import the C++ bindings
try:
    import pynlm
    _has_c_bindings = True
except ImportError:
    _has_c_bindings = False
    pynlm = None

# Custom exceptions for better error handling
class NLMLibError(Exception):
    """Base exception for all NLM library errors."""
    pass

class ConfigError(NLMLibError):
    """Raised when configuration errors occur."""
    pass

class BrainError(NLMLibError):
    """Raised when brain operation errors occur."""
    pass

class WorldError(NLMLibError):
    """Raised when world operation errors occur."""
    pass

class SimulationError(NLMLibError):
    """Raised when simulation errors occur."""
    pass

# Pythonic type wrappers and result objects
class Result:
    """
    Generic result object for operations that return success/failure.
    
    Examples:
        >>> result = Result.from_operation(lambda: brain.initialize())
        >>> if result.success:
        ...     print("Brain initialized successfully")
        ... else:
        ...     print(f"Error: {result.error}")
    """
    
    def __init__(self, success: bool, data: Any = None, error: Optional[str] = None):
        self.success = success
        self.data = data
        self.error = error
    
    @classmethod
    def from_operation(cls, operation: Callable, *args, **kwargs) -> 'Result':
        """
        Execute an operation and return a Result object.
        
        Args:
            operation: Callable to execute
            *args, **kwargs: Arguments to pass to the operation
            
        Returns:
            Result object with success status and either data or error
        """
        try:
            data = operation(*args, **kwargs)
            return cls(True, data)
        except Exception as e:
            return cls(False, error=str(e))
    
    def __repr__(self) -> str:
        return f"<Result success={self.success} error={self.error}>"

class SimulationStats:
    """
    Comprehensive statistics about a simulation run.
    
    Provides easy access to brain and agent statistics for analysis.
    """
    
    def __init__(self, brain_stats: Dict[str, Any], agent_stats: Dict[str, Any], 
                 world_stats: Dict[str, Any], step_count: int):
        self.brain_stats = brain_stats
        self.agent_stats = agent_stats
        self.world_stats = world_stats
        self.step_count = step_count
    
    def get_active_neurons(self) -> int:
        """Get the number of currently active neurons."""
        return self.brain_stats.get('active_neuron_count', 0)
    
    def get_firing_rate(self) -> float:
        """Get the average firing rate across all neurons."""
        return self.brain_stats.get('average_firing_rate', 0.0)
    
    def get_ei_ratio(self) -> float:
        """Get the excitation/inhibition ratio."""
        return self.brain_stats.get('excitation_inhibition_ratio', 0.0)
    
    def get_total_spikes(self) -> int:
        """Get the total number of spikes recorded."""
        return self.brain_stats.get('total_spike_count', 0)
    
    def get_curiosity_level(self) -> float:
        """Get the curiosity level from the agent."""
        return self.agent_stats.get('curiosity_level', 0.0)
    
    def get_novelty_level(self) -> float:
        """Get the novelty level from the agent."""
        return self.agent_stats.get('novelty_level', 0.0)
    
    def get_development_stage(self) -> Any:
        """Get the current developmental stage."""
        return self.brain_stats.get('developmental_stage')
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert statistics to dictionary for easy export."""
        return {
            'step_count': self.step_count,
            'brain_stats': self.brain_stats,
            'agent_stats': self.agent_stats,
            'world_stats': self.world_stats
        }
    
    def __repr__(self) -> str:
        return (f"<SimulationStats steps={self.step_count} "
                f"active_neurons={self.get_active_neurons()} "
                f"firing_rate={self.get_firing_rate():.3f}>')

# Configuration presets for different use cases
class ConfigPresets:
    """
    Pre-configured settings presets for different NLM use cases.
    
    Examples:
        >>> # Quick development environment
        >>> config = nlm.create_config(preset='quick_dev')
        >>> 
        >>> # High-performance simulation
        >>> config = nlm.create_config(preset='high_perf')
        >>> 
        >>> # Learning-focused simulation
        >>> config = nlm.create_config(preset='learning')
    """
    
    @staticmethod
    def quick_dev() -> Dict[str, Any]:
        """Quick development configuration with fast iteration."""
        return {
            'brain.neuron_count': 500,
            'brain.synapse_density': 0.15,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.005,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'environment.max_steps': 100,
            'environment.render': False
        }
    
    @staticmethod
    def high_perf() -> Dict[str, Any]:
        """High-performance configuration for maximum throughput."""
        return {
            'brain.neuron_count': 5000,
            'brain.synapse_density': 0.2,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': False,  # Performance trade-off
            'neuromod.curiosity.enable': False,  # Performance trade-off
            'neuromod.novelty.enable': True,
            'environment.max_steps': 10000,
            'environment.render': False,
            'core.threads': 4,
            'memory.pool_size': 1024
        }
    
    @staticmethod
    def learning() -> Dict[str, Any]:
        """Learning-focused configuration for development and training."""
        return {
            'brain.neuron_count': 2000,
            'brain.synapse_density': 0.25,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'neuromod.dopamine.scale': 1.0,
            'development.initial_stage': 'CriticalPeriod',
            'environment.max_steps': 5000,
            'environment.render': False,
            'brain.enable_prediction': True,
            'brain.enable_memory': True,
            'brain.enable_cognition': True
        }
    
    @staticmethod
    def production() -> Dict[str, Any]:
        """Production-ready configuration for stable simulations."""
        return {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.1,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.0005,
            'plasticity.hebbian.enable': False,  # Stability over learning
            'plasticity.structural.enable': False,
            'neuromod.curiosity.enable': True,  # For exploration
            'neuromod.novelty.enable': True,
            'neuromod.dopamine.scale': 0.8,
            'development.initial_stage': 'Maturation',
            'environment.max_steps': 1000,
            'environment.render': False,
            'brain.enable_prediction': True,
            'brain.enable_memory': True,
            'brain.enable_cognition': False,  # Simplicity for production
            'brain.checkpoints': True,
            'brain.save_frequency': 100
        }
    
    @staticmethod
    def get_preset(preset_name: str) -> Dict[str, Any]:
        """
        Get configuration preset by name.
        
        Args:
            preset_name: Name of preset ('quick_dev', 'high_perf', 'learning', 'production')
            
        Returns:
            Dictionary of configuration settings
            
        Raises:
            ConfigError: If preset name is not recognized
        """
        presets = {
            'quick_dev': ConfigPresets.quick_dev(),
            'high_perf': ConfigPresets.high_perf(),
            'learning': ConfigPresets.learning(),
            'production': ConfigPresets.production()
        }
        
        preset_name = preset_name.lower()
        if preset_name not in presets:
            raise ConfigError(f"Unknown preset '{preset_name}'. Available: {list(presets.keys())}")
        
        return presets[preset_name]

# Simulation helpers and convenience functions
class BrainFactory:
    """
    Factory class for creating brains with convenient presets.
    
    Examples:
        >>> # Create a simple brain for testing
        >>> brain = nlm.create_brain(size='small')
        >>> 
        >>> # Create a learning brain
        >>> brain = nlm.create_brain(preset='learning')
        >>> brain = brain.with_development(True).with_neuromodulation(True)
        >>> 
        >>> # Create a production brain
        >>> brain = nlm.create_brain(preset='production')
    """
    
    def __init__(self):
        if not _has_c_bindings:
            raise NLMLibError("pynlm C++ bindings not available")
    
    def create_brain(self, size: str = 'medium', preset: Optional[str] = None, 
                     custom_config: Optional[Dict[str, Any]] = None) -> Any:
        """
        Create a brain with specified size and preset.
        
        Args:
            size: Brain size ('small', 'medium', 'large')
            preset: Optional preset configuration
            custom_config: Additional custom configuration
            
        Returns:
            Configured Brain object
            
        Raises:
            BrainError: If brain creation fails
        """
        if not _has_c_bindings:
            raise NLMLibError("pynlm C++ bindings not available")
        
        try:
            # Create base configuration
            config = pynlm.createDefaultConfig()
            
            # Apply size preset
            size_configs = {
                'small': {'brain.neuron_count': 500, 'brain.synapse_density': 0.15},
                'medium': {'brain.neuron_count': 1000, 'brain.synapse_density': 0.2},
                'large': {'brain.neuron_count': 5000, 'brain.synapse_density': 0.25}
            }
            
            if size in size_configs:
                for key, value in size_configs[size].items():
                    config.set(key, value)
            
            # Apply preset if specified
            if preset:
                preset_config = ConfigPresets.get_preset(preset)
                for key, value in preset_config.items():
                    config.set(key, value)
            
            # Apply custom configuration
            if custom_config:
                for key, value in custom_config.items():
                    config.set(key, value)
            
            # Create and return brain
            brain = pynlm.createBrain(config)
            if not brain.initialize():
                raise BrainError("Failed to initialize brain")
            
            return brain
            
        except Exception as e:
            raise BrainError(f"Failed to create brain: {str(e)}")
    
    def create_brain_with_dev(self, development_enabled: bool = True, **kwargs) -> Any:
        """
        Create a brain with development enabled.
        
        Args:
            development_enabled: Whether to enable development
            **kwargs: Additional configuration options
            
        Returns:
            Configured Brain object
        """
        config = {'brain.development.enable': development_enabled}
        config.update(kwargs)
        return self.create_brain(custom_config=config)
    
    def create_brain_with_neuromod(self, neuromodulation_enabled: bool = True, **kwargs) -> Any:
        """
        Create a brain with neuromodulation enabled.
        
        Args:
            neuromodulation_enabled: Whether to enable neuromodulation
            **kwargs: Additional configuration options
            
        Returns:
            Configured Brain object
        """
        config = {
            'neuromod.curiosity.enable': neuromodulation_enabled,
            'neuromod.novelty.enable': neuromodulation_enabled,
            'neuromod.dopamine.scale': 1.0
        }
        config.update(kwargs)
        return self.create_brain(custom_config=config)

class WorldHelper:
    """
    Helper class for world creation and management.
    
    Examples:
        >>> # Create a simple world
        >>> world = nlm.create_world(width=20, height=20)
        >>> 
        >>> # Create world with agent
        >>> world = nlm.create_world(width=30, height=30)
        >>> world.set_agent_start(15.0, 15.0)
        >>> 
        >>> # Run world with simulation helper
        >>> stats = nlm.run_simulation(world, brain, agent, steps=1000)
    """
    
    def __init__(self):
        if not _has_c_bindings:
            raise NLMLibError("pynlm C++ bindings not available")
    
    def create_world(self, width: int = 20, height: int = 20, 
                     vision_width: int = 8, vision_height: int = 8,
                     agent_start: Optional[tuple] = None) -> Any:
        """
        Create a simple world with specified dimensions.
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Agent vision width in cells
            vision_height: Agent vision height in cells
            agent_start: Tuple of (x, y) starting position for agent
            
        Returns:
            Configured SimpleWorld object
        """
        try:
            world = pynlm.createSimpleWorld()
            world.configure(width, height, vision_width, vision_height)
            
            if agent_start:
                world.setAgentStart(agent_start[0], agent_start[1])
            
            world.reset()
            return world
            
        except Exception as e:
            raise WorldError(f"Failed to create world: {str(e)}")

class SimulationRunner:
    """
    High-level simulation runner with logging and statistics collection.
    
    Examples:
        >>> # Run simple simulation
        >>> stats = nlm.run_simulation(world, brain, agent, steps=1000)
        >>> 
        >>> # Run with custom configuration
        >>> stats = nlm.run_simulation(world, brain, agent, steps=500,
        ...                           callback=lambda s: print(f"Step {s.step}: {s.active_neurons} neurons"))
    """
    
    def __init__(self):
        if not _has_c_bindings:
            raise NLMLibError("pynlm C++ bindings not available")
    
    def run_simulation(self, world: Any, brain: Any, agent: Any, 
                      steps: int, timestep: float = 0.1,
                      callback: Optional[Callable] = None) -> SimulationStats:
        """
        Run a complete simulation with agent, brain, and world.
        
        Args:
            world: SimpleWorld instance
            brain: Brain instance
            agent: AgentBrain instance
            steps: Number of simulation steps to run
            timestep: Time step size
            callback: Optional callback function called each step
            
        Returns:
            SimulationStats object with comprehensive statistics
            
        Raises:
            SimulationError: If simulation fails
        """
        try:
            # Initialize agent
            agent.initialize(world)
            
            stats = {
                'brain_stats': [],
                'agent_stats': [],
                'world_stats': []
            }
            
            for step in range(steps):
                # Update world
                world.update(timestep)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Decode motor command
                motor_cmd = agent.decodeMotorCommand()
                
                # Apply motor command
                action_result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Apply reward modulation
                reward = 0.0
                if percept.getInternal():
                    reward = percept.getInternal()[0]
                
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(timestep)
                
                # Collect statistics
                brain_stats = {
                    'step': step,
                    'active_neuron_count': brain.getActiveNeuronCount(),
                    'firing_neuron_count': brain.getFiringNeuronCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                    'total_spike_count': brain.getTotalSpikeCount(),
                    'developmental_stage': brain.getDevelopmentalStage()
                }
                
                agent_stats = {
                    'step': step,
                    'curiosity_level': agent.getCuriosityLevel(),
                    'novelty_level': agent.getNoveltyLevel(),
                    'neuromodulation_level': agent.getNeuromodulationLevel(),
                    'prediction_error': agent.getPredictionError()
                }
                
                world_stats = {
                    'step': step,
                    'simulation_time': world.getSimulationTime(),
                    'agent_x': world.getAgentBody().x,
                    'agent_y': world.getAgentBody().y,
                    'agent_energy': world.getAgentBody().energy,
                    'agent_health': world.getAgentBody().health
                }
                
                stats['brain_stats'].append(brain_stats)
                stats['agent_stats'].append(agent_stats)
                stats['world_stats'].append(world_stats)
                
                # Call callback if provided
                if callback:
                    callback({
                        'step': step,
                        'active_neurons': brain_stats['active_neuron_count'],
                        'firing_rate': brain_stats['average_firing_rate'],
                        'curiosity': agent_stats['curiosity_level'],
                        'novelty': agent_stats['novelty_level'],
                        'dev_stage': brain_stats['developmental_stage']
                    })
            
            # Calculate final statistics
            final_brain_stats = stats['brain_stats'][-1] if stats['brain_stats'] else {}
            final_agent_stats = stats['agent_stats'][-1] if stats['agent_stats'] else {}
            final_world_stats = stats['world_stats'][-1] if stats['world_stats'] else {}
            
            return SimulationStats(
                final_brain_stats, final_agent_stats, final_world_stats, steps
            )
            
        except Exception as e:
            raise SimulationError(f"Simulation failed: {str(e)}")

# Global functions for backward compatibility and convenience

def create_config(preset: Optional[str] = None, 
                 custom_config: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create a configuration with optional preset.
    
    Examples:
        >>> # Create default config
        >>> config = nlm.create_config()
        >>> 
        >>> # Create with preset
        >>> config = nlm.create_config(preset='learning')
        >>> 
        >>> # Create with custom settings
        >>> config = nlm.create_config(custom_config={'brain.neuron_count': 2000})
    """
    if not _has_c_bindings:
        raise NLMLibError("pynlm C++ bindings not available")
    
    config = pynlm.createDefaultConfig()
    
    if preset:
        preset_config = ConfigPresets.get_preset(preset)
        for key, value in preset_config.items():
            config.set(key, value)
    
    if custom_config:
        for key, value in custom_config.items():
            config.set(key, value)
    
    return config

def create_brain(size: str = 'medium', preset: Optional[str] = None,
                custom_config: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create a brain with specified size and preset.
    
    This is a convenience wrapper around BrainFactory.
    """
    factory = BrainFactory()
    return factory.create_brain(size, preset, custom_config)

def create_world(width: int = 20, height: int = 20,
                vision_width: int = 8, vision_height: int = 8,
                agent_start: Optional[tuple] = None) -> Any:
    """
    Create a simple world with specified dimensions.
    
    This is a convenience wrapper around WorldHelper.
    """
    helper = WorldHelper()
    return helper.create_world(width, height, vision_width, vision_height, agent_start)

def run_simulation(world: Any, brain: Any, agent: Any, steps: int,
                  timestep: float = 0.1, callback: Optional[Callable] = None) -> SimulationStats:
    """
    Run a complete simulation with agent, brain, and world.
    
    This is a convenience wrapper around SimulationRunner.
    """
    runner = SimulationRunner()
    return runner.run_simulation(world, brain, agent, steps, timestep, callback)

def save_config(config: Any, filepath: str) -> Result:
    """
    Save configuration to file.
    
    Examples:
        >>> result = nlm.save_config(config, 'my_config.json')
        >>> if result.success:
        ...     print("Configuration saved successfully")
        ... else:
        ...     print(f"Error saving config: {result.error}")
    """
    return Result.from_operation(lambda: config.saveToFile(filepath))

def load_config(filepath: str) -> Result:
    """
    Load configuration from file.
    
    Examples:
        >>> result = nlm.load_config('my_config.json')
        >>> if result.success:
        ...     config = result.data
        ...     print(f"Configuration loaded with {config.getKeys()} keys")
        ... else:
        ...     print(f"Error loading config: {result.error}")
    """
    return Result.from_operation(lambda: _load_config_from_file(filepath))

def _load_config_from_file(filepath: str) -> Any:
    """Internal function to load config from file."""
    if not _has_c_bindings:
        raise NLMLibError("pynlm C++ bindings not available")
    
    config = pynlm.createDefaultConfig()
    if not config.loadFromFile(filepath):
        raise ConfigError(f"Failed to load config from {filepath}")
    
    return config

# Initialize the module
if _has_c_bindings:
    # Add C++ bindings methods to the main module
    globals().update({
        'NeuronId': pynlm.NeuronId,
        'SynapseId': pynlm.SynapseId,
        'RegionId': pynlm.RegionId,
        'PopulationId': pynlm.PopulationId,
        'NeuronType': pynlm.NeuronType,
        'SynapseType': pynlm.SynapseType,
        'DevelopmentalStage': pynlm.DevelopmentalStage,
        'FiringState': pynlm.FiringState,
        'ActionType': pynlm.ActionType,
        'MotorCommand': pynlm.MotorCommand,
        'WorldObjectType': pynlm.WorldObjectType,
        'Config': pynlm.Config,
        'SensoryInput': pynlm.SensoryInput,
        'Vision': pynlm.Vision,
        'Audio': pynlm.Audio,
        'InternalSignals': pynlm.InternalSignals,
        'Action': pynlm.Action,
        'WorldObject': pynlm.WorldObject,
        'AgentBody': pynlm.AgentBody,
        'ActionResult': pynlm.ActionResult,
        'SensoryPercept': pynlm.SensoryPercept,
        'SimpleWorld': pynlm.SimpleWorld,
        'Brain': pynlm.Brain,
        'AgentBrain': pynlm.AgentBrain,
        'INVALID_NEURON_ID': pynlm.INVALID_NEURON_ID,
        'INVALID_SYNAPSE_ID': pynlm.INVALID_SYNAPSE_ID,
        'INVALID_REGION_ID': pynlm.INVALID_REGION_ID,
        'INVALID_POPULATION_ID': pynlm.INVALID_POPULATION_ID
    })

# Add version information
__version__ = "1.0.0"
__author__ = "NLM Development Team"
__description__ = "Pythonic interface to NLM (Neural Learning Machine)"

# Module exports
__all__ = [
    # Custom exceptions
    'NLMLibError', 'ConfigError', 'BrainError', 'WorldError', 'SimulationError',
    
    # Type wrappers
    'Result', 'SimulationStats',
    
    # Configuration presets
    'ConfigPresets',
    
    # Factory classes
    'BrainFactory', 'WorldHelper', 'SimulationRunner',
    
    # Global functions
    'create_config', 'create_brain', 'create_world', 'run_simulation',
    'save_config', 'load_config',
    
    # C++ bindings (if available)
    'pynlm', '_has_c_bindings'
]

# Example usage docstring
__doc_examples__ = """
# Example 1: Basic Usage
>>> import nlm
>>> 
>>> # Create a simple brain
>>> config = nlm.create_config(preset='quick_dev')
>>> brain = nlm.create_brain(size='medium')
>>> brain.initialize()
>>> 
>>> # Run simulation
>>> for i in range(100):
...     brain.step(i)
...     if i % 20 == 0:
...         print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
>>> 
# Example 2: Complete Agent Simulation
>>> import nlm
>>> 
>>> # Create all components
>>> config = nlm.create_config(preset='learning')
>>> brain = nlm.create_brain()
>>> brain.initialize()
>>> 
>>> world = nlm.create_world(width=20, height=20, agent_start=(10, 10))
>>> agent = nlm.create_agent_brain(brain)
>>> 
>>> # Run simulation with statistics
>>> stats = nlm.run_simulation(world, brain, agent, steps=1000)
>>> 
>>> print(f"Simulation completed with {stats.step_count} steps")
>>> print(f"Final firing rate: {stats.get_firing_rate():.3f}")
>>> print(f"Final curiosity: {stats.get_curiosity_level():.3f}")
>>> print(f"Development stage: {stats.get_development_stage()}")
>>> 
# Example 3: Error Handling
>>> import nlm
>>> 
>>> try:
...     config = nlm.create_config(preset='nonexistent')
... except nlm.ConfigError as e:
...     print(f"Config error: {e}")
... 
# Example 4: Advanced Usage with Callbacks
>>> import nlm
>>> 
>>> def simulation_callback(stats):
...     if stats['step'] % 100 == 0:
...         print(f"Step {stats['step']}: {stats['active_neurons']} neurons, "
...               f"curiosity={stats['curiosity']:.3f}")
>>> 
>>> config = nlm.create_config(preset='production')
>>> brain = nlm.create_brain()
>>> brain.initialize()
>>> 
>>> world = nlm.create_world(width=30, height=30)
>>> agent = nlm.create_agent_brain(brain)
>>> 
>>> # Run with callback for monitoring
>>> stats = nlm.run_simulation(world, brain, agent, steps=500,
...                           callback=simulation_callback)
"""

if __name__ == '__main__':
    print(__doc__)
    print("\nExamples are available in __doc_examples__")
    print(f"Version {__version__}")