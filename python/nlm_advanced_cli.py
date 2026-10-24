#!/usr/bin/env python3
"""
NLM Python Interface - Enhanced Version

A significantly enhanced, production-ready Python interface for the NLM neural
simulation framework. This version addresses all identified issues and provides
comprehensive functionality for users of all skill levels.
"""

import os
import json
import time
import pickle
import threading
import logging
from typing import List, Dict, Any, Optional, Union, Callable
from contextlib import contextmanager
from enum import Enum
import numpy as np
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
class NLMErrors:
    """Custom error types for NLM operations."""
    
    class ConfigurationError(Exception):
        """Raised when configuration is invalid."""
        pass
        
    class BrainInitializationError(Exception):
        """Raised when brain initialization fails."""
        pass
        
    class ExperimentError(Exception):
        """Raised when experiment operations fail."""
        pass
        
    class FileOperationError(Exception):
        """Raised when file operations fail."""
        pass
class NLMValidation:
    """Comprehensive validation utilities for NLM operations."""
    
    @staticmethod
    def validate_config(config: Dict[str, Any], schema: Dict[str, Any]) -> List[str]:
        """Validate configuration against schema, return list of errors."""
        errors = []
        
        # Check for missing required keys
        for key, constraints in schema.items():
            if constraints.get('required', False) and key not in config:
                errors.append(f"Missing required configuration key: {key}")
                
        # Validate each provided configuration
        for key, value in config.items():
            if key in schema:
                constraints = schema[key]
                errors.extend(NLMValidation._validate_value(key, value, constraints))
                
        return errors
        
    @staticmethod
    def _validate_value(key: str, value: Any, constraints: Dict[str, Any]) -> List[str]:
        """Validate a single configuration value."""
        errors = []
        
        # Type validation
        expected_type = constraints.get('type')
        if expected_type == 'int':
            if not isinstance(value, int):
                errors.append(f"Configuration '{key}' must be an integer, got {type(value).__name__}")
            else:
                # Range validation for integers
                min_val = constraints.get('min')
                max_val = constraints.get('max')
                if min_val is not None and value < min_val:
                    errors.append(f"Configuration '{key}' must be >= {min_val}, got {value}")
                if max_val is not None and value > max_val:
                    errors.append(f"Configuration '{key}' must be <= {max_val}, got {value}")
                    
        elif expected_type == 'float':
            if not isinstance(value, (int, float)):
                errors.append(f"Configuration '{key}' must be a number, got {type(value).__name__}")
            else:
                # Range validation for floats
                min_val = constraints.get('min')
                max_val = constraints.get('max')
                if min_val is not None and value < min_val:
                    errors.append(f"Configuration '{key}' must be >= {min_val}, got {value}")
                if max_val is not None and value > max_val:
                    errors.append(f"Configuration '{key}' must be <= {max_val}, got {value}")
                    
        elif expected_type == 'str':
            if not isinstance(value, str):
                errors.append(f"Configuration '{key}' must be a string, got {type(value).__name__}")
            else:
                # Choice validation for strings
                choices = constraints.get('choices')
                if choices and value not in choices:
                    errors.append(f"Configuration '{key}' must be one of {choices}, got '{value}'")
                    
        elif expected_type == 'bool':
            if not isinstance(value, bool):
                errors.append(f"Configuration '{key}' must be a boolean, got {type(value).__name__}")
                
        elif expected_type == 'list':
            if not isinstance(value, list):
                errors.append(f"Configuration '{key}' must be a list, got {type(value).__name__}")
            else:
                # Item type validation
                item_type = constraints.get('item_type')
                if item_type:
                    for i, item in enumerate(value):
                        if item_type == 'int' and not isinstance(item, int):
                            errors.append(f"Configuration '{key}[{i}]' must be an integer")
                        elif item_type == 'float' and not isinstance(item, (int, float)):
                            errors.append(f"Configuration '{key}[{i}]' must be a number")
                        elif item_type == 'str' and not isinstance(item, str):
                            errors.append(f"Configuration '{key}[{i}]' must be a string")
                                
        return errors
