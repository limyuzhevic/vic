#!/usr/bin/env python3
"""
Advanced NLM Example Scripts for Power Users

This file contains comprehensive example scripts demonstrating advanced
NLM usage patterns for experienced users and researchers.
"""

import sys
import time
import numpy as np
import tempfile
import os
from typing import List, Dict, Any, Optional

# Import NLM Python bindings and advanced utilities
try:
    import pynlm
    from nlm_utils.nlm_advanced import NLMExperiment, NLMPerformanceProfiler, NLMPythonAPI
    from nlm_utils.nlm_visualization import NLMDataPlotter, NLMPlotlyVisualizer, NLMVisualReporter
    ADVANCED_UTILS_AVAILABLE = True
except ImportError:
    ADVANCED_UTILS_AVAILABLE = False
    print("Warning: Advanced utilities not available. Using basic NLM only.")

def example_1_advanced_experiment_manager():
    """
    Example 1: Advanced Experiment Manager with comprehensive control
    
    Demonstrates the NLMExperiment class for managing complex experiments
    with checkpointing, metrics tracking, and advanced configuration.
    """
    print("=" * 60)
    print("EXAMPLE 1: Advanced Experiment Manager")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 1.")
        return
    
    # Create experiment with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    experiment = NLMExperiment(config)
    
    # Setup advanced experiment environment
    experiment.setup_experiment(
        width=30, height=30,
        vision_width=10, vision_height=10,
        enable_all_subsystems=True
    )
    
    print(f"Experiment ID: {experiment.experiment_id}")
    print(f"World size: {experiment.world.getWidth()}x{experiment.world.getHeight()}")
    print(f"Brain neurons: {experiment.brain.getTotalNeuronCount()}")
    
    # Run multiple episodes with different strategies
    episode_configs = [
        {'num_steps': 100, 'reward_strategy': 'constant', 'enable_logging': True},
        {'num_steps': 200, 'reward_strategy': 'adaptive', 'enable_logging': True},
        {'num_steps': 150, 'reward_strategy': 'curiosity', 'enable_logging': True},
    ]
    
    results = experiment.run_experiment_sequence(episode_configs)
    
    print(f"\nExperiment Results:")
    print(f"Total episodes: {results['total_episodes']}")
    print(f"Successful episodes: {results['successful_episodes']}")
    print(f"Average performance: {results['average_performance']}")
    
    # Create checkpoint
    checkpoint_path = experiment.create_checkpoint()
    print(f"\nCheckpoint created: {checkpoint_path}")
    
    # Get performance metrics
    metrics = experiment.get_performance_metrics()
    print(f"\nPerformance Metrics:")
    for key, value in metrics.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    return experiment

def example_2_performance_profiling():
    """
    Example 2: Performance Profiling with NLMPerformanceProfiler
    
    Demonstrates comprehensive performance profiling of NLM operations.
    """
    print("\n" + "=" * 60)
    print("EXAMPLE 2: Performance Profiling")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 2.")
        return
    
    # Create performance profiler
    profiler = NLMPerformanceProfiler()
    
    # Create brain for testing
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Profile basic operations
    print("Profiling basic operations...")
    step_profile = profiler.profile_brain_operations(brain, 'step', 50)
    print(f"Step operations: {step_profile['ops_per_second']:.0f} ops/sec")
    print(f"Mean step time: {step_profile['mean_time']*1000:.2f} ms")
    
    # Profile statistics operations
    stats_profile = profiler.profile_brain_operations(brain, 'statistics', 50)
    print(f"Statistics operations: {stats_profile['ops_per_second']:.0f} ops/sec")
    
    # Profile brain implementation comparison
    print("\nProfiling brain implementations...")
    configs = [config]
    
    # Add different configurations
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 500)
    config2.set("connection_probability", 0.05)
    configs.append(config2)
    
    comparison = profiler.compare_brain_implementations(configs, {'steps': 100})
    print(f"Best configuration: {comparison['best_configuration']['steps_per_second']:.0f} steps/sec")
    
    # Create visualization of results
    try:
        plotter = NLMDataPlotter()
        fig = plotter.plot_performance_benchmarks(
            {'basic_operations': {'step': step_profile, 'statistics': stats_profile},
             'agent_world_simulation': profiler.profiles.get('agent_world_simulation', {}),
             'brain_implementation_comparison': comparison}
        )
        plt.savefig('benchmark_results.png', dpi=300, bbox_inches='tight')
        print(f"\nBenchmark visualization saved as 'benchmark_results.png'")
    except Exception as e:
        print(f"Visualization generation failed: {e}")
    
    return profiler

