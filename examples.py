#!/usr/bin/env python3
"""
NLM High-Level API Example Scripts

This module provides example scripts that demonstrate the use of the high-level
API abstractions for common NLM use cases. These examples show how to use the
simplified APIs while maintaining access to advanced features.
"""

import pynlm
import pynlm_api
from pynlm_api import (
    create_simple_neural_simulation,
    create_agent_training_simulation,
    create_experiment_simulation,
    NLMAPI,
    SimulationResult,
    TrainingResult
)


def example_basic_simulation():
    """
    Example 1: Basic Neural Simulation
    
    This example shows the simplest way to run a neural simulation.
    """
    print("=== Example 1: Basic Neural Simulation ===")
    
    # Simple one-line simulation
    result = create_simple_neural_simulation(neuron_count=500, steps=100)
    
    print(f"✓ Simulation completed successfully")
    print(f"  - Steps executed: {result.steps_completed}")
    print(f"  - Average firing rate: {result.avg_firing_rate:.3f} Hz")
    print(f"  - Total spikes: {result.total_spikes}")
    print(f"  - Execution time: {result.execution_time:.2f} seconds")
    print()


def example_agent_training():
    """
    Example 2: Agent Training with Reinforcement Learning
    
    This example shows how to train an agent using the high-level API.
    """
    print("=== Example 2: Agent Training ===")
    
    # Create a training environment
    training_result = create_agent_training_simulation(
        neuron_count=1000,
        episodes=20,
        steps_per_episode=50
    )
    
    print(f"✓ Agent training completed successfully")
    print(f"  - Episodes trained: {training_result.experience_gained // 50}")
    print(f"  - Total experience gained: {training_result.experience_gained}")
    print(f"  - Final performance: {training_result.performance_metrics['final_avg_reward']:.3f}")
    print(f"  - Convergence score: {training_result.convergence_score:.3f}")
    print(f"  - Final firing rate: {training_result.performance_metrics['final_firing_rate']:.3f}")
    
    # Show learning curve summary
    if training_result.learning_curve:
        initial_perf = training_result.learning_curve[0]['avg_reward']
        final_perf = training_result.learning_curve[-1]['avg_reward']
        improvement = final_perf - initial_perf
        print(f"  - Performance improvement: {improvement:.3f}")
    
    print()


def example_experiment_setup():
    """
    Example 3: Experiment Setup with Custom Configuration
    
    This example shows how to set up and run a custom experiment.
    """
    print("=== Example 3: Experiment Setup ===")
    
    # Define experiment configuration
    experiment_config = {
        'neuron_count': 1500,
        'max_steps': 500,
        'enable_development': True,
        'world_size': 15
    }
    
    # Run experiment
    experiment_result = create_experiment_simulation(
        experiment_config,
        "memory_development_test",
        max_steps=200
    )
    
    print(f"✓ Experiment '{experiment_result['experiment_name']}' completed")
    print(f"  - Configuration: neuron_count={experiment_result['metadata']['brain_configured']}, "
          f"world_configured={experiment_result['metadata']['world_configured']}")
    print(f"  - Steps completed: {experiment_result['steps_completed']}")
    print(f"  - Total reward: {experiment_result['total_reward']:.3f}")
    print(f"  - Average reward: {experiment_result['avg_reward']:.3f}")
    print(f"  - Final developmental stage: {experiment_result['final_developmental_stage']}")
    print(f"  - Novelty level: {experiment_result['final_novelty']}")
    print(f"  - Curiosity level: {experiment_result['final_curiosity']}")
    print()


def example_custom_api_usage():
    """
    Example 4: Custom API Usage for Advanced Scenarios
    
    This example shows how to use the NLMAPI class for more complex scenarios.
    """
    print("=== Example 4: Custom API Usage ===")
    
    # Create API instance
    api = NLMAPI.create_agent_training_brain(neuron_count=1200)
    
    # Create a custom learning callback
    def learning_monitor(episode: int, step: int, total_reward: float, current_reward: float):
        if episode % 5 == 0 and step % 20 == 0:
            print(f"  Training: Episode {episode}, Step {step}, Reward: {current_reward:.3f}")
    
    # Train with custom monitoring
    training_result = api.train_agent(
        episodes=15,
        steps_per_episode=30,
        exploration_rate=0.15,
        learning_callback=learning_monitor
    )
    
    print(f"✓ Custom training completed")
    print(f"  - Final performance metrics:")
    for key, value in training_result.performance_metrics.items():
        if isinstance(value, (int, float)):
            print(f"    {key}: {value:.3f}")
    
    print()


def example_data_analysis():
    """
    Example 5: Data Analysis and Visualization Preparation
    
    This example shows how to analyze simulation data for visualization.
    """
    print("=== Example 5: Data Analysis ===")
    
    # Create API instance for analysis
    api = NLMAPI.create_simple_brain(neuron_count=800)
    
    # Run simulation to get data
    result = api.run_basic_simulation(steps=50)
    
    # Create visualization data
    viz_data = api.create_visualization_data(api._brain)
    
    print(f"✓ Visualization data prepared")
    print(f"  - Neural activity metrics:")
    for key, value in viz_data['neural_activity'].items():
        print(f"    {key}: {value}")
    
    print(f"  - Memory metrics:")
    for key, value in viz_data['memory'].items():
        print(f"    {key}: {value}")
    
    print(f"  - Neuromodulation metrics:")
    for key, value in viz_data['neuromodulation'].items():
        print(f"    {key}: {value}")
    
    print()


def example_backward_compatibility():
    """
    Example 6: Backward Compatibility with Legacy APIs
    
    This example shows how to use the legacy APIs while benefiting from
    the high-level abstractions.
    """
    print("=== Example 6: Backward Compatibility ===")
    
    # Create legacy components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Use legacy API (backward compatibility)
    result = pynlm_api.run_legacy_agent_simulation(brain, world, agent, steps=100)
    
    print(f"✓ Legacy simulation completed")
    print(f"  - Steps completed: {result.steps_completed}")
    print(f"  - Total reward: {result.total_reward:.3f}")
    print(f"  - Active neurons: {result.active_neurons}")
    print()


def main():
    """
    Run all examples to demonstrate the high-level API.
    """
    print("NLM High-Level API Examples")
    print("=" * 50)
    print()
    
    try:
        example_basic_simulation()
        example_agent_training()
        example_experiment_setup()
        example_custom_api_usage()
        example_data_analysis()
        example_backward_compatibility()
        
        print("=" * 50)
        print("All examples completed successfully!")
        print()
        print("The high-level API provides simplified interfaces for:")
        print("• Basic neural simulations (create_simple_neural_simulation)")
        print("• Agent training (create_agent_training_simulation)")
        print("• Experiment setup (create_experiment_simulation)")
        print("• Custom API usage (NLMAPI class)")
        print("• Data analysis and visualization preparation")
        print("• Backward compatibility with legacy APIs")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
