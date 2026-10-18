"""
Examples for the Enhanced NLM Python API

This module contains comprehensive examples demonstrating the enhanced features
of the NLM Python API, including configuration management, experiment running,
performance monitoring, and advanced usage patterns.
"""

import time
import numpy as np
from typing import Dict, List, Any
from .core import NLM, create_default_config, create_default_nlm
from .utils import (
    get_config_template,
    create_default_nlm,
    run_multiple_experiments,
    get_performance_summary,
    validate_config,
    save_results,
    load_results,
)

# ============================================================================
# Basic Usage Examples
# ============================================================================

def example_basic_usage():
    """Example 1: Basic NLM usage"""
    print("Example 1: Basic NLM Usage")
    print("-" * 40)
    
    # Create NLM with default configuration
    nlm = create_default_nlm()
    print(f"Created NLM: {nlm}")
    
    # Run a simple experiment
    result = nlm.run_experiment("simple_test", steps=1000)
    print(f"Experiment completed: {result['success']}")
    print(f"Duration: {result['duration']:.2f} seconds")
    print(f"Neurons/sec: {result['neurons_per_second']:,.0f}")
    print(f"Average firing rate: {result['avg_firing_rate_hz']:.2f} Hz")
    print()

def example_custom_configuration():
    """Example 2: Custom configuration"""
    print("Example 2: Custom Configuration")
    print("-" * 40)
    
    # Create custom configuration
    custom_config = {
        'neuron_count': 5000,
        'region_count': 3,
        'connection_probability': 0.15,
        'plasticity_learning_rate': 0.005,
        'synaptogenesis_rate': 0.0005,
        'pruning_rate': 0.0001,
        'enable_SIMD': True,
        'enable_multithreading': True,
    }
    
    # Validate configuration
    errors = validate_config(custom_config)
    if errors:
        print("Configuration errors:")
        for error in errors:
            print(f"  - {error}")
    else:
        print("Configuration is valid!")
    
    # Create NLM with custom configuration
    nlm = NLM(custom_config)
    print(f"Created NLM with custom config: {nlm}")
    
    # Run experiment
    result = nlm.run_experiment("custom_test", steps=2000)
    print(f"Custom experiment result: {result['success']}")
    print(f"Performance: {result['task_performance']:.3f}")
    print()

def example_configuration_templates():
    """Example 3: Configuration templates"""
    print("Example 3: Configuration Templates")
    print("-" * 40)
    
    # Available templates
    templates = ['small', 'medium', 'large', 'research', 'production']
    
    for template_name in templates:
        try:
            template = get_config_template(template_name)
            print(f"\nTemplate: {template_name}")
            print(f"  Neurons: {template['neuron_count']:,}")
            print(f"  Regions: {template['region_count']}")
            print(f"  Connection probability: {template['connection_probability']}")
            print(f"  Enable SIMD: {template['enable_SIMD']}")
            print(f"  Enable multithreading: {template['enable_multithreading']}")
        except ValueError as e:
            print(f"Error with template {template_name}: {e}")
    
    print()

def example_batch_experiments():
    """Example 4: Batch experiment execution"""
    print("Example 4: Batch Experiment Execution")
    print("-" * 40)
    
    # Define multiple experiments with different configurations
    experiments = [
        {
            'name': 'small_scale',
            'steps': 1000,
            'neurons': 100,
        },
        {
            'name': 'medium_scale',
            'steps': 2000,
            'neurons': 1000,
        },
        {
            'name': 'large_scale',
            'steps': 5000,
            'neurons': 10000,
        },
    ]
    
    # Create NLM with configuration
    config = get_config_template('medium')
    nlm = NLM(config)
    
    # Progress callback
    def progress_callback(current, total, name):
        print(f"  Progress: {current+1}/{total} - {name}")
    
    # Run batch experiments
    results = nlm.run_batch(experiments, on_progress=progress_callback)
    
    print(f"\nBatch experiment completed:")
    print(f"  Total experiments: {len(results)}")
    print(f"  Successful: {sum(1 for r in results if r.get('success', True))}")
    print(f"  Failed: {sum(1 for r in results if not r.get('success', True))}")
    
    # Get summary
    summary = get_performance_summary(results)
    print(f"  Average performance: {summary['average_performance']:.3f}")
    print(f"  Best performance: {summary['best_performance']:.3f}")
    print(f"  Worst performance: {summary['worst_performance']:.3f}")
    print()

def example_performance_monitoring():
    """Example 5: Performance monitoring"""
    print("Example 5: Performance Monitoring")
    print("-" * 40)
    
    # Create NLM
    nlm = create_default_nlm()
    
    # Define performance experiments
    experiments = [
        {'name': 'perf_test_1', 'steps': 1000},
        {'name': 'perf_test_2', 'steps': 2000},
        {'name': 'perf_test_3', 'steps': 3000},
    ]
    
    # Run experiments
    results = run_multiple_experiments(experiments, on_progress=lambda c, t, n: print(f"  {c+1}/{t}: {n}"))
    
    # Get performance summary
    summary = get_performance_summary(results)
    
    print(f"\nPerformance Summary:")
    print(f"  Average performance: {summary['average_performance']:.3f}")
    print(f"  Performance standard deviation: {summary['performance_stddev']:.3f}")
    print(f"  Best performance: {summary['best_performance']:.3f}")
    print(f"  Worst performance: {summary['worst_performance']:.3f}")
    print(f"  Total neurons/sec: {summary['total_neurons_per_second']:,.0f}")
    
    # Save results
    save_results(results, 'experiment_results.json')
    print(f"\nResults saved to 'experiment_results.json'")
    
    # Load results
    loaded_results = load_results('experiment_results.json')
    print(f"Loaded {len(loaded_results)} results from file")
    print()

