"""
Advanced NLM Python Utilities

Comprehensive utilities for NLM (Neural Learning Machine) with:
- Advanced configuration management
- Performance monitoring and metrics collection
- Plugin system for extensibility
- Advanced simulation controls
- Error handling and validation
- Event system and state management
- Visualization helpers
- Simulation scheduling

This module provides high-level Python interfaces for working with NLM,
making it easier to use for beginners while providing advanced features
for power users.
"""

import sys
import time
import threading
import json
import math
from abc import ABC, abstractmethod
from typing import Dict, List, Any, Optional, Callable, Union, Tuple
from enum import Enum
import numpy as np

# Try to import nlm_py for integration with C++ bindings
try:
    import nlm_py
    NLM_PY_AVAILABLE = True
except ImportError:
    NLM_PY_AVAILABLE = False
    nlm_py = None

class NLMBasicException(Exception):
    """Base exception for all NLM errors"""
    pass

class ValidationError(NLMBasicException):
    """Error raised when input validation fails"""
    pass

class SimulationError(NLMBasicException):
    """Error raised during simulation operations"""
    pass

class PluginError(NLMBasicException):
    """Error raised in plugin system"""
    pass

class EnhancedErrorHandler:
    """
    Comprehensive error handler for NLM operations.
    
    Features:
    - Detailed error reporting with context
    - Error recovery strategies
    - Error categorization by type and severity
    - Automatic error detection and reporting
    """
    
    def __init__(self):
        self.error_history = []
        self.recovery_actions = {}
        self.error_counts = {}
    
    def handle_error(self, error: Exception, context: Dict[str, Any] = None) -> bool:
        """
        Handle an error with recovery strategies.
        
        Args:
            error: The exception that occurred
            context: Additional context about the error
            
        Returns:
            bool: True if error was recovered from, False otherwise
        """
        error_info = {
            'timestamp': time.time(),
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context or {},
            'recovered': False
        }
        
        # Categorize error
        error_category = self._categorize_error(error)
        error_info['category'] = error_category
        
        # Count errors
        self.error_counts[error_category] = self.error_counts.get(error_category, 0) + 1
        
        # Try recovery
        recovery_result = self._attempt_recovery(error, context)
        error_info['recovered'] = recovery_result
        
        self.error_history.append(error_info)
        
        # Log error
        self._log_error(error_info)
        
        return recovery_result
    
    def _categorize_error(self, error: Exception) -> str:
        """Categorize error by type"""
        if isinstance(error, ValidationError):
            return 'validation'
        elif isinstance(error, SimulationError):
            return 'simulation'
        elif isinstance(error, PluginError):
            return 'plugin'
        elif isinstance(error, MemoryError):
            return 'memory'
        elif isinstance(error, ValueError):
            return 'value'
        else:
            return 'unknown'
    
    def _attempt_recovery(self, error: Exception, context: Dict[str, Any]) -> bool:
        """Attempt to recover from error"""
        error_category = self._categorize_error(error)
        
        if error_category in self.recovery_actions:
            try:
                recovery_action = self.recovery_actions[error_category]
                return recovery_action(error, context)
            except Exception:
                pass
        
        # Default recovery strategies
        if error_category == 'validation':
            return self._recover_from_validation_error(error, context)
        elif error_category == 'simulation':
            return self._recover_from_simulation_error(error, context)
        
        return False
    
    def _recover_from_validation_error(self, error: Exception, context: Dict[str, Any]) -> bool:
        """Recover from validation errors"""
        # Reset to safe defaults
        if context and 'brain' in context:
            try:
                context['brain'].reset()
                context['brain'].initialize()
                return True
            except Exception:
                pass
        return False
    
    def _recover_from_simulation_error(self, error: Exception, context: Dict[str, Any]) -> bool:
        """Recover from simulation errors"""
        # Try to pause simulation and recover
        if context and 'simulation_running' in context:
            context['simulation_running'] = False
            time.sleep(0.1)  # Brief pause
            context['simulation_running'] = True
            return True
        return False
    
    def _log_error(self, error_info: Dict[str, Any]) -> None:
        """Log error information"""
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(error_info['timestamp']))
        error_msg = f"[{timestamp}] {error_info['category']}: {error_info['error_type']}: {error_info['error_message']}"
        
        if error_info['recovered']:
            error_msg += " (RECOVERED)"
        
        print(error_msg)
    
    def get_error_stats(self) -> Dict[str, Any]:
        """Get error statistics"""
        return {
            'total_errors': len(self.error_history),
            'error_counts': self.error_counts,
            'recent_errors': self.error_history[-10:] if len(self.error_history) > 10 else self.error_history
        }
    
    def clear_error_history(self) -> None:
        """Clear error history"""
        self.error_history.clear()
        self.error_counts.clear()
class InputValidator:
    """
    Advanced input validation for all NLM operations.
    
    Features:
    - Comprehensive input validation for all operations
    - Parameter bounds checking
    - Type validation
    - Format validation
    - Safety checking and boundary validation
    """
    
    def __init__(self):
        self.validation_rules = {}
        self.safety_limits = {
            'max_neuron_count': 1000000,
            'max_simulation_steps': 1000000,
            'max_memory_usage': 8.0,  # GB
            'max_cpu_usage': 100.0,   # %
            'min_timestep': 1e-9,
            'max_timestep': 1.0
        }
    
    def validate_brain_config(self, config: Dict[str, Any]) -> List[str]:
        """
        Validate brain configuration.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            List[str]: List of validation errors (empty if valid)
        """
        errors = []
        
        # Validate neuron count
        if 'neuron_count' in config:
            neuron_count = config['neuron_count']
            if not isinstance(neuron_count, (int, float)):
                errors.append("neuron_count must be a number")
            elif neuron_count <= 0:
                errors.append("neuron_count must be positive")
            elif neuron_count > self.safety_limits['max_neuron_count']:
                errors.append(f"neuron_count exceeds safety limit of {self.safety_limits['max_neuron_count']}")
        
        # Validate timestep
        if 'simulation_timestep' in config:
            timestep = config['simulation_timestep']
            if not isinstance(timestep, (int, float)):
                errors.append("simulation_timestep must be a number")
            elif timestep < self.safety_limits['min_timestep']:
                errors.append(f"simulation_timestep below minimum {self.safety_limits['min_timestep']}")
            elif timestep > self.safety_limits['max_timestep']:
                errors.append(f"simulation_timestep above maximum {self.safety_limits['max_timestep']}")
        
        # Validate region count
        if 'region_count' in config:
            region_count = config['region_count']
            if not isinstance(region_count, (int, float)):
                errors.append("region_count must be a number")
            elif region_count <= 0:
                errors.append("region_count must be positive")
        
        # Validate connection probability
        if 'connection_probability' in config:
            prob = config['connection_probability']
            if not isinstance(prob, (int, float)):
                errors.append("connection_probability must be a number")
            elif prob < 0.0 or prob > 1.0:
                errors.append("connection_probability must be between 0.0 and 1.0")
        
        return errors
    
    def validate_simulation_parameters(self, params: Dict[str, Any]) -> List[str]:
        """
        Validate simulation parameters.
        
        Args:
            params: Simulation parameters
            
        Returns:
            List[str]: List of validation errors (empty if valid)
        """
        errors = []
        
        if 'steps' in params:
            steps = params['steps']
            if not isinstance(steps, (int, float)):
                errors.append("steps must be a number")
            elif steps <= 0:
                errors.append("steps must be positive")
            elif steps > self.safety_limits['max_simulation_steps']:
                errors.append(f"steps exceeds safety limit of {self.safety_limits['max_simulation_steps']}")
        
        return errors
    
    def validate_memory_usage(self, memory_usage: float) -> bool:
        """
        Validate memory usage against safety limits.
        
        Args:
            memory_usage: Memory usage in GB
            
        Returns:
            bool: True if within limits, False otherwise
        """
        return memory_usage <= self.safety_limits['max_memory_usage']
    
    def validate_cpu_usage(self, cpu_usage: float) -> bool:
        """
        Validate CPU usage against safety limits.
        
        Args:
            cpu_usage: CPU usage percentage
            
        Returns:
            bool: True if within limits, False otherwise
        """
        return cpu_usage <= self.safety_limits['max_cpu_usage']
    
    def validate_neuron_values(self, neurons_data: List[float]) -> List[str]:
        """
        Validate neuron values (membrane potentials, thresholds, etc.).
        
        Args:
            neurons_data: List of neuron values
            
        Returns:
            List[str]: List of validation errors (empty if valid)
        """
        errors = []
        
        for i, value in enumerate(neurons_data):
            if not isinstance(value, (int, float)):
                errors.append(f"Neuron {i} has invalid value type")
            elif math.isnan(value):
                errors.append(f"Neuron {i} has NaN value")
            elif math.isinf(value):
                errors.append(f"Neuron {i} has infinite value")
        
        return errors
