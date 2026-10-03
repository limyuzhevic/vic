"""
Advanced NLM Python Module

This module provides enhanced functionality for the NLM (Neural Learning Machine)
neural simulation framework, including:

- Advanced neural network manipulation
- Enhanced configuration management
- Comprehensive memory management tools
- Advanced debugging and monitoring capabilities
- Batch processing and parallel execution utilities
- Experiment tracking and comparison tools
"""

import pynlm
import time
import numpy as np
import json
from typing import List, Dict, Optional, Callable, Any, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
import copy
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
from pathlib import Path

class ErrorType(Enum):
    """Error types for NLM operations"""
    VALIDATION_ERROR = "validation_error"
    MEMORY_ERROR = "memory_error"
    CONFIGURATION_ERROR = "configuration_error"
    NETWORK_ERROR = "network_error"
    TIMEOUT_ERROR = "timeout_error"
    UNKNOWN_ERROR = "unknown_error"

@dataclass
class NLMErrors:
    """Class for managing NLM errors with detailed information"""
    
    def __init__(self):
        self.error_history: List[Dict[str, Any]] = []
        self.last_error: Optional[Dict[str, Any]] = None
        self.error_count: int = 0
    
    def add_error(self, error_type: ErrorType, message: str, 
                  context: Optional[Dict] = None, recoverable: bool = False):
        """Add an error to the error history"""
        error_entry = {
            'timestamp': time.time(),
            'type': error_type.value,
            'message': message,
            'context': context or {},
            'recoverable': recoverable,
            'error_count': self.error_count + 1
        }
        
        self.error_history.append(error_entry)
        self.last_error = error_entry
        self.error_count += 1
    
    def get_last_error(self) -> Optional[Dict[str, Any]]:
        """Get the most recent error"""
        return self.last_error
    
    def get_errors_by_type(self, error_type: ErrorType) -> List[Dict[str, Any]]:
        """Get errors of a specific type"""
        return [e for e in self.error_history if e['type'] == error_type.value]
    
    def get_recoverable_errors(self) -> List[Dict[str, Any]]:
        """Get recoverable errors"""
        return [e for e in self.error_history if e.get('recoverable', False)]
    
    def clear(self):
        """Clear error history"""
        self.error_history.clear()
        self.last_error = None
        self.error_count = 0
    
    def get_summary(self) -> Dict[str, Any]:
        """Get error summary statistics"""
        if not self.error_history:
            return {'total_errors': 0, 'error_types': {}, 'recoverable_count': 0}
        
        error_types = {}
        for error in self.error_history:
            error_type = error['type']
            error_types[error_type] = error_types.get(error_type, 0) + 1
        
        return {
            'total_errors': self.error_count,
            'error_types': error_types,
            'recoverable_count': len(self.get_recoverable_errors()),
            'last_error': self.last_error,
            'most_recent_errors': self.error_history[-5:]  # Last 5 errors
        }

