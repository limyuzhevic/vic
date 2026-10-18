"""
Utility functions and tools for NLM Python API
"""

import numpy as np
import json
import time
from typing import Dict, List, Any, Callable, Optional
import warnings

# Import core classes
from .core import NLM, MockBrain, MockConfig

# Common configuration templates
TEMPLATES = {
    'small': {
        'neuron_count': 100,
        'region_count': 1,
        'log_level': 'WARN',
    },
    'medium': {
        'neuron_count': 1000,
        'region_count': 2,
        'log_level': 'INFO',
    },
    'large': {
        'neuron_count': 10000,
        'region_count': 5,
        'log_level': 'INFO',
    },
    'research': {
        'neuron_count': 5000,
        'region_count': 3,
        'plasticity_learning_rate': 0.005,
        'synaptogenesis_rate': 0.0005,
        'enable_multithreading': True,
        'enable_SIMD': True,
    },
    'production': {
        'neuron_count': 50000,
        'region_count': 10,
        'enable_sparse_connectivity': True,
        'enable_event_driven': True,
        'enable_multithreading': True,
        'enable_SIMD': True,
        'log_to_file': True,
        'log_level': 'ERROR',
    },
}

def create_default_config():
    """Create default NLM configuration"""
    return {
        'random_seed': 42,
        'simulation_timestep': 0.001,
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.1,
        'plasticity_learning_rate': 0.01,
        'stdp_ltp_weight': 0.01,
        'stdp_ltd_weight': 0.012,
        'stdp_time_constant': 20.0,
        'synaptogenesis_rate': 0.001,
        'pruning_rate': 0.0001,
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
        'visualization_update_rate': 30.0,
        'enable_multithreading': True,
        'enable_SIMD': True,
        'enable_sparse_connectivity': True,
        'enable_event_driven': True,
    }

def get_config_template(name: str) -> Dict[str, Any]:
    """
    Get predefined configuration template
    
    Args:
        name: Template name ('small', 'medium', 'large', 'research', 'production')
    
    Returns:
        Configuration dictionary
    
    Raises:
        ValueError: If template name is not recognized
    """
    name = name.lower()
    if name not in TEMPLATES:
        available = ', '.join(TEMPLATES.keys())
        raise ValueError(f"Unknown template '{name}'. Available templates: {available}")
    
    # Create a copy of the template
    return TEMPLATES[name].copy()

def validate_config(config: Dict[str, Any]) -> List[str]:
    """
    Validate configuration dictionary
    
    Args:
        config: Configuration dictionary
    
    Returns:
        List of validation errors (empty if valid)
    """
    errors = []
    
    # Check required keys
    required_keys = ['neuron_count', 'region_count', 'simulation_timestep']
    for key in required_keys:
        if key not in config:
            errors.append(f"Missing required key: {key}")
    
    # Validate values
    if 'neuron_count' in config and config['neuron_count'] <= 0:
        errors.append("neuron_count must be positive")
    
    if 'region_count' in config and config['region_count'] <= 0:
        errors.append("region_count must be positive")
    
    if 'simulation_timestep' in config and config['simulation_timestep'] <= 0:
        errors.append("simulation_timestep must be positive")
    
    if 'connection_probability' in config:
        cp = config['connection_probability']
        if not (0.0 <= cp <= 1.0):
            errors.append("connection_probability must be between 0 and 1")
    
    if 'random_seed' in config and config['random_seed'] < 0:
        errors.append("random_seed must be non-negative")
    
    return errors

def create_default_nlm(config: Optional[Dict[str, Any]] = None) -> 'NLM':
    """
    Create NLM with default configuration
    
    Args:
        config: Configuration dictionary (optional)
    
    Returns:
        NLM object
    """
    if config is None:
        config = create_default_config()
    elif isinstance(config, str):
        # Load from file
        with open(config, 'r') as f:
            config = json.load(f)
    
    # Validate configuration
    errors = validate_config(config)
    if errors:
        raise ValueError(f"Configuration validation failed:\n" + "\n".join(f"  - {error}" for error in errors))
    
    return NLM(config)