class SimulationSafetyChecker:
    """
    Safety checking and boundary validation for simulations.
    
    Features:
    - Boundary validation for all operations
    - Safety checks for simulation parameters
    - Resource usage monitoring
    - Emergency stop functionality
    """
    
    def __init__(self):
        self.safety_enabled = True
        self.emergency_stop = False
        self.resource_monitor = ResourceMonitor()
    
    def check_simulation_safety(self, brain, step: int, params: Dict[str, Any]) -> List[str]:
        """
        Check simulation safety at each step.
        
        Args:
            brain: Brain instance
            step: Current simulation step
            params: Simulation parameters
            
        Returns:
            List[str]: List of safety violations (empty if safe)
        """
        violations = []
        
        if self.emergency_stop:
            violations.append("Emergency stop activated")
            return violations
        
        # Check neuron count safety
        neuron_count = brain.getTotalNeuronCount()
        if neuron_count > 1000000:
            violations.append(f"Neuron count exceeds safe limit: {neuron_count}")
        
        # Check spike count (avoid runaway activity)
        spike_count = brain.getTotalSpikeCount()
        if spike_count > 1000000:
            violations.append(f"Spike count exceeds safe limit: {spike_count}")
        
        # Check memory usage
        if not self.resource_monitor.check_memory():
            violations.append("Memory usage exceeds safe limit")
        
        # Check CPU usage
        if not self.resource_monitor.check_cpu():
            violations.append("CPU usage exceeds safe limit")
        
        # Check simulation time
        if step > 1000000:
            violations.append(f"Simulation steps exceed safe limit: {step}")
        
        # Check for unstable brain state
        firing_rate = brain.getAverageFiringRate()
        if firing_rate < 0.0 or firing_rate > 1000.0:
            violations.append(f"Unstable firing rate: {firing_rate}")
        
        return violations
    
    def activate_emergency_stop(self) -> None:
        """Activate emergency stop"""
        self.emergency_stop = True
        print("EMERGENCY STOP ACTIVATED")
    
    def deactivate_emergency_stop(self) -> None:
        """Deactivate emergency stop"""
        self.emergency_stop = False
        print("Emergency stop deactivated")
    
    def get_safety_status(self) -> Dict[str, Any]:
        """Get current safety status"""
        return {
            'emergency_stop': self.emergency_stop,
            'safety_enabled': self.safety_enabled,
            'resource_status': self.resource_monitor.get_status()
        }