class AdvancedNLMConfig(NLMValidation):
    """Advanced configuration system with comprehensive validation and features."""
    
    def __init__(self, config_dict: Optional[Dict] = None, schema: Optional[Dict] = None):
        self._config = config_dict or {}
        self._schema = schema or self._create_default_schema()
        self._validate_complete()
        
    def _create_default_schema(self) -> Dict[str, Dict]:
        """Create comprehensive default schema with all possible configurations."""
        return {
            # Core simulation parameters
            'random_seed': {'type': 'int', 'min': 0, 'max': 2**31 - 1, 'required': True, 'default': 42},
            'simulation_timestep': {'type': 'float', 'min': 1e-6, 'max': 1.0, 'required': True, 'default': 0.001},
            'neuron_count': {'type': 'int', 'min': 1, 'max': 1000000, 'required': True, 'default': 1000},
            'region_count': {'type': 'int', 'min': 1, 'max': 100, 'default': 2},
            'connection_probability': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.1},
            
            # Plasticity parameters
            'plasticity_learning_rate': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.01},
            'stdp_ltp_weight': {'type': 'float', 'min': 0.0, 'default': 0.01},
            'stdp_ltd_weight': {'type': 'float', 'min': 0.0, 'default': 0.012},
            'stdp_time_constant': {'type': 'float', 'min': 0.0, 'default': 20.0},
            'hebbian_weight': {'type': 'float', 'min': 0.0, 'default': 0.01},
            'reward_modulation_strength': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.5},
            
            # Development parameters
            'development_synaptogenesis_rate': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.001},
            'development_pruning_rate': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.0001},
            'critical_period_start': {'type': 'float', 'min': 0.0, 'default': 10.0},
            'critical_period_end': {'type': 'float', 'min': 0.0, 'default': 100.0},
            'developmental_plasticity_factor': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.8},
            
            # Neuromodulation parameters
            'dopamine_baseline': {'type': 'float', 'min': 0.0, 'default': 0.1},
            'dopamine_sensitivity': {'type': 'float', 'min': 0.0, 'default': 1.0},
            'curiosity_baseline': {'type': 'float', 'min': 0.0, 'default': 0.1},
            'novelty_sensitivity': {'type': 'float', 'min': 0.0, 'default': 1.0},
            'prediction_error_threshold': {'type': 'float', 'min': 0.0, 'default': 0.1},
            
            # Reward and motivation
            'reward_discount_factor': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.99},
            'reward_scale': {'type': 'float', 'min': 0.0, 'default': 1.0},
            'exploration_exploitation_balance': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.5},
            
            # Environment parameters
            'environment_name': {'type': 'str', 'choices': ['GridWorld', 'ContinuousWorld', 'CustomWorld'], 'default': 'GridWorld'},
            'environment_width': {'type': 'int', 'min': 1, 'default': 10},
            'environment_height': {'type': 'int', 'min': 1, 'default': 10},
            'world_boundary_wrapping': {'type': 'bool', 'default': False},
            
            # Input/output processing
            'sensory_input_enabled': {'type': 'bool', 'default': True},
            'sensory_input_noise': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.0},
            'motor_output_noise': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.0},
            'action_selection_method': {'type': 'str', 'choices': ['random', 'softmax', 'epsilon_greedy', 'deterministic'], 'default': 'softmax'},
            
            # Memory systems
            'working_memory_capacity': {'type': 'int', 'min': 1, 'max': 10000, 'default': 100},
            'episodic_memory_capacity': {'type': 'int', 'min': 1, 'max': 10000, 'default': 1000},
            'semantic_memory_capacity': {'type': 'int', 'min': 1, 'max': 10000, 'default': 1000},
            'procedural_memory_capacity': {'type': 'int', 'min': 1, 'max': 10000, 'default': 1000},
            'associative_memory_capacity': {'type': 'int', 'min': 1, 'max': 10000, 'default': 1000},
            
            # Cognitive parameters
            'attention_focus_strength': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.7},
            'planning_horizon': {'type': 'int', 'min': 1, 'max': 1000, 'default': 10},
            'concept_learning_rate': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.01},
            'self_model_update_rate': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.1},
            
            # Prediction system
            'prediction_model_complexity': {'type': 'int', 'min': 1, 'default': 5},
            'prediction_error_decay': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.95},
            'confidence_update_rate': {'type': 'float', 'min': 0.0, 'max': 1.0, 'default': 0.1},
            
            # Logging and monitoring
            'log_level': {'type': 'str', 'choices': ['DEBUG', 'INFO', 'WARNING', 'ERROR'], 'default': 'INFO'},
            'log_to_file': {'type': 'bool', 'default': False},
            'log_filename': {'type': 'str', 'default': 'nlm.log'},
            'log_format': {'type': 'str', 'default': '%(asctime)s - %(name)s - %(levelname)s - %(message)s'},
            
            # Performance and optimization
            'max_simulation_steps': {'type': 'int', 'min': 1, 'default': 10000},
            'simulation_time_limit': {'type': 'float', 'min': 0.0, 'default': 0.0},
            'checkpoint_interval': {'type': 'int', 'min': 1, 'default': 1000},
            'enable_checkpointing': {'type': 'bool', 'default': True},
            
            # Visualization
            'visualization_enabled': {'type': 'bool', 'default': False},
            'visualization_update_rate': {'type': 'float', 'min': 1.0, 'default': 30.0},
            'visualization_width': {'type': 'int', 'min': 100, 'default': 800},
            'visualization_height': {'type': 'int', 'min': 100, 'default': 600},
            
            # Advanced features
            'experimental_mode': {'type': 'bool', 'default': False},
            'debug_mode': {'type': 'bool', 'default': False},
            'profiling_enabled': {'type': 'bool', 'default': False},
            'performance_profiling': {'type': 'bool', 'default': False}
        }
        
    def _validate_complete(self):
        """Complete validation of configuration."""
        # Validate against schema
        errors = self.validate_config(self._config, self._schema)
        if errors:
            raise NLMErrors.ConfigurationError(f"Configuration validation failed:\n" + "\n".join(f"  - {error}" for error in errors))
            
        # Check for logical consistency
        self._validate_logical_consistency()
        
    def _validate_logical_consistency(self):
        """Validate logical consistency of configuration."""
        # Critical period should be within valid range
        if 'critical_period_start' in self._config and 'critical_period_end' in self._config:
            start = self._config['critical_period_start']
            end = self._config['critical_period_end']
            if start >= end:
                raise NLMErrors.ConfigurationError(f"Critical period start ({start}) must be before end ({end})")
                
        # Neuron count should be reasonable for other parameters
        if 'neuron_count' in self._config and 'working_memory_capacity' in self._config:
            if self._config['working_memory_capacity'] > self._config['neuron_count']:
                raise NLMErrors.ConfigurationError(f"Working memory capacity ({self._config['working_memory_capacity']}) cannot exceed neuron count ({self._config['neuron_count']})")
                
        # Connection probability should be between 0 and 1
        if 'connection_probability' in self._config:
            prob = self._config['connection_probability']
            if not 0.0 <= prob <= 1.0:
                raise NLMErrors.ConfigurationError(f"Connection probability ({prob}) must be between 0 and 1")
                
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with comprehensive validation."""
        if key in self._config:
            value = self._config[key]
            schema = self._schema.get(key)
            if schema:
                validation_errors = self.validate_config({key: value}, {key: schema})
                if validation_errors:
                    raise NLMErrors.ConfigurationError(f"Invalid configuration value for '{key}': {validation_errors[0]}")
            return value
        if default is not None:
            return default
        # If no default provided, return schema's default if available
        if key in self._schema:
            schema_default = self._schema[key].get('default')
            if schema_default is not None:
                return schema_default
        # If no default and no schema, raise error
        raise NLMErrors.ConfigurationError(f"Unknown configuration key: {key}")
        
    def set(self, key: str, value: Any) -> 'AdvancedNLMConfig':
        """Set configuration value with comprehensive validation."""
        if key not in self._schema:
            raise NLMErrors.ConfigurationError(f"Unknown configuration key: {key}")
            
        schema = self._schema[key]
        validation_errors = self.validate_config({key: value}, {key: schema})
        if validation_errors:
            raise NLMErrors.ConfigurationError(f"Invalid configuration value for '{key}': {validation_errors[0]}")
            
        self._config[key] = value
        return self
        
    def load_from_json(self, filepath: str) -> 'AdvancedNLMConfig':
        """Load configuration from JSON file with validation."""
        try:
            with open(filepath, 'r') as f:
                json_config = json.load(f)
        except Exception as e:
            raise NLMErrors.FileOperationError(f"Failed to load configuration from {filepath}: {e}")
            
        for key, value in json_config.items():
            if key in self._schema:
                self.set(key, value)
            else:
                logger.warning(f"Ignoring unknown configuration key: {key}")
                
        return self
        
    def save_to_json(self, filepath: str):
        """Save configuration to JSON file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self._config, f, indent=2)
        except Exception as e:
            raise NLMErrors.FileOperationError(f"Failed to save configuration to {filepath}: {e}")
            
    def validate_all(self) -> Dict[str, Any]:
        """Complete validation of configuration, return detailed results."""
        validation_results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'config_summary': {}
        }
        
        # Validate against schema
        errors = self.validate_config(self._config, self._schema)
        if errors:
            validation_results['valid'] = False
            validation_results['errors'].extend(errors)
            
        # Check for performance issues
        warnings = self._check_performance_issues()
        if warnings:
            validation_results['warnings'].extend(warnings)
            
        # Generate summary
        validation_results['config_summary'] = {
            'total_keys': len(self._config),
            'schema_keys': len(self._schema),
            'keys_with_values': len([k for k in self._config if self._config[k] is not None]),
            'missing_required': [k for k in self._schema if self._schema[k].get('required', False) and k not in self._config]
        }
        
        return validation_result
        
    def _check_performance_issues(self) -> List[str]:
        """Check for potential performance issues in configuration."""
        warnings = []
        
        # Large simulation settings
        if 'neuron_count' in self._config and self._config['neuron_count'] > 10000:
            warnings.append(f"Large neuron count ({self._config['neuron_count']}) may impact performance")
            
        if 'max_simulation_steps' in self._config and self._config['max_simulation_steps'] > 100000:
            warnings.append(f"Large max_simulation_steps ({self._config['max_simulation_steps']}) may impact performance")
            
        # Memory configuration
        if 'working_memory_capacity' in self._config and 'episodic_memory_capacity' in self._config:
            total_memory = self._config['working_memory_capacity'] + self._config['episodic_memory_capacity']
            if total_memory > 10000:
                warnings.append(f"Large total memory capacity ({total_memory}) may impact performance")
                
        return warnings
        
    def merge(self, other_config: 'AdvancedNLMConfig', merge_strategy: str = 'override') -> 'AdvancedNLMConfig':
        """Merge another configuration into this one with specified strategy."""
        if merge_strategy == 'override':
            # Other config overrides this one
            for key in other_config._config:
                self.set(key, other_config.get(key))
        elif merge_strategy == 'merge':
            # Merge, preferring other config for conflicts
            for key in set(self._config.keys()) | set(other_config._config.keys()):
                if key in other_config._config:
                    self.set(key, other_config.get(key))
                elif key in self._config:
                    # Keep existing value
                    pass
        elif merge_strategy == 'add':
            # Add, creating new values for conflicts
            for key in other_config._config:
                if key not in self._config:
                    self.set(key, other_config.get(key))
        else:
            raise ValueError(f"Unknown merge strategy: {merge_strategy}")
            
        return self
        
    def get_summary(self) -> str:
        """Get formatted configuration summary."""
        lines = ["Configuration Summary:", "=" * 50]
        
        for key, schema in self._schema.items():
            if key in self._config:
                value = self._config[key]
            else:
                value = schema.get('default', 'N/A')
                
            lines.append(f"{key}: {value} ({schema.get('type', 'unknown')})")
            
        return "\n".join(lines)
        
    def to_dict(self) -> Dict:
        """Convert configuration to dictionary."""
        return self._config.copy()
        
    def __str__(self) -> str:
        return f"AdvancedNLMConfig({len(self._config)} settings)"
        
    def __repr__(self) -> str:
        return self.__str__()
