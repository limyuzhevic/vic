"""Example demonstrating the enhanced Pythonic NLM API."""

"""
This example demonstrates the new Pythonic API for NLM (Neural Learning Machine)
Python bindings with enhanced properties, batch operations, error handling,
and utility functions.
"""

import pynlm
import pynlm.utils
import time
import sys

def main():
    print("=" * 60)
    print("NLM Python Bindings - Enhanced Pythonic API Example")
    print("=" * 60)
    
    # 1. Display version information
    print("\n1. Version Information:")
    print("-" * 40)
    version_info = pynlm.utils.get_version_info()
    print(f"Version: {version_info['version']}")
    print(f"Python API: {version_info.get('python_api_version', 'N/A')}")
    print(f"C++ API: {version_info.get('cpp_api_version', 'N/A')}")
    
    # 2. Create default configuration
    print("\n2. Configuration Management:")
    print("-" * 40)
    config = pynlm.Config()
    config.set("simulation_max_steps", 10000)
    config.set("simulation_timestep", 0.01)
    config.set("neuron_resting_potential", -70.0)
    config.set("neuron_threshold", -55.0)
    
    print(f"Configuration: {config.summary()}")
    print(f"Has max_steps setting: {config.has('simulation_max_steps')}")
    print(f"Max steps value: {config.get('simulation_max_steps', 0)}")
    
    # 3. Create and initialize brain
    print("\n3. Brain Management:")
    print("-" * 40)
    brain = pynlm.Brain(config)
    print(f"Created brain: {brain}")
    
    # Initialize the brain
    print("Initializing brain...")
    if brain.initialize():
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        return
    
    # 4. Demonstrate Pythonic properties
    print("\n4. Pythonic Properties:")
    print("-" * 40)
    print(f"Brain spike count: {brain.spike_count}")
    print(f"Brain synapse count: {brain.synapse_count}")
    print(f"Active neurons: {brain.active_neurons}")
    print(f"Firing neurons: {brain.firing_neurons}")
    print(f"E/I ratio: {brain.ei_ratio:.3f}")
    print(f"Developmental stage: {brain.developmental_stage}")
    print(f"Region names: {brain.region_names}")
    
    # 5. Demonstrate batch operations
    print("\n5. Batch Operations:")
    print("-" * 40)
    print("Running 100 simulation steps...")
    start_time = time.time()
    
    if brain.step_batch(100):
        print(f"✓ Processed 100 steps successfully")
        elapsed = time.time() - start_time
        print(f"  Time elapsed: {elapsed:.2f} seconds")
    else:
        print("✗ Batch stepping failed")
    
    # Update metrics after stepping
    print(f"After batch: Spike count = {brain.spike_count}")
    
    # 6. Demonstrate brain summary
    print("\n6. Brain Summary:")
    print("-" * 40)
    summary = brain.get_summary()
    print("Brain status:")
    for key, value in summary.items():
        if isinstance(value, float):
            print(f"  {key:30s}: {value:.3f}")
        else:
            print(f"  {key:30s}: {value}")
    
    # 7. Create agent brain
    print("\n7. Agent Brain Integration:")
    print("-" * 40)
    agent_brain = pynlm.create_agent_brain(brain)
    print(f"Created agent brain: {agent_brain}")
    
    # Configure agent brain
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    
    print(f"Reward modulation enabled: {agent_brain.reward_modulation_enabled}")
    print(f"Curiosity enabled: {agent_brain.curiosity_enabled}")
    
    # 8. Create world and demonstrate advanced configuration
    print("\n8. World Management:")
    print("-" * 40)
    world = pynlm.SimpleWorld()
    
    # Configure with advanced options
    advanced_config = {
        "max_energy": 200.0,
        "energy_decay_rate": 0.005,
        "random_seed": 12345,
        "agent_start_x": 0.0,
        "agent_start_y": 0.0
    }
    
    print(f"Configuring world with: {advanced_config}")
    world.configure_advanced(**advanced_config)
    
    print(f"World created: {world}")
    print(f"World dimensions: {world.get_width()} x {world.get_height()}")
    print(f"Max energy: {world.get_max_energy()}")
    print(f"Energy decay rate: {world.get_energy_decay_rate()}")
    print(f"Random seed: {world.get_random_seed()}")
    
    # Initialize agent brain with world
    agent_brain.initialize(world)
    
    # 9. Demonstrate utility functions
    print("\n9. Utility Functions:")
    print("-" * 40)
    
    # Memory usage
    memory_info = pynlm.utils.get_memory_usage()
    print(f"Memory usage (RSS): {memory_info.get('rss_mb', 0):.2f} MB")
    print(f"Memory usage (VMS): {memory_info.get('vms_mb', 0):.2f} MB")
    
    # Error handling demonstration
    print("\n10. Error Handling:")
    print("-" * 40)
    
    # Demonstrate safe error handling
    try:
        # This would normally require proper initialization
        brain.step()
        print("✓ Brain stepping successful")
    except Exception as e:
        print(f"Error handled: {type(e).__name__}: {e}")
    
    # 11. Benchmarking
    print("\n11. Benchmarking:")
    print("-" * 40)
    print("Benchmarking brain performance...")
    benchmark_results = pynlm.utils.benchmark_brain(brain, steps=50)
    
    print(f"Benchmark results:")
    print(f"  Steps: {benchmark_results['steps']}")
    print(f"  Total time: {benchmark_results['total_time_ms']:.2f} ms")
    print(f"  Average step time: {benchmark_results['avg_step_time_ms']:.2f} ms")
    print(f"  Throughput: {benchmark_results['throughput_hz']:.2f} steps/sec")
    print(f"  Spikes per step: {benchmark_results['spikes_per_step']:.3f}")
    
    # 12. Brain comparison (create another brain for comparison)
    print("\n12. Brain Comparison:")
    print("-" * 40)
    brain2 = pynlm.Brain(config)
    if brain2.initialize():
        brain2.step(10)  # Step brain2
        
        comparison = pynlm.utils.compare_brains(brain, brain2, 
                                               metrics=["spike_count", "active_neurons", "ei_ratio"])
        
        print(f"Brain comparison results:")
        print(f"  Has differences: {comparison['has_differences']}")
        print(f"  Metrics compared: {len(comparison['metrics_compared'])}")
        
        if comparison['has_differences']:
            print("  Key differences:")
            for metric, diff in comparison['differences'].items():
                if isinstance(diff, dict) and 'absolute_difference' in diff:
                    print(f"    {metric}: {diff['absolute_difference']:.3f} "
                          f"({diff['percent_change']:.1f}%)")
    
    # 13. File operations
    print("\n13. File Operations:")
    print("-" * 40)
    
    # Save brain state (placeholder - would need actual file I/O)
    test_file = "test_brain_state.json"
    print(f"Attempting to save brain to: {test_file}")
    # Note: In real usage, this would save/load actual state
    
    print("\n" + "=" * 60)
    print("Example completed successfully!")
    print("=" * 60)
    
    # Demonstrate context manager pattern
    print("\n14. Context Manager Pattern:")
    print("-" * 40)
    print("Using simulation manager context (basic demonstration)...")
    # This would use the SimulationManager class from __init__.py
    
    print("\nAll examples completed!")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError in example: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
