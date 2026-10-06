"""
NLM Utilities and Helper Functions

This module provides additional utility functions for working with the NLM
neural simulation framework. These utilities help with common tasks like
configuration management, result analysis, visualization preparation, and
simulation management.
"""

import pynlm
import numpy as np
from typing import List, Dict, Any, Optional, Callable, Tuple
import json
import time
import os

class NLMConfigManager:
    """
    Configuration management utilities for NLM simulations.
    
    This class provides convenient methods for creating, managing, and
    optimizing NLM configurations for different types of simulations.
    """
    
    @staticmethod
    def create_research_config(neuron_count: int = 1000, 
                              learning_rate: float = 0.01,
                              simulation_time: float = 10.0) -> pynlm.Config:
        """
        Create a configuration optimized for research simulations.
        
        Args:
            neuron_count: Number of neurons in the simulation
            learning_rate: Plasticity learning rate
            simulation_time: Total simulation time in seconds
            
        Returns:
            Configured NLM config
        """
        config = pynlm.Config()
        config.set("neuron_count", neuron_count)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1f)
        config.set("random_seed", 42)
        config.set("simulation_timestep", 0.001)
        config.set("stdp_ltp_weight", learning_rate)
        config.set("stdp_ltd_weight", learning_rate * 0.9)
        config.set("stdp_tau", 20.0f)
        config.set("synaptogenesis_rate", 0.0001f)
        config.set("pruning_rate", 0.00001f)
        config.set("replay_interval", 100)
        config.set("consolidation_interval", 1000)
        
        return config
    
    @staticmethod
    def create_learning_config(neuron_count: int = 500,
                             exploration_factor: float = 0.3,
                             curiosity_enabled: bool = True) -> pynlm.Config:
        """
        Create a configuration optimized for learning simulations.
        
        Args:
            neuron_count: Number of neurons
            exploration_factor: Factor for curiosity-driven exploration
            curiosity_enabled: Whether to enable curiosity system
            
        Returns:
            Configured NLM config
        """
        config = pynlm.Config()
        config.set("neuron_count", neuron_count)
        config.set("region_count", 2)  # Multiple regions for exploration
        config.set("connection_probability", 0.15f)
        config.set("random_seed", 123)
        config.set("simulation_timestep", 0.001)
        config.set("curiosity_threshold", exploration_factor)
        
        return config
    
    @staticmethod
    def create_visualization_config(width: int = 800, height: int = 600,
                                   fps: int = 30) -> Dict[str, Any]:
        """
        Create visualization parameters.
        
        Args:
            width: Display width in pixels
            height: Display height in pixels  
            fps: Frames per second for animation
            
        Returns:
            Dictionary with visualization parameters
        """
        return {
            'width': width,
            'height': height,
            'fps': fps,
            'neuron_size': 2,
            'synapse_width': 0.5,
            'color_scheme': 'default',
            'show_labels': False,
            'show_connections': True
        }


class SimulationManager:
    """
    Simulation management utilities for NLM.
    
    This class provides tools for running, monitoring, and analyzing
    NLM simulations more efficiently.
    """
    
    def __init__(self, config: pynlm.Config):
        """Initialize with configuration."""
        self.config = config
        self.brain = pynlm.Brain(config)
        self.metrics_history = []
        self.checkpoint_enabled = config.getOr<bool>("enable_checkpointing", False)
    
    def run_simulation(self, 
                      steps: int, 
                      callbacks: Optional[Dict[str, Callable]] = None,
                      verbose: bool = False) -> Dict[str, Any]:
        """
        Run a simulation with configurable callbacks and monitoring.
        
        Args:
            steps: Number of simulation steps to run
            callbacks: Dictionary of callback functions for different events
            verbose: Whether to print progress information
            
        Returns:
            Dictionary with simulation results and metrics
        """
        if callbacks is None:
            callbacks = {}
        
        start_time = time.time()
        
        # Initialize brain
        if not self.brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        metrics = {
            'steps_completed': 0,
            'total_reward': 0.0,
            'avg_firing_rate': 0.0,
            'max_firing_rate': 0.0,
            'total_spikes': 0,
            'start_time': start_time,
            'end_time': None,
            'duration_seconds': None
        }
        
        try:
            for step in range(steps):
                # Record current state
                current_time = step * self.config.getOr<double>("simulation_timestep", 0.001)
                
                # Run simulation step
                self.brain.step(step, current_time)
                
                # Collect metrics
                firing_rate = self.brain.getAverageFiringRate()
                total_spikes = self.brain.getTotalSpikeCount()
                
                metrics['avg_firing_rate'] += firing_rate
                metrics['max_firing_rate'] = max(metrics['max_firing_rate'], firing_rate)
                metrics['total_spikes'] = total_spikes
                
                # Check for callbacks
                if 'on_step_complete' in callbacks:
                    callbacks['on_step_complete'](step, current_time, self.brain)
                
                if verbose and step % 100 == 0:
                    print(f"Step {step}/{steps}: Firing rate = {firing_rate:.3f}")
                
                metrics['steps_completed'] = step + 1
            
            # Calculate final metrics
            metrics['avg_firing_rate'] /= steps
            metrics['end_time'] = time.time()
            metrics['duration_seconds'] = metrics['end_time'] - metrics['start_time']
            metrics['performance_steps_per_sec'] = steps / metrics['duration_seconds']
            
            # Record final metrics
            self.metrics_history.append(metrics.copy())
            
            # Check for completion callback
            if 'on_simulation_complete' in callbacks:
                callbacks['on_simulation_complete'](metrics, self.brain)
            
            return metrics
            
        except Exception as e:
            print(f"Simulation interrupted at step {step}: {e}")
            metrics['error'] = str(e)
            metrics['steps_completed'] = step
            return metrics
    
    def save_metrics(self, filename: str):
        """Save simulation metrics to file."""
        with open(filename, 'w') as f:
            json.dump(self.metrics_history, f, indent=2)
    
    def load_metrics(self, filename: str):
        """Load simulation metrics from file."""
        with open(filename, 'r') as f:
            self.metrics_history = json.load(f)