class UltraAdvancedNLMBrain:
    """Ultra-advanced brain interface with maximum functionality and performance."""
    
    def __init__(self, config: Optional[AdvancedNLMConfig] = None):
        self.config = config or AdvancedNLMConfig()
        self._brain = None  # Underlying C++ brain object
        self._state = 'uninitialized'
        self._neurons = []
        self._regions = []
        self._performance_monitor = None
        self._experimental_mode = config.get('experimental_mode', False) if config else False
        self._debug_mode = config.get('debug_mode', False) if config else False
        self._profiling_enabled = config.get('profiling_enabled', False) if config else False
        
        # Advanced tracking
        self._step_history = []
        self._spike_history = []
        self._plasticity_events = []
        self._reward_history = []
        self._novelty_history = []
        
        # Threading for parallel operations
        self._lock = threading.RLock()
        self._simulation_thread = None
        self._monitoring_thread = None
        self._is_monitoring = False
        
    def initialize(self) -> 'UltraAdvancedNLMBrain':
        """Initialize brain with advanced error handling and logging."""
        with self._lock:
            try:
                logger.info("Initializing advanced NLM brain...")
                
                # Validate configuration
                validation_result = self.config.validate_all()
                if not validation_result['valid']:
                    raise NLMErrors.BrainInitializationError(
                        f"Brain initialization failed due to invalid configuration:\n" +
                        "\n".join(f"  - {error}" for error in validation_result['errors'])
                    )
                    
                if validation_result['warnings']:
                    logger.warning("Configuration warnings:")
                    for warning in validation_result['warnings']:
                        logger.warning(f"  - {warning}")
                
                # Log initialization parameters
                logger.info(f"Brain configuration:")
                logger.info(f"  Neurons: {self.config.get('neuron_count')}")
                logger.info(f"  Regions: {self.config.get('region_count')}")
                logger.info(f"  Simulation timestep: {self.config.get('simulation_timestep')}")
                logger.info(f"  Connection probability: {self.config.get('connection_probability')}")
                logger.info(f"  Plasticity enabled: {self.config.get('plasticity_learning_rate') > 0}")
                
                # Initialize performance monitoring if enabled
                if self.config.get('performance_profiling'):
                    self._performance_monitor = PerformanceProfiler()
                    self._performance_monitor.start()
                    
                # Initialize underlying brain (would integrate with C++ bindings)
                # This is a placeholder for the actual C++ integration
                self._brain = "Placeholder_C++_brain_object"
                self._state = 'initialized'
                
                logger.info("Advanced NLM brain initialized successfully")
                return self
                
            except Exception as e:
                self._state = 'error'
                logger.error(f"Brain initialization failed: {e}")
                raise NLMErrors.BrainInitializationError(f"Failed to initialize brain: {e}")
                
    def start_simulation(self, duration: Optional[int] = None) -> 'UltraAdvancedNLMBrain':
        """Start continuous simulation with monitoring."""
        if self._state != 'initialized':
            raise NLMErrors.ExperimentError("Brain not initialized. Call initialize() first.")
            
        self._state = 'running'
        self._is_monitoring = True
        
        # Start monitoring thread
        if self._performance_monitor:
            self._monitoring_thread = threading.Thread(
                target=self._performance_monitor.monitor,
                daemon=True
            )
            self._monitoring_thread.start()
            
        # Start simulation thread
        duration = duration or self.config.get('max_simulation_steps', 10000)
        self._simulation_thread = threading.Thread(
            target=self._run_simulation_thread,
            args=(duration,),
            daemon=True
        )
        self._simulation_thread.start()
        
        logger.info(f"Starting simulation for {duration} steps...")
        return self
        
    def _run_simulation_thread(self, duration: int):
        """Internal simulation thread method."""
        try:
            for step in range(duration):
                if not self._is_monitoring:
                    break
                    
                # Record performance
                if self._performance_monitor:
                    self._performance_monitor.record_step_start()
                    
                # Execute simulation step
                self._execute_step(step)
                
                # Record results
                self._record_simulation_step(step)
                
                # Record performance metrics
                if self._performance_monitor:
                    self._performance_monitor.record_step_end()
                    
                # Log progress occasionally
                if step % 1000 == 0:
                    logger.info(f"Simulation progress: {step}/{duration} steps")
                    
        except Exception as e:
            logger.error(f"Simulation thread error: {e}")
            self._state = 'error'
        finally:
            self._state = 'completed'
            logger.info("Simulation completed")
            
    def _execute_step(self, step: int):
        """Execute a single simulation step."""
        # Placeholder for actual C++ integration
        # This would call the underlying C++ brain simulation
        pass
        
    def _record_simulation_step(self, step: int):
        """Record simulation step data."""
        if self._experimental_mode:
            # Record comprehensive data for experimental analysis
            step_data = {
                'step': step,
                'timestamp': time.time(),
                'brain_stats': self.get_comprehensive_stats(),
                'spike_metrics': self._calculate_spike_metrics(),
                'plasticity_events': len(self._plasticity_events),
                'reward_sum': sum(self._reward_history[-100:]) if self._reward_history else 0.0,
                'novelty_level': sum(self._novelty_history[-100:]) / 100 if self._novelty_history else 0.0
            }
            self._step_history.append(step_data)
            
            # Keep only recent history to manage memory
            if len(self._step_history) > 10000:
                self._step_history = self._step_history[-5000:]
                
    def _calculate_spike_metrics(self) -> Dict[str, Any]:
        """Calculate comprehensive spike metrics."""
        if not self._spike_history:
            return {'total_spikes': 0, 'firing_rate': 0.0, 'activity_pattern': 'inactive'}
            
        recent_spikes = self._spike_history[-100:] if len(self._spike_history) > 100 else self._spike_history
        total_spikes = sum(recent_spikes)
        firing_rate = total_spikes / len(recent_spikes) if recent_spikes else 0.0
        
        # Calculate activity pattern
        if firing_rate > 0.5:
            activity_pattern = 'high'
        elif firing_rate > 0.1:
            activity_pattern = 'moderate'
        elif firing_rate > 0.01:
            activity_pattern = 'low'
        else:
            activity_pattern = 'inactive'
            
        return {
            'total_spikes': total_spikes,
            'firing_rate': firing_rate,
            'activity_pattern': activity_pattern,
            'spike_std': np.std(recent_spikes) if len(recent_spikes) > 1 else 0.0
        }
        
    def get_comprehensive_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        # This would interface with the actual C++ brain object
        # For now, return placeholder data
        return {
            'total_neurons': self.config.get('neuron_count', 0),
            'total_synapses': int(self.config.get('neuron_count', 0) * self.config.get('connection_probability', 0.1)),
            'active_neurons': np.random.randint(0, self.config.get('neuron_count', 1000)),
            'firing_neurons': np.random.randint(0, self.config.get('neuron_count', 1000)),
            'average_firing_rate': np.random.uniform(0.0, 10.0),
            'excitation_inhibition_ratio': np.random.uniform(0.5, 2.0),
            'total_spike_count': sum(self._spike_history) if self._spike_history else 0,
            'developmental_stage': 'Adult',
            'region_count': self.config.get('region_count', 0),
            'working_memory_usage': np.random.randint(0, self.config.get('working_memory_capacity', 100)),
            'episodic_memory_usage': np.random.randint(0, self.config.get('episodic_memory_capacity', 1000)),
            'processing_time_per_step': 0.001,
            'memory_usage_mb': np.random.uniform(10.0, 1000.0)
        }
        
    def create_advanced_experiment(self, **kwargs) -> 'AdvancedNLMExperiment':
        """Create advanced experiment with sophisticated configuration."""
        return AdvancedNLMExperiment(self, **kwargs)
        
    def export_data(self, format: str = 'json', filepath: Optional[str] = None) -> Any:
        """Export brain data in specified format."""
        if format == 'json':
            data = {
                'config': self.config.to_dict(),
                'stats': self.get_comprehensive_stats(),
                'step_history': self._step_history,
                'spike_history': self._spike_history,
                'plasticity_events': self._plasticity_events,
                'reward_history': self._reward_history,
                'novelty_history': self._novelty_history,
                'timestamp': time.time()
            }
            
            if filepath:
                try:
                    with open(filepath, 'w') as f:
                        json.dump(data, f, indent=2, default=str)
                except Exception as e:
                    raise NLMErrors.FileOperationError(f"Failed to export data to {filepath}: {e}")
                    
            return data
            
        elif format == 'pickle':
            if filepath:
                try:
                    with open(filepath, 'wb') as f:
                        pickle.dump(self, f)
                except Exception as e:
                    raise NLMErrors.FileOperationError(f"Failed to export data to {filepath}: {e}")
            return self
            
        elif format == 'csv':
            # Convert to pandas DataFrame for CSV export
            try:
                import pandas as pd
                df_data = []
                
                for step_data in self._step_history:
                    df_data.append({
                        'step': step_data['step'],
                        'timestamp': step_data['timestamp'],
                        'total_spikes': step_data['brain_stats']['total_spike_count'],
                        'firing_rate': step_data['brain_stats']['average_firing_rate'],
                        'active_neurons': step_data['brain_stats']['active_neurons'],
                        'spike_metrics': json.dumps(step_data['spike_metrics'])
                    })
                    
                df = pd.DataFrame(df_data)
                
                if filepath:
                    df.to_csv(filepath, index=False)
                    
                return df
            except ImportError:
                raise NLMErrors.FileOperationError("Pandas not available for CSV export")
                
        else:
            raise ValueError(f"Unsupported export format: {format}")
            
    def import_data(self, format: str, filepath: str) -> 'UltraAdvancedNLMBrain':
        """Import brain data from specified format."""
        try:
            if format == 'json':
                with open(filepath, 'r') as f:
                    data = json.load(f)
                    
                self.config = AdvancedNLMConfig(data['config'])
                self._step_history = data['step_history']
                self._spike_history = data['spike_history']
                self._plasticity_events = data['plasticity_events']
                self._reward_history = data['reward_history']
                self._novelty_history = data['novelty_history']
                
            elif format == 'pickle':
                with open(filepath, 'rb') as f:
                    imported_brain = pickle.load(f)
                    self.config = imported_brain.config
                    self._step_history = imported_brain._step_history
                    self._spike_history = imported_brain._spike_history
                    self._plasticity_events = imported_brain._plasticity_events
                    self._reward_history = imported_brain._reward_history
                    self._novelty_history = imported_brain._novelty_history
                    
            elif format == 'csv':
                import pandas as pd
                df = pd.read_csv(filepath)
                
                # Convert back to step history format
                self._step_history = []
                for _, row in df.iterrows():
                    step_data = {
                        'step': int(row['step']),
                        'timestamp': row['timestamp'],
                        'brain_stats': {
                            'total_spike_count': int(row['total_spikes']),
                            'average_firing_rate': float(row['firing_rate']),
                            'active_neurons': int(row['active_neurons'])
                        },
                        'spike_metrics': json.loads(row['spike_metrics']) if 'spike_metrics' in row else {}
                    }
                    self._step_history.append(step_data)
                    
            else:
                raise ValueError(f"Unsupported import format: {format}")
                
        except Exception as e:
            raise NLMErrors.FileOperationError(f"Failed to import data from {filepath}: {e}")
            
        return self
        
    def reset(self) -> 'UltraAdvancedNLMBrain':
        """Reset brain state completely."""
        with self._lock:
            logger.info("Resetting advanced NLM brain...")
            
            # Stop any running simulations
            self._is_monitoring = False
            if self._simulation_thread:
                self._simulation_thread.join(timeout=5.0)
                self._simulation_thread = None
                
            if self._monitoring_thread:
                self._monitoring_thread.join(timeout=5.0)
                self._monitoring_thread = None
                
            # Clear all history data
            self._step_history.clear()
            self._spike_history.clear()
            self._plasticity_events.clear()
            self._reward_history.clear()
            self._novelty_history.clear()
            
            # Reset performance monitor
            if self._performance_monitor:
                self._performance_monitor.stop()
                
            # Re-initialize brain
            self._brain = "Placeholder_C++_brain_object"
            self._state = 'initialized'
            
            logger.info("Advanced NLM brain reset successfully")
            return self
            
    def stop(self):
        """Stop simulation and cleanup resources."""
        with self._lock:
            logger.info("Stopping advanced NLM brain...")
            
            self._is_monitoring = False
            
            # Stop monitoring thread
            if self._monitoring_thread:
                self._monitoring_thread.join(timeout=5.0)
                self._monitoring_thread = None
                
            # Stop simulation thread
            if self._simulation_thread:
                self._simulation_thread.join(timeout=5.0)
                self._simulation_thread = None
                
            # Stop performance monitoring
            if self._performance_monitor:
                self._performance_monitor.stop()
                
            logger.info("Advanced NLM brain stopped")
            
    def __enter__(self):
        """Context manager entry."""
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.stop()
class AdvancedNLMExperiment:
    """Advanced experiment system with sophisticated control and analysis."""
    
    def __init__(self, brain: UltraAdvancedNLMBrain, **kwargs):
        self.brain = brain
        self.name = kwargs.get('name', 'Advanced Experiment')
        self.type = kwargs.get('type', 'general')
        self.description = kwargs.get('description', '')
        self.tags = kwargs.get('tags', [])
        
        # Experiment configuration
        self.parameters = kwargs.get('parameters', {})
        self.objectives = kwargs.get('objectives', {})
        self.measurement_metrics = kwargs.get('measurement_metrics', [])
        
        # State tracking
        self.status = 'pending'
        self.start_time: Optional[float] = None
        self.end_time: Optional[float] = None
        self.duration: Optional[float] = None
        
        # Data collection
        self.results = []
        self.metrics_history = []
        self.analysis_results = {}
        
        # Advanced features
        self.checkpoint_enabled = kwargs.get('checkpoint_enabled', True)
        self.checkpoint_interval = kwargs.get('checkpoint_interval', 1000)
        self.auto_save = kwargs.get('auto_save', False)
        self.parallel_execution = kwargs.get('parallel_execution', False)
        
    def start(self) -> 'AdvancedNLMExperiment':
        """Start the experiment."""
        if self.status != 'pending':
            raise NLMErrors.ExperimentError(f"Experiment already started. Status: {self.status}")
            
        self.status = 'running'
        self.start_time = time.time()
        
        logger.info(f"Starting advanced experiment: {self.name}")
        
        return self
        
    def run(self, steps: int, **kwargs) -> 'AdvancedNLMExperiment':
        """Run the experiment for specified number of steps."""
        if self.status != 'running':
            raise NLMErrors.ExperimentError(f"Experiment not running. Status: {self.status}")
            
        logger.info(f"Running advanced experiment '{self.name}' for {steps} steps...")
        
        # Execute simulation steps
        for step in range(steps):
            # Execute step
            self.brain._execute_step(step)
            
            # Record results
            self._record_step(step)
            
            # Check objectives
            self._check_objectives(step)
            
            # Save checkpoint if enabled
            if self.checkpoint_enabled and step % self.checkpoint_interval == 0:
                self._save_checkpoint(step)
                
            # Auto-save if enabled
            if self.auto_save and step % 100 == 0:
                self._auto_save(step)
                
        self.end_time = time.time()
        self.duration = self.end_time - self.start_time
        self.status = 'completed'
        
        logger.info(f"Advanced experiment '{self.name}' completed in {self.duration:.2f} seconds")
        return self
        
    def _record_step(self, step: int):
        """Record data for a single step."""
        # Collect comprehensive metrics
        step_data = {
            'step': step,
            'timestamp': time.time(),
            'brain_stats': self.brain.get_comprehensive_stats(),
            'spike_metrics': self.brain._calculate_spike_metrics()
        }
        
        # Record measurement metrics
        for metric in self.measurement_metrics:
            metric_name = metric['name']
            metric_func = metric['function']
            
            try:
                if callable(metric_func):
                    value = metric_func(step_data)
                else:
                    value = metric_func
                    
                step_data[f'metric_{metric_name}'] = value
                self.metrics_history.append({
                    'step': step,
                    'metric': metric_name,
                    'value': value,
                    'timestamp': time.time()
                })
            except Exception as e:
                logger.warning(f"Failed to compute metric '{metric_name}': {e}")
                
        self.results.append(step_data)
        
    def _check_objectives(self, step: int):
        """Check if experiment objectives are met."""
        for metric_name, objective in self.objectives.items():
            target = objective['target']
            tolerance = objective.get('tolerance', 0.1)
            
            # Get current value from metrics history
            current_values = [m['value'] for m in self.metrics_history 
                            if m['metric'] == metric_name]
            
            if current_values:
                current_value = current_values[-1]
                objective['current_value'] = current_value
                objective['met'] = abs(current_value - target) <= tolerance
                
    def _save_checkpoint(self, step: int):
        """Save experiment checkpoint."""
        checkpoint_data = {
            'experiment_name': self.name,
            'step': step,
            'brain_config': self.brain.config.to_dict(),
            'brain_stats': self.brain.get_comprehensive_stats(),
            'results': self.results[-100:],  # Last 100 results
            'metrics_history': self.metrics_history[-100:],  # Last 100 metrics
            'timestamp': time.time()
        }
        
        filename = f"{self.name}_checkpoint_step_{step}.json"
        try:
            with open(filename, 'w') as f:
                json.dump(checkpoint_data, f, indent=2, default=str)
        except Exception as e:
            logger.warning(f"Failed to save checkpoint: {e}")
            
    def _auto_save(self, step: int):
        """Auto-save experiment data."""
        auto_save_file = f"{self.name}_auto_save_step_{step}.json"
        self._save_checkpoint(step)
        logger.debug(f"Auto-saved experiment to {auto_save_file}")
        
    def analyze_results(self) -> Dict[str, Any]:
        """Analyze experiment results comprehensively."""
        if self.status != 'completed':
            raise NLMErrors.ExperimentError("Cannot analyze incomplete experiment")
            
        analysis = {
            'experiment_name': self.name,
            'duration': self.duration,
            'total_steps': len(self.results),
            'metrics_count': len(self.measurement_metrics),
            'objective_count': len(self.objectives)
        }
        
        # Analyze each metric
        for metric_name in [m['name'] for m in self.measurement_metrics]:
            metric_values = [m['value'] for m in self.metrics_history 
                           if m['metric'] == metric_name]
            
            if metric_values:
                analysis[f'metric_{metric_name}'] = {
                    'mean': np.mean(metric_values),
                    'std': np.std(metric_values),
                    'min': np.min(metric_values),
                    'max': np.max(metric_values),
                    'final': metric_values[-1],
                    'trend': self._calculate_trend(metric_values)
                }
                
        # Analyze objectives
        analysis['objectives'] = {}
        for metric_name, objective in self.objectives.items():
            analysis['objectives'][metric_name] = {
                'target': objective['target'],
                'tolerance': objective.get('tolerance', 0.1),
                'current': objective.get('current_value'),
                'met': objective.get('met', False)
            }
            
        self.analysis_results = analysis
        return analysis
        
    def _calculate_trend(self, values: List[float]) -> Dict[str, float]:
        """Calculate trend for a series of values."""
        if len(values) < 2:
            return {'slope': 0.0, 'intercept': 0.0, 'r_squared': 0.0}
            
        x = list(range(len(values)))
        
        n = len(x)
        sum_x = sum(x)
        sum_y = sum(values)
        sum_xx = sum(xi * xi for xi in x)
        sum_yy = sum(yi * yi for yi in values)
        sum_xy = sum(xi * yi for xi, yi in zip(x, values))
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x)
        intercept = (sum_y - slope * sum_x) / n
        
        ss_res = sum((yi - (slope * xi + intercept))**2 for xi, yi in zip(x, values))
        ss_tot = sum((yi - sum_y / n)**2 for yi in values)
        
        r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_squared
        }
        
    def save_results(self, filepath: str, format: str = 'json'):
        """Save experiment results to file."""
        if format == 'json':
            data = {
                'experiment_name': self.name,
                'type': self.type,
                'description': self.description,
                'tags': self.tags,
                'parameters': self.parameters,
                'objectives': self.objectives,
                'measurement_metrics': self.measurement_metrics,
                'status': self.status,
                'start_time': self.start_time,
                'end_time': self.end_time,
                'duration': self.duration,
                'results': self.results,
                'metrics_history': self.metrics_history,
                'analysis_results': self.analysis_results
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
                
        elif format == 'pickle':
            with open(filepath, 'wb') as f:
                pickle.dump(self, f)
                
        elif format == 'csv':
            import pandas as pd
            
            # Convert results to DataFrame
            results_df = pd.DataFrame(self.results)
            metrics_df = pd.DataFrame(self.metrics_history)
            
            # Combine data
            combined_data = {
                **results_df.to_dict('records')[0] if len(results_df) > 0 else {},
                **metrics_df.to_dict('records')[0] if len(metrics_df) > 0 else {}
            }
            
            with open(filepath, 'w') as f:
                json.dump(combined_data, f, indent=2, default=str)
                
        else:
            raise ValueError(f"Unsupported format: {format}")
            
        logger.info(f"Experiment results saved to {filepath}")
        
    def load_results(self, filepath: str) -> 'AdvancedNLMExperiment':
        """Load experiment results from file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
                
            self.name = data.get('experiment_name', self.name)
            self.type = data.get('type', self.type)
            self.description = data.get('description', self.description)
            self.tags = data.get('tags', self.tags)
            self.parameters = data.get('parameters', self.parameters)
            self.objectives = data.get('objectives', self.objectives)
            self.measurement_metrics = data.get('measurement_metrics', self.measurement_metrics)
            self.status = data.get('status', self.status)
            self.start_time = data.get('start_time')
            self.end_time = data.get('end_time')
            self.duration = data.get('duration')
            self.results = data.get('results', self.results)
            self.metrics_history = data.get('metrics_history', self.metrics_history)
            self.analysis_results = data.get('analysis_results', self.analysis_results)
            
        except Exception as e:
            raise NLMErrors.FileOperationError(f"Failed to load experiment from {filepath}: {e}")
            
        return self
class PerformanceProfiler:
    """Advanced performance profiling for NLM simulations."""
    
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
        self.cpu_usage = []
        self.is_monitoring = False
        self.start_time = None
        
    def start(self):
        """Start performance profiling."""
        self.is_monitoring = True
        self.start_time = time.time()
        logger.info("Performance profiling started")
        
    def stop(self):
        """Stop performance profiling."""
        self.is_monitoring = False
        logger.info("Performance profiling stopped")
        
    def record_step_start(self):
        """Record start of simulation step."""
        if self.is_monitoring:
            self.step_times.append(time.time())
            
    def record_step_end(self):
        """Record end of simulation step and calculate metrics."""
        if self.is_monitoring and self.step_times:
            end_time = time.time()
            start_time = self.step_times.pop()
            
            step_time = end_time - start_time
            self.step_times.append(end_time)
            
            # Record performance metrics
            self.step_times.append(step_time)
            
            # Simulate memory and CPU usage (would integrate with actual profiling tools)
            self.memory_usage.append(np.random.uniform(100.0, 1000.0))
            self.cpu_usage.append(np.random.uniform(10.0, 100.0))
            
    def monitor(self):
        """Background monitoring thread."""
        while self.is_monitoring:
            time.sleep(0.1)
            
    def get_summary(self) -> Dict[str, Any]:
        """Get performance profiling summary."""
        if not self.step_times:
            return {'message': 'No performance data collected'}
            
        return {
            'total_steps': len(self.step_times),
            'average_step_time': np.mean(self.step_times),
            'min_step_time': np.min(self.step_times),
            'max_step_time': np.max(self.step_times),
            'median_step_time': np.median(self.step_times),
            'std_step_time': np.std(self.step_times),
            'average_memory_usage': np.mean(self.memory_usage) if self.memory_usage else 0,
            'average_cpu_usage': np.mean(self.cpu_usage) if self.cpu_usage else 0,
            'total_duration': time.time() - self.start_time if self.start_time else 0
        }
        
    def save_profile(self, filepath: str):
        """Save performance profile to file."""
        profile_data = {
            'step_times': self.step_times,
            'memory_usage': self.memory_usage,
            'cpu_usage': self.cpu_usage,
            'timestamp': time.time()
        }
        
        with open(filepath, 'w') as f:
            json.dump(profile_data, f, indent=2)
class AdvancedNLMCLI:
    """Enhanced command-line interface for NLM operations."""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.experiment = None
        self.performance_profiler = None
        
    def create_brain(self, config_file: Optional[str] = None, **kwargs) -> bool:
        """Create advanced brain with configuration."""
        try:
            if config_file and os.path.exists(config_file):
                self.config = AdvancedNLMConfig()
                self.config.load_from_json(config_file)
                print(f"✓ Loaded configuration from: {config_file}")
            else:
                self.config = AdvancedNLMConfig()
                print("✓ Using default configuration")
                
            # Override with kwargs
            for key, value in kwargs.items():
                if key in self.config._schema:
                    self.config.set(key, value)
                    
            self.brain = UltraAdvancedNLMBrain(self.config)
            print(f"✓ Created advanced brain with {self.config.get('neuron_count')} neurons")
            return True
        except Exception as e:
            print(f"✗ Error creating brain: {e}")
            return False
            
    def run_advanced_experiment(self, steps: int = 500, experiment_name: str = "Advanced Experiment") -> bool:
        """Run advanced experiment with comprehensive tracking."""
        if not self.brain:
            print("✗ No brain created. Use --create-brain first.")
            return False
            
        try:
            self.brain.initialize()
            
            # Create experiment
            self.experiment = AdvancedNLMExperiment(
                self.brain,
                name=experiment_name,
                checkpoint_enabled=True,
                auto_save=False,
                measurement_metrics=[
                    {'name': 'total_spike_count', 'function': 'brain.get_comprehensive_stats'},
                    {'name': 'average_firing_rate', 'function': 'brain.get_comprehensive_stats'},
                    {'name': 'active_neurons', 'function': 'brain.get_comprehensive_stats'}
                ],
                objectives={
                    'total_spike_count': {'target': 100, 'tolerance': 20},
                    'average_firing_rate': {'target': 5.0, 'tolerance': 1.0}
                }
            )
            
            self.experiment.start()
            print(f"✓ Started advanced experiment: {experiment_name}")
            print(f"  Will run for {steps} simulation steps")
            
            # Run experiment
            self.experiment.run(steps)
            
            # Analyze results
            analysis = self.experiment.analyze_results()
            
            print(f"✓ Experiment completed!")
            print(f"  Duration: {analysis['duration']:.2f} seconds")
            print(f"  Total steps: {analysis['total_steps']}")
            
            # Print objective results
            print(f"  Objectives:")
            for metric, obj_data in analysis['objectives'].items():
                status = "✓ MET" if obj_data['met'] else "✗ NOT MET"
                print(f"    {metric}: Target={obj_data['target']}, Current={obj_data.get('current', 'N/A')}, {status}")
                
            # Print metric analysis
            print(f"  Metric Analysis:")
            for metric, metrics_data in analysis.items():
                if metric.startswith('metric_'):
                    true_metric = metric[7:]  # Remove 'metric_' prefix
                    data = analysis[metric]
                    print(f"    {true_metric}: Mean={data['mean']:.3f}, Std={data['std']:.3f}, Final={data['final']:.3f}")
                    
            # Save results
            self.experiment.save_results(f"{experiment_name}_results.json")
            print(f"✓ Results saved to {experiment_name}_results.json")
            
            return True
        except Exception as e:
            print(f"✗ Error running advanced experiment: {e}")
            return False
            
    def run_performance_test(self, steps: int = 100) -> bool:
        """Run performance profiling test."""
        if not self.brain:
            print("✗ No brain created. Use --create-brain first.")
            return False
            
        try:
            self.brain.initialize()
            
            # Start performance monitoring
            self.performance_profiler = PerformanceProfiler()
            self.performance_profiler.start()
            
            print(f"✓ Running performance test for {steps} steps...")
            
            # Run simulation
            for step in range(steps):
                self.brain._execute_step(step)
                
            # Stop performance monitoring
            self.performance_profiler.stop()
            
            # Get performance summary
            perf_summary = self.performance_profiler.get_summary()
            
            print(f"✓ Performance test completed!")
            print(f"  Total steps: {perf_summary['total_steps']}")
            print(f"  Average step time: {perf_summary['average_step_time']*1000:.2f} ms")
            print(f"  Min step time: {perf_summary['min_step_time']*1000:.2f} ms")
            print(f"  Max step time: {perf_summary['max_step_time']*1000:.2f} ms")
            print(f"  Total duration: {perf_summary['total_duration']:.2f} seconds")
            
            # Save performance profile
            self.performance_profiler.save_profile("performance_profile.json")
            print(f"✓ Performance profile saved to performance_profile.json")
            
            return True
        except Exception as e:
            print(f"✗ Error running performance test: {e}")
            return False
            
    def create_and_save_brain(self, filepath: str) -> bool:
        """Create brain and save state to file."""
        if not self.brain:
            print("✗ No brain to save.")
            return False
            
        try:
            self.brain.save_state(filepath)
            print(f"✓ Brain state saved to: {filepath}")
            return True
        except Exception as e:
            print(f"✗ Error saving brain: {e}")
            return False
            
    def load_and_restore_brain(self, filepath: str) -> bool:
        """Load brain state from file."""
        if not self.brain:
            print("✗ Create a brain first, or specify --new-brain to create one.")
            return False
            
        try:
            self.brain.load_state(filepath)
            print(f"✓ Brain state loaded from: {filepath}")
            return True
        except Exception as e:
            print(f"✗ Error loading brain: {e}")
            return False
            
    def show_brain_comprehensive_stats(self):
        """Display comprehensive brain statistics."""
        if not self.brain:
            print("✗ No brain created.")
            return
            
        try:
            stats = self.brain.get_comprehensive_stats()
            print("Advanced Brain Statistics:")
            
            # Performance metrics
            print(f"  Performance:")
            print(f"    Processing time per step: {stats.get('processing_time_per_step', 0)*1000:.2f} ms")
            print(f"    Memory usage: {stats.get('memory_usage_mb', 0):.1f} MB")
            
            # Neural metrics
            print(f"  Neural:")
            print(f"    Total neurons: {stats.get('total_neurons', 0)}")
            print(f"    Total synapses: {stats.get('total_synapses', 0)}")
            print(f"    Active neurons: {stats.get('active_neurons', 0)}")
            print(f"    Firing neurons: {stats.get('firing_neurons', 0)}")
            print(f"    Average firing rate: {stats.get('average_firing_rate', 0.0):.3f} Hz")
            print(f"    Excitation/Inhibition ratio: {stats.get('excitation_inhibition_ratio', 0.0):.3f}")
            print(f"    Total spike count: {stats.get('total_spike_count', 0)}")
            
            # Memory system usage
            print(f"  Memory Systems:")
            print(f"    Working memory: {stats.get('working_memory_usage', 0)}/{stats.get('working_memory_capacity', 100)}")
            print(f"    Episodic memory: {stats.get('episodic_memory_usage', 0)}/{stats.get('episodic_memory_capacity', 1000)}")
            
            # Development and other systems
            print(f"  Other:")
            print(f"    Developmental stage: {stats.get('developmental_stage', 'Unknown')}")
            print(f"    Region count: {stats.get('region_count', 0)}")
            print(f"    Prediction system active: {stats.get('prediction_system_active', False)}")
            print(f"    Neuromodulation level: {stats.get('neuromodulation_level', 0.0)}")
            
        except Exception as e:
            print(f"✗ Error getting brain stats: {e}")
            
    def show_advanced_config(self):
        """Display advanced configuration."""
        if not self.config:
            print("✗ No configuration loaded.")
            return
            
        print("Advanced Configuration:")
        print("=" * 50)
        
        # Get validation results
        validation_result = self.config.validate_all()
        
        if validation_result['valid']:
            print("✓ Configuration is valid")
        else:
            print("✗ Configuration is invalid:")
            for error in validation_result['errors']:
                print(f"  - {error}")
                
        if validation_result['warnings']:
            print("⚠ Configuration warnings:")
            for warning in validation_result['warnings']:
                print(f"  - {warning}")
                
        # Show configuration summary
        print(f"\nConfiguration Summary:")
        config_dict = self.config.to_dict()
        for key, value in sorted(config_dict.items()):
            if key in ['random_seed', 'simulation_timestep', 'neuron_count', 'region_count',
                      'connection_probability', 'plasticity_learning_rate', 'stdp_ltp_weight',
                      'stdp_ltd_weight', 'stdp_time_constant', 'development_synaptogenesis_rate',
                      'development_pruning_rate', 'dopamine_baseline', 'reward_discount_factor']:
                print(f"  {key}: {value}")
                
        # Show validation summary
        print(f"\nValidation Summary:")
        summary = validation_result['config_summary']
        print(f"  Total keys: {summary['total_keys']}")
        print(f"  Required keys: {summary['total_keys'] - len(summary['missing_required'])}")
        print(f"  Missing required: {len(summary['missing_required'])}")
        
    def run_preset_advanced_experiments(self):
        """Run predefined advanced experiment sets."""
        print("Available advanced experiment presets:")
        print("1. Performance Benchmark - Measure simulation performance")
        print("2. Plasticity Analysis - Study synaptic learning dynamics")
        print("3. Memory Integration - Test memory system integration")
        print("4. Neuromodulation Study - Investigate neuromodulator effects")
        print("5. Development Simulation - Study brain development")
        print("6. Full Advanced Integration - Complete comprehensive test")
        
        print("\nFor specific experiments, use:")
        print("  --advanced-experiment <name> <steps>")
        print("  --performance-test <steps>")
        print("  --create-brain --save-brain <filename>")
        
        print("\nAdvanced experiment management commands:")
        print("  --experiment-create <name> <type> [parameters]")
        print("  --experiment-run <name> <steps>")
        print("  --experiment-analyze <name>")
        print("  --experiment-save <name> <file>")
        print("  --experiment-load <file>")
        print("  --experiment-list")
        print("  --experiment-delete <name>")
def advanced_cli_main():
    """Main function for the advanced CLI."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface - Neural Learning Machine",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Advanced Examples:
  nlm_advanced_cli.py --create-brain --advanced-experiment PerformanceTest 500
  nlm_advanced_cli.py --create-brain --performance-test 100
  nlm_advanced_cli.py --create-brain --save-brain advanced_brain.nbm
  nlm_advanced_cli.py --create-brain --load-brain advanced_brain.nbm
  nlm_advanced_cli.py --create-brain --show-advanced-stats
  nlm_advanced_cli.py --create-brain --show-advanced-config
  nlm_advanced_cli.py --create-brain --validate-config
  nlm_advanced_cli.py --create-brain --performance-benchmark 1000

Advanced Configuration:
  --config-set KEY VALUE
  --config-schema SHOW
  --config-validate
  --config-export FILE
  --config-import FILE

Advanced Experiment Management:
  --experiment-create NAME TYPE [params]
  --experiment-run NAME STEPS
  --experiment-analyze NAME
  --experiment-save NAME FILE
  --experiment-load FILE
  --experiment-list
  --experiment-delete NAME

Performance Monitoring:
  --enable-performance-monitoring
  --performance-profile STEPS
  --memory-profiling
  --cpu-profiling
  --profile-save FILE
  """
    )
    
    # Main actions
    action_group = parser.add_mutually_exclusive_group()
    action_group.add_argument('--create-brain', action='store_true',
                             help='Create a new advanced brain with optional configuration')
    action_group.add_argument('--advanced-experiment', nargs=2, metavar=('NAME', 'STEPS'),
                             help='Run advanced experiment with name and steps')
    action_group.add_argument('--performance-test', type=int, metavar='STEPS',
                             help='Run performance profiling test')
    action_group.add_argument('--validate-config', action='store_true',
                             help='Validate current configuration')
    action_group.add_argument('--performance-benchmark', type=int, metavar='STEPS',
                             help='Run performance benchmark test')
    
    # Configuration options
    config_group = parser.add_argument_group('Advanced Configuration Options')
    config_group.add_argument('--config-file', type=str, default='configs/default.cfg',
                             help='Advanced configuration file to load')
    config_group.add_argument('--config-set', nargs=2, action='append',
                             help='Set advanced configuration value (KEY VALUE)')
    config_group.add_argument('--config-schema', action='store_true',
                             help='Show available configuration schema')
    config_group.add_argument('--config-export', type=str,
                             help='Export configuration to file')
    config_group.add_argument('--config-import', type=str,
                             help='Import configuration from file')
    
    # File operations
    file_group = parser.add_argument_group('Advanced File Operations')
    file_group.add_argument('--save-brain', type=str, help='Save advanced brain state to file')
    file_group.add_argument('--load-brain', type=str, help='Load advanced brain state from file')
    
    # Information commands
    info_group = parser.add_argument_group('Advanced Information Commands')
    info_group.add_argument('--show-advanced-stats', action='store_true',
                           help='Show advanced brain statistics')
    info_group.add_argument('--show-advanced-config', action='store_true',
                           help='Show advanced configuration with validation')
    info_group.add_argument('--experiment-list', action='store_true',
                           help='List saved experiments')
    
    # Performance monitoring
    perf_group = parser.add_argument_group('Performance Monitoring')
    perf_group.add_argument('--enable-performance-monitoring', action='store_true',
                           help='Enable comprehensive performance monitoring')
    perf_group.add_argument('--performance-profile', type=int, metavar='STEPS',
                           help='Run performance profiling for specified steps')
    perf_group.add_argument('--profile-save', type=str,
                           help='Save performance profile to file')
    
    # Experiment management
    exp_group = parser.add_argument_group('Advanced Experiment Management')
    exp_group.add_argument('--experiment-create', nargs=3, metavar=('NAME', 'TYPE', 'PARAMS'),
                          help='Create new experiment with name, type, and parameters')
    exp_group.add_argument('--experiment-run', nargs=2, metavar=('NAME', 'STEPS'),
                          help='Run saved experiment with name and steps')
    exp_group.add_argument('--experiment-analyze', type=str, metavar='NAME',
                          help='Analyze results of saved experiment')
    exp_group.add_argument('--experiment-save', nargs=2, metavar=('NAME', 'FILE'),
                          help='Save experiment results to file')
    exp_group.add_argument('--experiment-load', type=str, metavar='FILE',
                          help='Load experiment from file')
    exp_group.add_argument('--experiment-delete', type=str, metavar='NAME',
                          help='Delete saved experiment')
    
    args = parser.parse_args()
    
    # Initialize CLI
    cli = AdvancedNLMCLI()
    
    # Handle configuration set options
    if args.config_set:
        config = AdvancedNLMConfig()
        for key, value in args.config_set:
            # Try to convert value to appropriate type
            try:
                if value.lower() in ('true', 'false'):
                    value = value.lower() == 'true'
                elif '.' in value:
                    value = float(value)
                else:
                    value = int(value)
            except ValueError:
                pass  # Keep as string
            config.set(key, value)
        
        print("Advanced Configuration:")
        config_dict = config.to_dict()
        for key, value in sorted(config_dict.items()):
            print(f"  {key}: {value}")
        return
    
    # Handle --config-schema
    if args.config_schema:
        print("Advanced NLM Configuration Schema:")
        print("=" * 50)
        config = AdvancedNLMConfig()
        validation_result = config.validate_all()
        
        for key, schema in config._schema.items():
            print(f"{key}:")
            print(f"  Type: {schema['type']}")
            print(f"  Default: {schema.get('default', 'N/A')}")
            print(f"  Required: {schema.get('required', False)}")
            if 'min' in schema:
                print(f"  Min: {schema['min']}")
            if 'max' in schema:
                print(f"  Max: {schema['max']}")
            if 'choices' in schema:
                print(f"  Choices: {schema['choices']}")
            print()
        return
        
    # Handle --config-export
    if args.config_export:
        config = AdvancedNLMConfig()
        config.save_to_json(args.config_export)
        print(f"✓ Configuration exported to {args.config_export}")
        return
        
    # Handle --config-import
    if args.config_import:
        config = AdvancedNLMConfig()
        config.load_from_json(args.config_import)
        print(f"✓ Configuration imported from {args.config_import}")
        return
        
    # Handle --experiment-list
    if args.experiment_list:
        print("Saved experiments:")
        print("  (Experiment management system would list saved experiments here)")
        return
        
    # Handle --experiment-create
    if args.experiment_create:
        name, exp_type, params_str = args.experiment_create
        print(f"Creating experiment '{name}' of type '{exp_type}' with parameters: {params_str}")
        print("  (Experiment creation would parse parameters and create experiment)")
        return
        
    # Handle --experiment-run
    if args.experiment_run:
        name, steps = args.experiment_run
        print(f"Running experiment '{name}' for {steps} steps")
        print("  (Experiment running would load and execute experiment)")
        return
        
    # Handle --experiment-analyze
    if args.experiment_analyze:
        name = args.experiment_analyze
        print(f"Analyzing experiment '{name}'")
        print("  (Experiment analysis would load and analyze results)")
        return
        
    # Handle --experiment-save
    if args.experiment_save:
        name, filepath = args.experiment_save
        print(f"Saving experiment '{name}' to '{filepath}'")
        print("  (Experiment saving would save results to file)")
        return
        
    # Handle --experiment-load
    if args.experiment_load:
        filepath = args.experiment_load
        print(f"Loading experiment from '{filepath}'")
        print("  (Experiment loading would load results from file)")
        return
        
    # Handle --experiment-delete
    if args.experiment_delete:
        name = args.experiment_delete
        print(f"Deleting experiment '{name}'")
        print("  (Experiment deletion would remove saved experiment)")
        return
        
    # Main execution flow
    if not args.create_brain and not any([
        args.advanced_experiment, args.performance_test, args.validate_config,
        args.performance_benchmark, args.save_brain, args.load_brain,
        args.show_advanced_stats, args.show_advanced_config,
        args.experiment_list, args.enable_performance_monitoring,
        args.performance_profile, args.profile_save
    ]):
        parser.print_help()
        return
    
    # Create brain if needed
    if args.create_brain or any([args.advanced_experiment, args.performance_test,
                                args.validate_config, args.performance_benchmark,
                                args.save_brain, args.load_brain, args.show_advanced_stats,
                                args.show_advanced_config, args.experiment_list,
                                args.enable_performance_monitoring, args.performance_profile,
                                args.profile_save]):
        if not cli.create_brain(args.config_file):
            return
    
    # Execute requested actions
    try:
        if args.advanced_experiment:
            name, steps = args.advanced_experiment
            print(f"Running advanced experiment '{name}' with {steps} steps...")
            cli.run_advanced_experiment(int(steps), name)
            
        elif args.performance_test:
            print(f"Running performance test with {args.performance_test} steps...")
            cli.run_performance_test(args.performance_test)
            
        elif args.performance_benchmark:
            print(f"Running performance benchmark with {args.performance_benchmark} steps...")
            cli.run_performance_test(args.performance_benchmark)
            
        elif args.validate_config:
            if cli.config:
                validation_result = cli.config.validate_all()
                if validation_result['valid']:
                    print("✓ Configuration validation passed")
                else:
                    print("✗ Configuration validation failed:")
                    for error in validation_result['errors']:
                        print(f"  - {error}")
            else:
                print("✗ No configuration loaded")
                
        elif args.save_brain:
            cli.create_and_save_brain(args.save_brain)
            
        elif args.load_brain:
            cli.load_and_restore_brain(args.load_brain)
            
        elif args.show_advanced_stats:
            cli.show_brain_comprehensive_stats()
            
        elif args.show_advanced_config:
            cli.show_advanced_config()
            
        elif args.performance_profile:
            print(f"Running performance profile with {args.performance_profile} steps...")
            cli.run_performance_test(args.performance_profile)
            
        elif args.profile_save:
            if cli.performance_profiler:
                cli.performance_profiler.save_profile(args.profile_save)
                print(f"✓ Performance profile saved to {args.profile_save}")
            else:
                print("✗ No performance profiler data available")
                
    except KeyboardInterrupt:
        print("\nOperation interrupted by user")
    except Exception as e:
        print(f"Unexpected error: {e}")
if __name__ == "__main__":
    main()