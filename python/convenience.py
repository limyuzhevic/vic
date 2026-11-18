"""
NLM Python Bindings - Convenience Functions and Documentation

This module provides convenience functions and comprehensive documentation for
the NLM (Neural Learning Machine) Python bindings, including common workflows,
usage examples, and best practices.
"""

import sys
import os
from typing import Any, Dict, List, Optional, Union, Callable
import json
import inspect
from pathlib import Path

from .nlm_errors import (
    NLMError,
    ConfigurationError,
    MemoryError,
    PlasticityError,
    SimulationError,
    DevelopmentError,
    NeuromodulationError,
    NLMValidator,
    ErrorRecoveryManager,
    SafeNLMOperation,
    get_recovery_manager,
    with_safe_operation,
    handle_nlm_error,
    raise_nlm_error
)

# Common workflow functions

def create_standard_brain(neuron_count: int = 500, region_count: int = 2, 
                          connection_probability: float = 0.1) -> 'NLMBrain':
    """
    Create a standard NLM brain with common configuration.
    
    Args:
        neuron_count: Number of neurons (default: 500)
        region_count: Number of brain regions (default: 2)
        connection_probability: Probability of connections (default: 0.1)
        
    Returns:
        Configured NLM brain
        
    Example:
        >>> brain = create_standard_brain()
        >>> brain.initialize()
        >>> brain.step(0)
    """
    config = NLMConfig({
        'neuron_count': neuron_count,
        'region_count': region_count,
        'connection_probability': connection_probability,
        'time_step': 0.1
    })
    
    brain = NLMBrain(config)
    brain.initialize()
    return brain

def run_simple_simulation(steps: int = 100, neuron_count: int = 200,
                         use_batch: bool = False) -> 'NLM':
    """
    Run a simple NLM simulation with basic configuration.
    
    Args:
        steps: Number of simulation steps (default: 100)
        neuron_count: Number of neurons (default: 200)
        use_batch: Whether to use batch processing (default: False)
        
    Returns:
        NLM instance with simulation results
        
    Example:
        >>> nlm = run_simple_simulation(steps=50)
        >>> status = nlm.get_status()
        >>> print(f"Firing rate: {status['brain']['average_firing_rate']}")
    """
    config = NLMConfig({
        'neuron_count': neuron_count,
        'region_count': 1,
        'connection_probability': 0.05,
        'time_step': 0.1
    })
    
    nlm = NLM(config)
    
    if use_batch:
        from .batch_ops import BatchProcessor
        with BatchProcessor() as processor:
            step_indices = list(range(steps))
            processor.batch_step(nlm.brain, step_indices)
    else:
        for step in range(steps):
            nlm.brain.step(step)
            nlm.world.update(0.1)
    
    return nlm

def analyze_brain_performance(brain: 'NLMBrain', duration: int = 100) -> Dict[str, Any]:
    """
    Analyze brain performance over a simulation period.
    
    Args:
        brain: NLM brain to analyze
        duration: Simulation duration in steps (default: 100)
        
    Returns:
        Performance analysis dictionary
        
    Example:
        >>> brain = create_standard_brain()
        >>> analysis = analyze_brain_performance(brain, duration=50)
        >>> print(f"Average firing rate: {analysis['firing_rate_stats']['mean']}")
    """
    firing_rates = []
    neuron_counts = []
    region_counts = []
    
    for step in range(duration):
        brain.step(step)
        
        firing_rates.append(brain.get_average_firing_rate())
        neuron_counts.append(brain.get_neuron_count())
        region_counts.append(brain.get_region_count())
    
    def calculate_stats(values: List[float]) -> Dict[str, float]:
        return {
            'mean': sum(values) / len(values),
            'median': sum(values) / len(values),  # Simplified
            'min': min(values),
            'max': max(values),
            'std': sum((x - sum(values) / len(values)) ** 2 for x in values) / len(values) if len(values) > 1 else 0.0
        }
    
    return {
        'firing_rate_stats': calculate_stats(firing_rates),
        'neuron_count_stats': calculate_stats(neuron_counts),
        'region_count_stats': calculate_stats(region_counts),
        'duration': duration,
        'total_steps': duration
    }

