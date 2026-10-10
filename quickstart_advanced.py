#!/usr/bin/env python3
"""
Quick Start: Advanced NLM Python Integration

This script provides a quick introduction to the advanced NLM Python capabilities,
demonstrating the key features and workflows for getting started with sophisticated
NLM experimentation and analysis.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import numpy as np
import time

# Import advanced NLM modules
from advanced_nlm_utils import (
    create_advanced_brain,
    create_advanced_world, 
    create_experimental_setup,
    NLMExperiment,
    NLMAnalyzer,
    NLMSimulationController
)

def example_1_basic_advanced_setup():
    """
    Example 1: Basic Advanced Setup
    Demonstrates creating an advanced NLM experiment with sophisticated control.
    """
    print("=" * 60)
    print("Example 1: Basic Advanced Setup")
    print("=" * 60)
    
    # Create advanced experimental setup
    config = {
        'neuron_count': 1500,
        'region_count': 3,
        'connection_probability': 0.15,
        'simulation_timestep': 0.001
    }
    
    print("Creating advanced experimental setup...")
    brain, world, experiment = create_experimental_setup(config)
    
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ World configured for {world.getWidth()}x{world.getHeight()} area")
    print(f"✓ Experiment controller ready")
    
    # Run simple stimulus-response protocol
    stimuli = [
        {'type': 'vision', 'data': np.random.rand(256), 'timing': 0},
        {'type': 'touch', 'data': np.random.rand(8), 'timing': 100}
    ]
    
    print("\nRunning stimulus-response protocol...")
    start_time = time.time()
    
    results = experiment.run_stimulus_response_protocol(
        stimuli=stimuli,
        response_window=200,
        recording_interval=10
    )
    
    end_time = time.time()
    
    print(f"✓ Protocol completed in {end_time - start_time:.2f} seconds")
    print(f"✓ {len(results['response_peaks'])} response peaks detected")
    print(f"✓ Mean firing rate: {results['mean_firing_rate']:.3f}")
    print(f"✓ Performance: {results['std_firing_rate']:.3f} std dev")
    print()

def example_2_advanced_analysis():
    """
    Example 2: Advanced Analysis
    Demonstrates sophisticated neural and behavioral data analysis.
    """
    print("=" * 60)
    print("Example 2: Advanced Analysis")
    print("=" * 60)
    
    # Create experimental setup
    brain, world, experiment = create_experimental_setup({})
    
    # Create advanced analyzer
    from advanced_nlm_utils import NLMAnalyzer
    
    print("Creating advanced data analyzer...")
    analyzer = NLMAnalyzer
    
    # Create sample neural data
    print("Creating sample neural spike data...")
    spike_data = []
    for i in range(100):
        spike_data.append({
            'neuron_id': i % 25,  # 25 neurons
            'time': i * 10,
            'amplitude': np.random.random()
        })
    
    # Analyze neural dynamics
    print("Analyzing neural spike dynamics...")
    dynamics = analyzer.analyze_neural_dynamics(spike_data, window_size=50)
    
    print(f"✓ Total spikes analyzed: {dynamics['total_spikes']}")
    print(f"✓ Active neurons: {dynamics['neurons_active']}")
    print(f"✓ Firing rate available: {len(dynamics['firing_rates']) > 0}")
    
    # Create mock neural regions for topology analysis
    class MockRegion:
        def __init__(self, region_id, neuron_count):
            self.id = region_id
            self.neuron_count = neuron_count
    
    print("\nPerforming network topology analysis...")
    regions = [MockRegion(i, 100) for i in range(5)]
    topology = analyzer.analyze_network_topology(regions)
    
    print(f"✓ Network regions: {topology['total_regions']}")
    print(f"✓ Total connections: {topology['total_connections']}")
    print(f"✓ Community structure: {len(topology['community_structure'])} modules")
    print()

def example_3_simulation_control():
    """
    Example 3: Simulation Control
    Demonstrates real-time simulation monitoring and control.
    """
    print("=" * 60)
    print("Example 3: Simulation Control")
    print("=" * 60)
    
    # Create experimental setup
    brain, world, experiment = create_experimental_setup({})
    
    # Create simulation controller
    print("Creating simulation controller...")
    controller = NLMSimulationController(brain, world)
    
    # Start simulation with monitoring
    print("Starting simulation with real-time monitoring...")
    controller.start_simulation(max_steps=500, enable_monitoring=True)
    
    # Run simulation with monitoring
    print("Running simulation steps...")
    results = []
    
    for step in range(0, 300, 50):
        # Step simulation
        step_results = controller.step_simulation(steps=50)
        results.append(step_results)
        
        # Display progress
        progress = (controller.current_step / 500) * 100
        print(f"  Progress: {progress:.1f}% (Step {controller.current_step}/500)")
        
        if controller.current_step >= 500:
            break
    
    # Stop simulation
    controller.stop_simulation()
    
    print(f"\n✓ Simulation completed successfully")
    print(f"✓ Total simulation steps: {sum(r['steps_completed'] for r in results)}")
    print(f"✓ Total spikes generated: {sum(r['spikes_generated'] for r in results)}")
    print(f"✓ Total actions produced: {sum(r['actions_produced'] for r in results)}")
    print()

def example_4_learning_experiment():
    """
    Example 4: Learning Experiment
    Demonstrates adaptive learning curve experiments.
    """
    print("=" * 60)
    print("Example 4: Learning Experiment")
    print("=" * 60)
    
    # Create experimental setup
    brain, world, experiment = create_experimental_setup({})
    
    # Design learning task
    task_params = {
        'task_type': 'navigation',
        'reward_function': 'proportional',
        'feedback_delay': 10
    }
    
    print("Designing adaptive learning experiment...")
    print(f"Task: {task_params['task_type']}")
    print(f"Trials: 50")
    print(f"Recording frequency: 3 steps")
    
    # Run learning experiment
    print("\nRunning learning curve experiment...")
    start_time = time.time()
    
    learning_results = experiment.run_learning_curve_experiment(
        task_params=task_params,
        num_trials=30,
        recording_frequency=5
    )
    
    end_time = time.time()
    
    print(f"✓ Learning experiment completed in {end_time - start_time:.2f} seconds")
    print(f"✓ Final performance level: {learning_results['final_performance']:.3f}")
    print(f"✓ Learning rate: {learning_results['learning_rate']:.3f}")
    print(f"✓ Model fit R²: {learning_results['model_fit_r2']:.3f}")
    print(f"✓ Data points collected: {learning_results['data_points']}")
    print()

def example_5_comprehensive_workflow():
    """
    Example 5: Comprehensive Workflow
    Demonstrates complete experimental workflow with all advanced features.
    """
    print("=" * 60)
    print("Example 5: Comprehensive Workflow")
    print("=" * 60)
    
    # Create advanced experimental setup
    print("Creating comprehensive experimental setup...")
    config = {
        'neuron_count': 2000,
        'region_count': 3,
        'connection_probability': 0.18,
        'simulation_timestep': 0.001
    }
    
    brain, world, experiment = create_experimental_setup(config)
    controller = NLMSimulationController(brain, world)
    
    # Define multi-stage protocol
    print("Designing multi-stage experimental protocol...")
    protocol_stages = [
        {
            'name': 'Stimulus-Acquisition',
            'type': 'stimulus_response',
            'stimuli': [
                {'type': 'vision', 'data': np.random.rand(256), 'timing': 0},
                {'type': 'touch', 'data': np.random.rand(8), 'timing': 100}
            ],
            'duration': 500,
            'objectives': ['pattern discrimination', 'short-term memory']
        },
        {
            'name': 'Skill Learning',
            'type': 'learning_curve',
            'task_params': {'task_type': 'navigation', 'difficulty': 0.5},
            'num_trials': 50,
            'objectives': ['motor skill acquisition', 'behavioral adaptation']
        },
        {
            'name': 'Development Tracking',
            'type': 'developmental',
            'age_stages': [50, 200, 500],
            'objectives': ['plasticity analysis', 'developmental trajectories']
        }
    ]
    
    # Run comprehensive workflow
    print("\nRunning comprehensive workflow...")
    start_time = time.time()
    
    # Stage 1: Stimulus-response
    print("  Stage 1: Stimulus-response protocol")
    stage1_results = experiment.run_stimulus_response_protocol(
        stimuli=protocol_stages[0]['stimuli'], response_window=300, recording_interval=10
    )
    
    # Stage 2: Learning curve
    print("  Stage 2: Adaptive learning experiment")
    stage2_results = experiment.run_learning_curve_experiment(
        task_params=protocol_stages[1]['task_params'], 
        num_trials=protocol_stages[1]['num_trials'], 
        recording_frequency=5
    )
    
    # Stage 3: Development tracking
    print("  Stage 3: Developmental trajectory tracking")
    stage3_results = experiment.run_development_experiment(
        age_stages=protocol_stages[2]['age_stages']
    )
    
    # Integration with simulation monitoring
    print("  Stage 4: Real-time simulation monitoring")
    controller.start_simulation(max_steps=300)
    
    simulation_results = []
    for step in range(0, 300, 50):
        step_result = controller.step_simulation(steps=50)
        simulation_results.append(step_result)
        if controller.current_step >= 300:
            break
    
    controller.stop_simulation()
    
    end_time = time.time()
    
    print(f"\n✓ Comprehensive workflow completed in {end_time - start_time:.2f} seconds")
    print(f"✓ Stage 1 results: {len(stage1_results['response_peaks'])} response peaks")
    print(f"✓ Stage 2 results: {stage2_results['final_performance']:.3f} final performance")
    print(f"✓ Stage 3 results: {len(stage3_results)} age stages analyzed")
    print(f"✓ Simulation results: {sum(r['steps_completed'] for r in simulation_results)} steps")
    print(f"✓ Total integration points: {len(protocol_stages)}")
    print()

def main():
    """Run all examples demonstrating advanced NLM capabilities."""
    print("Advanced NLM Python Integration - Quick Start Examples")
    print("This script demonstrates the key features and workflows for")
    print("getting started with sophisticated NLM experimentation and analysis.")
    print()
    
    examples = [
        ("Basic Advanced Setup", example_1_basic_advanced_setup),
        ("Advanced Analysis", example_2_advanced_analysis),
        ("Simulation Control", example_3_simulation_control),
        ("Learning Experiment", example_4_learning_experiment),
        ("Comprehensive Workflow", example_5_comprehensive_workflow),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"❌ Error in {name}: {e}")
            print("Skipping example due to error")
            print()
    
    print("=" * 60)
    print("Quick Start Examples Complete!")
    print()
    print("The Advanced NLM Python API provides sophisticated tools for:")
    print("• Complex experimental design and control")
    print("• Advanced neural data analysis and visualization")
    print("• Real-time simulation monitoring and optimization")
    print("• Machine learning integration for task automation")
    print("• Reproducible research protocols and data management")
    print()
    print("For more detailed documentation and examples, see:")
    print("• advanced_nlm_utils.py - Core advanced utilities")
    print("• examples/advanced_nlm_demo.py - Complete demonstration")
    print("• complete_nlm_integration.py - Full integration system")
    print("• ADVANCED_NLM_UTILS.md - Comprehensive API documentation")
    print("=" * 60)

if __name__ == "__main__":
    main()
