# Advanced NLM Python API Improvements
# This file demonstrates the enhanced Python API for advanced users

import pynlm as nlm
import json
import time
from typing import Optional, List, Dict, Callable, Any
from enum import Enum

class AdvancedBrainSimulator:
    """
    Advanced NLM Python API for researchers and power users.
    
    This class provides enhanced functionality beyond the basic API,
    including advanced configuration, performance monitoring,
    debugging tools, and simulation control.
    """
    
    def __init__(self, config: Optional[nlm.Config] = None):
        """Initialize advanced brain simulator with optional configuration."""
        if config is None:
            config = nlm.createDefaultConfig()
        
        self.config = config
        self.brain = nlm.createBrain(config)
        self.brain.initialize()
        self.simulation_time = 0.0
        self.step_count = 0
        self.performance_stats = {
            'steps_per_second': 0.0,
            'neuron_firing_rate': 0.0,
            'memory_usage': 0.0,
            'cpu_usage': 0.0
        }
        self.debug_info = {
            'last_spike_patterns': [],
            'neuron_activity_history': [],
            'synaptic_weight_changes': [],
            'reward_history': []
        }
        self.callbacks = {
            'on_step': [],
            'on_reward': [],
            'on_spike': [],
            'on_error': []
        }
    
    def configure_advanced(self, **kwargs):
        """
        Advanced configuration with validation and validation.
        
        Args:
            **kwargs: Configuration options
        """
        config_options = {
            'brain': {
                'neuron_count': 1000,
                'region_count': 2,
                'connection_probability': 0.1,
                'stdp_ltp_weight': 0.01,
                'stdp_ltd_weight': 0.01,
                'stdp_tau': 20.0
            },
            'plasticity': {
                'hebbian_enabled': True,
                'stdp_enabled': True,
                'structural_enabled': True,
                'hebbian_rate': 0.001,
                'stdp_rate': 0.001,
                'structural_rate': 0.0001
            },
            'neuromodulation': {
                'dopamine_enabled': True,
                'curiosity_enabled': True,
                'novelty_enabled': True,
                'reward_scale': 1.0,
                'prediction_error_weight': 1.0
            },
            'memory': {
                'working_memory_size': 1000,
                'episodic_capacity': 100,
                'associative_threshold': 0.8
            },
            'environment': {
                'width': 20.0,
                'height': 20.0,
                'vision_width': 16,
                'vision_height': 16,
                'max_energy': 100.0,
                'energy_decay_rate': 0.01
            }
        }
        
        # Apply advanced configuration
        for category, options in kwargs.items():
            if category in config_options:
                for key, value in options.items():
                    if key in config_options[category]:
                        config_key = f"{category}.{key}"
                        self.config.set(config_key, value, nlm.ConfigSource::Default)
        
        # Update brain with new configuration
        self.brain.reset()
        # Note: In a real implementation, this would reinitialize the brain
        # with the new configuration
    
    def enable_advanced_features(self, features: List[str]):
        """Enable advanced brain features for research."""
        feature_map = {
            'advanced_plasticity': lambda: self.brain.getConfig().set("plasticity.advanced", True),
            'full_neuromodulation': lambda: self.brain.getConfig().set("neuromodulation.full", True),
            'enhanced_memory': lambda: self.brain.getConfig().set("memory.enhanced", True),
            'developmental_models': lambda: self.brain.getConfig().set("development.models", True),
            'predictive_learning': lambda: self.brain.getConfig().set("learning.predictive", True),
            'social_cognition': lambda: self.brain.getConfig().set("cognition.social", True)
        }
        
        for feature in features:
            if feature in feature_map:
                feature_map[feature]()
    
    def add_performance_monitoring(self, enabled: bool = True, interval: float = 0.1):
        """
        Add performance monitoring with real-time statistics.
        
        Args:
            enabled: Whether to enable performance monitoring
            interval: Monitoring interval in seconds
        """
        self.performance_monitoring_enabled = enabled
        self.performance_interval = interval
        
        if enabled:
            print(f"Performance monitoring enabled (interval: {interval}s)")
    
    def add_debug_logging(self, level: str = "info", enable_trace: bool = False):
        """
        Add enhanced debug logging for research and debugging.
        
        Args:
            level: Logging level (debug, info, warn, error)
            enable_trace: Enable detailed trace logging
        """
        self.debug_level = level
        self.trace_enabled = enable_trace
        
        if trace_enabled:
            print("Debug trace logging enabled")
        
        print(f"Debug logging level: {level}")
    
    def add_callback(self, event_type: str, callback: Callable):
        """
        Add callback function for simulation events.
        
        Args:
            event_type: Type of event ('step', 'reward', 'spike', 'error')
            callback: Function to call when event occurs
        """
        if event_type in self.callbacks:
            self.callbacks[event_type].append(callback)
            print(f"Callback added for {event_type} events")
        else:
            print(f"Unknown event type: {event_type}")
    
    def run_simulation_advanced(self, steps: int, timestep: float = 0.001, 
                               enable_checkpoints: bool = False,
                               checkpoint_interval: int = 1000):
        """
        Run advanced simulation with enhanced monitoring and control.
        
        Args:
            steps: Number of simulation steps to run
            timestep: Simulation timestep
            enable_checkpoints: Whether to enable checkpointing
            checkpoint_interval: Interval for saving checkpoints
        """
        print(f"Starting advanced simulation: {steps} steps, timestep: {timestep}")
        
        start_time = time.time()
        
        for step in range(steps):
            current_time = step * timestep
            self.simulation_time = current_time
            self.step_count = step
            
            # Record performance metrics
            if self.performance_monitoring_enabled and step % int(1.0 / timestep / self.performance_interval) == 0:
                self._update_performance_metrics()
            
            # Execute main simulation step
            self.brain.step(step, current_time)
            
            # Record debug information
            if self.trace_enabled:
                self._record_debug_info()
            
            # Execute callbacks
            for callback in self.callbacks['step']:
                callback(step, current_time, self.brain)
            
            # Handle checkpointing
            if enable_checkpoints and step > 0 and step % checkpoint_interval == 0:
                self.save_checkpoint(f"checkpoint_step_{step}.bin")
                print(f"Checkpoint saved at step {step}")
            
            # Progress reporting
            if step % (steps // 10) == 0:
                progress = (step / steps) * 100
                print(f"Progress: {progress:.1f}% (step {step}/{steps})")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        print(f"Simulation complete!")
        print(f"Total steps: {steps}")
        print(f"Total time: {total_time:.2f}s")
        print(f"Steps per second: {steps / total_time:.2f}")
        
        return {
            'total_steps': steps,
            'total_time': total_time,
            'steps_per_second': steps / total_time,
            'final_simulation_time': self.simulation_time,
            'final_step_count': self.step_count,
            'performance_stats': self.performance_stats,
            'debug_info': self.debug_info
        }
    
    def _update_performance_metrics(self):
        """Update performance monitoring metrics."""
        # Calculate performance metrics
        firing_rate = self.brain.getAverageFiringRate()
        self.performance_stats['neuron_firing_rate'] = firing_rate
        
        # Estimate performance based on simulation
        if self.step_count > 0:
            self.performance_stats['steps_per_second'] = self.step_count / (self.simulation_time + 0.001)
        
        # Memory usage (estimated)
        self.performance_stats['memory_usage'] = (
            self.brain.getTotalNeuronCount() * sizeof(float) +
            self.brain.getTotalSynapseCount() * sizeof(float)
        ) / (1024 * 1024)  # MB
    
    def _record_debug_info(self):
        """Record debug information for analysis."""
        # Record neuron activity patterns
        active_neuron_count = self.brain.getFiringNeuronCount()
        self.debug_info['neuron_activity_history'].append({
            'step': self.step_count,
            'active_neurons': active_neuron_count,
            'total_neurons': self.brain.getTotalNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate()
        })
        
        # Record spike patterns (simplified)
        if active_neuron_count > 0:
            spike_pattern = {
                'step': self.step_count,
                'active_ratio': active_neuron_count / self.brain.getTotalNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount()
            }
            self.debug_info['last_spike_patterns'].append(spike_pattern)
        
        # Keep only recent history for memory efficiency
        if len(self.debug_info['neuron_activity_history']) > 1000:
            self.debug_info['neuron_activity_history'] = self.debug_info['neuron_activity_history'][-100:]
        if len(self.debug_info['last_spike_patterns']) > 1000:
            self.debug_info['last_spike_patterns'] = self.debug_info['last_spike_patterns'][-100:]
    
    def save_checkpoint(self, filepath: str):
        """Save brain state to checkpoint file."""
        self.brain.save(filepath)
        print(f"Checkpoint saved: {filepath}")
    
    def load_checkpoint(self, filepath: str):
        """Load brain state from checkpoint file."""
        self.brain.load(filepath)
        print(f"Checkpoint loaded: {filepath}")
    
    def get_detailed_stats(self) -> Dict[str, Any]:
        """Get detailed simulation statistics."""
        return {
            'simulation': {
                'step_count': self.step_count,
                'simulation_time': self.simulation_time,
                'steps_per_second': self.performance_stats['steps_per_second']
            },
            'brain': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'excitement_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
                'developmental_stage': str(self.brain.getDevelopmentalStage())
            },
            'performance': self.performance_stats,
            'debug': {
                'recorded_steps': len(self.debug_info['neuron_activity_history']),
                'spike_patterns_recorded': len(self.debug_info['last_spike_patterns'])
            }
        }
    
    def export_data(self, filepath: str, format: str = 'json'):
        """
        Export simulation data for analysis.
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'binary')
        """
        stats = self.get_detailed_stats()
        
        if format == 'json':
            with open(filepath, 'w') as f:
                json.dump(stats, f, indent=2)
            print(f"Data exported to JSON: {filepath}")
        elif format == 'csv':
            self._export_csv(filepath, stats)
        elif format == 'binary':
            self._export_binary(filepath, stats)
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    def _export_csv(self, filepath: str, stats: Dict[str, Any]):
        """Export data in CSV format."""
        with open(filepath, 'w') as f:
            f.write("Metric,Value\n")
            for category, data in stats.items():
                for key, value in data.items():
                    f.write(f"{category}.{key},{value}\n")
    
    def _export_binary(self, filepath: str, stats: Dict[str, Any]):
        """Export data in binary format."""
        # Simple binary export - could be enhanced with serialization libraries
        with open(filepath, 'wb') as f:
            # Write a simple binary representation
            f.write(b'NLM_BINARY_STATS\x00')
            # This is a placeholder - real implementation would serialize stats properly
    
    def compare_with_baseline(self, baseline_stats: Dict[str, Any]) -> Dict[str, float]:
        """
        Compare current simulation with baseline statistics.
        
        Args:
            baseline_stats: Baseline statistics to compare against
            
        Returns:
            Dictionary of comparison metrics
        """
        current = self.get_detailed_stats()
        
        comparison = {}
        
        # Compare key metrics
        for baseline_key, baseline_value in baseline_stats.items():
            if baseline_key in current:
                if isinstance(baseline_value, dict) and isinstance(current[baseline_key], dict):
                    # Nested comparison
                    for subkey, subvalue in baseline_value.items():
                        if subkey in current[baseline_key]:
                            diff = current[baseline_key][subkey] - subvalue
                            rel_diff = (diff / subvalue) * 100 if subvalue != 0 else 0
                            comparison[f"{baseline_key}.{subkey}"] = {
                                'absolute_diff': diff,
                                'relative_diff_percent': rel_diff,
                                'current': current[baseline_key][subkey],
                                'baseline': subvalue
                            }
                elif isinstance(baseline_value, (int, float)) and isinstance(current.get(baseline_key), (int, float)):
                    diff = current[baseline_key] - baseline_value
                    rel_diff = (diff / baseline_value) * 100 if baseline_value != 0 else 0
                    comparison[baseline_key] = {
                        'absolute_diff': diff,
                        'relative_diff_percent': rel_diff,
                        'current': current[baseline_key],
                        'baseline': baseline_value
                    }
        
        return comparison