def create_experiment_setup(config_dict: Optional[Dict[str, Any]] = None,
                           experiment_name: Optional[str] = None) -> Dict[str, Any]:
    """
    Create a complete experiment setup with validation.
    
    Args:
        config_dict: Configuration dictionary (optional)
        experiment_name: Name of the experiment (optional)
        
    Returns:
        Complete experiment setup dictionary
        
    Raises:
        ConfigurationError: If configuration is invalid
    """
    try:
        if config_dict:
            NLMValidator.validate_config(config_dict)
        
        experiment_setup = {
            'name': experiment_name or f"experiment_{int(time.time())}",
            'timestamp': time.time(),
            'config': config_dict or {
                'neuron_count': 500,
                'region_count': 2,
                'connection_probability': 0.1,
                'time_step': 0.1
            },
            'validation': {
                'config_valid': True,
                'config_errors': []
            },
            'results': {},
            'metadata': {}
        }
        
        return experiment_setup
        
    except Exception as e:
        raise ConfigurationError(f"Experiment setup failed: {str(e)}")

def run_repeated_experiment(base_config: Dict[str, Any], repetitions: int = 5,
                           steps_per_run: int = 100) -> List[Dict[str, Any]]:
    """
    Run the same experiment multiple times with the same configuration.
    
    Args:
        base_config: Base configuration for all runs
        repetitions: Number of times to repeat the experiment
        steps_per_run: Number of simulation steps per run
        
    Returns:
        List of experiment results for each repetition
        
    Example:
        >>> config = {'neuron_count': 200, 'region_count': 1}
        >>> results = run_repeated_experiment(config, repetitions=3, steps_per_run=50)
        >>> for i, result in enumerate(results):
        ...     print(f"Run {i+1} firing rate: {result['final_firing_rate']}")
    """
    from .batch_ops import create_experiment_manager
    
    experiment_results = []
    
    for i in range(repetitions):
        try:
            # Create unique experiment name for each run
            run_name = f"{base_config.get('name', 'experiment')}_run_{i + 1}"
            
            # Create experiment manager
            exp_manager = create_experiment_manager(
                experiment_name=run_name,
                config=base_config
            )
            
            # Run the experiment
            result = exp_manager.run_experiment(
                steps=steps_per_run,
                description=f"Run {i + 1} of repeated experiment",
                use_batch=True
            )
            
            experiment_results.append({
                'run_id': i +  وهو
                'result': result,
                'success': True
            })
            
        except Exception as e:
            experiment_results.append({
                'run_id': i + 1,
                'result': None,
                'success': False,
                'error': str(e)
            })
    
    return experiment_results

