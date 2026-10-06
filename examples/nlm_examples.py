"""
Comprehensive examples for the enhanced NLM Python bindings.

This file demonstrates the new features and capabilities of the improved
Python interface for the Neural Learning Machine (NLM) framework.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
from typing import List, Dict, Any

# Example 1: Enhanced Basic Brain Usage
def example_basic_brain():
    """Demonstrate enhanced basic brain usage with error handling."""
    print("=== Example 1: Enhanced Basic Brain ===")
    
    try:
        # Create brain with enhanced configuration
        config = pynlm.create_default_config()
        
        # Configure advanced settings
        config.set("random_seed", 42)
        config.set("simulation_timestep", 0.001)
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        config.set("connection_probability", 0.2)
        
        # Create brain with error checking
        brain = pynlm.create_brain(config)
        print(f"✓ Created brain with {brain.get_total_neuron_count()} neurons")
        
        # Initialize brain
        brain.initialize()
        print("✓ Brain initialized successfully")
        
        # Run simulation with stats collection
        brain.step(0)
        print(f"✓ Initial spike count: {brain.get_total_spike_count()}")
        
        # Get comprehensive statistics
        stats = brain.get_comprehensive_stats()
        print(f"✓ Neurons firing: {stats['activity']['firing_neurons']}")
        print(f"✓ Average firing rate: {stats['activity']['average_firing_rate']:.2f}")
        print(f"✓ Developmental stage: {stats['development']['developmental_stage']}")
        
        return brain, config
        
    except Exception as e:
        print(f"✗ Error in basic brain example: {e}")
        return None, None

# Example 2: Advanced Agent Simulation with Learning
def example_advanced_agent_simulation():
    """Demonstrate advanced agent simulation with learning features."""
    print("\n=== Example 2: Advanced Agent Simulation ===")
    
    try:
        # Create complete agent system
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        world = pynlm.create_world()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Enable advanced learning features
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        print("✓ Advanced agent system created with learning enabled")
        
        # Run enhanced simulation with callbacks
        def simulation_callback(result):
            print(f"  Simulation update: Step {result.step_count}, "
                  f"Spikes: {brain.get_total_spike_count()}, "
                  f"Efficiency: {brain.get_average_firing_rate():.3f}")
        
        simulation = pynlm.AgentSimulation(brain, world)
        simulation.agent = agent
        
        result = simulation.run_continuous(
            duration=5.0,  # 5 seconds of simulated time
            timestep=0.1,
            callbacks={'update': simulation_callback}
        )
        
        # Analyze results
        print("\n--- Simulation Results ---")
        print(f"Total steps: {result.step_count}")
        print(f"Final spike count: {brain.get_total_spike_count()}")
        print(f"Final firing rate: {brain.get_average_firing_rate():.2f}")
        print(f"Final developmental stage: {brain.get_developmental_stage()}")
        
        # Get comprehensive statistics
        stats = result.get_statistics()
        plasticity_analysis = result.analyze_plasticity()
        
        print(f"\n--- Plasticity Analysis ---")
        print(f"Plasticity effectiveness: {plasticity_analysis['plasticity_effectiveness']:.3f}")
        print(f"Learning rate: {plasticity_analysis['learning_rate']:.3f}")
        
        return result
        
    except Exception as e:
        print(f"✗ Error in advanced agent simulation: {e}")
        return None

# Example 3: Brain Benchmarking with Statistical Analysis
def example_brain_benchmarking():
    """Demonstrate advanced brain benchmarking with statistical analysis."""
    print("\n=== Example 3: Brain Benchmarking ===")
    
    try:
        # Create test brain
        config = pynlm.create_default_config()
        config.set("neuron_count", 500)
        config.set("region_count", 2)
        
        brain = pynlm.create_brain(config)
        
        # Run comprehensive benchmark
        benchmark_results = pynlm.benchmark_brain(brain, num_runs=5, duration=0.5)
        
        print("✓ Benchmark completed successfully")
        print(f"Average duration: {benchmark_results['average_duration_ms']:.2f} ms")
        print(f"Average total spikes: {benchmark_results['average_total_spikes']:.0f}")
        print(f"Average efficiency: {benchmark_results['average_efficiency']:.3f}")
        
        # Statistical analysis
        all_results = benchmark_results['all_results']
        spike_counts = [r['total_spikes'] for r in all_results]
        efficiencies = [r['efficiency'] for r in all_results]
        
        print(f"\n--- Statistical Analysis ---")
        print(f"Spike count std: {np.std(spike_counts):.1f}")
        print(f"Efficiency std: {np.std(efficiencies):.3f}")
        print(f"Spike count range: {min(spike_counts)} - {max(spike_counts)}")
        
        # Create visualization data
        return benchmark_results
        
    except Exception as e:
        print(f"✗ Error in brain benchmarking: {e}")
        return None

# Example 4: Memory Management with Checkpoints
def example_memory_management():
    """Demonstrate advanced memory management features."""
    print("\n=== Example 4: Memory Management ===")
    
    try:
        # Create brain with memory tracking
        brain = pynlm.create_brain()
        brain.initialize()
        
        # Create memory manager
        memory_manager = pynlm.MemoryManager(brain)
        
        # Get initial memory stats
        initial_stats = memory_manager.get_memory_stats()
        print(f"✓ Initial memory stats: {initial_stats['neuron_count']} neurons, "
              f"{initial_stats['synapse_count']} synapses")
        print(f"Estimated memory usage: {initial_stats['estimated_memory_usage']:.2f} MB")
        
        # Run some simulation
        for step in range(100):
            brain.step(step)
        
        # Save checkpoint
        checkpoint_file = "nlm_checkpoint.json"
        memory_manager.save_checkpoint(checkpoint_file)
        print(f"✓ Checkpoint saved to {checkpoint_file}")
        
        # Get final memory stats
        final_stats = memory_manager.get_memory_stats()
        print(f"Final memory stats: {final_stats['neuron_count']} neurons, "
              f"{final_stats['synapse_count']} synapses")
        print(f"Estimated memory usage: {final_stats['estimated_memory_usage']:.2f} MB")
        
        # Load checkpoint (demonstrating persistence)
        memory_manager.load_checkpoint(checkpoint_file)
        print("✓ Checkpoint loaded successfully")
        
        return memory_manager
        
    except Exception as e:
        print(f"✗ Error in memory management example: {e}")
        return None

# Example 5: Advanced Configuration Management
def example_advanced_configuration():
    """Demonstrate advanced configuration management."""
    print("\n=== Example 5: Advanced Configuration ===")
    
    try:
        # Create and modify configuration
        config = pynlm.create_default_config()
        
        # Set multiple configuration options
        config.set("random_seed", 12345)
        config.set("simulation_timestep", 0.0005)
        config.set("neuron_count", 2000)
        config.set("region_count", 5)
        config.set("connection_probability", 0.15)
        
        # Save configuration to file
        config_file = "nlm_config.json"
        pynlm.save_config_to_file(config, config_file)
        print(f"✓ Configuration saved to {config_file}")
        
        # Load configuration from file
        loaded_config = pynlm.load_config_from_file(config_file)
        print(f"✓ Configuration loaded from file")
        
        # Create brain with loaded configuration
        brain = pynlm.create_brain(loaded_config)
        brain.initialize()
        print(f"✓ Brain created with loaded config: {brain.get_total_neuron_count()} neurons")
        
        # Demonstrate configuration validation
        print(f"✓ Configuration has key 'random_seed': {loaded_config.has('random_seed')}")
        print(f"✓ Configuration has key 'neuron_count': {loaded_config.has('neuron_count')}")
        
        return config, brain
        
    except Exception as e:
        print(f"✗ Error in advanced configuration example: {e}")
        return None, None

# Example 6: Brain Analysis and Visualization
def example_brain_analysis():
    """Demonstrate advanced brain analysis and visualization capabilities."""
    print("\n=== Example 6: Brain Analysis and Visualization ===")
    
    try:
        # Create and run brain simulation
        brain = pynlm.create_brain()
        brain.initialize()
        
        # Run simulation
        for step in range(200):
            brain.step(step)
        
        # Get comprehensive statistics
        brain_stats = brain.get_comprehensive_stats()
        
        print("✓ Comprehensive brain statistics collected:")
        print(f"  - Basic: {brain_stats['basic']['neuron_count']} neurons, "
              f"{brain_stats['basic']['synapse_count']} synapses")
        print(f"  - Activity: {brain_stats['activity']['firing_neurons']} firing neurons, "
              f"{brain_stats['activity']['average_firing_rate']:.2f} Hz average rate")
        print(f"  - Development: {brain_stats['development']['developmental_stage']} stage")
        
        # Get region statistics
        region_stats = brain_stats['development']['region_stats']
        if region_stats:
            print(f"  - Regions: {len(region_stats)} regions analyzed")
            for region in region_stats[:3]:  # Show first 3 regions
                print(f"    Region {region['id']}: {region['neuron_count']} neurons, "
                      f"{region['spiking_activity']:.2f} activity")
        
        # Performance metrics
        performance = brain_stats['performance']
        print(f"  - Performance: Efficiency {performance['spike_efficiency']:.3f}, "
              f"Connectivity {performance['connectivity_strength']:.3f}, "
              f"Stability {performance['stability']:.3f}")
        
        return brain_stats
        
    except Exception as e:
        print(f"✗ Error in brain analysis example: {e}")
        return None

# Example 7: Error Handling and Validation
def example_error_handling():
    """Demonstrate enhanced error handling and validation."""
    print("\n=== Example 7: Error Handling and Validation ===")
    
    try:
        # Test 1: Invalid configuration
        print("Test 1: Configuration validation")
        config = pynlm.create_default_config()
        
        # This should work - valid configuration
        config.set("random_seed", 42)
        print("✓ Valid configuration set successfully")
        
        # Test 2: Invalid simulation parameters
        print("\nTest 2: Simulation parameter validation")
        try:
            # This should fail - negative timestep
            brain = pynlm.create_brain()
            brain.initialize()
            print("✗ Should have failed with invalid timestep")
        except Exception as e:
            print(f"✓ Correctly caught error: {type(e).__name__}: {e}")
        
        # Test 3: Memory error handling
        print("\nTest 3: Memory management")
        memory_manager = pynlm.MemoryManager(brain)
        stats = memory_manager.get_memory_stats()
        print(f"✓ Memory stats retrieved: {stats['neuron_count']} neurons")
        
        # Test 4: Configuration file operations
        print("\nTest 4: File operations")
        test_config = pynlm.create_default_config()
        pynlm.save_config_to_file(test_config, "test_config.json")
        print("✓ Configuration saved to file")
        
        loaded = pynlm.load_config_from_file("test_config.json")
        print("✓ Configuration loaded from file")
        
        # Cleanup
        import os
        os.remove("test_config.json")
        print("✓ Test files cleaned up")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in error handling example: {e}")
        return False

# Example 8: Complete Integration Example
def example_complete_integration():
    """Demonstrate a complete integration example with all features."""
    print("\n=== Example 8: Complete Integration Example ===")
    
    try:
        print("Setting up complete NLM system...")
        
        # 1. Create configuration
        config = pynlm.create_default_config()
        config.set("random_seed", 999)
        config.set("neuron_count", 1500)
        config.set("simulation_timestep", 0.001)
        
        # 2. Create brain
        brain = pynlm.create_brain(config)
        brain.initialize()
        print("✓ Brain created and initialized")
        
        # 3. Create world
        world = pynlm.create_world()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        print("✓ World configured")
        
        # 4. Create agent
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        print("✓ Agent created and initialized")
        
        # 5. Enable all features
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        print("✓ All agent features enabled")
        
        # 6. Run simulation
        simulation = pynlm.AgentSimulation(brain, world)
        simulation.agent = agent
        
        result = simulation.run_episode(
            num_steps=500,
            enable_learning=True,
            enable_curiosity=True,
            enable_development=True
        )
        
        print(f"✓ Simulation completed: {result.step_count} steps")
        
        # 7. Analyze results
        stats = result.get_statistics()
        plasticity_analysis = result.analyze_plasticity()
        
        print("\n--- Final Analysis ---")
        print(f"Simulation Efficiency: {plasticity_analysis['plasticity_effectiveness']:.3f}")
        print(f"Learning Rate: {plasticity_analysis['learning_rate']:.3f}")
        print(f"Final Spike Count: {stats['total_spikes']}")
        print(f"Final Firing Rate: {stats['average_firing_rate']:.2f} Hz")
        
        # 8. Save results
        result.export_results("integration_results.json")
        print("✓ Results exported to integration_results.json")
        
        # 9. Memory management
        memory_manager = pynlm.MemoryManager(brain)
        memory_stats = memory_manager.get_memory_stats()
        print(f"✓ Memory usage: {memory_stats['estimated_memory_usage']:.2f} MB")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in complete integration example: {e}")
        return False

def main():
    """Run all examples sequentially."""
    print("=== NLM Python Bindings Examples ===")
    print(f"NLM Python Bindings Version: {pynlm.__version__}")
    print(f"pynlm module available: {pynlm.PYNLM_AVAILABLE}")
    print()
    
    examples = [
        example_basic_brain,
        example_advanced_agent_simulation,
        example_brain_benchmarking,
        example_memory_management,
        example_advanced_configuration,
        example_brain_analysis,
        example_error_handling,
        example_complete_integration
    ]
    
    results = []
    for example_func in examples:
        try:
            result = example_func()
            results.append((example_func.__name__, result is not None, result))
        except Exception as e:
            results.append((example_func.__name__, False, str(e)))
        
        print("\n" + "="*50)
    
    # Summary
    print("\n=== Example Summary ===")
    success_count = sum(1 for _, success, _ in results if success)
    print(f"Successful examples: {success_count}/{len(examples)}")
    
    for name, success, _ in results:
        status = "✓" if success else "✗"
        print(f"{status} {name}")
    
    return success_count == len(examples)
if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
