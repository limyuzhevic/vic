from __future__ import annotations

"""
NLM (Neural Learning Machine) Python Bindings
===============================================

A Python binding for the NLM C++ neural simulation framework.

This package provides access to:
- Virtual brain simulation with LIF neurons
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, reward)
- Cognitive functions (prediction, planning, attention)
- World simulation and interaction
- Agent perception and action

Key Features:
- Simple, high-level interface for getting started
- Advanced features for research and experimentation
- Comprehensive documentation and examples
- Visualization and monitoring tools

Usage Examples:

>>> import nlm
>>> brain = nlm.create_brain()
>>> brain.initialize()
>>> for step in range(100):
...     brain.step(step)
>>> print(f"Total spikes: {brain.get_total_spike_count()}")

>>> # Agent in world simulation
>>> brain = nlm.create_brain()
>>> world = nlm.create_simple_world()
>>> agent = nlm.create_agent(brain)
>>> agent.initialize(world)
>>> for step in range(50):
...     world.update(0.1)
...     percept = world.get_sensory_percept()
...     agent.process_sensory_input(percept)
...     brain.step(step)
...     action = agent.decode_motor_command()
...     world.apply_motor_command(action, world.get_simulation_time())

The module provides both low-level access to all NLM systems
and high-level convenience functions for common use cases.
"""

import warnings
import sys
import os
from typing import Optional, Dict, Any, List, Tuple

# Import core components
from .bindings import (
    # Config
    Config,
    create_default_config,
    
    # Brain components
    Brain,
    NeuronType,
    FiringState,
    DevelopmentalStage,
    
    # Sensory and motor
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    ActionType,
    MotorCommand,
    SensoryPercept,
    
    # Agent and world
    AgentBrain,
    AgentBody,
    SimpleWorld,
    WorldObject,
    WorldObjectType,
    
    # Memory and utilities
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    ActionResult,
)

# Set up module version
__version__ = "0.1.0"
__author__ = "NLM Authors"
__copyright__ = "MIT"

# Custom exceptions
class NLMBrainError(Exception):
    """Base exception for NLM brain errors."""
    pass
class NLMConfigError(NLMBrainError):
    """Exception for configuration errors."""
    pass
class NLMInitializationError(NLMBrainError):
    """Exception for initialization errors."""
    pass
class NLMMemoryError(NLMBrainError):
    """Exception for memory-related errors."""
    pass
class NLMInvalidActionError(NLMBrainError):
    """Exception for invalid action errors."""
    pass
class NLMSimulationError(NLMBrainError):
    """Exception for simulation errors."""
    pass

# Global constants
INVALID_NEURON_ID = NeuronId(-1)
INVALID_SYNAPSE_ID = SynapseId(-1)
INVALID_REGION_ID = RegionId(-1)
INVALID_POPULATION_ID = PopulationId(-1)

# Convenience functions and classes
def create_agent(
    brain: Brain,
    enable_reward_modulation: bool = True,
    enable_curiosity: bool = True,
    enable_development: bool = True,
    enable_structural_plasticity: bool = True,
) -> AgentBrain:
    """
    Convenience function to create an agent brain with default settings.
    
    Args:
        brain: The NLM brain to connect
        enable_reward_modulation: Enable reward-based learning
        enable_curiosity: Enable curiosity-driven exploration
        enable_development: Enable developmental processes
        enable_structural_plasticity: Enable structural learning
        
    Returns:
        Configured AgentBrain instance
    """
    agent = AgentBrain(brain)
    agent.enable_reward_modulation(enable_reward_modulation)
    agent.enable_curiosity(enable_curiosity)
    agent.enable_development(enable_development)
    agent.enable_structural_plasticity(enable_structural_plasticity)
    return agent