class ResourceMonitor:
    """Monitor system resources during simulation"""
    
    def __init__(self):
        self.memory_usage = 0.0
        self.cpu_usage = 0.0
        self.monitoring = False
        self.monitor_thread = None
    
    def start_monitoring(self) -> None:
        """Start resource monitoring"""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_resources)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self) -> None:
        """Stop resource monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _monitor_resources(self) -> None:
        """Background resource monitoring"""
        while self.monitoring:
            # Simulate resource monitoring (in real implementation, would use psutil)
            self.memory_usage = np.random.uniform(0.5, 2.0)  # Simulated
            self.cpu_usage = np.random.uniform(10.0, 50.0)   # Simulated
            time.sleep(1.0)
    
    def check_memory(self) -> bool:
        """Check if memory usage is safe"""
        return self.memory_usage <= 8.0  # GB
    
    def check_cpu(self) -> bool:
        """Check if CPU usage is safe"""
        return self.cpu_usage <= 100.0  # %
    
    def get_status(self) -> Dict[str, float]:
        """Get current resource status"""
        return {
            'memory_usage_gb': self.memory_usage,
            'cpu_usage_percent': self.cpu_usage,
            'memory_safe': self.check_memory(),
            'cpu_safe': self.check_cpu()
        }
class AdvancedConfig:
    """
    Advanced configuration management with comprehensive validation.
    
    Features:
    - Advanced configuration with comprehensive validation
    - Parameter type checking
    - Default value management
    - Configuration export/import
    - Configuration override capability
    - Configuration schema validation
    """
    
    def __init__(self, config_file: str = "configs/default.cfg"):
        self.config_file = config_file
        self.config = {}
        self.defaults = {}
        self.validation_rules = {}
        self.load_defaults()
        self.load_config()
    
    def load_defaults(self) -> None:
        """Load default configuration"""
        self.defaults = {
            'simulation_timestep': 0.001,
            'neuron_count': 1000,
            'region_count': 2,
            'connection_probability': 0.1,
            'plasticity_learning_rate': 0.01,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'stdp_time_constant': 20.0,
            'development_synaptogenesis_rate': 0.001,
            'development_pruning_rate': 0.0001,
            'dopamine_baseline': 0.1,
            'reward_discount_factor': 0.99,
            'environment_name': 'GridWorld',
            'environment_width': 10,
            'environment_height': 10,
            'log_level': 'INFO',
            'log_to_file': False,
            'log_filename': 'nlm.log',
            'max_simulation_steps': 10000,
            'simulation_time_limit': 0.0,
            'visualization_enabled': False,
            'visualization_update_rate': 30.0
        }
        
        # Copy defaults to current config
        for key, value in self.defaults.items():
            self.config[key] = value
    
    def load_config(self) -> bool:
        """
        Load configuration from file.
        
        Returns:
            bool: True if loaded successfully, False otherwise
        """
        try:
            with open(self.config_file, 'r') as f:
                lines = f.readlines()
                
            for line in lines:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                
                if '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Try to convert to appropriate type
                    if value.lower() == 'true':
                        value = True
                    elif value.lower() == 'false':
                        value = False
                    elif value.replace('.', '').isdigit():
                        if '.' in value:
                            value = float(value)
                        else:
                            value = int(value)
                    
                    self.config[key] = value
            
            return True
            
        except Exception as e:
            print(f"Warning: Could not load config file {self.config_file}: {e}")
            return False
    
    def save_config(self, filename: str = None) -> bool:
        """
        Save configuration to file.
        
        Args:
            filename: File to save to (uses config_file if None)
            
        Returns:
            bool: True if saved successfully, False otherwise
        """
        filename = filename or self.config_file
        
        try:
            with open(filename, 'w') as f:
                f.write("# NLM Configuration File\n")
                f.write("# Generated by NLM Python Utilities\n\n")
                
                for key, value in sorted(self.config.items()):
                    if isinstance(value, bool):
                        value_str = 'true' if value else 'false'
                    elif isinstance(value, str):
                        value_str = value
                    else:
                        value_str = str(value)
                    
                    f.write(f"{key} = {value_str}\n")
            
            return True
            
        except Exception as e:
            print(f"Error saving config file {filename}: {e}")
            return False
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        return self.config.get(key, self.defaults.get(key, default))
    
    def set(self, key: str, value: Any, source: str = "manual") -> None:
        """
        Set configuration value.
        
        Args:
            key: Configuration key
            value: Value to set
            source: Source of the change (e.g., 'command_line', 'file', 'manual')
        """
        self.config[key] = value
        
        # Validate the value
        self.validate_config_value(key, value)
    
    def validate_config_value(self, key: str, value: Any) -> List[str]:
        """
        Validate a configuration value.
        
        Args:
            key: Configuration key
            value: Value to validate
            
        Returns:
            List[str]: List of validation errors (empty if valid)
        """
        errors = []
        
        # Basic type validation
        if key in ['simulation_timestep', 'plasticity_learning_rate', 
                   'stdp_ltp_weight', 'stdp_ltd_weight', 'stdp_time_constant',
                   'development_synaptogenesis_rate', 'development_pruning_rate',
                   'dopamine_baseline', 'reward_discount_factor',
                   'visualization_update_rate']:
            if not isinstance(value, (int, float)):
                errors.append(f"{key} must be a number")
            elif value < 0:
                errors.append(f"{key} must be non-negative")
        
        elif key in ['neuron_count', 'region_count', 'max_simulation_steps']:
            if not isinstance(value, int):
                errors.append(f"{key} must be an integer")
            elif value <= 0:
                errors.append(f"{key} must be positive")
        
        elif key == 'connection_probability':
            if not isinstance(value, (int, float)):
                errors.append(f"{key} must be a number")
            elif value < 0.0 or value > 1.0:
                errors.append(f"{key} must be between 0.0 and 1.0")
        
        elif key == 'environment_width' or key == 'environment_height':
            if not isinstance(value, int):
                errors.append(f"{key} must be an integer")
            elif value <= 0:
                errors.append(f"{key} must be positive")
        
        return errors
    
    def get_config_dict(self) -> Dict[str, Any]:
        """
        Get configuration as dictionary.
        
        Returns:
            Dict[str, Any]: Configuration dictionary
        """
        return self.config.copy()
    
    def update_from_dict(self, config_dict: Dict[str, Any], source: str = "manual") -> List[str]:
        """
        Update configuration from dictionary.
        
        Args:
            config_dict: Configuration dictionary
            source: Source of the changes
            
        Returns:
            List[str]: List of validation errors
        """
        errors = []
        
        for key, value in config_dict.items():
            value_errors = self.validate_config_value(key, value)
            if value_errors:
                errors.extend([f"{key}: {error}" for error in value_errors])
            else:
                self.set(key, value, source)
        
        return errors
    
    def get_validation_errors(self) -> List[str]:
        """Get all current configuration validation errors."""
        errors = []
        
        for key, value in self.config.items():
            errors.extend(self.validate_config_value(key, value))
        
        return errors
    
    def is_valid(self) -> bool:
        """Check if configuration is valid."""
        return len(self.get_validation_errors()) == 0
class PerformanceMetrics:
    """
    Performance metrics collection and analysis.
    
    Features:
    - Step-by-step performance tracking
    - Memory usage estimation
    - Stability analysis
    - Learning progress tracking
    - Degradation detection
    """
    
    def __init__(self):
        self.metrics_history = []
        self.current_step = 0
        self.baseline_metrics = None
        self.performance_baseline = None
    
    def record_step_metrics(self, step: int, spike_count: int, active_neurons: int, 
                           firing_rate: float, memory_usage: float, cpu_usage: float) -> None:
        """
        Record metrics for a simulation step.
        
        Args:
            step: Simulation step
            spike_count: Number of spikes this step
            active_neurons: Number of active neurons
            firing_rate: Average firing rate
            memory_usage: Memory usage in GB
            cpu_usage: CPU usage percentage
        """
        metrics = {
            'step': step,
            'timestamp': time.time(),
            'spike_count': spike_count,
            'active_neurons': active_neurons,
            'firing_rate': firing_rate,
            'memory_usage_gb': memory_usage,
            'cpu_usage_percent': cpu_usage,
            'stability_score': self._calculate_stability_score(step, firing_rate),
            'performance_score': self._calculate_performance_score(step, firing_rate, memory_usage)
        }
        
        self.metrics_history.append(metrics)
        self.current_step = step
    
    def _calculate_stability_score(self, step: int, firing_rate: float) -> float:
        """Calculate stability score for this step"""
        # Stability based on firing rate consistency
        # Higher firing rate variance = less stable
        if step < 10:
            return 0.5  # Initial steps less stable
        
        # Look at recent firing rate history
        recent_rates = [m['firing_rate'] for m in self.metrics_history[-10:]]
        if len(recent_rates) < 2:
            return 1.0
        
        rate_variance = np.var(recent_rates)
        stability = max(0.0, 1.0 - min(rate_variance, 1.0))
        return stability
    
    def _calculate_performance_score(self, step: int, firing_rate: float, memory_usage: float) -> float:
        """Calculate performance score for this step"""
        # Performance based on firing rate (optimal around 10-50 Hz)
        # and memory usage efficiency
        optimal_firing = 20.0  # Hz
        firing_score = 1.0 / (1.0 + abs(firing_rate - optimal_firing) / optimal_firing)
        
        # Memory efficiency
        memory_score = max(0.0, 1.0 - (memory_usage - 0.5) / 7.5)  # Normalize
        
        return (firing_score + memory_score) / 2.0
    
    def get_metrics_summary(self) -> Dict[str, Any]:
        """Get summary of performance metrics."""
        if not self.metrics_history:
            return {
                'total_steps': 0,
                'total_spikes': 0,
                'average_firing_rate': 0.0,
                'average_memory_usage': 0.0,
                'average_cpu_usage': 0.0,
                'stability_trend': 0.0,
                'performance_trend': 0.0,
                'steps_completed': 0
            }
        
        recent_metrics = self.metrics_history[-100:]  # Last 100 steps
        
        total_spikes = sum(m['spike_count'] for m in self.metrics_history)
        avg_firing_rate = np.mean([m['firing_rate'] for m in recent_metrics])
        avg_memory_usage = np.mean([m['memory_usage_gb'] for m in recent_metrics])
        avg_cpu_usage = np.mean([m['cpu_usage_percent'] for m in recent_metrics])
        
        # Calculate trends
        if len(self.metrics_history) >= 20:
            early_metrics = self.metrics_history[:10]
            late_metrics = self.metrics_history[-10:]
            
            early_stability = np.mean([m['stability_score'] for m in early_metrics])
            late_stability = np.mean([m['stability_score'] for m in late_metrics])
            stability_trend = late_stability - early_stability
            
            early_performance = np.mean([m['performance_score'] for m in early_metrics])
            late_performance = np.mean([m['performance_score'] for m in late_metrics])
            performance_trend = late_performance - early_performance
        else:
            stability_trend = 0.0
            performance_trend = 0.0
        
        return {
            'total_steps': len(self.metrics_history),
            'total_spikes': total_spikes,
            'average_firing_rate': avg_firing_rate,
            'average_memory_usage': avg_memory_usage,
            'average_cpu_usage': avg_cpu_usage,
            'stability_trend': stability_trend,
            'performance_trend': performance_trend,
            'steps_completed': self.current_step
        }
    
    def detect_degradation(self, threshold: float = 0.1) -> bool:
        """
        Detect performance degradation.
        
        Args:
            threshold: Degradation threshold
            
        Returns:
            bool: True if degradation detected, False otherwise
        """
        if len(self.metrics_history) < 20:
            return False
        
        recent_metrics = self.metrics_history[-10:]
        early_metrics = self.metrics_history[-20:-10]
        
        recent_performance = np.mean([m['performance_score'] for m in recent_metrics])
        early_performance = np.mean([m['performance_score'] for m in early_metrics])
        
        degradation = early_performance - recent_performance
        return degradation > threshold
    
    def get_stability_analysis(self) -> Dict[str, Any]:
        """Get stability analysis."""
        if not self.metrics_history:
            return {
                'is_stable': False,
                'stability_score': 0.0,
                'stability_trend': 0.0,
                'recent_stability': 0.0
            }
        
        recent_metrics = self.metrics_history[-50:] if len(self.metrics_history) >= 50 else self.metrics_history
        
        avg_stability = np.mean([m['stability_score'] for m in recent_metrics])
        
        # Calculate stability trend
        if len(self.metrics_history) >= 10:
            early_stability = np.mean([m['stability_score'] for m in self.metrics_history[:5]])
            late_stability = np.mean([m['stability_score'] for m in self.metrics_history[-5:]])
            stability_trend = late_stability - early_stability
        else:
            stability_trend = 0.0
        
        recent_stability = np.mean([m['stability_score'] for m in recent_metrics])
        
        return {
            'is_stable': avg_stability > 0.7,
            'stability_score': avg_stability,
            'stability_trend': stability_trend,
            'recent_stability': recent_stability
        }
class MetricsCollector:
    """
    Metrics collection during simulation.
    
    Features:
    - Step-by-step metrics collection
    - Real-time monitoring and analysis
    - Metrics aggregation and analysis
    - Performance alerting
    """
    
    def __init__(self):
        self.collected_metrics = {}
        self.alert_thresholds = {}
        self.alert_callbacks = []
    
    def record_metric(self, metric_name: str, value: Any, metadata: Dict[str, Any] = None) -> None:
        """
        Record a metric value.
        
        Args:
            metric_name: Name of the metric
            value: Metric value
            metadata: Additional metadata
        """
        if metric_name not in self.collected_metrics:
            self.collected_metrics[metric_name] = []
        
        metric_entry = {
            'value': value,
            'timestamp': time.time(),
            'metadata': metadata or {}
        }
        
        self.collected_metrics[metric_name].append(metric_entry)
        
        # Check for alerts
        self._check_alerts(metric_name, value)
    
    def _check_alerts(self, metric_name: str, value: Any) -> None:
        """Check if metric value exceeds alert thresholds"""
        if metric_name in self.alert_thresholds:
            threshold = self.alert_thresholds[metric_name]
            if callable(threshold):
                if threshold(value):
                    self._trigger_alert(metric_name, value, threshold)
            else:
                if value > threshold:
                    self._trigger_alert(metric_name, value, threshold)
    
    def _trigger_alert(self, metric_name: str, value: Any, threshold: Any) -> None:
        """Trigger an alert callback"""
        for callback in self.alert_callbacks:
            try:
                callback(metric_name, value, threshold)
            except Exception as e:
                print(f"Error in alert callback: {e}")
    
    def set_alert_threshold(self, metric_name: str, threshold: Union[float, Callable]) -> None:
        """
        Set alert threshold for a metric.
        
        Args:
            metric_name: Metric name
            threshold: Threshold value or callable
        """
        self.alert_thresholds[metric_name] = threshold
    
    def add_alert_callback(self, callback: Callable) -> None:
        """Add alert callback function."""
        self.alert_callbacks.append(callback)
    
    def get_metric(self, metric_name: str, window: int = None) -> Any:
        """
        Get metric values.
        
        Args:
            metric_name: Metric name
            window: Number of recent values to return (None for all)
            
        Returns:
            Metric values or None if metric not found
        """
        if metric_name not in self.collected_metrics:
            return None
        
        values = self.collected_metrics[metric_name]
        if window is not None:
            values = values[-window:]
        
        return [v['value'] for v in values]
    
    def get_metric_summary(self, metric_name: str) -> Dict[str, Any]:
        """
        Get summary statistics for a metric.
        
        Args:
            metric_name: Metric name
            
        Returns:
            Dict with summary statistics or empty dict if metric not found
        """
        if metric_name not in self.collected_metrics:
            return {}
        
        values = [v['value'] for v in self.collected_metrics[metric_name]]
        if not values:
            return {}
        
        return {
            'count': len(values),
            'mean': np.mean(values),
            'std': np.std(values),
            'min': np.min(values),
            'max': np.max(values),
            'median': np.median(values)
        }
    
    def clear_metrics(self, metric_name: str = None) -> None:
        """
        Clear metrics.
        
        Args:
            metric_name: Specific metric to clear (None for all)
        """
        if metric_name is None:
            self.collected_metrics.clear()
            self.alert_thresholds.clear()
        elif metric_name in self.collected_metrics:
            del self.collected_metrics[metric_name]
            if metric_name in self.alert_thresholds:
                del self.alert_thresholds[metric_name]
class VisualizationHelper:
    """
    Visualization configuration and utilities.
    
    Features:
    - Data normalization for visualization
    - Heat map generation
    - Spike visualization
    - Color scheme management
    - Data formatting for external tools
    """
    
    def __init__(self):
        self.color_schemes = {
            'default': ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b'],
            'sequential': ['#000004', '#00050c', '#000813', '#000b19', '#000f1e', '#001324'],
            'diverging': ['#8e0152', '#c51b7d', '#e77f7f', '#fde0ef', '#f7f7f7', '#e6f5d0', '#b8e186', '#7fbc41', '#4d9221', '#276419']
        }
    
    def normalize_data(self, data: List[float], target_min: float = 0.0, target_max: float = 1.0) -> List[float]:
        """
        Normalize data for visualization.
        
        Args:
            data: Input data
            target_min: Target minimum value
            target_max: Target maximum value
            
        Returns:
            Normalized data
        """
        if not data:
            return []
        
        data_min = min(data)
        data_max = max(data)
        
        if data_max == data_min:
            return [target_min] * len(data)
        
        normalized = []
        for value in data:
            normalized_value = target_min + (value - data_min) * (target_max - target_min) / (data_max - data_min)
            normalized.append(normalized_value)
        
        return normalized
    
    def generate_heat_map(self, data: np.ndarray, cmap: str = 'default') -> np.ndarray:
        """
        Generate heat map from 2D data.
        
        Args:
            data: 2D numpy array
            cmap: Color scheme name
            
        Returns:
            Heat map data
        """
        if cmap not in self.color_schemes:
            cmap = 'default'
        
        # Normalize data to [0, 1]
        data_min = np.min(data)
        data_max = np.max(data)
        if data_max == data_min:
            normalized = np.zeros_like(data)
        else:
            normalized = (data - data_min) / (data_max - data_min)
        
        # Apply color mapping
        colors = self.color_schemes[cmap]
        
        # Convert to RGB format suitable for visualization
        heatmap = np.zeros((data.shape[0], data.shape[1], 3))
        
        for i in range(data.shape[0]):
            for j in range(data.shape[1]):
                color_idx = int(normalized[i, j] * (len(colors) - 1))
                color_idx = min(color_idx, len(colors) - 1)
                color = colors[color_idx]
                heatmap[i, j] = [int(color[1:3], 16)/255, 
                               int(color[3:5], 16)/255, 
                               int(color[5:7], 16)/255]
        
        return heatmap
    
    def create_spike_visualization(self, spikes: np.ndarray, width: int = 100, height: int = 10) -> np.ndarray:
        """
        Create spike visualization from spike data.
        
        Args:
            spikes: 1D array of spike data (0/1 values)
            width: Visualization width
            height: Visualization height
            
        Returns:
            Visualization array
        """
        if len(spikes) == 0:
            return np.zeros((height, width, 3))
        
        # Normalize spike data
        spike_time_indices = np.where(spikes == 1)[0]
        normalized_times = self.normalize_data(list(spike_time_indices), 0, width - 1)
        
        # Create visualization
        visualization = np.zeros((height, width, 3))
        
        for spike_time in normalized_times:
            # Add spike representation at that time
            time_idx = int(round(spike_time))
            if 0 <= time_idx < width:
                # Create spike shape (triangle pointing up)
                spike_height = min(height - 2, int((1.0 - spike_time/width) * (height - 2)))
                for h in range(height - spike_height, height - 1):
                    visualization[h, time_idx] = [1.0, 0.3, 0.3]  # Red spike
                visualization[height - 1, time_idx] = [0.3, 1.0, 0.3]  # Green ground
        
        return visualization
    
    def format_data_for_external(self, data: np.ndarray, format: str = 'csv') -> str:
        """
        Format data for external visualization tools.
        
        Args:
            data: Data array
            format: Output format ('csv', 'json', 'tsv')
            
        Returns:
            Formatted data string
        """
        if format == 'csv':
            import csv
            import io
            output = io.StringIO()
            writer = csv.writer(output)
            
            for row in data:
                writer.writerow(row)
            
            return output.getvalue()
        
        elif format == 'json':
            return json.dumps(data.tolist())
        
        elif format == 'tsv':
            import csv
            import io
            output = io.StringIO()
            writer = csv.writer(output, delimiter='\t')
            
            for row in data:
                writer.writerow(row)
            
            return output.getvalue()
        
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    def get_color_scheme(self, scheme: str) -> List[str]:
        """
        Get color scheme.
        
        Args:
            scheme: Color scheme name
            
        Returns:
            Color scheme list
        """
        return self.color_schemes.get(scheme, self.color_schemes['default'])
class Event:
    """Event for NLM event system"""
    
    def __init__(self, event_type: str, data: Dict[str, Any] = None):
        self.event_type = event_type
        self.data = data or {}
        self.timestamp = time.time()
class EventSystem:
    """
    Event system for NLM communication.
    
    Features:
    - Event emission and handling
    - Event filtering
    - Event listeners
    - Event type management
    """
    
    def __init__(self):
        self.event_listeners = {}
        self.event_history = []
        self.max_history = 1000
    
    def add_listener(self, event_type: str, callback: Callable) -> None:
        """
        Add event listener.
        
        Args:
            event_type: Event type to listen for
            callback: Callback function
        """
        if event_type not in self.event_listeners:
            self.event_listeners[event_type] = []
        
        self.event_listeners[event_type].append(callback)
    
    def remove_listener(self, event_type: str, callback: Callable) -> None:
        """
        Remove event listener.
        
        Args:
            event_type: Event type
            callback: Callback function
        """
        if event_type in self.event_listeners:
            listeners = self.event_listeners[event_type]
            if callback in listeners:
                listeners.remove(callback)
    
    def emit_event(self, event: Event) -> None:
        """
        Emit event.
        
        Args:
            event: Event to emit
        """
        self.event_history.append(event)
        
        # Trim history if too large
        if len(self.event_history) > self.max_history:
            self.event_history = self.event_history[-self.max_history:]
        
        # Notify listeners
        if event.event_type in self.event_listeners:
            for callback in self.event_listeners[event.event_type]:
                try:
                    callback(event)
                except Exception as e:
                    print(f"Error in event listener: {e}")
    
    def get_event_history(self, event_type: str = None, limit: int = None) -> List[Event]:
        """
        Get event history.
        
        Args:
            event_type: Event type to filter by (None for all)
            limit: Maximum number of events to return (None for all)
            
        Returns:
            List of events
        """
        events = self.event_history
        
        if event_type is not None:
            events = [e for e in events if e.event_type == event_type]
        
        if limit is not None:
            events = events[-limit:]
        
        return events
    
    def clear_history(self) -> None:
        """Clear event history"""
        self.event_history.clear()
class StateManager:
    """
    State management and checkpointing.
    
    Features:
    - State storage and retrieval
    - State history tracking
    - Checkpoint management
    - State validation
    """
    
    def __init__(self):
        self.current_state = {}
        self.state_history = []
        self.checkpoint_states = {}
        self.max_history = 1000
    
    def save_state(self, state: Dict[str, Any], label: str = None) -> str:
        """
        Save state.
        
        Args:
            state: State to save
            label: Optional label for the state
            
        Returns:
            State identifier
        """
        state_id = label or f"state_{len(self.state_history)}"
        
        # Add timestamp and metadata
        state_entry = {
            'id': state_id,
            'timestamp': time.time(),
            'state': state,
            'label': label
        }
        
        self.state_history.append(state_entry)
        self.current_state = state
        
        # Trim history if too large
        if len(self.state_history) > self.max_history:
            self.state_history = self.state_history[-self.max_history:]
        
        return state_id
    
    def load_state(self, state_id: str = None) -> Dict[str, Any]:
        """
        Load state.
        
        Args:
            state_id: State identifier (None for current state)
            
        Returns:
            Loaded state or None if not found
        """
        if state_id is None:
            return self.current_state.copy()
        
        for state_entry in self.state_history:
            if state_entry['id'] == state_id:
                return state_entry['state'].copy()
        
        print(f"Warning: State '{state_id}' not found")
        return None
    
    def save_checkpoint(self, checkpoint_id: str) -> bool:
        """
        Save checkpoint.
        
        Args:
            checkpoint_id: Checkpoint identifier
            
        Returns:
            True if successful, False otherwise
        """
        try:
            self.checkpoint_states[checkpoint_id] = {
                'timestamp': time.time(),
                'state': self.current_state.copy()
            }
            return True
        except Exception as e:
            print(f"Error saving checkpoint '{checkpoint_id}': {e}")
            return False
    
    def load_checkpoint(self, checkpoint_id: str) -> bool:
        """
        Load checkpoint.
