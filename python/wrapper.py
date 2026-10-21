# This file provides Pythonic wrappers and advanced features for the NLM Python bindings
# It creates a more intuitive Python API while maintaining access to all C++ functionality

import pybind11
import numpy as np
from typing import List, Dict, Optional, Union, Any, Callable
import time
import json
from contextlib import contextmanager

# Import the core bindings
import pynlm

# Re-export core classes for convenience
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Main classes
    'Brain', 'Config', 'SimpleWorld', 'AgentBrain', 'Action',
    'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'AgentBody', 'ActionResult', 'WorldObject',
    
    # Advanced classes
    'SmartBrain', 'ExperimentRunner', 'PatternBuilder',
    'ConfigBuilder', 'VisualDebugger', 'PerformanceProfiler',
    
    # Helper functions
    'run_simulation', 'create_brain_from_pattern', 
    'benchmark_learning', 'validate_connection',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID'
]

# Advanced configuration builders
class ConfigBuilder:
    """Advanced configuration builder for creating complex brain configurations."""
    
    @staticmethod
    def create_learning_brain(neuron_count: int = 1000, 
                            learning_rate: float = 0.01,
                            enable_curiosity: bool = True,
                            enable_reward_modulation: bool = True,
                            enable_development: bool = True) -> pynlm.Config:
        """Create a configuration optimized for learning."""
        config = pynlm.createDefaultConfig()
        
        # Neural configuration
        config.set("neuron_count", neuron_count)
        config.set("connection_probability", 0.15)
        
        # Plasticity configuration
        config.set("stdp_ltp_weight", 0.02 * learning_rate)
        config.set("stdp_ltd_weight", 0.015 * learning_rate)
        config.set("hebbian_learning_rate", 0.01 * learning_rate)
        
        # Neuromodulation configuration
        config.set("dopamine_sensitivity", 0.8 if enable_reward_modulation else 0.0)
        config.set("curiosity_gain", 0.5 if enable_curiosity else 0.0)
        config.set("novelty_threshold", 0.3)
        
        # Development configuration
        config.set("enable_development", 1 if enable_development else 0)
        config.set("critical_period_end", 0.6)
        config.set("plasticity_decay_rate", 0.01)
        
        # Memory configuration
        config.set("working_memory_capacity", 100)
        config.set("episodic_memory_capacity", 1000)
        config.set("associative_memory_decay", 0.95)
        
        # Simulation configuration
        config.set("simulation_timestep", 0.001)
        config.set("max_steps", 10000)
        
        return config
    
    @staticmethod
    def create_explosive_brain(neuron_count: int = 500,
                              initial_excitability: float = 2.0,
                              connection_density: float = 0.25) -> pynlm.Config:
        """Create a configuration for explosive, high-excitability networks."""
        config = pynlm.createDefaultConfig()
        
        config.set("neuron_count", neuron_count)
        config.set("connection_probability", connection_density)
        config.set("initial_excitability", initial_excitability)
        config.set("threshold", -40.0)  # Lower threshold for easier firing
        config.set("resting_potential", -65.0)
        
        # Strong excitatory connections
        config.set("stdp_ltp_weight", 0.05)
        config.set("stdp_ltd_weight", 0.02)
        config.set("hebbian_learning_rate", 0.1)
        
        # Fast dynamics
        config.set("simulation_timestep", 0.0005)
        config.set("refractory_period", 2)
        
        return config
    
    @staticmethod
    def create_stable_brain(neuron_count: int = 2000,
                          long_term_stability: float = 0.9,
                          careful_plasticity: bool = True) -> pynlm.Config:
        """Create a configuration for stable, long-term memory networks."""
        config = pynlm.createDefaultConfig()
        
        config.set("neuron_count", neuron_count)
        config.set("connection_probability", 0.08)  # Sparse connections
        config.set("stdp_ltp_weight", 0.01)
        config.set("stdp_ltd_weight", 0.008)
        config.set("hebbian_learning_rate", 0.005)
        
        if careful_plasticity:
            config.set("plasticity_window", 50)
            config.set("weight_change_threshold", 0.005)
        
        # Strong memory consolidation
        config.set("episodic_memory_capacity", 5000)
        config.set("associative_memory_decay", 0.99)
        config.set("replay_frequency", 100)
        
        # Stable development
        config.set("enable_development", 1)
        config.set("plasticity_decay_rate", 0.001)
        config.set("maturation_rate", 0.3)
        
        return config