def run_multiple_experiments(
    experiment_configs: List[Dict[str, Any]],
    config: Optional[Dict[str, Any]] = None,
    on_progress: Optional[Callable[[int, int, str], None]] = None
) -> List[Dict[str, Any]]:
    """
    Run multiple experiments with shared configuration
    
    Args:
        experiment_configs: List of experiment configurations
        config: Configuration dictionary (optional)
        on_progress: Progress callback function
    
    Returns:
        List of experiment results
    """
    if config is None:
        config = create_default_config()
    
    nlm = NLM(config)
    results = []
    
    for i, exp_config in enumerate(experiment_configs):
        name = exp_config.get('name', f'Experiment {i+1}')
        
        if on_progress:
            on_progress(i, len(experiment_configs), name)
        
        try:
            result = nlm.run_experiment(**exp_config)
            results.append(result)
        except Exception as e:
            warnings.warn(f"Experiment {name} failed: {str(e)}")
            # Create failed result entry
            results.append({
                'name': name,
                'success': False,
                'error': str(e),
                'timestamp': time.time(),
            })
    
    return results

def get_performance_summary(experiment_results: List[Dict[str, Any]]) -> Dict[str, Any]:
    """
    Get performance summary from experiment results
    
    Args:
        experiment_results: List of experiment results
    
    Returns:
        Performance summary dictionary
    """
    if not experiment_results:
        return {
            'total_experiments': 0,
            'successful_experiments': 0,
            'failed_experiments': 0,
            'average_performance': 0.0,
            'best_performance': 0.0,
            'worst_performance': 0.0,
        }
    
    successful = [r for r in experiment_results if r.get('success', True)]
    total = len(experiment_results)
    successful_count = len(successful)
    
    if successful:
        performances = [r.get('task_performance', 0.0) for r in successful]
        
        avg_performance = sum(performances) / len(performances)
        best_performance = max(performances)
        worst_performance = min(performances)
        
        # Calculate additional metrics
        durations = [r.get('duration', 0.0) for r in successful]
        neurons_per_second = [r.get('neurons_per_second', 0.0) for r in successful]
        
        summary = {
            'total_experiments': total,
            'successful_experiments': successful_count,
            'failed_experiments': total - successful_count,
            'average_performance': avg_performance,
            'best_performance': best_performance,
            'worst_performance': worst_performance,
            'performance_stddev': np.std(performances) if len(performances) > 1 else 0.0,
            'average_duration': sum(durations) / len(durations),
            'total_neurons_per_second': sum(neurons_per_second),
            'experiments': experiment_results,
        }
    else:
        summary = {
            'total_experiments': total,
            'successful_experiments': 0,
            'failed_experiments': total,
            'average_performance': 0.0,
            'best_performance': 0.0,
            'worst_performance': 0.0,
        }
    
    return summary

def save_results(results: List[Dict[str, Any]], filepath: str) -> None:
    """
    Save experiment results to file
    
    Args:
        results: List of experiment results
        filepath: File path to save results
    """
    with open(filepath, 'w') as f:
        json.dump({
            'timestamp': time.time(),
            'results': results,
        }, f, indent=2)

def load_results(filepath: str) -> List[Dict[str, Any]]:
    """
    Load experiment results from file
    
    Args:
        filepath: File path to load results
    
    Returns:
        List of experiment results
    """
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    return data.get('results', [])

def print_experiment_summary(summary: Dict[str, Any]) -> None:
    """
    Print experiment summary
    
    Args:
        summary: Experiment summary dictionary
    """
    print("=" * 60)
    print("NLM Experiment Summary")
    print("=" * 60)
    print(f"Total experiments: {summary['total_experiments']}")
    print(f"Successful experiments: {summary['successful_experiments']}")
    print(f"Failed experiments: {summary['failed_experiments']}")
    print(f"Average performance: {summary['average_performance']:.3f}")
    print(f"Best performance: {summary['best_performance']:.3f}")
    print(f"Worst performance: {summary['worst_performance']:.3f}")
    print(f"Average duration: {summary['average_duration']:.2f} seconds")
    print(f"Total neurons/sec: {summary['total_neurons_per_second']:.0f}")
    print("=" * 60)

# Export commonly used functions and classes
__all__ = [
    # Core functions
    'create_default_config',
    'get_config_template',
    'validate_config',
    'create_default_nlm',
    'run_multiple_experiments',
    'get_performance_summary',
    'save_results',
    'load_results',
    'print_experiment_summary',
    # Constants
    'TEMPLATES',
]

__version__ = "0.1.0"
