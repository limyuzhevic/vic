#!/usr/bin/env python3
"""
Demonstration script for the Enhanced NLM Python API

This script demonstrates the key features and capabilities of the enhanced
Python API for NLM (Neural Learning Machine).
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'pynlm'))

from pynlm import create_default_nlm, NLM
from pynlm.utils import (
    get_config_template,
    create_default_nlm,
    run_multiple_experiments,
    get_performance_summary,
    validate_config,
    save_results,
    load_results,
)
from pynlm.core import NLM as CoreNLM

def demo_basic_usage():
    """Demonstrate basic NLM usage"""
    print("=" * 60)
    print("Demo 1: Basic NLM Usage")
    print("=" * 60)
    
    # Create NLM with default configuration
    nlm = create_default_nlm()
    print(f"Created NLM: {nlm}")
    
    # Run a simple experiment
    result = nlm.run_experiment("simple_test", steps=1000)
    print(f"\nExperiment Results:")
    print(f"  Success: {result['success']}")
    print(f"  Duration: {result['duration']:.2f} seconds")
    print(f"  Neurons/sec: {result['neurons_per_second']:,.0f}")
    print(f"  Average firing rate: {result['avg_firing_rate_hz']:.2f} Hz")
    print(f"  Task performance: {result['task_performance']:.3f}")
    print(f"  Learning accuracy: {result['learning_accuracy']:.3f}")
    
    print("\n" + "=" * 60)
    return True

def demo_configuration_templates():
    """Demonstrate configuration templates"""
    print("Demo 2: Configuration Templates")
    print("=" * 60)
    
    templates = ['small', 'medium', 'large', 'research', 'production']
    
    for template_name in templates:
        try:
            config = get_config_template(template_name)
            print(f"\nTemplate: {template_name.upper()}")
            print(f"  Neurons: {config['neuron_count']:,}")
            print(f"  Regions: {config['region_count']}")
            print(f"  Connection probability: {config['connection_probability']}")
            print(f"  Enable SIMD: {config['enable_SIMD']}")
            print(f"  Enable multithreading: {config['enable_multithreading']}")
            
            # Create NLM with template
            nlm = create_default_nlm(config)
            result = nlm.run_experiment(f"{template_name}_test", steps=1000)
            print(f"  Performance: {result['task_performance']:.3f}")
            
        except ValueError as e:
            print(f"  Error: {e}")
    
    print("\n" + "=" * 60)
    return True

def demo_configuration_validation():
    """Demonstrate configuration validation"""
    print("Demo 3: Configuration Validation")
    print("=" * 60)
    
    # Valid configuration
    valid_config = {
        'neuron_count': 1000,
        'region_count': 2,
        'simulation_timestep': 0.001,
        'connection_probability': 0.1,
    }
    
    print("\nValid configuration:")
    errors = validate_config(valid_config)
    if not errors:
        print("  ✓ Configuration is valid")
    else:
        print("  ✗ Configuration errors:")
        for error in errors:
            print(f"    - {error}")
    
    # Invalid configuration
    invalid_config = {
        'neuron_count': -100,  # Invalid
        'region_count': 0,     # Invalid
        'connection_probability': 1.5,  # Invalid
    }
    
    print("\nInvalid configuration:")
    errors = validate_config(invalid_config)
    if not errors:
        print("  ✓ Invalid configuration is valid (unexpected)")
    else:
        print("  ✗ Configuration validation errors (expected):")
        for error in errors:
            print(f"    - {error}")
    
    print("\n" + "=" * 60)
    return True

def demo_batch_experiments():
    """Demonstrate batch experiment execution"""
    print("Demo 4: Batch Experiment Execution")
    print("=" * 60)
    
    # Define multiple experiments
    experiments = [
        {'name': 'small_scale', 'steps': 1000, 'neurons': 100},
        {'name': 'medium_scale', 'steps': 2000, 'neurons': 1000},
        {'name': 'large_scale', 'steps': 5000, 'neurons': 10000},
    ]
    
    # Progress callback
    def progress_callback(current, total, name):
        print(f"  Processing: {current+1}/{total} - {name}")
    
    print("\nRunning batch experiments...")
    
    # Create NLM with configuration
    config = get_config_template('medium')
    nlm = NLM(config)
    
    # Run batch experiments
    results = nlm.run_batch(experiments, on_progress=progress_callback)
    
    print(f"\nBatch experiment completed:")
    print(f"  Total experiments: {len(results)}")
    print(f"  Successful: {sum(1 for r in results if r.get('success', True))}")
    print(f"  Failed: {sum(1 for r in results if not r.get('success', True))}")
    
    # Get performance summary
    summary = get_performance_summary(results)
    print(f"\nPerformance Summary:")
    print(f"  Average performance: {summary['average_performance']:.3f}")
    print(f"  Best performance: {summary['best_performance']:.3f}")
    print(f"  Worst performance: {summary['worst_performance']:.3f}")
    print(f"  Performance standard deviation: {summary['performance_stddev']:.3f}")
    
    # Save results
    save_results(results, 'demo_results.json')
    print(f"\nResults saved to 'demo_results.json'")
    
    # Load results
    loaded_results = load_results('demo_results.json')
    print(f"Loaded {len(loaded_results)} results from file")
    
    print("\n" + "=" * 60)
    return True

def demo_performance_monitoring():
    """Demonstrate performance monitoring"""
    print("Demo 5: Performance Monitoring")
    print("=" * 60)
    
    # Create NLM with custom configuration
    config = {
        'neuron_count': 5000,
        'region_count': 3,
        'plasticity_learning_rate': 0.005,
        'synaptogenesis_rate': 0.0005,
        'enable_SIMD': True,
        'enable_multithreading': True,
    }
    
    nlm = NLM(config)
    print(f"Created NLM with custom configuration")
    print(f"  Neurons: {config['neuron_count']:,}")
    print(f"  Regions: {config['region_count']}")
    
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
    
    print("\n" + "=" * 60)
    return True

def demo_advanced_configuration():
    """Demonstrate advanced configuration"""
    print("Demo 6: Advanced Configuration")
    print("=" * 60)
    
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
    print(f"  Neurons: {advanced_config['neuron_count']:,}")
    print(f"  Regions: {advanced_config['region_count']}")
    print(f"  Enable SIMD: {advanced_config['enable_SIMD']}")
    print(f"  Enable multithreading: {advanced_config['enable_multithreading']}")
    print(f"  Enable sparse connectivity: {advanced_config['enable_sparse_connectivity']}")
    print(f"  Enable event driven: {advanced_config['enable_event_driven']}")
    
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
    save_results([result], 'advanced_demo_results.json')
    print(f"\nAdvanced results saved to 'advanced_demo_results.json'")
    
    print("\n" + "=" * 60)
    return True

def demo_file_operations():
    """Demonstrate file operations"""
    print("Demo 7: File Operations")
    print("=" * 60)
    
    # Create sample results
    sample_results = [
        {'name': 'test_1', 'success': True, 'task_performance': 0.85, 'duration': 1.5},
        {'name': 'test_2', 'success': True, 'task_performance': 0.92, 'duration': 2.3},
        {'name': 'test_3', 'success': False, 'task_performance': 0.0, 'duration': 0.0, 'error': 'Memory error'},
    ]
    
    # Save results
    save_results(sample_results, 'sample_results.json')
    print(f"Saved {len(sample_results)} results to 'sample_results.json'")
    
    # Load results
    loaded_results = load_results('sample_results.json')
    print(f"Loaded {len(loaded_results)} results from file")
    
    # Display results
    print(f"\nLoaded results:")
    for i, result in enumerate(loaded_results):
        status = "✓" if result.get('success', True) else "✗"
        print(f"  {i+1}. {result['name']}: {status} Performance: {result.get('task_performance', 0):.3f}")
    
    # Get performance summary
    summary = get_performance_summary(loaded_results)
    print(f"\nPerformance Summary:")
    print(f"  Total experiments: {summary['total_experiments']}")
    print(f"  Successful experiments: {summary['successful_experiments']}")
    print(f"  Average performance: {summary['average_performance']:.3f}")
    
    print("\n" + "=" * 60)
    return True

def run_all_demos():
    """Run all demonstrations"""
    print("=" * 60)
    print("NLM Enhanced Python API Demo")
    print("=" * 60)
    print("\nThis script demonstrates the key features and capabilities")
    print("of the enhanced Python API for NLM (Neural Learning Machine).")
    print()
    
    demos = [
        ("Basic Usage", demo_basic_usage),
        ("Configuration Templates", demo_configuration_templates),
        ("Configuration Validation", demo_configuration_validation),
        ("Batch Experiments", demo_batch_experiments),
        ("Performance Monitoring", demo_performance_monitoring),
        ("Advanced Configuration", demo_advanced_configuration),
        ("File Operations", demo_file_operations),
    ]
    
    results = []
    for name, demo_func in demos:
        try:
            print(f"\n{'=' * 60}")
            print(f"Running: {name}")
            print(f"{'=' * 60}")
            
            success = demo_func()
            results.append((name, success))
            
        except Exception as e:
            print(f"Error in {name}: {str(e)}")
            results.append((name, False))
    
    # Summary
    print(f"\n{'=' * 60}")
    print("Demo Summary")
    print(f"{'=' * 60}")
    
    successful_demos = [r for r, s in results if s]
    total_demos = len(results)
    
    print(f"Total demos: {total_demos}")
    print(f"Successful demos: {len(successful_demos)}")
    print(f"Failed demos: {total_demos - len(successful_demos)}")
    
    if len(successful_demos) == total_demos:
        print(f"\n🎉 All demos completed successfully!")
    else:
        print(f"\n⚠️  Some demos failed. Check output for details.")
    
    print(f"\n{'=' * 60}")

if __name__ == "__main__":
    run_all_demos()
