"""
pynlm - Enhanced Python Interface for NLM (Neural Learning Machine)

This module provides a user-friendly Python interface to the NLM brain simulation framework.
It wraps the C++ bindings to provide a more Pythonic API with convenience functions,
error handling, and high-level abstractions.

Usage examples:
    >>> import pynlm
    >>> # Create a simple brain
    >>> brain = pynlm.create_brain()
    >>> brain.initialize()
    >>> for i in range(100):
    ...     brain.step(i)
    >>> print(f"Total spikes: {brain.get_total_spikes()}")
    
    >>> # Create a complete agent-world simulation
    >>> brain = pynlm.create_brain()
    >>> world = pynlm.create_simple_world(width=20, height=20)
    >>> agent = pynlm.create_agent(brain, world)
    >>> agent.enable_learning(True)
    >>> 
    >>> for step in range(1000):
    ...     world.update(0.1)
    ...     percept = world.get_sensory_percept()
    ...     agent.process_sensory(percept)
    ...     brain.step(step)
    ...     action = agent.decode_action()
    ...     world.apply_action(action, world.get_simulation_time())
    ...     agent.apply_reward(world.get_reward())
"""

import warnings
import json
import os
from typing import Dict, List, Optional, Any, Union
from enum import Enum

# Try to import the C++ bindings
try:
    import pynlm as _pynlm_cpp
    _CPP_AVAILABLE = True
except ImportError as e:
    warnings.warn(f"C++ bindings not available: {e}. Limited functionality.")
    _CPP_AVAILABLE = False

# Global configuration
DEFAULT_CONFIG = {
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
}

# Configuration file templates
CONFIG_TEMPLATES = {
    "simple": {
        "name": "Simple Brain",
        "description": "Basic brain with minimal configuration",
        "config": {"neuron_count": 100, "connection_probability": 0.1}
    },
    "rich": {
        "name": "Rich Simulation",
        "description": "Brain with lots of neurons and strong plasticity",
        "config": {
            "neuron_count": 1000,
            "connection_probability": 0.3,
            "stdp_ltp_weight": 0.05,
            "stdp_ltd_weight": 0.03,
            "synaptogenesis_rate": 0.001,
            "enable_development": True
        }
    },
    "fast": {
        "name": "Fast Test",
        "description": "Small brain for quick testing",
        "config": {"neuron_count": 50, "connection_probability": 0.05}
    }
}


class _MissingCPPAccessor:
    """Provides default behavior when C++ bindings aren't available."""
    
    def __init__(self):
        self._validate_cpp_available()
    
    def _validate_cpp_available(self):
        if not _CPP_AVAILABLE:
            raise RuntimeError("C++ bindings not available. Please install pynlm with C++ support.")
    
    def __getattr__(self, name):
        return getattr(_pynlm_cpp, name)


# Create C++ access wrapper
if _CPP_AVAILABLE:
    _pynlm = _pynlm_cpp
else:
    _pynlm = _MissingCPPAccessor()


class Config:
    """Pythonic configuration wrapper for NLM brain settings.
    
    Provides a clean, Python interface to brain configuration with
    validation and helpful defaults.
    """
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None, name: str = ""):
        """Create a new configuration.
        
        Args:
            config_dict: Dictionary of configuration settings
            name: Optional name for this configuration
        """
        if config_dict is None:
            config_dict = {}
        
        self._config_dict = config_dict.copy()
        self._name = name
        self._validate_config()
    
    def _validate_config(self):
        """Validate configuration parameters."""
        if "neuron_count" in self._config_dict:
            if not isinstance(self._config_dict["neuron_count"], int) or self._config_dict["neuron_count"] <= 0:
                raise ValueError("neuron_count must be a positive integer")
        
        if "connection_probability" in self._config_dict:
            prob = self._config_dict["connection_probability"]
            if not isinstance(prob, (int, float)) or not 0 <= prob <= 1:
                raise ValueError("connection_probability must be between 0 and 1")
        
        if "simulation_timestep" in self._config_dict:
            timestep = self._config_dict["simulation_timestep"]
            if not isinstance(timestep, (int, float)) or timestep <= 0:
                raise ValueError("simulation_timestep must be positive")
    
    def set(self, key: str, value: Any):
        """Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        self._config_dict[key] = value
        self._validate_config()
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value or default
        """
        return self._config_dict.get(key, default)
    
    def has(self, key: str) -> bool:
        """Check if configuration has a key.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists
        """
        return key in self._config_dict
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return self._config_dict.copy()
    
    def update(self, other_dict: Dict[str, Any]):
        """Update configuration from another dictionary.
        
        Args:
            other_dict: Dictionary with configuration updates
        """
        self._config_dict.update(other_dict)
        self._validate_config()
    
    def __repr__(self) -> str:
        return f"Config(name='{self._name}', keys={list(self._config_dict.keys())})"