def example_3_advanced_visualization():
    """
    Example 3: Advanced Visualization with Multiple Backends
    
    Demonstrates advanced visualization capabilities using different backends.
    """
    print("\n" + "=" * 60)
    print("EXAMPLE 3: Advanced Visualization")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 3.")
        return
    
    # Setup experiment
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Create data plotter
    plotter = NLMDataPlotter(style='scientific')
    
    # Generate neural dynamics plot
    print("Generating neural dynamics plot...")
    fig1 = plotter.plot_neuron_dynamics(brain, save_path='neuron_dynamics.png', show_plot=False)
    print("  - Saved neuron_dynamics.png")
    
    # Generate agent simulation plot
    print("Generating agent simulation plot...")
    fig2 = plotter.plot_agent_simulation(brain, world, agent, num_steps=50, 
                                        save_path='agent_simulation.png', show_plot=False)
    print("  - Saved agent_simulation.png")
    
    # Create experimental comparison plot
    print("Generating experimental comparison plot...")
    experiment_results = [
        {'experiment_id': 'exp_1', 'steps': 100, 'total_spikes': 250, 
         'average_firing_rate': 2.5, 'duration': 1.2, 'curiosity_level': 0.8},
        {'experiment_id': 'exp_2', 'steps': 150, 'total_spikes': 380,
         'average_firing_rate': 2.5, 'duration': 1.8, 'curiosity_level': 0.6},
        {'experiment_id': 'exp_3', 'steps': 200, 'total_spikes': 520,
         'average_firing_rate': 2.6, 'duration': 2.5, 'curiosity_level': 0.9},
    ]
    
    fig3 = plotter.plot_brain_metrics_comparison(experiment_results, 'total_spikes',
                                                save_path='experiment_comparison.png', show_path=False)
    print("  - Saved experiment_comparison.png")
    
    # Create interactive plotly visualization
    if 'plotly' in sys.modules:
        print("Creating interactive plotly visualization...")
        visualizer = NLMPlotlyVisualizer()
        
        # Interactive neural plot
        interactive_fig = visualizer.create_interactive_neural_plot(brain, 100)
        interactive_fig.write_html('interactive_neural_plot.html')
        print("  - Saved interactive_neural_plot.html")
        
        # Interactive agent trajectory plot
        interactive_agent_fig = visualizer.create_agent_trajectory_plot(brain, world, agent, 50)
        interactive_agent_fig.write_html('interactive_agent_trajectory.html')
        print("  - Saved interactive_agent_trajectory.html")
    
    # Create text-based plot (terminal output)
    if 'plotext' in sys.modules:
        print("Creating text-based plot...")
        txt_plotter = NLMTxtPlotter()
        txt_output = txt_plotter.plot_neuron_dynamics_text(brain, 30)
        print("  Text plot generated (first 500 chars):")
        print(txt_output[:500] + "...")
    
    return plotter, world, agent

def example_4_experiment_reporting():
    """
    Example 4: Comprehensive Experiment Reporting
    
    Demonstrates the NLMVisualReporter for creating comprehensive experiment reports.
    """
    print("\n" + "=" * 60)
    print("EXAMPLE 4: Comprehensive Experiment Reporting")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 4.")
        return
    
    # Create visual reporter
    reporter = NLMVisualReporter()
    
    # Setup experiment
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Run experiment to generate data
    experiment = NLMExperiment(config)
    experiment.setup_experiment()
    
    # Run episode to generate data for report
    results = experiment.run_episode(50, reward_strategy='adaptive', enable_logging=True)
    
    # Create comprehensive report
    print("Creating comprehensive experiment report...")
    report_path = reporter.create_experiment_report(
        experiment_name="Advanced_NLM_Experiment",
        experiment_results=[results],
        brain=brain,
        world=world,
        agent=agent
    )
    
    print(f"Report created: {report_path}")
    print(f"Report includes: Experiment overview, detailed results, brain configuration, agent state, world configuration")
    
    # List all reports
    print(f"\nTotal reports created: {len(reporter.report_files)}")
    for i, report_file in enumerate(reporter.report_files, 1):
        print(f"  {i}. {report_file}")
    
    return reporter

