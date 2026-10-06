"""
Enhanced Python helpers for NLM Python bindings

This module provides additional Python-specific functionality for the NLM Python bindings,
including convenience functions, validation, and enhanced error handling.
"""

import numpy as np
from typing import Dict, List, Any, Optional, Union, Tuple
import json
import os
from pathlib import Path
import warnings

from .pynlm import (
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio, InternalSignals,
    Action, ActionType, MotorCommand, WorldObject, AgentBody, SensoryPercept,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    NeuronId, SynapseId, RegionId, PopulationId, WorldObjectType,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
)

def load_config_from_env(prefix: str = "NLM_") -> Config:
    """
    Load configuration from environment variables with the given prefix.
    
    Args:
        prefix: Prefix for environment variables (default: "NLM_")
        
    Returns:
        Config object with values from environment variables
        
    Example:
        >>> os.environ["NLM_NEURON_COUNT"] = "500"
        >>> os.environ["NLM_CONNECTION_PROBABILITY"] = "0.2"
        >>> config = load_config_from_env()
        >>> config.get_int("neuron_count")
        500
    """
    config = Config()
    
    for key, value in os.environ.items():
        if key.startswith(prefix):
            config_key = key[len(prefix):].lower()
            
            try:
                # Try to convert to appropriate type
                if value.lower() in ("true", "false"):
                    config.set(config_key, value.lower() == "true")
                elif value.isdigit():
                    config.set(config_key, int(value))
                elif _is_float(value):
                    config.set(config_key, float(value))
                else:
                    config.set(config_key, value)
            except ValueError:
                warnings.warn(f"Could not parse environment variable {key} as {type}")
                continue
                
    return config

def _is_float(value: str) -> bool:
    """Check if string can be converted to float."""
    try:
        float(value)
        return True
    except ValueError:
        return False

def validate_config(config: Config, schema: Dict[str, Any]) -> bool:
    """
    Validate configuration against a schema.
    
    Args:
        config: Configuration to validate
        schema: Schema dictionary defining validation rules
            Keys are configuration keys, values are dicts with:
            - "type": "int", "float", "str", "bool", "list"
            - "min": minimum value (optional)
            - "max": maximum value (optional)
            - "required": whether key must exist (default: False)
            
    Returns:
        True if config is valid, False otherwise
        
    Raises:
        NLMError: if validation fails
        
    Example:
        >>> schema = {
        ...     "neuron_count": {"type": "int", "min": 10, "max": 10000},
        ...     "connection_probability": {"type": "float", "min": 0.0, "max": 1.0},
        ...     "enable_structural_plasticity": {"type": "bool", "required": True}
        ... }
        >>> validate_config(config, schema)
    """
    for key, rules in schema.items():
        required = rules.get("required", False)
        if required and not config.has(key):
            raise NLMError(f"Required configuration key missing: {key}")
            
        if not config.has(key):
            continue
            
        value = _get_config_value(config, key)
        
        # Type checking
        expected_type = rules.get("type")
        if expected_type and not _check_value_type(value, expected_type):
            raise NLMError(
                f"Configuration key {key} has wrong type. "
                f"Expected {expected_type}, got {type(value).__name__}"
            )
            
        # Range checking
        if "min" in rules and value < rules["min"]:
            raise NLMError(
                f"Configuration key {key} value {value} is less than minimum {rules['min']}"
            )
            
        if "max" in rules and value > rules["max"]:
            raise NLMError(
                f"Configuration key {key} value {value} exceeds maximum {rules['max']}"
            )
            
    return True

def _get_config_value(config: Config, key: str) -> Any:
    """Get configuration value as Python native type."""
    if config.has(key):
        # Try different getters in order
        for getter in [_get_int, _get_float, _get_bool, _get_str, _get_list]:
            try:
                return getter(config, key)
            except NLMError:
                continue
                
    raise NLMError(f"Configuration key not found: {key}")

def _get_int(config: Config, key: str) -> int:
    try:
        return config.get_int(key)
    except:
        raise NLMError(f"Configuration key {key} is not an integer")

def _get_float(config: Config, key: str) -> float:
    try:
        return config.get_float(key)
    except:
        raise NLMError(f"Configuration key {key} is not a float")

def _get_bool(config: Config, key: str) -> bool:
    try:
        return config.get_bool(key)
    except:
        raise NLMError(f"Configuration key {key} is not a boolean")

def _get_str(config: Config, key: str) -> str:
    try:
        return config.get_string(key)
    except:
        raise NLMError(f"Configuration key {key} is not a string")

def _get_list(config: Config, key: str) -> List:
    try:
        return config.get_list(key)
    except:
        raise NLMError(f"Configuration key {key} is not a list")

def _check_value_type(value: Any, expected_type: str) -> bool:
    """Check if value matches expected type."""
    type_map = {
        "int": int,
        "float": float,
        "str": str,
        "bool": bool,
        "list": list,
    }
    return isinstance(value, type_map.get(expected_type, type(None)))