class _PythonicBrainWrapper:
    """Pythonic wrapper around the C++ Brain class.
    
    Provides a user-friendly interface to brain operations with
    sensible defaults and helpful error messages.
    """
    
    def __init__(self, cpp_brain):
        """Initialize with C++ brain instance.
        
        Args:
            cpp_brain: The C++ Brain instance
        """
        self._brain = cpp_brain
        self._ensure_initialized()
    
    def _ensure_initialized(self):
        """Ensure brain is initialized."""
        if not hasattr(self._brain, '_initialized') or not self._brain._initialized:
            warnings.warn("Brain may not be initialized. Call initialize() first.")
    
    def initialize(self) -> bool:
        """Initialize the brain.
        
        Returns:
            True if initialization succeeded
        """
        result = self._brain.initialize()
        if not result:
            raise RuntimeError("Brain initialization failed")
        return result
    
    def step(self, step_number: int, time: Optional[float] = None) -> bool:
        """Advance brain by one simulation step.
        
        Args:
            step_number: Current step number
            time: Current simulation time (uses step_number * 0.001 if not provided)
            
        Returns:
            True if step succeeded
        """
        if time is None:
            time = step_number * 0.001
        
        try:
            self._brain.step(step_number, time)
            return True
        except Exception as e:
            raise RuntimeError(f"Brain step failed: {e}")
    
    def get_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics.
        
        Returns:
            Dictionary of brain statistics
        """
        stats = {
            "total_neurons": self._brain.get_total_neuron_count(),
            "total_synapses": self._brain.get_total_synapse_count(),
            "active_neurons": self._brain.get_active_neuron_count(),
            "firing_neurons": self._brain.get_firing_neuron_count(),
            "total_spikes": self._brain.get_total_spike_count(),
            "average_firing_rate": self._brain.get_average_firing_rate(),
            "excitation_inhibition_ratio": self._brain.get_excitation_inhibition_ratio(),
            "developmental_stage": str(self._brain.get_developmental_stage()),
        }
        
        # Get brain status if available
        try:
            status = self._brain.log_status()
            if status:
                stats["status"] = status
        except:
            pass
        
        return stats
    
    def get_total_spikes(self) -> int:
        """Get total spike count.
        
        Returns:
            Total number of spikes generated
        """
        return self._brain.get_total_spike_count()
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons.
        
        Returns:
            Number of firing neurons
        """
        return self._brain.get_firing_neuron_count()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons.
        
        Returns:
            Average firing rate in Hz
        """
        return self._brain.get_average_firing_rate()
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file.
        
        Args:
            filepath: Path to save file
            
        Returns:
            True if save succeeded
        """
        try:
            result = self._brain.save(filepath)
            if not result:
                raise RuntimeError("Failed to save brain state")
            return True
        except Exception as e:
            raise RuntimeError(f"Failed to save brain state: {e}")
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file.
        
        Args:
            filepath: Path to load file
            
        Returns:
            True if load succeeded
        """
        try:
            result = self._brain.load(filepath)
            if not result:
                raise RuntimeError("Failed to load brain state")
            return True
        except Exception as e:
            raise RuntimeError(f"Failed to load brain state: {e}")
    
    def reset(self) -> bool:
        """Reset brain to initial state.
        
        Returns:
            True if reset succeeded
        """
        try:
            self._brain.reset()
            return True
        except Exception as e:
            raise RuntimeError(f"Failed to reset brain: {e}")
    
    def inject_current(self, neuron_id, current: float):
        """Inject current into a specific neuron.
        
        Args:
            neuron_id: Target neuron ID
            current: Current to inject
        """
        self._brain.inject_current(neuron_id, current)
    
    def produce_action(self):
        """Produce motor action based on brain activity.
        
        Returns:
            Motor action
        """
        return self._brain.produce_action()
    
    def __repr__(self) -> str:
        return f"Brain(stats={self.get_stats()})"


class _PythonicAgentWrapper:
    """Pythonic wrapper around the C++ AgentBrain class.
    
    Provides a high-level interface for agents that interact with the world.
    """
    
    def __init__(self, cpp_brain):
        """Initialize with C++ brain instance.
        
        Args:
            cpp_brain: The C++ Brain instance
        """
        self._agent = cpp_brain
        self._ensure_initialized()
    
    def _ensure_initialized(self):
        """Ensure agent is initialized."""
        if not hasattr(self._agent, '_initialized') or not self._agent._initialized:
            warnings.warn("Agent may not be initialized.")
    
    def initialize(self, world) -> bool:
        """Initialize agent with world.
        
        Args:
            world: The world instance
            
        Returns:
            True if initialization succeeded
        """
        self._agent.initialize(world)
        return True
    
    def process_sensory(self, percept) -> bool:
        """Process sensory percept.
        
        Args:
            percept: Sensory percept to process
            
        Returns:
            True if processing succeeded
        """
        self._agent.process_sensory_input(percept)
        return True
    
    def decode_action(self):
        """Decode brain motor activity into action.
        
        Returns:
            Motor command
        """
        return self._agent.decode_motor_command()
    
    def apply_reward(self, reward: float, predicted_reward: float = 0.0):
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward (default: 0)
        """
        self._agent.apply_reward_modulation(reward, predicted_reward)
    
    def update_development(self, timestep: float):
        """Update development system.
        
        Args:
            timestep: Time step for development update
        """
        self._agent.update_development(timestep)
    
    def enable_feature(self, feature: str, enable: bool):
        """Enable or disable agent features.
        
        Args:
            feature: Feature name (reward_modulation, structural_plasticity, development, curiosity)
            enable: Whether to enable or disable
        """
        feature_methods = {
            'reward_modulation': self._agent.enable_reward_modulation,
            'structural_plasticity': self._agent.enable_structural_plasticity,
            'development': self._agent.enable_development,
            'curiosity': self._agent.enable_curiosity
        }
        
        if feature in feature_methods:
            feature_methods[feature](enable)
        else:
            raise ValueError(f"Unknown feature: {feature}")
    
    def is_feature_enabled(self, feature: str) -> bool:
        """Check if feature is enabled.
        
        Args:
            feature: Feature name
            
        Returns:
            True if feature is enabled
        """
        feature_methods = {
            'reward_modulation': self._agent.is_reward_modulation_enabled,
            'structural_plasticity': self._agent.is_structural_plasticity_enabled,
            'development': self._agent.is_development_enabled,
            'curiosity': self._agent.is_curiosity_enabled
        }
        
        if feature in feature_methods:
            return feature_methods[feature]()
        else:
            raise ValueError(f"Unknown feature: {feature}")
    
    def get_development_stage(self):
        """Get current developmental stage.
        
        Returns:
            Developmental stage
        """
        return self._agent.get_developmental_stage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Neuromodulation level
        """
        return self._agent.get_neuromodulation_level()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level.
        
        Returns:
            Curiosity level
        """
        return self._agent.get_curiosity_level()
    
    def get_novelty_level(self) -> float:
        """Get novelty level.
        
        Returns:
            Novelty level
        """
        return self._agent.get_novelty_level()
    
    def get_prediction_error(self) -> float:
        """Get prediction error.
        
        Returns:
            Prediction error
        """
        return self._agent.get_prediction_error()
    
    def reset(self) -> bool:
        """Reset agent for new episode.
        
        Returns:
            True if reset succeeded
        """
        self._agent.reset()
        return True