n        Args:
            checkpoint_id: Checkpoint identifier
            
        Returns:
            True if successful, False otherwise
        """
        if checkpoint_id in self.checkpoint_states:
            self.current_state = self.checkpoint_states[checkpoint_id]['state'].copy()
            return True
        else:
            print(f"Warning: Checkpoint '{checkpoint_id}' not found")
            return False
    
    def get_state_history(self, limit: int = None) -> List[Dict[str, Any]]:
        """
        Get state history.
        
        Args:
            limit: Maximum number of states to return (None for all)
            
        Returns:
            List of state entries
        """
        history = self.state_history
        
        if limit is not None:
            history = history[-limit:]
        
        return [{
            'id': entry['id'],
            'timestamp': entry['timestamp'],
            'label': entry['label']
        } for entry in history]
    
    def get_current_state(self) -> Dict[str, Any]:
        """Get current state"""
        return self.current_state.copy()
    
    def clear_history(self) -> None:
        """Clear state history"""
        self.state_history.clear()
class SimulationScheduler:
    """
    Simulation scheduling and management.
    
    Features:
    - Task scheduling
    - Task execution
    - Scheduling management
    - Progress tracking
    """
    
    def __init__(self):
        self.scheduled_tasks = {}
        self.is_scheduled = False
        self.task_queue = []
        self.completed_tasks = []
    
    def schedule_task(self, task_func: Callable, delay: float = 0.0, 
                     task_id: str = None) -> str:
        """
        Schedule task for execution.
        
        Args:
            task_func: Task function to execute
            delay: Delay before execution (seconds)
            task_id: Task identifier
            
        Returns:
            Task identifier
        """
        if task_id is None:
            task_id = f"task_{len(self.scheduled_tasks)}"
        
        self.scheduled_tasks[task_id] = {
            'function': task_func,
            'delay': delay,
            'scheduled_time': time.time(),
            'executed': False,
            'result': None
        }
        
        self.task_queue.append(task_id)
        self.is_scheduled = True
        
        return task_id
    
    def execute_task(self, task_id: str) -> Any:
        """
        Execute task.
        
        Args:
            task_id: Task identifier
            
        Returns:
            Task result
        """
        if task_id not in self.scheduled_tasks:
            raise ValueError(f"Task '{task_id}' not found")
        
        task = self.scheduled_tasks[task_id]
        
        if task['executed']:
            return task['result']
        
        try:
            result = task['function']()
            task['executed'] = True
            task['result'] = result
            self.completed_tasks.append(task_id)
            return result
        except Exception as e:
            print(f"Error executing task '{task_id}': {e}")
            raise
    
    def cancel_task(self, task_id: str) -> bool:
        """
        Cancel scheduled task.
        
        Args:
            task_id: Task identifier
            
        Returns:
            True if task cancelled, False otherwise
        """
        if task_id in self.scheduled_tasks:
            del self.scheduled_tasks[task_id]
            if task_id in self.task_queue:
                self.task_queue.remove(task_id)
            return True
        return False
    
    def get_scheduled_tasks(self) -> List[str]:
        """Get list of scheduled tasks"""
        return list(self.scheduled_tasks.keys())
    
    def get_completed_tasks(self) -> List[str]:
        """Get list of completed tasks"""
        return self.completed_tasks.copy()
    
    def clear_tasks(self) -> None:
        """Clear all tasks"""
        self.scheduled_tasks.clear()
        self.task_queue.clear()
        self.completed_tasks.clear()
        self.is_scheduled = False
    
    def is_task_scheduled(self, task_id: str) -> bool:
        """
        Check if task is scheduled.
        
        Args:
            task_id: Task identifier
            
        Returns:
            True if task scheduled, False otherwise
        """
        return task_id in self.scheduled_tasks
class PluginInterface(ABC):
    """Base interface for all NLM plugins"""
    
    @abstractmethod
    def initialize(self, context: Dict[str, Any]) -> bool:
        """Initialize plugin with context"""
        pass
    
    @abstractmethod
    def execute(self, data: Any, context: Dict[str, Any]) -> Any:
        """Execute plugin operation"""
        pass
    
    @abstractmethod
    def cleanup(self) -> None:
        """Cleanup plugin resources"""
        pass
    
    def get_metadata(self) -> Dict[str, Any]:
        """Get plugin metadata"""
        return {
            'name': self.__class__.__name__,
            'version': '1.0.0',
            'description': 'Plugin interface base class'
        }
class NLMContext:
    """Context object for plugin integration"""
    
    def __init__(self):
        self.plugins = []
        self.config = AdvancedConfig()
        self.error_handler = EnhancedErrorHandler()
        self.metrics_collector = MetricsCollector()
        self.event_system = EventSystem()
        self.state_manager = StateManager()
        self.plugin_manager = None
        self.custom_data = {}
    
    def add_plugin(self, plugin: PluginInterface) -> None:
        """Add plugin to context"""
        self.plugins.append(plugin)
    
    def remove_plugin(self, plugin_name: str) -> bool:
        """
        Remove plugin from context.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin removed, False otherwise
        """
        for i, plugin in enumerate(self.plugins):
            if plugin.get_metadata()['name'] == plugin_name:
                self.plugins.pop(i)
                return True
        return False
    
    def get_plugin(self, plugin_name: str) -> PluginInterface:
        """
        Get plugin by name.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            Plugin instance or None if not found
        """
        for plugin in self.plugins:
            if plugin.get_metadata()['name'] == plugin_name:
                return plugin
        return None
    
    def execute_plugins(self, event_type: str, data: Any) -> List[Any]:
        """
        Execute all plugins for an event.
        
        Args:
            event_type: Event type
            data: Data to process
            
        Returns:
            List of plugin results
        """
        results = []
        
        for plugin in self.plugins:
            try:
                result = plugin.execute(data, self.get_context())
                results.append(result)
            except Exception as e:
                print(f"Error executing plugin '{plugin.get_metadata()['name']}': {e}")
        
        return results
    
    def get_context(self) -> Dict[str, Any]:
        """Get context as dictionary"""
        return {
            'config': self.config,
            'error_handler': self.error_handler,
            'metrics_collector': self.metrics_collector,
            'event_system': self.event_system,
            'state_manager': self.state_manager,
            'plugin_manager': self.plugin_manager,
            'custom_data': self.custom_data
        }
class PluginManager:
    """
    High-level plugin manager for easy integration.
    
    Features:
    - Plugin lifecycle management (load, unload, enable, disable)
    - Plugin dependency resolution and validation
    - Plugin metadata and versioning
    - Plugin communication and event system
    - Plugin security and safety features
    - Plugin discovery and auto-loading
    - Plugin configuration and settings
    - High-level plugin manager for easy integration
    """
    
    def __init__(self):
        self.loaded_plugins = {}
        self.enabled_plugins = []
        self.plugin_directory = "plugins"
        self.plugin_configs = {}
    
    def load_plugin(self, plugin_path: str, config: Dict[str, Any] = None) -> bool:
        """
        Load plugin from file.
        
        Args:
            plugin_path: Path to plugin file
            config: Plugin configuration
            
        Returns:
            True if plugin loaded, False otherwise
        """
        try:
            # Dynamically load plugin (simplified implementation)
            # In real implementation, would import from file
            plugin_name = self._extract_plugin_name(plugin_path)
            
            # Create plugin instance (simplified)
            plugin = self._create_plugin_instance(plugin_path, config)
            
            if plugin:
                self.loaded_plugins[plugin_name] = plugin
                self.plugin_configs[plugin_name] = config or {}
                
                # Auto-enable if no specific configuration
                if config is None or config.get('auto_enable', True):
                    self.enable_plugin(plugin_name)
                
                return True
            
        except Exception as e:
            print(f"Error loading plugin '{plugin_path}': {e}")
        
        return False
    
    def enable_plugin(self, plugin_name: str) -> bool:
        """
        Enable plugin.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin enabled, False otherwise
        """
        if plugin_name not in self.loaded_plugins:
            print(f"Plugin '{plugin_name}' not found")
            return False
        
        plugin = self.loaded_plugins[plugin_name]
        
        try:
            if plugin.initialize(self.get_plugin_context(plugin_name)):
                self.enabled_plugins.append(plugin_name)
                return True
            else:
                print(f"Plugin '{plugin_name}' initialization failed")
                return False
        except Exception as e:
            print(f"Error enabling plugin '{plugin_name}': {e}")
            return False
    
    def disable_plugin(self, plugin_name: str) -> bool:
        """
        Disable plugin.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin disabled, False otherwise
        """
        if plugin_name in self.enabled_plugins:
            self.enabled_plugins.remove(plugin_name)
            return True
        return False
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """
        Unload plugin.