def example_5_reproducible_research():
    """
    Example 5: Reproducible Research Setup
    
    Demonstrates how to setup reproducible research with fixed seeds and
    comprehensive documentation.
    """
    print("\n" + "=" * 60)
    print("EXAMPLE 5: Reproducible Research")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 5.")
        return
    
    # Create reproducible experiment with fixed seed
    experiment = NLMPythonAPI.create_reproducible_experiment(seed=42)
    
    print(f"Reproducible experiment created with seed: 42")
    print(f"Experiment ID: {experiment.experiment_id}")
    
    # Setup experiment
    experiment.setup_experiment(width=25, height=25, enable_all_subsystems=True)
    
    # Run experiment with logging
    results = experiment.run_episode(
        num_steps=100,
        reward_strategy='adaptive',
        enable_logging=True
    )
    
    print(f"\nExperiment completed successfully:")
    print(f"  Experiment ID: {results['experiment_id']}")
    print(f"  Steps: {results['steps']}")
    print(f"  Total Spikes: {results['total_spikes']}")
    print(f"  Duration: {results['duration']:.3f} seconds")
    print(f"  Average Firing Rate: {results['average_firing_rate']:.3f} Hz")
    print(f"  Final Curiosity Level: {results['curiosity_level']:.3f}")
    
    # Create checkpoint for reproducibility
    checkpoint_file = experiment.create_checkpoint()
    print(f"\nCheckpoint created: {checkpoint_file}")
    print("  This checkpoint can be used to reproduce the experiment exactly")
    
    # Validate installation
    print("\nValidating NLM installation...")
    validation = NLMPythonAPI.validate_installation()
    
    if validation['errors']:
        print(f"  WARNING: Installation validation issues: {validation['errors']}")
    else:
        print("  Installation validation passed successfully")
    
    return experiment

def example_6_parallel_research():
    """
    Example 6: Parallel Research Experiments
    
    Demonstrates parallel execution of multiple experiments for research scalability.
    """
    print("\n" + "=" * 60)
    print("EXAMPLE 6: Parallel Research Experiments")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 6.")
        return
    
    # Define multiple experiment configurations
    experiment_configs = [
        {
            'width': 20,
            'height': 20,
            'num_steps': 100,
            'reward_strategy': 'constant',
            'enable_subsystems': True,
            'episode_config': {'num_steps': 100, 'reward_strategy': 'constant'}
        },
        {
            'width': 30,
            'height': 30,
            'num_steps': 150,
            'reward_strategy': 'adaptive',
            'enable_subsystems': True,
            'episode_config': {'num_steps': 150, 'reward_strategy': 'adaptive'}
        },
        {
            'width': 15,
            'height': 15,
            'num_steps': 80,
            'reward_strategy': 'curiosity',
            'enable_subsystems': True,
            'episode_config': {'num_steps': 80, 'reward_strategy': 'curiosity'}
        },
        {
            'width': 40,
            'height': 40,
            'num_steps': 120,
            'reward_strategy': 'adaptive',
            'enable_subsystems': False,  # Test with minimal subsystems
            'episode_config': {'num_steps': 120, 'reward_strategy': 'adaptive'}
        },
        {
            'width': 25,
            'height': 25,
            'num_steps': 90,
            'reward_strategy': 'curiosity',
            'enable_subsystems': True,
            'episode_config': {'num_steps': 90, 'reward_strategy': 'curiosity'}
        }
    ]
    
    print(f"Running {len(experiment_configs)} parallel experiments...")
    print(f"Max parallel workers: 4")
    
    # Run parallel experiments
    parallel_results = NLMPythonAPI.create_parallel_experiment(
        experiment_configs,
        max_workers=4
    )
    
    print(f"\nParallel Experiment Results:")
    print(f"  Total experiments: {parallel_results['total_experiments']}")
    print(f"  Successful experiments: {parallel_results['successful_experiments']}")
    print(f"  Failed experiments: {parallel_results['failed_experiments']}")
    print(f"  Parallel execution time: {parallel_results['parallel_execution_time']:.2f} seconds")
    
    # Analyze results
    successful_results = [
        exp for exp in parallel_results['experiment_results'] 
        if exp['success']
    ]
    
    if successful_results:
        print(f"\nTop performing experiments:")
        # Sort by total spikes (simplified metric)
        sorted_results = sorted(successful_results, 
                              key=lambda x: x['results'].get('total_spikes', 0), 
                              reverse=True)
        
        for i, result in enumerate(sorted_results[:3], 1):
            exp_id = result['experiment_id']
            total_spikes = result['results'].get('total_spikes', 0)
            print(f"  {i}. {exp_id}: {total_spikes} total spikes")
    
    # Save parallel results
    import json
    with open('parallel_experiment_results.json', 'w') as f:
        json.dump(parallel_results, f, indent=2, default=str)
    
    print(f"\nResults saved to 'parallel_experiment_results.json'")
    
    return parallel_results