class PatternBuilder:
    """Builder for creating neural connectivity patterns."""
    
    @staticmethod
    def create_feedforward_pattern(brain: pynlm.Brain,
                                 input_region_id: pynlm.RegionId,
                                 hidden_region_ids: List[pynlm.RegionId],
                                 output_region_id: pynlm.RegionId,
                                 input_size: int, hidden_size: int, output_size: int) -> None:
        """Create a feedforward neural network pattern."""
        # Add hidden regions if they don't exist
        for region_id in hidden_region_ids:
            if not brain.getRegion(region_id):
                brain.addRegion()
        
        # Connect input to first hidden layer
        for i in range(input_size):
            input_neuron_id = pynlm.NeuronId(i)
            for j, region_id in enumerate(hidden_region_ids):
                # Find neurons in hidden region
                region = brain.getRegion(region_id)
                if region:
                    for neuron in region.getAllNeurons():
                        if j == 0 and neuron.getId().value < hidden_size:
                            # Create connection with appropriate weight
                            synapse_weight = 0.5 if j == 0 else 0.3
                            # Note: Direct synapse manipulation requires internal access
                            # This is a placeholder for the pattern
                            pass
    
    @staticmethod
    def create_recurrent_pattern(brain: pynlm.Brain,
                               region_id: pynlm.RegionId,
                               density: float = 0.1) -> None:
        """Create a recurrent connection pattern within a region."""
        region = brain.getRegion(region_id)
        if not region:
            return
        
        neurons = region.getAllNeurons()
        num_neurons = len(neurons)
        
        # Create recurrent connections
        for i, neuron in enumerate(neurons):
            for j, target in enumerate(neurons):
                if i != j and np.random.random() < density:
                    # Create recurrent connection
                    # Weight depends on distance in the sequence
                    distance = abs(i - j)
                    weight = 0.1 / (distance + 1)
                    # Note: This would require direct access to synapse creation
                    # which is not currently exposed in the API
    
    @staticmethod
    def create_hierarchical_pattern(brain: pynlm.Brain,
                                  levels: List[int],
                                  connection_probabilities: List[float]) -> List[pynlm.RegionId]:
        """Create a hierarchical multi-layer neural network."""
        if len(levels) < 2 or len(connection_probabilities) != len(levels) - 1:
            raise ValueError("Invalid pattern parameters")
        
        region_ids = []
        
        # Create regions for each level
        for i, size in enumerate(levels):
            region_id = brain.addRegion(f"level_{i}")
            region_ids.append(region_id)
            
            # Add neurons to region (placeholder - actual neuron creation not exposed)
        
        # Connect layers
        for layer_idx in range(len(levels) - 1):
            prob = connection_probabilities[layer_idx]
            from_size = levels[layer_idx]
            to_size = levels[layer_idx + 1]
            
            # Create connections from layer i to layer i+1
            # This would require direct access to synapse management
        
        return region_ids