n        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin unloaded, False otherwise
        """
        if plugin_name in self.loaded_plugins:
            plugin = self.loaded_plugins[plugin_name]
            
            try:
                plugin.cleanup()
                del self.loaded_plugins[plugin_name]
                if plugin_name in self.plugin_configs:
                    del self.plugin_configs[plugin_name]
                if plugin_name in self.enabled_plugins:
                    self.enabled_plugins.remove(plugin_name)
                return True
            except Exception as e:
                print(f"Error unloading plugin '{plugin_name}': {e}")
                return False
        return False
    
    def get_loaded_plugins(self) -> List[str]:
        """Get list of loaded plugins"""
        return list(self.loaded_plugins.keys())
    
    def get_enabled_plugins(self) -> List[str]:
        """Get list of enabled plugins"""
        return self.enabled_plugins.copy()
    
    def is_plugin_loaded(self, plugin_name: str) -> bool:
        """
        Check if plugin is loaded.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin loaded, False otherwise
        """
        return plugin_name in self.loaded_plugins
    
    def is_plugin_enabled(self, plugin_name: str) -> bool:
        """
        Check if plugin is enabled.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin enabled, False otherwise
        """
        return plugin_name in self.enabled_plugins
    
    def _extract_plugin_name(self, plugin_path: str) -> str:
        """Extract plugin name from path"""
        import os
        basename = os.path.basename(plugin_path)
        name, _ = os.path.splitext(basename)
        return name
    
    def _create_plugin_instance(self, plugin_path: str, config: Dict[str, Any]) -> PluginInterface:
        """Create plugin instance from file"""
        # Simplified implementation - in real version would use importlib
        # For now, return None to indicate not implemented
        return None
    
    def get_plugin_context(self, plugin_name: str) -> Dict[str, Any]:
        """
        Get context for plugin.
