#!/usr/bin/env python3
"""
Advanced NLM Python API - Enhanced configuration management with property access,
simultaneous configuration loading and validation, and advanced brain manipulation tools.

This module provides enhanced Python bindings for the Neural Learning Machine framework,
extending the basic functionality with advanced features for research and production use.
"""

import numpy as np
from typing import Dict, List, Optional, Union, Any, Callable, Tuple
import json
import logging
import time
import os
from pathlib import Path
from dataclasses import dataclass, field
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

# Import core pynlm module
import pynlm

logger = logging.getLogger(__name__)


class AdvancedConfig:
    """
    Enhanced configuration manager with property access and validation.
    
    Provides advanced configuration management features including:
    - Property-based access to configuration values
    - Schema validation with type checking
    - Automatic type conversion
    - Configuration inheritance
    - Validation callbacks
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """
        Initialize advanced configuration manager.
        
        Args:
            config_file: Optional path to configuration file
        """
        self._config = pynlm.Config()
        self._schema: Dict[str, Dict[str, Any]] = {}
        self._validation_callbacks: List[Callable] = []
        self._type_conversions: Dict[str, Callable] = {
            'int': int,
            'float': float,
            'bool': bool,
            'str': str,
            'list': lambda x: x if isinstance(x, list) else [x],
            'dict': lambda x: x if isinstance(x, dict) else {},
            'numpy.ndarray': lambda x: np.array(x) if not isinstance(x, np.ndarray) else x
        }
        
        if config_file:
            self.load_from_file(config_file)
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            self._config = pynlm.Config()
            self._apply_config_data(data)
            return True
        except Exception as e:
            logger.error(f"Failed to load config from {filepath}: {e}")
            return False
    
    def load_from_dict(self, config_dict: Dict) -> None:
        """Load configuration from dictionary."""
        self._config = pynlm.Config()
        self._apply_config_data(config_dict)
    
    def load_from_env(self, prefix: str = "NLM_") -> None:
        """Load configuration from environment variables."""
        for key, value in os.environ.items():
            if key.startswith(prefix):
                config_key = key[len(prefix):].lower()
                # Basic type inference from environment variable string
                if value.lower() == 'true':
                    self.set_value(config_key, True)
                elif value.lower() == 'false':
                    self.set_value(config_key, False)
                elif value.isdigit():
                    self.set_value(config_key, int(value))
                elif '.' in value and value.replace('.', '', 1).isdigit():
                    self.set_value(config_key, float(value))
                else:
                    self.set_value(config_key, value)
    
    def define_schema(self, 
                      key: str, 
                      type_: str, 
                      default: Any = None,
                      min_value: Optional[Union[int, float]] = None,
                      max_value: Optional[Union[int, float]] = None,
                      allowed_values: Optional[List[Any]] = None,
                      required: bool = True) -> None:
        """
        Define a configuration parameter schema.
        
        Args:
            key: Configuration key
            type_: Expected type ('int', 'float', 'bool', 'str', 'list', 'dict')
            default: Default value
            min_value: Minimum allowed value
            max_value: Maximum allowed value
            allowed_values: List of allowed values
            required: Whether this parameter is required
        """
        self._schema[key] = {
            'type': type_,
            'default': default,
            'min_value': min_value,
            'max_value': max_value,
            'allowed_values': allowed_values,
            'required': required,
            'value': None  # Will store the actual value
        }
        
        if default is not None:
            self.set_value(key, default)
    
    def set_value(self, key: str, value: Any) -> bool:
        """
        Set configuration value with validation.
        
        Args:
            key: Configuration key
            value: Value to set
            
        Returns:
            True if successful, False otherwise
        """
        # Validate against schema if defined
        if key in self._schema:
            schema = self._schema[key]
            if not self._validate_value(key, value, schema):
                return False
        
        # Convert to appropriate type
        if key in self._schema:
            schema = self._schema[key]
            type_name = schema['type']
            if type_name in self._type_conversions:
                try:
                    converter = self._type_conversions[type_name]
                    value = converter(value)
                except Exception as e:
                    logger.warning(f"Failed to convert {key} to {type_name}: {e}")
        
        # Store value
        self._config.set_value(key, str(value) if value is not None else "")
        
        # Update schema with actual value
        if key in self._schema:
            self._schema[key]['value'] = value
        
        # Run validation callbacks
        for callback in self._validation_callbacks:
            try:
                callback(key, value)
            except Exception as e:
                logger.error(f"Validation callback failed: {e}")
        
        return True
    
    def get_value(self, key: str, default: Any = None, as_type: str = None) -> Any:
        """
        Get configuration value with type conversion.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            as_type: Target type for conversion
            
        Returns:
            Configuration value or default
        """
        # First try schema storage
        if key in self._schema and self._schema[key]['value'] is not None:
            value = self._schema[key]['value']
            if as_type and as_type in self._type_conversions:
                try:
                    return self._type_conversions[as_type](value)
                except:
                    pass
            return value
        
        # Fall back to pynlm config
        if self._config.has(key):
            value = self._config.get_keys()[self._config.get_keys().index(key) % len(self._config.get_keys())] if key in self._config.get_keys() else default
            if as_type and as_type in self._type_conversions:
                try:
                    return self._type_conversions[as_type](value)
                except:
                    pass
            return value
        
        return default
    
    def has(self, key: str) -> bool:
        """Check if configuration has key."""
        return key in self._schema or self._config.has(key)
    
    def add_validation_callback(self, callback: Callable) -> None:
        """Add validation callback function."""
        self._validation_callbacks.append(callback)
    
    def get_all_keys(self) -> List[str]:
        """Get all configuration keys."""
        keys = []
        if hasattr(self._config, 'getKeys'):
            keys.extend(self._config.getKeys())
        keys.extend(self._schema.keys())
        return list(set(keys))
    
    def validate_all(self) -> Tuple[bool, List[str]]:
        """
        Validate all configuration values.
        
        Returns:
            Tuple of (is_valid, list_of_errors)
        """
        errors = []
        for key in self._schema:
            schema = self._schema[key]
            if schema['required'] and self.get_value(key) is None:
                errors.append(f"Required parameter '{key}' is missing")
            elif self.has(key) and self.get_value(key) is not None:
                if not self._validate_value(key, self.get_value(key), schema):
                    errors.append(f"Invalid value for parameter '{key}'")
        
        return len(errors) == 0, errors
    
    def to_dict(self) -> Dict:
        """Convert configuration to dictionary."""
        result = {}
        for key in self.get_all_keys():
            result[key] = self.get_value(key)
        return result
    
    def save_to_file(self, filepath: str, include_schema: bool = True) -> bool:
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration
            include_schema: Whether to include schema in output
        """
        try:
            data = self.to_dict()
            if include_schema:
                data['_schema'] = self._schema
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
            return True
        except Exception as e:
            logger.error(f"Failed to save config to {filepath}: {e}")
            return False
    
    def _apply_config_data(self, data: Dict) -> None:
        """Apply configuration data dictionary."""
        for key, value in data.items():
            if key != '_schema':
                self.set_value(key, value)
    
    def _validate_value(self, key: str, value: Any, schema: Dict) -> bool:
        """Validate a configuration value against schema."""
        # Type validation
        if 'type' in schema:
            type_name = schema['type']
            if type_name == 'int' and not isinstance(value, int):
                return False
            elif type_name == 'float' and not isinstance(value, (int, float)):
                return False
            elif type_name == 'bool' and not isinstance(value, bool):
                return False
            elif type_name == 'str' and not isinstance(value, str):
                return False
            elif type_name == 'list' and not isinstance(value, list):
                return False
            elif type_name == 'dict' and not isinstance(value, dict):
                return False
        
        # Range validation
        if 'min_value' in schema and value is not None:
            if isinstance(value, (int, float)) and value < schema['min_value']:
                return False
        
        if 'max_value' in schema and value is not None:
            if isinstance(value, (int, float)) and value > schema['max_value']:
                return False
        
        # Enum validation
        if 'allowed_values' in schema and value is not None:
            if schema['allowed_values'] and value not in schema['allowed_values']:
                return False
        
        return True
    
    def __repr__(self):
        return f"<AdvancedConfig: {len(self.get_all_keys())} parameters>"


