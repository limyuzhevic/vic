#!/usr/bin/env python3
"""
Unified NLM Module

Complete Python interface for NLM with comprehensive functionality,
automatic error handling, and modern Python API design.
"""

import time
import logging
from typing import Dict, List, Optional, Union, Any, Callable, Iterator
from dataclasses import dataclass, field, asdict
from enum import Enum
from contextlib import contextmanager
import numpy as np
from pathlib import Path

# Import submodules
from pynlm import _TypeValidator, get_config, set_config, NLMConfig
from pynlm.simulation import Simulation, SimulationStatus, EventType
from pynlm.advanced import EnhancedBrainWrapper, AdvancedNLM

# Configure logging
logging.basicConfig(level=logging.WARNING)
logger = logging.getLogger(__name__)


# Public API exports
__all__ = [
    # Core types
    'NLMConfig',
    'ConfigManager',
    'SimulationStatus',
    'EventType',
    
    # Main classes
    'AdvancedNLM',
    'EnhancedBrainWrapper',
    'Simulation',
    
    # Convenience functions
    'create_brain',
    'create_config',
    'create_simulation',
    'run_experiment',
    'train_brain',
    'evaluate_brain',
    
    # Constants and utilities
    '__version__',
    'get_version',
    'setup_logging',
    'benchmark_performance',
]

__version__ = "2.0.0"
__author__ = "NLM Research Team"
__license__ = "MIT"


# Convenience functions
def get_version() -> str:
    """Get NLM Python module version."""
    return __version__