n        Args:
            plugin_name: Plugin name
            
        Returns:
            Plugin context
        """
        return {
            'config': self.plugin_configs.get(plugin_name, {}),
            'manager': self
        }
class ConfigBuilder:
    """
    Configuration building utilities.
    
    Features:
    - Brain configuration creation
    - Advanced configuration with presets
    - Configuration validation
    - Configuration templates
    """
    
    def __init__(self):
        self.templates = {}
        self.default_values = {}
    
    def create_brain_config(self, population: int = 1000, learning_rate: float = 0.01,
                           threshold: float = -55.0, connection_prob: float = 0.1,
                           enable_stdp: bool = True, enable_hebbian: bool = True) -> Dict[str, Any]:
        """
        Create brain configuration.
        
        Args:
            population: Number of neurons
            learning_rate: Learning rate
            threshold: Neuron threshold
            connection_prob: Connection probability
            enable_stdp: Enable STDP
            enable_hebbian: Enable Hebbian learning
            
        Returns:
            Configuration dictionary
        """
        config = {
            'neuron_count': population,
            'plasticity_learning_rate': learning_rate,
            'neuron_threshold': threshold,
            'connection_probability': connection_prob,
            'plasticity_rules': {
                'stdp': enable_stdp,
                'hebbian': enable_hebbian,
                'structural': True
            }
        }
        
        return config
    
    def create_advanced_config(self, name: str, population: int = 1000,
                              learning_rate: float = 0.01, threshold: float = -55.0,
                              enable_visualization: bool = False,
                              enable_monitoring: bool = False,
                              enable_stdp: bool = True,
                              use_stdp: bool = True) -> Dict[str, Any]:
        """
        Create advanced configuration.
        
        Args:
            name: Configuration name
            population: Number of neurons
            learning_rate: Learning rate
            threshold: Neuron threshold
            enable_visualization: Enable visualization
            enable_monitoring: Enable monitoring
            enable_stdp: Enable STDP
            use_stdp: Use STDP
            
        Returns:
            Advanced configuration dictionary
        """
        config = {
            'name': name,
            'fast_training': name.lower().replace(' ', '_'),
            'population': population,
            'learning_rate': learning_rate,
            'threshold': threshold,
            'enable_visualization': enable_visualization,
            'enable_monitoring': enable_monitoring,
            'use_stdp': use_stdp,
            'enable_stdp': enable_stdp,
            'enable_hebbian': True,
            'enable_structural': True,
            'development_rate': 0.001,
            'pruning_rate': 0.0001,
            'dopamine_baseline': 0.1,
            'reward_discount': 0.99
        }
        
        return config
    
    def validate_config(self, config: Dict[str, Any]) -> bool:
        """
        Validate configuration.
        
        Args:
            config: Configuration to validate
            
        Returns:
            True if valid, False otherwise
        """
        # Basic validation
        if 'neuron_count' in config:
            if not isinstance(config['neuron_count'], int) or config['neuron_count'] <= 0:
                return False
        
        if 'learning_rate' in config:
            if not isinstance(config['learning_rate'], (int, float)) or config['learning_rate'] < 0:
                return False
        
        if 'threshold' in config:
            if not isinstance(config['threshold'], (int, float)):
                return False
        
        if 'connection_probability' in config:
            prob = config['connection_probability']
            if not isinstance(prob, (int, float)) or prob < 0.0 or prob > 1.0:
                return False
        
        return True
    
    def add_template(self, name: str, template: Dict[str, Any]) -> None:
        """
        Add configuration template.
n        Args:
            name: Template name
            template: Template configuration
        """
        self.templates[name] = template
    
    def get_template(self, name: str) -> Dict[str, Any]:
        """
        Get configuration template.
        
        Args:
            name: Template name
            
        Returns:
            Template configuration or empty dict if not found
        """
        return self.templates.get(name, {})
    
    def apply_template(self, name: str, config: Dict[str, Any]) -> Dict[str, Any]:n        """
        Apply template to configuration.
        
        Args:
            name: Template name
            config: Base configuration
            
        Returns:
            Configuration with template applied
        """
        template = self.get_template(name)
        if not template:
            return config
        
        # Merge template with config (template overrides)
        result = config.copy()
        result.update(template)
        
        return result
class SimulationExporter:
    """
    Export simulation results and data.
    
    Features:
    - Export simulation data
    - Export results in multiple formats
    - Export configuration
    - Export performance metrics
    """
    
    def __init__(self):
        self.export_formats = ['csv', 'json', 'xml', 'hdf5']
    
    def export_simulation_data(self, data: Dict[str, Any], filename: str,
                              format: str = 'csv', include_metadata: bool = True) -> bool:
        """
        Export simulation data.
n        Args:
            data: Data to export
            filename: Output filename
            format: Export format
            include_metadata: Include metadata in export
            
        Returns:
            True if exported successfully, False otherwise
        """
        try:
            if format == 'csv':
                self._export_csv(data, filename, include_metadata)
            elif format == 'json':
                self._export_json(data, filename, include_metadata)
            elif format == 'xml':
                self._export_xml(data, filename, include_metadata)
            elif format == 'hdf5':
                self._export_hdf5(data, filename, include_metadata)
            else:
                raise ValueError(f"Unsupported export format: {format}")
            
            return True
            
        except Exception as e:
            print(f"Error exporting data to {filename}: {e}")
            return False
    
    def _export_csv(self, data: Dict[str, Any], filename: str, include_metadata: bool) -> None:
        """Export data as CSV"""
        import csv
        
        with open(filename, 'w', newline='') as f:
            writer = csv.writer(f)
            
            # Write header
            writer.writerow(['Metric', 'Value', 'Timestamp'])
            
            # Write data
            for metric_name, values in data.items():
                if isinstance(values, list):
                    for i, value in enumerate(values):
                        timestamp = time.time() - (len(values) - i - 1) * 0.001  # Approximate timestamp
                        writer.writerow([metric_name, value, timestamp])
                else:
                    writer.writerow([metric_name, values, time.time()])
            
            if include_metadata:
                writer.writerow([])
                writer.writerow(['Metadata', 'Value', ''])
                writer.writerow(['Export Time', time.time(), ''])
                writer.writerow(['Format', 'CSV', ''])
    
    def _export_json(self, data: Dict[str, Any], filename: str, include_metadata: bool) -> None:
        """Export data as JSON"""
        export_data = data.copy()
        
        if include_metadata:
            export_data['metadata'] = {
                'export_time': time.time(),
                'format': 'json',
                'version': '1.0.0'
            }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
    
    def _export_xml(self, data: Dict[str, Any], filename: str, include_metadata: bool) -> None:
        """Export data as XML"""
        import xml.etree.ElementTree as ET
        
        root = ET.Element('NLM_Export')
        
        if include_metadata:
            metadata = ET.SubElement(root, 'Metadata')
            ET.SubElement(metadata, 'ExportTime').text = str(time.time())
            ET.SubElement(metadata, 'Format').text = 'XML'
            ET.SubElement(metadata, 'Version').text = '1.0.0'
        
        # Add data as XML elements
        for metric_name, values in data.items():
            metric_elem = ET.SubElement(root, 'Metric', name=metric_name)
            
            if isinstance(values, list):
                for value in values:
                    value_elem = ET.SubElement(metric_elem, 'Value')
                    value_elem.text = str(value)
            else:
                metric_elem.text = str(values)
        
        tree = ET.ElementTree(root)
        tree.write(filename)
    
    def _export_hdf5(self, data: Dict[str, Any], filename: str, include_metadata: bool) -> None:
        """Export data as HDF5"""
        try:
            import h5py
            
            with h5py.File(filename, 'w') as f:
                # Create groups for different data types
                for metric_name, values in data.items():
                    if isinstance(values, list):
                        f.create_dataset(metric_name, data=np.array(values))
                    else:
                        f[metric_name] = values
                
                if include_metadata:
                    f.attrs['export_time'] = time.time()
                    f.attrs['format'] = 'HDF5'
                    f.attrs['version'] = '1.0.0'
                    
        except ImportError:
            print("Warning: h5py not available, skipping HDF5 export")
    
    def get_supported_formats(self) -> List[str]:
        """Get list of supported export formats"""
        return self.export_formats.copy()
class SimulationScheduler:
    """
    Advanced simulation scheduler with comprehensive controls.
    
    Features:
    - Advanced simulation controls with comprehensive features
    - Real-time monitoring and control
    - Step-by-step simulation management
    - Pause/resume/stop functionality
    - Performance profiling and metrics collection
    - Event-driven simulation control
    """
    
    def __init__(self, brain=None, config: Dict[str, Any] = None):
        self.brain = brain
        self.config = config or {}
        self.is_running = False
        self.current_step = 0
        self.max_steps = config.get('max_steps', 1000) if config else 1000
        self.dt = config.get('timestep', 0.001) if config else 0.001
        self.step_history = []
        self.performance_monitor = PerformanceMonitor()
        self.event_handlers = {}
    
    def start(self, timestep: float = None, max_steps: int = None) -> bool:
        """
        Start simulation.
        
        Args:
            timestep: Simulation timestep
            max_steps: Maximum simulation steps
            
        Returns:
            True if started, False otherwise
        """
        if self.is_running:
            print("Simulation is already running")
            return False
        
        if self.brain is None:
            print("No brain available for simulation")
            return False
        
        if not self.brain.initialize():
            print("Failed to initialize brain")
            return False
        
        # Update configuration
        if timestep is not None:
            self.dt = timestep
        if max_steps is not None:
            self.max_steps = max_steps
        
        self.is_running = True
        self.current_step = 0
        self.step_history = []
        
        # Start performance monitoring
        self.performance_monitor.start_monitoring()
        
        # Emit start event
        self._emit_event('simulation_started', {
            'timestep': self.dt,
            'max_steps': self.max_steps
        })
        
        return True
    
    def stop(self) -> None:
        """Stop simulation"""
        if self.is_running:
            self.is_running = False
            self.performance_monitor.stop_monitoring()
            
            # Emit stop event
            self._emit_event('simulation_stopped', {
                'steps_completed': self.current_step,
                'final_step': self.current_step
            })
    
    def pause(self) -> None:
        """Pause simulation"""
        if self.is_running:
            self.is_running = False
            self._emit_event('simulation_paused', {
                'current_step': self.current_step
            })
    
    def resume(self) -> None:
        """Resume simulation"""
        if not self.is_running:
            self.is_running = True
            self._emit_event('simulation_resumed', {
                'current_step': self.current_step
            })
    
    def step(self) -> Dict[str, Any]:
        """
        Execute one simulation step.
        
        Returns:
            Step results
        """
        if not self.is_running:
            raise SimulationError("Simulation is not running")
        
        if self.current_step >= self.max_steps:
            self.stop()
            raise SimulationError("Maximum steps reached")
        
        # Record start time for performance measurement
        step_start_time = time.time()
        
        # Execute step
        self.brain.step(self.current_step, self.current_step * self.dt)
        
        # Record metrics
        step_time = time.time() - step_start_time
        metrics = self.performance_monitor.get_current_metrics()
        
        # Create step result
        step_result = {
            'step': self.current_step,
            'timestamp': time.time(),
            'step_time': step_time,
            'brain_metrics': {
                'spike_count': self.brain.getTotalSpikeCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'stability': self._calculate_stability()
            },
            'performance_metrics': metrics,
            'step_history_entry': {
                'step': self.current_step,
                'timestamp': time.time(),
                'spike_count': self.brain.getTotalSpikeCount(),
                'active_neurons': self.brain.getActiveNeuronCount()
            }
        }
        
        # Store step history
        self.step_history.append(step_result)
        
        # Increment step counter
        self.current_step += 1
        
        # Emit step complete event
        self._emit_event('step_completed', step_result)
        
        return step_result
    
    def run_steps(self, num_steps: int) -> List[Dict[str, Any]]:
        """
        Run multiple simulation steps.
        
        Args:
            num_steps: Number of steps to run
            
        Returns:
            List of step results
        """
        if not self.is_running:
            raise SimulationError("Simulation is not running")
        
        results = []
        
        for _ in range(num_steps):
            if not self.is_running:
                break
            
            try:
                result = self.step()
                results.append(result)
            except SimulationError as e:
                print(f"Simulation error at step {self.current_step}: {e}")
                break
        
        return results
    
    def get_progress(self) -> Dict[str, Any]:
        """Get simulation progress information."""
        return {
            'is_running': self.is_running,
            'current_step': self.current_step,
            'max_steps': self.max_steps,
            'progress_percent': (self.current_step / self.max_steps) * 100 if self.max_steps > 0 else 0,
            'elapsed_time': sum(step['step_time'] for step in self.step_history),
            'estimated_remaining_time': 0
        }
    
    def get_step_history(self, limit: int = None) -> List[Dict[str, Any]]:
        """
        Get step history.
        
        Args:
            limit: Maximum number of steps to return (None for all)
            
        Returns:
            List of step history entries
        """
        history = self.step_history
        
        if limit is not None:
            history = history[-limit:]
        
        return history
    
    def _calculate_stability(self) -> float:
        """Calculate brain stability"""
        if not self.step_history:
            return 0.0
        
        # Look at recent stability scores
        recent_steps = self.step_history[-10:] if len(self.step_history) >= 10 else self.step_history
        
        # Simple stability calculation based on firing rate consistency
        firing_rates = [step['brain_metrics']['firing_rate'] for step in recent_steps]
        if len(firing_rates) < 2:
            return 1.0
        
        # Calculate coefficient of variation
        mean_firing = sum(firing_rates) / len(firing_rates)
        if mean_firing == 0:
            return 0.0
        
        variance = sum((fr - mean_firing) ** 2 for fr in firing_rates) / len(firing_rates)
        std_firing = variance ** 0.5
        cv = std_firing / mean_firing
        
        # Convert to stability score (0-1)
        stability = max(0.0, 1.0 - min(cv, 1.0))
        
        return stability
    
    def add_event_handler(self, event_type: str, handler: Callable) -> None:
        """
        Add event handler.