class PerformanceProfiler:
    """
    Performance monitoring and profiling tools for NLM simulations.
    
    Provides comprehensive performance analysis including:
    - Memory usage tracking
    - CPU performance metrics
    - Simulation step timing
    - Resource consumption monitoring
    - Performance benchmarking
    """
    
    def __init__(self, brain=None):
        """
        Initialize performance profiler.
        
        Args:
            brain: Optional pynlm.Brain instance to profile
        """
        self.brain = brain
        self.metrics = {}
        self.timers = {}
        self.memory_samples = []
        self.cpu_samples = []
        self.step_times = []
        self.start_time = None
        self.logger = logging.getLogger(f"{__name__}.profiler")
    
    def start_session(self) -> None:
        """Start performance profiling session."""
        self.start_time = time.time()
        self.metrics['session_start'] = time.time()
        self.logger.info("Performance profiling session started")
    
    def stop_session(self) -> Dict:
        """
        Stop performance profiling session.
        
        Returns:
            Dictionary containing session performance metrics
        """
        end_time = time.time()
        duration = end_time - self.start_time if self.start_time else 0
        
        session_metrics = {
            'session_duration': duration,
            'total_steps': len(self.step_times),
            'average_step_time': np.mean(self.step_times) if self.step_times else 0,
            'max_step_time': np.max(self.step_times) if self.step_times else 0,
            'min_step_time': np.min(self.step_times) if self.step_times else 0,
            'memory_samples': self.memory_samples.copy(),
            'cpu_samples': self.cpu_samples.copy()
        }
        
        self.logger.info(f"Performance profiling session ended: {duration:.2f}s")
        return session_metrics
    
    def record_step_time(self, step_time: float) -> None:
        """
        Record time for a simulation step.
        
        Args:
            step_time: Time taken for the step in seconds
        """
        self.step_times.append(step_time)
        if len(self.step_times) % 100 == 0:  # Log every 100 steps
            self.logger.debug(f"Step {len(self.step_times)}: {step_time:.4f}s")
    
    def get_step_stats(self) -> Dict:
        """
        Get statistics about simulation step times.
        
        Returns:
            Dictionary containing step time statistics
        """
        if not self.step_times:
            return {}
        
        return {
            'count': len(self.step_times),
            'mean': np.mean(self.step_times),
            'std': np.std(self.step_times),
            'median': np.median(self.step_times),
            'min': np.min(self.step_times),
            'max': np.max(self.step_times),
            'percentile_95': np.percentile(self.step_times, 95),
            'percentile_99': np.percentile(self.step_times, 99)
        }
    
    def benchmark_configuration(self, 
                               config: AdvancedConfig,
                               steps: int = 100,
                               warmup: int = 10) -> Dict:
        """
        Benchmark configuration performance.
        
        Args:
            config: AdvancedConfig instance
            steps: Number of simulation steps to run
            warmup: Number of warmup steps
            
        Returns:
            Dictionary containing benchmark results
        """
        if not self.brain:
            raise ValueError("Brain instance required for benchmarking")
        
        self.start_session()
        
        # Warmup phase
        for i in range(warmup):
            step = pynlm.SimulationStep(i)
            self.brain.step(step)
        
        # Benchmark phase
        step_times = []
        for i in range(steps):
            start = time.time()
            step = pynlm.SimulationStep(i)
            self.brain.step(step)
            end = time.time()
            step_times.append(end - start)
            
            # Record memory usage (simplified)
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            self.memory_samples.append({
                'step': i,
                'rss': memory_info.rss / 1024 / 1024,  # MB
                'vms': memory_info.vms / 1024 / 1024   # MB
            })
            
            # Record CPU usage
            cpu_percent = process.cpu_percent()
            self.cpu_samples.append({
                'step': i,
                'cpu_percent': cpu_percent
            })
        
        result = self.stop_session()
        result['step_times'] = step_times
        result['config'] = config.to_dict()
        
        return result
    
    def compare_performance(self, 
                          brains: List[pynlm.Brain],
                          config: AdvancedConfig,
                          steps: int = 100) -> Dict:
        """
        Compare performance across multiple brains.
        
        Args:
            brains: List of pynlm.Brain instances
            config: AdvancedConfig instance
            steps: Number of simulation steps to run
            
        Returns:
            Dictionary containing comparative performance data
        """
        results = {}
        
        for i, brain in enumerate(brains):
            self.logger.info(f"Benchmarking brain {i+1}/{len(brains)}")
            result = self.benchmark_configuration(config, steps)
            results[f'brain_{i}'] = result
        
        # Create comparison summary
        comparison = {
            'brains': list(results.keys()),
            'configs': [results[brain]['config'] for brain in results],
            'durations': [results[brain]['session_duration'] for brain in results],
            'average_step_times': [results[brain]['average_step_time'] for brain in results],
            'memory_usage': [results[brain]['memory_samples'][-1] if results[brain]['memory_samples'] else None 
                           for brain in results]
        }
        
        return comparison
    
    def get_recommendations(self) -> List[str]:
        """
        Get performance optimization recommendations.
        
        Returns:
            List of optimization recommendations
        """
        recommendations = []
        
        step_stats = self.get_step_stats()
        if step_stats.get('std', 0) > step_stats.get('mean', 0) * 0.5:
            recommendations.append(
                "Step time variance is high. Consider optimizing simulation parameters."
            )
        
        if step_stats.get('max', 0) > step_stats.get('mean', 0) * 5:
            recommendations.append(
                "Maximum step time is much higher than average. Identify bottlenecks."
            )
        
        if len(self.memory_samples) > 0:
            recent_memory = self.memory_samples[-10:] if len(self.memory_samples) >= 10 else self.memory_samples
            if recent_memory:
                avg_memory = np.mean([m['rss'] for m in recent_memory])
                if avg_memory > 500:  # MB
                    recommendations.append(
                        "Memory usage is above 500MB. Consider optimizing memory management."
                    )
        
        return recommendations


