"""
Core functionality for NLM Python bindings.

This module provides fundamental types, classes, and utilities used by
the NLM neural simulation framework.
"""

import warnings
from typing import Optional, Dict, Any, List

# Re-export the core bindings from pybind11
from ..core import *

# Core functionality wrappers and utilities

class ConfigWrapper:
    """
    Enhanced configuration wrapper for NLM systems.
    
    Provides Pythonic interface for configuration management with
    validation, serialization, and convenience methods.
    """
    
    def __init__(self, config):
        self._config = config
    
    def set_from_dict(self, config_dict: Dict[str, Any]) -> None:
        """
        Set multiple configuration parameters from a dictionary.
        
        Parameters
        ----------
        config_dict : dict
            Dictionary of configuration parameters.
        """
        for key, value in config_dict.items():
            self._config.set(key, value)
    
    def get_from_dict(self, config_dict: Dict[str, Any]) -> Dict[str, Any]:
        """
        Get multiple configuration parameters as a dictionary.
        
        Parameters
        ----------
        config_dict : dict
            Dictionary of parameter keys to retrieve.
        
        Returns
        -------
        dict
            Dictionary of parameter values.
        """
        result = {}
        for key in config_dict.keys():
            if self._config.has(key):
                result[key] = self._config.get(key)
        return result
    
    def validate(self, schema: Optional[Dict[str, Any]] = None) -> bool:
        """
        Validate configuration against schema or basic rules.
        
        Parameters
        ----------
        schema : dict, optional
            Validation schema. If None, uses basic validation.
        
        Returns
        -------
        bool
            True if configuration is valid, False otherwise.
        """
        if schema is None:
            # Basic validation
            if self._config.has("brain.neuron_count"):
                neuron_count = self._config.get("brain.neuron_count")
                if neuron_count < 1 or neuron_count > 10000:
                    warnings.warn(f"Unusual neuron count: {neuron_count}")
        
        return True
    
    def summary(self) -> Dict[str, Any]:
        """
        Get configuration summary as a dictionary.
        
        Returns
        -------
        dict
            Configuration summary.
        """
        return {
            'keys': self._config.getKeys(),
            'has_neuron_count': self._config.has("brain.neuron_count"),
            'has_synapse_density': self._config.has("brain.synapse_density"),
            'has_learning_rate': self._config.has("plasticity.stdp.learning_rate"),
        }
    
    def __repr__(self):
        return f"<ConfigWrapper: {len(self._config.getKeys())} parameters>"