def example_7_standard_benchmark_suite():
    """
    Example 7: Standard Benchmark Suite
    
    Demonstrates the complete standard benchmark suite for NLM evaluation.
    """
    print("\n" + "=" * 60)
    print("EXAMPLE 7: Standard Benchmark Suite")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 7.")
        return
    
    print("Running standard NLM benchmarks...")
    
    # Run standard benchmarks
    benchmarks = NLMPythonAPI.run_standard_benchmarks()
    
    print(f"\nBenchmark Results:")
    print(f"  Basic Operations: {len(benchmarks['basic_operations'])} profiles")
    print(f"  Agent-World Simulation: {benchmarks['agent_world_simulation'] is not None}")
    print(f"  Brain Implementation Comparison: {benchmarks['brain_implementation_comparison'] is not None}")
    
    # Display detailed benchmark information
    if 'basic_operations' in benchmarks:
        print(f"\nBasic Operations Performance:")
        for operation, profile in benchmarks['basic_operations'].items():
            steps_per_sec = profile.get('ops_per_second', 0)
            mean_time = profile.get('mean_time', 0) * 1000
            print(f"    {operation:15s}: {steps_per_sec:8.0f} ops/sec, {mean_time:6.2f} ms per operation")
    
    # Create visualization of benchmarks
    try:
        plotter = NLMDataPlotter()
        fig = plotter.plot_performance_benchmarks(benchmarks, save_path='complete_benchmarks.png')
        print(f"\nComplete benchmark visualization saved as 'complete_benchmarks.png'")
    except Exception as e:
        print(f"Benchmark visualization failed: {e}")
    
    # Generate benchmark report
    print(f"\nBenchmark Suite completed successfully!")
    
    return benchmarks

