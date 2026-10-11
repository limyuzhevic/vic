# NLM Advanced Configuration and Error Handling Examples

This document provides advanced examples for using the NLM Python API with enhanced error handling, configuration management, and batch processing.

## Table of Contents

1. [JSON Configuration File Examples](#json-configuration-file-examples)
2. [Advanced Error Handling and Recovery](#advanced-error-handling-and-recovery)
3. [Batch Processing with Configuration Management](#batch-processing-with-configuration-management)
4. [Performance Monitoring and Optimization](#performance-monitoring-and-optimization)
5. [Checkpoint and Recovery System](#checkpoint-and-recovery-system)
6. [Multi-Agent System Examples](#multi-agent-system-examples)
7. [Real-Time Control and Integration Examples](#real-time-control-and-integration-examples)
8. [Experimental Protocol Examples](#experimental-protocol-examples)

---

## JSON Configuration File Examples

### Example 1: Complete JSON Configuration Template

```python
import pynlm
import json
import os

def create_default_config():
    """Create a comprehensive JSON configuration for NLM."""
    config = {
        "brain": {
            "neuron_count": 2000,
            "region_count": 4,
            "connection_probability": 0.08,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "v_reset": -75.0,
            "tau_mem": 20.0,
            "tau_ref": 2.0
        },
        "plasticity": {
            "stdp": {
                "enable": True,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enable": True,
                "learning_rate": 0.0005
            },
            "structural": {
                "enable": True,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.0
            },
            "curiosity": {
                "enable": True,
                "decay_rate": 0.001
            },
            "novelty": {
                "enable": True,
                "threshold": 0.5
            },
            "prediction_error": {
                "enable": True,
                "sensitivity": 0.1
            }
        },
        "development": {
            "stages": {
                "initial_plasticity": 1.0,
                "critical_period": 0.8,
                "maturation": 0.5,
                "adult": 0.2,
                "aging": 0.1
            },
            "checkpoint_interval": 1000,
            "consolidation_interval": 5000
        },
        "memory": {
            "working_memory": {
                "capacity": 200,
                "decay_rate": 0.01
            },
            "episodic": {
                "max_episodes": 5000,
                "replay_interval": 100,
                "consolidation_rate": 0.3
            }
        },
        "world": {
            "simple_world": {
                "width": 30,
                "height": 30,
                "vision_width": 12,
                "vision_height": 12
            }
        },
        "advanced": {
            "checkpoint_system": {
                "enabled": True,
                "compression": "balanced",
                "max_checkpoints": 100
            },
            "performance_optimization": {
                "enable_memory_pools": True,
                "enable_simd": True,
                "enable_multithreading": True,
                "thread_count": 4
            }
        }
    }
    
    return config

def save_config_to_file(config, filename):
    """Save configuration to JSON file with error handling."""
    try:
        # Validate configuration
        if not is_valid_config(config):
            raise ConfigurationError("Invalid configuration structure")
        
        # Create backup if file exists
        if os.path.exists(filename):
            backup_path = filename + ".backup"
            os.rename(filename, backup_path)
            print(f"Created backup: {backup_path}")
        
        # Write configuration
        with open(filename, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Configuration saved to {filename}")
        return True
        
    except Exception as e:
        print(f"Error saving configuration: {e}")
        # Try to restore backup if available
        if os.path.exists(filename + ".backup"):
            try:
                os.rename(filename + ".backup", filename)
                print("Restored original configuration")
            except:
                pass
        return False

def load_config_from_file(filename):
    """Load configuration from JSON file with validation."""
    try:
        if not os.path.exists(filename):
            raise FileNotFoundError(f"Configuration file not found: {filename}")
        
        with open(filename, 'r') as f:
            config = json.load(f)
        
        # Validate configuration
        if not is_valid_config(config):
            raise ConfigurationError("Invalid configuration data")
        
        # Apply defaults for missing values
        config = apply_config_defaults(config)
        
        print(f"Configuration loaded from {filename}")
        return config
        
    except json.JSONDecodeError as e:
        raise ConfigurationError(f"Invalid JSON format: {e}")
    except Exception as e:
        print(f"Error loading configuration: {e}")
        raise

def is_valid_config(config):
    """Validate configuration structure and values."""
    required_sections = ['brain', 'plasticity', 'neuromodulation', 'development', 'world']
    
    for section in required_sections:
        if section not in config:
            print(f"Missing required section: {section}")
            return False
    
    # Validate brain section
    brain = config['brain']
    if 'neuron_count' not in brain or brain['neuron_count'] <= 0:
        print("Invalid brain.neuron_count")
        return False
    
    # Validate ranges
    if brain.get('connection_probability', 0) < 0 or brain.get('connection_probability', 0) > 1:
        print("brain.connection_probability must be between 0 and 1")
        return False
    
    return True

def apply_config_defaults(config):
    """Apply default values for missing configuration parameters."""
    defaults = {
        'brain': {
            'synapse_density': 0.1,
            'v_thresh': -50.0,
            'v_rest': -70.0,
            'v_reset': -75.0,
            'tau_mem': 20.0,
            'tau_ref': 2.0
        },
        'plasticity': {
            'stdp': {
                'enable': True,
                'learning_rate': 0.001,
                'tau_plus': 20.0,
                'tau_minus': 20.0
            },
            'hebbian': {
                'enable': True,
                'learning_rate': 0.0005
            },
            'structural': {
                'enable': True,
                'synaptogenesis_rate': 0.0001,
                'pruning_rate': 0.00001
            }
        },
        'neuromodulation': {
            'dopamine': {'scale': 1.0},
            'curiosity': {'enable': True, 'decay_rate': 0.001},
            'novelty': {'enable': True, 'threshold': 0.5},
            'prediction_error': {'enable': True, 'sensitivity': 0.1}
        },
        'development': {
            'stages': {
                'initial_plasticity': 1.0,
                'critical_period': 0.8,
                'maturation': 0.5,
                'adult': 0.2,
                'aging': 0.1
            },
            'checkpoint_interval': 1000,
            'consolidation_interval': 5000
        },
        'memory': {
            'working_memory': {'capacity': 200, 'decay_rate': 0.01},
            'episodic': {'max_episodes': 5000, 'replay_interval': 100, 'consolidation_rate': 0.3}
        }
    }
    
    for section, section_defaults in defaults.items():
        if section not in config:
            config[section] = {}
        
        for key, default_value in section_defaults.items():
            if key not in config[section]:
                config[section][key] = default_value
            elif isinstance(default_value, dict) and isinstance(config[section][key], dict):
                # Recursively apply defaults for nested dictionaries
                for nested_key, nested_default in default_value.items():
                    if nested_key not in config[section][key]:
                        config[section][key][nested_key] = nested_default
    
    return config

# Example usage
if __name__ == "__main__":
    # Create and save configuration
    config = create_default_config()
    save_config_to_file(config, "nlm_config.json")
    
    # Load configuration
    loaded_config = load_config_from_file("nlm_config.json")
    
    # Create brain with loaded configuration
    # brain = pynlm.createBrain(loaded_config)
    # brain.initialize()
    
    print("Configuration example completed successfully")
```

### Example 2: Configuration Validation and Schema Examples

```python
import pynlm
from typing import Dict, Any, List
import re

class NLMConfigValidator:
    """Comprehensive configuration validator for NLM."""
    
    def __init__(self):
        self.validation_rules = self._load_validation_rules()
        self.error_messages = []
    
    def _load_validation_rules(self) -> Dict[str, Dict[str, Any]]:
        """Load validation rules for different configuration sections."""
        return {
            'brain': {
                'neuron_count': {'type': int, 'min': 1, 'max': 1000000},
                'synapse_density': {'type': float, 'min': 0.0, 'max': 1.0},
                'v_thresh': {'type': float, 'min': -100, 'max': -10},
                'v_rest': {'type': float, 'min': -100, 'max': -50},
                'v_reset': {'type': float, 'min': -100, 'max': -50},
                'tau_mem': {'type': float, 'min': 1, 'max': 100},
                'tau_ref': {'type': int, 'min': 0, 'max': 20}
            },
            'plasticity': {
                'stdp_learning_rate': {'type': float, 'min': 0.0, 'max': 1.0},
                'hebbian_learning_rate': {'type': float, 'min': 0.0, 'max': 1.0},
                'synaptogenesis_rate': {'type': float, 'min': 0.0, 'max': 0.01},
                'pruning_rate': {'type': float, 'min': 0.0, 'max': 0.001}
            },
            'development': {
                'checkpoint_interval': {'type': int, 'min': 100, 'max': 100000},
                'consolidation_interval': {'type': int, 'min': 100, 'max': 100000}
            },
            'memory': {
                'working_memory_capacity': {'type': int, 'min': 10, 'max': 10000},
                'episodic_max_episodes': {'type': int, 'min': 100, 'max': 100000},
                'replay_interval': {'type': int, 'min': 1, 'max': 10000},
                'consolidation_rate': {'type': float, 'min': 0.0, 'max': 1.0}
            }
        }
    
    def validate_config(self, config: Dict[str, Any]) -> List[str]:
        """Validate configuration and return list of errors."""
        self.error_messages = []
        
        # Check required sections
        required_sections = ['brain', 'plasticity', 'neuromodulation', 'development']
        for section in required_sections:
            if section not in config:
                self.error_messages.append(f"Missing required section: {section}")
        
        # Validate each section
        for section, section_config in config.items():
            if section in self.validation_rules:
                self._validate_section(section, section_config)
        
        return self.error_messages.copy()
    
    def _validate_section(self, section: str, section_config: Dict[str, Any]):
        """Validate a specific configuration section."""
        rules = self.validation_rules.get(section, {})
        
        for key, value in section_config.items():
            # Check type validation
            if key in rules:
                rule = rules[key]
                if 'type' in rule:
                    if not isinstance(value, rule['type']):
                        self.error_messages.append(
                            f"Section '{section}', key '{key}': "
                            f"expected type {rule['type'].__name__}, got {type(value).__name__}"
                        )
                        continue
                
                # Check range validation
                if 'min' in rule and value < rule['min']:
                    self.error_messages.append(
                        f"Section '{section}', key '{key}': "
                        f"value {value} is less than minimum {rule['min']}"
                    )
                
                if 'max' in rule and value > rule['max']:
                    self.error_messages.append(
                        f"Section '{section}', key '{key}': "
                        f"value {value} exceeds maximum {rule['max']}"
                    )
            
            # Special validation for nested structures
            elif isinstance(value, dict):
                self._validate_nested_config(section, key, value)
    
    def _validate_nested_config(self, section: str, key: str, nested_config: Dict[str, Any]):
        """Validate nested configuration structure."""
        nested_path = f"{section}.{key}"
        
        # Check for known nested structures
        if key == 'stdp':
            for param in ['learning_rate', 'tau_plus', 'tau_minus']:
                if param in nested_config and 'stdp' in self.validation_rules:
                    rule_key = f"stdp_{param}"
                    if rule_key in self.validation_rules['stdp']:
                        rule = self.validation_rules['stdp'][rule_key]
                        if 'min' in rule and nested_config[param] < rule['min']:
                            self.error_messages.append(
                                f"Section '{nested_path}', key '{param}': "
                                f"value {nested_config[param]} is less than minimum {rule['min']}"
                            )
        
        elif key == 'hebbian':
            if 'learning_rate' in nested_config:
                if 'hebbian_learning_rate' in self.validation_rules['plasticity']:
                    rule = self.validation_rules['plasticity']['hebbian_learning_rate']
                    if nested_config['learning_rate'] < rule['min']:
                        self.error_messages.append(
                            f"Section '{nested_path}', key 'learning_rate': "
                            f"value {nested_config['learning_rate']} is less than minimum {rule['min']}"
                        )
    
    def generate_config_schema(self) -> Dict[str, Any]:
        """Generate JSON schema for configuration validation."""
        schema = {
            "$schema": "http://json-schema.org/draft-07/schema#",
            "type": "object",
            "properties": {},
            "required": []
        }
        
        for section, rules in self.validation_rules.items():
            properties = {}
            for key, rule in rules.items():
                prop = {
                    "type": rule['type'].__name__ if 'type' in rule else "object",
                    "description": f"Configuration parameter {key} for section {section}"
                }
                
                if 'min' in rule:
                    prop["minimum"] = rule['min']
                if 'max' in rule:
                    prop["maximum"] = rule['max']
                
                properties[key] = prop
            
            schema["properties"][section] = {
                "type": "object",
                "properties": properties
            }
            
            schema["required"].append(section)
        
        return schema

# Usage example
if __name__ == "__main__":
    validator = NLMConfigValidator()
    
    # Example configuration
    config = {
        'brain': {
            'neuron_count': 2000,
            'synapse_density': 0.08,
            'v_thresh': -50.0,
            'v_rest': -70.0
        },
        'plasticity': {
            'stdp': {
                'learning_rate': 0.001,
                'tau_plus': 20.0,
                'tau_minus': 20.0
            },
            'hebbian': {
                'learning_rate': 0.0005
            }
        },
        'neuromodulation': {
            'dopamine': {'scale': 1.0},
            'curiosity': {'enable': True}
        },
        'development': {
            'checkpoint_interval': 1000,
            'consolidation_interval': 5000
        }
    }
    
    # Validate configuration
    errors = validator.validate_config(config)
    
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  - {error}")
    else:
        print("Configuration is valid!")
    
    # Generate schema
    schema = validator.generate_config_schema()
    
    print(f"\nGenerated schema with {len(schema['properties'])} required sections")
```

---

## Advanced Error Handling and Recovery

### Example 3: Comprehensive Error Recovery System

```python
import pynlm
import time
import traceback
import logging
from typing import Dict, Any, Optional, List
import numpy as np

class ErrorRecoveryManager:
    """Advanced error recovery and system management for NLM simulations."""
    
    def __init__(self, max_retries: int = 3, recovery_timeout: float = 10.0):
        self.max_retries = max_retries
        self.recovery_timeout = recovery_timeout
        self.error_history = []
        self.system_state = 'healthy'
        self.last_error = None
        self.recovery_attempts = {}
        
        # Setup logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        self.logger = logging.getLogger('error_recovery')
    
    def execute_with_recovery(self, operation: callable, operation_name: str, 
                            critical: bool = True, **kwargs) -> tuple:
        """Execute operation with comprehensive error handling and recovery."""
        attempt = 0
        last_error = None
        
        while attempt < self.max_retries:
            try:
                self.logger.info(f"Executing {operation_name} (attempt {attempt + 1}/{self.max_retries})")
                
                # Execute operation
                result = operation(**kwargs)
                
                # Log success
                self.logger.info(f"Successfully completed {operation_name}")
                self._log_success(operation_name)
                
                return result, None
                
            except Exception as e:
                last_error = self._categorize_error(e)
                attempt += 1
                
                self.logger.warning(f"Attempt {attempt} failed for {operation_name}: {last_error['message']}")
                
                # Log error details
                self._log_error(operation_name, e, last_error)
                
                # Check if error is recoverable
                if not self._is_recoverable_error(last_error):
                    self.logger.error(f"Unrecoverable error in {operation_name}: {last_error['message']}")
                    break
                
                # Attempt recovery
                if attempt < self.max_retries:
                    recovery_result = self._attempt_recovery(operation_name, last_error)
                    if recovery_result:
                        self.logger.info(f"Recovery successful for {operation_name}")
                        continue
                    else:
                        self.logger.warning(f"Recovery failed for {operation_name}")
                
                # Small delay before retry
                time.sleep(min(1.0, self.recovery_timeout / self.max_retries))
        
        # All attempts failed
        self._handle_operation_failure(operation_name, last_error)
        return None, last_error
    
    def _categorize_error(self, error: Exception) -> Dict[str, Any]:
        """Categorize error based on type and message."""
        error_info = {
            'type': type(error).__name__,
            'message': str(error),
            'timestamp': time.time(),
            'traceback': traceback.format_exc(),
            'recoverable': True,
            'category': self._determine_error_category(error)
        }
        
        # Add specific handling for known error types
        if isinstance(error, pynlm.NLMException):
            error_info['category'] = 'nlm_exception'
        elif isinstance(error, ValueError):
            error_info['category'] = 'value_error'
            error_info['recoverable'] = False
        elif isinstance(error, RuntimeError):
            error_info['category'] = 'runtime_error'
        elif isinstance(error, MemoryError):
            error_info['category'] = 'memory_error'
            error_info['recoverable'] = True
        
        return error_info
    
    def _determine_error_category(self, error: Exception) -> str:
        """Determine error category based on error content."""
        message = str(error).lower()
        
        # Memory-related errors
        if any(keyword in message for keyword in ['memory', 'out of memory', 'allocation failed']):
            return 'memory'
        
        # File system errors
        if any(keyword in message for keyword in ['file not found', 'permission denied', 'disk full']):
            return 'file_system'
        
        # Configuration errors
        if any(keyword in message for keyword in ['configuration', 'invalid config', 'missing']):
            return 'configuration'
        
        # Neural computation errors
        if any(keyword in message for keyword in ['neuron', 'synapse', 'neural', 'computation']):
            return 'neural_computation'
        
        # Network/connection errors
        if any(keyword in message for keyword in ['connection', 'network', 'timeout']):
            return 'network'
        
        # Default category
        return 'generic'
    
    def _is_recoverable_error(self, error_info: Dict[str, Any]) -> bool:
        """Determine if an error is recoverable."""
        if not error_info.get('recoverable', True):
            return False
        
        category = error_info['category']
        message = error_info['message'].lower()
        
        # Some categories are never recoverable
        non_recoverable_categories = ['configuration', 'value_error']
        
        if category in non_recoverable_categories:
            return False
        
        # Check for specific non-recoverable patterns
        non_recoverable_patterns = [
            'segmentation fault',
            'access violation',
            'stack overflow',
            'divide by zero'
        ]
        
        for pattern in non_recoverable_patterns:
            if pattern in message:
                return False
        
        return True
    
    def _attempt_recovery(self, operation_name: str, error_info: Dict[str, Any]) -> bool:
        """Attempt to recover from an error."""
        category = error_info['category']
        attempt_key = f"{operation_name}_{category}"
        
        # Check if we've already attempted recovery for this error
        if attempt_key in self.recovery_attempts:
            attempts = self.recovery_attempts[attempt_key]
            if attempts >= 3:  # Limit recovery attempts
                self.logger.warning(f"Too many recovery attempts for {attempt_key}")
                return False
        else:
            self.recovery_attempts[attempt_key] = 0
        
        # Perform recovery based on error category
        recovery_success = False
        
        if category == 'memory':
            recovery_success = self._recover_from_memory_error()
        elif category == 'file_system':
            recovery_success = self._recover_from_file_system_error()
        elif category == 'configuration':
            recovery_success = self._recover_from_configuration_error()
        elif category == 'neural_computation':
            recovery_success = self._recover_from_neural_error()
        elif category == 'network':
            recovery_success = self._recover_from_network_error()
        else:
            recovery_success = self._attempt_generic_recovery()
        
        if recovery_success:
            self.recovery_attempts[attempt_key] += 1
            self.logger.info(f"Recovery successful for {operation_name}")
            return True
        else:
            self.logger.warning(f"Recovery failed for {operation_name}")
            return False
    
    def _recover_from_memory_error(self) -> bool:
        """Attempt recovery from memory errors."""
        try:
            import gc
            import psutil
            
            # Force garbage collection
            gc.collect()
            
            # Check memory usage
            process = psutil.Process()
            memory_info = process.memory_info()
            memory_percent = psutil.virtual_memory().percent
            
            self.logger.info(f"Memory usage: {memory_info.rss / (1024**2):.2f} MB ({memory_percent}%)")
            
            # If memory usage is high, try to free up memory
            if memory_percent > 85:
                self.logger.warning("High memory usage, attempting cleanup...")
                # Clear some internal caches
                import numpy as np
                np.random.seed(42)  # Reset random state
                
                # Force another garbage collection
                gc.collect()
                
                # Check if memory improved
                new_memory = psutil.virtual_memory().percent
                if new_memory < 85:
                    return True
                else:
                    return False
            else:
                return True
                
        except Exception as e:
            self.logger.error(f"Memory recovery failed: {e}")
            return False
    
    def _recover_from_file_system_error(self) -> bool:
        """Attempt recovery from file system errors."""
        try:
            import os
            from datetime import datetime
            
            # Check if we can create a temporary file
            temp_path = f"/tmp/nlm_test_{int(datetime.now().timestamp())}.tmp"
            
            try:
                with open(temp_path, 'w') as f:
                    f.write("test")
                os.remove(temp_path)
                return True
            except Exception as e:
                self.logger.error(f"File system recovery failed: {e}")
                return False
                
        except Exception as e:
            self.logger.error(f"File system recovery exception: {e}")
            return False
    
    def _recover_from_configuration_error(self) -> bool:
        """Attempt recovery from configuration errors."""
        try:
            # Reload default configuration
            # This would depend on the specific implementation
            self.logger.info("Attempting to reload default configuration")
            # config = pynlm.createDefaultConfig()
            return True
        except Exception as e:
            self.logger.error(f"Configuration recovery failed: {e}")
            return False
    
    def _recover_from_neural_error(self) -> bool:
        """Attempt recovery from neural computation errors."""
        try:
            # Reset neural state
            # This would depend on the specific implementation
            self.logger.info("Attempting to reset neural state")
            return True
        except Exception as e:
            self.logger.error(f"Neural recovery failed: {e}")
            return False
    
    def _recover_from_network_error(self) -> bool:
        """Attempt recovery from network errors."""
        try:
            # Retry network operations
            self.logger.info("Attempting network recovery")
            return True
        except Exception as e:
            self.logger.error(f"Network recovery failed: {e}")
            return False
    
    def _attempt_generic_recovery(self) -> bool:
        """Attempt generic recovery."""
        try:
            # Basic recovery: reset system state
            self.logger.info("Attempting generic recovery")
            # Reset logging
            import logging
            for handler in logging.getLogger().handlers[:]:
                logging.getLogger().removeHandler(handler)
            
            # Re-initialize logging
            logging.basicConfig(level=logging.INFO)
            
            return True
        except Exception as e:
            self.logger.error(f"Generic recovery failed: {e}")
            return False
    
    def _log_success(self, operation_name: str):
        """Log successful operation."""
        self.error_history.append({
            'timestamp': time.time(),
            'operation': operation_name,
            'status': 'success',
            'error': None
        })
        
        # Keep only recent history
        if len(self.error_history) > 100:
            self.error_history = self.error_history[-50:]
    
    def _log_error(self, operation_name: str, error: Exception, error_info: Dict[str, Any]):
        """Log error details."""
        self.error_history.append({
            'timestamp': time.time(),
            'operation': operation_name,
            'status': 'error',
            'error': error_info,
            'traceback': error_info.get('traceback')
        })
        
        # Update last error
        self.last_error = error_info
        
        # Keep only recent history
        if len(self.error_history) > 100:
            self.error_history = self.error_history[-50:]
    
    def _handle_operation_failure(self, operation_name: str, error_info: Dict[str, Any]):
        """Handle operation failure."""
        self.logger.error(f"Operation {operation_name} failed after {self.max_retries} attempts")
        
        # Check if this is a critical failure
        if error_info.get('recoverable', False):
            self.system_state = 'degraded'
            self.logger.warning("System state: DEGRADED")
        else:
            self.system_state = 'failed'
            self.logger.error("System state: FAILED")
        
        # Log failure summary
        self.logger.error(f"Failure summary: {error_info['category']} - {error_info['message']}")
    
    def get_system_health(self) -> Dict[str, Any]:
        """Get current system health status."""
        health_info = {
            'state': self.system_state,
            'last_error': self.last_error,
            'error_count': len([e for e in self.error_history if e['status'] == 'error']),
            'success_count': len([e for e in self.error_history if e['status'] == 'success']),
            'recovery_attempts': self.recovery_attempts.copy(),
            'recent_errors': self.error_history[-10:] if len(self.error_history) > 10 else self.error_history
        }
        
        return health_info
    
    def reset_system(self):
        """Reset the entire system."""
        self.logger.info("Resetting system...")
        
        # Reset error history
        self.error_history = []
        
        # Reset recovery attempts
        self.recovery_attempts = {}
        
        # Reset system state
        self.system_state = 'healthy'
        self.last_error = None
        
        self.logger.info("System reset completed")

# Usage example
if __name__ == "__main__":
    # Initialize error recovery manager
    recovery_manager = ErrorRecoveryManager(max_retries=3, recovery_timeout=15.0)
    
    # Example operations with error recovery
    def risky_operation(should_fail=False):
        """Example operation that might fail."""
        if should_fail:
            raise ValueError("Simulated operation failure")
        
        # Simulate successful operation
        time.sleep(0.1)
        return "Operation completed successfully"
    
    def risky_file_operation(should_fail=False):
        """Example file operation that might fail."""
        if should_fail:
            raise FileNotFoundError("Simulated file not found")
        
        # Simulate successful file operation
        time.sleep(0.1)
        return {"file_size": 1024, "content": "success"}
    
    def risky_neural_operation(should_fail=False):
        """Example neural operation that might fail."""
        if should_fail:
            raise RuntimeError("Simulated neural computation failure")
        
        # Simulate successful neural operation
        time.sleep(0.1)
        return {
            'neurons_firing': 150,
            'spikes_generated': 1000,
            'developmental_stage': 'maturation'
        }
    
    print("=== Error Recovery Demonstration ===\n")
    
    # Test successful operations
    print("1. Testing successful operations:")
    result, error = recovery_manager.execute_with_recovery(
        risky_operation, "Test Successful Operation", should_fail=False
    )
    
    if result:
        print(f"   Success: {result}")
    else:
        print(f"   Error: {error}")
    
    print()
    
    # Test recoverable operations
    print("2. Testing recoverable operations (with failures):")
    for i, should_fail in enumerate([True, False, True]):
        result, error = recovery_manager.execute_with_recovery(
            risky_operation, f"Recoverable Operation {i+1}", should_fail=should_fail
        )
        
        if result:
            print(f"   Operation {i+1}: Success - {result}")
        else:
            print(f"   Operation {i+1}: Failed - {error['message']}")
    
    print()
    
    # Test system health
    print("3. System health status:")
    health = recovery_manager.get_system_health()
    print(f"   System state: {health['state']}")
    print(f"   Total operations: {health['success_count'] + health['error_count']}")
    print(f"   Success rate: {health['success_count'] / (health['success_count'] + health['error_count']) * 100:.1f}%")
    
    if health['last_error']:
        print(f"   Last error: {health['last_error']['category']} - {health['last_error']['message']}")
    
    print()
    
    # Reset system
    print("4. System reset:")
    recovery_manager.reset_system()
    print("   System reset completed")
```

---

## Batch Processing with Configuration Management

### Example 4: Intelligent Batch Processing Framework

```python
import pynlm
import numpy as np
import time
import json
import hashlib
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor, as_completed
from typing import Dict, Any, List, Optional, Callable
import psutil
import os

class IntelligentBatchProcessor:
    """Advanced batch processing framework with intelligent configuration management."""
    
    def __init__(self, max_memory_gb: float = 16.0, max_cpu_percent: float = 85.0,
                 max_workers: Optional[int] = None):
        self.max_memory_gb = max_memory_gb
        self.max_cpu_percent = max_cpu_percent
        self.max_workers = max_workers or mp.cpu_count()
        
        # System monitoring
        self.monitoring_active = True
        self.current_memory_usage = 0.0
        self.current_cpu_usage = 0.0
        self.system_load = 0.0
        
        # Batch configuration
        self.batch_results = {}
        self.config_history = {}
        self.performance_metrics = {}
        
        # Initialize performance tracking
        self.start_time = time.time()
        self.total_operations = 0
        
        # Setup logging
        self.setup_logging()
    
    def setup_logging(self):
        """Setup comprehensive logging for batch processing."""
        import logging
        
        # Create formatters
        detailed_formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - BatchID:%(batch_id)s - Task:%(task_id)s - %(message)s'
        )
        
        simple_formatter = logging.Formatter(
            '%(asctime)s - %(levelname)s - %(message)s'
        )
        
        # Setup batch logger
        self.logger = logging.getLogger('intelligent_batch_processor')
        self.logger.setLevel(logging.INFO)
        
        # Remove existing handlers
        for handler in self.logger.handlers[:]:
            self.logger.removeHandler(handler)
        
        # Setup file handler
        file_handler = logging.FileHandler('batch_processing.log')
        file_handler.setFormatter(detailed_formatter)
        self.logger.addHandler(file_handler)
        
        # Setup console handler
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(simple_formatter)
        self.logger.addHandler(console_handler)
    
    def log_with_context(self, level: str, message: str, batch_id: str = "default", 
                        task_id: str = "default", **kwargs):
        """Log message with contextual information."""
        # Add context to log record
        extra = {
            'batch_id': batch_id,
            'task_id': task_id,
            'message': message
        }
        extra.update(kwargs)
        
        if level == 'INFO':
            self.logger.info(message, extra=extra)
        elif level == 'WARNING':
            self.logger.warning(message, extra=extra)
        elif level == 'ERROR':
            self.logger.error(message, extra=extra)
        elif level == 'DEBUG':
            self.logger.debug(message, extra=extra)
    
    def monitor_system_resources(self) -> Dict[str, Any]:
        """Monitor system resources and return status."""
        try:
            # Get memory usage
            memory = psutil.virtual_memory()
            self.current_memory_usage = memory.percent
            
            # Get CPU usage
            cpu_percent = psutil.cpu_percent(interval=0.1)
            self.current_cpu_usage = cpu_percent
            
            # Get system load
            self.system_load = psutil.getloadavg()[0] / psutil.cpu_count() if hasattr(psutil, 'getloadavg') else 0.0
            
            # Check if system is healthy
            healthy = (
                self.current_memory_usage < self.max_memory_gb * 10 and  # Convert GB to percent
                self.current_cpu_usage < self.max_cpu_percent and
                self.system_load < 0.9
            )
            
            return {
                'healthy': healthy,
                'memory_usage_gb': memory.used / (1024**3),
                'memory_available_gb': memory.available / (1024**3),
                'memory_percent': self.current_memory_usage,
                'cpu_percent': self.current_cpu_usage,
                'system_load': self.system_load,
                'timestamp': time.time()
            }
            
        except Exception as e:
            self.log_with_context('ERROR', f"System monitoring failed: {e}")
            return {
                'healthy': False,
                'error': str(e),
                'timestamp': time.time()
            }
    
    def should_continue_batch(self) -> bool:
        """Check if batch processing should continue based on system resources."""
        resources = self.monitor_system_resources()
        
        if not resources['healthy']:
            self.log_with_context('WARNING', f"System resources unhealthy: {resources}")
            return False
        
        # Check specific thresholds
        if resources['memory_percent'] > self.max_memory_gb * 10 - 10:  # 10% buffer
            self.log_with_context('WARNING', f"High memory usage: {resources['memory_percent']:.1f}%")
            return False
        
        if resources['cpu_percent'] > self.max_cpu_percent - 10:  # 10% buffer
            self.log_with_context('WARNING', f"High CPU usage: {resources['cpu_percent']:.1f}%")
            return False
        
        if resources['system_load'] > 0.8:  # 80% threshold
            self.log_with_context('WARNING', f"High system load: {resources['system_load']:.2f}")
            return False
        
        return True
    
    def generate_config_variations(self, base_config: Dict[str, Any], 
                                 variations: Dict[str, Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Generate configuration variations with intelligent parameter ranges."""
        configs = []
        
        for var_name, var_params in variations.items():
            config = base_config.copy()
            
            # Apply variations with parameter validation
            for key, value in var_params.items():
                if '.' in key:  # Nested configuration
                    self._set_nested_config(config, key, value)
                else:
                    config[key] = self._validate_config_value(key, value)
            
            # Generate checksum for config
            config_checksum = self._generate_config_checksum(config)
            
            # Store config history
            self.config_history[config_checksum] = {
                'config': config.copy(),
                'variations': var_params,
                'timestamp': time.time()
            }
            
            configs.append(config)
            
            self.log_with_context('INFO', f"Generated config variation: {var_name} (checksum: {config_checksum})")
        
        return configs
    
    def _set_nested_config(self, config: Dict[str, Any], key: str, value: Any):
        """Set nested configuration value."""
        keys = key.split('.')
        current = config
        
        for i, k in enumerate(keys[:-1]):
            if k not in current:
                current[k] = {}
            current = current[k]
        
        current[keys[-1]] = self._validate_config_value(key, value)
    
    def _validate_config_value(self, key: str, value: Any) -> Any:
        """Validate configuration value based on key type."""
        # Validate based on key pattern
        if 'neuron_count' in key:
            if isinstance(value, (int, float)):
                value = int(max(100, min(value, 1000000)))  # Reasonable bounds
        
        elif 'synapse_density' in key:
            if isinstance(value, (int, float)):
                value = max(0.0, min(value, 1.0))  # Must be between 0 and 1
        
        elif 'learning_rate' in key:
            if isinstance(value, (int, float)):
                value = max(1e-6, min(value, 0.1))  # Reasonable learning rates
        
        elif 'v_thresh' in key or 'v_rest' in key or 'v_reset' in key:
            if isinstance(value, (int, float)):
                value = max(-100.0, min(value, -10.0))  # Reasonable membrane potentials
        
        elif 'tau_' in key:
            if isinstance(value, (int, float)):
                value = max(1.0, min(value, 100.0))  # Time constants
        
        return value
    
    def _generate_config_checksum(self, config: Dict[str, Any]) -> str:
        """Generate checksum for configuration."""
        config_str = json.dumps(config, sort_keys=True)
        return hashlib.md5(config_str.encode()).hexdigest()
    
    def run_batch_synchronous(self, configs: List[Dict[str, Any]], 
                            batch_id: str = "default") -> Dict[str, Any]:
        """Run batch processing synchronously."""
        self.log_with_context('INFO', f"Starting synchronous batch processing with {len(configs)} configurations", 
                            batch_id=batch_id)
        
        results = []
        start_time = time.time()
        
        for i, config in enumerate(configs):
            self.log_with_context('INFO', f"Processing configuration {i+1}/{len(configs)}", 
                                batch_id=batch_id, task_id=f"sync_{i}")
            
            # Check system resources
            if not self.should_continue_batch():
                self.log_with_context('WARNING', f"System resources insufficient, stopping batch", 
                                    batch_id=batch_id)
                break
            
            # Run simulation
            result = self._run_single_simulation(config, batch_id, f"sync_{i}")
            results.append(result)
            
            # Small delay to prevent overwhelming system
            time.sleep(0.01)
        
        end_time = time.time()
        
        # Calculate performance metrics
        metrics = self._calculate_batch_metrics(results, start_time, end_time)
        
        # Store results
        self.batch_results[batch_id] = {
            'type': 'synchronous',
            'results': results,
            'metrics': metrics,
            'timestamp': start_time,
            'end_time': end_time
        }
        
        self.log_with_context('INFO', f"Synchronous batch completed: {len(results)} results, "
                            f"{metrics['success_rate']*100:.1f}% success rate", 
                            batch_id=batch_id)
        
        return metrics
    
    def run_batch_parallel(self, configs: List[Dict[str, Any]], 
                         use_processes: bool = False,
                         batch_id: str = "default") -> Dict[str, Any]:
        """Run batch processing in parallel."""
        self.log_with_context('INFO', f"Starting parallel batch processing with {len(configs)} configurations "
                            f"({'processes' if use_processes else 'threads'})", 
                            batch_id=batch_id)
        
        results = []
        start_time = time.time()
        
        # Prepare tasks
        tasks = [(config, i, batch_id, f"parallel_{i}") for i, config in enumerate(configs)]
        
        # Create executor
        if use_processes:
            executor = ProcessPoolExecutor(max_workers=self.max_workers)
        else:
            executor = ThreadPoolExecutor(max_workers=self.max_workers)
        
        # Submit all tasks
        future_to_task = {
            executor.submit(self._run_single_simulation_static, task): task
            for task in tasks
        }
        
        # Collect results as they complete
        completed = 0
        for future in as_completed(future_to_task):
            task = future_to_task[future]
            
            try:
                result = future.result(timeout=300)  # 5 minute timeout per task
                results.append(result)
                completed += 1
                
                self.log_with_context('INFO', f"Completed {completed}/{len(configs)} tasks", 
                                    batch_id=batch_id, task_id=task[3])
                
            except Exception as e:
                self.log_with_context('ERROR', f"Task failed: {e}", 
                                    batch_id=batch_id, task_id=task[3])
                
                # Create error result
                error_result = {
                    'batch_id': batch_id,
                    'task_id': task[3],
                    'config': task[0],
                    'success': False,
                    'error': str(e),
                    'timestamp': time.time(),
                    'end_time': time.time()
                }
                results.append(error_result)
        
        end_time = time.time()
        
        # Calculate performance metrics
        metrics = self._calculate_batch_metrics(results, start_time, end_time)
        
        # Store results
        self.batch_results[batch_id] = {
            'type': 'parallel',
            'results': results,
            'metrics': metrics,
            'timestamp': start_time,
            'end_time': end_time
        }
        
        self.log_with_context('INFO', f"Parallel batch completed: {len(results)} results, "
                            f"{metrics['success_rate']*100:.1f}% success rate, "
                            f"{metrics['total_time']:.2f}s total time", 
                            batch_id=batch_id)
        
        return metrics
    
    def _run_single_simulation(self, config: Dict[str, Any], 
                             batch_id: str, task_id: str) -> Dict[str, Any]:
        """Run a single simulation with error handling."""
        start_time = time.time()
        
        try:
            # Create brain with configuration
            brain_config = pynlm.createDefaultConfig()
            
            # Apply configuration
            for key, value in config.items():
                if '.' in key:
                    brain_config.set(key, value)
                else:
                    # Handle top-level configuration
                    if key == 'brain.neuron_count':
                        brain_config.set('brain.neuron_count', value)
                    elif key == 'brain.synapse_density':
                        brain_config.set('brain.synapse_density', value)
                    elif key == 'brain.v_thresh':
                        brain_config.set('brain.v_thresh', value)
                    elif key == 'brain.v_rest':
                        brain_config.set('brain.v_rest', value)
            
            # Create and initialize brain
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            # Run simulation
            steps = config.get('steps', 100)
            spike_counts = []
            firing_rates = []
            
            for step in range(steps):
                brain.step(step)
                
                # Collect metrics
                spike_counts.append(brain.getTotalSpikeCount())
                firing_rates.append(brain.getAverageFiringRate())
                
                # Check system resources periodically
                if step % 50 == 0 and not self.should_continue_batch():
                    self.log_with_context('WARNING', f"System resources insufficient during simulation", 
                                        batch_id=batch_id, task_id=task_id)
                    break
            
            end_time = time.time()
            
            # Calculate performance metrics
            total_time = end_time - start_time
            total_spikes = sum(spike_counts)
            avg_firing_rate = np.mean(firing_rates) if firing_rates else 0.0
            
            result = {
                'batch_id': batch_id,
                'task_id': task_id,
                'config': config,
                'success': True,
                'total_spikes': total_spikes,
                'avg_firing_rate': avg_firing_rate,
                'total_time': total_time,
                'steps_completed': len(spike_counts),
                'final_developmental_stage': brain.getDevelopmentalStage(),
                'memory_usage': self._get_memory_usage(),
                'timestamp': start_time,
                'end_time': end_time
            }
            
            self.log_with_context('INFO', f"Simulation completed: {total_spikes} spikes, "
                                f"{avg_firing_rate:.2f} Hz avg firing rate", 
                                batch_id=batch_id, task_id=task_id)
            
            return result
            
        except Exception as e:
            end_time = time.time()
            
            error_result = {
                'batch_id': batch_id,
                'task_id': task_id,
                'config': config,
                'success': False,
                'error': str(e),
                'error_type': type(e).__name__,
                'total_time': end_time - start_time,
                'steps_completed': 0,
                'timestamp': start_time,
                'end_time': end_time
            }
            
            self.log_with_context('ERROR', f"Simulation failed: {e}", 
                                batch_id=batch_id, task_id=task_id)
            
            return error_result
    
    def _run_single_simulation_static(self, task):
        """Static method for parallel execution."""
        config, config_id, batch_id, task_id = task
        processor = IntelligentBatchProcessor()
        return processor._run_single_simulation(config, batch_id, task_id)
    
    def _calculate_batch_metrics(self, results: List[Dict[str, Any]], 
                               start_time: float, end_time: float) -> Dict[str, Any]:
        """Calculate comprehensive batch processing metrics."""
        total_time = end_time - start_time
        
        successful_results = [r for r in results if r.get('success', False)]
        failed_results = [r for r in results if not r.get('success', False)]
        
        metrics = {
            'total_time': total_time,
            'total_configurations': len(results),
            'successful_configurations': len(successful_results),
            'failed_configurations': len(failed_results),
            'success_rate': len(successful_results) / len(results) if results else 0.0,
            'average_time_per_config': total_time / len(results) if results else 0.0,
            'total_spikes': sum(r.get('total_spikes', 0) for r in successful_results),
            'average_firing_rate': np.mean([r.get('avg_firing_rate', 0) for r in successful_results]) if successful_results else 0.0,
            'min_time': min([r['total_time'] for r in successful_results]) if successful_results else 0.0,
            'max_time': max([r['total_time'] for r in successful_results]) if successful_results else 0.0,
            'std_time': np.std([r['total_time'] for r in successful_results]) if successful_results and len(successful_results) > 1 else 0.0,
            'memory_efficiency': self._calculate_memory_efficiency(results),
            'throughput': len(results) / total_time if total_time > 0 else 0.0,
            'timestamp': start_time,
            'end_time': end_time
        }
        
        return metrics
    
    def _calculate_memory_efficiency(self, results: List[Dict[str, Any]]) -> float:
        """Calculate memory efficiency metrics."""
        if not results:
            return 0.0
        
        # Calculate average memory usage per spike
        memory_per_spike = []
        for result in results:
            if result.get('success', False):
                memory = result.get('memory_usage', 0)
                spikes = result.get('total_spikes', 0)
                if spikes > 0:
                    memory_per_spike.append(memory / spikes)
        
        if not memory_per_spike:
            return 0.0
        
        # Lower memory per spike is better
        avg_memory_per_spike = np.mean(memory_per_spike)
        return 1.0 / (1.0 + avg_memory_per_spike)
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        try:
            process = psutil.Process()
            memory_info = process.memory_info()
            return memory_info.rss / (1024 * 1024)  # Convert to MB
        except:
            return 0.0
    
    def save_batch_results(self, batch_id: str, filename: str):
        """Save batch results to file."""
        if batch_id not in self.batch_results:
            self.log_with_context('ERROR', f"Batch ID {batch_id} not found", 
                                batch_id=batch_id)
            return False
        
        try:
            results = self.batch_results[batch_id]
            
            # Convert numpy arrays to lists for JSON serialization
            serializable_results = []
            for result in results['results']:
                serializable_result = result.copy()
                # Convert any numpy types
                if 'config' in serializable_result:
                    serializable_result['config'] = json.loads(json.dumps(serializable_result['config']))
                serializable_results.append(serializable_result)
            
            # Save to file
            with open(filename, 'w') as f:
                json.dump({
                    'batch_id': batch_id,
                    'type': results['type'],
                    'timestamp': results['timestamp'],
                    'end_time': results['end_time'],
                    'results': serializable_results,
                    'metrics': results['metrics']
                }, f, indent=2)
            
            self.log_with_context('INFO', f"Batch results saved to {filename}", 
                                batch_id=batch_id)
            return True
            
        except Exception as e:
            self.log_with_context('ERROR', f"Failed to save batch results: {e}", 
                                batch_id=batch_id)
            return False
    
    def generate_performance_report(self, batch_id: str) -> str:
        """Generate comprehensive performance report."""
        if batch_id not in self.batch_results:
            return f"Batch ID {batch_id} not found"
        
        results = self.batch_results[batch_id]
        metrics = results['metrics']
        
        report = f"""# NLM Batch Processing Performance Report

## Batch Information
- Batch ID: {batch_id}
- Type: {results['type']}
- Start Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(results['timestamp']))}
- End Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(results['end_time']))}
- Total Duration: {metrics['total_time']:.2f} seconds

## Performance Summary
- Total Configurations: {metrics['total_configurations']}
- Successful Configurations: {metrics['successful_configurations']}
- Failed Configurations: {metrics['failed_configurations']}
- Success Rate: {metrics['success_rate']*100:.1f}%

## Processing Performance
- Average Processing Time per Configuration: {metrics['average_time_per_config']:.4f} seconds
- Throughput: {metrics['throughput']:.2f} configurations/second
- Min Processing Time: {metrics['min_time']:.4f} seconds
- Max Processing Time: {metrics['max_time']:.4f} seconds
- Processing Time Standard Deviation: {metrics['std_time']:.4f} seconds

## Neural Performance
- Total Spikes Generated: {metrics['total_spikes']:,}
- Average Firing Rate: {metrics['average_firing_rate']:.2f} Hz

## Efficiency Metrics
- Memory Efficiency: {metrics['memory_efficiency']:.4f}

## Configuration Analysis
- Unique Configurations: {len(set(r['config'].get('brain.neuron_count', 0) for r in results['results'] if r.get('success', False)))}
- Neuron Count Range: {min(r.get('config', {}).get('brain', {}).get('neuron_count', 0) for r in results['results'] if r.get('success', False))} - {max(r.get('config', {}).get('brain', {}).get('neuron_count', 0) for r in results['results'] if r.get('success', False)}}

## Recommendations
{self._generate_recommendations(metrics)}

---
Report generated by NLM Intelligent Batch Processor
"""
        
        return report
    
    def _generate_recommendations(self, metrics: Dict[str, Any]) -> str:
        """Generate recommendations based on performance metrics."""
        recommendations = []
        
        if metrics['success_rate'] < 0.8:
            recommendations.append("❌ High failure rate. Consider simplifying configurations or checking for common errors.")
        
        if metrics['average_time_per_config'] > 1.0:
            recommendations.append("⏱️  Slow processing time. Consider parallel processing or optimizing configurations.")
        
        if metrics['average_firing_rate'] < 1.0:
            recommendations.append("🧠 Low neural activity. Consider adjusting brain parameters (thresholds, connectivity).")
        
        if metrics['memory_efficiency'] < 0.5:
            recommendations.append("💾 Low memory efficiency. Consider reducing simulation complexity or increasing neuron count.")
        
        if len(recommendations) == 0:
            recommendations.append("✅ Excellent performance! Consider scaling up to larger configurations.")
        
        return '\n'.join(f"  {rec}" for rec in recommendations)
    
    def run_comprehensive_batch(self, base_config: Dict[str, Any], 
                              variations: Dict[str, Dict[str, Any]],
                              batch_id: str = "comprehensive_batch"):
        """Run comprehensive batch processing with multiple strategies."""
        self.log_with_context('INFO', f"Starting comprehensive batch processing: {len(variations)} variation groups", 
                            batch_id=batch_id)
        
        # Generate all configuration variations
        all_configs = self.generate_config_variations(base_config, variations)
        
        # Split into chunks for memory management
        chunk_size = min(10, len(all_configs))  # Process 10 configs at a time
        chunks = [all_configs[i:i + chunk_size] for i in range(0, len(all_configs), chunk_size)]
        
        print(f"Generated {len(all_configs)} configurations in {len(chunks)} chunks")
        
        # Process each chunk
        all_results = []
        for chunk_index, chunk in enumerate(chunks):
            print(f"\nProcessing chunk {chunk_index + 1}/{len(chunks)}")
            
            if not self.should_continue_batch():
                print("System resources insufficient, stopping batch processing")
                break
            
            # Try parallel processing first
            try:
                chunk_results = self.run_batch_parallel(chunk, use_processes=True, batch_id=f"{batch_id}_chunk_{chunk_index}")
                all_results.extend(chunk_results['results'])
                print(f"Chunk {chunk_index + 1} completed successfully")
            except Exception as e:
                print(f"Parallel processing failed: {e}, falling back to synchronous")
                
                # Fallback to synchronous processing
                sync_results = self.run_batch_synchronous(chunk, batch_id=f"{batch_id}_chunk_{chunk_index}_sync")
                all_results.extend(sync_results['results'])
                print(f"Chunk {chunk_index + 1} completed with fallback")
        
        # Generate comprehensive report
        report_filename = f"batch_report_{int(time.time())}.txt"
        report = self.generate_performance_report(batch_id)
        
        # Save detailed results
        self.save_batch_results(batch_id, f"detailed_results_{int(time.time())}.json")
        
        # Save report
        with open(report_filename, 'w') as f:
            f.write(report)
        
        print(f"\n=== Comprehensive Batch Processing Complete ===")
        print(f"Total configurations processed: {len(all_results)}")
        print(f"Success rate: {len([r for r in all_results if r.get('success', False)]) / len(all_results) * 100:.1f}%")
        print(f"Performance report saved to: {report_filename}")
        
        return all_results

# Example usage
if __name__ == "__main__":
    print("=== Intelligent Batch Processing Example ===\n")
    
    # Create base configuration
    base_config = {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "brain.v_thresh": -50.0,
        "brain.v_rest": -70.0,
        "steps": 100
    }
    
    # Define variations
    variations = {
        "small_network": {
            "brain.neuron_count": 500,
            "steps": 50
        },
        "medium_network": {
            "brain.neuron_count": 2000,
            "steps": 150,
            "brain.synapse_density": 0.08
        },
        "large_network": {
            "brain.neuron_count": 5000,
            "steps": 300,
            "brain.synapse_density": 0.05
        },
        "high_connectivity": {
            "brain.neuron_count": 1000,
            "steps": 100,
            "brain.synapse_density": 0.2
        },
        "low_connectivity": {
            "brain.neuron_count": 1000,
            "steps": 100,
            "brain.synapse_density": 0.02
        }
    }
    
    # Create batch processor
    processor = IntelligentBatchProcessor(
        max_memory_gb=8.0,
        max_cpu_percent=80.0,
        max_workers=4
    )
    
    # Run comprehensive batch processing
    results = processor.run_comprehensive_batch(base_config, variations)
    
    print(f"\nBatch processing completed successfully!")
```

---

## Performance Monitoring and Optimization

### Example 5: Advanced Performance Monitoring

```python
import pynlm
import numpy as np
import time
import psutil
import matplotlib.pyplot as plt
from typing import Dict, Any, List
import json

class PerformanceMonitor:
    """Advanced performance monitoring and optimization for NLM simulations."""
    
    def __init__(self, brain, config_file: str = "performance_metrics.json"):
        self.brain = brain
        self.config_file = config_file
        self.metrics_history = []
        self.performance_targets = self._load_performance_targets()
        self.optimization_history = []
        
        # Initialize monitoring
        self.monitoring_active = True
        self.start_time = time.time()
        
        # Setup plotting
        self.setup_plots()
    
    def _load_performance_targets(self) -> Dict[str, Any]:
        """Load performance targets for different metrics."""
        return {
            'firing_rate': {'min': 1.0, 'max': 100.0, 'target': 20.0},
            'memory_usage': {'min': 0, 'max': 1024, 'target': 256},  # MB
            'processing_time': {'min': 0.1, 'max': 60.0, 'target': 5.0},  # seconds
            'spike_density': {'min': 0.0, 'max': 1.0, 'target': 0.5},
            'stability': {'min': 0.0, 'max': 1.0, 'target': 0.8}
        }
    
    def setup_plots(self):
        """Setup real-time plotting for performance monitoring."""
        try:
            self.fig, self.axes = plt.subplots(2, 3, figsize=(15, 10))
            self.fig.suptitle('NLM Performance Monitoring', fontsize=16)
            
            # Define subplot positions
            self.plot_positions = {
                'firing_rate': (0, 0),
                'spike_count': (0, 1),
                'memory_usage': (0, 2),
                'processing_time': (1, 0),
                'stability': (1, 1),
                'correlation': (1, 2)
            }
            
            # Initialize plots
            for name, (row, col) in self.plot_positions.items():
                ax = self.axes[row, col]
                ax.set_title(name.replace('_', ' ').title())
                ax.grid(True, alpha=0.3)
            
            plt.tight_layout()
            
        except Exception as e:
            print(f"Warning: Could not setup real-time plotting: {e}")
    
    def collect_metrics(self, step: int, brain, force_collection: bool = False) -> Dict[str, Any]:
        """Collect comprehensive performance metrics."""
        try:
            current_time = time.time()
            elapsed_time = current_time - self.start_time
            
            # Basic neural metrics
            total_spikes = brain.getTotalSpikeCount()
            firing_neuron_count = brain.getFiringNeuronCount()
            avg_firing_rate = brain.getAverageFiringRate()
            excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
            developmental_stage = brain.getDevelopmentalStage()
            
            # Memory and system metrics
            memory_info = psutil.virtual_memory()
            cpu_percent = psutil.cpu_percent(interval=0.1)
            
            # Processing metrics
            processing_time = elapsed_time / (step + 1) if step > 0 else 0
            steps_per_second = step / elapsed_time if elapsed_time > 0 else 0
            
            # Stability metrics
            stability = self._calculate_stability()
            
            # Neuron statistics
            total_neurons = brain.getTotalNeuronCount()
            active_neurons = firing_neuron_count
            neuron_activity_ratio = active_neurons / total_neurons if total_neurons > 0 else 0
            
            # Spike statistics
            spike_rate = total_spikes / elapsed_time if elapsed_time > 0 else 0
            spikes_per_neuron = total_spikes / total_neurons if total_neurons > 0 else 0
            
            # Collect all metrics
            metrics = {
                'timestamp': current_time,
                'step': step,
                'elapsed_time': elapsed_time,
                
                # Neural metrics
                'total_spikes': total_spikes,
                'firing_neuron_count': firing_neuron_count,
                'avg_firing_rate': avg_firing_rate,
                'excitation_inhibition_ratio': excitation_inhibition_ratio,
                'developmental_stage': developmental_stage,
                
                # System metrics
                'memory_usage_mb': memory_info.used / (1024**2),
                'memory_available_mb': memory_info.available / (1024**2),
                'memory_percent': memory_info.percent,
                'cpu_percent': cpu_percent,
                
                # Processing metrics
                'processing_time': processing_time,
                'steps_per_second': steps_per_second,
                'spike_rate': spike_rate,
                
                # Stability and quality metrics
                'stability': stability,
                'neuron_activity_ratio': neuron_activity_ratio,
                'spikes_per_neuron': spikes_per_neuron,
                
                # Derived metrics
                'firing_efficiency': self._calculate_firing_efficiency(avg_firing_rate),
                'network_activity': self._calculate_network_activity(avg_firing_rate, neuron_activity_ratio),
                'learning_progress': self._calculate_learning_progress(developmental_stage),
                
                # Performance targets comparison
                'performance_violations': self._check_performance_targets(metrics),
                
                # System health
                'system_healthy': self._check_system_health(metrics)
            }
            
            # Store metrics
            self.metrics_history.append(metrics)
            
            # Update real-time plots
            if hasattr(self, 'fig'):
                self._update_plots(step)
            
            return metrics
            
        except Exception as e:
            print(f"Error collecting metrics: {e}")
            return self._get_default_metrics(step)
    
    def _calculate_stability(self) -> float:
        """Calculate stability metric based on recent firing rate variations."""
        if len(self.metrics_history) < 5:
            return 1.0
        
        # Get recent firing rates
        recent_rates = [m['avg_firing_rate'] for m in self.metrics_history[-10:]]
        
        # Calculate coefficient of variation (std/mean)
        if len(recent_rates) < 2 or np.mean(recent_rates) == 0:
            return 1.0
        
        cv = np.std(recent_rates) / np.mean(recent_rates)
        
        # Convert to stability score (lower CV = higher stability)
        stability = max(0.0, min(1.0, 1.0 - cv))
        
        return stability
    
    def _calculate_firing_efficiency(self, avg_firing_rate: float) -> float:
        """Calculate firing efficiency based on firing rate."""
        # Ideal firing rate is around 10-20 Hz
        target_range = (5.0, 50.0)
        
        if avg_firing_rate < target_range[0]:
            return avg_firing_rate / target_range[0]  # Low rate = inefficient
        elif avg_firing_rate > target_range[1]:
            return target_range[1] / avg_firing_rate  # High rate = inefficient
        else:
            return 1.0  # Within target range = efficient
    
    def _calculate_network_activity(self, avg_firing_rate: float, neuron_activity_ratio: float) -> float:
        """Calculate overall network activity level."""
        # Combine firing rate and neuron activity
        activity_score = (avg_firing_rate / 50.0) * 0.6 + neuron_activity_ratio * 0.4
        return min(1.0, activity_score)
    
    def _calculate_learning_progress(self, developmental_stage) -> float:
        """Calculate learning progress based on developmental stage."""
        # Map developmental stage to progress score
        stage_scores = {
            'DevelopmentalStage.Initial': 0.0,
            'DevelopmentalStage.CriticalPeriod': 0.3,
            'DevelopmentalStage.Maturation': 0.6,
            'DevelopmentalStage.Adult': 0.9,
            'DevelopmentalStage.Aging': 1.0
        }
        
        stage_name = str(developmental_stage)
        return stage_scores.get(stage_name, 0.0)
    
    def _check_performance_targets(self, metrics: Dict[str, Any]) -> List[str]:
        """Check if metrics violate performance targets."""
        violations = []
        
        targets = self.performance_targets
        
        # Check firing rate
        firing_rate = metrics['avg_firing_rate']
        if firing_rate < targets['firing_rate']['min'] or firing_rate > targets['firing_rate']['max']:
            violations.append(f"Firing rate out of range: {firing_rate:.2f} Hz (target: {targets['firing_rate']['min']}-{targets['firing_rate']['max']})")
        
        # Check memory usage
        memory_mb = metrics['memory_usage_mb']
        if memory_mb < targets['memory_usage']['min'] or memory_mb > targets['memory_usage']['max']:
            violations.append(f"Memory usage out of range: {memory_mb:.1f} MB (target: {targets['memory_usage']['min']}-{targets['memory_usage']['max']})")
        
        # Check processing time
        proc_time = metrics['processing_time']
        if proc_time < targets['processing_time']['min'] or proc_time > targets['processing_time']['max']:
            violations.append(f"Processing time out of range: {proc_time:.4f} s (target: {targets['processing_time']['min']}-{targets['processing_time']['max']})")
        
        return violations
    
    def _check_system_health(self, metrics: Dict[str, Any]) -> bool:
        """Check if system is healthy based on metrics."""
        # System health criteria
        health_criteria = [
            metrics['memory_percent'] < 90,  # Memory usage < 90%
            metrics['cpu_percent'] < 85,     # CPU usage < 85%
            metrics['stability'] > 0.5,      # Stability > 0.5
            metrics['steps_per_second'] > 0.1  # Reasonable processing speed
        ]
        
        return all(health_criteria)
    
    def _update_plots(self, step: int):
        """Update real-time plots with new metrics."""
        if not hasattr(self, 'axes'):
            return
        
        recent_metrics = self.metrics_history[-100:] if len(self.metrics_history) > 100 else self.metrics_history
        
        if len(recent_metrics) < 2:
            return
        
        # Extract data for plotting
        steps = [m['step'] for m in recent_metrics]
        
        # Firing rate plot
        firing_rates = [m['avg_firing_rate'] for m in recent_metrics]
        self.axes[0, 0].clear()
        self.axes[0, 0].plot(steps, firing_rates, 'b-', alpha=0.7, linewidth=1)
        self.axes[0, 0].set_ylabel('Firing Rate (Hz)')
        self.axes[0, 0].set_title('Firing Rate Over Time')
        self.axes[0, 0].grid(True, alpha=0.3)
        
        # Spike count plot
        spike_counts = [m['total_spikes'] for m in recent_metrics]
        self.axes[0, 1].clear()
        self.axes[0, 1].plot(steps, spike_counts, 'r-', alpha=0.7, linewidth=1)
        self.axes[0, 1].set_ylabel('Total Spikes')
        self.axes[0, 1].set_title('Spike Accumulation')
        self.axes[0, 1].grid(True, alpha=0.3)
        
        # Memory usage plot
        memory_mb = [m['memory_usage_mb'] for m in recent_metrics]
        self.axes[0, 2].clear()
        self.axes[0, 2].plot(steps, memory_mb, 'g-', alpha=0.7, linewidth=1)
        self.axes[0, 2].set_ylabel('Memory (MB)')
        self.axes[0, 2].set_title('Memory Usage')
        self.axes[0, 2].grid(True, alpha=0.3)
        
        # Processing time plot
        proc_times = [m['processing_time'] for m in recent_metrics]
        self.axes[1, 0].clear()
        self.axes[1, 0].plot(steps, proc_times, 'orange', alpha=0.7, linewidth=1)
        self.axes[1, 0].set_ylabel('Time (s)')
        self.axes[1, 0].set_title('Processing Time per Step')
        self.axes[1, 0].grid(True, alpha=0.3)
        
        # Stability plot
        stability_scores = [m['stability'] for m in recent_metrics]
        self.axes[1, 1].clear()
        self.axes[1, 1].plot(steps, stability_scores, 'purple', alpha=0.7, linewidth=1)
        self.axes[1, 1].set_ylabel('Stability (0-1)')
        self.axes[1, 1].set_title('System Stability')
        self.axes[1, 1].set_ylim(0, 1)
        self.axes[1, 1].grid(True, alpha=0.3)
        
        # Correlation plot
        if len(recent_metrics) >= 10:
            # Calculate correlation between firing rate and stability
            firing_rates = [m['avg_firing_rate'] for m in recent_metrics[-20:]]
            stabilities = [m['stability'] for m in recent_metrics[-20:]]
            
            if len(firing_rates) > 1 and len(stabilities) > 1:
                corr = np.corrcoef(firing_rates, stabilities)[0, 1]
                self.axes[1, 2].clear()
                self.axes[1, 2].bar(['Correlation'], [corr], color=['blue' if corr > 0 else 'red'])
                self.axes[1, 2].set_ylabel('Correlation (Firing Rate vs Stability)')
                self.axes[1, 2].set_title('Relationship Analysis')
                self.axes[1, 2].axhline(y=0, color='gray', linestyle='--', alpha=0.5)
                self.axes[1, 2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.pause(0.01)  # Small pause to allow GUI update
    
    def generate_performance_report(self) -> Dict[str, Any]:
        """Generate comprehensive performance report."""
        if not self.metrics_history:
            return {'error': 'No metrics data available'}
        
        recent_metrics = self.metrics_history[-100:] if len(self.metrics_history) > 100 else self.metrics_history
        
        report = {
            'summary': {
                'total_steps': len(recent_metrics),
                'total_time': recent_metrics[-1]['elapsed_time'],
                'final_step': recent_metrics[-1]['step'],
                'final_firing_rate': recent_metrics[-1]['avg_firing_rate'],
                'final_stability': recent_metrics[-1]['stability'],
                'final_memory_usage_mb': recent_metrics[-1]['memory_usage_mb'],
                'system_healthy': recent_metrics[-1]['system_healthy']
            },
            
            'statistics': {
                'firing_rate_mean': np.mean([m['avg_firing_rate'] for m in recent_metrics]),
                'firing_rate_std': np.std([m['avg_firing_rate'] for m in recent_metrics]),
                'firing_rate_min': np.min([m['avg_firing_rate'] for m in recent_metrics]),
                'firing_rate_max': np.max([m['avg_firing_rate'] for m in recent_metrics]),
                'stability_mean': np.mean([m['stability'] for m in recent_metrics]),
                'stability_std': np.std([m['stability'] for m in recent_metrics]),
                'memory_mean': np.mean([m['memory_usage_mb'] for m in recent_metrics]),
                'memory_std': np.std([m['memory_usage_mb'] for m in recent_metrics]),
                'processing_time_mean': np.mean([m['processing_time'] for m in recent_metrics]),
                'processing_time_std': np.std([m['processing_time'] for m in recent_metrics])
            },
            
            'performance_targets': {
                'violations': [],
                'compliance_rate': self._calculate_target_compliance(recent_metrics)
            },
            
            'trends': {
                'firing_rate_trend': self._calculate_trend(recent_metrics, 'avg_firing_rate'),
                'stability_trend': self._calculate_trend(recent_metrics, 'stability'),
                'memory_trend': self._calculate_trend(recent_metrics, 'memory_usage_mb')
            },
            
            'recommendations': self._generate_recommendations(recent_metrics)
        }
        
        return report
    
    def _calculate_target_compliance(self, metrics: List[Dict[str, Any]]) -> float:
        """Calculate compliance rate with performance targets."""
        if not metrics:
            return 0.0
        
        target_compliance = []
        
        for metric in metrics:
            violations = metric.get('performance_violations', [])
            compliant = len(violations) == 0
            target_compliance.append(1.0 if compliant else 0.0)
        
        return np.mean(target_compliance) if target_compliance else 0.0
    
    def _calculate_trend(self, metrics: List[Dict[str, Any]], metric_name: str) -> Dict[str, Any]:
        """Calculate trend for a specific metric."""
        if len(metrics) < 5:
            return {'trend': 'insufficient_data', 'coefficient': 0.0, 'p_value': 1.0}
        
        # Extract metric values and step numbers
        values = [m[metric_name] for m in metrics]
        steps = list(range(len(values)))
        
        # Calculate linear regression
        try:
            slope, intercept = np.polyfit(steps, values, 1)
            
            # Calculate R-squared
            predicted = [slope * x + intercept for x in steps]
            ss_res = np.sum((np.array(values) - np.array(predicted)) ** 2)
            ss_tot = np.sum((np.array(values) - np.mean(values)) ** 2)
            r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0.0
            
            # Determine trend
            if abs(slope) < 0.001:
                trend = 'stable'
            elif slope > 0:
                trend = 'increasing'
            else:
                trend = 'decreasing'
            
            return {
                'trend': trend,
                'coefficient': slope,
                'r_squared': r_squared,
                'direction': 'up' if slope > 0 else 'down' if slope < 0 else 'flat'
            }
            
        except Exception as e:
            print(f"Error calculating trend: {e}")
            return {'trend': 'error', 'coefficient': 0.0, 'p_value': 1.0}
    
    def _generate_recommendations(self, metrics: List[Dict[str, Any]]) -> List[str]:
        """Generate recommendations based on performance analysis."""
        recommendations = []
        
        if len(metrics) < 10:
            recommendations.append("Collect more metrics for better analysis")
            return recommendations
        
        recent_metrics = metrics[-50:]  # Last 50 data points
        
        # Check firing rate stability
        firing_rates = [m['avg_firing_rate'] for m in recent_metrics]
        firing_rate_cv = np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0
        
        if firing_rate_cv > 0.5:
            recommendations.append("⚠️ High firing rate variability detected. Consider adjusting neural parameters or checking for external influences.")
        
        # Check stability trends
        stability_trend = self._calculate_trend(recent_metrics, 'stability')
        if stability_trend['trend'] == 'decreasing':
            recommendations.append("⚠️ Stability is decreasing over time. Consider system maintenance or parameter tuning.")
        
        # Check memory usage
        memory_usages = [m['memory_usage_mb'] for m in recent_metrics]
        recent_memory = memory_usages[-10:]
        if np.mean(recent_memory) > 512:  # High memory usage
            recommendations.append("⚠️ High memory usage detected. Consider optimizing memory allocation or reducing simulation complexity.")
        
        # Check processing performance
        processing_times = [m['processing_time'] for m in recent_metrics]
        if np.mean(processing_times) > 1.0:  # Slow processing
            recommendations.append("⚠️ Slow processing time. Consider parallel processing or optimization of neural computation.")
        
        if not recommendations:
            recommendations.append("✅ Performance metrics are within expected ranges. System is operating normally.")
        
        return recommendations
    
    def save_metrics_to_file(self, filename: str):
        """Save metrics to file."""
        try:
            # Convert metrics to serializable format
            serializable_metrics = []
            for metric in self.metrics_history:
                serializable_metric = metric.copy()
                # Convert any non-serializable values
                for key, value in serializable_metric.items():
                    if isinstance(value, dict):
                        serializable_metric[key] = self._convert_to_serializable(value)
                serializable_metrics.append(serializable_metric)
            
            # Save to file
            with open(filename, 'w') as f:
                json.dump(serializable_metrics, f, indent=2)
            
            print(f"Metrics saved to {filename}")
            return True
            
        except Exception as e:
            print(f"Error saving metrics: {e}")
            return False
    
    def _convert_to_serializable(self, value: Any) -> Any:
        """Convert value to JSON-serializable format."""
        if isinstance(value, dict):
            return {k: self._convert_to_serializable(v) for k, v in value.items()}
        elif isinstance(value, list):
            return [self._convert_to_serializable(v) for v in value]
        elif isinstance(value, (int, float, str, bool, type(None))):
            return value
        else:
            return str(value)
    
    def _get_default_metrics(self, step: int) -> Dict[str, Any]:
        """Get default metrics when data collection fails."""
        return {
            'timestamp': time.time(),
            'step': step,
            'elapsed_time': 0,
            'total_spikes': 0,
            'firing_neuron_count': 0,
            'avg_firing_rate': 0,
            'excitation_inhibition_ratio': 1.0,
            'developmental_stage': 'DevelopmentalStage.Initial',
            'memory_usage_mb': 0,
            'memory_available_mb': 0,
            'memory_percent': 0,
            'cpu_percent': 0,
            'processing_time': 0,
            'steps_per_second': 0,
            'spike_rate': 0,
            'stability': 0,
            'neuron_activity_ratio': 0,
            'spikes_per_neuron': 0,
            'firing_efficiency': 0,
            'network_activity': 0,
            'learning_progress': 0,
            'performance_violations': ['Metrics collection failed'],
            'system_healthy': False
        }

# Usage example
if __name__ == "__main__":
    print("=== Advanced Performance Monitoring Example ===\n")
    
    # Create brain configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create performance monitor
    monitor = PerformanceMonitor(brain)
    
    print("Starting performance monitoring...")
    print("Simulating neural activity for 100 steps...")
    
    # Run simulation with monitoring
    for step in range(100):
        brain.step(step)
        
        # Collect metrics
        metrics = monitor.collect_metrics(step, brain)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: Firing rate = {metrics['avg_firing_rate']:.2f} Hz, "
                  f"Stability = {metrics['stability']:.3f}, "
                  f"Memory = {metrics['memory_usage_mb']:.1f} MB")
        
        # Small delay for demo
        time.sleep(0.01)
    
    print("\nGenerating performance report...")
    
    # Generate and display report
    report = monitor.generate_performance_report()
    
    print("\n=== Performance Report ===")
    print(f"Total steps: {report['summary']['total_steps']}")
    print(f"Final firing rate: {report['summary']['final_firing_rate']:.2f} Hz")
    print(f"Final stability: {report['summary']['final_stability']:.3f}")
    print(f"Final memory usage: {report['summary']['final_memory_usage_mb']:.1f} MB")
    print(f"System healthy: {'✅ Yes' if report['summary']['system_healthy'] else '❌ No'}")
    
    print(f"\nFiring Rate Statistics:")
    print(f"  Mean: {report['statistics']['firing_rate_mean']:.2f} Hz")
    print(f"  Std: {report['statistics']['firing_rate_std']:.2f} Hz")
    print(f"  Min: {report['statistics']['firing_rate_min']:.2f} Hz")
    print(f"  Max: {report['statistics']['firing_rate_max']:.2f} Hz")
    
    print(f"\nStability Statistics:")
    print(f"  Mean: {report['statistics']['stability_mean']:.3f}")
    print(f"  Std: {report['statistics']['stability_std']:.3f}")
    
    print(f"\nTarget Compliance: {report['performance_targets']['compliance_rate']*100:.1f}%")
    
    print(f"\nTrends:")
    print(f"  Firing Rate: {report['trends']['firing_rate_trend']['trend']}")
    print(f"  Stability: {report['trends']['stability_trend']['trend']}")
    print(f"  Memory: {report['trends']['memory_trend']['trend']}")
    
    print(f"\nRecommendations:")
    for recommendation in report['recommendations']:
        print(f"  {recommendation}")
    
    # Save metrics to file
    monitor.save_metrics_to_file("simulation_metrics.json")
    
    print(f"\nMetrics saved to simulation_metrics.json")
```