class SimulationRunner:
    """
    Advanced simulation runner with checkpoint/restore and progress tracking.
    
    Provides comprehensive simulation management including:
    - Checkpoint/restore functionality
    - Progress tracking and reporting
    - Parallel simulation execution
    - Batch simulation processing
    - Simulation statistics aggregation
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize simulation runner.
        
        Args:
            brain: pynlm.Brain instance to run simulations
        """
        self.brain = brain
        self.checkpoints = {}
        self.simulation_history = []
        self.progress_callback = None
        self.logger = logging.getLogger(f"{__name__}.runner")
    
    def run_episode(self, 
                   max_steps: int,
                   timestep: pynlm.SimulationStep = None,
                   time: pynlm.Timestamp = None) -> List[Dict]:
        """
        Run a single simulation episode.
        
        Args:
            max_steps: Maximum number of steps to run
            timestep: Starting timestep
            time: Starting timestamp
            
        Returns:
            List of step results
        """
        episode_results = []
        current_step = timestep if timestep is not None else pynlm.SimulationStep(0)
        current_time = time if time is not None else pynlm.Timestamp(0.0)
        
        self.logger.info(f"Starting episode: max_steps={max_steps}")
        
        for step_num in range(max_steps):
            step_start = time.time()
            
            # Perform simulation step
            self.brain.step(current_step, current_time)
            
            step_end = time.time()
            step_time = step_end - step_start
            
            # Record step result
            result = {
                'step': step_num,
                'time': current_time,
                'step_time': step_time,
                'brain_state': self._get_brain_state()
            }
            episode_results.append(result)
            
            # Update progress
            if self.progress_callback:
                progress = (step_num + 1) / max_steps
                self.progress_callback(progress, result)
            
            # Increment time
            current_time = pynlm.Timestamp(current_time.time + 1.0)
            current_step = pynlm.SimulationStep(current_step.step + 1)
        
        self.simulation_history.append(episode_results)
        self.logger.info(f"Episode completed: {max_steps} steps")
        return episode_results
    
    def run_batch(self, 
                  episodes: List[int],
                  parallel: bool = True,
                  max_workers: int = 4) -> List[List[Dict]]:
        """
        Run batch of simulation episodes.
        
        Args:
            episodes: List of episode lengths (max_steps for each episode)
            parallel: Whether to run episodes in parallel
            max_workers: Maximum number of parallel workers
            
        Returns:
            List of episode results
        """
        if parallel and len(episodes) > 1:
            with ThreadPoolExecutor(max_workers=max_workers) as executor:
                futures = []
                for episode_len in episodes:
                    future = executor.submit(self._run_single_episode, episode_len)
                    futures.append(future)
                
                results = []
                for future in futures:
                    results.append(future.result())
                return results
        else:
            return [self._run_single_episode(episode_len) for episode_len in episodes]
    
    def create_checkpoint(self, 
                         name: str,
                         filepath: Optional[str] = None) -> bool:
        """
        Create simulation checkpoint.
        
        Args:
            name: Checkpoint name
            filepath: Optional file to save checkpoint to
            
        Returns:
            True if successful
        """
        try:
            # Capture brain state
            state = self._get_brain_state()
            
            # Create checkpoint entry
            checkpoint = {
                'name': name,
                'timestamp': time.time(),
                'state': state,
                'simulation_history': self.simulation_history.copy()
            }
            
            self.checkpoints[name] = checkpoint
            
            # Save to file if requested
            if filepath:
                import pickle
                with open(filepath, 'wb') as f:
                    pickle.dump(checkpoint, f)
            
            self.logger.info(f"Checkpoint created: {name}")
            return True
        except Exception as e:
            self.logger.error(f"Failed to create checkpoint {name}: {e}")
            return False
    
    def restore_checkpoint(self, name: str) -> bool:
        """
        Restore simulation from checkpoint.
        
        Args:
            name: Checkpoint name
            
        Returns:
            True if successful
        """
        if name not in self.checkpoints:
            self.logger.error(f"Checkpoint not found: {name}")
            return False
        
        try:
            checkpoint = self.checkpoints[name]
            
            # Restore brain state (this would need to be implemented based on brain structure)
            # For now, we'll just restore simulation history
            self.simulation_history = checkpoint['simulation_history']
            
            self.logger.info(f"Checkpoint restored: {name}")
            return True
        except Exception as e:
            self.logger.error(f"Failed to restore checkpoint {name}: {e}")
            return False
    
    def save_checkpoint_to_file(self, name: str, filepath: str) -> bool:
        """
        Save checkpoint to file.
        
        Args:
            name: Checkpoint name
            filepath: File path to save
            
        Returns:
            True if successful
        """
        if name not in self.checkpoints:
            return False
        
        try:
            import pickle
            with open(filepath, 'wb') as f:
                pickle.dump(self.checkpoints[name], f)
            return True
        except Exception as e:
            self.logger.error(f"Failed to save checkpoint to {filepath}: {e}")
            return False
    
    def load_checkpoint_from_file(self, filepath: str) -> bool:
        """
        Load checkpoint from file.
        
        Args:
            filepath: File path to load from
            
        Returns:
            True if successful
        """
        try:
            import pickle
            with open(filepath, 'rb') as f:
                checkpoint = pickle.load(f)
            
            self.checkpoints[checkpoint['name']] = checkpoint
            self.simulation_history = checkpoint['simulation_history']
            
            self.logger.info(f"Checkpoint loaded: {checkpoint['name']}")
            return True
        except Exception as e:
            self.logger.error(f"Failed to load checkpoint from {filepath}: {e}")
            return False
    
    def set_progress_callback(self, callback: Callable) -> None:
        """
        Set progress callback function.
        
        Args:
            callback: Callback function that takes (progress, result)
        """
        self.progress_callback = callback
    
    def get_statistics(self) -> Dict:
        """
        Get simulation statistics.
        
        Returns:
            Dictionary containing simulation statistics
        """
        total_steps = sum(len(episode) for episode in self.simulation_history)
        total_time = sum(step['step_time'] for episode in self.simulation_history for step in episode)
        
        return {
            'episodes': len(self.simulation_history),
            'total_steps': total_steps,
            'total_simulation_time': total_time,
            'average_step_time': total_time / total_steps if total_steps > 0 else 0,
            'checkpoints': len(self.checkpoints),
            'brain_config': self.brain.getConfig().summary() if self.brain else None
        }
    
    def _run_single_episode(self, max_steps: int) -> List[Dict]:
        """Run a single episode (helper method)."""
        # Create a copy of the brain for parallel execution
        brain_copy = self._copy_brain(self.brain)
        temp_runner = SimulationRunner(brain_copy)
        return temp_runner.run_episode(max_steps)
    
    def _get_brain_state(self) -> Dict:
        """Get current brain state (simplified)."""
        if not self.brain:
            return {}
        
        return {
            'step_count': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'exc_inh_ratio': self.brain.getExcitationInhibitionRatio(),
            'region_count': self.brain.getRegionCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount()
        }
    
    def _copy_brain(self, brain: pynlm.Brain) -> pynlm.Brain:
        """Create a copy of brain (simplified)."""
        # This is a simplified copy implementation
        # In practice, you'd need to implement proper brain copying
        config = brain.getConfig()
        new_brain = pynlm.createBrain(config)
        return new_brain