class SmartBrain:
    """Enhanced brain wrapper with smart features and utilities."""
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None):
        self._brain = brain
        self._world = world
        self._initialized = False
    
    def initialize(self, world: Optional[pynlm.SimpleWorld] = None) -> 'SmartBrain':
        """Initialize the brain with world and return self for chaining."""
        if world is not None:
            self._world = world
        if self._world:
            self._brain.initialize()
            self._initialized = True
        return self
    
    def step(self, steps: int = 1, **kwargs) -> 'SmartBrain':
        """Take multiple steps and return self for chaining."""
        if not self._initialized:
            raise RuntimeError("Brain must be initialized before stepping")
        
        for step in range(steps):
            step_num = kwargs.get('step_number', step)
            self._brain.step(step_num)
        
        return self
    
    def run_episode(self, max_steps: int, 
                   render: bool = False,
                   record_metrics: bool = True) -> Dict[str, Any]:
        """Run a complete episode and return metrics."""
        if not self._world:
            raise RuntimeError("SmartBrain requires a world for episode running")
        
        metrics = {
            'spikes': [],
            'firing_rates': [],
            'total_spikes': 0,
            'max_firing_rate': 0,
            'steps_completed': 0,
            'agent_position': None,
            'actions_taken': []
        }
        
        for step in range(max_steps):
            # Update world
            self._world.update(step * 0.1)
            
            # Get sensory input
            percept = self._world.getSensoryPercept()
            
            # Process input if agent brain exists
            # This would require an AgentBrain wrapper
            # brain.step(step)
            
            # Get action
            # action = agent.decodeMotorCommand()
            # self._world.applyMotorCommand(action, self._world.getSimulationTime())
            
            # Record metrics
            metrics['spikes'].append(self._brain.getTotalSpikeCount())
            metrics['firing_rates'].append(self._brain.getAverageFiringRate())
            metrics['total_spikes'] = self._brain.getTotalSpikeCount()
            metrics['max_firing_rate'] = max(metrics['firing_rates'])
            metrics['steps_completed'] = step + 1
            
            if render:
                # Render visualization (placeholder)
                pass
        
        return metrics
    
    def find_active_neurons(self, threshold: float = 0.5) -> List[pynlm.NeuronId]:
        """Find neurons that are currently active above threshold."""
        # This would require access to internal neuron states
        # which are not directly exposed
        active_ids = []
        
        # Get all regions and search for active neurons
        region_ids = self._brain.getRegionIds()
        for region_id in region_ids:
            region = self._brain.getRegion(region_id)
            if region:
                for neuron in region.getAllNeurons():
                    if neuron.isFiring():
                        active_ids.append(neuron.getId())
        
        return active_ids
    
    def get_network_statistics(self) -> Dict[str, Any]:
        """Get comprehensive network statistics."""
        return {
            'region_count': self._brain.getRegionCount(),
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'excitatory_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'developmental_stage': self._brain.getDevelopmentalStage().value,
            'config_summary': self._brain.getConfig().summary()
        }
    
    def enable_all_features(self) -> 'SmartBrain':
        """Enable all available brain features for maximum learning."""
        # Note: This would require access to AgentBrain methods
        # if this SmartBrain wraps an AgentBrain
        return self
    
    def disable_all_features(self) -> 'SmartBrain':
        """Disable all advanced features to return to basic mode."""
        return self
class VisualDebugger:
    """Visualization and debugging tools for brain simulation."""
    
    @staticmethod
    def plot_neural_activity(brain: pynlm.Brain, 
                           duration: int = 100,
                           save_path: Optional[str] = None) -> str:
        """Create a visualization of neural activity over time."""
        # This would integrate with matplotlib or other plotting libraries
        # For now, return a JSON representation of the activity
        activity_data = {
            'steps': list(range(duration)),
            'total_spikes': [],
            'firing_rates': [],
            'active_neurons': []
        }
        
        # Placeholder for actual simulation
        for step in range(duration):
            activity_data['total_spikes'].append(brain.getTotalSpikeCount())
            activity_data['firing_rates'].append(brain.getAverageFiringRate())
        
        if save_path:
            with open(save_path, 'w') as f:
                json.dump(activity_data, f, indent=2)
        
        return json.dumps(activity_data, indent=2)
    
    @staticmethod
    def analyze_connectivity(brain: pynlm.Brain) -> Dict[str, Any]:
        """Analyze the connectivity patterns in the brain."""
        regions = brain.getRegions()
        
        analysis = {
            'region_count': len(regions),
            'region_details': [],
            'connectivity_stats': {
                'total_synapses': brain.getTotalSynapseCount(),
                'neurons_per_region': [],
                'synapses_per_region': []
            }
        }
        
        for region in regions:
            neuron_count = 0
            synapse_count = 0
            
            # Placeholder for actual region analysis
            # neurons = region.getAllNeurons()
            # neuron_count = len(neurons)
            # for neuron in neurons:
            #     synapse_count += len(neuron.getIncomingSynapses())
            
            analysis['region_details'].append({
                'name': region.getName(),
                'neuron_count': neuron_count,
                'synapse_count': synapse_count
            })
            
            analysis['connectivity_stats']['neurons_per_region'].append(neuron_count)
            analysis['connectivity_stats']['synapses_per_region'].append(synapse_count)
        
        return analysis