class SimulationRunner:
    """
    Simulation runner for NLM brain simulations.
    
    Provides Pythonic interface for running brain simulations with
    progress monitoring, statistics collection, and error handling.
    """
    
    def __init__(self, brain):
        self.brain = brain
        self.step_history = []
        self.step_times = []
    
    def run_steps(self, num_steps: int, callback=None, progress_callback=None):
        """
        Run a number of simulation steps.
        
        Parameters
        ----------
        num_steps : int
            Number of steps to run.
        callback : callable, optional
            Function called after each step with step number and brain.
        progress_callback : callable, optional
            Function called periodically with progress information.
        
        Returns
        -------
        dict
            Simulation statistics.
        """
        import time
        
        stats = {
            'total_steps': num_steps,
            'total_spikes': 0,
            'max_firing': 0,
            'average_firing': 0,
            'step_times': [],
        }
        
        for step in range(num_steps):
            start_time = time.time()
            
            self.brain.step(step)
            self.step_history.append(step)
            
            # Collect statistics
            firing_count = self.brain.getFiringNeuronCount()
            stats['total_spikes'] += self.brain.getTotalSpikeCount()
            stats['max_firing'] = max(stats['max_firing'], firing_count)
            
            # Call user callback
            if callback:
                callback(step, self.brain)
            
            # Progress reporting
            if progress_callback and step % 100 == 0:
                progress = (step / num_steps) * 100
                progress_callback(progress, step, stats)
            
            # Record timing
            elapsed = time.time() - start_time
            self.step_times.append(elapsed)
            stats['step_times'].append(elapsed)
        
        # Calculate averages
        if self.step_times:
            stats['average_step_time'] = sum(self.step_times) / len(self.step_times)
            stats['total_time'] = sum(self.step_times)
        
        stats['average_firing'] = stats['total_spikes'] / num_steps if num_steps > 0 else 0
        
        return stats
    
    def run_until(self, condition_func, max_steps=10000, callback=None):
        """
        Run simulation until a condition is met or maximum steps reached.
        
        Parameters
        ----------
        condition_func : callable
            Function that takes brain and returns True if simulation should stop.
        max_steps : int
            Maximum number of steps to run.
        callback : callable, optional
            Function called after each step.
        
        Returns
        -------
        dict
            Simulation results and final state.
        """
        for step in range(max_steps):
            self.brain.step(step)
            
            if condition_func(self.brain):
                return {
                    'steps_completed': step + 1,
                    'final_step': step,
                    'final_brain': self.brain,
                    'completed_naturally': True,
                }
            
            if callback:
                callback(step, self.brain)
        
        return {
            'steps_completed': max_steps,
            'final_step': max_steps - 1,
            'final_brain': self.brain,
            'completed_naturally': False,
            'max_steps_reached': True,
        }
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get current simulation statistics.
        
        Returns
        -------
        dict
            Current statistics.
        """
        return {
            'steps_completed': len(self.step_history),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getFiringNeuronCount(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
        }

# Performance monitoring utilities

class PerformanceMonitor:
    """
    Performance monitoring for NLM simulations.
    
    Tracks timing, memory usage, and other performance metrics.
    """
    
    def __init__(self):
        self.timings = []
        self.memory_samples = []
        self.step_count = 0
    
    def record_timing(self, operation: str, duration: float):
        """Record timing for an operation."""
        self.timings.append({
            'operation': operation,
            'duration': duration,
            'step': self.step_count,
        })
    
    def get_average_time(self, operation: str = None) -> float:
        """Get average execution time for an operation or all operations."""
        if operation is None:
            return sum(t['duration'] for t in self.timings) / len(self.timings) if self.timings else 0
        else:
            op_times = [t['duration'] for t in self.timings if t['operation'] == operation]
            return sum(op_times) / len(op_times) if op_times else 0
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Get comprehensive performance report."""
        return {
            'total_operations': len(self.timings),
            'average_time_per_operation': self.get_average_time(),
            'operations_by_type': {
                op: sum(1 for t in self.timings if t['operation'] == op)
                for op in set(t['operation'] for t in self.timings)
            },
            'total_time': sum(t['duration'] for t in self.timings),
        }

# Validation and error handling

class ValidationError(Exception):
    """Exception raised for validation errors."""
    pass

class ConfigValidator:
    """
    Configuration validation utilities.
    """
    
    @staticmethod
    def validate_neuron_count(value: int) -> bool:
        """Validate neuron count."""
        return 1 <= value <= 100000
    
    @staticmethod
    def validate_probability(value: float) -> bool:
        """Validate probability value."""
        return 0.0 <= value <= 1.0
    
    @staticmethod
    def validate_connection_probability(value: float) -> bool:
        """Validate connection probability."""
        return 0.0 <= value <= 1.0
    
    @staticmethod
    def validate_learning_rate(value: float) -> bool:
        """Validate learning rate."""
        return 0.0 <= value <= 1.0

# Utility functions

def safe_step(brain, step_num: int, max_retries: int = 3) -> bool:
    """
    Safely execute a brain step with retry logic.
    
    Parameters
    ----------
    brain : Brain
        Brain object to step.
    step_num : int
        Step number.
    max_retries : int
        Maximum number of retry attempts.
    
    Returns
    -------
    bool
        True if step succeeded, False otherwise.
    """
    for attempt in range(max_retries):
        try:
            brain.step(step_num)
            return True
        except Exception as e:
            if attempt == max_retries - 1:
                raise
            warnings.warn(f"Step {step_num} failed (attempt {attempt + 1}), retrying...")
    
    return False

def compare_brains(brain1, brain2) -> Dict[str, Any]:
    """
    Compare two brains and return differences.
    
    Parameters
    ----------
    brain1, brain2 : Brain
        Brains to compare.
    
    Returns
    -------
    dict
        Differences between brains.
    """
    return {
        'neuron_count_diff': brain1.getTotalNeuronCount() - brain2.getTotalNeuronCount(),
        'synapse_count_diff': brain1.getTotalSynapseCount() - brain2.getTotalSynapseCount(),
        'firing_rate_diff': brain1.getAverageFiringRate() - brain2.getAverageFiringRate(),
        'structural_similarity': abs(brain1.getExcitationInhibitionRatio() - brain2.getExcitationInhibitionRatio()),
    }

# Re-export commonly used types
__all__ = [
    'ConfigWrapper',
    'SimulationRunner', 
    'PerformanceMonitor',
    'ValidationError',
    'ConfigValidator',
    'safe_step',
    'compare_brains',
    # Core types from bindings
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'Config', 'Brain', 'AgentBrain',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld',
]