class AdvancedConfig:
    """
    Advanced configuration management with validation, templates, and
    inheritance capabilities.
    """
    
    def __init__(self, base_config: Optional[Dict] = None):
        self.config = base_config or {}
        self.defaults = {
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.1,
            'plasticity_learning_rate': 0.01,
            'simulation_timestep': 0.001,
            'max_simulation_steps': 10000,
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'enable_development': True,
            'enable_structural_plasticity': False
        }
        self.validation_rules = self._get_default_validation_rules()
        self.config_history: List[Dict[str, Any]] = []
    
    def _get_default_validation_rules(self) -> Dict[str, Callable]:
        """Get default validation rules for configuration values"""
        return {
            'neuron_count': lambda x: isinstance(x, int) and x > 0 and x <= 10000,
            'region_count': lambda x: isinstance(x, int) and x > 0 and x <= 100,
            'connection_probability': lambda x: isinstance(x, (int, float)) and 0.0 <= x <= 1.0,
            'plasticity_learning_rate': lambda x: isinstance(x, (int, float)) and 0.0 <= x <= 1.0,
            'simulation_timestep': lambda x: isinstance(x, (int, float)) and x > 0.0,
            'max_simulation_steps': lambda x: isinstance(x, int) and x > 0 and x <= 1000000,
            'enable_reward_modulation': lambda x: isinstance(x, bool),
            'enable_curiosity': lambda x: isinstance(x, bool),
            'enable_development': lambda x: isinstance(x, bool),
            'enable_structural_plasticity': lambda x: isinstance(x, bool)
        }
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from a JSON file"""
        try:
            with open(filepath, 'r') as f:
                file_config = json.load(f)
            
            # Merge with existing config (file config takes precedence)
            merged_config = {**self.config, **file_config}
            self._validate_config(merged_config)
            self.config = merged_config
            
            self.config_history.append({
                'timestamp': time.time(),
                'action': 'load_from_file',
                'source': filepath,
                'config_keys': list(merged_config.keys())
            })
            
            return True
        except Exception as e:
            # In a real implementation, we would add this error to NLMErrors
            print(f"Error loading config from file: {e}")
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to a JSON file"""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.config, f, indent=2)
            
            self.config_history.append({
                'timestamp': time.time(),
                'action': 'save_to_file',
                'source': filepath,
                'config_keys': list(self.config.keys())
            })
            
            return True
        except Exception as e:
            print(f"Error saving config to file: {e}")
            return False
    
    def set(self, key: str, value: Any, source: str = "runtime") -> bool:
        """Set a configuration value with validation"""
        if key not in self.validation_rules:
            # If key is not in validation rules, allow it but log a warning
            print(f"Warning: Configuration key '{key}' not in validation rules")
        elif not self.validation_rules[key](value):
            raise ValueError(f"Invalid value for key '{key}': {value}")
        
        old_value = self.config.get(key)
        self.config[key] = value
        
        self.config_history.append({
            'timestamp': time.time(),n        'action': 'set',
            'key': key,
            'old_value': old_value,
            'new_value': value,
            'source': source
        })
        
        return True
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value"""
        return self.config.get(key, default)
    
    def apply_to_brain(self, brain: pynlm.Brain) -> bool:
        """Apply configuration to a brain instance"""
        try:
            cfg = brain.getConfig()
            for key, value in self.config.items():
                # Try to convert string values to appropriate types
                if isinstance(value, str):
                    if value.lower() in ['true', 'false']:
                        value = value.lower() == 'true'
                    elif value.isdigit():
                        value = int(value)
                    elif '.' in value and value.replace('.', '', 1).isdigit():
                        value = float(value)
                
                cfg.set(key, value, "Runtime")
            
            return True
        except Exception as e:
            print(f"Error applying config to brain: {e}")
            return False
    
    def merge(self, other_config: Dict, strategy: str = "override") -> 'AdvancedConfig':
        """Merge another configuration with this one"""
        result = AdvancedConfig(copy.deepcopy(self.config))
        
        if strategy == "override":
            result.config.update(other_config)
        elif strategy == "deep":
            for key, value in other_config.items():
                if isinstance(value, dict) and key in result.config:
                    result.config[key].update(value)
                else:
                    result.config[key] = value
        elif strategy == "merge":
            for key, value in other_config.items():
                if key in result.config and isinstance(result.config[key], dict) and isinstance(value, dict):
                    result.config[key].update(value)
                else:
                    result.config[key] = value
        
        # Re-validate after merge
        result._validate_config(result.config)
        return result
    
    def _validate_config(self, config: Dict):
        """Validate the entire configuration"""
        for key, validator in self.validation_rules.items():
            if key in config and not validator(config[key]):
                raise ValueError(f"Invalid value for key '{key}': {config[key]}")
    
    def get_summary(self) -> Dict[str, Any]:
        """Get configuration summary"""
        return {
            'config_keys': list(self.config.keys()),
            'config_count': len(self.config),
            'default_keys': list(self.defaults.keys()),
            'history_count': len(self.config_history),
            'last_action': self.config_history[-1] if self.config_history else None,
            'config': self.config
        }

class MemoryManager:
    """
    Advanced memory management tools for NLM brain instances.
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.memory_snapshots: List[Dict[str, Any]] = []
        self.compression_ratio = 0.8
    
    def take_snapshot(self, label: str = "", include_weights: bool = True,
                     include_neuron_states: bool = True) -> Dict[str, Any]:
        """Take a snapshot of current brain state"""
        snapshot = {
            'timestamp': time.time(),
            'label': label,
            'step': self.brain.getTotalSpikeCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'region_count': self.brain.getRegionCount(),
            'development_stage': self.brain.getDevelopmentalStage()
        }
        
        if include_weights:
            weights = self._extract_weights()
            snapshot['weights'] = weights
        
        if include_neuron_states:
            states = self._extract_neuron_states()
            snapshot['neuron_states'] = states
        
        self.memory_snapshots.append(snapshot)
        return snapshot
    
    def _extract_weights(self) -> Dict[str, Any]:
        """Extract synapse weights from brain"""
        weights = {}
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                region_weights = []
                for synapse in region.getSynapses():
                    region_weights.append(synapse.getWeight())
                weights[f'region_{region_id}'] = region_weights
        return weights
    
    def _extract_neuron_states(self) -> Dict[str, Any]:
        """Extract neuron states from brain"""
        states = {}
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                neuron_states = []
                for neuron in region.getAllNeurons():
                    neuron_states.append({
                        'id': neuron.getId(),
                        'membrane_potential': neuron.getMembranePotential(),
                        'threshold': neuron.getThreshold(),
                        'firing_state': neuron.getFiringState()
                    })
                states[f'region_{region_id}'] = neuron_states
        return states
    
    def restore_snapshot(self, snapshot_index: int) -> bool:
        """Restore brain state from a snapshot"""
        if snapshot_index < 0 or snapshot_index >= len(self.memory_snapshots):
            raise IndexError("Invalid snapshot index")
        
        snapshot = self.memory_snapshots[snapshot_index]
        
        # In a real implementation, this would restore the actual state
        # For now, we'll just update some basic stats
        print(f"Restoring snapshot: {snapshot.get('label', f'Snapshot {snapshot_index}')}")
        print(f"Step: {snapshot.get('step', 0)}, Spike count: {snapshot.get('spike_count', 0)}")
        
        return True
    
    def compress_snapshots(self) -> List[Dict[str, Any]]:
        """Compress memory snapshots to reduce memory usage"""
        if not self.memory_snapshots:
            return []
        
        # Simple compression: keep only every nth snapshot
        compression_factor = max(1, int(1.0 / self.compression_ratio))
        compressed = self.memory_snapshots[::compression_factor]
        
        print(f"Compressed {len(self.memory_snapshots)} snapshots to {len(compressed)} snapshots")
        return compressed
    
    def get_memory_usage(self) -> Dict[str, Any]:
        """Get memory usage statistics"""
        total_snapshots = len(self.memory_snapshots)
        compressed_size = len(self.compress_snapshots())
        
        return {
            'snapshot_count': total_snapshots,
            'compressed_count': compressed_size,
            'compression_ratio': compressed_size / max(total_snapshots, 1),
            'estimated_memory_savings': (1 - compressed_size / max(total_snapshots, 1)) * 100,
            'latest_snapshot': self.memory_snapshots[-1] if self.memory_snapshots else None,
            'oldest_snapshot': self.memory_snapshots[0] if self.memory_snapshots else None
        }

