#!/usr/bin/env python3
"""
NLM Advanced Utilities
======================

Advanced utilities for NLM (Neural Learning Machine) research and development.

This module provides specialized tools for:
- Configuration management and validation
- Statistical analysis of neural data
- Performance benchmarking
- Visualization data preparation
- Experiment tracking and reproducibility
- Custom neural network architectures
- Advanced plasticity rule configurations

These utilities are designed for researchers and advanced users who need
to push the boundaries of neural network simulation.
"""

import json
import statistics
import math
from typing import Dict, List, Any, Optional, Union
from pathlib import Path
import pynlm

class NLMConfigManager:
    """
    Advanced configuration manager for NLM systems.
    
    Provides sophisticated configuration management with validation,
    template support, and experiment tracking.
    """
    
    def __init__(self):
        self.configs = {}
        self.config_templates = {}
        self.experiment_history = []
        
    def load_config_from_file(self, filepath: str) -> Dict[str, Any]:
        """
        Load configuration from JSON file with validation.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            Dictionary containing configuration parameters
            
        Raises:
            FileNotFoundError: If file doesn't exist
            ValueError: If configuration is invalid
        """
        filepath = Path(filepath)
        if not filepath.exists():
            raise FileNotFoundError(f"Configuration file not found: {filepath}")
            
        with open(filepath, 'r') as f:
            config_data = json.load(f)
            
        # Validate configuration
        validated_config = self._validate_config(config_data)
        
        # Store for experiment tracking
        self.configs[filepath.stem] = validated_config
        
        return validated_config
        
    def create_research_config(self, 
                               neuron_count: int = 2000,
                               network_type: str = "dense",
                               plasticity_mode: str = "stdp+hebbian",
                               neuromodulation_enabled: bool = True) -> Dict[str, Any]:
        """
        Create a research-grade configuration with sensible defaults.
        
        Args:
            neuron_count: Number of neurons in the network
            network_type: Type of network architecture
            plasticity_mode: Plasticity rule configuration
            neuromodulation_enabled: Whether neuromodulation is enabled
            
        Returns:
            Research configuration dictionary
        """
        # Base configuration
        config = {
            "brain": {
                "neuron_count": neuron_count,
                "region_count": 2,
                "connection_probability": 0.05,
                "initial_weight_mean": 0.5,
                "initial_weight_std": 0.1,
                "v_thresh": -50.0,
                "v_rest": -70.0,
                "v_reset": -75.0,
                "tau_mem": 20.0,
                "tau_ref": 2.0
            },
            "plasticity": {
                "stdp": {
                    "enable": True,
                    "learning_rate": 0.01,
                    "tau_plus": 20.0,
                    "tau_minus": 20.0,
                    "weight_range": [-1.0, 1.0]
                },
                "hebbian": {
                    "enable": True,
                    "learning_rate": 0.005,
                    "max_weight": 2.0
                },
                "structural": {
                    "enable": True,
                    "synaptogenesis_rate": 0.0001,
                    "pruning_rate": 0.00001
                }
            },
            "neuromodulation": {
                "dopamine": {
                    "scale": 1.0,
                    "plasticity_factor": 1.0
                },
                "curiosity": {
                    "enable": neuromodulation_enabled,
                    "sensitivity": 1.0
                },
                "novelty": {
                    "enable": neuromodulation_enabled,
                    "threshold": 0.1
                }
            },
            "development": {
                "enable": True,
                "critical_period_duration": 300.0,
                "maturation_threshold": 0.8,
                "plasticity_decay_rate": 0.001
            },
            "memory": {
                "working": {
                    "capacity": neuron_count // 10,
                    "decay_rate": 0.95
                },
                "episodic": {
                    "max_episodes": 1000,
                    "replay_interval": 100,
                    "consolidation_threshold": 0.3
                }
            }
        }
        
        # Apply network-specific configurations
        if network_type == "sparse":
            config["brain"]["connection_probability"] *= 0.5
        elif network_type == "dense":
            config["brain"]["connection_probability"] *= 1.5
        elif network_type == "modular":
            config["brain"]["region_count"] = 4
            
        # Apply plasticity mode
        if plasticity_mode == "stdp_only":
            config["plasticity"]["hebbian"]["enable"] = False
        elif plasticity_mode == "hebbian_only":
            config["plasticity"]["stdp"]["enable"] = False
        elif plasticity_mode == "reward_modulated":
            config["neuromodulation"]["dopamine"]["scale"] *= 2.0
            
        # Store configuration
        self.configs[f"research_{neuron_count}"] = config
        
        return config
        
    def _validate_config(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration parameters."""
        # Basic validation
        if "brain" not in config:
            raise ValueError("Configuration must contain 'brain' section")
            
        brain_config = config["brain"]
        if "neuron_count" not in brain_config:
            raise ValueError("Brain configuration must specify 'neuron_count'")
            
        # Validate ranges
        if brain_config["neuron_count"] <= 0:
            raise ValueError("Neuron count must be positive")
            
        if not 0 <= brain_config.get("connection_probability", 0.1) <= 1:
            raise ValueError("Connection probability must be between 0 and 1")
            
        return config
        
    def save_config_to_file(self, config: Dict[str, Any], filepath: str):
        """Save configuration to JSON file."""
        with open(filepath, 'w') as f:
            json.dump(config, f, indent=2)
            
    def compare_configs(self, config1: Dict[str, Any], config2: Dict[str, Any]) -> Dict[str, Any]:
        """Compare two configurations and show differences."""
        differences = {
            "added_keys": [],
            "removed_keys": [],
            "modified_keys": [],
            "equal_keys": []
        }
        
        all_keys = set(config1.keys()) | set(config2.keys())
        
        for key in all_keys:
            if key not in config1:
                differences["added_keys"].append(key)
            elif key not in config2:
                differences["removed_keys"].append(key)
            elif config1[key] != config2[key]:
                differences["modified_keys"].append({
                    "key": key,
                    "value1": config1[key],
                    "value2": config2[key]
                })
            else:
                differences["equal_keys"].append(key)
                
        return differences
class NLMDataAnalyzer:
    """
    Advanced data analysis utilities for NLM experiments.
    
    Provides statistical analysis, visualization preparation,
    and performance metrics calculation for neural simulation data.
    """
    
    def __init__(self):
        self.analysis_history = []
        
    def analyze_spike_dynamics(self, spike_counts: List[int]) -> Dict[str, Any]:
        """
        Analyze spike train dynamics.
        
        Args:
            spike_counts: List of spike counts per time step
            
        Returns:
            Dictionary containing spike dynamics analysis
        """
        if not spike_counts:
            return {}
            
        analysis = {
            "total_spikes": sum(spike_counts),
            "mean_spikes": statistics.mean(spike_counts),
            "std_spikes": statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0,
            "max_spikes": max(spike_counts),
            "min_spikes": min(spike_counts),
            "spike_frequency": sum(spike_counts) / len(spike_counts),
            "cv_of_spikes": 0,  # Coefficient of variation
            "high_activity_periods": 0,
            "silent_periods": 0,
            "burst_detection": self._detect_bursts(spike_counts)
        }
        
        if analysis["mean_spikes"] > 0:
            analysis["cv_of_spikes"] = analysis["std_spikes"] / analysis["mean_spikes"]
            
        # Identify high activity periods (above mean + std)
        mean_spikes = analysis["mean_spikes"]
        std_spikes = analysis["std_spikes"]
        threshold = mean_spikes + std_spikes
        analysis["high_activity_periods"] = sum(1 for s in spike_counts if s > threshold)
        analysis["silent_periods"] = sum(1 for s in spike_counts if s == 0)
        
        return analysis
        
    def _detect_bursts(self, spike_counts: List[int], threshold: float = 2.0) -> List[Dict[str, Any]]:
        """Detect spike bursts in the data."""
        if len(spike_counts) < 5:
            return []
            
        mean_spikes = statistics.mean(spike_counts)
        std_spikes = statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0
        burst_threshold = mean_spikes + threshold * std_spikes
        
        bursts = []
        in_burst = False
        burst_start = 0
        
        for i, spikes in enumerate(spike_counts):
            if spikes > burst_threshold and not in_burst:
                in_burst = True
                burst_start = i
            elif spikes <= burst_threshold and in_burst:
                in_burst = False
                if i - burst_start > 2:  # Minimum burst duration
                    bursts.append({
                        "start": burst_start,
                        "end": i - 1,
                        "duration": i - burst_start,
                        "peak_spikes": max(spike_counts[burst_start:i]),
                        "mean_spikes": statistics.mean(spike_counts[burst_start:i])
                    })
        
        if in_burst:
            bursts.append({
                "start": burst_start,
                "end": len(spike_counts) - 1,
                "duration": len(spike_counts) - burst_start,
                "peak_spikes": max(spike_counts[burst_start:]),
                "mean_spikes": statistics.mean(spike_counts[burst_start:])
            })
            
        return bursts
        
    def analyze_neural_activity(self, firing_rates: List[float], 
                               e_i_ratios: List[float]) -> Dict[str, Any]:
        """
        Analyze overall neural activity patterns.
        
        Args:
            firing_rates: List of firing rates per time step
            e_i_ratios: List of excitation/inhibition ratios per time step
            
        Returns:
            Dictionary containing neural activity analysis
        """
        analysis = {}
        
        if firing_rates:
            analysis["firing_rate"] = {
                "mean": statistics.mean(firing_rates),
                "std": statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
                "cv": 0,  # Coefficient of variation
                "min": min(firing_rates),
                "max": max(firing_rates),
                "stability": self._calculate_stability(firing_rates)
            }
            
            if analysis["firing_rate"]["mean"] > 0:
                analysis["firing_rate"]["cv"] = (
                    analysis["firing_rate"]["std"] / 
                    analysis["firing_rate"]["mean"]
                )
                
        if e_i_ratios:
            analysis["e_i_balance"] = {
                "mean": statistics.mean(e_i_ratios),
                "std": statistics.stdev(e_i_ratios) if len(e_i_ratios) > 1 else 0,
                "stability": self._calculate_stability(e_i_ratios),
                "imbalance_periods": sum(1 for ratio in e_i_ratios if abs(ratio - 1.0) > 0.5)
            }
            
        return analysis
        
    def _calculate_stability(self, data: List[float]) -> float:
        """Calculate stability metric (inverse of coefficient of variation)."""
        if not data or len(data) < 2:
            return 1.0
            
        mean_val = statistics.mean(data)
        if mean_val == 0:
            return 0.0
            
        std_val = statistics.stdev(data)
        cv = std_val / mean_val
        
        # Stability ranges from 0 (unstable) to 1 (very stable)
        stability = max(0.0, 1.0 - min(cv, 2.0) / 2.0)
        
        return stability
        
    def calculate_performance_metrics(self, experiment_data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Calculate comprehensive performance metrics.
        
        Args:
            experiment_data: Dictionary containing experiment data
            
        Returns:
            Dictionary containing performance metrics
        """
        metrics = {}
        
        # Time efficiency metrics
        if 'steps' in experiment_data and 'time_taken' in experiment_data:
            steps = experiment_data['steps']
            time_taken = experiment_data['time_taken']
            
            metrics['efficiency'] = {
                'steps_per_second': steps / time_taken,
                'time_per_step': time_taken / steps,
                'scalability_index': steps / time_taken / 1000  # Normalized
            }
            
        # Neural performance metrics
        if 'firing_rates' in experiment_data:
            firing_rates = experiment_data['firing_rates']
            metrics['neural_performance'] = {
                'average_firing_rate': statistics.mean(firing_rates),
                'firing_range': max(firing_rates) - min(firing_rates),
                'stable_firing_periods': sum(1 for rate in firing_rates 
                                           if 0.1 * statistics.mean(firing_rates) < rate < 10 * statistics.mean(firing_rates))
            }
            
        # Learning metrics
        if 'prediction_errors' in experiment_data:
            pred_errors = experiment_data['prediction_errors']
            metrics['learning'] = {
                'final_prediction_error': pred_errors[-1] if pred_errors else 0,
                'error_reduction': (pred_errors[0] - pred_errors[-1]) / pred_errors[0] 
                                 if pred_errors and pred_errors[0] > 0 else 0,
                'error_variance': statistics.variance(pred_errors) if len(pred_errors) > 1 else 0
            }
            
        return metrics
        
    def prepare_visualization_data(self, experiment_data: Dict[str, Any], 
                                  metric_subset: Optional[List[str]] = None) -> Dict[str, Any]:
        """
        Prepare data for visualization.
        
        Args:
            experiment_data: Dictionary containing experiment data
            metric_subset: List of metrics to include (None for all)
            
        Returns:
            Dictionary formatted for visualization
        """
        if metric_subset is None:
            metric_subset = list(experiment_data.keys())
            
        viz_data = {
            'time_series': {},
            'summary_statistics': {},
            'processed_metrics': {}
        }
        
        for metric in metric_subset:
            if metric not in experiment_data:
                continue
                
            values = experiment_data[metric]
            viz_data['time_series'][metric] = values
            
            # Calculate summary statistics
            if len(values) > 0:
                viz_data['summary_statistics'][metric] = {
                    'mean': statistics.mean(values),
                    'std': statistics.stdev(values) if len(values) > 1 else 0,
                    'min': min(values),
                    'max': max(values),
                    'range': max(values) - min(values)
                }
                
                # Add processed metrics (smoothed, normalized, etc.)
                viz_data['processed_metrics'][metric] = {
                    'moving_average': self._moving_average(values, window=5),
                    'normalized': self._normalize(values),
                    'detrended': self._detrend(values)
                }
                
        return viz_data
        
    def _moving_average(self, data: List[float], window: int = 5) -> List[float]:
        """Calculate moving average."""
        if len(data) < window:
            return data
            
        ma = []
        for i in range(len(data) - window + 1):
            ma.append(statistics.mean(data[i:i + window]))
            
        return ma
        
    def _normalize(self, data: List[float]) -> List[float]:
        """Normalize data to [0, 1] range."""
        if not data:
            return []
            
        min_val = min(data)
        max_val = max(data)
        
        if max_val == min_val:
            return [0.5] * len(data)
            
        return [(x - min_val) / (max_val - min_val) for x in data]
        
    def _detrend(self, data: List[float]) -> List[float]:
        """Remove linear trend from data."""
        if len(data) < 2:
            return data
            
        # Calculate linear trend
        n = len(data)
        x = list(range(n))
        y = data
        
        mean_x = statistics.mean(x)
        mean_y = statistics.mean(y)
        
        numerator = sum((xi - mean_x) * (yi - mean_y) for xi, yi in zip(x, y))
        denominator = sum((xi - mean_x) ** 2 for xi in x)
        
        if denominator == 0:
            return data
            
        slope = numerator / denominator
        intercept = mean_y - slope * mean_x
        
        # Remove trend
        detrended = [yi - (slope * xi + intercept) for xi, yi in zip(x, y)]
        
        return detrended
class NLMSimulationProfiler:
    """
    Performance profiler for NLM simulations.
    
    Provides detailed profiling information for optimization analysis
    and performance benchmarking.
    """
    
    def __init__(self):
        self.profiles = {}
        self.performance_benchmarks = {}
        
    def profile_simulation(self, simulation_func, *args, **kwargs) -> Dict[str, Any]:
        """
        Profile a simulation and return performance metrics.
        
        Args:
            simulation_func: Simulation function to profile
            *args, **kwargs: Arguments to pass to simulation function
            
        Returns:
            Dictionary containing performance metrics
        """
        import time
        
        # Time the simulation
        start_time = time.time()
        result = simulation_func(*args, **kwargs)
        end_time = time.time()
        
        # Calculate performance metrics
        execution_time = end_time - start_time
        
        profile = {
            'execution_time': execution_time,
            'timestamp': start_time,
            'result': result,
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': self._get_cpu_usage(),
            'network_io': self._get_network_io()
        }
        
        return profile
        
    def _get_memory_usage(self) -> Dict[str, Any]:
        """Get memory usage information."""
        import psutil
        process = psutil.Process()
        
        return {
            'rss': process.memory_info().rss,  # Resident set size
            'vms': process.memory_info().vms,  # Virtual memory size
            'share': process.memory_info().shared,
            'text': process.memory_info().text,
            'data': process.memory_info().data,
            'dirty': process.memory_info().dirty
        }
        
    def _get_cpu_usage(self) -> Dict[str, Any]:
        """Get CPU usage information."""
        import psutil
        
        return {
            'cpu_percent': psutil.cpu_percent(),
            'cpu_count': psutil.cpu_count(),
            'cpu_freq': psutil.cpu_freq().current if psutil.cpu_freq() else 0,
            'load_average': psutil.getloadavg() if hasattr(psutil, 'getloadavg') else 0
        }
        
    def _get_network_io(self) -> Dict[str, Any]:
        """Get network I/O information."""
        import psutil
        
        net_io = psutil.net_io_counters()
        
        return {
            'bytes_sent': net_io.bytes_sent,
            'bytes_recv': net_io.bytes_recv,
            'packets_sent': net_io.packets_sent,
            'packets_recv': net_io.packets_recv,
            'dropin': net_io.dropin,
            'dropout': net_io.dropout,
            'errorsin': net_io.errin,
            'errorsout': net_io.errout
        }
        
    def benchmark_performance(self, simulation_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Run performance benchmark across different configurations.
        
        Args:
            simulation_configs: List of configuration dictionaries
            
        Returns:
            Dictionary containing benchmark results
        """
        benchmarks = {}
        
        for i, config in enumerate(simulation_configs):
            config_key = f"config_{i}"
            benchmarks[config_key] = {
                'config': config,
                'profiles': []
            }
            
            # Run simulation multiple times for averaging
            for run in range(3):  # 3 runs for averaging
                profile = self.profile_simulation(self._run_simulation_from_config, config)
                benchmarks[config_key]['profiles'].append(profile)
                
        # Calculate averages and benchmarks
        benchmark_results = {
            'configurations': len(simulation_configs),
            'total_runs': len(simulation_configs) * 3,
            'configuration_results': {}
        }
        
        for config_key, config_data in benchmarks.items():
            profiles = config_data['profiles']
            
            # Calculate averages
            avg_execution_time = statistics.mean(p['execution_time'] for p in profiles)
            avg_memory = statistics.mean(p['memory_usage']['rss'] for p in profiles)
            avg_cpu = statistics.mean(p['cpu_usage']['cpu_percent'] for p in profiles)
            
            benchmark_results['configuration_results'][config_key] = {
                'average_execution_time': avg_execution_time,
                'average_memory_usage': avg_memory,
                'average_cpu_usage': avg_cpu,
                'efficiency_score': self._calculate_efficiency_score(
                    avg_execution_time, avg_memory, avg_cpu
                )
            }
            
        return benchmark_results
        
    def _run_simulation_from_config(self, config: Dict[str, Any]):
        """Run simulation from configuration."""
        # This would typically create and run an NLM simulation
        # For demonstration, we'll return a dummy result
        import time
        time.sleep(0.1)  # Simulate simulation time
        
        return {
            'success': True,
            'final_state': 'simulated'
        }
        
    def _calculate_efficiency_score(self, execution_time: float, 
                                   memory_usage: float, cpu_usage: float) -> float:
        """
        Calculate efficiency score (higher is better).
        
        Args:
            execution_time: Execution time in seconds
            memory_usage: Memory usage in bytes
            cpu_usage: CPU usage percentage
            
        Returns:
            Efficiency score (0-100)
        """
        # Normalize factors
        time_score = max(0, 100 - execution_time * 10)  # Faster = better
        memory_score = max(0, 100 - (memory_usage / (1024 * 1024)))  # Less memory = better
        cpu_score = cpu_usage  # Higher CPU usage = better utilization
        
        # Weighted average
        efficiency = (time_score * 0.4 + memory_score * 0.3 + cpu_score * 0.3)
        
        return max(0, min(efficiency, 100))
        
    def generate_performance_report(self, benchmark_results: Dict[str, Any]) -> str:
        """
        Generate performance report string.
        
        Args:
            benchmark_results: Benchmark results dictionary
            
        Returns:
            Formatted performance report string
        """
        report = "NLM Performance Benchmark Report\n"
        report += "=" * 50 + "\n\n"
        
        report += f"Total Configurations Tested: {benchmark_results['configurations']}\n"
        report += f"Total Simulation Runs: {benchmark_results['total_runs']}\n\n"
        
        report += "Configuration Performance:\n"
        for config_key, config_result in benchmark_results['configuration_results'].items():
            report += f"  {config_key}:\n"
            report += f"    Execution Time: {config_result['average_execution_time']:.3f}s\n"
            report += f"    Memory Usage: {config_result['average_memory_usage'] / (1024 * 1024):.1f} MB\n"
            report += f"    CPU Usage: {config_result['average_cpu_usage']:.1f}%\n"
            report += f"    Efficiency Score: {config_result['efficiency_score']:.1f}/100\n\n"
            
        # Determine best configuration
        best_config = max(benchmark_results['configuration_results'].items(),
                         key=lambda x: x[1]['efficiency_score'])
        
        report += f"Best Performing Configuration: {best_config[0]}\n"
        report += f"Best Efficiency Score: {best_config[1]['efficiency_score']:.1f}/100\n"
        
        return report
class NLMSimulationProfiler:
    """
    Performance profiler for NLM simulations.
    
    Provides detailed profiling information for optimization analysis
    and performance benchmarking.
    """
    
    def __init__(self):
        self.profiles = {}
        self.performance_benchmarks = {}
        
    def profile_simulation(self, simulation_func, *args, **kwargs) -> Dict[str, Any]:
        """
        Profile a simulation and return performance metrics.
        
        Args:
            simulation_func: Simulation function to profile
            *args, **kwargs: Arguments to pass to simulation function
            
        Returns:
            Dictionary containing performance metrics
        """
        import time
        
        # Time the simulation
        start_time = time.time()
        result = simulation_func(*args, **kwargs)
        end_time = time.time()
        
        # Calculate performance metrics
        execution_time = end_time - start_time
        
        profile = {
            'execution_time': execution_time,
            'timestamp': start_time,
            'result': result,
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': self._get_cpu_usage(),
            'network_io': self._get_network_io()
        }
        
        return profile
        
    def _get_memory_usage(self) -> Dict[str, Any]:
        """Get memory usage information."""
        import psutil
        process = psutil.Process()
        
        return {
            'rss': process.memory_info().rss,  # Resident set size
            'vms': process.memory_info().vms,  # Virtual memory size
            'share': process.memory_info().shared,
            'text': process.memory_info().text,
            'data': process.memory_info().data,
            'dirty': process.memory_info().dirty
        }
        
    def _get_cpu_usage(self) -> Dict[str, Any]:
        """Get CPU usage information."""
        import psutil
        
        return {
            'cpu_percent': psutil.cpu_percent(),
            'cpu_count': psutil.cpu_count(),
            'cpu_freq': psutil.cpu_freq().current if psutil.cpu_freq() else 0,
            'load_average': psutil.getloadavg() if hasattr(psutil, 'getloadavg') else 0
        }
        
    def _get_network_io(self) -> Dict[str, Any]:
        """Get network I/O information."""
        import psutil
        
        net_io = psutil.net_io_counters()
        
        return {
            'bytes_sent': net_io.bytes_sent,
            'bytes_recv': net_io.bytes_recv,
            'packets_sent': net_io.packets_sent,
            'packets_recv': net_io.packets_recv,
            'dropin': net_io.dropin,
            'dropout': net_io.dropout,
            'errorsin': net_io.errin,
            'errorsout': net_io.errout
        }
        
    def benchmark_performance(self, simulation_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Run performance benchmark across different configurations.
        
        Args:
            simulation_configs: List of configuration dictionaries
            
        Returns:
            Dictionary containing benchmark results
        """
        benchmarks = {}
        
        for i, config in enumerate(simulation_configs):
            config_key = f"config_{i}"
            benchmarks[config_key] = {
                'config': config,
                'profiles': []
            }
            
            # Run simulation multiple times for averaging
            for run in range(3):  # 3 runs for averaging
                profile = self.profile_simulation(self._run_simulation_from_config, config)
                benchmarks[config_key]['profiles'].append(profile)
                
        # Calculate averages and benchmarks
        benchmark_results = {
            'configurations': len(simulation_configs),
            'total_runs': len(simulation_configs) * 3,
            'configuration_results': {}
        }
        
        for config_key, config_data in benchmarks.items():
            profiles = config_data['profiles']
            
            # Calculate averages
            avg_execution_time = statistics.mean(p['execution_time'] for p in profiles)
            avg_memory = statistics.mean(p['memory_usage']['rss'] for p in profiles)
            avg_cpu = statistics.mean(p['cpu_usage']['cpu_percent'] for p in profiles)
            
            benchmark_results['configuration_results'][config_key] = {
                'average_execution_time': avg_execution_time,
                'average_memory_usage': avg_memory,
                'average_cpu_usage': avg_cpu,
                'efficiency_score': self._calculate_efficiency_score(
                    avg_execution_time, avg_memory, avg_cpu
                )
            }
            
        return benchmark_results
        
    def _run_simulation_from_config(self, config: Dict[str, Any]):
        """Run simulation from configuration."""
        # This would typically create and run an NLM simulation
        # For demonstration, we'll return a dummy result
        import time
        time.sleep(0.1)  # Simulate simulation time
        
        return {
            'success': True,
            'final_state': 'simulated'
        }
        
    def _calculate_efficiency_score(self, execution_time: float, 
                                   memory_usage: float, cpu_usage: float) -> float:
        """
        Calculate efficiency score (higher is better).
        
        Args:
            execution_time: Execution time in seconds
            memory_usage: Memory usage in bytes
            cpu_usage: CPU usage percentage
            
        Returns:
            Efficiency score (0-100)
        """
        # Normalize factors
        time_score = max(0, 100 - execution_time * 10)  # Faster = better
        memory_score = max(0, 100 - (memory_usage / (1024 * 1024)))  # Less memory = better
        cpu_score = cpu_usage  # Higher CPU usage = better utilization
        
        # Weighted average
        efficiency = (time_score * 0.4 + memory_score * 0.3 + cpu_score * 0.3)
        
        return max(0, min(efficiency, 100))
        
    def generate_performance_report(self, benchmark_results: Dict[str, Any]) -> str:
        """
        Generate performance report string.
        
        Args:
            benchmark_results: Benchmark results dictionary
            
        Returns:
            Formatted performance report string
        """
        report = "NLM Performance Benchmark Report\n"
        report += "=" * 50 + "\n\n"
        
        report += f"Total Configurations Tested: {benchmark_results['configurations']}\n"
        report += f"Total Simulation Runs: {benchmark_results['total_runs']}\n\n"
        
        report += "Configuration Performance:\n"
        for config_key, config_result in benchmark_results['configuration_results'].items():
            report += f"  {config_key}:\n"
            report += f"    Execution Time: {config_result['average_execution_time']:.3f}s\n"
            report += f"    Memory Usage: {config_result['average_memory_usage'] / (1024 * 1024):.1f} MB\n"
            report += f"    CPU Usage: {config_result['average_cpu_usage']:.1f}%\n"
            report += f"    Efficiency Score: {config_result['efficiency_score']:.1f}/100\n\n"
            
        # Determine best configuration
        best_config = max(benchmark_results['configuration_results'].items(),
                         key=lambda x: x[1]['efficiency_score'])
        
        report += f"Best Performing Configuration: {best_config[0]}\n"
        report += f"Best Efficiency Score: {best_config[1]['efficiency_score']:.1f}/100\n"
        
        return report
        
    def profile_simulation(self, simulation_func, *args, **kwargs) -> Dict[str, Any]:
        """
        Profile a simulation and return performance metrics.
        
        Args:
            simulation_func: Simulation function to profile
            *args, **kwargs: Arguments to pass to simulation function
            
        Returns:
            Dictionary containing performance metrics
        """
        import time
        
        # Time the simulation
        start_time = time.time()
        result = simulation_func(*args, **kwargs)
        end_time = time.time()
        
        # Calculate performance metrics
        execution_time = end_time - start_time
        
        profile = {
            'execution_time': execution_time,
            'timestamp': start_time,
            'result': result,
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': self._get_cpu_usage(),
            'network_io': self._get_network_io()
        }
        
        return profile
        
    def _get_memory_usage(self) -> Dict[str, Any]:
        """Get memory usage information."""
        import psutil
        process = psutil.Process()
        
        return {
            'rss': process.memory_info().rss,  # Resident set size
            'vms': process.memory_info().vms,  # Virtual memory size
            'share': process.memory_info().shared,
            'text': process.memory_info().text,
            'data': process.memory_info().data,
            'dirty': process.memory_info().dirty
        }
        
    def _get_cpu_usage(self) -> Dict[str, Any]:
        """Get CPU usage information."""
        import psutil
        
        return {
            'cpu_percent': psutil.cpu_percent(),
            'cpu_count': psutil.cpu_count(),
            'cpu_freq': psutil.cpu_freq().current if psutil.cpu_freq() else 0,
            'load_average': psutil.getloadavg() if hasattr(psutil, 'getloadavg') else 0
        }
        
    def _get_network_io(self) -> Dict[str, Any]:
        """Get network I/O information."""
        import psutil
        
        net_io = psutil.net_io_counters()
        
        return {
            'bytes_sent': net_io.bytes_sent,
            'bytes_recv': net_io.bytes_recv,
            'packets_sent': net_io.packets_sent,
            'packets_recv': net_io.packets_recv,
            'dropin': net_io.dropin,
            'dropout': net_io.dropout,
            'errorsin': net_io.errin,
            'errorsout': net_io.errout
        }
        
    def benchmark_performance(self, simulation_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Run performance benchmark across different configurations.
        
        Args:
            simulation_configs: List of configuration dictionaries
            
        Returns:
            Dictionary containing benchmark results
        """
        benchmarks = {}
        
        for i, config in enumerate(simulation_configs):
            config_key = f"config_{i}"
            benchmarks[config_key] = {
                'config': config,
                'profiles': []
            }
            
            # Run simulation multiple times for averaging
            for run in range(3):  # 3 runs for averaging
                profile = self.profile_simulation(self._run_simulation_from_config, config)
                benchmarks[config_key]['profiles'].append(profile)
                
        # Calculate averages and benchmarks
        benchmark_results = {
            'configurations': len(simulation_configs),
            'total_runs': len(simulation_configs) * 3,
            'configuration_results': {}
        }
        
        for config_key, config_data in benchmarks.items():
            profiles = config_data['profiles']
            
            # Calculate averages
            avg_execution_time = statistics.mean(p['execution_time'] for p in profiles)
            avg_memory = statistics.mean(p['memory_usage']['rss'] for p in profiles)
            avg_cpu = statistics.mean(p['cpu_usage']['cpu_percent'] for p in profiles)
            
            benchmark_results['configuration_results'][config_key] = {
                'average_execution_time': avg_execution_time,
                'average_memory_usage': avg_memory,
                'average_cpu_usage': avg_cpu,
                'efficiency_score': self._calculate_efficiency_score(
                    avg_execution_time, avg_memory, avg_cpu
                )
            }
            
        return benchmark_results
        
    def _run_simulation_from_config(self, config: Dict[str, Any]):
        """Run simulation from configuration."""
        # This would typically create and run an NLM simulation
        # For demonstration, we'll return a dummy result
        import time
        time.sleep(0.1)  # Simulate simulation time
        
        return {
            'success': True,
            'final_state': 'simulated'
        }
        
    def _calculate_efficiency_score(self, execution_time: float, 
                                   memory_usage: float, cpu_usage: float) -> float:
        """
        Calculate efficiency score (higher is better).
        
        Args:
            execution_time: Execution time in seconds
            memory_usage: Memory usage in bytes
            cpu_usage: CPU usage percentage
            
        Returns:
            Efficiency score (0-100)
        """
        # Normalize factors
        time_score = max(0, 100 - execution_time * 10)  # Faster = better
        memory_score = max(0, 100 - (memory_usage / (1024 * 1024)))  # Less memory = better
        cpu_score = cpu_usage  # Higher CPU usage = better utilization
        
        # Weighted average
        efficiency = (time_score * 0.4 + memory_score * 0.3 + cpu_score * 0.3)
        
        return max(0, min(efficiency, 100))
        
    def generate_performance_report(self, benchmark_results: Dict[str, Any]) -> str:
        """
        Generate performance report string.
        
        Args:
            benchmark_results: Benchmark results dictionary
            
        Returns:
            Formatted performance report string
        """
        report = "NLM Performance Benchmark Report\n"
        report += "=" * 50 + "\n\n"
        
        report += f"Total Configurations Tested: {benchmark_results['configurations']}\n"
        report += f"Total Simulation Runs: {benchmark_results['total_runs']}\n\n"
        
        report += "Configuration Performance:\n"
        for config_key, config_result in benchmark_results['configuration_results'].items():
            report += f"  {config_key}:\n"
            report += f"    Execution Time: {config_result['average_execution_time']:.3f}s\n"
            report += f"    Memory Usage: {config_result['average_memory_usage'] / (1024 * 1024):.1f} MB\n"
            report += f"    CPU Usage: {config_result['average_cpu_usage']:.1f}%\n"
            report += f"    Efficiency Score: {config_result['efficiency_score']:.1f}/100\n\n"
            
        # Determine best configuration
        best_config = max(benchmark_results['configuration_results'].items(),
                         key=lambda x: x[1]['efficiency_score'])
        
        report += f"Best Performing Configuration: {best_config[0]}\n"
        report += f"Best Efficiency Score: {best_config[1]['efficiency_score']:.1f}/100\n"
        
        return report