class AnalysisTools:
    """
    Analysis and visualization utilities for NLM simulation data.
    
    This class provides tools for analyzing simulation results and preparing
    data for visualization.
    """
    
    @staticmethod
    def compute_neural_dynamics(neural_data: List[Dict]) -> Dict[str, Any]:
        """
        Compute neural dynamics metrics from simulation data.
        
        Args:
            neural_data: List of neural state dictionaries
            
        Returns:
            Dictionary with computed metrics
        """
        if not neural_data:
            return {}
        
        # Extract key metrics
        firing_rates = [d.get('firing_rate', 0) for d in neural_data]
        spike_counts = [d.get('spike_count', 0) for d in neural_data]
        rewards = [d.get('reward', 0) for d in neural_data]
        
        # Compute statistics
        metrics = {
            'firing_rate': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'min': np.min(firing_rates),
                'max': np.max(firing_rates),
                'median': np.median(firing_rates)
            },
            'spike_count': {
                'total': np.sum(spike_counts),
                'mean': np.mean(spike_counts),
                'std': np.std(spike_counts)
            },
            'reward': {
                'total': np.sum(rewards),
                'mean': np.mean(rewards),
                'std': np.std(rewards),
                'positive_episodes': sum(1 for r in rewards if r > 0),
                'negative_episodes': sum(1 for r in rewards if r < 0)
            }
        }
        
        return metrics
    
    @staticmethod
    def prepare_visualization_data(brain: pynlm.Brain, 
                                  neurons: Optional[List] = None,
                                  connections: Optional[List] = None) -> Dict[str, Any]:
        """
        Prepare data for neural network visualization.
        
        Args:
            brain: NLM brain instance
            neurons: Optional list of neurons to highlight
            connections: Optional list of connections to highlight
            
        Returns:
            Dictionary with visualization data
        """
        # Get brain statistics
        stats = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        # Get region information
        regions = []
        for i in range(brain.getRegionCount()):
            region = brain.getRegion(pynlm.RegionId(i + 1))
            if region:
                regions.append({
                    'id': i + 1,
                    'name': region.getName(),
                    'neuron_count': region.getTotalNeuronCount(),
                    'synapse_count': region.getSynapseCount(),
                    'average_synaptic_weight': region.getAverageSynapticWeight()
                })
        
        # Get neuron information if specified
        neuron_data = []
        if neurons:
            for neuron_id in neurons:
                neuron = brain.getNeuron(neuron_id) if hasattr(brain, 'getNeuron') else None
                if neuron:
                    neuron_data.append({
                        'id': neuron_id,
                        'type': 'unknown',  # Would need neuron->getType()
                        'membrane_potential': 0.0,  # Would need neuron->getMembranePotential()
                        'firing_state': 'unknown'
                    })
        
        return {
            'statistics': stats,
            'regions': regions,
            'neurons': neuron_data,
            'timestamp': time.time()
        }
    
    @staticmethod
    def compare_simulations(sim_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Compare multiple simulation results.
        
        Args:
            sim_results: List of simulation result dictionaries
            
        Returns:
            Dictionary with comparison metrics
        """
        if len(sim_results) < 2:
            return {}
        
        # Identify common metrics
        common_metrics = set()
        for result in sim_results:
            common_metrics.update(result.keys())
        
        comparison = {
            'simulation_count': len(sim_results),
            'metrics_compared': list(common_metrics)
        }
        
        # Compare each common metric
        for metric in common_metrics:
            values = []
            for result in sim_results:
                if metric in result and isinstance(result[metric], (int, float)):
                    values.append(result[metric])
            
            if values:
                comparison[f'{metric}_comparison'] = {
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'range': np.max(values) - np.min(values)
                }
        
        return comparison


class ExperimentRunner:
    """
    Run predefined NLM experiments with consistent setup and analysis.
    
    This class provides predefined experiments for testing NLM functionality
    and comparing results across different configurations.
    """
    
    @staticmethod
    def run_plasticity_learning_experiment(neuron_count: int = 1000, 
                                          steps: int = 2000) -> Dict[str, Any]:
        """
        Run the plasticity learning experiment.
        
        This simulates learning through experience and measures synaptic changes.
        """
        config = NLMConfigManager.create_research_config(neuron_count)
        sim_manager = SimulationManager(config)
        
        # Create callback to record learning metrics
        learning_metrics = []
        
        def learning_callback(step, time, brain):
            learning_metrics.append({
                'step': step,
                'time': time,
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'synapse_count': brain.getTotalSynapseCount()
            })
        
        result = sim_manager.run_simulation(
            steps, 
            {'on_step_complete': learning_callback},
            verbose=True
        )
        
        return {
            'simulation_result': result,
            'learning_metrics': learning_metrics,
            'experiment_type': 'plasticity_learning',
            'parameters': {
                'neuron_count': neuron_count,
                'steps': steps
            }
        }
    
    @staticmethod
    def run_neural_dynamics_experiment(neuron_count: int = 500,
                                      exploration_steps: int = 1000) -> Dict[str, Any]:
        """
        Run the neural dynamics exploration experiment.
        
        This explores how neural activity changes with different stimulation patterns.
        """
        config = NLMConfigManager.create_learning_config(neuron_count)
        sim_manager = SimulationManager(config)
        
        # Run exploration simulation
        result = sim_manager.run_simulation(exploration_steps, verbose=True)
        
        return {
            'simulation_result': result,
            'experiment_type': 'neural_dynamics',
            'parameters': {
                'neuron_count': neuron_count,
                'steps': exploration_steps
            }
        }
    
    @staticmethod
    def run_integration_experiment() -> Dict[str, Any]:
        """
        Run the integration experiment.
        
        This tests whether all NLM systems are properly connected.
        """
        # For integration experiment, we need to check which systems are available
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        
        brain = pynlm.Brain(config)
        if not brain.initialize():
            return {'error': 'Failed to initialize brain for integration test'}
        
        # Check system availability
        integration_results = {
            'working_memory': brain.getWorkingMemory() is not None,
            'episodic_memory': brain.getEpisodicMemory() is not None,
            'associative_memory': brain.getAssociativeMemory() is not None,
            'prediction_system': brain.getPredictionSystem() is not None,
            'planner': brain.getPlanner() is not None,
            'concept_formation': brain.getConceptFormation() is not None,
            'attention': brain.getAttention() is not None,
            'development_system': brain.getDevelopmentSystem() is not None,
            'dopamine': brain.getDopamine() is not None,
            'curiosity': brain.getCuriosity() is not None,
            'novelty': brain.getNovelty() is not None,
            'prediction_error': brain.getPredictionErrorSignal() is not None,
        }
        
        return {
            'integration_results': integration_results,
            'experiment_type': 'integration',
            'system_count': sum(integration_results.values()),
            'total_systems': len(integration_results)
        }


# Convenience functions for common operations
def create_demo_agent(config_file: Optional[str] = None) -> SimulationManager:
    """
    Create a demo agent with optimized settings for demonstrations.
    
    Args:
        config_file: Optional path to configuration file
        
    Returns:
        Configured SimulationManager instance
    """
    if config_file and os.path.exists(config_file):
        config = pynlm.Config()
        config.loadFromFile(config_file)
    else:
        config = NLMConfigManager.create_research_config(500)
    
    return SimulationManager(config)


def analyze_simulation_results(results_file: str) -> Dict[str, Any]:
    """
    Load and analyze simulation results from file.
    
    Args:
        results_file: Path to results file
        
    Returns:
        Dictionary with analysis results
    """
    sim_manager = SimulationManager(pynlm.Config())
    sim_manager.load_metrics(results_file)
    
    analysis = {}
    
    for i, metrics in enumerate(sim_manager.metrics_history):
        analysis[f'simulation_{i}'] = AnalysisTools.compute_neural_dynamics([metrics])
    
    return AnalysisTools.compare_simulations(
        [AnalysisTools.compute_neural_dynamics([m]) for m in sim_manager.metrics_history]
    )


# Export commonly used functions
__all__ = [
    'NLMConfigManager',
    'SimulationManager', 
    'AnalysisTools',
    'ExperimentRunner',
    'create_demo_agent',
    'analyze_simulation_results'
]