class ResearchEnvironment:
    """
    Research environment for controlled experiments and parameter sweeps.
    """
    
    def __init__(self):
        self.experiments = []
        self.results = []
    
    def run_parameter_sweep(self, param_space: Dict[str, List], 
                           base_config: Dict, n_experiments: int = 10):
        """
        Run parameter sweep for research experiments.
        
        Args:
            param_space: Dictionary mapping parameter names to lists of values
            base_config: Base configuration to start from
            n_experiments: Number of experiments to run
            
        Returns:
            List of experiment results
        """
        print(f"Running parameter sweep with {n_experiments} experiments")
        print(f"Parameter space: {param_space}")
        
        # Generate parameter combinations
        param_names = list(param_space.keys())
        param_values = list(param_space.values())
        
        experiments = []
        for i in range(min(n_experiments, len(param_values[0]))):  # Limited for demonstration
            config = base_config.copy()
            for j, param_name in enumerate(param_names):
                if j < len(param_values):
                    config[param_name] = param_values[j][i] if i < len(param_values[j]) else param_values[j][0]
            
            experiments.append({
                'id': i,
                'config': config,
                'parameters': {name: param_space[name][i] for name in param_names}
            })
        
        # Run experiments
        for experiment in experiments:
            print(f"Running experiment {experiment['id']} with parameters: {experiment['parameters']}")
            
            # Create and run simulation
            simulator = AdvancedBrainSimulator()
            simulator.configure_advanced(**experiment['config'])
            
            stats = simulator.run_simulation_advanced(steps=100)
            
            experiment['results'] = stats
            self.results.append(experiment)
        
        return self.results
    
    def analyze_results(self, metrics: List[str]) -> Dict[str, Any]:
        """
        Analyze and visualize experiment results.
        
        Args:
            metrics: List of metrics to analyze
            
        Returns:
            Analysis results
        """
        analysis = {
            'experiments_run': len(self.results),
            'metrics_analyzed': metrics,
            'summary_statistics': {},
            'correlations': {}
        }
        
        # Calculate summary statistics for each metric
        for metric in metrics:
            values = []
            for exp in self.results:
                if metric in exp['results']:
                    values.append(exp['results'][metric])
            
            if values:
                analysis['summary_statistics'][metric] = {
                    'mean': sum(values) / len(values),
                    'min': min(values),
                    'max': max(values),
                    'std': (sum((x - sum(values)/len(values))**2 for x in values) / len(values))**0.5,
                    'count': len(values)
                }
        
        return analysis


