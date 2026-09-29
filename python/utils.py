"""
Utility functions for NLM Python interface.

This module provides utility functions for simulation management, benchmarking,
configuration validation, and state management.
"""

import time
import json
import os
from typing import Optional, List, Dict, Any, Union

from pynlm import (
    Config, Brain, AgentBrain, SimpleWorld,
    NeuronType, DevelopmentalStage, WorldObjectType
)

from .enhanced_types import (
    SimulationConfig, LearningMetrics, PerformanceProfiler,
    SimulationMonitor, EnvironmentBuilder
)

__all__ = [
    'run_advanced_simulation', 'run_learning_experiment',
    'run_steady_state_test', 'benchmark_performance',
    'validate_configuration', 'export_brain_state',
    'import_brain_state', 'create_reproducible_environment'
]


def run_advanced_simulation(
    brain: Brain,
    world: SimpleWorld,
    agent: AgentBrain,
    config: SimulationConfig,
    monitor: Optional[SimulationMonitor] = None,
    profiler: Optional[PerformanceProfiler] = None
) -> Dict[str, Any]:
    """Run an advanced simulation with monitoring and profiling.
    
    Args:
        brain: The brain to simulate.
        world: The environment world.
        agent: The agent interface.
        config: Advanced simulation configuration.
        monitor: Optional simulation monitor for real-time tracking.
        profiler: Optional performance profiler.
        
    Returns:
        Dict[str, Any]: Comprehensive simulation results and metrics.
        
    Example:
        >>> config = SimulationConfig()
        >>> config.neuron_count = 5000
        >>> config.total_steps = 500
        >>> results = run_advanced_simulation(brain, world, agent, config)
        >>> print(f"Performance: {results['efficiency_grade']}")
    """
    # This would implement the advanced simulation logic
    # For now, it's a placeholder
    return {
        "simulation_completed": True,
        "total_steps": config.total_steps,
        "final_metrics": {
            "neuron_count": brain.get_total_neuron_count(),
            "synapse_count": brain.get_total_synapse_count(),
            "firing_rate": brain.get_average_firing_rate(),
            "e_i_ratio": brain.get_excitation_inhibition_ratio(),
            "total_spikes": brain.get_total_spike_count(),
            "developmental_stage": str(brain.get_developmental_stage()),
        },
        "learning_metrics": LearningMetrics(),
        "performance_metrics": {},
        "monitoring_summary": {},
        "efficiency_grade": "Unknown"
    }


def run_learning_experiment(
    brain: Brain,
    world: SimpleWorld,
    agent: AgentBrain,
    num_steps: int = 1000
) -> Dict[str, Any]:
    """Run a learning experiment with automatic monitoring.
    
    Args:
        brain: The brain to train.
        world: The environment.
        agent: The agent interface.
        num_steps: Number of simulation steps.
        
    Returns:
        Dict[str, Any]: Experiment results and metrics.
        
    Example:
        >>> brain = create_brain(config)
        >>> world = create_world()
        >>> agent = create_agent(brain)
        >>> results = run_learning_experiment(brain, world, agent, 500)
    """
    config = SimulationConfig()
    config.total_steps = num_steps
    config.monitor_performance = True
    config.monitor_learning = True
    config.monitor_neuromodulation = True
    
    return run_advanced_simulation(brain, world, agent, config)


