#!/usr/bin/env python3
"""
NLM Python Example - Enhanced Usage

This script demonstrates the enhanced Python API for NLM (Neural Learning Machine).
It shows how to use the new features like numpy integration, preset configurations,
and learning experiment helpers.
"""

import sys
import time
import numpy as np

# Add the python bindings directory to the path
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_6b8dd9b8-4ecc-4ccc-af60-514955e32bad/python')

try:
    import pynlm_enhanced as nlm
except ImportError:
    print("Error: Could not import enhanced NLM bindings")
    print("Please compile the enhanced bindings first using CMake")
    sys.exit(1)

def example_basic_usage():
    """Example 1: Basic brain setup and simulation"""
    print("=== Example 1: Basic Brain Usage ===")
    
    # Create configuration with research settings
    config = nlm.create_config()
    config.use_research_settings()
    
    # Create brain
    brain = nlm.Brain(config.get_raw_config())
    brain.initialize()
    
    # Run simulation
    print("Running simulation...")
    brain.step_batch(1000, verbose=True)
    
    # Get summary
    summary = brain.get_summary()
    print(f"Simulation completed!")
    print(f"Total neurons: {summary['total_neurons']}")
    print(f"Average firing rate: {summary['avg_firing_rate']:.4f}")
    print(f"Total spikes: {summary['total_spikes']}")
    
    return brain

def example_learning_experiment():
    """Example 2: Learning experiment with enhanced API"""
    print("\n=== Example 2: Learning Experiment ===")
    
    # Create brain with quick test settings
    config = nlm.create_config()
    config.use_quick_test_settings()
    
    brain = nlm.Brain(config.get_raw_config())
    agent_brain = nlm.AgentBrain(brain.get_raw_brain())
    
    # Create world
    world = nlm.create_world()
    world.setup_town(32)
    
    # Enable all systems
    agent_brain.enable_all_systems()
    
    # Run learning experiment
    print("Starting learning experiment...")
    results = agent_brain.run_learning_experiment(steps=500)
    
    print(f"Learning experiment results:")
    print(f"  Initial weight mean: {results['initial_weight_mean']:.4f}")
    print(f"  Final weight mean: {results['final_weight_mean']:.4f}")
    print(f"  Weight change: {results['weight_change']:.4f}")
    
    return brain, agent_brain, world

def example_numpy_integration():
    """Example 3: Direct numpy array integration"""
    print("\n=== Example 3: NumPy Integration ===")
    
    # Create world and brain
    config = nlm.create_config()
    config.use_quick_test_settings()
    
    brain = nlm.Brain(config.get_raw_config())
    agent_brain = nlm.AgentBrain(brain.get_raw_brain())
    world = nlm.create_world()
    world.setup_town(16)
    
    # Create vision data as numpy array (simulating camera input)
    vision_size = 16 * 16  # 16x16 vision
    vision_data = np.random.rand(vision_size) * 0.1  # Random noise
    vision_data[0] = 1.0  # Add a signal
    
    print(f"Vision data shape: {vision_data.shape}")
    print(f"Vision data type: {vision_data.dtype}")
    
    # Process vision directly from numpy array
    agent_brain.process_vision(vision_data, width=16, height=16)
    
    # Get neural activity for plotting
    plot_data = brain.get_neural_activity_plot_data()
    print(f"Neuron activities shape: {plot_data['neuron_activities'].shape}")
    print(f"Synapse weights shape: {plot_data['synapse_weights'].shape}")
    
    # Simulate a few steps
    for i in range(10):
        brain.step(i, i * 0.001)
    
    summary = brain.get_summary()
    print(f"Average firing rate after processing: {summary['avg_firing_rate']:.4f}")
    
    return brain, agent_brain, world