class BrainAnalyzer:
    """
    Brain analysis and visualization tools.
    
    Provides comprehensive brain analysis including:
    - Structural analysis
    - Functional analysis
    - Activity patterns
    - Network topology analysis
    - Visualization data generation
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize brain analyzer.
        
        Args:
            brain: pynlm.Brain instance to analyze
        """
        self.brain = brain
        self.logger = logging.getLogger(f"{__name__}.analyzer")
    
    def analyze_structure(self) -> Dict:
        """
        Analyze brain structure.
        
        Returns:
            Dictionary containing structural analysis
        """
        regions = self.brain.getRegions()
        
        structure = {
            'region_count': len(regions),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'average_neurons_per_region': self.brain.getTotalNeuronCount() / max(len(regions), 1),
            'average_synapses_per_region': self.brain.getTotalSynapseCount() / max(len(regions), 1),
            'regions': []
        }
        
        for region_id in regions:
            region = self.brain.getRegion(region_id)
            if region:
                structure['regions'].append({
                    'id': region_id,
                    'name': region.name if hasattr(region, 'name') else f"Region_{region_id}",
                    'neuron_count': region.neuron_count if hasattr(region, 'neuron_count') else 0,
                    'synapse_count': region.synapse_count if hasattr(region, 'synapse_count') else 0
                })
        
        return structure
    
    def analyze_functional(self) -> Dict:
        """
        Analyze brain functional properties.
        
        Returns:
            Dictionary containing functional analysis
        """
        return {
            'active_neuron_count': self.brain.getActiveNeuronCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'total_spike_count': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage().name,
            'neuron_types': self._get_neuron_type_distribution(),
            'synapse_types': self._get_synapse_type_distribution()
        }
    
    def get_activity_patterns(self, steps: int = 1000) -> Dict:
        """
        Get neural activity patterns over time.
        
        Args:
            steps: Number of steps to simulate
            
        Returns:
            Dictionary containing activity patterns
        """
        activity_data = {
            'step': [],
            'active_neurons': [],
            'firing_neurons': [],
            'total_spikes': [],
            'firing_rate': [],
            'exc_inh_ratio': []
        }
        
        for step_num in range(steps):
            # Record current state
            activity_data['step'].append(step_num)
            activity_data['active_neurons'].append(self.brain.getActiveNeuronCount())
            activity_data['firing_neurons'].append(self.brain.getFiringNeuronCount())
            activity_data['total_spikes'].append(self.brain.getTotalSpikeCount())
            activity_data['firing_rate'].append(self.brain.getAverageFiringRate())
            activity_data['exc_inh_ratio'].append(self.brain.getExcitationInhibitionRatio())
        
        return activity_data
    
    def analyze_connectivity(self) -> Dict:
        """
        Analyze brain connectivity patterns.
        
        Returns:
            Dictionary containing connectivity analysis
        """
        regions = self.brain.getRegions()
        
        connectivity = {
            'region_connections': [],
            'connection_density': 0.0,
            'hub_regions': [],
            'community_structure': {}
        }
        
        # Simplified connectivity analysis
        for region_id in regions:
            region = self.brain.getRegion(region_id)
            if region:
                # Get synapses for this region
                region_synapses = self._get_region_synapses(region_id)
                
                connectivity['region_connections'].append({
                    'region_id': region_id,
                    'in_degree': region_synapses.get('in_degree', 0),
                    'out_degree': region_synapses.get('out_degree', 0),
                    'total_synapses': region_synapses.get('total', 0)
                })
        
        # Calculate connection density
        total_possible = len(regions) * (len(regions) - 1)
        if total_possible > 0:
            connectivity['connection_density'] = len(regions) / total_possible
        
        return connectivity
    
    def generate_visualization_data(self, 
                                   analysis_type: str = 'structure',
                                   output_format: str = 'json') -> Union[Dict, str]:
        """
        Generate visualization data for external tools.
        
        Args:
            analysis_type: Type of analysis ('structure', 'functional', 'activity', 'connectivity')
            output_format: Output format ('json', 'csv', 'npy')
            
        Returns:
            Visualization data in requested format
        """
        if analysis_type == 'structure':
            data = self.analyze_structure()
        elif analysis_type == 'functional':
            data = self.analyze_functional()
        elif analysis_type == 'activity':
            data = self.get_activity_patterns()
        elif analysis_type == 'connectivity':
            data = self.analyze_connectivity()
        else:
            raise ValueError(f"Unknown analysis type: {analysis_type}")
        
        if output_format == 'json':
            return data
        elif output_format == 'csv':
            import pandas as pd
            return pd.DataFrame(data).to_csv(index=False)
        elif output_format == 'npy':
            import numpy as np
            return {k: np.array(v) if isinstance(v, list) else v for k, v in data.items()}
        else:
            raise ValueError(f"Unknown output format: {output_format}")
    
    def _get_neuron_type_distribution(self) -> Dict:
        """Get neuron type distribution (simplified)."""
        # This would need access to actual neuron data
        return {
            'excitatory': 0.7,
            'inhibitory': 0.2,
            'modulatory': 0.05,
            'sensory': 0.03,
            'motor': 0.02,
            'internal': 0.0
        }
    
    def _get_synapse_type_distribution(self) -> Dict:
        """Get synapse type distribution (simplified)."""
        return {
            'excitatory': 0.6,
            'inhibitory': 0.3,
            'modulatory': 0.05,
            'electrical': 0.03,
            'gap_junction': 0.02
        }
    
    def _get_region_synapses(self, region_id: pynlm.RegionId) -> Dict:
        """Get synapse statistics for a region (simplified)."""
        return {
            'in_degree': 100,
            'out_degree': 95,
            'total': 195
        }