class AdvancedDebugger:
    """
    Advanced debugging tools for NLM brain instances with real-time monitoring,
    performance profiling, and detailed logging.
    """
    
    def __init__(self, brain: pynlm.Brain, log_file: Optional[str] = None):
        self.brain = brain
        self.log_file = log_file
        self.performance_history: List[Dict[str, Any]] = []
        self.error_handler = NLMErrors()
        self.breakpoints: List[Dict[str, Any]] = []
        self.monitoring = False
        self.monitor_thread: Optional[threading.Thread] = None
    
    def start_monitoring(self, interval: float = 0.1):
        """Start continuous monitoring in a separate thread"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(interval,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print(f"Monitoring started with interval: {interval}s")
    
    def stop_monitoring(self):
        """Stop monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        print("Monitoring stopped")
    
    def _monitor_loop(self, interval: float):
        """Internal monitoring loop"""
        while self.monitoring:
            self.take_performance_sample()
            time.sleep(interval)
    
    def take_performance_sample(self) -> Dict[str, Any]:
        """Take a performance sample"""
        sample = {
            'timestamp': time.time(),
            'step': self.brain.getTotalSpikeCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'region_count': self.brain.getRegionCount(),
            'memory_usage': self._get_memory_usage(),
            'performance_metrics': self._get_performance_metrics()
        }
        
        self.performance_history.append(sample)
        
        # Check for performance issues
        self._check_performance_issues(sample)
        
        # Check for breakpoints
        self._check_breakpoints(sample)
        
        return sample
    
    def _get_memory_usage(self) -> Dict[str, Any]:
        """Get memory usage information"""
        # This would depend on the actual memory management implementation
        return {
            'brain_objects': 'estimated_based_on_neuron_count',
            'synapse_data': 'estimated_based_on_synapse_count',
            'network_overhead': 'minimal'
        }
    
    def _get_performance_metrics(self) -> Dict[str, Any]:
        """Get performance metrics"""
        if len(self.performance_history) < 2:
            return {'spike_rate': 0.0, 'performance_trend': 'stable'}
        
        recent_samples = self.performance_history[-10:]  # Last 10 samples
        spike_rates = [s['firing_rate'] for s in recent_samples]
        
        # Calculate trend
        if len(spike_rates) >= 2:
            trend = 'increasing' if spike_rates[-1] > spike_rates[0] else \
                   'decreasing' if spike_rates[-1] < spike_rates[0] else 'stable'
        else:
            trend = 'stable'
        
        return {
            'avg_spike_rate': np.mean(spike_rates) if spike_rates else 0.0,
            'std_spike_rate': np.std(spike_rates) if spike_rates else 0.0,
            'min_spike_rate': np.min(spike_rates) if spike_rates else 0.0,
            'max_spike_rate': np.max(spike_rates) if spike_rates else 0.0,
            'performance_trend': trend,
            'samples_analyzed': len(recent_samples)
        }
    
    def _check_performance_issues(self, sample: Dict[str, Any]):
        """Check for performance issues in the sample"""
        metrics = sample['performance_metrics']
        
        # Check for high spike rate variability
        if metrics['std_spike_rate'] > metrics['avg_spike_rate'] * 2:
            self.error_handler.add_error(
                ErrorType.PERFORMANCE_ERROR,
                f"High spike rate variability: std={metrics['std_spike_rate']:.3f}, avg={metrics['avg_spike_rate']:.3f}",
                {'type': 'performance', 'sample': sample},
                recoverable=True
            )
        
        # Check for extremely high spike rate
        if metrics['max_spike_rate'] > 100.0:
            self.error_handler.add_error(
                ErrorType.PERFORMANCE_ERROR,
                f"Extremely high spike rate: {metrics['max_spike_rate']:.3f}",
                {'type': 'performance', 'sample': sample},
                recoverable=False
            )
    
    def add_breakpoint(self, condition: Callable[[Dict[str, Any]], bool],
                      action: Callable[[Dict[str, Any]], None],
                      label: str = ""):
        """Add a breakpoint that triggers when condition is met"""
        breakpoint = {
            'condition': condition,
            'action': action,
            'label': label,
            'created': time.time()
        }
        
        self.breakpoints.append(breakpoint)
    
    def _check_breakpoints(self, sample: Dict[str, Any]):
        """Check all breakpoints against the current sample"""
        for breakpoint in self.breakpoints:
            try:
                if breakpoint['condition'](sample):
                    print(f"Breakpoint triggered: {breakpoint.get('label', 'Unnamed breakpoint')}")
                    breakpoint['action'](sample)
            except Exception as e:
                self.error_handler.add_error(
                    ErrorType.NETWORK_ERROR,
                    f"Breakpoint condition failed: {e}",
                    {'breakpoint': breakpoint, 'sample': sample},
                    recoverable=False
                )
    
    def get_debug_info(self) -> Dict[str, Any]:
        """Get comprehensive debug information"""
        return {
            'performance_samples': len(self.performance_history),
            'error_count': self.error_handler.error_count,
            'last_error': self.error_handler.get_last_error(),
            'breakpoints_count': len(self.breakpoints),
            'monitoring_active': self.monitoring,
            'latest_sample': self.performance_history[-1] if self.performance_history else None,
            'performance_summary': self._get_performance_summary()
        }
    
    def _get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary"""
        if not self.performance_history:
            return {}
        
        all_rates = [s['firing_rate'] for s in self.performance_history]
        
        return {
            'total_samples': len(self.performance_history),
            'avg_firing_rate': np.mean(all_rates),
            'std_firing_rate': np.std(all_rates),
            'min_firing_rate': np.min(all_rates),
            'max_firing_rate': np.max(all_rates),
            'time_range': {
                'start': self.performance_history[0]['timestamp'],
                'end': self.performance_history[-1]['timestamp'],
                'duration': self.performance_history[-1]['timestamp'] - self.performance_history[0]['timestamp']
            },
            'error_summary': self.error_handler.get_summary()
        }

class ExperimentRunner:
    """
    Advanced experiment runner for systematic testing and parameter sweeps.
    """
    
    def __init__(self, base_config: Optional[Dict] = None):
        self.base_config = base_config or {}
        self.experiments: List[Dict[str, Any]] = []
        self.results: List[Dict[str, Any]] = []
        self.parameter_sweeps: List[Dict[str, Any]] = []
    
    def create_parameter_sweep(self, param_name: str, param_values: List[Any],
                              **kwargs) -> 'ExperimentRunner':
        """
        Create a parameter sweep experiment.
        
        Args:
            param_name: Name of the parameter to vary
            param_values: List of values to test
            **kwargs: Additional experiment parameters
            
        Returns:
            ExperimentRunner for method chaining
        """
        sweep = {
            'name': f"sweep_{param_name}",
            'param_name': param_name,
            'param_values': param_values,
            'kwargs': kwargs,
            'created': time.time()
        }
        
        self.parameter_sweeps.append(sweep)
        return self
    
    def create_experiment(self, name: str, config: Dict, 
                         agent_factory: Callable, 
                         simulation_steps: int, **kwargs) -> 'ExperimentRunner':
        """
        Create a standard experiment.
        
        Args:
            name: Experiment name
            config: Configuration dictionary
            agent_factory: Function that creates an agent instance
            simulation_steps: Number of simulation steps
            **kwargs: Additional experiment parameters
            
        Returns:
            ExperimentRunner for method chaining
        """
        experiment = {
            'name': name,
            'config': config,
            'agent_factory': agent_factory,
            'simulation_steps': simulation_steps,
            'kwargs': kwargs,
            'created': time.time()
        }
        
        self.experiments.append(experiment)
        return self
    
    def run_experiment(self, experiment: Dict[str, Any], 
                      progress_callback: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Run a single experiment.
        
        Args:
            experiment: Experiment dictionary
            progress_callback: Optional callback for progress updates
            
        Returns:
            Experiment results
        """
        start_time = time.time()
        experiment_name = experiment['name']
        
        print(f"Running experiment: {experiment_name}")
        
        # Create agent using factory function
        agent = experiment['agent_factory'](experiment['config'])
        
        # Run simulation
        if progress_callback:
            def step_callback(result):
                progress = result['step'] / experiment['simulation_steps']
                progress_callback(progress, result)
            
            result = agent.run_simulation(experiment['simulation_steps'], 
                                         callback=step_callback)
        else:
            result = agent.run_simulation(experiment['simulation_steps'])
        
        end_time = time.time()
        
        experiment_result = {
            'experiment_name': experiment_name,
            'config': experiment['config'],
            'simulation_result': result,
            'start_time': start_time,
            'end_time': end_time,
            'duration': end_time - start_time,
            'completed': True,
            'timestamp': time.time()
        }
        
        self.results.append(experiment_result)
        return experiment_result
    
    def run_parameter_sweep(self, progress_callback: Optional[Callable] = None) -> List[Dict[str, Any]]:
        """
        Run all parameter sweeps.
        
        Args:
            progress_callback: Optional callback for progress updates
            
        Returns:
            List of experiment results
        """
        all_results = []
        
        for sweep in self.parameter_sweeps:
            print(f"Running parameter sweep: {sweep['name']}")
            
            total_params = len(sweep['param_values'])
            for i, param_value in enumerate(sweep['param_values']):
                # Create modified config with parameter value
                config = copy.deepcopy(self.base_config)
                config[sweep['param_name']] = param_value
                config.update(sweep['kwargs'])
                
                # Create experiment
                experiment = {
                    'name': f"{sweep['name']}_{i+1}",
                    'config': config,
                    'agent_factory': self._create_agent_factory(),
                    'simulation_steps': sweep['kwargs'].get('simulation_steps', 100),
                    'kwargs': sweep['kwargs']
                }
                
                # Run experiment
                progress = (i + 1) / total_params if progress_callback else None
                if progress_callback:
                    def cb(p, r):
                        overall_progress = ((i + p) / total_params) * 100
                        progress_callback(overall_progress, {'sweep': sweep['name'], 
                                                           'param_value': param_value, 
                                                           'result': r})
                
                result = self.run_experiment(experiment, progress_callback)
                all_results.append(result)
        
        return all_results
    
    def _create_agent_factory(self) -> Callable:
        """Create a default agent factory"""
        def agent_factory(config: Dict) -> 'NLMAgent':
            return NLMAgent(config, headless=True)
        
        return agent_factory
    
    def get_experiment_summary(self) -> Dict[str, Any]:
        """Get summary of all experiments"""
        total_experiments = len(self.experiments) + len(self.parameter_sweeps)
        completed_experiments = len(self.results)
        
        # Calculate average metrics
        if self.results:
            avg_duration = np.mean([r['duration'] for r in self.results])
            avg_spikes = np.mean([r['simulation_result'].total_spikes for r in self.results])
            avg_firing_rate = np.mean([r['simulation_result'].firing_rate for r in self.results])
        else:
            avg_duration = avg_spikes = avg_firing_rate = 0.0
        
        return {
            'total_experiments': total_experiments,
            'completed_experiments': completed_experiments,
            'success_rate': completed_experiments / max(total_experiments, 1),
            'average_duration': avg_duration,
            'average_spikes': avg_spikes,
            'average_firing_rate': avg_firing_rate,
            'experiments': [exp['name'] for exp in self.experiments],
            'parameter_sweeps': [sweep['name'] for sweep in self.parameter_sweeps],
            'results_count': len(self.results)
        }

