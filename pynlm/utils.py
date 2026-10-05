#!/usr/bin/env python3
"""
NLM Utils Module

This module provides additional utility functions and advanced features
for working with the NLM Python API.
"""

import sys
import json
from pathlib import Path

# Import core bindings
from pynlm.bindings import (
    create_brain_with_preset as _create_brain_with_preset,
    run_simulation_sequence as _run_simulation_sequence,
    benchmark_performance as _benchmark_performance,
    create_experiment_config as _create_experiment_config,
    validate_brain_state as _validate_brain_state,
    get_all_presets as _get_all_presets,
    compare_brains as _compare_brains,
    export_brain_data as _export_brain_data,
    import_brain_data as _import_brain_data,
    run_repeated_experiments as _run_repeated_experiments,
    run_with_visualization as _run_with_visualization,
    create_network_plot as _create_network_plot,
    create_activity_plot as _create_activity_plot,
    create_spike_raster as _create_spike_raster,
)

# Python utility functions (implemented in pure Python)
import time
from typing import List, Dict, Any, Callable, Optional

def create_brain_with_preset(preset_name: str) -> Any:
    """Create a brain with a predefined configuration preset.
    
    Args:
        preset_name: Size preset ("small", "medium", or "large")
        
    Returns:
        Brain object configured with the specified preset
        
    Raises:
        ValueError: If preset_name is not recognized
    """
    preset_name = preset_name.lower()
    if preset_name not in ["small", "medium", "large"]:
        raise ValueError(f"Unknown preset '{preset_name}'. Available: small, medium, large")
    
    return _create_brain_with_preset(preset_name)

def get_all_presets() -> Dict[str, Dict[str, Any]]:
    """Get all available configuration presets.
    
    Returns:
        Dictionary of presets with their configuration parameters
    """
    return _get_all_presets()

def create_experiment_config(experiment_name: str) -> Any:
    """Create a configuration optimized for specific experiment types.
    
    Args:
        experiment_name: Experiment type ("curiosity", "development", 
                        "prediction", or "memory")
        
    Returns:
        Config object optimized for the specified experiment
        
    Raises:
        ValueError: If experiment_name is not recognized
    """
    experiment_name = experiment_name.lower()
    valid_experiments = ["curiosity", "development", "prediction", "memory"]
    if experiment_name not in valid_experiments:
        raise ValueError(f"Unknown experiment '{experiment_name}'. Available: {', '.join(valid_experiments)}")
    
    return _create_experiment_config(experiment_name)

def benchmark_performance(neuron_count: int = 1000, steps: int = 100) -> Dict[str, Any]:
    """Benchmark brain performance.
    
    Args:
        neuron_count: Number of neurons in the brain
        steps: Number of simulation steps to run
        
    Returns:
        Dictionary containing benchmark results including timing and statistics
    """
    return _benchmark_performance(neuron_count, steps)

def compare_brains(brains: List[Any], steps: int = 100) -> Dict[str, Dict[str, Any]]:
    """Compare multiple brains side by side.
    
    Args:
        brains: List of Brain objects to compare
        steps: Number of simulation steps for each brain
        
    Returns:
        Dictionary with comparison results for each brain
    """
    return _compare_brains(brains, steps)

def validate_brain_state(brain: Any) -> Dict[str, Any]:
    """Validate brain state and return validation results.
    
    Args:
        brain: Brain object to validate
        
    Returns:
        Dictionary with validation results including validity status and error messages
    """
    return _validate_brain_state(brain)

def export_brain_data(brain: Any, filename: str) -> bool:
    """Export brain state to a file for checkpointing or sharing.
    
    Args:
        brain: Brain object to export
        filename: Output file path
        
    Returns:
        True if export was successful
        
    Raises:
        RuntimeError: If export fails
    """
    return _export_brain_data(brain, filename)

def import_brain_data(filename: str) -> Any:
    """Import brain state from a file and return a new brain object.
    
    Args:
        filename: Input file path
        
    Returns:
        New Brain object with loaded state
        
    Raises:
        RuntimeError: If import fails
    """
    return _import_brain_data(filename)