def run_episode(
    brain: Brain,
    world: SimpleWorld,
    agent: AgentBrain,
    max_steps: int = 100,
    auto_reset: bool = True,
) -> Dict[str, Any]:
    """
    Run a complete episode with agent in world.
    
    Args:
        brain: The neural brain
        world: The simulation world
        agent: The agent interface
        max_steps: Maximum number of simulation steps
        auto_reset: Reset brain and agent after episode
        
    Returns:
        Dictionary with episode statistics
    """
    agent.initialize(world)
    
    stats = {
        'steps': 0,
        'total_spikes': 0,
        'avg_firing_rate': 0.0,
        'total_neurons': 0,
        'active_neurons': 0,
        'final_reward': 0.0,
        'curiosity_level': 0.0,
        'novelty_level': 0.0,
        'developmental_stage': brain.get_developmental_stage(),
    }
    
    try:
        for step in range(max_steps):
            # World update
            world.update(0.1)
            
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process sensory input
            agent.process_sensory_input(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decode_motor_command()
            
            # Apply action
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Update stats
            stats['steps'] += 1
            stats['total_spikes'] += brain.get_total_spike_count()
            stats['active_neurons'] = brain.get_active_neuron_count()
            stats['total_neurons'] = brain.get_total_neuron_count()
            
            # Check for stop conditions
            if world.get_simulation_time() > 1000.0 or action == MotorCommand.Wait:
                break
                
    except Exception as e:
        raise NLMSimulationError(f"Episode failed: {str(e)}") from e
        
    finally:
        if auto_reset:
            brain.reset()
            agent.reset()
    
    # Calculate averages
    if stats['steps'] > 0:
        stats['avg_firing_rate'] = stats['total_spikes'] / stats['steps']
        
    return stats
def monitor_brain(
    brain: Brain,
    interval: int = 10,
    duration: int = 100,
    callback: Optional[callable] = None,
) -> List[Dict[str, Any]]:
    """
    Monitor brain activity over time.
    
    Args:
        brain: The brain to monitor
        interval: Monitoring interval in steps
        duration: Total monitoring duration
        callback: Optional callback function for each monitoring point
        
    Returns:
        List of monitoring data snapshots
    """
    if not brain:
        raise NLMInitializationError("Brain not initialized")
        
    monitor_data = []
    
    try:
        for step in range(duration):
            if step % interval == 0:
                snapshot = {
                    'step': step,
                    'total_spikes': brain.get_total_spike_count(),
                    'firing_rate': brain.get_average_firing_rate(),
                    'active_neurons': brain.get_active_neuron_count(),
                    'total_neurons': brain.get_total_neuron_count(),
                    'firing_neuron_count': brain.get_firing_neuron_count(),
                    'exc_inhibition_ratio': brain.get_excitation_inhibition_ratio(),
                    'developmental_stage': brain.get_developmental_stage(),
                    'step_time': step,
                }
                
                monitor_data.append(snapshot)
                
                if callback:
                    callback(snapshot)
                    
    except Exception as e:
        raise NLMSimulationError(f"Monitoring failed: {str(e)}") from e
        
    return monitor_data
def visualize_brain(
    brain: Brain,
    format: str = 'text',
    output_file: Optional[str] = None,
) -> Optional[str]:
    """
    Create a text or JSON representation of brain state.
    
    Args:
        brain: The brain to visualize
        format: Output format ('text', 'json', 'html')
        output_file: Optional file path to save output
        
    Returns:
        Visualized representation as string, or None if saved to file
    """
    if format.lower() == 'json':
        import json
        
        brain_data = {
            'total_neurons': brain.get_total_neuron_count(),
            'total_synapses': brain.get_total_synapse_count(),
            'active_neurons': brain.get_active_neuron_count(),
            'firing_neurons': brain.get_firing_neuron_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'exc_inhibition_ratio': brain.get_excitation_inhibition_ratio(),
            'total_spikes': brain.get_total_spike_count(),
            'developmental_stage': str(brain.get_developmental_stage()),
            'regions': [],
        }
        
        for region_id in brain.get_region_ids():
            region = brain.get_region(region_id)
            if region:
                brain_data['regions'].append({
                    'id': str(region_id),
                    'neuron_count': region.get_neuron_count(),
                    'population_count': region.get_population_count(),
                })
                
        result = json.dumps(brain_data, indent=2)
        
    elif format.lower() == 'html':
        html = f"""
        <!DOCTYPE html>
        <html>
        <head><title>NLM Brain Visualization</title></head>
        <body>
            <h1>NLM Brain State</h1>
            <p><strong>Total Neurons:</strong> {brain.get_total_neuron_count()}</p>
            <p><strong>Total Synapses:</strong> {brain.get_total_synapse_count()}</p>
            <p><strong>Active Neurons:</strong> {brain.get_active_neuron_count()}</p>
            <p><strong>Firing Neurons:</strong> {brain.get_firing_neuron_count()}</p>
            <p><strong>Average Firing Rate:</strong> {brain.get_average_firing_rate():.2f}</p>
            <p><strong>Excitation/Inhibition Ratio:</strong> {brain.get_excitation_inhibition_ratio():.3f}</p>
            <p><strong>Total Spikes:</strong> {brain.get_total_spike_count()}</p>
            <p><strong>Developmental Stage:</strong> {brain.get_developmental_stage()}</p>
        </body>
        </html>
        """
        result = html
        
    else:  # text format
        result = f"""
NLM Brain Visualization
=======================
Total Neurons: {brain.get_total_neuron_count()}
Total Synapses: {brain.get_total_synapse_count()}
Active Neurons: {brain.get_active_neuron_count()}
Firing Neurons: {brain.get_firing_neuron_count()}
Average Firing Rate: {brain.get_average_firing_rate():.2f}
Excitation/Inhibition Ratio: {brain.get_excitation_inhibition_ratio():.3f}
Total Spikes: {brain.get_total_spike_count()}
Developmental Stage: {brain.get_developmental_stage()}
Regions: {brain.get_region_count()}

"""
    
    if output_file:
        with open(output_file, 'w') as f:
            f.write(result)
        return None
        
    return result
def compare_brains(
    brain1: Brain,
    brain2: Brain,
    metrics: Optional[List[str]] = None,
) -> Dict[str, float]:
    """
    Compare two brains across specified metrics.
    
    Args:
        brain1: First brain to compare
        brain2: Second brain to compare
        metrics: List of metric names to compare (default: all)
        
    Returns:
        Dictionary with comparison results
    """
    if metrics is None:
        metrics = [
            'total_neurons',
            'total_synapses',
            'active_neurons',
            'firing_neurons',
            'average_firing_rate',
            'exc_inhibition_ratio',
            'total_spikes',
        ]
        
    comparison = {}
    
    for metric in metrics:
        try:
            if metric == 'total_neurons':
                comparison[metric] = {
                    'brain1': brain1.get_total_neuron_count(),
                    'brain2': brain2.get_total_neuron_count(),
                    'difference': brain1.get_total_neuron_count() - brain2.get_total_neuron_count(),
                    'percent_change': (
                        (brain1.get_total_neuron_count() - brain2.get_total_neuron_count()) / max(1, brain2.get_total_neuron_count()) * 100
                    )
                }
                
            elif metric == 'total_synapses':
                comparison[metric] = {
                    'brain1': brain1.get_total_synapse_count(),
                    'brain2': brain2.get_total_synapse_count(),
                    'difference': brain1.get_total_synapse_count() - brain2.get_total_synapse_count(),
                    'percent_change': (
                        (brain1.get_total_synapse_count() - brain2.get_total_synapse_count()) / max(1, brain2.get_total_synapse_count()) * 100
                    )
                }
                
            elif metric == 'active_neurons':
                comparison[metric] = {
                    'brain1': brain1.get_active_neuron_count(),
                    'brain2': brain2.get_active_neuron_count(),
                    'difference': brain1.get_active_neuron_count() - brain2.get_active_neuron_count(),
                    'percent_change': (
                        (brain1.get_active_neuron_count() - brain2.get_active_neuron_count()) / max(1, brain2.get_active_neuron_count()) * 100
                    )
                }
                
            elif metric == 'firing_neurons':
                comparison[metric] = {
                    'brain1': brain1.get_firing_neuron_count(),
                    'brain2': brain2.get_firing_neuron_count(),
                    'difference': brain1.get_firing_neuron_count() - brain2.get_firing_neuron_count(),
                    'percent_change': (
                        (brain1.get_firing_neuron_count() - brain2.get_firing_neuron_count()) / max(1, brain2.get_firing_neuron_count()) * 100
                    )
                }
                
            elif metric == 'average_firing_rate':
                comparison[metric] = {
                    'brain1': brain1.get_average_firing_rate(),
                    'brain2': brain2.get_average_firing_rate(),
                    'difference': brain1.get_average_firing_rate() - brain2.get_average_firing_rate(),
                    'percent_change': (
                        (brain1.get_average_firing_rate() - brain2.get_average_firing_rate()) / max(0.001, brain2.get_average_firing_rate()) * 100
                    )
                }
                
            elif metric == 'exc_inhibition_ratio':
                comparison[metric] = {
                    'brain1': brain1.get_excitation_inhibition_ratio(),
                    'brain2': brain2.get_excitation_inhibition_ratio(),
                    'difference': brain1.get_excitation_inhibition_ratio() - brain2.get_excitation_inhibition_ratio(),
                    'percent_change': (
                        (brain1.get_excitation_inhibition_ratio() - brain2.get_excitation_inhibition_ratio()) / max(0.001, brain2.get_excitation_inhibition_ratio()) * 100
                    )
                }
                
            elif metric == 'total_spikes':
                comparison[metric] = {
                    'brain1': brain1.get_total_spike_count(),
                    'brain2': brain2.get_total_spike_count(),
                    'difference': brain1.get_total_spike_count() - brain2.get_total_spike_count(),
                    'percent_change': (
                        (brain1.get_total_spike_count() - brain2.get_total_spike_count()) / max(1, brain2.get_total_spike_count()) * 100
                    )
                }
                
        except Exception as e:
            comparison[metric] = {'error': str(e)}
            
    return comparison
def get_version_info() -> Dict[str, Any]:
    """
    Get version and build information for NLM Python bindings.
    
    Returns:
        Dictionary with version and build information
    """
    import platform
    
    return {
        'package_version': __version__,
        'python_version': sys.version,
        'platform': platform.platform(),
        'python_implementation': platform.python_implementation(),
    }
def benchmark_brain(
    brain: Brain,
    duration: float = 1.0,
    timestep: float = 0.1,
) -> Dict[str, float]:
    """
    Benchmark brain performance.
    
    Args:
        brain: The brain to benchmark
        duration: Total benchmark duration in seconds
        timestep: Time step size
        
    Returns:
        Dictionary with performance metrics
    """
    import time
    
    start_time = time.time()
    steps_completed = 0
    
    try:
        for step in range(int(duration / timestep)):
            brain.step(step)
            steps_completed += 1
            
    except Exception as e:
        raise NLMSimulationError(f"Benchmark failed: {str(e)}") from e
        
    end_time = time.time()
    actual_duration = end_time - start_time
    
    return {
        'steps_completed': steps_completed,
        'duration_seconds': actual_duration,
        'steps_per_second': steps_completed / actual_duration if actual_duration > 0 else 0,
        'timestep': timestep,
    }

# Create a module-level function to expose all the above
def _get_all_public_names() -> List[str]:
    """Get all public names from this module for star imports."""
    return [
        'Config',
        'create_default_config',
        'create_brain',
        'create_simple_world',
        'create_agent_brain',
        'create_agent',
        'run_episode',
        'monitor_brain',
        'visualize_brain',
        'compare_brains',
        'get_version_info',
        'benchmark_brain',
        'NLMBrainError',
        'NLMConfigError',
        'NLMInitializationError',
        'NLMMemoryError',
        'NLMInvalidActionError',
        'NLMSimulationError',
        'INVALID_NEURON_ID',
        'INVALID_SYNAPSE_ID',
        'INVALID_REGION_ID',
        'INVALID_POPULATION_ID',
        'NeuronType',
        'FiringState',
        'DevelopmentalStage',
        'ActionType',
        'MotorCommand',
        'WorldObjectType',
    ]

# Mark all public names
__all__ = _get_all_public_names()

# Export convenience factory functions (will be added by bindings.cpp)
if 'create_brain' not in globals():
    create_brain = None
if 'create_simple_world' not in globals():
    create_simple_world = None
if 'create_agent_brain' not in globals():
    create_agent_brain = None

print(f"NLM Python bindings version {__version__} loaded successfully.")
print("Use 'help(nlm)' for detailed documentation or 'nlm.get_version_info()' for system info.")