# Convenience functions for common research tasks
def create_research_config(config_type: str = 'standard') -> nlm.Config:
    """
    Create commonly used research configurations.
    
    Args:
        config_type: Type of configuration ('standard', 'exploration', 'exploitation', 'developmental')
        
    Returns:
        Configured NLM Config object
    """
    config = nlm.createDefaultConfig()
    
    configs = {
        'standard': {
            'brain.neuron_count': 1000,
            'brain.region_count': 2,
            'plasticity.stdp_enabled': True,
            'neuromodulation.reward_scale': 1.0
        },
        'exploration': {
            'brain.neuron_count': 2000,
            'plasticity.structural_enabled': True,
            'neuromodulation.curiosity_enabled': True,
            'neuromodulation.novelty_enabled': True
        },
        'exploitation': {
            'brain.neuron_count': 500,
            'plasticity.hebbian_enabled': True,
            'neuromodulation.reward_scale': 2.0
        },
        'developmental': {
            'brain.development_enabled': True,
            'brain.maturation_rate': 0.01,
            'plasticity.age_dependent': True
        }
    }
    
    if config_type in configs:
        for key, value in configs[config_type].items():
            config.set(key, value, nlm.ConfigSource::Default)
    
    return config


def run_research_simulation(steps: int, config: Optional[nlm.Config] = None,
                            **kwargs) -> Dict[str, Any]:
    """
    Quick research simulation with enhanced monitoring.
    
    Args:
        steps: Number of simulation steps
        config: Optional configuration
        **kwargs: Additional simulation parameters
        
    Returns:
        Simulation results
    """
    simulator = AdvancedBrainSimulator(config)
    
    # Apply any additional parameters
    if 'enable_performance_monitoring' in kwargs:
        simulator.add_performance_monitoring(**{k: v for k, v in kwargs.items() if k != 'enable_performance_monitoring'})
    
    if 'enable_debug_logging' in kwargs:
        simulator.add_debug_logging(**{k: v for k, v in kwargs.items() if k != 'enable_debug_logging'})
    
    # Run simulation
    results = simulator.run_simulation_advanced(steps)
    
    return results


# Export advanced API components
__all__ = [
    'AdvancedBrainSimulator',
    'ResearchEnvironment',
    'create_research_config',
    'run_research_simulation'
]
EOF