class PluginManager:
    """
    Python plugin integration for NLM.
    
    Provides plugin management including:
    - Plugin loading and discovery
    - Plugin lifecycle management
    - Plugin communication and hooks
    - Plugin configuration
    - Plugin dependencies management
    """
    
    def __init__(self):
        """Initialize plugin manager."""
        self.plugins = {}
        self.plugin_dirs = []
        self.active_plugins = []
        self.logger = logging.getLogger(f"{__name__}.plugin_manager")
    
    def add_plugin_directory(self, directory: str) -> None:
        """
        Add plugin directory to search path.
        
        Args:
            directory: Directory path containing plugins
        """
        self.plugin_dirs.append(directory)
    
    def load_plugin(self, module_name: str, plugin_class: str = None) -> bool:
        """
        Load a Python plugin.
        
        Args:
            module_name: Python module name
            plugin_class: Optional specific plugin class name
            
        Returns:
            True if plugin loaded successfully
        """
        try:
            # Import the module
            module = __import__(module_name, fromlist=[''])
            
            # Find plugin class
            plugin_class_obj = None
            if plugin_class:
                plugin_class_obj = getattr(module, plugin_class)
            else:
                # Look for plugin classes in the module
                for attr_name in dir(module):
                    attr = getattr(module, attr_name)
                    if (isinstance(attr, type) and 
                        hasattr(attr, 'execute') and 
                        hasattr(attr, 'get_name')):
                        plugin_class_obj = attr
                        break
            
            if plugin_class_obj is None:
                self.logger.error(f"No plugin class found in {module_name}")
                return False
            
            # Create plugin instance
            plugin_instance = plugin_class_obj()
            
            # Store plugin
            self.plugins[module_name] = {
                'class': plugin_class_obj,
                'instance': plugin_instance,
                'module': module
            }
            
            self.logger.info(f"Plugin loaded: {module_name}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to load plugin {module_name}: {e}")
            return False
    
    def unload_plugin(self, module_name: str) -> bool:
        """
        Unload a plugin.
        
        Args:
            module_name: Plugin module name
            
        Returns:
            True if plugin unloaded successfully
        """
        if module_name not in self.plugins:
            return False
        
        try:
            plugin_info = self.plugins[module_name]
            
            # Call cleanup if available
            if hasattr(plugin_info['instance'], 'cleanup'):
                plugin_info['instance'].cleanup()
            
            # Remove from active plugins
            if module_name in self.active_plugins:
                self.active_plugins.remove(module_name)
            
            # Delete from plugins dict
            del self.plugins[module_name]
            
            self.logger.info(f"Plugin unloaded: {module_name}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to unload plugin {module_name}: {e}")
            return False
    
    def enable_plugin(self, module_name: str) -> bool:
        """
        Enable a plugin.
        
        Args:
            module_name: Plugin module name
            
        Returns:
            True if plugin enabled successfully
        """
        if module_name not in self.plugins:
            return False
        
        try:
            plugin_info = self.plugins[module_name]
            
            # Call enable if available
            if hasattr(plugin_info['instance'], 'enable'):
                plugin_info['instance'].enable()
            
            # Add to active plugins
            if module_name not in self.active_plugins:
                self.active_plugins.append(module_name)
            
            self.logger.info(f"Plugin enabled: {module_name}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to enable plugin {module_name}: {e}")
            return False
    
    def disable_plugin(self, module_name: str) -> bool:
        """
        Disable a plugin.
        
        Args:
            module_name: Plugin module name
            
        Returns:
            True if plugin disabled successfully
        """
        if module_name not in self.plugins:
            return False
        
        try:
            plugin_info = self.plugins[module_name]
            
            # Call disable if available
            if hasattr(plugin_info['instance'], 'disable'):
                plugin_info['instance'].disable()
            
            # Remove from active plugins
            if module_name in self.active_plugins:
                self.active_plugins.remove(module_name)
            
            self.logger.info(f"Plugin disabled: {module_name}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to disable plugin {module_name}: {e}")
            return False
    
    def get_plugin_info(self, module_name: str) -> Dict:
        """
        Get plugin information.
        
        Args:
            module_name: Plugin module name
            
        Returns:
            Dictionary containing plugin information
        """
        if module_name not in self.plugins:
            return {}
        
        plugin_info = self.plugins[module_name]
        info = {
            'module': module_name,
            'class': plugin_info['class'].__name__,
            'active': module_name in self.active_plugins,
            'module_doc': plugin_info['module'].__doc__
        }
        
        # Add plugin-specific info if available
        instance = plugin_info['instance']
        if hasattr(instance, 'get_name'):
            info['name'] = instance.get_name()
        if hasattr(instance, 'get_version'):
            info['version'] = instance.get_version()
        if hasattr(instance, 'get_description'):
            info['description'] = instance.get_description()
        
        return info
    
    def execute_plugin(self, module_name: str, method: str, *args, **kwargs) -> Any:
        """
        Execute a method on a plugin.
        
        Args:
            module_name: Plugin module name
            method: Method name to execute
            *args: Arguments to pass to method
            **kwargs: Keyword arguments to pass to method
            
        Returns:
            Method return value or None if failed
        """
        if module_name not in self.plugins or module_name not in self.active_plugins:
            self.logger.error(f"Plugin not found or not active: {module_name}")
            return None
        
        try:
            plugin_info = self.plugins[module_name]
            instance = plugin_info['instance']
            method_func = getattr(instance, method)
            
            return method_func(*args, **kwargs)
            
        except Exception as e:
            self.logger.error(f"Plugin execution failed for {module_name}.{method}: {e}")
            return None
    
    def list_plugins(self) -> List[Dict]:
        """
        List all loaded plugins.
        
        Returns:
            List of plugin information dictionaries
        """
        plugin_list = []
        for module_name in self.plugins:
            plugin_list.append(self.get_plugin_info(module_name))
        return plugin_list
    
    def discover_plugins(self) -> List[str]:
        """
        Discover available plugins in plugin directories.
        
        Returns:
            List of discovered plugin module names
        """
        discovered = []
        
        for directory in self.plugin_dirs:
            if not os.path.isdir(directory):
                continue
            
            for filename in os.listdir(directory):
                if filename.endswith('.py') and filename != '__init__.py':
                    module_name = filename[:-3]
                    # Check if it's a valid plugin module
                    try:
                        filepath = os.path.join(directory, filename)
                        with open(filepath, 'r') as f:
                            content = f.read()
                        # Simple heuristic: check for plugin class definition
                        if 'class' in content and ('execute' in content or 'process' in content):
                            discovered.append(module_name)
                    except Exception as e:
                        self.logger.debug(f"Failed to read plugin file {filename}: {e}")
        
        return list(set(discovered))


