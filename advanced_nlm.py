"""
Advanced Python utilities for NLM (Neural Learning Machine)

This module provides enhanced Python APIs for NLM with advanced features
like experiment management, data visualization, and machine learning integration.
"""

import pynlm
from typing import Optional, List, Dict, Any, Callable
import json
import time
import os

__all__ = [
    'ExperimentManager',
    'AdvancedBrain',
    'DataCollector',
    'VisualProfiler',
    'MLIntegration',
    'createAdvancedBrain',
    'runAdvancedExperiment',
    'profileNeuronalActivity',
    'analyzeLearningDynamics'
]

class ExperimentManager:
    """
    Advanced experiment management for NLM simulations.
    
    Provides functionality to manage complex experimental setups,
    parameter sweeps, and result analysis.
    """
    
    def __init__(self, base_brain=None):
        """
        Initialize experiment manager.
        
        Args:
            base_brain: Optional base brain to use for experiments
        """
        self.experiments = []
        self.results = {}
        self.base_brain = base_brain
        self.current_experiment = None
        
    def create_parameter_sweep(self, param_name: str, param_values: List[Any], 
                               **kwargs) -> Dict[str, Any]:
        """
        Create a parameter sweep experiment.
        
        Args:
            param_name: Name of parameter to vary
            param_values: List of values for the parameter
            **kwargs: Additional experiment parameters
            
        Returns:
            Dictionary containing sweep configuration
        """
        sweep_config = {
            'type': 'parameter_sweep',
            'param_name': param_name,
            'param_values': param_values,
            'kwargs': kwargs
        }
        self.experiments.append(sweep_config)
        return sweep_config
        
    def run_sweep_experiment(self) -> Dict[str, Any]:
        """
        Run the parameter sweep experiment.
        
        Returns:
            Dictionary containing sweep results
        """
        if not self.experiments:
            raise ValueError("No experiments configured")
            
        sweep_config = self.experiments[0]
        if sweep_config['type'] != 'parameter_sweep':
            raise ValueError("Expected parameter sweep experiment")
            
        results = {}
        param_name = sweep_config['param_name']
        param_values = sweep_config['param_values']
        
        for value in param_values:
            experiment_key = f"{param_name}_{value}"
            
            # Create brain with parameter variation
            config = pynlm.createDefaultConfig()
            config.set(f"brain.{param_name}", value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run experiment
            self._run_single_experiment(brain, sweep_config['kwargs'])
            
            # Collect results
            results[experiment_key] = self._collect_experiment_results(brain)
            
        return results
        
    def _run_single_experiment(self, brain, kwargs):
        """Run a single experiment with given parameters."""
        steps = kwargs.get('steps', 100)
        
        for step in range(steps):
            brain.step(step)
            
    def _collect_experiment_results(self, brain) -> Dict[str, Any]:
        """Collect results from an experiment."""
        return {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount()
        }
        
    def save_experiment_results(self, filepath: str):
        """Save experiment results to file."""
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
            
    def load_experiment_results(self, filepath: str):
        """Load experiment results from file."""
        with open(filepath, 'r') as f:
            self.results = json.load(f)
class AdvancedBrain:
    """
    Enhanced brain interface with advanced functionality.
    
    Provides additional methods and properties for advanced brain manipulation
    and analysis.
    """
    
    def __init__(self, config=None):
        """
        Initialize advanced brain.
        
        Args:
            config: Optional configuration object
        """
        if config is None:
            config = pynlm.createDefaultConfig()
            
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.metrics_history = []
        self.neuron_activity_history = []
        
    def enable_advanced_features(self, **kwargs):
        """Enable advanced brain features."""
        defaults = {
            'reward_modulation': True,
            'structural_plasticity': True,
            'development': True,
            'curiosity': True,
            'prediction': True
        }
        
        for key, value in {**defaults, **kwargs}.items():
            if hasattr(self.brain, f'enable{key.title()}'):
                getattr(self.brain, f'enable{key.title()}')(value)
                
    def run_dynamics(self, steps: int, dt: float = 0.001, 
                    collect_metrics: bool = True) -> List[Dict[str, Any]]:
        """
        Run brain dynamics for specified number of steps.
        
        Args:
            steps: Number of steps to run
            dt: Time step duration
            collect_metrics: Whether to collect metrics
            
        Returns:
            List of metric dictionaries (if collect_metrics is True)
        """
        if collect_metrics:
            self.metrics_history.clear()
            self.neuron_activity_history.clear()
            
        for step in range(steps):
            self.brain.step(step, step * dt)
            
            if collect_metrics:
                metrics = self._collect_current_metrics(step, dt)
                self.metrics_history.append(metrics)
                self.neuron_activity_history.append(
                    self._collect_neuron_activity()
                )
                
        return self.metrics_history if collect_metrics else []
        
    def _collect_current_metrics(self, step: int, dt: float) -> Dict[str, Any]:
        """Collect current brain metrics."""
        return {
            'step': step,
            'time': step * dt,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'curiosity_level': getattr(self, 'curiosity_level', 0.0),
            'prediction_error': getattr(self, 'prediction_error', 0.0)
        }
        
    def _collect_neuron_activity(self) -> Dict[str, Any]:
        """Collect neuron activity statistics."""
        return {
            'active_neurons': [],
            'firing_rates': [],
            'membrane_potentials': []
        }
        
    def get_activity_pattern(self, window_size: int = 100) -> Dict[str, Any]:
        """Analyze activity patterns over time window."""
        if len(self.metrics_history) < window_size:
            return {}
            
        recent_metrics = self.metrics_history[-window_size:]
        
        return {
            'spike_trends': self._analyze_spike_trends(recent_metrics),
            'stability_index': self._calculate_stability(recent_metrics),
            'exploration_index': self._calculate_exploration_index(),
            'learning_curve': self._extract_learning_curve(recent_metrics)
        }
        
    def _analyze_spike_trends(self, metrics: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze spike trends over time."""
        if len(metrics) < 2:
            return {}
            
        spike_counts = [m['total_spikes'] for m in metrics]
        first_half = spike_counts[:len(spike_counts)//2]
        second_half = spike_counts[len(spike_counts)//2:]
        
        return {
            'first_half_avg': sum(first_half) / len(first_half),
            'second_half_avg': sum(second_half) / len(second_half),
            'growth_rate': (second_half[-1] - first_half[0]) / len(first_half) if first_half else 0,
            'stability': 1.0 - (max(spike_counts) - min(spike_counts)) / max(spike_counts) if max(spike_counts) > 0 else 0
        }
        
    def _calculate_stability(self, metrics: List[Dict[str, Any]]) -> float:
        """Calculate stability index."""
        if len(metrics) < 2:
            return 1.0
            
        firing_rates = [m['average_firing_rate'] for m in metrics]
        if len(firing_rates) < 2:
            return 1.0
            
        mean_rate = sum(firing_rates) / len(firing_rates)
        if mean_rate == 0:
            return 1.0
            
        variance = sum((rate - mean_rate) ** 2 for rate in firing_rates) / len(firing_rates)
        coefficient_of_variation = (variance ** 0.5) / mean_rate
        
        return max(0.0, 1.0 - coefficient_of_variation)
        
    def _calculate_exploration_index(self) -> float:
        """Calculate exploration index based on curiosity."""
        return getattr(self, 'curiosity_level', 0.0)
        
    def _extract_learning_curve(self, metrics: List[Dict[str, Any]]) -> List[float]:
        """Extract learning curve from metrics."""
        return [m['average_firing_rate'] for m in metrics]
        
    def save_brain_state(self, filepath: str):
        """Save brain state to file."""
        self.brain.save(filepath)
        
    def load_brain_state(self, filepath: str):
        """Load brain state from file."""
        self.brain.load(filepath)
        
    def get_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'metrics_collected': len(self.metrics_history),
            'experiment_duration': len(self.metrics_history) * 0.001 if self.metrics_history else 0
        }
class DataCollector:
    """
    Advanced data collection for NLM simulations.
    
    Provides functionality to collect, process, and analyze data from
    NLM simulations with support for different data types and formats.
    """
    
    def __init__(self):
        self.raw_data = []
        self.processed_data = {}
        self.filters = []
        
    def collect_data(self, brain, experiment_name: str, 
                    metadata: Optional[Dict[str, Any]] = None):
        """
        Collect data from brain simulation.
        
        Args:
            brain: Brain object to collect data from
            experiment_name: Name of experiment
            metadata: Optional metadata dictionary
        """
        data_point = {
            'timestamp': time.time(),
            'experiment_name': experiment_name,
            'brain_stats': brain.get_stats(),
            'metadata': metadata or {}
        }
        
        self.raw_data.append(data_point)
        
    def apply_filter(self, filter_func: Callable):
        """Apply filter function to collected data."""
        self.filters.append(filter_func)
        
    def process_data(self):
        """Process collected data using applied filters."""
        self.processed_data = {}
        
        for filter_func in self.filters:
            filtered_data = [filter_func(d) for d in self.raw_data]
            filter_name = filter_func.__name__
            self.processed_data[filter_name] = filtered_data
            
    def get_summary_statistics(self) -> Dict[str, Any]:
        """Get summary statistics of processed data."""
        if not self.raw_data:
            return {}
            
        all_spikes = [d['brain_stats']['total_spikes'] for d in self.raw_data]
        all_firing = [d['brain_stats']['firing_neurons'] for d in self.raw_data]
        
        return {
            'total_experiments': len(self.raw_data),
            'total_spikes': sum(all_spikes),
            'avg_spikes_per_experiment': sum(all_spikes) / len(all_spikes),
            'total_firing_neurons': sum(all_firing),
            'avg_firing_per_experiment': sum(all_firing) / len(all_firing),
            'spike_range': (min(all_spikes), max(all_spikes)),
            'firing_range': (min(all_firing), max(all_firing))
        }
        
    def export_data(self, filepath: str, format: str = 'json'):
        """Export data to file."""
        if format == 'json':
            with open(filepath, 'w') as f:
                json.dump(self.raw_data, f, indent=2)
        elif format == 'csv':
            import csv
            if self.raw_data:
                with open(filepath, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(self.raw_data[0].keys())
                    for row in self.raw_data:
                        writer.writerow(row.values())
class VisualProfiler:
    """
    Visualization and profiling utilities for NLM.
    
    Provides basic visualization capabilities for NLM simulation data.
    """
    
    @staticmethod
    def create_spike_histogram(spike_data: List[int], bins: int = 20) -> Dict[str, Any]:
        """Create spike count histogram."""
        if not spike_data:
            return {'bins': [], 'counts': []}
            
        min_val = min(spike_data)
        max_val = max(spike_data)
        bin_width = (max_val - min_val) / bins if max_val > min_val else 1
        
        bins_list = []
        counts_list = []
        
        for i in range(bins):
            bin_min = min_val + i * bin_width
            bin_max = bin_min + bin_width
            count = sum(1 for val in spike_data if bin_min <= val < bin_max)
            
            bins_list.append(f"[{bin_min:.1f}, {bin_max:.1f})")
            counts_list.append(count)
            
        return {
            'bins': bins_list,
            'counts': counts_list,
            'mean': sum(spike_data) / len(spike_data),
            'std': (sum((x - sum(spike_data)/len(spike_data))**2 for x in spike_data) / len(spike_data))**0.5
        }
        
    @staticmethod
    def analyze_activity_patterns(metrics: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze activity patterns from metrics."""
        if not metrics:
            return {}
            
        firing_rates = [m['average_firing_rate'] for m in metrics]
        
        return {
            'mean_firing_rate': sum(firing_rates) / len(firing_rates),
            'firing_rate_variance': sum((rate - sum(firing_rates)/len(firing_rates))**2 for rate in firing_rates) / len(firing_rates),
            'coefficient_of_variation': (sum((rate - sum(firing_rates)/len(firing_rates))**2 for rate in firing_rates) / len(firing_rates))**0.5 / (sum(firing_rates)/len(firing_rates)) if sum(firing_rates) > 0 else 0,
            'trend': 'increasing' if len(firing_rates) >= 2 and firing_rates[-1] > firing_rates[0] else 'decreasing' if firing_rates[-1] < firing_rates[0] else 'stable'
        }
class MLIntegration:
    """
    Machine learning integration for NLM.
    
    Provides interfaces to connect NLM with popular machine learning frameworks
    and algorithms.
    """
    
    def __init__(self):
        self.ml_models = {}
        self.feature_extractors = {}
        
    def register_feature_extractor(self, name: str, extractor: Callable):
        """Register a feature extractor function."""
        self.feature_extractors[name] = extractor
        
    def extract_features(self, brain, extractor_name: str) -> Dict[str, Any]:
        """Extract features from brain using registered extractor."""
        if extractor_name not in self.feature_extractors:
            raise ValueError(f"Extractor '{extractor_name}' not found")
            
        extractor = self.feature_extractors[extractor_name]
        return extractor(brain)
        
    def train_supervised_model(self, X, y, model_type: str = 'random_forest'):
        """Train a supervised ML model on NLM data."""
        if model_type not in ['random_forest', 'neural_network', 'svm']:
            raise ValueError(f"Model type '{model_type}' not supported")
            
        # Placeholder for ML model training
        self.ml_models[model_type] = {
            'trained': True,
            'parameters': X.shape if hasattr(X, 'shape') else len(X),
            'labels': len(set(y)) if hasattr(y, '__len__') else 2
        }
        
        return self.ml_models[model_type]
        
    def predict_with_model(self, model_name: str, X):
        """Make predictions using trained ML model."""
        if model_name not in self.ml_models:
            raise ValueError(f"Model '{model_name}' not found or not trained")
            
        # Placeholder for ML prediction
        return [0.5] * (len(X) if hasattr(X, '__len__') else 1)
def createAdvancedBrain(config=None):
    """Convenience function to create an advanced brain."""
    return AdvancedBrain(config)
def runAdvancedExperiment(brain, experiment_type: str, **kwargs):
    """Convenience function to run advanced experiments."""
    if experiment_type == 'dynamics':
        return brain.run_dynamics(**kwargs)
    elif experiment_type == 'parameter_sweep':
        return brain.parameter_sweep(**kwargs)
    else:
        raise ValueError(f"Experiment type '{experiment_type}' not supported")
def profileNeuronalActivity(brain, window_size: int = 100) -> Dict[str, Any]:
    """Profile neuronal activity patterns."""
    return brain.get_activity_pattern(window_size)
def analyzeLearningDynamics(brain) -> Dict[str, Any]:
    """Analyze learning dynamics from brain simulation."""
    if not hasattr(brain, 'metrics_history') or not brain.metrics_history:
        return {}
        
    return VisualProfiler.analyze_activity_patterns(brain.metrics_history)
