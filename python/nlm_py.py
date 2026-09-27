#!/usr/bin/env python3
"""
Enhanced NLM Python Wrapper - Advanced API for NLM (Neural Learning Machine)

This is the enhanced Python wrapper for the NLM (Neural Learning Machine) C++ framework.
It provides a Pythonic interface to all the advanced features implemented in the C++ bindings.

The wrapper follows Python best practices and provides:
- Context managers for resource management
- Pythonic error handling and exceptions
- Advanced configuration management
- High-level convenience functions
- Documentation and examples

For the complete API documentation, see API_REFERENCE.md
"""

import json
import logging
import os
import sys
import time
from contextlib import contextmanager
from pathlib import Path
from typing import Any, Dict, List, Optional, Union

import numpy as np

# Import the core NLM bindings
try:
    import pynlm
    HAS_PYNL = True
except ImportError:
    HAS_PYNL = False
    print("Warning: pynlm module not available. Install with: pip install pynlm")

logger = logging.getLogger(__name__)

# Version information
__version__ = "2.0.0"
__author__ = "NLM Team"
__license__ = "MIT"

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
class NLMError(Exception):
    """Base exception class for all NLM-related errors."""
    pass
class ConfigurationError(NLMError):
    """Raised when there is an error in configuration."""
    pass
class SimulationError(NLMError):
    """Raised when there is an error during simulation."""
    pass
class ValidationError(NLMError):
    """Raised when validation fails."""
    pass
class MemoryError(NLMError):
    """Raised when there is an error in memory management."""
    pass