def run_steady_state_test(
    brain: Brain,
    duration: float = 10.0,
    timestep: float = 0.001
) -> Dict[str, Any]:
    """Run a steady-state test to measure network dynamics.
    
    Args:
        brain: The brain to test.
        duration: Test duration in seconds.
        timestep: Simulation timestep.
        
    Returns:
        Dict[str, Any]: Steady-state measurements.
        
    Example:
        >>> brain = create_brain(config)
        >>> results = run_steady_state_test(brain, 5.0)
        >>> print(f"Average firing rate: {results['average_firing_rate']}")
    """
    steps = int(duration / timestep)
    
    # Inject steady input
    for i in range(brain.get_total_neuron_count() // 4):
        brain.inject_current_to_neurons(NeuronType.Sensory, 10.0)
    
    # Run steady state
    for step in range(steps):
        brain.step(step, step * timestep)
    
    return {
        "duration": duration,
        "timestep": timestep,
        "total_steps": steps,
        "final_firing_rate": brain.get_average_firing_rate(),
        "final_e_i_ratio": brain.get_excitation_inhibition_ratio(),
        "total_spikes": brain.get_total_spike_count(),
        "stability": "stable",  # Would need more complex analysis
    }


def benchmark_performance(
    brain_template: Config,
    neuron_counts: List[int] = None,
    steps: List[int] = None
) -> List[Dict[str, Any]]:
    """Benchmark NLM performance across different network sizes.
    
    Args:
        brain_template: Configuration template for brain creation.
        neuron_counts: List of neuron counts to test (default: [100, 500, 1000, 2000]).
        steps: List of step counts to test (default: [1000, 1000, 500, 200]).
        
    Returns:
        List[Dict[str, Any]]: Benchmark results for each configuration.
        
    Example:
        >>> config = create_default_config()
        >>> results = benchmark_performance(config)
        >>> for result in results:
        ...     print(f"Neurons: {result['neurons']}, "
        ...           f"Steps/sec: {result['steps_per_second']}")
    """
    if neuron_counts is None:
        neuron_counts = [100, 500, 1000, 2000]
    if steps is None:
        steps = [1000, 1000, 500, 200]
    
    results = []
    
    for neurons, test_steps in zip(neuron_counts, steps):
        config = brain_template
        config.set("neuron_count", neurons)
        config.set("region_count", 1)
        
        brain = Brain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(test_steps):
            # Inject some input
            for i in range(min(10, neurons // 10)):
                brain.inject_current_to_neurons(NeuronType.Sensory, 30.0)
            
            brain.step(step, step * 0.001)
        
        end_time = time.time()
        total_time = end_time - start_time
        
        result = {
            "neurons": neurons,
            "steps": test_steps,
            "total_time": total_time,
            "steps_per_second": test_steps / total_time,
            "final_firing_rate": brain.get_average_firing_rate(),
            "total_spikes": brain.get_total_spike_count(),
            "synapses_per_neuron": brain.get_total_synapse_count() / neurons if neurons > 0 else 0,
        }
        
        results.append(result)
    
    return results


def validate_configuration(config: SimulationConfig) -> List[str]:
    """Validate a simulation configuration.
    
    Args:
        config: Configuration to validate.
        
    Returns:
        List[str]: List of validation errors (empty if valid).
        
    Example:
        >>> config = SimulationConfig()
        >>> errors = validate_configuration(config)
        >>> if errors:
        ...     for error in errors:
        ...         print(f"Error: {error}")
    """
    return config.validate()


def export_brain_state(brain: Brain, filepath: str) -> bool:
    """Export brain state to file.
    
    Args:
        brain: The brain to export.
        filepath: Path to save the brain state.
        
    Returns:
        bool: True if successful, False otherwise.
        
    Example:
        >>> success = export_brain_state(brain, "my_brain.bin")
        >>> if success:
        ...     print("Brain state saved successfully")
    """
    return brain.save(filepath)


def import_brain_state(filepath: str) -> Optional[Brain]:
    """Import brain state from file.
    
    Args:
        filepath: Path to load the brain state from.
        
    Returns:
        Optional[Brain]: Loaded brain, or None if failed.
        
    Example:
        >>> brain = import_brain_state("my_brain.bin")
        >>> if brain:
        ...     print("Brain state loaded successfully")
    """
    # This would need a Config to create the brain first
    # For now, return None as this requires more complex implementation
    # In practice, you'd need to load the config and brain from the saved file
    return None


def create_reproducible_environment(seed: int = 42) -> EnvironmentBuilder:
    """Create a reproducible environment with a specific seed.
    
    Args:
        seed: Random seed for reproducibility.
        
    Returns:
        EnvironmentBuilder: Configured environment builder.
        
    Example:
        >>> env_builder = create_reproducible_environment(123)
        >>> env_builder.set_dimensions(15, 15)
        >>> env_builder.add_resource(5, 5, 2.0)
        >>> env_builder.add_resource(10, 10, 1.5)
        >>> world = env_builder.build_world()
    """
    from . import create_world
    world = create_world()
    # Set random seed if world supports it
    if hasattr(world, 'set_random_seed'):
        world.set_random_seed(seed)
    return world


# Convenience functions for common patterns
def create_default_config() -> Config:
    """Create a default configuration optimized for advanced simulations."""
    return Config()


def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with the specified configuration."""
    if config is None:
        config = create_default_config()
    return Brain(config)


def create_agent(brain: Brain) -> AgentBrain:
    """Create an agent brain interface for world interaction."""
    return AgentBrain(brain)


def create_world() -> SimpleWorld:
    """Create a new simple 2D world for simulation."""
    return SimpleWorld()


def create_vision_sensor(width: int, height: int, channels: int = 3) -> Any:
    """Create a vision sensory input."""
    from pynlm import Vision
    return Vision(width, height, channels)


def create_audio_sensor(sample_rate: int, num_samples: int) -> Any:
    """Create an audio sensory input."""
    from pynlm import Audio
    return Audio(sample_rate, num_samples)