def setup_logging(level: int = logging.INFO, format_string: Optional[str] = None) -> None:
    """
    Setup NLM logging.
    
    Args:
        level: Log level
        format_string: Log format string
    """
    if format_string is None:
        format_string = (
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
    
    logging.basicConfig(
        level=level,
        format=format_string,
        datefmt='%Y-%m-%d %H:%M:%S'
    )


def create_config(config_dict: Optional[Dict[str, Any]] = None) -> NLMConfig:
    """
    Create NLM configuration.
    
    Args:
        config_dict: Configuration dictionary (optional)
        
    Returns:
        NLMConfig instance
    """
    if config_dict is None:
        return NLMConfig()
    return NLMConfig(**config_dict)


def create_brain(config: Optional[Union[Dict, NLMConfig]] = None) -> AdvancedNLM:
    """
    Create NLM brain instance.
    
    Args:
        config: Configuration (dict or NLMConfig, optional)
        
    Returns:
        AdvancedNLM instance
    """
    return AdvancedNLM(config)


def create_simulation(brain: AdvancedNLM, name: str, **kwargs) -> Simulation:
    """
    Create simulation.
    
    Args:
        brain: AdvancedNLM instance
        name: Simulation name
        **kwargs: Simulation configuration
        
    Returns:
        Simulation instance
    """
    return brain.create_simulation(name, **kwargs)


def run_experiment(brain: AdvancedNLM, experiment_name: str, 
                  steps: int, **kwargs) -> Dict[str, Any]:
    """
    Run experiment.
    
    Args:
        brain: AdvancedNLM instance
        experiment_name: Experiment name
        steps: Number of steps to run
        **kwargs: Experiment options
        
    Returns:
        Experiment results
    """
    return brain.run_experiment(experiment_name, steps, **kwargs)


def train_brain(brain: AdvancedNLM, num_episodes: int, 
                max_steps_per_episode: int, **kwargs) -> Dict[str, Any]:
    """
    Train brain.
    
    Args:
        brain: AdvancedNLM instance
        num_episodes: Number of training episodes
        max_steps_per_episode: Maximum steps per episode
        **kwargs: Training options
        
    Returns:
        Training results
    """
    return brain.train_brain(num_episodes, max_steps_per_episode, **kwargs)


def evaluate_brain(brain: AdvancedNLM, test_scenarios: List[Dict[str, Any]]) -> Dict[str, Any]:
    """
    Evaluate brain.
    
    Args:
        brain: AdvancedNLM instance
        test_scenarios: List of test scenarios
        
    Returns:
        Evaluation results
    """
    return brain.evaluate_brain(test_scenarios)


def benchmark_performance(brain: AdvancedNLM, test_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
    """
    Benchmark brain performance across different configurations.
    
    Args:
        brain: AdvancedNLM instance
        test_configs: List of test configurations
        
    Returns:
        Benchmark results
    """
    benchmark_results = []
    
    for i, config in enumerate(test_configs):
        config_name = config.get("name", f"Config_{i}")
        steps = config.get("steps", 100)
        
        logger.info(f"Benchmarking {config_name}")
        
        # Create brain with specific configuration
        test_brain = AdvancedNLM(config)
        
        # Run benchmark
        start_time = time.time()
        for step in range(steps):
            test_brain.brain.step(step)
        
        elapsed_time = time.time() - start_time
        
        # Collect metrics
        metrics = {
            "config_name": config_name,
            "steps_completed": steps,
            "elapsed_time": elapsed_time,
            "steps_per_second": steps / elapsed_time if elapsed_time > 0 else 0,
            "final_spike_count": test_brain.brain.getTotalSpikeCount(),
            "final_firing_rate": test_brain.brain.getAverageFiringRate(),
            "final_e_i_ratio": test_brain.brain.getExcitationInhibitionRatio(),
        }
        
        benchmark_results.append(metrics)
    
    # Calculate benchmark statistics
    avg_steps_per_second = np.mean([r["steps_per_second"] for r in benchmark_results])
    avg_final_spike_count = np.mean([r["final_spike_count"] for r in benchmark_results])
    avg_final_firing_rate = np.mean([r["final_firing_rate"] for r in benchmark_results])
    
    return {
        "total_configs_tested": len(test_configs),
        "average_performance": {
            "steps_per_second": avg_steps_per_second,
            "final_spike_count": avg_final_spike_count,
            "final_firing_rate": avg_final_firing_rate,
        },
        "benchmark_results": benchmark_results,
    }


@contextmanager
def nlm_session(config: Optional[Union[Dict, NLMConfig]] = None) -> Iterator[AdvancedNLM]:
    """
    Context manager for NLM session.
    
    Args:
        config: Configuration (dict or NLMConfig, optional)
        
    Yields:
        AdvancedNLM instance
    """
    brain = AdvancedNLM(config)
    
    try:
        yield brain
    finally:
        # Cleanup
        logger.info("NLM session completed")


# Default configuration presets
def get_default_config() -> NLMConfig:
    """Get default NLM configuration."""
    return NLMConfig()


def get_minimal_config() -> NLMConfig:
    """Get minimal NLM configuration for fast testing."""
    return NLMConfig(
        neuron_count=100,
        region_count=1,
        connection_probability=0.05,
        simulation_timestep=0.01,
        random_seed=42,
    )


def get_high_performance_config() -> NLMConfig:
    """Get high-performance NLM configuration."""
    return NLMConfig(
        neuron_count=5000,
        region_count=4,
        connection_probability=0.15,
        simulation_timestep=0.001,
        random_seed=None,
        stdp_enable=True,
        hebbian_enable=True,
        structural_enable=True,
        reward_modulation_enabled=True,
        curiosity_enabled=True,
        novelty_enabled=True,
        prediction_enable=True,
        development_enable=True,
    )


def get_research_config() -> NLMConfig:
    """Get research-oriented NLM configuration."""
    return NLMConfig(
        neuron_count=10000,
        region_count=8,
        connection_probability=0.2,
        simulation_timestep=0.0005,
        random_seed=None,
        stdp_enable=True,
        hebbian_enable=True,
        structural_enable=True,
        neuromod_dopamine_scale=2.0,
        neuromod_curiosity_enable=True,
        neuromod_novelty_enable=True,
        neuromod_prediction_error_scale=1.5,
        memory_working_capacity=500,
        memory_episodic_capacity=5000,
        prediction_horizon=0.2,
        developmental_stage_duration=500.0,
    )


# Example usage functions (for documentation)
def example_basic_usage():
    """
    Example: Basic NLM usage.
    
    This function demonstrates the basic usage of NLM.
    """
    brain = AdvancedNLM()
    
    # Run simulation
    results = brain.run_experiment("basic_test", 100)
    
    return results


def example_training():
    """
    Example: Training NLM brain.
    
    This function demonstrates training an NLM brain.
    """
    brain = AdvancedNLM()
    
    # Train brain
    training_results = brain.train_brain(
        num_episodes=50,
        max_steps_per_episode=200,
    )
    
    return training_results


def example_evaluation():
    """
    Example: Evaluating NLM brain.
    
    This function demonstrates evaluating an NLM brain.
    """
    brain = AdvancedNLM()
    
    # Define test scenarios
    test_scenarios = [
        {"name": "stability_test", "steps": 100},
        {"name": "learning_test", "steps": 200},
        {"name": "development_test", "steps": 150},
    ]
    
    # Evaluate
    evaluation_results = brain.evaluate_brain(test_scenarios)
    
    return evaluation_results


# Error handling and utilities
def safe_simulation(func: Callable) -> Callable:
    """
    Decorator for safe simulation execution.
    
    Args:
        func: Function to decorate
        
    Returns:
        Decorated function
    """
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            logger.error(f"Simulation error: {e}")
            raise
    
    return wrapper


def validate_config(config: Union[Dict, NLMConfig]) -> bool:
    """
    Validate configuration.
    
    Args:
        config: Configuration to validate
        
    Returns:
        True if valid, False otherwise
    """
    try:
        if isinstance(config, dict):
            config_obj = NLMConfig(**config)
        else:
            config_obj = config
        
        config_obj.validate()
        return True
    except Exception as e:
        logger.error(f"Configuration validation failed: {e}")
        return False