class _PythonicWorldWrapper:
    """Pythonic wrapper around the C++ SimpleWorld class.
    
    Provides a high-level interface for world simulation.
    """
    
    def __init__(self, cpp_world):
        """Initialize with C++ world instance.
        
        Args:
            cpp_world: The C++ SimpleWorld instance
        """
        self._world = cpp_world
        self._ensure_initialized()
    
    def _ensure_initialized(self):
        """Ensure world is initialized."""
        pass
    
    def initialize(self, width: int = 10, height: int = 10, 
                   vision_width: int = 8, vision_height: int = 8):
        """Initialize world with dimensions.
        
        Args:
            width: World width
            height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        self._world.configure(width, height, vision_width, vision_height)
        self._world.reset()
    
    def update(self, timestep: float):
        """Update world state.
        
        Args:
            timestep: Time step for update
        """
        self._world.update(timestep)
    
    def get_sensory_percept(self):
        """Get sensory percept from world.
        
        Returns:
            Sensory percept
        """
        return self._world.get_sensory_percept()
    
    def apply_action(self, action, time: float):
        """Apply motor command to world.
        
        Args:
            action: Motor action to apply
            time: Current simulation time
        """
        self._world.apply_motor_command(action, time)
    
    def get_agent_body(self):
        """Get agent body state.
        
        Returns:
            Agent body state
        """
        return self._world.get_agent_body()
    
    def get_simulation_time(self) -> float:
        """Get current simulation time.
        
        Returns:
            Current simulation time
        """
        return self._world.get_simulation_time()
    
    def set_random_seed(self, seed: int):
        """Set random seed for reproducibility.
        
        Args:
            seed: Random seed
        """
        self._world.set_random_seed(seed)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid
        """
        return self._world.is_valid_position(x, y)