class NLMContext:
    """
    Context manager for NLM simulation sessions.
    
    Ensures proper resource cleanup and logging of simulation sessions.
    """
    
    def __init__(self, name: str = "nlm_session"):
        """
        Initialize NLM context.
        
        Args:
            name: Name for the simulation session
        """
        self.name = name
        self.start_time = None
        self.end_time = None
        self.config = None
        self.brain = None
        self.world = None
        self.agent_brain = None
        
    def __enter__(self):
        """Enter the context manager."""
        logger.info(f"Entering NLM context: {self.name}")
        self.start_time = time.time()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit the context manager and clean up resources."""
        self.end_time = time.time()
        duration = self.end_time - self.start_time
        logger.info(f"Exiting NLM context: {self.name} (duration: {duration:.2f}s)")
        
        # Clean up resources
        if self.agent_brain:
            self.agent_brain.clear()
        if self.brain:
            self.brain.reset()
        if self.world:
            self.world.reset()
            
        if exc_type:
            logger.error(f"Context {self.name} exited with exception: {exc_val}")
        else:
            logger.info(f"Context {self.name} completed successfully")
        
        # Re-raise exception if any
        return False
    
    def get_duration(self) -> float:
        """Get context duration in seconds."""
        if self.start_time and self.end_time:
            return self.end_time - self.start_time
        elif self.start_time:
            return time.time() - self.start_time
        return 0.0
class AdvancedConfig:
    """
    Enhanced configuration manager with Pythonic interface.
    
    Provides advanced configuration features including validation,
    schema support, inheritance, and environment variable integration.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """
        Initialize advanced configuration.
        
        Args:
            config_file: Optional path to configuration file
        """
        if not HAS_PYNL:
            raise NLMError("pynlm module not available")
            
        self._config = pynlm.Config()
        self._schema: Dict[str, Any] = {}
        self._environment_prefix = "NLM_"
        
        if config_file:
            self.load_from_file(config_file)
            
        # Define common schema
        self._define_default_schema()
        
    def _define_default_schema(self):
        """Define default configuration schema."""
        self._schema = {
            "random_seed": {"type": "int", "default": 42, "min": 0},
            "simulation_timestep": {"type": "float", "default": 0.001, "min": 0},
            "neuron_count": {"type": "int", "default": 1000, "min": 1},
            "region_count": {"type": "int", "default": 2, "min": 1},
            "connection_probability": {"type": "float", "default": 0.1, "min": 0, "max": 1},
            "plasticity_learning_rate": {"type": "float", "default": 0.01, "min": 0},
            "stdp_ltp_weight": {"type": "float", "default": 0.01, "min": 0},
            "stdp_ltd_weight": {"type": "float", "default": 0.012, "min": 0},
            "stdp_time_constant": {"type": "float", "default": 20.0, "min": 0},
            "development_synaptogenesis_rate": {"type": "float", "default": 0.001, "min": 0},
            "development_pruning_rate": {"type": "float", "default": 0.0001, "min": 0},
            "dopamine_baseline": {"type": "float", "default": 0.1, "min": 0},
            "reward_discount_factor": {"type": "float", "default": 0.99, "min": 0, "max": 1},
            "environment_name": {"type": "str", "default": "GridWorld"},
            "environment_width": {"type": "int", "default": 10, "min": 1},
            "environment_height": {"type": "int", "default": 10, "min": 1},
            "max_simulation_steps": {"type": "int", "default": 10000, "min": 1},
            "visualization_enabled": {"type": "bool", "default": False},
            "log_level": {"type": "str", "default": "INFO"},
            "log_to_file": {"type": "bool", "default": False},
            "log_filename": {"type": "str", "default": "nlm.log"}
        }
        
    def load_from_file(self, filepath: str, format: str = "json") -> bool:
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            format: File format (json, yaml, cfg)
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if not Path(filepath).exists():
                raise ConfigurationError(f"Configuration file not found: {filepath}")
                
            if format.lower() == "json":
                with open(filepath, 'r') as f:
                    data = json.load(f)
                self._config.fromDict(data)
            elif format.lower() == "yaml":
                import yaml
                with open(filepath, 'r') as f:
                    data = yaml.safe_load(f)
                self._config.fromDict(data)
            elif format.lower() == "cfg":
                self._load_cfg_file(filepath)
            else:
                raise ConfigurationError(f"Unsupported format: {format}")
                
            logger.info(f"Configuration loaded from {filepath}")
            return True
            
        except Exception as e:
            raise ConfigurationError(f"Failed to load configuration from {filepath}: {e}")
            
    def _load_cfg_file(self, filepath: str):
        """Load configuration from .cfg file format."""
        config_dict = {}
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Type conversion
                    if value.lower() in ('true', 'false'):
                        value = value.lower() == 'true'
                    elif value.isdigit():
                        value = int(value)
                    elif '.' in value and value.replace('.', '', 1).isdigit():
                        value = float(value)
                        
                    config_dict[key] = value
                    
        self._config.fromDict(config_dict)
        
    def load_from_env(self, prefix: Optional[str] = None) -> None:
        """
        Load configuration from environment variables.
        
        Args:
            prefix: Environment variable prefix (defaults to NLM_)
        """
        if prefix is None:
            prefix = self._environment_prefix
            
        for key, value in os.environ.items():
            if key.startswith(prefix):
                config_key = key[len(prefix):].lower()
                
                # Type conversion
                if value.lower() in ('true', 'false'):
                    converted_value = value.lower() == 'true'
                elif value.isdigit():
                    converted_value = int(value)
                elif '.' in value and value.replace('.', '', 1).isdigit():
                    converted_value = float(value)
                else:
                    converted_value = value
                    
                self.set_value(config_key, converted_value)
                logger.debug(f"Loaded from environment: {config_key} = {converted_value}")
                
    def validate(self) -> List[str]:
        """
        Validate configuration.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = self._config.validate()
        
        # Additional schema validation
        for key, schema in self._schema.items():
            if schema.get('required', False) and not self._config.has(key):
                errors.append(f"Required field missing: {key}")
                
            if key in self._config.toDict():
                value = self._config.get(key)
                if 'type' in schema:
                    if not self._validate_type(value, schema['type']):
                        errors.append(f"Invalid type for {key}: expected {schema['type']}, got {type(value)}")
                        
                if 'min' in schema and isinstance(value, (int, float)):
                    if value < schema['min']:
                        errors.append(f"Value for {key} below minimum: {value} < {schema['min']}")
                        
                if 'max' in schema and isinstance(value, (int, float)):
                    if value > schema['max']:
                        errors.append(f"Value for {key} above maximum: {value} > {schema['max']}")
                        
        return errors
        
    def _validate_type(self, value: Any, expected_type: str) -> bool:
        """Validate value type against expected type."""
        type_map = {
            'int': int,
            'float': float,
            'bool': bool,
            'str': str,
            'list': list,
            'dict': dict
        }
        
        if expected_type not in type_map:
            return True
            
        return isinstance(value, type_map[expected_type])
        
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value
        """
        return self._config.get(key, default)
        
    def set(self, key: str, value: Any) -> None:
        """
        Set configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        self._config.set(key, value)
        logger.debug(f"Set configuration: {key} = {value}")
        
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert configuration to dictionary.
        
        Returns:
            Configuration as dictionary
        """
        return self._config.toDict()
        
    def from_dict(self, config_dict: Dict[str, Any]) -> None:
        """
        Load configuration from dictionary.
        
        Args:
            config_dict: Configuration dictionary
        """
        self._config.fromDict(config_dict)
        
    def save_to_file(self, filepath: str, format: str = "json") -> bool:
        """
        Save configuration to file.
        
        Args:
            filepath: Path to output file
            format: File format (json, yaml, cfg)
            
        Returns:
            True if successful, False otherwise
        """
        try:
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            if format.lower() == "json":
                with open(filepath, 'w') as f:
                    json.dump(self.to_dict(), f, indent=2)
            elif format.lower() == "yaml":
                import yaml
                with open(filepath, 'w') as f:
                    yaml.dump(self.to_dict(), f, default_flow_style=False)
            elif format.lower() == "cfg":
                with open(filepath, 'w') as f:
                    for key, value in self.to_dict().items():
                        if isinstance(value, bool):
                            f.write(f"{key} = {str(value).lower()}\n")
                        else:
                            f.write(f"{key} = {value}\n")
            else:
                raise ConfigurationError(f"Unsupported format: {format}")
                
            logger.info(f"Configuration saved to {filepath}")
            return True
            
        except Exception as e:
            raise ConfigurationError(f"Failed to save configuration: {e}")
            
    def merge_with_defaults(self) -> None:
        """Merge current configuration with defaults."""
        # Implementation would merge with defaults
        pass
        
    def apply_template(self, template_name: str) -> None:
        """
        Apply predefined configuration template.
        
        Args:
            template_name: Name of template to apply
        """
        # Predefined templates
        templates = {
            "default": {
                "random_seed": 42,
                "simulation_timestep": 0.001,
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
                "plasticity_learning_rate": 0.01
            },
            "large": {
                "random_seed": 123,
                "simulation_timestep": 0.0005,
                "neuron_count": 5000,
                "region_count": 5,
                "connection_probability": 0.05,
                "plasticity_learning_rate": 0.02
            },
            "small": {
                "random_seed": 456,
                "simulation_timestep": 0.002,
                "neuron_count": 100,
                "region_count": 1,
                "connection_probability": 0.2,
                "plasticity_learning_rate": 0.005
            }
        }
        
        if template_name not in templates:
            raise ConfigurationError(f"Unknown template: {template_name}")
            
        self.from_dict(templates[template_name])
        logger.info(f"Applied template: {template_name}")
