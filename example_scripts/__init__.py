#!/usr/bin/env python3
"""
Example Scripts for NLM Advanced Features

This directory contains example scripts demonstrating various use cases
of the NLM advanced features and utilities.

Each example script showcases different aspects of the enhanced NLM API,
from basic usage to advanced experiments and analysis.

Usage:
    Run individual examples with:
        python example_scripts/example_1_basic_enhanced.py

Available examples:
    - example_1_basic_enhanced.py: Basic enhanced NLM usage
    - example_2_advanced_learning.py: Advanced learning experiments
    - example_3_neural_plasticity.py: Neural plasticity analysis
    - example_4_multi_agent.py: Multi-agent simulation
    - example_5_performance.py: Performance benchmarking
    - example_6_optimization.py: Configuration optimization
    - example_7_monitoring.py: Real-time monitoring
    - example_8_experimental_design.py: Experimental design
"""

import sys

__all__ = [
    'example_1_basic_enhanced',
    'example_2_advanced_learning', 
    'example_3_neural_plasticity',
    'example_4_multi_agent',
    'example_5_performance',
    'example_6_optimization',
    'example_7_monitoring',
    'example_8_experimental_design'
]

# Import example modules
from .example_1_basic_enhanced import example_basic_usage, example_configuration_validation, example_analysis_tools
from .example_2_advanced_learning import example_advanced_learning, example_configuration_optimization, example_performance_benchmarking
from .example_3_neural_plasticity import example_plasticity_analysis, example_plasticity_visualization, example_comparison_studies
from .example_4_multi_agent import example_multi_agent_simulation, example_social_learning_analysis
from .example_5_performance import example_performance_benchmarking
from .example_6_optimization import example_configuration_optimization, example_parameter_sensitivity
from .example_7_monitoring import example_realtime_monitoring_example, example_monitoring_visualization
from .example_8_experimental_design import example_experimental_design, example_power_analysis, example_reproducible_research

# Convenience functions for running all examples

def run_all_examples():
    """Run all example scripts."""
    examples = [
        ('Example 1: Basic Enhanced Usage', example_1_basic_enhanced),
        ('Example 2: Advanced Learning Experiment', example_2_advanced_learning),
        ('Example 3: Neural Plasticity Analysis', example_3_neural_plasticity),
        ('Example 4: Multi-Agent Simulation', example_4_multi_agent),
        ('Example 5: Performance Benchmarking', example_5_performance),
        ('Example 6: Configuration Optimization', example_6_optimization),
        ('Example 7: Real-Time Monitoring', example_7_monitoring),
        ('Example 8: Experimental Design', example_8_experimental_design)
    ]
    
    results = {}
    for name, module in examples:
        print(f"\n{'='*60}")
        print(f"Running {name}")
        print(f"{'='*60}")
        
        try:
            if name == 'Example 1: Basic Enhanced Usage':
                results[name] = example_basic_usage()
            elif name == 'Example 2: Advanced Learning Experiment':
                results[name] = example_advanced_learning()
            elif name == 'Example 3: Neural Plasticity Analysis':
                results[name] = example_plasticity_analysis()
            elif name == 'Example 4: Multi-Agent Simulation':
                results[name] = example_multi_agent_simulation()
            elif name == 'Example 5: Performance Benchmarking':
                results[name] = example_performance_benchmarking()
            elif name == 'Example 6: Configuration Optimization':
                results[name] = example_configuration_optimization()
            elif name == 'Example 7: Real-Time Monitoring':
                results[name] = example_realtime_monitoring_example()
            elif name == 'Example 8: Experimental Design':
                results[name] = example_experimental_design()
                
            print(f"✓ {name} completed successfully")
        except Exception as e:
            print(f"✗ {name} failed: {e}")
            results[name] = {'error': str(e)}
    
    return results

def list_examples():
    """List all available examples."""
    examples = [
        ('example_1_basic_enhanced.py', 'Basic enhanced NLM usage and analysis'),
        ('example_2_advanced_learning.py', 'Advanced learning experiments with curiosity and development'),
        ('example_3_neural_plasticity.py', 'Neural plasticity analysis and visualization'),
        ('example_4_multi_agent.py', 'Multi-agent simulation with social learning'),
        ('example_5_performance.py', 'Performance benchmarking and optimization'),
        ('example_6_optimization.py', 'Configuration optimization and sensitivity analysis'),
        ('example_7_monitoring.py', 'Real-time monitoring and visualization'),
        ('example_8_experimental_design.py', 'Experimental design and power analysis')
    ]
    
    print("Available NLM Advanced Feature Examples:")
    print("=" * 50)
    for filename, description in examples:
        print(f"{filename}")
        print(f"  {description}")
        print()
    
    print("To run an example, use:")
    print("  python example_scripts/example_1_basic_enhanced.py")
    print()
    print("To run all examples:")
    print("  python -c 'from example_scripts import run_all_examples; run_all_examples()'")

if __name__ == "__main__":
    print("NLM Advanced Features - Example Scripts")
    print("=" * 50)
    
    # Check if user wants to run all examples or list them
    if len(sys.argv) > 1 and sys.argv[1] == '--all':
        run_all_examples()
    else:
        list_examples()