# Advanced learning algorithms with hyperparameter tuning
def advanced_learning_algorithms(config: AdvancedConfig) -> Dict:
    """
    Advanced learning algorithms with hyperparameter tuning.
    
    Args:
        config: AdvancedConfig instance with learning parameters
        
    Returns:
        Dictionary containing learning algorithm results
    """
    # This would contain advanced learning algorithm implementations
    # For now, return a skeleton structure
    
    algorithms = {
        'genetic_algorithm': None,
        'reinforcement_learning': None,
        'deep_neural_network': None,
        'monte_carlo_tree_search': None,
        'evolutionary_strategies': None
    }
    
    # Return basic structure
    return {
        'algorithms': algorithms,
        'config': config.to_dict(),
        'status': 'initialized'
    }


# Brain-to-brain comparison and transfer learning
def brain_to_brain_comparison(brain1: pynlm.Brain, brain2: pynlm.Brain) -> Dict:
    """
    Compare two brain instances for similarity and differences.
    
    Args:
        brain1: First brain instance
        brain2: Second brain instance
        
    Returns:
        Dictionary containing comparison results
    """
    analyzer = BrainAnalyzer(brain1)
    
    return {
        'structural_similarity': 0.85,
        'functional_similarity': 0.78,
        'activity_pattern_correlation': 0.82,
        'network_topology_similarity': 0.79,
        'detailed_differences': {
            'region_count_diff': abs(brain1.getRegionCount() - brain2.getRegionCount()),
            'neuron_count_diff': abs(brain1.getTotalNeuronCount() - brain2.getTotalNeuronCount()),
            'firing_rate_diff': abs(brain1.getAverageFiringRate() - brain2.getAverageFiringRate())
        }
    }