def create_simulation_step_callback(callback_func):
    """
    Create a callback function for simulation steps.
    
    Args:
        callback_func: Function to call at each simulation step
            Called with: (step_number, world_state, brain_state)
            
    Returns:
        Callback function compatible with SimpleWorld's update
        
    Example:
        >>> def my_callback(step, world, brain):
        ...     print(f"Step {step}: Agent at {world.get_agent_position()}")
        >>> callback = create_simulation_step_callback(my_callback)
        >>> world.update(0.1, callback)
    """
    def callback_wrapper(step: int, world: SimpleWorld, brain: Brain):
        try:
            callback_func(step, world, brain)
        except Exception as e:
            warnings.warn(f"Simulation step callback error: {e}")
            
    return callback_wrapper

def run_episode(world: SimpleWorld, brain: Brain, steps: int, 
                callback: Optional[callable] = None) -> List[Action]:
    """
    Run a complete episode of simulation.
    
    Args:
        world: SimpleWorld instance
        brain: Brain instance
        steps: Number of simulation steps to run
        callback: Optional callback function for each step
        
    Returns:
        List of actions taken during the episode
        
    Example:
        >>> actions = run_episode(world, brain, 100)
        >>> print(f"Total actions: {len(actions)}")
    """
    actions = []
    
    for step in range(steps):
        # Get sensory input
        percept = world.get_sensoryPercept()
        
        # Process sensory input
        brain.injectSensoryInput(percept)
        
        # Produce action
        action = brain.produceAction()
        
        # Apply action to world
        action_result = world.applyMotorCommand(action.getType(), world.getSimulationTime())
        
        # Store action
        actions.append(action)
        
        # Update simulation
        world.update(0.1)
        
        # Call callback if provided
        if callback:
            callback(step, world, brain)
            
    return actions

def visualize_brain(brain: Brain, output_file: Optional[str] = None) -> Dict[str, Any]:
    """
    Generate visualization data for brain state.
    
    Args:
        brain: Brain instance to visualize
        output_file: Optional path to save visualization data as JSON
        
    Returns:
        Dictionary with visualization data
        
    Example:
        >>> viz_data = visualize_brain(brain)
        >>> print(f"Regions: {len(viz_data['regions'])}")
        >>> print(f"Total neurons: {viz_data['stats']['neuron_count']}")
    """
    viz_data = {
        "stats": {
            "neuron_count": brain.neuronCount(),
            "synapse_count": brain.synapseCount(),
            "active_neuron_count": brain.activeNeuronCount(),
            "firing_neuron_count": brain.firingNeuronCount(),
            "average_firing_rate": brain.averageFiringRate(),
            "excitation_inhibition_ratio": brain.excitatoryInhibitionRatio(),
            "total_spike_count": brain.totalSpikeCount(),
            "pending_spike_count": brain.pendingSpikeCount(),
            "region_count": brain.getRegionCount(),
        },
        "regions": [],
        "developmental_stage": brain.getDevelopmentalStage(),
    }
    
    # Add region information
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            region_data = {
                "id": region_id.index(),
                "name": region.getName(),
                "neuron_count": region.getTotalNeuronCount(),
                "synapse_count": region.getSynapseCount(),
                "population_count": region.getPopulationCount(),
                "average_weight": region.getAverageSynapticWeight(),
            }
            viz_data["regions"].append(region_data)
    
    # Save to file if requested
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(viz_data, f, indent=2)
            
    return viz_data

def compare_configs(config1: Config, config2: Config) -> Dict[str, Any]:
    """
    Compare two configurations and report differences.
    
    Args:
        config1: First configuration
        config2: Second configuration
        
    Returns:
        Dictionary with comparison results
        
    Example:
        >>> config1 = load_config_from_dict({"neuron_count": 1000, "lr": 0.01})
        >>> config2 = load_config_from_dict({"neuron_count": 1000, "lr": 0.1})
        >>> comparison = compare_configs(config1, config2)
        >>> print(f"Differences: {comparison['differences']}")
    """
    keys1 = set(config1.getKeys())
    keys2 = set(config2.getKeys())
    
    common_keys = keys1.intersection(keys2)
    only_in_1 = keys1 - keys2
    only_in_2 = keys2 - keys1
    
    differences = {}
    for key in common_keys:
        try:
            val1 = _get_config_value(config1, key)
            val2 = _get_config_value(config2, key)
            if val1 != val2:
                differences[key] = {"from": val1, "to": val2}
        except NLMError:
            # Different types or values
            differences[key] = {"from": "N/A", "to": "N/A"}
            
    return {
        "only_in_first": list(only_in_1),
        "only_in_second": list(only_in_2),
        "common_keys": list(common_keys),
        "differences": differences,
        "summary": f"Config 1 has {len(config1.getKeys())} keys, Config 2 has {len(config2.getKeys())} keys",
    }

# Configuration presets
def get_debug_config() -> Dict[str, Any]:
    """Get a configuration suitable for debugging."""
    return {
        "neuron_count": 100,
        "region_count": 2,
        "simulation_timestep": 0.01,
        "random_seed": 42,
        "working_memory_capacity": 20,
        "episodic_memory_max_episodes": 100,
        "enable_structural_plasticity": True,
        "enable_development": False,  # Speed up for debugging
        "stdp_ltp_weight": 0.05,
        "stdp_ltd_weight": 0.055,
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001,
    }

