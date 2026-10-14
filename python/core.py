"""
pynlm - Enhanced Python bindings for NLM (Neural Learning Machine)

This module provides a comprehensive Python interface to the NLM C++ neural simulation framework,
featuring:
- High-level convenience APIs for common use cases
- Comprehensive error handling and validation
- Extensive documentation and examples
- Type hints and IDE support
- Simulation utilities and helpers

The underlying C++ bindings are accessed through pybind11, providing full access to
the power of the NLM neural simulation framework while maintaining a clean,
Pythonic API.
"""

import sys
import os
from typing import Dict, Any, Optional, List, Union, Callable
from pathlib import Path

# Import the actual pybind11 module
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Lazy import of the actual bindings to avoid import errors before compilation
try:
    from .python_bindings import pynlm_module
    _has_bindings = True
except ImportError:
    _has_bindings = False
    pynlm_module = None

class NLMBindingError(Exception):
    """Base exception for NLM binding errors."""
    pass

class ConfigurationError(NLMBindingError):
    """Error in configuration."""
    pass

class SimulationError(NLMBindingError):
    """Error during simulation."""
    pass

class ValidationError(NLMBindingError):
    """Validation error."""
    pass

def _ensure_bindings():
    """Ensure the pybind11 bindings are available."""
    if not _has_bindings:
        raise NLMBindingError(
            "NLM bindings not available. Please build the Python bindings first.\n"
            "Run: pip install scikit-build-core pybind11 && pip install -e ."
        )

def create_simulation(config_dict: Optional[Dict[str, Any]] = None):
    """
    Create a complete simulation setup with brain, world, and agent.
    
    This is a convenience function that creates all the necessary components
    for a complete simulation and returns them in a dictionary.
    
    Args:
        config_dict: Optional dictionary of configuration parameters.
    
    Returns:
        Dictionary containing 'brain', 'world', 'agent', and 'config'.
    
    Example:
        >>> sim = create_simulation({'brain.neuron_count': 500})
        >>> brain = sim['brain']
        >>> world = sim['world']
        >>> agent = sim['agent']
    """
    _ensure_bindings()
    
    # Create configuration
    config = pynlm_module.createDefaultConfig()
    
    if config_dict:
        for key, value in config_dict.items():
            try:
                config.set(key, value)
            except Exception as e:
                raise ConfigurationError(f"Failed to set configuration '{key}': {e}")
    
    # Create components
    brain = pynlm_module.createBrain(config)
    brain.initialize()
    
    world = pynlm_module.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm_module.createAgentBrain(brain)
    agent.initialize(world)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'config': config
    }

def run_agent_episode(steps: int = 100, enable_features: bool = True):
    """
    Run a complete agent simulation episode.
    
    This function creates a simulation and runs it through a complete episode,
    optionally enabling various brain features.
    
    Args:
        steps: Number of simulation steps to run.
        enable_features: Whether to enable learning features (reward, curiosity, etc.).
    
    Returns:
        Dictionary containing the simulation components and statistics.
    
    Example:
        >>> results = run_agent_episode(200, enable_features=True)
        >>> print(f"Final curiosity: {results['final_curiosity']}")
    """
    _ensure_bindings()
    
    sim = create_simulation()
    brain = sim['brain']
    world = sim['world']
    agent = sim['agent']
    
    # Enable learning features if requested
    if enable_features:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
    
    # Store initial stats
    initial_stats = {
        'neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'curiosity': agent.getCuriosityLevel(),
        'development_stage': brain.getDevelopmentalStage()
    }
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if enabled
        if enable_features:
            # Get reward from world if available
            internal_signals = percept.getInternal()
            reward = internal_signals[0] if internal_signals and len(internal_signals) > 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
    
    # Store final stats
    final_stats = {
        'neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'curiosity': agent.getCuriosityLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio()
    }
    
    return {
        'simulation': sim,
        'initial_stats': initial_stats,
        'final_stats': final_stats,
        'steps_run': steps
    }

def quick_test():
    """
    Quick test of basic NLM functionality.
    
    This function runs a minimal test to verify that the bindings are working
    correctly and provides basic output statistics.
    
    Returns:
        The brain object for further testing.
    
    Example:
        >>> brain = quick_test()
        >>> print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    """
    _ensure_bindings()
    
    # Create default brain
    brain = pynlm_module.createBrain(pynlm_module.createDefaultConfig())
    brain.initialize()
    
    # Run for 50 steps
    for i in range(50):
        brain.step(i)
    
    print(f"Quick test passed! Brain has {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Currently firing: {brain.getFiringNeuronCount()}")
    
    return brain

def benchmark_simulation(config_dict: Optional[Dict[str, Any]] = None, 
                        steps: int = 1000):
    """
    Run a benchmark simulation to measure performance.
    
    Args:
        config_dict: Optional configuration parameters.
        steps: Number of simulation steps to run.
    
    Returns:
        Dictionary containing benchmark results.
    
    Example:
        >>> results = benchmark_simulation({'brain.neuron_count': 1000}, 2000)
        >>> print(f"Performance: {results['steps_per_second']} steps/sec")
    """
    _ensure_bindings()
    
    import time
    
    sim = create_simulation(config_dict)
    brain = sim['brain']
    world = sim['world']
    agent = sim['agent']
    
    # Enable features for realistic benchmarking
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation periodically
        if step % 10 == 0:
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    results = {
        'steps': steps,
        'elapsed_seconds': elapsed,
        'steps_per_second': steps / elapsed if elapsed > 0 else 0,
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'final_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'config_used': config_dict
    }
    
    return results

def save_simulation_state(sim_dict: Dict[str, Any], filepath: str):
    """
    Save the state of a simulation to a file.
    
    Args:
        sim_dict: Simulation dictionary as returned by create_simulation().
        filepath: Path to save the state file.
    
    Example:
        >>> sim = create_simulation()
        >>> save_simulation_state(sim, 'my_simulation.bin')
    """
    _ensure_bindings()
    
    brain = sim_dict['brain']
    brain.save(filepath)

def load_simulation_state(filepath: str, config_dict: Optional[Dict[str, Any]] = None):
    """
    Load a simulation state from a file.
    
    Args:
        filepath: Path to the saved state file.
        config_dict: Optional configuration dictionary for the new brain.
    
    Returns:
        Dictionary containing the loaded brain, world, agent, and config.
    
    Example:
        >>> sim = load_simulation_state('my_simulation.bin', {'brain.neuron_count': 500})
    """
    _ensure_bindings()
    
    config = pynlm_module.createDefaultConfig()
    
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value)
    
    brain = pynlm_module.createBrain(config)
    brain.initialize()
    
    try:
        brain.load(filepath)
    except Exception as e:
        raise SimulationError(f"Failed to load simulation state from {filepath}: {e}")
    
    # Create minimal world and agent for compatibility
    world = pynlm_module.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm_module.createAgentBrain(brain)
    agent.initialize(world)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'config': config
    }