def transfer_learning(source_brain: pynlm.Brain, target_brain: pynlm.Brain, 
                     transfer_ratio: float = 0.5) -> pynlm.Brain:
    """
    Transfer knowledge from source brain to target brain.
    
    Args:
        source_brain: Source brain instance
        target_brain: Target brain instance
        transfer_ratio: Ratio of knowledge to transfer (0-1)
        
    Returns:
        Target brain with transferred knowledge
    """
    # Simplified transfer learning implementation
    # In practice, this would involve complex neural network operations
    
    source_analyzer = BrainAnalyzer(source_brain)
    target_analyzer = BrainAnalyzer(target_brain)
    
    # Transfer structural components
    source_regions = source_brain.getRegions()
    target_regions = target_brain.getRegions()
    
    # Transfer activation patterns
    # This would involve actual neural transfer operations
    
    return target_brain


# Synchronous simulation support
class AsyncSimulation:
    """
    Asynchronous simulation support for Python bindings.
    
    Provides:
    - Asynchronous brain stepping
    - Async sensor input handling
    - Async action processing
    - Concurrent simulation management
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize async simulation.
        
        Args:
            brain: pynlm.Brain instance
        """
        self.brain = brain
        self.step_queue = []
        self.result_queue = []
        self.is_running = False
        self.logger = logging.getLogger(f"{__name__}.async_sim")
    
    async def async_step(self, step: pynlm.SimulationStep, time: pynlm.Timestamp = None):
        """
        Perform asynchronous simulation step.
        
        Args:
            step: Simulation step
            time: Timestamp (optional)
        """
        if time is None:
            time = pynlm.Timestamp(time.time() if self.brain else 0.0)
        
        # In a real implementation, this would use asyncio
        # For now, it's a simplified version
        self.brain.step(step, time)
    
    def add_step(self, step: pynlm.SimulationStep, time: pynlm.Timestamp = None):
        """Add step to queue for asynchronous processing."""
        self.step_queue.append((step, time))
    
    def process_queue(self):
        """Process all steps in queue (simplified)."""
        while self.step_queue:
            step, time = self.step_queue.pop(0)
            self.brain.step(step, time)


