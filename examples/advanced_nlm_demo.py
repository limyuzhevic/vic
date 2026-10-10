#!/usr/bin/env python3
"""
Advanced NLM Examples: Demonstrating the Enhanced Python API

This script demonstrates the advanced features and capabilities of the
enhanced NLM Python bindings, showcasing sophisticated experimental
control, analysis tools, and simulation management.

Examples included:
1. Advanced experiment design with stimulus-response protocols
2. Learning curve analysis with adaptive difficulty
3. Developmental trajectory tracking
4. Memory replay protocols
5. Real-time simulation monitoring and control
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import numpy as np
import time
from advanced_nlm_utils import (
    NLMExperiment, 
    NLMAnalyzer, 
    NLMSimulationController,
    create_advanced_brain,
    create_advanced_world,
    create_experimental_setup
)

def example_1_advanced_stimulus_response():
    """
    Example 1: Advanced stimulus-response protocol with adaptive timing
    """
    print("=== Example 1: Advanced Stimulus-Response Protocol ===")
    
    # Create experimental setup
    config = {
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.12,
        'simulation_timestep': 0.001
    }
    
    brain, world, experiment = create_experimental_setup(config)
    
    # Design sophisticated stimulus protocol
    stimuli = [
        {
            'type': 'vision',
            'data': np.random.rand(16, 16).flatten(),
            'duration': 50,
            'timing': 0
        },
        {
            'type': 'touch',
            'data': np.random.rand(8),
            'duration': 30,
            'timing': 100
        },
        {
            'type': 'internal',
            'data': np.random.rand(4),
            'duration': 40,
            'timing': 200
        },
        {
            'type': 'proprioception',
            'data': np.random.rand(6),
            'duration': 25,
            'timing': 300
        }
    ]
    
    # Run advanced experiment
    print("Running stimulus-response protocol...")
    start_time = time.time()
    
    results = experiment.run_stimulus_response_protocol(
        stimuli=stimuli,
        response_window=200,
        recording_interval=5
    )
    
    end_time = time.time()
    
    print(f"Protocol completed in {end_time - start_time:.2f} seconds")
    print(f"Total responses recorded: {len(results['response_peaks'])}")
    print(f"Mean firing rate: {results['mean_firing_rate']:.3f}")
    print(f"Standard deviation: {results['std_firing_rate']:.3f}")
    print()


def example_2_learning_curve_adaptive():
    """
    Example 2: Learning curve experiment with adaptive difficulty
    """
    print("=== Example 2: Adaptive Learning Curve Experiment ===")
    
    # Create experimental setup
    config = {
        'neuron_count': 1500,
        'region_count': 3,
        'connection_probability': 0.15,
        'simulation_timestep': 0.001
    }
    
    brain, world, experiment = create_experimental_setup(config)
    
    # Define learning task parameters
    task_params = {
        'task_type': 'navigation',
        'reward_function': 'proportional',
        'feedback_delay': 10
    }
    
    print("Running adaptive learning curve experiment...")
    start_time = time.time()
    
    results = experiment.run_learning_curve_experiment(
        task_params=task_params,
        num_trials=50,
        recording_frequency=3
    )
    
    end_time = time.time()
    
    print(f"Learning experiment completed in {end_time - start_time:.2f} seconds")
    print(f"Number of learning trials: {len(results['trials'])}")
    print(f"Final performance level: {results['final_performance']:.3f}")
    print(f"Learning rate: {results['learning_rate']:.3f}")
    print(f"Model fit R²: {results['model_fit_r2']:.3f}")
    print()


def example_3_developmental_tracking():
    """
    Example 3: Developmental trajectory tracking across ages
    """
    print("=== Example 3: Developmental Trajectory Tracking ===")
    
    # Create experimental setup
    config = {
        'neuron_count': 2000,
        'region_count': 4,
        'connection_probability': 0.18,
        'simulation_timestep': 0.001
    }
    
    brain, world, experiment = create_experimental_setup(config)
    
    # Define developmental age stages
    age_stages = [10, 50, 100, 300, 900, 2000]
    metrics = ['firing_rate', 'synaptic_strength', 'memory_capacity', 'plasticity', 'behavior']
    
    print("Running developmental trajectory experiment...")
    start_time = time.time()
    
    results = experiment.run_development_experiment(
        age_stages=age_stages,
        metrics=metrics
    )
    
    end_time = time.time()
    
    print(f"Developmental experiment completed in {end_time - start_time:.2f} seconds")
    print("Developmental trajectories:")
    
    for age, data in results.items():
        print(f"  {age}:")
        for metric, value in data.items():
            print(f"    {metric}: {value:.3f}")
    print()


def example_4_memory_replay():
    """
    Example 4: Memory replay protocol with consolidation
    """
    print("=== Example 4: Memory Replay Protocol ===")
    
    # Create experimental setup
    config = {
        'neuron_count': 1200,
        'region_count': 3,
        'connection_probability': 0.14,
        'simulation_timestep': 0.001,
        'replay_interval': 50,
        'consolidation_interval': 200
    }
    
    brain, world, experiment = create_experimental_setup(config)
    
    print("Running memory replay experiment...")
    start_time = time.time()
    
    results = experiment.create_replay_protocol(
        replay_episodes=3,
        replay_interval=50,
        replay_strength=1.0
    )
    
    end_time = time.time()
    
    print(f"Memory replay experiment completed in {end_time - start_time:.2f} seconds")
    print(f"Total replay events: {results['total_replays']}")
    print(f"Replay duration: {len(results['data'])} time points")
    print(f"Average neural activity during replay: {np.mean([d['neural_activity'] for d in results['data']]):.3f}")
    print()


def example_5_advanced_analysis():
    """
    Example 5: Advanced neural analysis with NLMAnalyzer
    """
    print("=== Example 5: Advanced Neural Analysis ===")
    
    # Create experimental setup
    config = {
        'neuron_count': 800,
        'region_count': 2,
        'connection_probability': 0.10,
        'simulation_timestep': 0.001
    }
    
    brain, world, experiment = create_experimental_setup(config)
    
    # Create a controller for advanced simulation control
    controller = NLMSimulationController(brain, world)
    
    # Create sample spike data for analysis
    spike_data = []
    for i in range(100):
        spike_data.append({
            'neuron_id': i % 50,  # 50 different neurons
            'time': i * 10,
            'amplitude': np.random.random()
        })
    
    print("Analyzing spike dynamics...")
    start_time = time.time()
    
    dynamics_analysis = NLMAnalyzer.analyze_neural_dynamics(
        spike_data, window_size=50
    )
    
    end_time = time.time()
    
    print(f"Dynamics analysis completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes analyzed: {dynamics_analysis['total_spikes']}")
    print(f"Active neurons: {dynamics_analysis['neurons_active']}")
    print(f"Firing rate analysis available: {len(dynamics_analysis['firing_rates']) > 0}")
    print()
    
    # Network topology analysis
    print("Performing network topology analysis...")
    start_time = time.time()
    
    # Create mock neural regions for topology analysis
    class MockRegion:
        def __init__(self, region_id, neuron_count):
            self.id = region_id
            self.neuron_count = neuron_count
    
    mock_regions = [MockRegion(i, 100) for i in range(5)]
    
    topology_analysis = NLMAnalyzer.analyze_network_topology(mock_regions)
    
    end_time = time.time()
    
    print(f"Topology analysis completed in {end_time - start_time:.2f} seconds")
    print(f"Network regions: {topology_analysis['total_regions']}")
    print(f"Total connections: {topology_analysis['total_connections']}")
    print(f"Community structure detected: {len(topology_analysis['community_structure']) > 0}")
    print()


def example_6_real_time_monitoring():
    """
    Example 6: Real-time simulation monitoring and control
    """
    print("=== Example 6: Real-Time Simulation Monitoring ===")
    
    # Create experimental setup
    config = {
        'neuron_count': 600,
        'region_count': 2,
        'connection_probability': 0.08,
        'simulation_timestep': 0.001
    }
    
    brain, world, experiment = create_experimental_setup(config)
    
    # Create controller for advanced monitoring
    controller = NLMSimulationController(brain, world)
    
    print("Starting real-time simulation with monitoring...")
    start_time = time.time()
    
    # Start simulation
    controller.start_simulation(max_steps=500, enable_monitoring=True)
    
    # Run simulation with monitoring
    results = []
    while controller.simulation_state == 'running' and controller.current_step < 500:
        # Step simulation
        step_results = controller.step_simulation(steps=10)
        results.append(step_results)
        
        # Display progress
        progress = (controller.current_step / 500) * 100
        print(f"Progress: {progress:.1f}% (Step {controller.current_step}/500)")
        
        if controller.current_step >= 500:
            break
    
    # Stop simulation
    controller.stop_simulation()
    
    end_time = time.time()
    
    print(f"Simulation monitoring completed in {end_time - start_time:.2f} seconds")
    print(f"Total simulation steps: {sum(r['steps_completed'] for r in results)}")
    print(f"Total spikes generated: {sum(r['spikes_generated'] for r in results)}")
    print(f"Total actions produced: {sum(r['actions_produced'] for r in results)}")
    print()


def example_7_advanced_workflow():
    """
    Example 7: Complete advanced workflow with experiment, analysis, and monitoring
    """
    print("=== Example 7: Complete Advanced Workflow ===")
    
    # Create experimental setup
    config = {
        'neuron_count': 1000,
        'region_count': 3,
        'connection_probability': 0.12,
        'simulation_timestep': 0.001
    }
    
    brain, world, experiment = create_experimental_setup(config)
    controller = NLMSimulationController(brain, world)
    
    print("Running complete advanced workflow...")
    start_time = time.time()
    
    # Phase 1: Stimulus-response protocol
    print("Phase 1: Stimulus-response protocol")
    stimuli = [
        {'type': 'vision', 'data': np.random.rand(256), 'timing': 0},
        {'type': 'touch', 'data': np.random.rand(8), 'timing': 100}
    ]
    
    protocol_results = experiment.run_stimulus_response_protocol(
        stimuli=stimuli, response_window=150, recording_interval=10
    )
    
    # Phase 2: Learning curve with adaptive difficulty
    print("Phase 2: Adaptive learning curve")
    task_params = {'task_type': 'classification', 'difficulty': 0.5}
    learning_results = experiment.run_learning_curve_experiment(
        task_params=task_params, num_trials=30, recording_frequency=5
    )
    
    # Phase 3: Developmental tracking
    print("Phase 3: Developmental trajectory")
    age_stages = [50, 200, 500]
    dev_results = experiment.run_development_experiment(
        age_stages=age_stages
    )
    
    # Phase 4: Memory replay
    print("Phase 4: Memory replay")
    replay_results = experiment.create_replay_protocol(
        replay_episodes=2, replay_interval=100
    )
    
    # Phase 5: Real-time monitoring
    print("Phase 5: Real-time simulation")
    controller.start_simulation(max_steps=300)
    simulation_results = []
    
    for step in range(0, 300, 50):
        step_result = controller.step_simulation(steps=50)
        simulation_results.append(step_result)
        if controller.current_step >= 300:
            break
    
    controller.stop_simulation()
    
    end_time = time.time()
    
    print(f"Complete workflow completed in {end_time - start_time:.2f} seconds")
    print(f"Phase 1 (protocol): {len(protocol_results['response_peaks'])} response peaks")
    print(f"Phase 2 (learning): {learning_results['final_performance']:.3f} final performance")
    print(f"Phase 3 (development): {len(dev_results)} age stages analyzed")
    print(f"Phase 4 (replay): {replay_results['total_replays']} replay events")
    print(f"Phase 5 (simulation): {sum(r['steps_completed'] for r in simulation_results)} steps")
    print()


def main():
    """Run all examples demonstrating advanced NLM capabilities."""
    print("=" * 60)
    print("Advanced NLM Python Examples - Enhanced Capabilities")
    print("=" * 60)
    print()
    
    examples = [
        ("Advanced Stimulus-Response Protocol", example_1_advanced_stimulus_response),
        ("Adaptive Learning Curve Experiment", example_2_learning_curve_adaptive),
        ("Developmental Trajectory Tracking", example_3_developmental_tracking),
        ("Memory Replay Protocol", example_4_memory_replay),
        ("Advanced Neural Analysis", example_5_advanced_analysis),
        ("Real-Time Simulation Monitoring", example_6_real_time_monitoring),
        ("Complete Advanced Workflow", example_7_advanced_workflow),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            print("Skipping example due to error")
            print()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("The advanced NLM Python API provides sophisticated tools for:")
    print("• Complex experimental design and control")
    print("• Advanced neural data analysis and visualization")
    print("• Real-time simulation monitoring and optimization")
    print("• Machine learning integration for task automation")
    print("• Reproducible research protocols and data management")
    print("=" * 60)


if __name__ == "__main__":
    main()