class NLMBatchProcessor:
    """
    Batch processor for running multiple NLM simulations.
    
    Provides efficient batch processing with parallel execution,
    progress monitoring, and comprehensive reporting.
    """
    
    def __init__(self, max_workers: int = 4):
        """
        Initialize batch processor.
        
        Args:
            max_workers: Maximum number of worker threads
        """
        self.max_workers = max_workers
        self.results: List[Dict[str, Any]] = []
        self.processors: List['NLMBatchProcessor'] = []
        
    def create_batch_config(self, base_config: Dict[str, Any],
                          variations: List[Dict[str, Any]],
                          output_dir: str = "batch_configs") -> List[str]:
        """
        Create batch configuration files from base configuration.
        
        Args:
            base_config: Base configuration dictionary
            variations: List of configuration variations
            output_dir: Output directory for configuration files
            
        Returns:
            List of created configuration file paths
        """
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        created_files = []
        
        for i, variation in enumerate(variations):
            # Create variation by merging with base
            config_dict = base_config.copy()
            config_dict.update(variation)
            
            # Create config file
            config_file = Path(output_dir) / f"config_{i+1:04d}.cfg"
            
            with open(config_file, 'w') as f:
                f.write("# NLM Batch Configuration File\n")
                f.write(f"# Variation {i+1}\n")
                
                for key, value in config_dict.items():
                    if isinstance(value, bool):
                        f.write(f"{key} = {str(value).lower()}\n")
                    elif isinstance(value, (list, dict)):
                        f.write(f"{key} = {json.dumps(value)}\n")
                    else:
                        f.write(f"{key} = {value}\n")
                        
            created_files.append(str(config_file))
            logger.info(f"Created batch config: {config_file}")
            
        return created_files
        
    def run_batch(self, config_files: List[str],
                 step_func: callable = None,
                 **kwargs) -> List[Dict[str, Any]]:
        """
        Run batch processing on multiple configuration files.
        
        Args:
            config_files: List of configuration file paths
            step_func: Function to call for each configuration
            **kwargs: Additional arguments for step_func
            
        Returns:
            List of processing results
        """
        from concurrent.futures import ThreadPoolExecutor, as_completed
        
        results = []
        
        if len(config_files) <= 1 or self.max_workers <= 1:
            # Sequential processing
            for config_file in config_files:
                result = self._process_single_config(config_file, step_func, **kwargs)
                results.append(result)
        else:
            # Parallel processing
            with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
                # Submit all tasks
                future_to_file = {
                    executor.submit(self._process_single_config, config_file, step_func, **kwargs): config_file
                    for config_file in config_files
                }
                
                # Collect results as they complete
                for future in as_completed(future_to_file):
                    config_file = future_to_file[future]
                    try:
                        result = future.result()
                        results.append(result)
                    except Exception as e:
                        logger.error(f"Error processing {config_file}: {e}")
                        results.append({
                            "config_file": config_file,
                            "status": "error",
                            "timestamp": time.time(),
                            "errors": [str(e)],
                            "output": None
                        })
        
        self.results = results
        return results
        
    def _process_single_config(self, config_file: str,
                             step_func: callable,
                             **kwargs) -> Dict[str, Any]:
        """Process a single configuration file."""
        result = {
            "config_file": config_file,
            "status": "success",
            "timestamp": time.time(),
            "errors": [],
            "output": None
        }
        
        try:
            logger.info(f"Processing {config_file}...")
            
            # Call the processing function
            output = step_func(config_file, **kwargs)
            result["output"] = output
            
        except Exception as e:
            result["status"] = "error"
            result["errors"].append(str(e))
            logger.error(f"Error processing {config_file}: {e}")
        
        return result
        
    def generate_report(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Generate comprehensive report from batch processing results.
        
        Args:
            results: List of processing results
            
        Returns:
            Comprehensive report dictionary
        """
        total_files = len(results)
        successful = sum(1 for r in results if r["status"] == "success")
        failed = total_files - successful
        
        # Count different types of errors
        error_types = {}
        for result in results:
            if result["status"] == "error":
                for error in result["errors"]:
                    error_type = error.split(':')[0] if ':' in error else "Unknown"
                    error_types[error_type] = error_types.get(error_type, 0) + 1
        
        # Calculate processing time
        processing_times = [r.get("processing_time", 0.1) for r in results if r.get("processing_time")]
        processing_time = sum(processing_times) if processing_times else 0
        
        report = {
            "batch_summary": {
                "total_files": total_files,
                "successful": successful,
                "failed": failed,
                "success_rate": successful / total_files if total_files > 0 else 0,
                "processing_time_seconds": processing_time,
                "average_time_per_file": processing_time / total_files if total_files > 0 else 0,
                "max_workers": self.max_workers
            },
            "error_summary": error_types,
            "detailed_results": results,
            "timestamp": time.time(),
            "completed_at": time.strftime("%Y-%m-%d %H:%M:%S")
        }
        
        return report
        
    def export_results(self, results: List[Dict[str, Any]], output_file: str,
                     format: str = "json") -> bool:
        """
        Export batch results to file.
        
        Args:
            results: List of processing results
            output_file: Output file path
            format: Export format
            
        Returns:
            True if successful, False otherwise
        """
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            if format.lower() == "json":
                with open(output_file, 'w') as f:
                    json.dump(results, f, indent=2)
            elif format.lower() == "yaml":
                import yaml
                with open(output_file, 'w') as f:
                    yaml.dump(results, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
                
            logger.info(f"Batch results exported to {output_file}")
            return True
            
        except Exception as e:
            logger.error(f"Error exporting results: {e}")
            return False
class NLMValidator:
    """
    Validator for NLM configurations and parameters.
    
    Provides comprehensive validation of configurations, parameters,
    and simulation settings with detailed error reporting.
    """
    
    def __init__(self):
        self.validation_rules: Dict[str, List[callable]] = {}
        self._define_validation_rules()
        
    def _define_validation_rules(self):
        """Define validation rules for different parameter types."""
        self.validation_rules = {
            "neuron_count": [self._validate_neuron_count],
            "region_count": [self._validate_region_count],
            "connection_probability": [self._validate_connection_probability],
            "learning_rate": [self._validate_learning_rate],
            "timestep": [self._validate_timestep],
            "environment_width": [self._validate_dimension],
            "environment_height": [self._validate_dimension],
            "max_steps": [self._validate_max_steps]
        }
        
    def _validate_neuron_count(self, value):
        """Validate neuron count."""
        if not isinstance(value, int):
            return False, f"Neuron count must be integer, got {type(value)}"
        if value < 1:
            return False, f"Neuron count must be positive, got {value}"
        if value > 100000:
            return False, f"Neuron count too large (max 100000), got {value}"
        return True, "Valid neuron count"
        
    def _validate_region_count(self, value):
        """Validate region count."""
        if not isinstance(value, int):
            return False, f"Region count must be integer, got {type(value)}"
        if value < 1:
            return False, f"Region count must be positive, got {value}"
        if value > 100:
            return False, f"Region count too large (max 100), got {value}"
        return True, "Valid region count"
        
    def _validate_connection_probability(self, value):
        """Validate connection probability."""
        if not isinstance(value, (int, float)):
            return False, f"Connection probability must be numeric, got {type(value)}"
        if value < 0 or value > 1:
            return False, f"Connection probability must be between 0 and 1, got {value}"
        return True, "Valid connection probability"
        
    def _validate_learning_rate(self, value):
        """Validate learning rate."""
        if not isinstance(value, (int, float)):
            return False, f"Learning rate must be numeric, got {type(value)}"
        if value < 0:
            return False, f"Learning rate must be non-negative, got {value}"
        if value > 1:
            return False, f"Learning rate too large (max 1), got {value}"
        return True, "Valid learning rate"
        
    def _validate_timestep(self, value):
        """Validate timestep."""
        if not isinstance(value, (int, float)):
            return False, f"Timestep must be numeric, got {type(value)}"
        if value <= 0:
            return False, f"Timestep must be positive, got {value}"
        if value > 0.1:
            return False, f"Timestep too large (max 0.1), got {value}"
        return True, "Valid timestep"
        
    def _validate_dimension(self, value):
        """Validate dimension (width or height)."""
        if not isinstance(value, int):
            return False, f"Dimension must be integer, got {type(value)}"
        if value < 1:
            return False, f"Dimension must be positive, got {value}"
        if value > 1000:
            return False, f"Dimension too large (max 1000), got {value}"
        return True, "Valid dimension"
        
    def _validate_max_steps(self, value):
        """Validate maximum steps."""
        if not isinstance(value, int):
            return False, f"Max steps must be integer, got {type(value)}"
        if value < 1:
            return False, f"Max steps must be positive, got {value}"
        if value > 1000000:
            return False, f"Max steps too large (max 1000000), got {value}"
        return True, "Valid max steps"
        
    def validate_config(self, config: Dict[str, Any]) -> List[Dict[str, Any]]:
        """
        Validate configuration dictionary.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            List of validation results
        """
        results = []
        
        for key, value in config.items():
            # Check if validation rule exists for this key
            if key in self.validation_rules:
                for validator in self.validation_rules[key]:
                    is_valid, message = validator(value)
                    results.append({
                        "parameter": key,
                        "value": value,
                        "valid": is_valid,
                        "message": message
                    })
            else:
                # No specific rule, just report presence
                results.append({
                    "parameter": key,
                    "value": value,
                    "valid": True,
                    "message": "No validation rule specified"
                })
                
        return results
        
    def validate_simulation_parameters(self, params: Dict[str, Any]) -> bool:
        """
        Validate simulation parameters.
        
        Args:
            params: Simulation parameters
            
        Returns:
            True if valid, False otherwise
        """
        validation_results = self.validate_config(params)
        
        all_valid = all(result["valid"] for result in validation_results)
        if not all_valid:
            invalid_results = [r for r in validation_results if not r["valid"]]
            for result in invalid_results:
                logger.warning(f"Invalid parameter {result['parameter']}: {result['message']}")
                
        return all_valid
class NLMComparison:
    """
    Comparison tools for NLM simulation results.
    
    Provides comparison of simulation results, statistical analysis,
    and visualization of differences between simulations.
    """
    
    def __init__(self):
        self.comparison_results: List[Dict[str, Any]] = []
        
    def compare_results(self, results_list: List[Dict[str, Any]],
                       comparison_keys: List[str] = None) -> Dict[str, Any]:
        """
        Compare multiple simulation results.
        
        Args:
            results_list: List of simulation result dictionaries
            comparison_keys: Keys to compare (defaults to common keys)
            
        Returns:
            Comparison results dictionary
        """
        if comparison_keys is None:
            # Find common keys across all results
            if not results_list:
                return {}
                
            common_keys = set(results_list[0].keys())
            for result in results_list[1:]:
                common_keys.intersection_update(result.keys())
                
            comparison_keys = list(common_keys)
            
        comparison_result = {
            "num_comparisons": len(results_list),
            "comparison_keys": comparison_keys,
            "results": [],
            "summary": {}
        }
        
        for i, result in enumerate(results_list):
            comparison_result["results"].append({
                "index": i,
                "result": result
            })
            
        # Calculate summary statistics
        for key in comparison_keys:
            values = []
            for result in results_list:
                if key in result and isinstance(result[key], (int, float)):
                    values.append(result[key])
                    
            if values:
                comparison_result["summary"][key] = {
                    "mean": sum(values) / len(values),
                    "min": min(values),
                    "max": max(values),
                    "std": (sum((x - sum(values)/len(values))**2 for x in values) / len(values))**0.5 if len(values) > 1 else 0,
                    "count": len(values)
                }
                
        self.comparison_results.append(comparison_result)
        return comparison_result
        
    def get_comparison_statistics(self, comparison_result: Dict[str, Any]) -> Dict[str, Any]:
        """
        Get detailed statistics for comparison.
        
        Args:
            comparison_result: Comparison result dictionary
            
        Returns:
            Statistics dictionary
        """
        stats = {
            "num_simulations": comparison_result["num_comparisons"],
            "compared_keys": len(comparison_result["comparison_keys"]),
            "has_summary": bool(comparison_result["summary"]),
            "timestamp": time.time()
        }
        
        return stats
        
    def generate_comparison_report(self, comparison_result: Dict[str, Any],
                                 format: str = "text") -> str:
        """
        Generate comparison report.
        
        Args:
            comparison_result: Comparison result dictionary
            format: Report format (text, json, yaml)
            
        Returns:
            Report string
        """
        if format.lower() == "text":
            return self._generate_text_report(comparison_result)
        elif format.lower() == "json":
            import json
            return json.dumps(comparison_result, indent=2)
        elif format.lower() == "yaml":
            import yaml
            return yaml.dump(comparison_result, default_flow_style=False)
        else:
            raise ValueError(f"Unsupported format: {format}")
            
    def _generate_text_report(self, comparison_result: Dict[str, Any]) -> str:
        """Generate text report."""
        lines = []
        lines.append("=== NLM Simulation Comparison Report ===")
        lines.append(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        lines.append(f"Number of simulations compared: {comparison_result['num_comparisons']}")
        lines.append(f"Parameters compared: {', '.join(comparison_result['comparison_keys'])}")
        lines.append("")
        
        if comparison_result["summary"]:
            lines.append("Summary Statistics:")
            for key, stats in comparison_result["summary"].items():
                lines.append(f"  {key}:")
                lines.append(f"    Mean: {stats['mean']:.4f}")
                lines.append(f"    Min:  {stats['min']:.4f}")
                lines.append(f"    Max: {stats['max']:.4f}")
                lines.append(f"    Std:  {stats['std']:.4f}")
                lines.append(f"    N:    {stats['count']}")
                lines.append("")
                
        return "\n".join(lines)
class NLMProfiler:
    """
    Profiler for NLM simulations.
    
    Provides performance profiling and timing analysis for NLM simulations.
    """
    
    def __init__(self):
        self.profiling_data: List[Dict[str, Any]] = []
        self.current_profile = None
        
    def start_profile(self, name: str = "simulation") -> str:
        """
        Start profiling a simulation.
        
        Args:
            name: Profile name
            
        Returns:
            Profile ID
        """
        profile_id = f"{name}_{int(time.time() * 1000)}"
        
        self.current_profile = {
            "id": profile_id,
            "name": name,
            "start_time": time.time(),
            "end_time": None,
            "duration": None,
            "memory_usage": None,
            "steps": 0,
            "neurons": 0,
            "config": None
        }
        
        logger.info(f"Started profiling: {profile_id} ({name})")
        return profile_id
        
    def stop_profile(self, profile_id: str) -> Dict[str, Any]:
        """
        Stop profiling.
        
        Args:
            profile_id: Profile ID
            
        Returns:
            Profile data
        """
        for profile in self.profiling_data:
            if profile["id"] == profile_id:
                profile["end_time"] = time.time()
                profile["duration"] = profile["end_time"] - profile["start_time"]
                self.current_profile = None
                logger.info(f"Stopped profiling: {profile_id}")
                return profile
                
        raise ValueError(f"Profile not found: {profile_id}")
        
    def record_step(self, profile_id: str, neurons: int = None, memory: int = None):
        """
        Record simulation step.
        
        Args:
            profile_id: Profile ID
            neurons: Number of neurons
            memory: Memory usage in bytes
        """
        if self.current_profile and self.current_profile["id"] == profile_id:
            self.current_profile["steps"] += 1
            if neurons is not None:
                self.current_profile["neurons"] = neurons
            if memory is not None:
                self.current_profile["memory_usage"] = memory
                
    def get_profile_data(self, profile_id: str = None) -> Union[Dict[str, Any], List[Dict[str, Any]]]:
        """
        Get profile data.
        
        Args:
            profile_id: Profile ID (None for all profiles)
            
        Returns:
            Profile data
        """
        if profile_id:
            for profile in self.profiling_data:
                if profile["id"] == profile_id:
                    return profile
            return None
        else:
            return self.profiling_data
            
    def get_performance_summary(self) -> Dict[str, Any]:
        """
        Get performance summary.
        
        Returns:
            Performance summary
        """
        if not self.profiling_data:
            return {"total_profiles": 0, "avg_duration": 0, "total_steps": 0}
            
        total_duration = sum(p["duration"] for p in self.profiling_data if p["duration"])
        avg_duration = total_duration / len(self.profiling_data) if self.profiling_data else 0
        total_steps = sum(p["steps"] for p in self.profiling_data)
        
        return {
            "total_profiles": len(self.profiling_data),
            "avg_duration": avg_duration,
            "total_steps": total_steps,
            "profiles_per_hour": (len(self.profiling_data) / (total_duration / 3600)) if total_duration > 0 else 0
        }
class NLMExporter:
    """
    Exporter for NLM data and results.
    
    Provides export functionality for NLM data in various formats
    including JSON, YAML, CSV, and specialized NLM formats.
    """
    
    def __init__(self):
        self.export_history: List[Dict[str, Any]] = []
        
    def export_simulation_results(self, results: Dict[str, Any],
                                 filepath: str, format: str = "json") -> bool:
        """
        Export simulation results.
        
        Args:
            results: Simulation results
            filepath: Output file path
            format: Export format
            
        Returns:
            True if successful, False otherwise
        """
        try:
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            if format.lower() == "json":
                with open(filepath, 'w') as f:
                    json.dump(results, f, indent=2)
            elif format.lower() == "yaml":
                import yaml
                with open(filepath, 'w') as f:
                    yaml.dump(results, f, default_flow_style=False)
            elif format.lower() == "csv":
                self._export_csv_results(results, filepath)
            else:
                raise ValueError(f"Unsupported format: {format}")
                
            # Record export
            self.export_history.append({
                "timestamp": time.time(),
                "filepath": filepath,
                "format": format,
                "success": True
            })
                
            logger.info(f"Results exported to {filepath}")
            return True
            
        except Exception as e:
            logger.error(f"Error exporting results: {e}")
            
            # Record failed export
            self.export_history.append({
                "timestamp": time.time(),
                "filepath": filepath,
                "format": format,
                "success": False,
                "error": str(e)
            })
                
            return False
            
    def _export_csv_results(self, results: Dict[str, Any], filepath: str):
        """Export results in CSV format."""
        import csv
        
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            
            # Write header
            writer.writerow(results.keys())
            
            # Write data (flatten nested dictionaries)
            flattened = self._flatten_dict(results)
            writer.writerow(flattened.keys())
            writer.writerow(flattened.values())
            
    def _flatten_dict(self, d: Dict[str, Any], parent_key: str = '', sep: str = '_') -> Dict[str, Any]:
        """Flatten nested dictionary."""
        items = []
        for k, v in d.items():
            new_key = f"{parent_key}{sep}{k}" if parent_key else k
            if isinstance(v, dict):
                items.extend(self._flatten_dict(v, new_key, sep=sep).items())
            elif isinstance(v, list):
                for i, item in enumerate(v):
                    items.extend(self._flatten_dict({f"{k}_{i}": item}, new_key, sep=sep).items())
            else:
                items.append((new_key, v))
        return dict(items)
        
    def get_export_history(self) -> List[Dict[str, Any]]:
        """Get export history."""
        return self.export_history
        
    def clear_export_history(self) -> None:
        """Clear export history."""
        self.export_history = []
# High-level convenience functions
def create_brain_from_config(config_file: str = None, config_dict: Dict[str, Any] = None) -> 'pynlm.Brain':
    """
    Create brain from configuration.
    
    Args:
        config_file: Path to configuration file
        config_dict: Configuration dictionary
        
    Returns:
        Brain instance
    """
    if not HAS_PYNL:
        raise NLMError("pynlm module not available")
        
    if config_dict:
        config = pynlm.Config()
        config.fromDict(config_dict)
    elif config_file:
        config = pynlm.Config()
        config.loadFromFile(config_file)
    else:
        config = pynlm.Config()
        
    return pynlm.Brain(config)
def create_simulation(config_file: str = None, **kwargs) -> NLMContext:
    """
    Create NLM simulation context.
    
    Args:
        config_file: Path to configuration file
        **kwargs: Additional configuration parameters
        
    Returns:
        NLMContext instance
    """
    context = NLMContext("simulation")
    
    # Load configuration
    if config_file:
        context.config = pynlm.Config()
        context.config.loadFromFile(config_file)
    else:
        context.config = pynlm.Config()
        
    # Apply additional configuration
    for key, value in kwargs.items():
        context.config.set(key, value)
        
    # Create brain
    context.brain = pynlm.Brain(context.config)
    
    # Create world
    context.world = pynlm.SimpleWorld()
    
    # Create agent brain
    context.agent_brain = pynlm.AgentBrain(context.brain)
    
    # Initialize
    context.brain.initialize()
    context.agent_brain.initialize(context.world)
    
    logger.info(f"Created simulation context: {context.name}")
    return context
def run_simulation(context: NLMContext, steps: int, timestep: float = 0.001) -> Dict[str, Any]:
    """
    Run simulation for specified steps.
    
    Args:
        context: NLM context
        steps: Number of simulation steps
        timestep: Simulation timestep
        
    Returns:
        Simulation results
    """
    if not context.brain:
        raise SimulationError("Brain not initialized in context")
        
    results = {
        "steps": steps,
        "timestep": timestep,
        "start_time": time.time(),
        "status": "running",
        "neuron_count": context.brain.getTotalNeuronCount(),
        "total_spikes": 0,
        "firing_neurons": 0,
        "average_firing_rate": 0,
        "final_spikes": 0,
        "final_firing_neurons": 0
    }
    
    try:
        for step in range(steps):
            context.brain.step(step, timestep)
            
            # Record progress every 100 steps
            if (step + 1) % 100 == 0:
                logger.info(f"Step {step + 1}/{steps}")
                
        # Get final statistics
        results["status"] = "completed"
        results["end_time"] = time.time()
        results["duration"] = results["end_time"] - results["start_time"]
        results["total_spikes"] = context.brain.getTotalSpikeCount()
        results["firing_neurons"] = context.brain.getFiringNeuronCount()
        results["average_firing_rate"] = context.brain.getAverageFiringRate()
        results["final_spikes"] = results["total_spikes"]
        results["final_firing_neurons"] = results["firing_neurons"]
        
    except Exception as e:
        results["status"] = "error"
        results["error"] = str(e)
        raise SimulationError(f"Simulation failed: {e}")
        
    logger.info(f"Simulation completed: {results['duration']:.2f}s")
    return results
def run_batch_simulation(config_files: List[str], max_workers: int = 4) -> List[Dict[str, Any]]:
    """
    Run batch simulation on multiple configuration files.
    
    Args:
        config_files: List of configuration file paths
        max_workers: Maximum number of worker threads
        
    Returns:
        List of batch results
    """
    processor = NLMBatchProcessor(max_workers)
    
    def process_config(config_file: str, **kwargs):
        """Process single configuration."""
        # Create simulation for this config
        context = create_simulation(config_file)
        
        # Run simulation
        results = run_simulation(context, steps=100, timestep=0.001)
        
        # Clean up
        context.agent_brain.clear()
        context.brain.reset()
        context.world.reset()
        
        return results
    
    # Run batch processing
    batch_results = processor.run_batch(config_files, process_config)
    return batch_results
def export_results(results: Dict[str, Any], output_file: str, format: str = "json") -> bool:
    """
    Export results to file.
    
    Args:
        results: Results to export
        output_file: Output file path
        format: Export format
        
    Returns:
        True if successful, False otherwise
    """
    exporter = NLMExporter()
    return exporter.export_simulation_results(results, output_file, format)
def load_config_with_env(filepath: Optional[str] = None, 
                       prefix: str = "NLM_") -> Dict[str, Any]:
    """
    Load configuration from file with environment variable overrides.
    
    Args:
        filepath: Path to configuration file
        prefix: Environment variable prefix
        
    Returns:
        Configuration dictionary
    """
    config_manager = AdvancedConfig()
    
    if filepath:
        config_manager.load_from_file(filepath)
        
    config_manager.load_from_env(prefix)
    
    return config_manager.to_dict()
# Get default template
def get_default_template() -> Dict[str, Any]:
    """Get default configuration template."""
    return {
        "title": "NLM Default Configuration",
        "description": "Default configuration for NLM brain simulation",
        "version": "2.0",
        "config": {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_time_constant": 20.0,
            "development_synaptogenesis_rate": 0.001,
            "development_pruning_rate": 0.0001,
            "dopamine_baseline": 0.1,
            "reward_discount_factor": 0.99,
            "environment_name": "GridWorld",
            "environment_width": 10,
            "environment_height": 10,
            "max_simulation_steps": 10000,
            "visualization_enabled": False,
            "log_level": "INFO",
            "log_to_file": False,
            "log_filename": "nlm.log"
        }
    }
# Save default template
def save_default_template(filepath: str = "configs/default.cfg") -> bool:
    """
    Save default configuration template.
    
    Args:
        filepath: Output file path
        
    Returns:
        True if successful, False otherwise
    """
    try:
        template = get_default_template()
        
        Path(filepath).parent.mkdir(parents=True, exist_ok=True)
        
        with open(filepath, 'w') as f:
            f.write(f"# {template['title']}\n")
            f.write(f"# {template['description']}\n")
            f.write(f"# Version: {template['version']}\n\n")
            
            for key, value in template['config'].items():
                if isinstance(value, bool):
                    f.write(f"{key} = {str(value).lower()}\n")
                else:
                    f.write(f"{key} = {value}\n")
                    
        logger.info(f"Default configuration template saved to {filepath}")
        return True
        
    except Exception as e:
        logger.error(f"Error saving default template: {e}")
        return False
# Main function
if __name__ == "__main__":
    print("Enhanced NLM Python Wrapper")
    print(f"Version: {__version__}")
    print(f"Author: {__author__}")
    print(f"License: {__license__}")
    print("\nAvailable components:")
    print("- AdvancedConfig: Enhanced configuration management")
    print("- NLMContext: Context manager for simulations")
    print("- NLMBatchProcessor: Batch processing")
    print("- NLMValidator: Validation")
    print("- NLMComparison: Comparison tools")
    print("- NLMProfiler: Performance profiling")
    print("- NLMExporter: Export functionality")
    print("- High-level convenience functions")
    
    # Show help
    print("\nUsage examples:")
    print("1. Create simulation: create_simulation('config.cfg')")
    print("2. Run simulation: run_simulation(context, steps=100)")
    print("3. Run batch: run_batch_simulation(['config1.cfg', 'config2.cfg'])")
    print("4. Export results: export_results(results, 'output.json')")
    print("5. Load config with env: load_config_with_env('config.cfg')")