def example_configuration_validation():
    """Example 6: Configuration validation"""
    print("Example 6: Configuration Validation")
    print("-" * 40)
    
    # Valid configuration
    valid_config = {
        'neuron_count': 1000,
        'region_count': 2,
        'simulation_timestep': 0.001,
        'connection_probability': 0.1,
    }
    
    errors = validate_config(valid_config)
    if not errors:
        print("✓ Valid configuration")
    else:
        print("✗ Configuration errors:")
        for error in errors:
            print(f"  - {error}")
    
    # Invalid configuration
    invalid_config = {
        'neuron_count': -100,  # Invalid
        'region_count': 0,     # Invalid
        'connection_probability': 1.5,  # Invalid
    }
    
    errors = validate_config(invalid_config)
    if not errors:
        print("✓ Invalid configuration is valid (unexpected)")
    else:
        print("✗ Invalid configuration (expected):")
        for error in errors:
            print(f"  - {error}")
    print()

def example_advanced_features():
    """Example 7: Advanced features"""
    print("Example 7: Advanced Features")
    print("-" * 40)
    
    # Create NLM with advanced configuration
    advanced_config = {
        'neuron_count': 50000,
        'region_count': 10,
        'enable_SIMD': True,
        'enable_multithreading': True,
        'enable_sparse_connectivity': True,
        'enable_event_driven': True,
        'plasticity_learning_rate': 0.001,
        'synaptogenesis_rate': 0.0001,
        'pruning_rate': 0.00001,
    }
    
    nlm = NLM(advanced_config)
    print(f"Created advanced NLM: {nlm}")
    
    # Run complex experiment
    result = nlm.run_experiment("advanced_test", steps=10000)
    
    print(f"\nAdvanced experiment results:")
    print(f"  Success: {result['success']}")
    print(f"  Duration: {result['duration']:.2f} seconds")
    print(f"  Neurons/sec: {result['neurons_per_second']:,.0f}")
    print(f"  Task performance: {result['task_performance']:.3f}")
    print(f"  Learning accuracy: {result['learning_accuracy']:.3f}")
    print(f"  Memory retention: {result['memory_retention']:.3f}")
    
    # Save detailed results
    save_results([result], 'advanced_experiment_results.json')
    print(f"\nAdvanced results saved to 'advanced_experiment_results.json'")
    print()

def example_data_analysis():
    """Example 8: Data analysis and visualization (simulated)"""
    print("Example 8: Data Analysis and Visualization (Simulated)")
    print("-" * 40)
    
    # Load saved results
    try:
        results = load_results('experiment_results.json')
        print(f"Loaded {len(results)} experiment results")
        
        if results:
            # Create performance summary
            summary = get_performance_summary(results)
            
            print(f"\nData Analysis Summary:")
            print(f"  Experiments analyzed: {summary['total_experiments']}")
            print(f"  Success rate: {summary['successful_experiments']/summary['total_experiments']*100:.1f}%")
            print(f"  Average performance: {summary['average_performance']:.3f}")
            print(f"  Performance stddev: {summary['performance_stddev']:.3f}")
            print(f"  Performance coefficient of variation: {summary['performance_stddev']/summary['average_performance']*100:.1f}%")
            
            # Identify best and worst performers
            if 'experiments' in summary:
                exp_results = summary['experiments']
                if exp_results:
                    best = max(exp_results, key=lambda x: x.get('task_performance', 0))
                    worst = min(exp_results, key=lambda x: x.get('task_performance', 1))
                    
                    print(f"\nBest performer: {best.get('name', 'Unknown')}")
                    print(f"  Performance: {best.get('task_performance', 0):.3f}")
                    print(f"  Duration: {best.get('duration', 0):.2f}s")
                    
                    print(f"\nWorst performer: {worst.get('name', 'Unknown')}")
                    print(f"  Performance: {worst.get('task_performance', 0):.3f}")
                    print(f"  Duration: {worst.get('duration', 0):.2f}s")
    
    except FileNotFoundError:
        print("No experiment results file found. Run some experiments first.")
    
    print()

# ============================================================================
# Main execution
# ============================================================================

def run_all_examples():
    """Run all examples"""
    print("=" * 60)
    print("NLM Enhanced Python API Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_basic_usage,
        example_custom_configuration,
        example_configuration_templates,
        example_batch_experiments,
        example_performance_monitoring,
        example_configuration_validation,
        example_advanced_features,
        example_data_analysis,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {str(e)}")
        print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)

# Example usage when run as script
if __name__ == "__main__":
    run_all_examples()