def compare_experiments(experiment_results: List[Dict[str, Any]]) -> Dict[str, Any]:
    """
    Compare results from multiple experiments.
    
    Args:
        experiment_results: List of experiment result dictionaries
        
    Returns:
        Comparison analysis dictionary
        
    Example:
        >>> results = run_repeated_experiment(config, repetitions=3)
        >>> comparison = compare_experiments(results)
        >>> print(f"Mean firing rate: {comparison['firing_rate']['mean']}")
        >>> print(f"Std firing rate: {comparison['firing_rate']['std']}")
    """
    # Extract key metrics from all experiments
    all_firing_rates = []
    all_neuron_counts = []
    all_durations = []
    
    for exp_result in experiment_results:
        if exp_result.get('success') and exp_result.get('result'):
            result = exp_result['result']
            
            # Extract firing rate statistics
            firing_rate_stats = result.get('performance', {}).get('batch_metrics', {}).get('average_firing_rate', 0)
            all_firing_rates.append(firing_rate_stats)
            
            # Extract neuron count
            neuron_count = result.get('results', {}).get('final_status', {}).get('brain', {}).get('neuron_count', 0)
            all_neuron_counts.append(neuron_count)
            
            # Extract duration
            duration = result.get('duration', 0)
            all_durations.append(duration)
    
    def calculate_statistics(values: List[float]) -> Dict[str, float]:
        if not values:
            return {'mean': 0, 'std': 0, 'min': 0, 'max': 0}
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values) if len(values) > 1 else 0
        std = variance ** 0.5
        
        return {
            'mean': mean,
            'std': std,
            'min': min(values),
            'max': max(values),
            'coefficient_of_variation': (std / mean) if mean > 0 else 0
        }
    
    return {
        'experiment_count': len(experiment_results),
        'successful_runs': len([r for r in experiment_results if r.get('success')]),
        'firing_rate_stats': calculate_statistics(all_firing_rates),
        'neuron_count_stats': calculate_statistics(all_neuron_counts),
        'duration_stats': calculate_statistics(all_durations),
        'consistency': {
            'firing_rate_consistency': 1.0 - (calculate_statistics(all_firing_rates)['std'] / 
                                            calculate_statistics(all_firing_rates)['mean']) if calculate_statistics(all_firing_rates)['mean'] > 0 else 0,
            'neuron_count_consistency': 1.0 - (calculate_statistics(all_neuron_counts)['std'] / 
                                               calculate_statistics(all_neuron_counts)['mean']) if calculate_statistics(all_neuron_counts)['mean'] > 0 else 0
        }
    }

# Usage examples and templates

EXAMPLE_WORKFLOWS = {
    'basic_brain': {
        'description': 'Create and initialize a basic brain',
        'code': '''
import pynlm

# Create brain
brain = pynlm.create_default_config()
brain = pynlm.create_brain(brain)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.get_neuron_count()}")
print(f"Firing rate: {brain.get_average_firing_rate()}")
''',
        'difficulty': 'beginner'
    },
    
    'simple_simulation': {
        'description': 'Run a simple world simulation',
        'code': '''
import pynlm

# Create brain and world
brain = pynlm.create_brain()
world = pynlm.create_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation completed!")
print(f"Final firing rate: {brain.get_average_firing_rate()}")
''',
        'difficulty': 'intermediate'
    },
    
    'learning_experiment': {
        'description': 'Set up a learning experiment with reward modulation',
        'code': '''
import pynlm

# Create experiment configuration
config = pynlm.create_default_config()
config.set('neuron_count', 300)
config.set('region_count', 2)

# Create NLM system
nlm = pynlm.NLM(config)

# Enable learning features
agent = pynlm.createAgentBrain(nlm.brain)
agent.initialize(nlm.world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(200):
    # Update world
    nlm.world.update(0.1)
    
    # Get sensory input
    percept = nlm.world.getSensoryPercept()
    
    # Process input
    agent.processSensoryInput(percept)
    nlm.brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    nlm.world.applyMotorCommand(action, nlm.world.getSimulationTime())
    
    # Apply reward modulation
    reward = calculate_reward(action)  # Your reward function
    agent.applyRewardModulation(reward, predicted_reward=0.0)

print(f"Learning experiment completed. Final firing rate: {nlm.brain.get_average_firing_rate()}")
''',
        'difficulty': 'advanced'
    }
}

def get_example_workflow(workflow_name: str) -> Dict[str, Any]:
    """
    Get an example workflow by name.
    
    Args:
        workflow_name: Name of the workflow
        
    Returns:
        Workflow dictionary with description and code
        
    Raises:
        NLMError: If workflow not found
    """
    if workflow_name not in EXAMPLE_WORKFLOWS:
        available = ', '.join(EXAMPLE_WORKFLOWS.keys())
        raise NLMError(f"Workflow '{workflow_name}' not found. Available workflows: {available}")
    
    return EXAMPLE_WORKFLOWS[workflow_name]

