# Python example for NLM integration
# This file demonstrates how to use the NLM Python bindings
# Phase 1: Basic Usage
# Phase 2: Advanced Features

"""
NLM (Neural Learning Machine) Python Interface
===============================================

This module provides Python bindings for the NLM neural simulation framework.
It enables researchers and developers to create, configure, and run neural
simulations programmatically.

Example Usage:
-------------

Basic Brain Creation:
    >>> import nlm
    >>> brain = nlm.create_brain("config.json")
    >>> spike_count = nlm.run_simple_simulation(brain, 1000)
    >>> print(f"Total spikes: {spike_count}")

Configuration Management:
    >>> config = nlm.get_default_config()
    >>> config.set("neuron_count", 1000)
    >>> config.set("simulation_timestep", 0.001)
    >>> config.save_to_file("custom_config.json")

Advanced Analysis:
    >>> import nlm
    >>> brain = nlm.create_brain("config.json")
    >>> nlm.run_simple_simulation(brain, 1000)
    >>> 
    >>> # Analyze network properties
    >>> topology = brain.analyze_topology()
    >>> learning_patterns = brain.analyze_learning()
    >>> complexity = brain.calculate_complexity()

Command Line Interface:
    >>> # Use the CLI tool
    >>> nlm-cli --help
    >>> nlm-sim --config config.json --steps 1000
    >>> nlm-config list
    >>> nlm-validate --file config.json

Simulation Management:
    >>> import nlm
    >>> brain = nlm.create_brain("config.json")
    >>> 
    >>> # Run adaptive simulation
    >>> results = brain.run_adaptive_simulation("exploration")
    >>> 
    >>> # Export network state
    >>> brain.export_network("network_state.json")

Visualization:
    >>> import nlm
    >>> brain = nlm.create_brain("config.json")
    >>> brain.run_simple_simulation(1000)
    >>> brain.visualize_network()
    >>> brain.plot_activity()
    >>> brain.save_visualization("network_viz.png")
"""

# Re-export commonly used classes and functions
from .core.config import Config
from .core.logger import Logger, LogLevel
from .brain.brain import Brain
from .utils.helpers import (
    format_number,
    format_time,
    format_bytes,
    generate_timestamp,
    sanitize_filename,
    file_exists,
    create_directory,
    split_string,
    string_contains,
)

# Advanced configuration presets
def get_research_config():
    """Return a configuration optimized for research experiments."""
    config = Config()
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    config.set("plasticity_learning_rate", 0.01)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    config.set("log_level", "INFO")
    config.set("log_to_file", False)
    return config

def get_realtime_config():
    """Return a configuration optimized for real-time applications."""
    config = Config()
    config.set("random_seed", 123)
    config.set("simulation_timestep", 0.0001)  # Faster timestep
    config.set("neuron_count", 100)  # Smaller for real-time
    config.set("region_count", 1)
    config.set("connection_probability", 0.05)
    config.set("plasticity_learning_rate", 0.005)
    config.set("log_level", "WARNING")  # Reduced logging overhead
    config.set("log_to_file", False)
    return config

def get_development_config():
    """Return a configuration optimized for development and testing."""
    config = Config()
    config.set("random_seed", 999)
    config.set("simulation_timestep", 0.01)  # Slower for debugging
    config.set("neuron_count", 10)  # Tiny for quick tests
    config.set("region_count", 1)
    config.set("connection_probability", 0.5)  # Higher connectivity
    config.set("plasticity_learning_rate", 0.1)  # Stronger plasticity
    config.set("log_level", "DEBUG")  # Maximum verbosity
    config.set("log_to_file", True)
    return config

# Simulation utilities
def run_experiment(config_file, simulation_steps, verbose=False):
    """
    Run a complete NLM experiment.
    
    Args:
        config_file: Path to configuration file
        simulation_steps: Number of simulation steps to run
        verbose: Enable verbose output
        
    Returns:
        dict: Experiment results including spike counts, learning metrics
    """
    # Implementation would go here
    pass

def compare_configurations(config_files, simulation_steps=1000):
    """
    Compare results from multiple configurations.
    
    Args:
        config_files: List of configuration file paths
        simulation_steps: Number of steps to run for each configuration
        
    Returns:
        list: Comparison results for each configuration
    """
    # Implementation would go here
    pass

# Quality of life functions
def ensure_directory(path):
    """Ensure a directory exists, creating it if necessary."""
    if not file_exists(path):
        create_directory(path)
    return path

def safe_filename(name, extension=""):
    """Create a safe filename from a string."""
    safe = sanitize_filename(name)
    if extension and not extension.startswith("."):
        extension = "." + extension
    return safe + extension

def timestamped_filename(base_name, extension=""):
    """Create a filename with timestamp."""
    timestamp = generate_timestamp()
    safe_base = sanitize_filename(base_name)
    if extension and not extension.startswith("."):
        extension = "." + extension
    return f"{safe_base}_{timestamp}{extension}"

# Error handling
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error related to configuration."""
    pass

class SimulationError(NLMError):
    """Error related to simulation."""
    pass

class ValidationError(NLMError):
    """Error related to validation."""
    pass

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Performance monitoring
performance_stats = {
    "configs_loaded": 0,
    "simulations_run": 0,
    "memory_usage_mb": 0,
    "avg_simulation_time_ms": 0,
}

def get_performance_stats():
    """Get current performance statistics."""
    return performance_stats.copy()

def reset_performance_stats():
    """Reset performance statistics."""
    global performance_stats
    performance_stats = {
        "configs_loaded": 0,
        "simulations_run": 0,
        "memory_usage_mb": 0,
        "avg_simulation_time_ms": 0,
    }

# Environment detection
import platform
import sys

is_windows = platform.system().lower() == "windows"
is_mac = platform.system().lower() == "darwin"
is_linux = platform.system().lower() == "linux"

python_version = sys.version_info
is_python3 = python_version.major == 3
is_python38_plus = is_python3 and python_version >= (3, 8)

# Feature detection
features = {
    "json_export": True,
    "yaml_import": False,  # Would require PyYAML
    "advanced_visualization": False,  # Would require matplotlib
    "parallel_simulation": False,  # Would require joblib/ray
    "distributed_computing": False,  # Would require MPI/dask
    "real_time_plotting": False,  # Would require matplotlib/interactive
    "gpu_acceleration": False,  # Would require CUDA bindings
}

def check_features():
    """Check which advanced features are available."""
    return features.copy()

# CLI shortcuts (for use from command line)
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Python Interface")
    parser.add_argument("--version", action="store_true", help="Show version")
    parser.add_argument("--features", action="store_true", help="Show available features")
    parser.add_argument("--create-config", help="Create a template configuration file")
    parser.add_argument("--validate", help="Validate a configuration file")
    
    args = parser.parse_args()
    
    if args.version:
        print(f"NLM Python Interface v{__version__}")
        print(f"Python {python_version.major}.{python_version.minor}.{python_version.micro}")
        print(f"Platform: {platform.system()}")
    
    elif args.features:
        print("Available features:")
        for feature, available in check_features().items():
            status = "✓" if available else "✗"
            print(f"  {status} {feature}")
    
    elif args.create_config:
        config = get_development_config()
        config.save_to_file(args.create_config)
        print(f"Created configuration template: {args.create_config}")
    
    elif args.validate:
        # Validate would go here
        print(f"Validating: {args.validate}")
        # Validation logic here
    
    else:
        parser.print_help()
