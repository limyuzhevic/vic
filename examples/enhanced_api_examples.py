#!/usr/bin/env python3
"""
Enhanced NLM API Usage Examples

This script demonstrates the new high-level API features added to NLM.
It showcases the improved Python interface for neural simulation.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from enhanced_pynlm import (
    NLMBrainTrainer,
    NLMExperimentRunner,
    NLMBrainOptimizer,
    create_learning_system,
    brain_session,
    run_standard_demo,
    create_brain_with_config
)

def example_1_basic_usage():
    """Example 1: Basic brain usage with enhanced API."""
    print("=== Example 1: Basic Brain Usage ===")
    
    # Method 1: Using enhanced API
    brain = create_brain_with_config()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Use context manager for automatic cleanup
    with brain_session() as ctx_brain:
        print(f"Context brain has {ctx_brain.getTotalNeuronCount()} neurons")
        ctx_brain.step(10)
        print(f"After 10 steps, {ctx_brain.getTotalSpikeCount()} spikes recorded")
    
    print("Example 1 complete\n")

def example_2_learning_system():
    """Example 2: Complete learning system setup."""
    print("=== Example 2: Complete Learning System ===")
    
    # Create complete learning system
    learning_system = create_learning_system({
        'width': 20.0,
        'height': 20.0,
        'vision_width': 8,
        'vision_height': 8
    })
    
    brain = learning_system['brain']
    world = learning_system['world']
    agent = learning_system['agent']
    trainer = learning_system['trainer']
    
    print(f"Learning system created:")
    print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  - World: {world.getWidth()}x{world.getHeight()}")
    print(f"  - Vision: {world.getAgentBody().x}, {world.getAgentBody().y}")
    
    # Run training
    metrics = trainer.train(num_episodes=3, max_steps_per_episode=50)
    
    summary = trainer.get_training_summary()
    print(f"\nTraining Summary:")
    print(f"  - Episodes trained: {summary['episodes_trained']}")
    print(f"  - Total reward: {summary['total_reward']:.2f}")
    print(f"  - Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
    print(f"  - Duration: {summary['duration']:.2f}s")
    
    print("Example 2 complete\n")

def example_3_experiment_runner():
    """Example 3: Running experiments with NLMExperimentRunner."""
    print("=== Example 3: Experiment Runner ===")
    
    # Create brain
    brain = create_brain_with_config()
    
    # Create experiment runner
    runner = NLMExperimentRunner(brain)
    
    # Define a simple evaluation function
    def evaluate_performance(brain_obj):
        """Simple evaluation function for demonstration."""
        firing_rate = brain_obj.getAverageFiringRate()
        spike_count = brain_obj.getTotalSpikeCount()
        return firing_rate * 0.1 + spike_count * 0.001
    
    # Create tasks for continual learning
    tasks = [
        {
            'name': 'Sensory Processing',
            'evaluate_function': lambda b: evaluate_performance(b)
        },
        {
            'name': 'Motor Control',
            'evaluate_function': lambda b: evaluate_performance(b) * 0.8
        },
        {
            'name': 'Memory Consolidation',
            'evaluate_function': lambda b: evaluate_performance(b) * 0.9
        }
    ]
    
    # Run continual learning experiment
    results = runner.run_continuous_learning_experiment(
        tasks=tasks,
        cycles=2,
        steps_between_tasks=100
    )
    
    print(f"Experiment Results:")
    print(f"  - Total cycles: {results['total_cycles']}")
    print(f"  - Tasks completed: {len(results['task_results'])}")
    print(f"  - Total duration: {results['total_duration']:.2f}s")
    print(f"  - Performance by task: {results['performance_by_task']}")
    
    print("Example 3 complete\n")

def example_4_brain_optimization():
    """Example 4: Brain analysis and optimization."""
    print("=== Example 4: Brain Analysis and Optimization ===")
    
    # Create brain
    brain = create_brain_with_config({
        'neuron_count': 500,
        'connection_probability': 0.05
    })
    
    # Analyze connectivity
    connectivity = NLMBrainOptimizer.analyze_connectivity(brain)
    
    print(f"Connectivity Analysis:")
    print(f"  - Total regions: {connectivity['region_count']}")
    print(f"  - Total neurons: {connectivity['total_neurons']}")
    print(f"  - Total synapses: {connectivity['total_synapses']}")
    print(f"  - Average neurons per region: {connectivity['connectivity_stats']['avg_neurons_per_region']:.1f}")
    print(f"  - Average synapses per region: {connectivity['connectivity_stats']['avg_synapses_per_region']:.1f}")
    
    # Export for visualization
    brain_json = NLMBrainOptimizer.visualize_brain_state(brain, 'json')
    print(f"\nBrain state exported ({len(brain_json)} characters)")
    
    # Export CSV
    brain_csv = NLMBrainOptimizer.visualize_brain_state(brain, 'csv')
    csv_lines = brain_csv.split('\n')
    print(f"CSV exported ({len(csv_lines)} lines)")
    
    print("Example 4 complete\n")

def example_5_standard_demo():
    """Example 5: Standard demo showcasing all features."""
    print("=== Example 5: Standard Demo ===")
    
    # Run standard demo
    results = run_standard_demo()
    
    learning_system = results['learning_system']
    summary = results['summary']
    
    print(f"Standard Demo Results:")
    print(f"  - Episodes completed: {results['episodes_completed']}")
    print(f"  - Steps per episode: {results['steps_per_episode']}")
    print(f"  - Training episodes: {summary['episodes_trained']}")
    print(f"  - Total reward: {summary['total_reward']:.2f}")
    print(f"  - Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
    print(f"  - Average steps per episode: {summary['avg_steps_per_episode']:.1f}")
    
    print("Example 5 complete\n")

def example_6_error_handling():
    """Example 6: Enhanced error handling and debugging."""
    print("=== Example 6: Error Handling ===")
    
    try:
        # Create brain
        brain = create_brain_with_config()
        
        # This should work
        brain.step(10)
        print("✓ Brain stepping successful")
        
        # Try to access non-existent region (should be handled gracefully)
        try:
            non_existent_region = brain.getRegion(999)  # Non-existent region ID
            if non_existent_region:
                print("✗ Should not reach here")
            else:
                print("✓ Non-existent region handled correctly")
        except Exception as e:
            print(f"✓ Region access error handled: {e}")
        
        print("Error handling example complete\n")
        
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        import traceback
        traceback.print_exc()

def main():
    """Run all examples."""
    print("Enhanced NLM API Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_1_basic_usage,
        example_2_learning_system,
        example_3_experiment_runner,
        example_4_brain_optimization,
        example_5_standard_demo,
        example_6_error_handling
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"Example {i} failed: {e}")
            import traceback
            traceback.print_exc()
        
        if i < len(examples):
            print("-" * 50)
    
    print("All examples completed!")
    print("\nEnhanced NLM API Features Demonstrated:")
    print("  ✓ Context managers for resource management")
    print("  ✓ High-level learning system setup")
    print("  ✓ Experiment runner for continual learning")
    print("  ✓ Brain optimization and analysis tools")
    print("  ✓ Standard demo showcasing all features")
    print("  ✓ Enhanced error handling")

if __name__ == '__main__':
    main()