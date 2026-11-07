#!/usr/bin/env python3
"""
Enhanced NLM Python API Demo

This script demonstrates the new enhanced NLM Python API with improved abstractions,
convenience functions, and advanced features.

The enhanced API provides:
1. Simplified experiment setup with create_simple_experiment()
2. High-level NLMExperiment manager with monitoring and checkpointing
3. Advanced world simulation with terrain and hazards
4. Batch processing and benchmarking capabilities
5. Performance monitoring and analysis tools
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

# Import the enhanced NLM API
from advanced_api import (
    create_simple_experiment,
    run_episode,
    NLMExperiment,
    create_advanced_world,
    batch_run_experiments,
    benchmark_performance
)


def demo_basic_usage():
    """Demonstrate basic usage of the enhanced API."""
    print("=" * 60)
    print("DEMO 1: Basic Usage with Enhanced API")
    print("=" * 60)
    
    # Create a simple experiment with default settings
    experiment = create_simple_experiment(
        brain_size=500,
        world_width=30,
        world_height=30
    )
    
    print(f"Created experiment:")
    print(f"  - Brain: {experiment['brain'].getTotalNeuronCount()} neurons")
    print(f"  - World: {experiment['world'].getWidth()}x{experiment['world'].getHeight()}")
    print(f"  - Agent initialized: {experiment['agent'].getBrain() is not None}")
    
    # Run a short episode
    print("\nRunning episode (50 steps)...")
    result = run_episode(experiment, max_steps=50, enable_visualization=False)
    
    print(f"Episode completed:")
    print(f"  - Steps: {result['step_count']}")
    print(f"  - Total reward: {result['performance_metrics']['total_reward']:.2f}")
    print(f"  - Final firing rate: {result['performance_metrics']['final_firing_rate']:.2f} Hz")
    print(f"  - Total spikes: {result['performance_metrics']['total_spikes']}")
    
    return experiment


def demo_experiment_manager():
    """Demonstrate the NLMExperiment manager with monitoring."""
    print("\n" + "=" * 60)
    print("DEMO 2: NLMExperiment Manager with Monitoring")
    print("=" * 60)
    
    # Create experiment manager with custom configuration
    exp_manager = NLMExperiment({
        'brain.neuron_count': 800,
        'brain.connection_probability': 0.12,
        'random_seed': 42
    })
    
    # Add callbacks for monitoring
    def step_callback(exp_state):
        step = exp_state['step_count']
        if step % 10 == 0:  # Log every 10 steps
            print(f"  Step {step}: Reward={exp_state['reward_history'][-1]:.3f}, "
                  f"Curiosity={exp_state['agent'].getCuriosityLevel():.3f}")
    
    def episode_callback(exp_state):
        metrics = exp_state['performance_metrics']
        print(f"Episode complete: Total reward={metrics['total_reward']:.2f}, "
              f"Firing rate={metrics['final_firing_rate']:.2f} Hz")
    
    exp_manager.add_step_callback(step_callback)
    exp_manager.add_episode_callback(episode_callback)
    
    # Start experiment
    exp_manager.start_experiment()
    print("Experiment started")
    
    # Run episode with checkpointing
    result = exp_manager.run_episode(max_steps=100, checkpoint_interval=25)
    
    # Save checkpoint
    exp_manager.save_checkpoint("demo_checkpoint", "Demo experiment checkpoint")
    
    # Load checkpoint and continue
    print("\nLoading checkpoint...")
    exp_manager2 = NLMExperiment()
    exp_manager2.start_experiment()
    exp_manager2.load_checkpoint("demo_checkpoint")
    
    print("Checkpoint loaded successfully")
    
    # Export results
    exp_manager.export_results("demo_results.json")
    print("\nResults exported to demo_results.json")
    
    # Analyze performance
    analysis = exp_manager.analyze_performance()
    print(f"\nPerformance Analysis:")
    print(f"  - Total steps: {analysis['total_steps']}")
    print(f"  - Average reward per step: {analysis['avg_reward_per_step']:.3f}")
    print(f"  - Firing rate: {analysis['firing_rate_stats']['mean']:.2f} Hz")
    print(f"  - Efficiency: {analysis['efficiency']['spikes_per_step']:.2f} spikes/step")
    
    return exp_manager


def demo_advanced_world():
    """Demonstrate advanced world simulation."""
    print("\n" + "=" * 60)
    print("DEMO 3: Advanced World Simulation")
    print("=" * 60)
    
    # Create advanced world with terrain, resources, and hazards
    print("Creating advanced world with terrain, resources, and hazards...")
    world = create_advanced_world(
        width=60,
        height=40,
        enable_terrain=True,
        enable_resources=True,
        enable_hazards=True
    )
    
    print(f"World created: {world}")
    print(f"Agent starting position: ({world.get_agent_state().x:.1f}, {world.get_agent_state().y:.1f})")
    
    # Create brain and agent for the world
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("brain.region_count", 1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(50):
        # Update world
        percept = world.update(0.1)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.apply_action(action, world.get_agent_state().lastActionTime)
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status every 10 steps
        if step % 10 == 0:
            print(f"  Step {step}: Pos=({world.get_agent_state().x:.1f},{world.get_agent_state().y:.1f}), "
                  f"Energy={world.get_agent_state().energy:.1f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"\nFinal world metrics:")
    metrics = world.get_metrics()
    for key, value in metrics.items():
        if key != 'agent_position':
            print(f"  - {key}: {value}")
    
    print(f"  - Agent position: {metrics['agent_position']}")
    
    return world, agent, brain


def demo_batch_processing():
    """Demonstrate batch processing capabilities."""
    print("\n" + "=" * 60)
    print("DEMO 4: Batch Processing")
    print("=" * 60)
    
    # Define different experiment configurations
    configs = [
        {'brain.neuron_count': 500, 'brain.connection_probability': 0.1},
        {'brain.neuron_count': 1000, 'brain.connection_probability': 0.15},
        {'brain.neuron_count': 1500, 'brain.connection_probability': 0.12},
    ]
    
    print(f"Running batch processing with {len(configs)} configurations...")
    print(f"Each configuration will run 3 episodes...")
    
    # Run batch experiments
    results = batch_run_experiments(
        configs,
        episodes_per_config=3
    )
    
    print(f"\nBatch processing complete!")
    print(f"Processed {len(results)} configurations")
    
    # Display results summary
    for i, result in enumerate(results):
        analysis = result['final_analysis']
        print(f"\nConfiguration {i+1}:")
        print(f"  - Neurons: {result['config']['brain.neuron_count']}")
        print(f"  - Connection probability: {result['config']['brain.connection_probability']}")
        print(f"  - Total reward: {analysis['total_rewards']:.2f}")
        print(f"  - Final firing rate: {analysis['firing_rate_stats']['mean']:.2f} Hz")
        print(f"  - Steps completed: {analysis['total_steps']}")
    
    return results


def demo_benchmarking():
    """Demonstrate benchmarking capabilities."""
    print("\n" + "=" * 60)
    print("DEMO 5: Performance Benchmarking")
    print("=" * 60)
    
    # Define benchmark configurations
    benchmark_configs = [
        {'brain.neuron_count': 500, 'brain.connection_probability': 0.1},
        {'brain.neuron_count': 800, 'brain.connection_probability': 0.12},
        {'brain.neuron_count': 1200, 'brain.connection_probability': 0.15},
    ]
    
    print(f"Running benchmark with {len(benchmark_configs)} configurations...")
    print(f"Each configuration will be run {5} times...")
    
    # Run benchmark
    benchmark_results = benchmark_performance(
        benchmark_configs,
        num_runs=3
    )
    
    print(f"\nBenchmark Results:")
    print("-" * 80)
    print(f"{'Config':<6} {'Neurons':<8} {'Conn Prob':<10} {'Mean Time(s)':<12} {'Time Std':<10} "
          f"{'Mean Reward':<12} {'Firing Rate':<12}")
    print("-" * 80)
    
    for i, result in enumerate(benchmark_results['results']):
        config = result['config']
        print(f"{i+1:<6} {config['brain.neuron_count']:<8} "
              f"{config['brain.connection_probability']:<10} "
              f"{result['mean_time']:<12.3f} "
              f"{result['time_std']:<10.3f} "
              f"{result['mean_reward_efficiency']:<12.3f} "
              f"{result['mean_firing_rate']:<12.2f}")
    
    overall = benchmark_results['overall_stats']
    print("-" * 80)
    print(f"{'Overall':<6} {'':<8} {'':<10} {overall['total_time']:<12.1f} {'':<10} "
          f"{'':<12} {overall['avg_performance']:<12.2f}")
    
    return benchmark_results


def main():
    """Run all demonstrations."""
    print("Enhanced NLM Python API Demo")
    print("This demo showcases the new NLM Python API with improved abstractions, "
          "convenience functions, and advanced features.")
    
    try:
        # Run all demos
        demo_basic_usage()
        demo_experiment_manager()
        demo_advanced_world()
        demo_batch_processing()
        demo_benchmarking()
        
        print("\n" + "=" * 60)
        print("All demonstrations completed successfully!")
        print("=" * 60)
        print("\nKey improvements demonstrated:")
        print("1. ✓ Simplified experiment setup with create_simple_experiment()")
        print("2. ✓ Enhanced NLMExperiment manager with monitoring")
        print("3. ✓ Advanced world simulation with terrain and hazards")
        print("4. ✓ Batch processing for multiple configurations")
        print("5. ✓ Performance benchmarking tools")
        print("6. ✓ Checkpointing and state persistence")
        print("7. ✓ Performance analysis and metrics")
        
    except Exception as e:
        print(f"\nError during demo: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0


if __name__ == "__main__":
    # Import pynlm here to handle import errors gracefully
    try:
        import pynlm
        sys.exit(main())
    except ImportError as e:
        print(f"Error: Could not import pynlm: {e}")
        print("Please ensure NLM is properly installed and the Python bindings are built.")
        sys.exit(1)