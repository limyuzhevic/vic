#!/usr/bin/env python3
"""
Example 1: Basic Enhanced Usage

This example demonstrates basic usage of the NLM advanced features.
It shows how to set up an experiment runner and run a simple simulation
with advanced monitoring and analysis.

Key features demonstrated:
- Basic experiment setup with enhanced NLM API
- Metrics collection and analysis
- Simple callback system
- Checkpoint management
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools
)

def example_basic_usage():
    """Demonstrate basic enhanced NLM usage."""
    print("=== Example 1: Basic Enhanced Usage ===\n")
    
    # Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create experiment runner with basic configuration
    runner = NLMExperimentRunner(brain, world, agent)
    
    # Add a simple callback to monitor activity
    def activity_monitor(runner, metrics, event_type):
        if metrics.get('firing_rate', 0) > 5.0:
            print(f"  High activity detected at step {metrics['step']}: {metrics['firing_rate']:.2f} Hz")
    
    runner.add_callback(activity_monitor)
    
    # Configure experiment
    experiment_config = ExperimentConfig(
        name="Basic Enhanced Experiment",
        max_steps=200,
        timestep=0.1,
        verbose=True,
        save_checkpoints=False  # Set to True to save checkpoints
    )
    
    # Run experiment
    print(f"\nRunning experiment: {experiment_config.name}")
    print(f"Max steps: {experiment_config.max_steps}")
    print(f"Timestep: {experiment_config.timestep}s")
    
    result = runner.run_experiment(experiment_config)
    
    # Display results
    print(f"\n=== Experiment Results ===")
    print(f"Success: {'✓' if result.success else '✗'}")
    print(f"Duration: {result.duration:.2f} seconds")
    print(f"Steps completed: {result.steps_completed}")
    print(f"Final firing rate: {result.final_firing_rate:.3f} Hz")
    print(f"Total spikes: {result.total_spikes}")
    
    if result.learning_metrics:
        print(f"\nLearning Metrics:")
        print(f"  Firing rate trend: {result.learning_metrics.get('firing_rate_trend', 0):.3f}")
        print(f"  Stability: {result.learning_metrics.get('stability', 0):.3f}")
        print(f"  Exploration index: {result.learning_metrics.get('exploration_index', 0):.3f}")
    
    return result

def example_configuration_validation():
    """Demonstrate configuration validation."""
    print("\n=== Configuration Validation Example ===\n")
    
    analyzer = NLMConfigAnalyzer()
    
    # Create configurations
    good_config = pynlm.createDefaultConfig()
    good_config.set("brain.neuron_count", 1000)
    good_config.set("simulation_timestep", 0.001)
    good_config.set("plasticity.stdp.enable", True)
    
    bad_config = pynlm.createDefaultConfig()
    bad_config.set("brain.neuron_count", 10)  # Too low
    bad_config.set("simulation_timestep", 0.1)  # Too high
    
    # Validate good configuration
    print("Validating good configuration...")
    good_validation = analyzer.validate_config(good_config)
    print(f"  Validation score: {good_validation['score']}/100")
    print(f"  Valid: {'Yes' if good_validation['valid'] else 'No'}")
    if good_validation['warnings']:
        print(f"  Warnings: {good_validation['warnings']}")
    
    # Validate bad configuration
    print("\nValidating bad configuration...")
    bad_validation = analyzer.validate_config(bad_config)
    print(f"  Validation score: {bad_validation['score']}/100")
    print(f"  Valid: {'Yes' if bad_validation['valid'] else 'No'}")
    if bad_validation['errors']:
        print(f"  Errors: {bad_validation['errors']}")
    
    return good_validation, bad_validation

def example_analysis_tools():
    """Demonstrate analysis tools usage."""
    print("\n=== Analysis Tools Example ===\n")
    
    analyzer = NLMAnalysisTools()
    
    # Example 1: Firing pattern analysis
    print("1. Firing Pattern Analysis")
    spike_times = [0.1, 0.3, 0.5, 0.7, 1.0, 1.2, 1.5, 1.8, 2.1, 2.4]
    pattern_analysis = analyzer.analyze_firing_patterns(spike_times)
    
    print(f"   Firing rate: {pattern_analysis['firing_rate']:.3f} Hz")
    print(f"   Mean ISI: {pattern_analysis['mean_isi']:.3f} s")
    print(f"   Regularity: {pattern_analysis['regularity']:.3f}")
    print(f"   Burst ratio: {pattern_analysis['burst_ratio']:.3f}")
    
    # Example 2: Learning curve analysis
    print("\n2. Learning Curve Analysis")
    steps = list(range(0, 100, 10))
    values = [0.1, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.82, 0.88, 0.92]
    
    learning_curve = analyzer.calculate_learning_curve(steps, values)
    
    print(f"   Initial value: {learning_curve['initial']:.3f}")
    print(f"   Final value: {learning_curve['final']:.3f}")
    print(f"   Improvement: {learning_curve['improvement_percentage']:.2f}%")
    print(f"   Learning rate: {learning_curve['learning_rate']:.6f}")
    print(f"   Convergence step: {learning_curve['convergence_step']}")
    
    # Example 3: Batch analysis
    print("\n3. Batch Analysis")
    experiments = [
        {'name': 'Exp1', 'duration': 10.5, 'success': True},
        {'name': 'Exp2', 'duration': 15.2, 'success': True},
        {'name': 'Exp3', 'duration': 8.7, 'success': False},
    ]
    
    batch_analysis = analyzer.batch_analyze_experiments(experiments)
    
    print(f"   Total experiments: {batch_analysis['experiment_count']}")
    print(f"   Success rate: {batch_analysis['successful_experiments']/batch_analysis['experiment_count']*100:.1f}%")
    print(f"   Average duration: {batch_analysis['average_duration']:.2f}s")

if __name__ == "__main__":
    print("NLM Advanced Features - Example 1: Basic Usage")
    print("=" * 50)
    
    try:
        # Run basic usage example
        result = example_basic_usage()
        
        # Run configuration validation example
        example_configuration_validation()
        
        # Run analysis tools example
        example_analysis_tools()
        
        print("\n" + "=" * 50)
        print("Example 1 completed successfully!")
        
    except Exception as e:
        print(f"\nError in example 1: {e}")
        import traceback
        traceback.print_exc()