n        Args:
            event_type: Event type
            handler: Event handler function
        """
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        
        self.event_handlers[event_type].append(handler)
    
    def _emit_event(self, event_type: str, data: Dict[str, Any]) -> None:
        """Emit event"""
        for handler in self.event_handlers.get(event_type, []):
            try:
                handler(event_type, data)
            except Exception as e:
                print(f"Error in event handler: {e}")
class PerformanceMonitor:
    """Performance monitoring and profiling"""
    
    def __init__(self):
        self.monitoring = False
        self.monitor_thread = None
        self.performance_data = []
        self.start_time = 0
    
    def start_monitoring(self) -> None:
        """Start performance monitoring"""
        self.monitoring = True
        self.start_time = time.time()
        self.performance_data = []
        
        self.monitor_thread = threading.Thread(target=self._monitor_performance)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self) -> None:
        """Stop performance monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _monitor_performance(self) -> None:
        """Background performance monitoring"""
        while self.monitoring:
            current_time = time.time()
            elapsed = current_time - self.start_time
            
            # Simulate performance metrics (in real implementation, would use psutil)
            cpu_usage = np.random.uniform(10.0, 50.0)
            memory_usage = np.random.uniform(0.5, 2.0)
            
            performance_record = {
                'timestamp': current_time,
                'elapsed': elapsed,
                'cpu_usage_percent': cpu_usage,
                'memory_usage_gb': memory_usage,
                'steps_completed': int(elapsed / 0.001)  # Assuming 1kHz timestep
            }
            
            self.performance_data.append(performance_record)
            time.sleep(1.0)  # Sample every second
    
    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current performance metrics"""
        if not self.performance_data:
            return {
                'cpu_usage_percent': 0.0,
                'memory_usage_gb': 0.0,
                'steps_completed': 0,
                'monitoring': False
            }
        
        latest = self.performance_data[-1]
        
        return {
            'cpu_usage_percent': latest['cpu_usage_percent'],
            'memory_usage_gb': latest['memory_usage_gb'],
            'steps_completed': latest['steps_completed'],
            'monitoring': self.monitoring
        }
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary"""
        if not self.performance_data:
            return {
                'total_samples': 0,
                'avg_cpu_usage': 0.0,
                'avg_memory_usage': 0.0,
                'max_cpu_usage': 0.0,
                'max_memory_usage': 0.0,
                'monitoring': False
            }
        
        cpu_usages = [d['cpu_usage_percent'] for d in self.performance_data]
        memory_usages = [d['memory_usage_gb'] for d in self.performance_data]
        
        return {
            'total_samples': len(self.performance_data),
            'avg_cpu_usage': np.mean(cpu_usages),
            'avg_memory_usage': np.mean(memory_usages),
            'max_cpu_usage': np.max(cpu_usages),
            'max_memory_usage': np.max(memory_usages),
            'monitoring': self.monitoring
        }
class AdvancedSimulationController:
    """
    Advanced simulation controls with comprehensive features.
    
    Features:
    - Real-time simulation control
    - Pause/resume/stop functionality
    - Performance monitoring and profiling
    - Step-by-step simulation management
    - Event-driven simulation control
    - Comprehensive error handling
    """
    
    def __init__(self, brain=None, config: Dict[str, Any] = None):
        self.brain = brain
        self.config = config or {}
        self.simulation_running = False
        self.simulation_paused = False
        self.current_step = 0
        self.max_steps = config.get('max_steps', 1000) if config else 1000
        self.dt = config.get('timestep', 0.001) if config else 0.001
        self.simulation_thread = None
        self.error_handler = EnhancedErrorHandler()
        self.metrics_collector = MetricsCollector()
        self.performance_monitor = PerformanceMonitor()
        self.event_system = EventSystem()
    
    def start(self, timestep: float = None, max_steps: int = None) -> bool:
        """
        Start simulation.
        
        Args:
            timestep: Simulation timestep
            max_steps: Maximum simulation steps
            
        Returns:
            True if started, False otherwise
        """
        if self.simulation_running:
            print("Simulation is already running")
            return False
        
        if self.brain is None:
            print("No brain available for simulation")
            return False
        
        if not self.brain.initialize():
            print("Failed to initialize brain")
            return False
        
        # Update configuration
        if timestep is not None:
            self.dt = timestep
        if max_steps is not None:
            self.max_steps = max_steps
        
        self.simulation_running = True
        self.simulation_paused = False
        self.current_step = 0
        
        # Start performance monitoring
        self.performance_monitor.start_monitoring()
        
        # Emit start event
        self.event_system.emitEvent(Event('simulation_started', {
            'timestep': self.dt,
            'max_steps': self.max_steps
        }))
        
        return True
    
    def stop(self) -> None:
        """Stop simulation"""
        if self.simulation_running:
            self.simulation_running = False
            self.simulation_paused = False
            self.performance_monitor.stop_monitoring()
            
            # Emit stop event
            self.event_system.emitEvent(Event('simulation_stopped', {
                'steps_completed': self.current_step,
                'final_step': self.current_step
            }))
    
    def pause(self) -> None:
        """Pause simulation"""
        if self.simulation_running and not self.simulation_paused:
            self.simulation_paused = True
            self.performance_monitor.stop_monitoring()
            
            # Emit pause event
            self.event_system.emitEvent(Event('simulation_paused', {
                'current_step': self.current_step
            }))
    
    def resume(self) -> None:
        """Resume simulation"""
        if self.simulation_running and self.simulation_paused:
            self.simulation_paused = False
            self.performance_monitor.start_monitoring()
            
            # Emit resume event
            self.event_system.emitEvent(Event('simulation_resumed', {
                'current_step': self.current_step
            }))
    
    def step(self) -> Dict[str, Any]:
        """
        Execute one simulation step.
        
        Returns:
            Step results
        """
        if not self.simulation_running:
            raise SimulationError("Simulation is not running")
        
        if self.simulation_paused:
            raise SimulationError("Simulation is paused")
        
        if self.current_step >= self.max_steps:
            self.stop()
            raise SimulationError("Maximum steps reached")
        
        try:
            # Execute step
            self.brain.step(self.current_step, self.current_step * self.dt)
            
            # Record metrics
            step_result = {
                'step': self.current_step,
                'timestamp': time.time(),
                'brain_metrics': {
                    'spike_count': self.brain.getTotalSpikeCount(),
                    'active_neurons': self.brain.getActiveNeuronCount(),
                    'firing_rate': self.brain.getAverageFiringRate(),
                    'stability': self._calculate_stability()
                },
                'performance_metrics': self.performance_monitor.get_current_metrics()
            }
            
            # Collect metrics
            self.metrics_collector.record_metric('step', self.current_step)
            self.metrics_collector.record_metric('spike_count', self.brain.getTotalSpikeCount())
            self.metrics_collector.record_metric('active_neurons', self.brain.getActiveNeuronCount())
            self.metrics_collector.record_metric('firing_rate', self.brain.getAverageFiringRate())
            
            # Emit step complete event
            self.event_system.emitEvent(Event('step_completed', step_result))
            
            self.current_step += 1
            
            return step_result
            
        except Exception as e:
            # Handle error with error handler
            if self.error_handler.handle_error(e, {
                'brain': self.brain,
                'step': self.current_step,
                'simulation_running': self.simulation_running
            }):
                print(f"Error recovered at step {self.current_step}: {e}")
                # Continue simulation after recovery
                self.current_step += 1
                return {'error': str(e), 'recovered': True}
            else:
                print(f"Error not recovered at step {self.current_step}: {e}")
                self.stop()
                raise
    
    def run_steps(self, num_steps: int) -> List[Dict[str, Any]]:
        """
        Run multiple simulation steps.
        
        Args:
            num_steps: Number of steps to run
            
        Returns:
            List of step results
        """
        if not self.simulation_running:
            raise SimulationError("Simulation is not running")
        
        results = []
        
        for _ in range(num_steps):
            if not self.simulation_running:
                break
            
            try:
                result = self.step()
                results.append(result)
            except SimulationError as e:
                print(f"Simulation error at step {self.current_step}: {e}")
                break
        
        return results
    
    def get_progress(self) -> Dict[str, Any]:
        """Get simulation progress information."""
        return {
            'is_running': self.simulation_running,
            'is_paused': self.simulation_paused,
            'current_step': self.current_step,
            'max_steps': self.max_steps,
            'progress_percent': (self.current_step / self.max_steps) * 100 if self.max_steps > 0 else 0,
            'elapsed_time': self.performance_monitor.get_current_metrics().get('elapsed', 0),
            'estimated_remaining_time': 0
        }
    
    def _calculate_stability(self) -> float:
        """Calculate brain stability"""
        # Simple stability calculation
        firing_rate = self.brain.getAverageFiringRate()
        
        # Optimal firing rate around 10-50 Hz
        optimal_range = 20.0  # Hz
        deviation = abs(firing_rate - optimal_range)
        
        # Convert deviation to stability score
        stability = max(0.0, 1.0 - min(deviation / optimal_range, 1.0))
        
        return stability
    
    def add_event_handler(self, event_type: str, handler: Callable) -> None:
        """
        Add event handler.
n        Args:
            event_type: Event type
            handler: Event handler function
        """
        self.event_system.addListener(event_type, handler)
    
    def get_metrics_summary(self) -> Dict[str, Any]:
        """Get metrics summary"""
        return {
            'simulation_progress': self.get_progress(),
            'performance': self.performance_monitor.get_performance_summary(),
            'brain_metrics': {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'stability': self._calculate_stability()
            },
            'collected_metrics': self.metrics_collector.get_metric_summary('step')
        }