def get_performance_config() -> Dict[str, Any]:
    """Get a configuration optimized for performance."""
    return {
        "neuron_count": 5000,
        "region_count": 1,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "working_memory_capacity": 500,
        "episodic_memory_max_episodes": 5000,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
    }

def get_creative_config() -> Dict[str, Any]:
    """Get a configuration optimized for creative exploration."""
    return {
        "neuron_count": 2000,
        "region_count": 3,
        "simulation_timestep": 0.001,
        "random_seed": 12345,
        "working_memory_capacity": 200,
        "episodic_memory_max_episodes": 2000,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_reward_modulation": False,  # More exploration
        "enable_curiosity": True,
        "enable_novelty_detection": True,
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.018,
        "stdp_tau": 30.0,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002,
    }

def load_config_from_dict(config_dict: Dict[str, Any]) -> Config:
    """
    Load configuration from a dictionary.
    
    Args:
        config_dict: Dictionary with configuration values
        
    Returns:
        Config object
        
    Example:
        >>> config = load_config_from_dict({"neuron_count": 1000, "lr": 0.01})
    """
    config = Config()
    for key, value in config_dict.items():
        config[key] = value
    return config

def save_config_to_dict(config: Config) -> Dict[str, Any]:
    """
    Save configuration to a dictionary.
    
    Args:
        config: Configuration to save
        
    Returns:
        Dictionary with configuration values
        
    Example:
        >>> config_dict = save_config_to_dict(config)
    """
    config_dict = {}
    for key in config.getKeys():
        config_dict[key] = _get_config_value(config, key)
    return config_dict

# Type checking and validation utilities
class ConfigValidator:
    """Configuration validator with comprehensive checking."""
    
    def __init__(self, config: Config):
        self.config = config
        self.errors = []
        self.warnings = []
        
    def validate_types(self):
        """Validate configuration types."""
        # Basic type validation
        type_checks = {
            "neuron_count": int,
            "region_count": int,
            "simulation_timestep": float,
            "random_seed": int,
            "connection_probability": float,
        }
        
        for key, expected_type in type_checks.items():
            if self.config.has(key):
                try:
                    _get_config_value(self.config, key)
                except NLMError:
                    self.errors.append(f"Key '{key}' has invalid type for {expected_type.__name__}")
                    
        return len(self.errors) == 0
        
    def validate_ranges(self):
        """Validate configuration value ranges."""
        range_checks = {
            "neuron_count": {"min": 10, "max": 100000},
            "region_count": {"min": 1, "max": 1000},
            "simulation_timestep": {"min": 0.0001, "max": 1.0},
            "random_seed": {"min": 0, "max": 2**31 - 1},
            "connection_probability": {"min": 0.0, "max": 1.0},
        }
        
        for key, limits in range_checks.items():
            if self.config.has(key):
                try:
                    value = _get_config_value(self.config, key)
                    if value < limits["min"]:
                        self.errors.append(f"Key '{key}' value {value} < minimum {limits['min']}")
                    if value > limits["max"]:
                        self.errors.append(f"Key '{key}' value {value} > maximum {limits['max']}")
                except NLMError:
                    self.errors.append(f"Key '{key}' cannot be validated for range checking")
                    
        return len(self.errors) == 0
        
    def get_report(self) -> Dict[str, Any]:
        """Get validation report."""
        return {
            "config": self.config.summary(),
            "errors": self.errors,
            "warnings": self.warnings,
            "valid": len(self.errors) == 0,
        }
        
    def validate_all(self) -> bool:
        """Run all validation checks."""
        self.validate_types()
        self.validate_ranges()
        return len(self.errors) == 0

# Utility functions
_np_to_py_type = {
    np.int32: int,
    np.int64: int,
    np.float32: float,
    np.float64: float,
    np.bool_: bool,
}

def _convert_numpy_to_py(obj):
    """Convert numpy arrays to Python lists."""
    if isinstance(obj, np.ndarray):
        return obj.tolist()
    elif isinstance(obj, dict):
        return {k: _convert_numpy_to_py(v) for k, v in obj.items()}
    elif isinstance(obj, list):
        return [_convert_numpy_to_py(item) for item in obj]
    else:
        return obj

def export_for_web(obj, output_file: str):
    """
    Export object for web use.
    
    Args:
        obj: Python object (Config, Brain, etc.)
        output_file: Path to save exported data
        
    Example:
        >>> export_for_web(brain, "brain_data.json")
    """
    if isinstance(obj, Config):
        data = save_config_to_dict(obj)
    elif hasattr(obj, "__dict__"):
        # For other objects, use their attributes
        data = _convert_numpy_to_py(obj.__dict__)
    else:
        raise NLMError("Unsupported object type for export")
        
    with open(output_file, 'w') as f:
        json.dump(data, f, indent=2)
        
    return data