def run_simulation_sequence(
    brains: List[Any], 
    experiment_names: Optional[List[str]] = None, 
    steps_per_brain: int = 100
) -> Dict[str, Dict[str, Any]]:
    """Run a sequence of simulation experiments with multiple brains.
    
    Args:
        brains: List of Brain objects to run experiments
        experiment_names: Optional list of experiment names for each brain
        steps_per_brain: Number of steps to run for each brain
        
    Returns:
        Dictionary with results from each experiment
    """
    if experiment_names is None:
        experiment_names = [f"brain_{i}" for i in range(len(brains))]
    
    return _run_simulation_sequence(brains, experiment_names, steps_per_brain)

def run_repeated_experiments(
    brain_factory: Callable[[], Any],
    experiment_name: str,
    num_runs: int = 10,
    steps_per_run: int = 100
) -> Dict[str, Dict[str, Any]]:
    """Run multiple independent experiments and return results.
    
    Args:
        brain_factory: Function that creates a new brain instance
        experiment_name: Name of the experiment for organization
        num_runs: Number of independent runs
        steps_per_run: Steps per run
        
    Returns:
        Dictionary with results from each run
    """
    return _run_repeated_experiments(brain_factory, experiment_name, num_runs, steps_per_run)

def run_with_visualization(
    brain: Any,
    title: str = "NLM Visualization",
    durationMs: int = 5000,
    fps: float = 30.0
) -> bool:
    """Run simulation with real-time visualization.
    
    Args:
        brain: Brain object to simulate
        title: Window title for visualization
        durationMs: Duration of visualization in milliseconds
        fps: Frames per second for visualization
        
    Returns:
        True if visualization was successful
    """
    return _run_with_visualization(brain, title, durationMs, fps)

def create_network_plot(
    brain: Any,
    outputFile: str = "network.png",
    show_neurons: bool = True,
    show_synapses: bool = True,
    layout: str = "circular"
) -> bool:
    """Create and save a network structure visualization.
    
    Args:
        brain: Brain object to visualize
        outputFile: Output file path
        show_neurons: Whether to display neurons
        show_synapses: Whether to display synapses
        layout: Network layout ("circular", "radial", "grid")
        
    Returns:
        True if plot was created successfully
    """
    return _create_network_plot(brain, outputFile)

def create_activity_plot(
    brain: Any,
    outputFile: str = "activity.png",
    timeWindow: int = 1000,
    show_raster: bool = True,
    show_summary: bool = True
) -> bool:
    """Create and save neural activity visualization.
    
    Args:
        brain: Brain object to visualize
        outputFile: Output file path
        timeWindow: Time window for activity visualization
        show_raster: Whether to show spike raster
        show_summary: Whether to show summary statistics
        
    Returns:
        True if plot was created successfully
    """
    return _create_activity_plot(brain, outputFile, timeWindow)

def create_spike_raster(
    brains: List[Any],
    outputFile: str = "spike_raster.png",
    timeWindow: int = 1000,
    show_legend: bool = True,
    normalize: bool = False
) -> bool:
    """Create and save spike raster plot for multiple brains.
    
    Args:
        brains: List of Brain objects
        outputFile: Output file path
        timeWindow: Time window for raster plot
        show_legend: Whether to show neuron legend
        normalize: Whether to normalize spike counts
        
    Returns:
        True if raster was created successfully
    """
    return _create_spike_raster(brains, outputFile, timeWindow)

def main():
    """Main function for utils module CLI"""
    print("NLM Utils Module v0.1.0")
    print("===================")
    print()
    print("Available utility functions:")
    print("  create_brain_with_preset(preset_name) - Create brain with preset size")
    print("  get_all_presets() - List all available presets")
    print("  create_experiment_config(experiment_name) - Create config for experiment")
    print("  benchmark_performance(neuron_count, steps) - Benchmark brain performance")
    print("  compare_brains(brains, steps) - Compare multiple brains")
    print("  validate_brain_state(brain) - Validate brain state")
    print("  export_brain_data(brain, filename) - Export brain data")
    print("  import_brain_data(filename) - Import brain data")
    print("  run_simulation_sequence(brains, experiment_names, steps) - Run multiple experiments")
    print("  run_repeated_experiments(brain_factory, experiment_name, num_runs, steps) - Run repeated experiments")
    print("  run_with_visualization(brain, title, duration, fps) - Run with visualization")
    print("  create_network_plot(brain, outputFile, ...) - Create network visualization")
    print("  create_activity_plot(brain, outputFile, ...) - Create activity visualization")
    print("  create_spike_raster(brains, outputFile, ...) - Create spike raster plot")
    print()
    print("For Python examples, see examples/enhanced_api_examples.py")
    return 0
if __name__ == "__main__":
    main()