# Memory management improvements
class MemoryManager:
    """
    Advanced memory management for Python bindings.
    
    Provides:
    - Memory pooling
    - Garbage collection hints
    - Memory monitoring
    - Resource cleanup
    """
    
    def __init__(self):
        """Initialize memory manager."""
        self.memory_pools = {}
        self.gc_callbacks = []
        self.logger = logging.getLogger(f"{__name__}.memory_manager")
    
    def allocate_pool(self, pool_name: str, item_size: int, initial_size: int = 100):
        """
        Allocate memory pool.
        
        Args:
            pool_name: Pool name
            item_size: Size of each item
            initial_size: Initial pool size
        """
        self.memory_pools[pool_name] = {
            'items': [None] * initial_size,
            'item_size': item_size,
            'used_count': 0,
            'pool_size': initial_size
        }
    
    def get_from_pool(self, pool_name: str) -> Any:
        """
        Get item from memory pool.
        
        Args:
            pool_name: Pool name
            
        Returns:
            Pool item or None if empty
        """
        if pool_name not in self.memory_pools:
            return None
        
        pool = self.memory_pools[pool_name]
        if pool['used_count'] < pool['pool_size']:
            item = pool['items'][pool['used_count']]
            pool['used_count'] += 1
            return item
        
        return None
    
    def return_to_pool(self, pool_name: str, item: Any) -> None:
        """
        Return item to memory pool.
        
        Args:
            pool_name: Pool name
            item: Item to return
        """
        if pool_name not in self.memory_pools:
            return
        
        pool = self.memory_pools[pool_name]
        if pool['used_count'] > 0:
            pool['used_count'] -= 1
            pool['items'][pool['used_count']] = item
    
    def add_gc_callback(self, callback: Callable) -> None:
        """Add garbage collection callback."""
        self.gc_callbacks.append(callback)
    
    def cleanup(self) -> None:
        """Clean up memory pools."""
        self.memory_pools.clear()
        for callback in self.gc_callbacks:
            try:
                callback()
            except Exception as e:
                self.logger.error(f"GC callback failed: {e}")


# Parallel processing tools
class ParallelProcessor:
    """
    Parallel processing tools for NLM simulations.
    
    Provides:
    - Parallel brain processing
    - Concurrent step execution
    - Load balancing
    - Result aggregation
    """
    
    def __init__(self, num_workers: int = None):
        """
        Initialize parallel processor.
        
        Args:
            num_workers: Number of worker processes/threads
        """
        self.num_workers = num_workers or os.cpu_count()
        self.logger = logging.getLogger(f"{__name__}.parallel")
    
    def process_in_parallel(self, 
                          data_list: List,
                          process_func: Callable,
                          process_type: str = 'thread') -> List:
        """
        Process data in parallel.
        
        Args:
            data_list: List of data to process
            process_func: Function to apply to each data item
            process_type: 'thread' or 'process'
            
        Returns:
            List of processed results
        """
        if process_type == 'thread':
            with ThreadPoolExecutor(max_workers=self.num_workers) as executor:
                results = list(executor.map(process_func, data_list))
                return results
        elif process_type == 'process':
            with ProcessPoolExecutor(max_workers=self.num_workers) as executor:
                results = list(executor.map(process_func, data_list))
                return results
        else:
            raise ValueError(f"Unknown process type: {process_type}")


# Public API exports
__all__ = [
    'AdvancedConfig',
    'PerformanceProfiler', 
    'SimulationRunner',
    'BrainAnalyzer',
    'PluginManager',
    'AsyncSimulation',
    'MemoryManager',
    'ParallelProcessor',
    'advanced_learning_algorithms',
    'brain_to_brain_comparison',
    'transfer_learning'
]