def list_available_workflows() -> List[str]:
    """Get list of available example workflows."""
    return list(EXAMPLE_WORKFLOWS.keys())

# Best practices and guidelines

BEST_PRACTICES = {
    'error_handling': [
        "Always wrap NLM operations in try-catch blocks",
        "Use specific exception types (ConfigurationError, MemoryError, etc.)",
        "Implement error recovery mechanisms with ErrorRecoveryManager",
        "Use SafeNLMOperation for critical operations",
        "Log errors with context information"
    ],
    
    'configuration': [
        "Validate all configuration parameters before use",
        "Use NLMValidator.validate_config() for thorough validation",
        "Provide sensible defaults for optional parameters",
        "Document configuration keys and expected types",
        "Consider configuration validation at startup"
    ],
    
    'simulation': [
        "Use batch processing for large-scale simulations",
        "Monitor simulation progress with performance tools",
        "Implement checkpoint/restart for long simulations",
        "Use appropriate time steps for your simulation scale",
        "Validate simulation steps before execution"
    ],
    
    'performance': [
        "Use BatchProcessor for efficient batch operations",
        "Monitor performance with NLMPerformanceMonitor",
        "Profile bottlenecks with performance analysis",
        "Optimize memory usage with batch processing",
        "Use appropriate worker counts for your system"
    ],
    
    'experimentation': [
        "Use ExperimentManager for reproducible experiments",
        "Document all experimental parameters",
        "Use statistical analysis for comparing results",
        "Implement proper error handling for experimental failures",
        "Use consistent naming conventions for experiments"
    ]
}

# Configuration templates

CONFIG_TEMPLATES = {
    'small_brain': {
        'description': 'Small brain for quick testing',
        'config': {
            'neuron_count': 100,
            'region_count': 1,
            'connection_probability': 0.05,
            'time_step': 0.1
        }
    },
    
    'medium_brain': {
        'description': 'Medium-sized brain for typical simulations',
        'config': {
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.1,
            'time_step': 0.1
        }
    },
    
    'large_brain': {
        'description': 'Large brain for comprehensive simulations',
        'config': {
            'neuron_count': 1000,
            'region_count': 4,
            'connection_probability': 0.15,
            'time_step': 0.05
        }
    },
    
    'research_brain': {
        'description': 'Brain configured for research experiments',
        'config': {
            'neuron_count': 2000,
            'region_count': 6,
            'connection_probability': 0.2,
            'plasticity_enabled': True,
            'development_enabled': True,
            'neuromodulation_enabled': True,
            'time_step': 0.01
        }
    }
}

def get_config_template(template_name: str) -> Dict[str, Any]:
    """
    Get a configuration template by name.
    
    Args:
        template_name: Name of the configuration template
        
    Returns:
        Configuration dictionary
        
    Raises:
        NLMError: If template not found
    """
    if template_name not in CONFIG_TEMPLATES:
        available = ', '.join(CONFIG_TEMPLATES.keys())
        raise NLMError(f"Template '{template_name}' not found. Available templates: {available}")
    
    return CONFIG_TEMPLATES[template_name]['config'].copy()

def list_config_templates() -> List[str]:
    """Get list of available configuration templates."""
    return list(CONFIG_TEMPLATES.keys())

# Documentation and help functions