class BatchProcessor:
    """
    Batch processing utilities for running multiple experiments in parallel
    and managing large-scale simulations.
    """
    
    def __init__(self, max_workers: Optional[int] = None):
        self.max_workers = max_workers or 4
        self.executor = ThreadPoolExecutor(max_workers=max_workers)
        self.running_jobs: List[Any] = []
        self.completed_jobs: List[Dict[str, Any]] = []
    
    def submit_task(self, task_func: Callable, *args, **kwargs) -> Any:
        """
        Submit a task for execution.
        
        Args:
            task_func: Function to execute
            *args: Positional arguments for the function
            **kwargs: Keyword arguments for the function
            
        Returns:
            Future object for the task
        """
        future = self.executor.submit(task_func, *args, **kwargs)
        self.running_jobs.append(future)
        return future
    
    def wait_for_tasks(self, timeout: Optional[float] = None) -> List[Dict[str, Any]]:
        """
        Wait for all tasks to complete and return results.
        
        Args:
            timeout: Maximum time to wait for tasks
            
        Returns:
            List of task results
        """
        completed = []
        
        for future in self.running_jobs:
            try:
                result = future.result(timeout=timeout)
                completed.append({
                    'result': result,
                    'status': 'completed',
                    'timestamp': time.time()
                })
            except Exception as e:
                completed.append({
                    'error': str(e),
                    'status': 'failed',
                    'timestamp': time.time()
                })
        
        self.completed_jobs.extend(completed)
        self.running_jobs.clear()
        return completed
    
    def run_batch_experiments(self, experiments: List[Dict[str, Any]],
                             progress_callback: Optional[Callable] = None) -> List[Dict[str, Any]]:
        """
        Run a batch of experiments in parallel.
        
        Args:
            experiments: List of experiment configurations
            progress_callback: Optional callback for progress updates
            
        Returns:
            List of experiment results
        """
        print(f"Running {len(experiments)} experiments in parallel...")
        
        # Submit all experiments
        futures = []
        for i, experiment in enumerate(experiments):
            def run_exp(exp):
                runner = ExperimentRunner()
                return runner.run_experiment(exp)
            
            future = self.submit_task(run_exp, experiment)
            futures.append(future)
            
            if progress_callback and (i + 1) % 5 == 0:
                progress = (i + 1) / len(experiments) * 100
                progress_callback(progress, f"Submitted {i + 1}/{len(experiments)} experiments")
        
        # Wait for all experiments to complete
        results = self.wait_for_tasks()
        
        if progress_callback:
            progress_callback(100.0, f"Completed {len(results)} experiments")
        
        return results
    
    def get_batch_status(self) -> Dict[str, Any]:
        """Get batch processing status"""
        return {
            'running_jobs': len(self.running_jobs),
            'completed_jobs': len(self.completed_jobs),
            'max_workers': self.max_workers,
            'executor_state': 'running' if self.running_jobs else 'idle'
        }
    
    def shutdown(self):
        """Shutdown the executor"""
        self.executor.shutdown(wait=True)

