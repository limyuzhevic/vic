#!/usr/bin/env python3
"""
Advanced learning example for NLM Python API

Demonstrates advanced learning scenarios with the enhanced NLM Python API,
featuring custom neural architectures and hyperparameter tuning.
"""

import numpy as np
import logging
import time
from typing import Dict, List, Any
import matplotlib.pyplot as plt

# Import NLM modules
import pynlm
import pynlm.pynlm_advanced as advanced
import pynlm.pynlm_profiling as profiling

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


def create_custom_brain(config_dict: Dict[str, Any]) -> pynlm.Brain:
    """
    Create a custom brain with advanced configuration.
    
    Args:
        config_dict: Configuration dictionary
        
    Returns:
        Configured pynlm.Brain instance
    """
    # Create advanced config
    config = advanced.AdvancedConfig()
    for key, value in config_dict.items():
        config.set_value(key, value)
    
    # Create brain with advanced config
    brain = pynlm.createBrain(config)
    
    # Configure advanced features
    brain.initialize()
    
    logger.info("Custom brain created with advanced configuration")
    return brain


def hyperparameter_tuning(config_template: Dict[str, Any], 
                          param_grid: Dict[str, List]) -> Dict:
    """
    Perform hyperparameter tuning for brain configuration.
    
    Args:
        config_template: Base configuration
        param_grid: Dictionary of parameters to tune
        
    Returns:
        Dictionary containing best configuration and performance
    """
    best_score = -float('inf')
    best_config = None
    results = []
    
    # Simple grid search
    param_combinations = generate_param_combinations(param_grid)
    
    for i, params in enumerate(param_combinations):
        # Merge with template
        current_config = {**config_template, **params}
        
        # Create brain
        brain = create_custom_brain(current_config)
        
        # Simulate and get performance
        performance = simulate_brain(brain, steps=100)
        
        results.append({
            'config': current_config,
            'performance': performance
        })
        
        # Update best if improved
        if performance > best_score:
            best_score = performance
            best_config = current_config
        
        logger.info(f"Completed {i+1}/{len(param_combinations)} configurations")
    
    return {
        'best_config': best_config,
        'best_score': best_score,
        'all_results': results
    }


def generate_param_combinations(param_grid: Dict[str, List]) -> List[Dict]:
    """
    Generate all parameter combinations for hyperparameter tuning.
    
    Args:
        param_grid: Dictionary of parameters to tune
        
    Returns:
        List of parameter combinations
    """
    if not param_grid:
        return [{}]
    
    # Simple implementation - for real use, consider more efficient methods
    param_names = list(param_grid.keys())
    param_values = list(param_grid.values())
    
    combinations = []
    for i in range(len(param_values[0])):
        combo = {}
        for j, param_name in enumerate(param_names):
            if j < len(param_values):
                combo[param_name] = param_values[j][i]
        combinations.append(combo)
    
    return combinations


def simulate_brain(brain: pynlm.Brain, steps: int) -> float:
    """
    Simulate brain and return performance score.
    
    Args:
        brain: pynlm.Brain instance
        steps: Number of simulation steps
        
    Returns:
        Performance score
    """
    # Simple performance metric: spike count / step
    total_spikes = 0
    
    for step in range(steps):
        # Perform simulation step
        current_step = pynlm.SimulationStep(step)
        current_time = pynlm.Timestamp(step * 0.1)
        
        brain.step(current_step, current_time)
        
        # Record spikes
        total_spikes += brain.getTotalSpikeCount()
    
    # Performance metric (normalized)
    performance = total_spikes / steps / 100.0  # Normalize to reasonable range
    
    return performance


def visualize_learning_results(hyperparam_results: Dict):
    """
    Visualize hyperparameter tuning results.
    
    Args:
        hyperparam_results: Results from hyperparameter tuning
    """
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))
    
    # Plot 1: Performance distribution
    performances = [r['performance'] for r in hyperparam_results['all_results']]
    ax1.hist(performances, bins=20, alpha=0.7, color='skyblue')
    ax1.axvline(hyperparam_results['best_score'], color='red', linestyle='--', 
                label=f"Best: {hyperparam_results['best_score']:.4f}")
    ax1.set_xlabel('Performance Score')
    ax1.set_ylabel('Frequency')
    ax1.set_title('Performance Distribution')
    ax1.legend()
    
    # Plot 2: Parameter importance (simplified)
    best_config = hyperparam_results['best_config']
    param_names = list(best_config.keys())
    param_values = list(best_config.values())
    
    # Filter numeric values for plotting
    numeric_data = [(name, val) for name, val in zip(param_names, param_values) 
                   if isinstance(val, (int, float))]
    
    if numeric_data:
        names, values = zip(*numeric_data)
        ax2.bar(range(len(names)), values, alpha=0.7)
        ax2.set_xticks(range(len(names)))
        ax2.set_xticklabels(names, rotation=45)
        ax2.set_ylabel('Value')
        ax2.set_title('Best Configuration Parameters')
    
    plt.tight_layout()
    plt.show()
    
    logger.info("Learning results visualization completed")


def main():
    """Main function demonstrating advanced learning."""
    logger.info("Starting advanced learning example")
    
    # Configuration template
    config_template = {
        'neuron_count': 100,
        'learning_rate': 0.01,
        'plasticity_enabled': True,
        'development_enabled': True,
        'reward_modulation': True,
        'curiosity_enabled': True
    }
    
    # Hyperparameter grid
    param_grid = {
        'learning_rate': [0.001, 0.01, 0.1],
        'plasticity_enabled': [True, False],
        'development_enabled': [True, False],
        'reward_modulation': [True, False],
        'curiosity_enabled': [True, False]
    }
    
    # Perform hyperparameter tuning
    logger.info("Starting hyperparameter tuning...")
    hyperparam_results = hyperparameter_tuning(config_template, param_grid)
    
    # Display results
    logger.info(f"Best configuration: {hyperparam_results['best_config']}")
    logger.info(f"Best performance score: {hyperparam_results['best_score']:.4f}")
    
    # Visualize results
    visualize_learning_results(hyperparam_results)
    
    logger.info("Advanced learning example completed")


if __name__ == "__main__":
    main()