# Create convenience functions for common operations
def create_brain(config_dict: Optional[Dict[str, Any]] = None, name: str = "") -> _PythonicBrainWrapper:
    """Create a new brain with given configuration.
    
    Args:
        config_dict: Configuration dictionary (uses defaults if None)
        name: Optional name for the brain
        
    Returns:
        Brain instance
    """
    if config_dict is None:
        config_dict = DEFAULT_CONFIG.copy()
    
    cpp_config = _pynlm.createDefaultConfig()
    
    # Apply configuration
    for key, value in config_dict.items():
        cpp_config.set(key, value)
    
    cpp_brain = _pynlm.createBrain(cpp_config)
    return _PythonicBrainWrapper(cpp_brain)


def create_simple_world(width: int = 10, height: int = 10, 
                       vision_width: int = 8, vision_height: int = 8) -> _PythonicWorldWrapper:
    """Create a simple world for simulation.
    
    Args:
        width: World width
        height: World height
        vision_width: Agent vision width
        vision_height: Agent vision height
        
    Returns:
        World instance
    """
    cpp_world = _pynlm.createSimpleWorld()
    wrapper = _PythonicWorldWrapper(cpp_world)
    wrapper.initialize(width, height, vision_width, vision_height)
    return wrapper


def create_agent(brain, world) -> _PythonicAgentWrapper:
    """Create an agent that interfaces brain and world.
    
    Args:
        brain: Brain instance
        world: World instance
        
    Returns:
        Agent instance
    """
    cpp_agent = _pynlm.createAgentBrain(brain._brain)
    wrapper = _PythonicAgentWrapper(cpp_agent)
    wrapper.initialize(world._world)
    return wrapper