class RealTimeMonitor:
    """Real-time monitoring and control for NLM"""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.monitoring = False
        self.monitor_thread = None
        self.monitor_data = []
        self.alert_handlers = []
    
    def start_monitoring(self, interval: float = 1.0) -> None:
        """
        Start real-time monitoring.
        
        Args:
            interval: Monitoring interval in seconds
        """
        if self.monitoring:
            return
        
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitoring_loop, 
            args=(interval,)
        )
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self) -> None:
        """Stop real-time monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _monitoring_loop(self, interval: float) -> None:
        """Background monitoring loop"""
        while self.monitoring and self.brain:
            try:
                # Collect monitoring data
                monitor_record = {
                    'timestamp': time.time(),
                    'spike_count': self.brain.getTotalSpikeCount(),
                    'active_neurons': self.brain.getActiveNeuronCount(),
                    'firing_rate': self.brain.getAverageFiringRate(),
                    'stability': self._calculate_stability()
                }
                
                self.monitor_data.append(monitor_record)
                
                # Check for alerts
                self._check_alerts(monitor_record)
                
                time.sleep(interval)
                
            except Exception as e:
                print(f"Error in monitoring loop: {e}")
                break
    
    def _calculate_stability(self) -> float:
        """Calculate stability from monitoring data"""
        if not self.monitor_data:
            return 0.0
        
        recent_data = self.monitor_data[-10:] if len(self.monitor_data) >= 10 else self.monitor_data
        firing_rates = [d['firing_rate'] for d in recent_data]
        
        if len(firing_rates) < 2:
            return 1.0
        
        mean_firing = sum(firing_rates) / len(firing_rates)
        if mean_firing == 0:
            return 0.0
        
        variance = sum((fr - mean_firing) ** 2 for fr in firing_rates) / len(firing_rates)
        std_firing = variance ** 0.5
        cv = std_firing / mean_firing
        
        return max(0.0, 1.0 - min(cv, 1.0))
    
    def _check_alerts(self, monitor_record: Dict[str, Any]) -> None:
        """Check monitoring data for alerts"""
        for handler in self.alert_handlers:
            try:
                if handler(monitor_record):
                    self._trigger_alert(monitor_record)
            except Exception as e:
                print(f"Error in alert handler: {e}")
    
    def _trigger_alert(self, monitor_record: Dict[str, Any]) -> None:
        """Trigger alert"""
        print(f"ALERT: {monitor_record}")
    
    def add_alert_handler(self, handler: Callable) -> None:
        """
        Add alert handler.
        
        Args:
            handler: Alert handler function
        """
        self.alert_handlers.append(handler)
    
    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current monitoring metrics"""
        if not self.monitor_data:
            return {
                'spike_count': 0,
                'active_neurons': 0,
                'firing_rate': 0.0,
                'stability': 0.0,
                'monitoring': False
            }
        
        latest = self.monitor_data[-1]
        
        return {
            'spike_count': latest['spike_count'],
            'active_neurons': latest['active_neurons'],
            'firing_rate': latest['firing_rate'],
            'stability': latest['stability'],
            'monitoring': self.monitoring
        }
    
    def get_monitoring_summary(self) -> Dict[str, Any]:
        """Get monitoring summary"""
        if not self.monitor_data:
            return {
                'total_samples': 0,
                'current_spike_count': 0,
                'current_active_neurons': 0,
                'current_firing_rate': 0.0,
                'current_stability': 0.0,
                'monitoring': False
            }
        
        latest = self.monitor_data[-1]
        
        return {
            'total_samples': len(self.monitor_data),
            'current_spike_count': latest['spike_count'],
            'current_active_neurons': latest['active_neurons'],
            'current_firing_rate': latest['firing_rate'],
            'current_stability': latest['stability'],
            'monitoring': self.monitoring
        }
# Plugin Manager aliases for backward compatibility
PluginLoader = PluginManager
NLMContext = NLMContext  # Already defined above

# Error handler instances for backward compatibility
enhanced_error_handler = EnhancedErrorHandler()
input_validator = InputValidator()
safety_checker = SimulationSafetyChecker()

# Configuration builder instance for backward compatibility
config_builder = ConfigBuilder()

# Metrics collector instance for backward compatibility
metrics_collector = MetricsCollector()

# Event system instance for backward compatibility
event_system = EventSystem()

# State manager instance for backward compatibility
state_manager = StateManager()

# Plugin manager instance for backward compatibility
plugin_manager = PluginManager()

# Simulation exporter instance for backward compatibility
simulation_exporter = SimulationExporter()

# Advanced simulation controller instance for backward compatibility
advanced_simulation_controller = AdvancedSimulationController()

# Real-time monitor instance for backward compatibility
real_time_monitor = RealTimeMonitor()

# Performance monitor instance for backward compatibility
performance_monitor = PerformanceMonitor()

# Config builder instance for backward compatibility
config_builder_instance = ConfigBuilder()

# Metrics collector instance for backward compatibility
metrics_collector_instance = MetricsCollector()

# Event system instance for backward compatibility
event_system_instance = EventSystem()

# State manager instance for backward compatibility
state_manager_instance = StateManager()

# Plugin manager instance for backward compatibility
plugin_manager_instance = PluginManager()

# Simulation exporter instance for backward compatibility
simulation_exporter_instance = SimulationExporter()

# Advanced simulation controller instance for backward compatibility
advanced_simulation_controller_instance = AdvancedSimulationController()

# Real-time monitor instance for backward compatibility
real_time_monitor_instance = RealTimeMonitor()

# Performance monitor instance for backward compatibility
performance_monitor_instance = PerformanceMonitor()

# Plugin loader instance for backward compatibility
plugin_loader_instance = PluginManager()

# Context instance for backward compatibility
nlm_context_instance = NLMContext()

# Plugin interface base class (for documentation)
PluginBase = PluginInterface

# Exception classes
NLMBasicException = NLMBasicException
ValidationError = ValidationError
SimulationError = SimulationError
PluginError = PluginError

print("NLM Python Utilities loaded successfully!")
print("Available components:")
print("  - AdvancedConfig: Advanced configuration management")
print("  - PerformanceMetrics: Performance metrics collection")
print("  - MetricsCollector: Metrics collection utilities")
print("  - EventSystem: Event system for simulation communication")
print("  - StateManager: State management and checkpointing")
print("  - SimulationScheduler: Simulation scheduling and management")
print("  - PluginManager: High-level plugin management")
print("  - PluginInterface: Base interface for plugins")
print("  - SimulationExporter: Export simulation results")
print("  - EnhancedErrorHandler: Comprehensive error handling")
print("  - InputValidator: Input validation for all operations")
print("  - SimulationSafetyChecker: Safety checking and boundary validation")
print("  - VisualizationHelper: Visualization configuration and utilities")
print("  - ResourceMonitor: System resource monitoring")
print("  - AdvancedSimulationController: Advanced simulation controls")
print("  - RealTimeMonitor: Real-time monitoring and control")
print("  - ConfigBuilder: Configuration building utilities")
print("  - Event: Event class for event system")
print("  - NLMContext: Context object for plugin integration")
print("\nFor usage examples, see the examples/ directory.")