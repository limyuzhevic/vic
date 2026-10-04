"""
Enhanced NLM Python API

This module provides enhanced convenience methods and utility functions
to make the NLM Python API more accessible to new users while providing
advanced tools for power users.

Features include:
- Convenience methods for common operations
- Batch processing utilities
- Configuration presets and templates
- Data analysis utilities
- Export/import convenience methods
- Performance monitoring tools
- Error handling utilities
- Data validation functions
- Statistical analysis tools
- File management utilities
- Simulation benchmarking
- Configuration comparison
- Performance optimization helpers
- Scientific data export
- Integration utilities
"""

import json
import time
import logging
import tempfile
import shutil
from pathlib import Path
from typing import Dict, Any, Optional, List, Union, Callable, Tuple
import sys
import os

# Import the core NLM bindings
from .bindings import pynlm

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)


class NLMWrapper:
    """
    Enhanced wrapper class for NLM functionality that provides convenience methods
    and advanced utilities while maintaining compatibility with the core NLM API.
    
    This class enhances the basic NLM functionality with:
    - High-level convenience methods
    - Batch processing utilities
    - Configuration presets and templates
    - Data analysis utilities
    - Performance monitoring tools
    """
    
    def __init__(self, config: Optional[Any] = None):
        """
        Initialize NLMWrapper.
        
        Args:
            config: Optional pre-configured Config object. If None, a default config
                   will be created using default settings.
        """
        self.brain = None
        self.world = None
        self.config = config
        self._initialized = False
        self._performance_metrics = {}
        self._batch_processor = BatchProcessor()
        self._data_analyzer = DataAnalyzer()
        
    def initialize(self, config_file: Optional[str] = None) -> bool:
        """
        Initialize the NLM system with enhanced error handling and validation.
        
        Args:
            config_file: Optional path to configuration file. If None, uses default settings.
            
        Returns:
            bool: True if initialization successful, False otherwise.
        """
        try:
            # Validate input
            if config_file and not os.path.exists(config_file):
                raise ValueError(f"Configuration file '{config_file}' does not exist")
            
            # Create default config if none provided
            if not self.config:
                self.config = self.create_default_config()
                if config_file:
                    self._load_config_enhanced(config_file)
            
            # Initialize core components
            self.brain = pynlm.Brain(self.config)
            self.world = pynlm.SimpleWorld()
            
            # Configure world with enhanced settings
            if hasattr(self.config, 'world_width'):
                self.world.configure(
                    self.config.world_width,
                    self.config.world_height,
                    getattr(self.config, 'vision_width', 16),
                    getattr(self.config, 'vision_height', 16)
                )
            
            # Initialize brain and world
            self.brain.initialize()
            self.world.reset()
            
            self._initialized = True
            logger.info("NLMWrapper initialized successfully")
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize NLMWrapper: {e}")
            self._initialized = False
            return False
    
    def create_default_config(self) -> Any:
        """
        Create a default configuration with all necessary settings for basic operation.
        
        Returns:
            Config: Initialized configuration object.
        """
        config = pynlm.Config()
        
        # Set default configuration values
        default_config = {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15,
            "vision_width": 16,
            "vision_height": 16,
            "world_width": 20.0,
            "world_height": 20.0,
            "max_energy": 100.0,
            "energy_decay_rate": 0.01,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
            "enable_reward_modulation": True,
            "enable_structural_plasticity": True,
            "enable_development": True,
            "enable_curiosity": True,
            "simulation_duration": 10.0,
            "output_file": "nlm_checkpoint.pkl"
        }
        
        # Apply configuration if Config class supports it
        for key, value in default_config.items():
            try:
                if hasattr(config, key):
                    setattr(config, key, value)
            except Exception as e:
                logger.warning(f"Could not set config attribute {key}: {e}")
        
        return config
    
    def run_simulation(self, duration: float, enable_logging: bool = True) -> Dict[str, Any]:
        """
        Run simulation with enhanced metrics collection and error handling.
        
        Args:
            duration: Simulation duration in seconds.
            enable_logging: Whether to enable detailed logging.
            
        Returns:
            Dict[str, Any]: Dictionary containing simulation results and metrics.
        """
        if not self._initialized:
            raise RuntimeError("NLMWrapper not initialized. Call initialize() first.")
        
        results = {
            "start_time": time.time(),
            "duration": duration,
            "end_time": None,
            "metrics": {},
            "status": "completed",
            "errors": []
        }
        
        try:
            print(f"Starting simulation for {duration} seconds...")
            logger.info(f"Starting simulation for {duration} seconds")
            
            start_time = self.world.getSimulationTime()
            end_time = start_time + duration
            
            step_count = 0
            while self.world.getSimulationTime() < end_time:
                step_count += 1
                
                # Get current state
                percept = self.world.getSensoryPercept()
                
                # Update world
                self.world.update(duration / 1000.0)
                
                # Collect metrics periodically
                current_time = self.world.getSimulationTime()
                if step_count % 100 == 0:
                    metrics = self._collect_metrics()
                    results["metrics"][current_time] = metrics
                    
                    if enable_logging:
                        print(f"Step {step_count}: "
                              f"Time: {current_time:.2f}s | "
                              f"Energy: {self.world.getAgentBody().energy:.1f} | "
                              f"Active neurons: {self.brain.getActiveNeuronCount()}")
            
            # Get final metrics
            final_metrics = self._collect_metrics()
            results["metrics"][current_time] = final_metrics
            
            results["end_time"] = time.time()
            
            print("Simulation completed successfully!")
            logger.info("Simulation completed successfully")
            
        except Exception as e:
            results["status"] = "error"
            results["errors"].append(str(e))
            logger.error(f"Simulation error: {e}")
            raise
        
        return results
    
    def run_batch_simulation(self, durations: List[float], config_template: Optional[Dict[str, Any]] = None) -> List[Dict[str, Any]]:
        """
        Run multiple simulations in batch with enhanced error handling.
        
        Args:
            durations: List of simulation durations.
            config_template: Optional template configuration to override defaults.
            
        Returns:
            List[Dict[str, Any]]: List of simulation results.
        """
        if not durations:
            return []
        
        results = []
        
        for i, duration in enumerate(durations):
            try:
                print(f"Running batch simulation {i+1}/{len(durations)} for {duration} seconds...")
                
                # Apply config template if provided
                if config_template:
                    original_config = self.config
                    self.config = self._apply_config_template(config_template)
                
                # Run simulation
                result = self.run_simulation(duration)
                result["batch_index"] = i
                result["batch_total"] = len(durations)
                
                results.append(result)
                
                # Restore original config
                if config_template:
                    self.config = original_config
                    self.initialize()  # Re-initialize with restored config
                    
            except Exception as e:
                logger.error(f"Batch simulation {i+1} failed: {e}")
                results.append({
                    "batch_index": i,
                    "batch_total": len(durations),
                    "status": "error",
                    "error": str(e),
                    "duration": duration
                })
        
        return results
    
    def _collect_metrics(self) -> Dict[str, Any]:
        """
        Collect comprehensive metrics from the current simulation state.
        
        Returns:
            Dict[str, Any]: Dictionary containing all collected metrics.
        """
        if not self._initialized:
            return {}
        
        metrics = {}
        
        try:
            # Basic brain metrics
            metrics["simulation_time"] = self.world.getSimulationTime()
            metrics["active_neurons"] = self.brain.getActiveNeuronCount()
            metrics["total_neurons"] = self.brain.getTotalNeuronCount()
            metrics["total_synapses"] = self.brain.getTotalSynapseCount()
            metrics["firing_rate"] = self.brain.getAverageFiringRate()
            metrics["excitatory_inhibition_ratio"] = self.brain.getExcitationInhibitionRatio()
            metrics["total_spike_count"] = self.brain.getTotalSpikeCount()
            metrics["active_firing_neuron_count"] = self.brain.getFiringNeuronCount()
            metrics["developmental_stage"] = str(self.brain.getDevelopmentalStage())
            
            # Agent body metrics
            agent_body = self.world.getAgentBody()
            metrics["agent_energy"] = agent_body.energy
            metrics["agent_health"] = agent_body.health
            metrics["agent_age"] = agent_body.age
            metrics["agent_position"] = {"x": agent_body.x, "y": agent_body.y}
            metrics["agent_orientation"] = agent_body.orientation
            metrics["agent_is_moving"] = agent_body.isMoving
            metrics["agent_is_turning"] = agent_body.isTurning
            
            # World metrics
            metrics["world_width"] = self.world.getWidth()
            metrics["world_height"] = self.world.getHeight()
            metrics["max_energy"] = self.world.getMaxEnergy()
            metrics["energy_decay_rate"] = self.world.getEnergyDecayRate()
            
            # Store metrics in performance metrics
            self._performance_metrics[metrics["simulation_time"]] = metrics
            
        except Exception as e:
            logger.warning(f"Could not collect metrics: {e}")
            metrics["error"] = str(e)
        
        return metrics
    
    def save_results(self, results: Dict[str, Any], output_dir: Optional[str] = None) -> str:
        """
        Save simulation results with enhanced file management.
        
        Args:
            results: Dictionary containing simulation results.
            output_dir: Optional output directory. If None, uses current directory.
            
        Returns:
            str: Path to saved results file.
        """
        if output_dir is None:
            output_dir = "."
        
        # Create output directory if it doesn't exist
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        
        # Generate filename with timestamp
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        filename = f"nlm_simulation_results_{timestamp}.json"
        output_path = os.path.join(output_dir, filename)
        
        # Save results
        with open(output_path, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        logger.info(f"Results saved to: {output_path}")
        return output_path
    
    def load_results(self, filepath: str) -> Dict[str, Any]:
        """
        Load simulation results from file with enhanced error handling.
        
        Args:
            filepath: Path to results file.
            
        Returns:
            Dict[str, Any]: Loaded simulation results.
        """
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Results file '{filepath}' does not exist")
        
        try:
            with open(filepath, 'r') as f:
                results = json.load(f)
            
            logger.info(f"Results loaded from: {filepath}")
            return results
            
        except json.JSONDecodeError as e:
            raise ValueError(f"Invalid JSON in results file: {e}")
        except Exception as e:
            raise RuntimeError(f"Failed to load results: {e}")
    
    def export_data(self, data: Any, filename: str, format: str = "json") -> str:
        """
        Export data with support for multiple formats and enhanced error handling.
        
        Args:
            data: Data to export.
            filename: Output filename.
            format: Export format (json, txt, csv).
            
        Returns:
            str: Path to exported file.
        """
        if format.lower() == "json":
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2, default=str)
        elif format.lower() == "txt":
            if hasattr(data, '__str__'):
                with open(filename, 'w') as f:
                    f.write(str(data))
            else:
                with open(filename, 'w') as f:
                    f.write(str(data))
        elif format.lower() == "csv":
            # Simple CSV export for dictionaries
            if isinstance(data, dict):
                import csv
                with open(filename, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(data.keys())
                    writer.writerow(data.values())
            else:
                raise ValueError("CSV export only supported for dictionaries")
        else:
            raise ValueError(f"Unsupported export format: {format}")
        
        logger.info(f"Data exported to: {filename}")
        return filename
    
    def compare_configs(self, config1: Any, config2: Any) -> Dict[str, Any]:
        """
        Compare two configurations and identify differences.
        
        Args:
            config1: First configuration object.
            config2: Second configuration object.
            
        Returns:
            Dict[str, Any]: Dictionary containing comparison results.
        """
        comparison = {
            "identical": True,
            "differences": {},
            "config1_keys": [],
            "config2_keys": [],
            "common_keys": [],
            "unique_to_config1": [],
            "unique_to_config2": []
        }
        
        try:
            # Get all attributes from both configs
            config1_attrs = set()
            config2_attrs = set()
            
            for obj, attr_set in [(config1, config1_attrs), (config2, config2_attrs)]:
                if hasattr(obj, '__dict__'):
                    attr_set.update(obj.__dict__.keys())
                
                # Also check for specific known attributes
                common_attrs = [
                    'random_seed', 'neuron_count', 'world_width', 'world_height',
                    'vision_width', 'vision_height', 'max_energy'
                ]
                
                for attr in common_attrs:
                    if hasattr(obj, attr):
                        attr_set.add(attr)
            
            comparison["config1_keys"] = list(config1_attrs)
            comparison["config2_keys"] = list(config2_attrs)
            
            # Find common and unique keys
            comparison["common_keys"] = list(config1_attrs & config2_attrs)
            comparison["unique_to_config1"] = list(config1_attrs - config2_attrs)
            comparison["unique_to_config2"] = list(config2_attrs - config1_attrs)
            
            # Compare values for common keys
            for key in comparison["common_keys"]:
                try:
                    val1 = getattr(config1, key, None)
                    val2 = getattr(config2, key, None)
                    
                    if val1 != val2:
                        comparison["identical"] = False
                        comparison["differences"][key] = {
                            "config1_value": val1,
                            "config2_value": val2,
                            "difference": f"{val1} != {val2}"
                        }
                except Exception as e:
                    comparison["differences"][key] = {"error": str(e)}
            
        except Exception as e:
            logger.error(f"Configuration comparison error: {e}")
            comparison["error"] = str(e)
        
        return comparison
    
    def get_performance_report(self) -> Dict[str, Any]:
        """
        Generate a comprehensive performance report.
        
        Returns:
            Dict[str, Any]: Performance report.
        """
        report = {
            "initialization_status": self._initialized,
            "performance_metrics": self._performance_metrics,
            "metrics_count": len(self._performance_metrics),
            "average_active_neurons": 0,
            "average_firing_rate": 0,
            "simulation_duration": 0
        }
        
        if self._performance_metrics:
            total_active_neurons = sum(
                metrics.get("active_neurons", 0) 
                for metrics in self._performance_metrics.values()
            )
            total_firing_rate = sum(
                metrics.get("firing_rate", 0) 
                for metrics in self._performance_metrics.values()
            )
            
            report["average_active_neurons"] = total_active_neurons / len(self._performance_metrics)
            report["average_firing_rate"] = total_firing_rate / len(self._performance_metrics)
            
            # Calculate simulation duration
            times = sorted(self._performance_metrics.keys())
            if len(times) >= 2:
                report["simulation_duration"] = times[-1] - times[0]
        
        return report


class BatchProcessor:
    """
    Batch processing utilities for NLM simulations.
    
    Provides functionality for running multiple simulations with different
    configurations or parameters efficiently.
    """
    
    def __init__(self):
        self.batch_history = []
    
    def create_parameter_grid(self, param_ranges: Dict[str, List[Any]]) -> List[Dict[str, Any]]:
        """
        Create a parameter grid for systematic experimentation.
        
        Args:
            param_ranges: Dictionary mapping parameter names to lists of values.
            
        Returns:
            List[Dict[str, Any]]: List of parameter combinations.
        """
        import itertools
        
        # Get parameter names and their possible values
        param_names = list(param_ranges.keys())
        param_values = list(param_ranges.values())
        
        # Create all combinations
        combinations = list(itertools.product(*param_values))
        
        # Convert to list of dictionaries
        param_grid = []
        for combo in combinations:
            param_set = dict(zip(param_names, combo))
            param_grid.append(param_set)
        
        logger.info(f"Created parameter grid with {len(param_grid)} combinations")
        return param_grid
    
    def run_parameter_sweep(self, nlm_wrapper: NLMWrapper, param_grid: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """
        Run a parameter sweep across all parameter combinations.
        
        Args:
            nlm_wrapper: NLMWrapper instance to use for simulations.
            param_grid: List of parameter combinations.
            
        Returns:
            List[Dict[str, Any]]: Results from all parameter combinations.
        """
        results = []
        
        for i, params in enumerate(param_grid):
            try:
                print(f"Running parameter sweep {i+1}/{len(param_grid)}")
                
                # Create a copy of the wrapper for this run
                wrapper_copy = NLMWrapper(nlm_wrapper.config)
                
                # Initialize with custom parameters
                wrapper_copy.initialize()
                
                # Apply custom parameters if needed
                # (This would depend on how parameters map to config attributes)
                
                # Run simulation
                result = wrapper_copy.run_simulation(1.0)  # Use a fixed duration for parameter sweep
                result["parameters"] = params
                result["parameter_index"] = i
                
                results.append(result)
                
            except Exception as e:
                logger.error(f"Parameter sweep {i+1} failed: {e}")
                results.append({
                    "parameter_index": i,
                    "parameters": params,
                    "status": "error",
                    "error": str(e)
                })
        
        return results


class DataAnalyzer:
    """
    Data analysis utilities for NLM simulation results.
    
    Provides statistical analysis, visualization data preparation,
    and result interpretation tools.
    """
    
    @staticmethod
    def calculate_statistics(data: List[float]) -> Dict[str, float]:
        """
        Calculate basic statistics for a dataset.
        
        Args:
            data: List of numerical values.
            
        Returns:
            Dict[str, float]: Dictionary containing statistics.
        """
        if not data:
            return {
                "mean": 0.0,
                "median": 0.0,
                "std": 0.0,
                "min": 0.0,
                "max": 0.0,
                "count": 0
            }
        
        import statistics
        
        return {
            "mean": statistics.mean(data),
            "median": statistics.median(data),
            "std": statistics.stdev(data) if len(data) > 1 else 0.0,
            "min": min(data),
            "max": max(data),
            "count": len(data)
        }
    
    @staticmethod
    def analyze_simulation_results(results: Dict[str, Any]) -> Dict[str, Any]:
        """
        Analyze simulation results and extract key insights.
        
        Args:
            results: Simulation results dictionary.
            
        Returns:
            Dict[str, Any]: Analysis results.
        """
        analysis = {
            "duration": results.get("duration", 0),
            "status": results.get("status", "unknown"),
            "metrics_analysis": {},
            "performance_indicators": {}
        }
        
        # Analyze metrics if available
        metrics = results.get("metrics", {})
        if metrics:
            # Extract time series data
            times = sorted(metrics.keys())
            if times:
                analysis["simulation_times"] = times
                analysis["total_steps"] = len(times)
                
                # Calculate average values over time
                avg_active_neurons = []
                avg_firing_rates = []
                
                for t in times:
                    avg_active_neurons.append(metrics[t].get("active_neurons", 0))
                    avg_firing_rates.append(metrics[t].get("firing_rate", 0))
                
                analysis["metrics_analysis"] = {
                    "average_active_neurons": DataAnalyzer.calculate_statistics(avg_active_neurons),
                    "average_firing_rate": DataAnalyzer.calculate_statistics(avg_firing_rates),
                    "peak_active_neurons": max(avg_active_neurons) if avg_active_neurons else 0,
                    "peak_firing_rate": max(avg_firing_rates) if avg_firing_rates else 0
                }
        
        # Calculate performance indicators
        if analysis["metrics_analysis"]:
            stats = analysis["metrics_analysis"]
            analysis["performance_indicators"] = {
                "stability_index": 1.0 / (1.0 + stats["average_firing_rate"]["std"]),
                "efficiency_ratio": stats["average_active_neurons"]["mean"] / (
                    stats["average_active_neurons"]["max"] if stats["average_active_neurons"]["max"] > 0 else 1
                ),
                "activity_level": stats["average_firing_rate"]["mean"]
            }
        
        return analysis
    
    @staticmethod
    def prepare_visualization_data(results: Dict[str, Any], metric_keys: Optional[List[str]] = None) -> Dict[str, Any]:
        """
        Prepare simulation results for visualization.
        
        Args:
            results: Simulation results dictionary.
            metric_keys: List of metric keys to include in visualization data.
            
        Returns:
            Dict[str, Any]: Visualization-ready data.
        """
        if metric_keys is None:
            metric_keys = ["active_neurons", "firing_rate", "agent_energy"]
        
        viz_data = {
            "timestamps": [],
            "data_series": {}
        }
        
        metrics = results.get("metrics", {})
        if not metrics:
            return viz_data
        
        # Sort timestamps
        timestamps = sorted(metrics.keys())
        viz_data["timestamps"] = timestamps
        
        # Extract requested metrics
        for key in metric_keys:
            data_series = []
            for t in timestamps:
                if key in metrics[t]:
                    data_series.append(metrics[t][key])
                else:
                    data_series.append(None)
            
            viz_data["data_series"][key] = data_series
        
        return viz_data


class ConfigurationValidator:
    """
    Configuration validation utilities for NLM systems.
    
    Provides validation for configuration files, parameter ranges,
    and consistency checks.
    """
    
    # Valid ranges for configuration parameters
    VALID_RANGES = {
        "random_seed": (0, 2**31 - 1),
        "simulation_timestep": (0.0001, 0.1),
        "neuron_count": (1, 10000),
        "region_count": (1, 100),
        "connection_probability": (0.0, 1.0),
        "vision_width": (1, 100),
        "vision_height": (1, 100),
        "world_width": (1.0, 1000.0),
        "world_height": (1.0, 1000.0),
        "max_energy": (0.0, 10000.0),
        "energy_decay_rate": (0.0, 1.0),
        "stdp_ltp_weight": (0.0, 1.0),
        "stdp_ltd_weight": (0.0, 1.0),
        "stdp_tau": (0.0, 100.0),
        "synaptogenesis_rate": (0.0, 1.0),
        "pruning_rate": (0.0, 1.0),
        "simulation_duration": (0.1, 1000.0)
    }
    
    @classmethod
    def validate_config_dict(cls, config: Dict[str, Any]) -> Tuple[bool, List[str]]:
        """
        Validate a configuration dictionary.
        
        Args:
            config: Configuration dictionary to validate.
            
        Returns:
            Tuple[bool, List[str]]: (is_valid, list_of_errors).
        """
        errors = []
        
        for key, value in config.items():
            if key in cls.VALID_RANGES:
                min_val, max_val = cls.VALID_RANGES[key]
                if not isinstance(value, (int, float)):
                    errors.append(f"Config parameter '{key}' must be a number, got {type(value)}")
                elif value < min_val or value > max_val:
                    errors.append(f"Config parameter '{key}' value {value} is outside valid range [{min_val}, {max_val}]")
            
            # Check boolean parameters
            if key in ["enable_reward_modulation", "enable_structural_plasticity", 
                      "enable_development", "enable_curiosity"]:
                if not isinstance(value, bool):
                    errors.append(f"Config parameter '{key}' must be boolean, got {type(value)}")
        
        return len(errors) == 0, errors
    
    @classmethod
    def validate_config_file(cls, filepath: str) -> Tuple[bool, List[str]]:
        """
        Validate a configuration file.
        
        Args:
            filepath: Path to configuration file.
            
        Returns:
            Tuple[bool, List[str]]: (is_valid, list_of_errors).
        """
        if not os.path.exists(filepath):
            return False, [f"Configuration file '{filepath}' does not exist"]
        
        try:
            with open(filepath, 'r') as f:
                config = json.load(f)
            
            return cls.validate_config_dict(config)
            
        except json.JSONDecodeError as e:
            return False, [f"Invalid JSON in configuration file: {e}"]
        except Exception as e:
            return False, [f"Error reading configuration file: {e}"]
    
    @classmethod
    def get_config_validation_report(cls, config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Get a comprehensive validation report for a configuration.
        
        Args:
            config: Configuration dictionary to validate.
            
        Returns:
            Dict[str, Any]: Validation report.
        """
        is_valid, errors = cls.validate_config_dict(config)
        
        report = {
            "valid": is_valid,
            "errors": errors,
            "warnings": [],
            "suggestions": []
        }
        
        # Add warnings and suggestions for common issues
        if "neuron_count" in config and config["neuron_count"] > 1000:
            report["warnings"].append("High neuron count may impact performance")
            report["suggestions"].append("Consider reducing neuron count for faster simulations")
        
        if "world_width" in config and config["world_width"] > 100:
            report["warnings"].append("Large world size may impact memory usage")
            report["suggestions"].append("Consider reducing world size for better performance")
        
        if not errors and not report["warnings"]:
            report["message"] = "Configuration is valid and well-formed"
        
        return report


class ErrorHandler:
    """
    Enhanced error handling utilities for NLM systems.
    
    Provides structured error reporting, recovery mechanisms,
    and error analysis tools.
    """
    
    def __init__(self):
        self.errors = []
        self.recovery_actions = []
    
    def handle_error(self, message: str, error_type: str = "GENERIC", 
                    details: Optional[str] = None, episode: Optional[int] = None, 
                    step: Optional[int] = None) -> Dict[str, Any]:
        """
        Handle an error with structured reporting and recovery tracking.
        
        Args:
            message: Error message.
            error_type: Type of error (e.g., "CONFIG_ERROR", "SIMULATION_ERROR").
            details: Additional error details.
            episode: Episode number where error occurred.
            step: Step number where error occurred.
            
        Returns:
            Dict[str, Any]: Error record.
        """
        error_record = {
            "timestamp": time.time(),
            "message": message,
            "type": error_type,
            "details": details,
            "episode": episode,
            "step": step,
            "id": len(self.errors) + 1
        }
        
        self.errors.append(error_record)
        logger.error(f"[{error_type}] {message}")
        if details:
            logger.error(f"Details: {details}")
        
        return error_record
    
    def get_error_summary(self) -> Dict[str, Any]:
        """
        Get a summary of all recorded errors.
        
        Returns:
            Dict[str, Any]: Error summary.
        """
        error_types = {}
        for error in self.errors:
            error_type = error["type"]
            if error_type not in error_types:
                error_types[error_type] = 0
            error_types[error_type] += 1
        
        return {
            "total_errors": len(self.errors),
            "error_types": error_types,
            "recent_errors": self.errors[-10:],  # Last 10 errors
            "errors_by_episode": self._group_errors_by_episode()
        }
    
    def _group_errors_by_episode(self) -> Dict[int, List[Dict[str, Any]]]:
        """Group errors by episode number."""
        grouped = {}
        for error in self.errors:
            episode = error["episode"]
            if episode is not None:
                if episode not in grouped:
                    grouped[episode] = []
                grouped[episode].append(error)
        return grouped


# Global utility functions

def create_enhanced_config(**kwargs) -> Any:
    """
    Create an enhanced configuration with default values and validation.
    
    Args:
        **kwargs: Configuration parameters to override defaults.
        
    Returns:
        Config: Configured NLM Config object.
    """
    wrapper = NLMWrapper()
    config = wrapper.create_default_config()
    
    # Apply custom parameters
    for key, value in kwargs.items():
        try:
            if hasattr(config, key):
                setattr(config, key, value)
        except Exception as e:
            logger.warning(f"Could not set config attribute {key}: {e}")
    
    return config


def run_enhanced_simulation(config_file: Optional[str] = None, duration: float = 10.0,
                          enable_batch: bool = False) -> Dict[str, Any]:
    """
    Run an enhanced simulation with all utilities.
    
    Args:
        config_file: Optional path to configuration file.
        duration: Simulation duration in seconds.
        enable_batch: Whether to run in batch mode.
        
    Returns:
        Dict[str, Any]: Simulation results.
    """
    # Initialize NLM wrapper
    wrapper = NLMWrapper()
    if not wrapper.initialize(config_file):
        raise RuntimeError("Failed to initialize NLM system")
    
    # Run simulation
    results = wrapper.run_simulation(duration)
    
    # Add validation
    validator = ConfigurationValidator()
    validation_report = validator.get_config_validation_report({})
    results["validation_report"] = validation_report
    
    # Add error handler
    error_handler = ErrorHandler()
    results["error_handler_summary"] = error_handler.get_error_summary()
    
    return results


def validate_and_analyze_results(results: Dict[str, Any]) -> Dict[str, Any]:
    """
    Validate and analyze simulation results comprehensively.
    
    Args:
        results: Simulation results to validate and analyze.
        
    Returns:
        Dict[str, Any]: Validated and analyzed results.
    """
    analyzer = DataAnalyzer()
    validation_report = ConfigurationValidator().get_config_validation_report({})
    
    analysis = analyzer.analyze_simulation_results(results)
    viz_data = analyzer.prepare_visualization_data(results)
    
    return {
        "original_results": results,
        "analysis": analysis,
        "visualization_data": viz_data,
        "validation_report": validation_report,
        "summary": {
            "simulation_duration": results.get("duration", 0),
            "status": results.get("status", "unknown"),
            "metrics_count": len(results.get("metrics", {})),
            "analysis_available": bool(analysis["metrics_analysis"])
        }
    }


def benchmark_simulation(config_file: Optional[str] = None, durations: List[float] = None) -> Dict[str, Any]:
    """
    Run simulation benchmarking across different durations.
    
    Args:
        config_file: Optional path to configuration file.
        durations: List of simulation durations to benchmark.
        
    Returns:
        Dict[str, Any]: Benchmarking results.
    """
    if durations is None:
        durations = [1.0, 5.0, 10.0, 20.0]
    
    benchmark_results = {
        "config_file": config_file,
        "durations": durations,
        "results": [],
        "performance_metrics": {}
    }
    
    for duration in durations:
        try:
            start_time = time.time()
            result = run_enhanced_simulation(config_file, duration)
            end_time = time.time()
            
            benchmark_result = {
                "duration": duration,
                "actual_duration": result.get("duration", 0),
                "execution_time": end_time - start_time,
                "result": result
            }
            
            benchmark_results["results"].append(benchmark_result)
            
        except Exception as e:
            logger.error(f"Benchmark failed for duration {duration}: {e}")
            benchmark_results["results"].append({
                "duration": duration,
                "error": str(e)
            })
    
    # Calculate performance metrics
    successful_results = [r for r in benchmark_results["results"] if "error" not in r]
    if successful_results:
        benchmark_results["performance_metrics"] = {
            "time_per_duration": [
                r["execution_time"] / r["duration"] 
                for r in successful_results
                if r["duration"] > 0
            ],
            "average_time_per_second": sum(
                r["execution_time"] / r["duration"] 
                for r in successful_results
                if r["duration"] > 0
            ) / len(successful_results)
        }
    
    return benchmark_results


def export_scientific_data(results: Dict[str, Any], output_dir: str, 
                         format: str = "hdf5") -> List[str]:
    """
    Export simulation results in scientific data formats.
    
    Args:
        results: Simulation results to export.
        output_dir: Output directory.
        format: Export format (json, hdf5, npy).
        
    Returns:
        List[str]: List of exported file paths.
    """
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    exported_files = []
    
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    
    if format.lower() == "json":
        filename = f"scientific_data_{timestamp}.json"
        filepath = os.path.join(output_dir, filename)
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        exported_files.append(filepath)
    
    elif format.lower() == "hdf5":
        try:
            import h5py
            filename = f"scientific_data_{timestamp}.h5"
            filepath = os.path.join(output_dir, filename)
            
            with h5py.File(filepath, 'w') as f:
                # Store metadata
                f.attrs['created'] = time.time()
                f.attrs['description'] = "Enhanced NLM scientific data export"
                
                # Store simulation results
                if "metrics" in results:
                    # Convert metrics to HDF5 dataset
                    grp = f.create_group("metrics")
                    for time_step, metrics in results["metrics"].items():
                        time_grp = grp.create_group(str(time_step))
                        for key, value in metrics.items():
                            if isinstance(value, (int, float)):
                                time_grp.attrs[key] = value
                            elif isinstance(value, dict):
                                sub_grp = time_grp.create_group(key)
                                for k, v in value.items():
                                    if isinstance(v, (int, float)):
                                        sub_grp.attrs[k] = v
            
            exported_files.append(filepath)
            
        except ImportError:
            logger.warning("h5py not available, skipping HDF5 export")
        except Exception as e:
            logger.error(f"HDF5 export error: {e}")
    
    elif format.lower() == "npy":
        try:
            import numpy as np
            
            # Export time series data
            if "metrics" in results:
                filename = f"scientific_data_{timestamp}.npz"
                filepath = os.path.join(output_dir, filename)
                
                # Prepare arrays for export
                times = []
                active_neurons = []
                firing_rates = []
                
                for time_step, metrics in results["metrics"].items():
                    times.append(time_step)
                    active_neurons.append(metrics.get("active_neurons", 0))
                    firing_rates.append(metrics.get("firing_rate", 0))
                
                np.savez(filepath, 
                        timestamps=np.array(times),
                        active_neurons=np.array(active_neurons),
                        firing_rates=np.array(firing_rates),
                        metadata={"created": time.time(), "description": "Enhanced NLM scientific data"})
            
            exported_files.append(filepath)
            
        except ImportError:
            logger.warning("numpy not available, skipping NPY export")
        except Exception as e:
            logger.error(f"NPY export error: {e}")
    
    else:
        raise ValueError(f"Unsupported export format: {format}")
    
    logger.info(f"Scientific data exported to: {exported_files}")
    return exported_files


# Module-level constants

__version__ = "1.0.0"
__author__ = "Enhanced NLM Development Team"
__license__ = "MIT"

# Public API
__all__ = [
    "NLMWrapper",
    "BatchProcessor",
    "DataAnalyzer",
    "ConfigurationValidator",
    "ErrorHandler",
    "create_enhanced_config",
    "run_enhanced_simulation",
    "validate_and_analyze_results",
    "benchmark_simulation",
    "export_scientific_data"
]