def example_monitoring_and_benchmarking():
    """Example 4: Monitoring and benchmarking"""
    print("\n=== Example 4: Monitoring and Benchmarking ===")
    
    # Get system info
    info = nlm.get_system_info()
    print(f"NLM Enhanced Version: {info}")
    
    # Create configuration and brain
    config = nlm.create_config()
    config.use_research_settings()
    
    # Benchmark different configurations
    def setup_brain_standard():
        config = nlm.create_config()
        config.use_quick_test_settings()
        brain = nlm.Brain(config.get_raw_config())
        brain.initialize()
        return brain
    
    def setup_brain_research():
        config = nlm.create_config()
        config.use_research_settings()
        brain = nlm.Brain(config.get_raw_config())
        brain.initialize()
        return brain
    
    # Benchmark brain creation
    print("Benchmarking brain creation...")
    
    standard_result = nlm.benchmark(setup_brain_standard, "Standard brain (100 neurons)")
    research_result = nlm.benchmark(setup_brain_research, "Research brain (5000 neurons)")
    
    print(f"Standard brain creation: {standard_result['duration']:.4f}s")
    print(f"Research brain creation: {research_result['duration']:.4f}s")
    
    return info

def example_visualization_data():
    """Example 5: Visualization data preparation"""
    print("\n=== Example 5: Visualization Data Preparation ===")
    
    # Create world for visualization
    world = nlm.create_world()
    world.setup_town(32)
    
    # Get visualization data
    viz_data = world.get_visualization_data()
    print(f"Map size: {viz_data['map_size']}")
    print(f"Agent position: {viz_data['agent_position']}")
    
    # Create brain for neural activity visualization
    config = nlm.create_config()
    config.use_research_settings()
    
    brain = nlm.Brain(config.get_raw_config())
    brain.initialize()
    
    # Run a short simulation
    brain.step_batch(100, verbose=False)
    
    # Get neural activity for plotting
    plot_data = brain.get_neural_activity_plot_data()
    
    print(f"Neural activity visualization data prepared:")
    print(f"  Neuron activities: {plot_data['neuron_activities'].shape}")
    print(f"  Synapse weights: {plot_data['synapse_weights'].shape}")
    
    # Calculate statistics for plotting
    neuron_activities = plot_data['neuron_activities']
    synapse_weights = plot_data['synapse_weights']
    
    print(f"  Average neuron activity: {np.mean(neuron_activities):.4f}")
    print(f"  Average synapse weight: {np.mean(synapse_weights):.4f}")
    
    return viz_data, plot_data

def example_error_handling():
    """Example 6: Enhanced error handling"""
    print("\n=== Example 6: Enhanced Error Handling ===")
    
    try:
        config = nlm.create_config()
        config.use_quick_test_settings()
        
        brain = nlm.Brain(config.get_raw_config())
        brain.initialize()
        
        # Try to process invalid vision data (too small)
        small_vision = np.random.rand(10)  # Too small for 16x16
        brain.process_vision(small_vision, width=16, height=16)
        
        print("This should not print - error should have been raised")
        
    except nlm.ConfigurationError as e:
        print(f"Caught ConfigurationError: {e}")
    except Exception as e:
        print(f"Caught generic exception: {type(e).__name__}: {e}")
    
    # Demonstrate mathematical utilities
    print(f"\nMathematical utilities:")
    print(f"  PI = {nlm.PI}")
    print(f"  E = {nlm.E}")
    print(f"  clamp(1.5, 0, 1) = {nlm.clamp(1.5, 0, 1)}")
    print(f"  normalize(0.5, 0, 1) = {nlm.normalize(0.5, 0, 1)}")
    
    return True

def main():
    """Run all examples"""
    print("NLM Enhanced Python Bindings - Examples")
    print("=" * 50)
    
    examples = [
        example_basic_usage,
        example_learning_experiment,
        example_numpy_integration,
        example_monitoring_and_benchmarking,
        example_visualization_data,
        example_error_handling
    ]
    
    results = []
    for example in examples:
        try:
            result = example()
            results.append((example.__name__, True, result))
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            import traceback
            traceback.print_exc()
            results.append((example.__name__, False, e))
    
    # Summary
    print("\n" + "=" * 50)
    print("SUMMARY")
    print("=" * 50)
    
    success_count = 0
    for name, success, result in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{name}: {status}")
        if success:
            success_count += 1
    
    print(f"\nExamples passed: {success_count}/{len(examples)}")
    
    if success_count == len(examples):
        print("\nAll examples completed successfully!")
        return 0
    else:
        print(f"\n{len(examples) - success_count} examples failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
