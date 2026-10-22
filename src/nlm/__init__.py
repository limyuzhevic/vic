"""
Python bindings for NLM (Neural Learning Machine)

This module provides Python bindings for the NLM neural network simulation framework,
enabling seamless integration with Python's machine learning ecosystem.
"""

import sys
import os
from typing import Optional, List, Dict, Any, Union
import json
import numpy as np

# Import C++ extensions (will be compiled during setup)
try:
    from .core.nlm_cpp import (
        Brain as CPUBrain,
        Neuron as CPUNeuron,
        Synapse as CPUSynapse,
        Config as CPUConfig,
        Logger as CPULogger,
        SimulationClock as CPUSimulationClock,
        RandomGenerator as CPURandomGenerator,
    )
    CPP_AVAILABLE = True
except ImportError:
    CPP_AVAILABLE = False

# Python interfaces
from .brain import Brain, Neuron, Synapse, Config, Logger, SimulationClock, RandomGenerator
from .visualization import (
    visualize_neural_network,
    visualize_spike_activity,
    visualize_network_topology,
    create_interactive_dashboard,
)
from .debug import DebugManager, MemoryTracker, SpikeProfiler, ConfigurationProfiler

__version__ = "0.1.0"

if CPP_AVAILABLE:
    __version__ = "0.1.0+cpp"

# Check for C++ backend availability
def check_cpp_backend():
    if not CPP_AVAILABLE:
        print("Warning: C++ backend not available. Using pure Python implementation.")
        return False
    return True

# Initialize logging
logger = Logger()

def get_logger():
    return logger

# Main Python API functions
def create_brain(
    neuron_count: int = 100,
    region_count: int = 2,
    connection_probability: float = 0.15,
    random_seed: Optional[int] = None,
    brain_type: str = "standard"
) -> Brain:
    """
    Create a new neural network brain.
    
    Args:
        neuron_count: Number of neurons in the brain
        region_count: Number of regions (for hierarchical architectures)
        connection_probability: Probability of connections between neurons
        random_seed: Random seed for reproducibility
        brain_type: Type of brain architecture ("standard", "scale_free", "small_world", "modular")
    
    Returns:
        Brain instance
    """
    return Brain(neuron_count, region_count, connection_probability, random_seed, brain_type)

def load_configuration(filename: str) -> Dict[str, Any]:
    """
    Load configuration from a file.
    
    Args:
        filename: Path to configuration file
    
    Returns:
        Configuration dictionary
    """
    with open(filename, 'r') as f:
        return json.load(f)

def save_configuration(config: Dict[str, Any], filename: str):
    """
    Save configuration to a file.
    
    Args:
        config: Configuration dictionary
        filename: Path to save configuration file
    """
    with open(filename, 'w') as f:
        json.dump(config, f, indent=2)

def run_simulation(
    brain: Brain,
    steps: int,
    output_file: Optional[str] = None,
    real_time_monitoring: bool = False
) -> Dict[str, Any]:
    """
    Run a neural network simulation.
    
    Args:
        brain: Brain instance to simulate
        steps: Number of simulation steps to run
        output_file: Optional file to save results
        real_time_monitoring: Enable real-time monitoring
    
    Returns:
        Dictionary containing simulation results
    """
    if real_time_monitoring:
        return brain.run_with_monitoring(steps, output_file)
    else:
        return brain.run(steps, output_file)

def analyze_results(brain: Brain) -> Dict[str, Any]:
    """
    Analyze simulation results.
    
    Args:
        brain: Brain instance to analyze
    
    Returns:
        Dictionary containing analysis results
    """
    return brain.analyze()

def compare_brains(
    brains: List[Brain],
    metrics: Optional[List[str]] = None
) -> Dict[str, Any]:
    """
    Compare multiple brain simulations.
    
    Args:
        brains: List of Brain instances to compare
        metrics: List of metrics to compare (default: all available metrics)
    
    Returns:
        Dictionary containing comparison results
    """
    if metrics is None:
        metrics = ["firing_rate", "weight_change", "learning_speed", "stability"]
    
    return brain.compare(brains, metrics)

def batch_run(
    configs: List[Dict[str, Any]],
    output_base: str = "batch_results",
    parallel: bool = False
) -> List[Dict[str, Any]]:
    """
    Run batch simulations with different configurations.
    
    Args:
        configs: List of configuration dictionaries
        output_base: Base name for output files
        parallel: Enable parallel execution
    
    Returns:
        List of result dictionaries
    """
    results = []
    for i, config in enumerate(configs):
        print(f"Running simulation {i+1}/{len(configs)}: {config.get('name', 'Unnamed')}")
        
        # Create brain from configuration
        brain = Brain.from_config(config)
        brain.initialize()
        
        # Run simulation
        output_file = f"{output_base}_{i}.json"
        result = brain.run(config.get('steps', 100), output_file)
        results.append(result)
    
    return results

def generate_report(brain: Brain, report_type: str = "comprehensive") -> str:
    """
    Generate a report for a brain simulation.
    
    Args:
        brain: Brain instance to generate report for
        report_type: Type of report ("comprehensive", "summary", "detailed")
    
    Returns:
        Report as string
    """
    if report_type == "comprehensive":
        return brain.generate_comprehensive_report()
    elif report_type == "summary":
        return brain.generate_summary_report()
    elif report_type == "detailed":
        return brain.generate_detailed_report()
    else:
        raise ValueError(f"Unknown report type: {report_type}")

def export_results(brain: Brain, format: str = "json") -> Any:
    """
    Export simulation results in specified format.
    
    Args:
        brain: Brain instance to export results from
        format: Export format ("json", "numpy", "csv")
    
    Returns:
        Exported results
    """
    if format == "json":
        return brain.export_to_json()
    elif format == "numpy":
        return brain.export_to_numpy()
    elif format == "csv":
        return brain.export_to_csv()
    else:
        raise ValueError(f"Unknown export format: {format}")

def import_results(data: Any, format: str = "json") -> Brain:
    """
    Import simulation results from specified format.
    
    Args:
        data: Imported data
        format: Import format ("json", "numpy", "csv")
    
    Returns:
        Brain instance
    """
    if format == "json":
        return Brain.from_json(data)
    elif format == "numpy":
        return Brain.from_numpy(data)
    elif format == "csv":
        return Brain.from_csv(data)
    else:
        raise ValueError(f"Unknown import format: {format}")

def main():
    """Main entry point for command-line interface."""
    print("NLM Python Interface")
    print("Version:", __version__)
    print()
    print("Usage:")
    print("  python -m nlm.python_interface [command] [options]")
    print()
    print("Commands:")
    print("  run       Run a simulation")
    print("  analyze   Analyze simulation results")
    print("  compare   Compare multiple simulations")
    print("  batch     Run batch simulations")
    print("  report    Generate reports")
    print("  help      Show help information")
    print()

if __name__ == "__main__":
    main()