# Global instances for convenience
_nlm_errors = NLMErrors()
_advanced_config = AdvancedConfig()

def get_errors() -> NLMErrors:
    """Get the global error handler instance"""
    return _nlm_errors

def get_config() -> AdvancedConfig:
    """Get the global configuration instance"""
    return _advanced_config

def reset_global_state():
    """Reset global state"""
    _nlm_errors.clear()
    _advanced_config = AdvancedConfig()

# Version information
__version__ = "2.0.0"
__author__ = "NLM Advanced Development Team"
__description__ = "Advanced NLM Python Module with enhanced features, debugging tools, and batch processing capabilities"

# Example usage patterns
if __name__ == "__main__":
    # Example 1: Create a simple agent
    print("Example 1: Simple agent creation")
    agent = NLMAgent()
    agent.initialize(world_width=30, world_height=30)
    
    # Example 2: Run a simulation
    print("\nExample 2: Running simulation")
    result = agent.run_simulation(100, dt=0.1)
    print(f"Simulation completed: {result.total_steps} steps, {result.total_spikes} spikes")
    
    # Example 3: Advanced debugging
    print("\nExample 3: Advanced debugging")
    debugger = AdvancedDebugger(agent.brain)
    debugger.start_monitoring(interval=0.5)
    
    # Run a few steps while monitoring
    for i in range(10):
        agent.run_step(0.1)
    
    debug_info = debugger.get_debug_info()
    print(f"Debug info: {len(debug_info['performance_samples'])} samples, "
          f"{debug_info['error_count']} errors")
    
    debugger.stop_monitoring()
    
    # Example 4: Experiment runner
    print("\nExample 4: Experiment runner")
    runner = ExperimentRunner()
    
    def simple_agent_factory(config):
        return NLMAgent(config)
    
    runner.create_experiment(
        name="Test Experiment",
        config={'neuron_count': 100, 'region_count': 2},
        agent_factory=simple_agent_factory,
        simulation_steps=50
    )
    
    # Run experiments
    experiment_results = []
    for exp in runner.experiments:
        result = runner.run_experiment(exp)
        experiment_results.append(result)
    
    summary = runner.get_experiment_summary()
    print(f"Experiment summary: {summary['success_rate']*100:.1f}% success rate")
    
    # Example 5: Batch processing
    print("\nExample 5: Batch processing")
    processor = BatchProcessor(max_workers=2)
    
    experiments = []
    for i in range(3):
        experiments.append({
            'name': f"Batch Experiment {i+1}",
            'config': {'neuron_count': 100 + i*50},
            'agent_factory': simple_agent_factory,
            'simulation_steps': 20
        })
    
    batch_results = processor.run_batch_experiments(experiments)
    print(f"Batch processing completed: {len(batch_results)} experiments processed")
    
    processor.shutdown()
    
    print("\nAll examples completed successfully!")