def example_8_comprehensive_workflow():
    """
    Example 8: Comprehensive Research Workflow
    
    Demonstrates a complete research workflow from setup to reporting.
    """
    print("\n" + "=" * 60)
    print("EXAMPLE 8: Comprehensive Research Workflow")
    print("=" * 60)
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("Advanced utilities not available. Skipping example 8.")
        return
    
    # Initialize workflow
    print("Starting comprehensive research workflow...")
    
    # Step 1: Setup experiment
    print("Step 1: Setting up experiment...")
    experiment = NLMPythonAPI.create_reproducible_experiment(seed=123)
    experiment.setup_experiment(width=35, height=35, enable_all_subsystems=True)
    
    # Step 2: Run experiment sequence
    print("Step 2: Running experiment sequence...")
    episode_configs = [
        {'num_steps': 100, 'reward_strategy': 'adaptive'},
        {'num_steps': 150, 'reward_strategy': 'curiosity'},
        {'num_steps': 200, 'reward_strategy': 'adaptive'},
    ]
    
    results = experiment.run_experiment_sequence(episode_configs)
    print(f"   Completed {len(results['episodes'])} episodes")
    
    # Step 3: Create checkpoint
    print("Step 3: Creating checkpoint...")
    checkpoint_file = experiment.create_checkpoint()
    print(f"   Checkpoint saved to: {checkpoint_file}")
    
    # Step 4: Performance profiling
    print("Step 4: Performance profiling...")
    profiler = NLMPerformanceProfiler()
    
    # Profile brain operations
    step_profile = profiler.profile_brain_operations(experiment.brain, 'step', 50)
    print(f"   Brain step performance: {step_profile['ops_per_second']:.0f} ops/sec")
    
    # Step 5: Create comprehensive report
    print("Step 5: Creating comprehensive report...")
    reporter = NLMVisualReporter()
    report_path = reporter.create_experiment_report(
        experiment_name="Comprehensive_Research_Workflow",
        experiment_results=results['episodes'],
        brain=experiment.brain,
        world=experiment.world,
        agent=experiment.agent
    )
    print(f"   Report saved to: {report_path}")
    
    # Step 6: Validation
    print("Step 6: Validating results...")
    validation = NLMPythonAPI.validate_installation()
    if not validation['errors']:
        print("   Validation passed successfully")
    else:
        print(f"   Validation warnings: {validation['warnings']}")
    
    # Step 7: Generate final summary
    print("Step 7: Generating final summary...")
    metrics = experiment.get_performance_metrics()
    
    print(f"\n" + "=" * 60)
    print("WORKFLOW SUMMARY")
    print("=" * 60)
    print(f"Experiment: {experiment.experiment_id}")
    print(f"Total steps executed: {experiment.step_count}")
    print(f"Total checkpoints created: {len(experiment.checkpoints)}")
    print(f"Reports generated: {len(reporter.report_files)}")
    print(f"Performance: {step_profile['ops_per_second']:.0f} ops/sec")
    print(f"Validation: {'PASSED' if not validation['errors'] else 'ISSUES FOUND'}")
    
    print(f"\nFiles created:")
    for i, report in enumerate(reporter.report_files, 1):
        print(f"  {i}. {os.path.basename(report)}")
    
    print(f"\nWorkflow completed successfully!")
    
    return experiment, profiler, reporter
def main():
    """
    Run all advanced example scripts in sequence.
    """
    print("\n" + "=" * 80)
    print("NLM ADVANCED EXAMPLE SCRIPTS - Power User Examples")
    print("=" * 80)
    print(f"Python version: {sys.version}")
    
    if not ADVANCED_UTILS_AVAILABLE:
        print("\nWARNING: Advanced utilities not available.")
        print("These examples require the nlm_utils package.")
        print("Please install it using: pip install -e .")
        return
    
    print(f"\nAdvanced NLM Python utilities are available.")
    print(f"Running comprehensive examples...")
    
    # Run examples sequentially
    examples = [
        ("Advanced Experiment Manager", example_1_advanced_experiment_manager),
        ("Performance Profiling", example_2_performance_profiling),
        ("Advanced Visualization", example_3_advanced_visualization),
        ("Experiment Reporting", example_4_experiment_reporting),
        ("Reproducible Research", example_5_reproducible_research),
        ("Parallel Research", example_6_parallel_research),
        ("Standard Benchmark Suite", example_7_standard_benchmark_suite),
        ("Comprehensive Workflow", example_8_comprehensive_workflow),
    ]
    
    completed_examples = 0
    failed_examples = []
    
    for example_name, example_function in examples:
        try:
            print(f"\n{'='*80}")
            print(f"Running: {example_name}")
            print(f"{'='*80}")
            
            example_function()
            completed_examples += 1
            print(f"✓ {example_name} completed successfully")
            
        except Exception as e:
            failed_examples.append((example_name, str(e)))
            print(f"✗ {example_name} failed: {e}")
    
    # Summary
    print(f"\n{'='*80}")
    print("EXAMPLE EXECUTION SUMMARY")
    print(f"{'='*80}")
    print(f"Total examples attempted: {len(examples)}")
    print(f"Completed successfully: {completed_examples}")
    print(f"Failed: {len(failed_examples)}")
    
    if failed_examples:
        print(f"\nFailed Examples:")
        for example_name, error in failed_examples:
            print(f"  - {example_name}: {error}")
    
    print(f"\nAll examples completed!")
    print(f"Check the output files in the current directory for generated reports and visualizations.")
    
    # List generated files
    print(f"\nGenerated files:")
    for filename in os.listdir('.'):
        if (filename.endswith(('.png', '.html', '.json', '.bin')) or
            'checkpoint' in filename or 'report' in filename or
            'benchmark' in filename or 'experiment' in filename):
            print(f"  - {filename}")
if __name__ == "__main__":
    main()