def load_config_template(template_name: str) -> Dict[str, Any]:
    """Load a predefined configuration template.
    
    Args:
        template_name: Template name ('simple', 'rich', 'fast')
        
    Returns:
        Configuration dictionary
    """
    if template_name not in CONFIG_TEMPLATES:
        available = list(CONFIG_TEMPLATES.keys())
        raise ValueError(f"Unknown template '{template_name}'. Available: {available}")
    
    return CONFIG_TEMPLATES[template_name]["config"].copy()


def save_config_to_file(config_dict: Dict[str, Any], filepath: str):
    """Save configuration to file (JSON format).
    
    Args:
        config_dict: Configuration dictionary
        filepath: Path to save file
    """
    with open(filepath, 'w') as f:
        json.dump(config_dict, f, indent=2)


def load_config_from_file(filepath: str) -> Dict[str, Any]:
    """Load configuration from file (JSON format).
    
    Args:
        filepath: Path to load file
        
    Returns:
        Configuration dictionary
    """
    with open(filepath, 'r') as f:
        return json.load(f)


def create_default_config() -> Config:
    """Create a default configuration.
    
    Returns:
        Default configuration instance
    """
    return Config(DEFAULT_CONFIG.copy(), "Default")


def create_test_agent_simulation(steps: int = 100, config_template: str = "fast") -> Dict[str, Any]:
    """Create a complete agent simulation for testing.
    
    Args:
        steps: Number of simulation steps
        config_template: Configuration template to use
        
    Returns:
        Simulation results dictionary
    """
    config_dict = load_config_template(config_template)
    brain = create_brain(config_dict)
    world = create_simple_world(20, 20, 16, 16)
    agent = create_agent(brain, world)
    
    # Enable learning features
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('curiosity', True)
    
    results = {
        'steps': steps,
        'config': config_dict,
        'brain_stats': [],
        'actions': [],
        'rewards': []
    }
    
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Simulate reward
        reward = 1.0 if action.getType() != _pynlm.MotorCommand.Wait else 0.0
        agent.apply_reward(reward)
        
        results['brain_stats'].append(brain.get_stats())
        results['actions'].append(action)
        results['rewards'].append(reward)
    
    return results


def benchmark_performance(config_dict: Dict[str, Any], steps: int = 1000) -> Dict[str, Any]:
    """Run performance benchmark.
    
    Args:
        config_dict: Brain configuration
        steps: Number of steps to benchmark
        
    Returns:
        Benchmark results
    """
    import time
    
    brain = create_brain(config_dict)
    brain.initialize()
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    stats = brain.get_stats()
    stats['benchmark'] = {
        'total_time': elapsed,
        'steps_per_second': steps / elapsed,
        'time_per_step_ms': (elapsed / steps) * 1000
    }
    
    return stats


def get_available_templates() -> List[str]:
    """Get list of available configuration templates.
    
    Returns:
        List of template names
    """
    return list(CONFIG_TEMPLATES.keys())


def get_template_info(template_name: str) -> Dict[str, Any]:
    """Get information about a configuration template.
    
    Args:
        template_name: Template name
        
    Returns:
        Template information
    """
    if template_name not in CONFIG_TEMPLATES:
        raise ValueError(f"Unknown template: {template_name}")
    
    return CONFIG_TEMPLATES[template_name].copy()


# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python interface for NLM brain simulation"


# Expose commonly used constants and types
__all__ = [
    'Config',
    'create_brain',
    'create_simple_world',
    'create_agent',
    'load_config_template',
    'save_config_to_file',
    'load_config_from_file',
    'create_default_config',
    'create_test_agent_simulation',
    'benchmark_performance',
    'get_available_templates',
    'get_template_info',
    'DEFAULT_CONFIG',
    'CONFIG_TEMPLATES',
    '__version__',
    '__author__',
    '__description__'
]