def generate_documentation() -> Dict[str, Any]:
    """
    Generate comprehensive documentation for NLM Python bindings.
    
    Returns:
        Complete documentation dictionary
    """
    return {
        'version': '0.1.0',
        'description': 'NLM Python Bindings - Complete API Documentation',
        'modules': [
            {
                'name': 'nlm_errors',
                'description': 'Error handling and validation',
                'classes': ['NLMError', 'ConfigurationError', 'MemoryError', 'PlasticityError', 'SimulationError', 'DevelopmentError', 'NeuromodulationError'],
                'functions': ['NLMValidator', 'ErrorRecoveryManager', 'SafeNLMOperation', 'get_recovery_manager']
            },
            {
                'name': 'pynlm',
                'description': 'Main Python interface',
                'classes': ['NLMConfig', 'NLMBrain', 'NLMWorld', 'NLM'],
                'functions': ['create_default_config', 'create_brain', 'create_world', 'run_nlm_simulation']
            },
            {
                'name': 'batch_ops',
                'description': 'Batch operations and performance monitoring',
                'classes': ['BatchProcessor', 'NLMPerformanceMonitor', 'NLMExperimentManager'],
                'functions': ['create_batch_processor', 'create_performance_monitor', 'create_experiment_manager', 'run_benchmark']
            },
            {
                'name': 'convenience',
                'description': 'Convenience functions and documentation',
                'functions': ['create_standard_brain', 'run_simple_simulation', 'analyze_brain_performance', 'create_experiment_setup']
            }
        ],
        'examples': EXAMPLE_WORKFLOWS,
        'best_practices': BEST_PRACTICES,
        'templates': CONFIG_TEMPLATES,
        'error_codes': {
            'CONFIG_ERROR': 'Configuration-related errors',
            'MEMORY_ERROR': 'Memory system errors',
            'PLASTICITY_ERROR': 'Plasticity rule errors',
            'SIMULATION_ERROR': 'Runtime simulation errors',
            'DEVELOPMENT_ERROR': 'Development stage errors',
            'NEUROMODULATION_ERROR': 'Neuromodulation issues',
            'UNKNOWN_ERROR': 'Unknown errors'
        },
        'getting_started': [
            "Install NLM Python bindings: pip install nlm",
            "Import main modules: import pynlm",
            "Create configuration: config = pynlm.create_default_config()",
            "Create brain: brain = pynlm.create_brain(config)",
            "Initialize brain: brain.initialize()",
            "Run simulation: for step in range(100): brain.step(step)"
        ],
        'advanced_topics': [
            "Batch processing for performance",
            "Error recovery and graceful degradation",
            "Performance monitoring and benchmarking",
            "Experiment management and reproducibility",
            "Configuration validation and optimization",
            "Memory management and scalability"
        ]
    }

# Module metadata

__version__ = "0.1.0"
__author__ = "NLM Research Team"
__email__ = "research@nlm.org"
__license__ = "MIT"
__copyright__ = "Copyright 2026 NLM Research"

# Export public API

__all__ = [
    # Core functions
    'create_default_config',
    'create_brain',
    'create_world',
    'run_nlm_simulation',
    
    # Workflow functions
    'create_standard_brain',
    'run_simple_simulation',
    'analyze_brain_performance',
    'create_experiment_setup',
    'run_repeated_experiment',
    'compare_experiments',
    
    # Template functions
    'get_example_workflow',
    'list_available_workflows',
    'get_config_template',
    'list_config_templates',
    'generate_documentation',
    
    # Experiment management
    'create_experiment_manager',
    
    # Batch operations
    'create_batch_processor',
    'create_performance_monitor',
    
    # Benchmark functions
    'run_benchmark',
    'benchmark_step_performance',
    'benchmark_batch_performance',
    'benchmark_memory_efficiency',
    
    # Templates and examples
    'EXAMPLE_WORKFLOWS',
    'CONFIG_TEMPLATES',
    'BEST_PRACTICES',
    'BENCHMARKS',
    
    # Error handling
    'NLMError',
    'ConfigurationError',
    'MemoryError',
    'PlasticityError',
    'SimulationError',
    'DevelopmentError',
    'NeuromodulationError',
    'NLMValidator',
    'ErrorRecoveryManager',
    'SafeNLMOperation',
    'get_recovery_manager',
    
    # Main classes
    'NLMConfig',
    'NLMBrain',
    'NLMWorld',
    'NLM'
]