class ExperimentRunner:
    """Tools for running and comparing different experiments."""
    
    @staticmethod
    def run_multiple_configurations(configs: List[pynlm.Config], 
                                   brain_factory: Callable[[pynlm.Config], pynlm.Brain],
                                   steps: int = 100) -> List[Dict[str, Any]]:
        """Run the same experiment with multiple configurations."""
        results = []
        
        for i, config in enumerate(configs):
            brain = brain_factory(config)
            brain.initialize()
            
            metrics = {
                'config_index': i,
                'config_summary': config.summary(),
                'final_spikes': 0,
                'final_firing_rate': 0,
                'steps_completed': 0
            }
            
            for step in range(steps):
                brain.step(step)
                metrics['final_spikes'] = brain.getTotalSpikeCount()
                metrics['final_firing_rate'] = brain.getAverageFiringRate()
                metrics['steps_completed'] = step + 1
            
            results.append(metrics)
            
            # Clean up
            brain.reset()
        
        return results
    
    @staticmethod
    def compare_performance(results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare performance across multiple experiment runs."""
        if not results:
            return {}
        
        comparison = {
            'config_count': len(results),
            'spike_statistics': {
                'mean': np.mean([r['final_spikes'] for r in results]),
                'std': np.std([r['final_spikes'] for r in results]),
                'min': min(r['final_spikes'] for r in results),
                'max': max(r['final_spikes'] for r in results)
            },
            'firing_rate_statistics': {
                'mean': np.mean([r['final_firing_rate'] for r in results]),
                'std': np.std([r['final_firing_rate'] for r in results]),
                'min': min(r['final_firing_rate'] for r in results),
                'max': max(r['final_firing_rate'] for r in results)
            }
        }
        
        return comparison
class PerformanceProfiler:
    """Performance profiling and benchmarking tools."""
    
    @staticmethod
    def benchmark_brain_creation(brain_factory: Callable[[], pynlm.Brain],
                                iterations: int = 100) -> Dict[str, float]:
        """Benchmark brain creation performance."""
        times = []
        
        for _ in range(iterations):
            start_time = time.time()
            brain = brain_factory()
            end_time = time.time()
            
            times.append(end_time - start_time)
        
        return {
            'mean_time_ms': np.mean(times) * 1000,
            'std_time_ms': np.std(times) * 1000,
            'min_time_ms': np.min(times) * 1000,
            'max_time_ms': np.max(times) * 1000,
            'median_time_ms': np.median(times) * 1000
        }
    
    @staticmethod
    def benchmark_simulation(brain: pynlm.Brain,
                           steps: int = 1000,
                           iterations: int = 10) -> Dict[str, Any]:
        """Benchmark simulation performance."""
        times = []
        
        brain.reset()
        brain.initialize()
        
        for _ in range(iterations):
            start_time = time.time()
            
            for step in range(steps):
                brain.step(step)
            
            end_time = time.time()
            times.append(end_time - start_time)
        
        return {
            'mean_time_per_step_ms': np.mean(times) * 1000 / steps,
            'std_time_per_step_ms': np.std(times) * 1000 / steps,
            'mean_steps_per_second': np.mean(steps / np.array(times)),
            'total_time_ms': np.mean(times) * 1000
        }
# Convenience functions
def run_simulation(brain: pynlm.Brain, 
                  world: Optional[pynlm.SimpleWorld] = None,
                  steps: int = 100) -> Dict[str, Any]:
    """Simple wrapper for running a simulation with a brain."""
    brain.initialize()
    
    metrics = {
        'total_spikes': [],
        'firing_rates': [],
        'steps_completed': 0
    }
    
    for step in range(steps):
        brain.step(step)
        metrics['total_spikes'].append(brain.getTotalSpikeCount())
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['steps_completed'] = step + 1
    
    return metrics

def create_brain_from_pattern(pattern_name: str, **kwargs) -> pynlm.Brain:
    """Create a brain using a predefined pattern."""
    config = pynlm.createDefaultConfig()
    
    patterns = {
        'learning': ConfigBuilder.create_learning_brain(**kwargs),
        'explosive': ConfigBuilder.create_explosive_brain(**kwargs),
        'stable': ConfigBuilder.create_stable_brain(**kwargs)
    }
    
    if pattern_name not in patterns:
        raise ValueError(f"Unknown pattern: {pattern_name}")
    
    config = patterns[pattern_name]
    return pynlm.createBrain(config)

def benchmark_learning(brain: pynlm.Brain,
                       pattern_duration: int = 1000,
                       analysis_window: int = 100) -> Dict[str, Any]:
    """Benchmark learning performance over a pattern."""
    brain.initialize()
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    
    # Apply pattern (simplified)
    for step in range(pattern_duration):
        brain.step(step)
    
    # Analyze learning
    final_spikes = brain.getTotalSpikeCount()
    spike_increase = final_spikes - initial_spikes
    
    # Calculate learning metrics
    metrics = {
        'spike_increase': spike_increase,
        'learning_efficiency': spike_increase / pattern_duration,
        'final_firing_rate': brain.getAverageFiringRate(),
        'network_stability': abs(spike_increase) / pattern_duration,
        'convergence_step': pattern_duration if spike_increase > 0 else -1
    }
    
    return metrics

def validate_connection(neuron1_id: pynlm.NeuronId, 
                       neuron2_id: pynlm.NeuronId,
                       brain: pynlm.Brain) -> bool:
    """Validate that a connection exists between two neurons."""
    # This would require access to the brain's synapse information
    # which is not directly exposed in the current API
    return False  # Placeholder
# Add module-level attributes
INVALID_NEURON_ID = pynlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = pynlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = pynlm.INVALID_REGION_ID
INVALID_POPULATION_ID = pynlm.INVALID_POPULATION_ID

# Set up the Python module
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Advanced Python interface for NLM neural simulation"

# Example usage (can be accessed via help())
EXAMPLES = """
Advanced NLM Usage Examples
===========================

1. Creating different types of brains:
>>> from pynlm_advanced import ConfigBuilder, SmartBrain
>>> learning_brain = SmartBrain(pynlm.createBrain(ConfigBuilder.create_learning_brain(neuron_count=500)))

2. Running an experiment:
>>> metrics = learning_brain.run_episode(1000, record_metrics=True)
>>> print(f"Final firing rate: {metrics['max_firing_rate']:.2f} Hz")

3. Analyzing network properties:
>>> stats = learning_brain.get_network_statistics()
>>> print(f"Network has {stats['total_neurons']} neurons")

4. Comparing configurations:
>>> configs = [ConfigBuilder.create_learning_brain(), ConfigBuilder.create_stable_brain()]
>>> results = ExperimentRunner.run_multiple_configurations(configs, lambda c: pynlm.createBrain(c))

For more examples, see the documentation or tutorials in the NLM repository.
"""

# Provide module documentation
__doc__ = __doc__ or """
Advanced NLM Python Interface
=============================

This module provides enhanced Python interfaces for the NLM (Neural Learning Machine) simulation framework.
It extends the basic pybind11 bindings with Pythonic wrappers, advanced configuration builders,
pattern creation tools, and debugging utilities.

Key Features:
- Pythonic wrappers (SmartBrain, ConfigBuilder, PatternBuilder)
- Advanced configuration presets (learning, stable, explosive brains)
- Network analysis and debugging tools
- Experiment runner for comparing configurations
- Performance profiling and benchmarking
- Context managers for resource management

For basic usage, see the easy_usage.md documentation. For advanced usage, consult this module.
"""