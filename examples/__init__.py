"""Individual NLM Python examples demonstrating various use cases and features.

This module provides a collection of standalone examples for the NLM Python package.
Each example focuses on a specific aspect of the NLM framework and demonstrates
best practices for using the enhanced Python bindings.

Examples included:
1. basic_brain_example.py - Simple brain simulation
2. agent_world_example.py - Complete agent with world interaction
3. learning_example.py - Advanced learning with neuromodulation
4. batch_experiments_example.py - Multiple experiment comparison
5. error_handling_example.py - Error handling and validation
6. benchmark_example.py - Performance benchmarking
7. custom_config_example.py - Custom configuration management

Each example can be run individually using:
    python -m pynlm.examples.example_name

Author: NLM Project Authors
License: MIT
Version: 0.1.0
"""

# Example 1: Basic Brain Simulation
# File: basic_brain_example.py
"""
Basic Brain Simulation Example

This example demonstrates the simplest way to use NLM: creating a brain,
initializing it, and running a simulation.

This is ideal for beginners who want to understand the basic concepts
of neural simulation without getting into world interaction or agent control.
"""

import pynlm

def main():
    """Run a basic brain simulation."""
    print("=== Basic Brain Simulation Example ===")
    
    # Method 1: Create brain with default configuration
    print("\n1. Creating brain with default configuration...")
    brain = pynlm.create_brain_with_default_config()
    brain.initialize()
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Method 2: Create brain with custom configuration
    print("\n2. Creating brain with custom configuration...")
    config = pynlm.create_identity_config()
    config.setFloat("brain.neuron_count", 5000)
    config.setFloat("brain.synapse_density", 0.05)
    brain = pynlm.create_brain_from_config(config)
    brain.initialize()
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    print("\n3. Running simulation...")
    for i in range(100):
        brain.step(i)
        if i % 20 == 0:
            print(f"   Step {i}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Get and display statistics
    print("\n4. Simulation results:")
    stats = pynlm.run_brain_simulation(brain, 100)
    print(f"   Total neurons: {stats['total_neurons']}")
    print(f"   Total synapses: {stats['total_synapses']}")
    print(f"   Firing neurons: {stats['firing_neurons']}")
    print(f"   Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"   Total spikes: {stats['total_spikes']}")
    print(f"   E/I ratio: {stats['excitation_inhibition_ratio']:.3f}")
    print(f"   Developmental stage: {stats['developmental_stage']}")
    
    return stats

if __name__ == "__main__":
    try:
        result = main()
        print(f"\n=== Example completed successfully ===")
    except Exception as e:
        print(f"\n=== Example failed with error: {e} ===")

# Example 2: Complete Agent with World Interaction
# File: agent_world_example.py
"""
Complete Agent with World Interaction Example

This example demonstrates a complete agent setup where a brain interacts
with a simulated world, senses its environment, and takes actions to achieve goals.

This is a more realistic example that shows how NLM agents can navigate,
learn from their environment, and develop over time.
"""

import pynlm

def main():
    """Run a complete agent in a world simulation."""
    print("=== Complete Agent with World Interaction Example ===")
    
    # Create complete agent setup
    print("\n1. Creating complete agent setup...")
    brain, world, agent = pynlm.create_complete_agent(width=30, height=30)
    
    # Enable learning subsystems
    print("\n2. Enabling learning subsystems...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Print initial state
    print("\n3. Initial state:")
    initial_stats = pynlm.validate_agent_setup(brain, world, agent)
    print(f"   Agent valid: {initial_stats['is_valid']}")
    if initial_stats['issues']:
        print(f"   Issues: {initial_stats['issues']}")
    print(f"   Initial curiosity: {initial_stats['agent_curiosity']:.3f}")
    print(f"   Initial novelty: {initial_stats['agent_novelty']:.3f}")
    
    # Run agent episode
    print("\n4. Running agent episode (500 steps)...")
    episode_stats = pynlm.run_agent_episode(agent, world, 500)
    
    print(f"   Steps completed: {episode_stats['steps']}")
    print(f"   Total reward: {episode_stats['total_reward']:.2f}")
    print(f"   Max curiosity: {episode_stats['max_curiosity']:.3f}")
    print(f"   Max novelty: {episode_stats['max_novelty']:.3f}")
    print(f"   Final curiosity: {episode_stats['final_curiosity']:.3f}")
    print(f"   Final novelty: {episode_stats['final_novelty']:.3f}")
    print(f"   Final prediction error: {episode_stats['final_prediction_error']:.3f}")
    
    # Print world state
    print("\n5. Final state:")
    world_stats = pynlm.print_simulation_summary(brain, world, agent)
    print(world_stats)
    
    # Validate final state
    print("\n6. Final validation:")
    final_stats = pynlm.validate_agent_setup(brain, world, agent)
    print(f"   Agent valid: {final_stats['is_valid']}")
    if final_stats['warnings']:
        print(f"   Warnings: {final_stats['warnings']}")
    
    return episode_stats

if __name__ == "__main__":
    try:
        result = main()
        print(f"\n=== Example completed successfully ===")
    except Exception as e:
        print(f"\n=== Example failed with error: {e} ===")

# Example 3: Advanced Learning with Neuromodulation
# File: learning_example.py
"""
Advanced Learning with Neuromodulation Example

This example demonstrates advanced NLM features including reward modulation,
curiosity-driven exploration, and developmental plasticity. It shows how
trained agents can learn from their experiences and adapt over time.
"""

import pynlm
import time

def main():
    """Run advanced learning experiments."""
    print("=== Advanced Learning with Neuromodulation Example ===")
    
    # Create agent with enhanced learning capabilities
    print("\n1. Creating agent with enhanced learning...")
    brain, world, agent = pynlm.create_complete_agent(width=40, height=40)
    
    # Configure learning parameters
    print("\n2. Configuring learning parameters...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Print learning configuration
    print("\n3. Learning configuration:")
    config = brain.getConfig()
    print(f"   Reward modulation: Enabled")
    print(f"   Curiosity: Enabled")
    print(f"   Development: Enabled")
    print(f"   Structural plasticity: Enabled")
    print(f"   Developmental stage: {agent.getDevelopmentalStage()}")
    
    # Run extended learning session
    print("\n4. Running extended learning session (2000 steps)...")
    start_time = time.time()
    
    episode_stats = pynlm.run_agent_episode(agent, world, 2000)
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"   Learning completed in {duration:.2f} seconds")
    print(f"   Steps per second: {episode_stats['steps'] / duration:.1f}")
    
    # Analyze learning patterns
    print("\n5. Learning analysis:")
    neural_activities = episode_stats['neural_activity']
    
    if neural_activities:
        avg_firing = sum(a['firing_neurons'] for a in neural_activities) / len(neural_activities)
        avg_curiosity = sum(a['curiosity'] for a in neural_activities) / len(neural_activities)
        avg_novelty = sum(a['novelty'] for a in neural_activities) / len(neural_activities)
        
        print(f"   Average firing neurons: {avg_firing:.1f}")
        print(f"   Average curiosity level: {avg_curiosity:.3f}")
        print(f"   Average novelty level: {avg_novelty:.3f}")
        
        # Check for learning patterns
        curiosity_increased = episode_stats['final_curiosity'] > avg_curiosity
        novelty_increased = episode_stats['final_novelty'] > avg_novelty
        
        print(f"   Curiosity increased: {curiosity_increased}")
        print(f"   Novelty increased: {novelty_increased}")
        
        if curiosity_increased or novelty_increased:
            print("   → Agent showed exploratory learning behavior")
        else:
            print("   → Agent showed stable behavior")
    
    # Print final summary
    print("\n6. Final learning summary:")
    learning_summary = pynlm.print_simulation_summary(brain, world, agent)
    print(learning_summary)
    
    return episode_stats

if __name__ == "__main__":
    try:
        result = main()
        print(f"\n=== Example completed successfully ===")
    except Exception as e:
        print(f"\n=== Example failed with error: {e} ===")

# Example 4: Batch Experiments and Comparison
# File: batch_experiments_example.py
"""
Batch Experiments and Comparison Example

This example demonstrates how to run multiple experiments in parallel,
compare results, and analyze patterns across different agent configurations.
It's useful for scientific research and performance benchmarking.
"""

import pynlm

def main():
    """Run multiple experiments and compare results."""
    print("=== Batch Experiments and Comparison Example ===")
    
    # Run multiple experiments with different configurations
    print("\n1. Running multiple experiments...")
    experiments = pynlm.run_batch_experiments(num_experiments=3, steps_per_experiment=1000)
    
    # Analyze results
    print("\n2. Experiment analysis:")
    for i, exp in enumerate(experiments):
        print(f"\n   Experiment {i + 1}:")
        print(f"     Steps: {exp['steps']}")
        print(f"     Total reward: {exp['total_reward']:.2f}")
        print(f"     Max curiosity: {exp['max_curiosity']:.3f}")
        print(f"     Final curiosity: {exp['final_curiosity']:.3f}")
        print(f"     Final novelty: {exp['final_novelty']:.3f}")
        print(f"     Final prediction error: {exp['final_prediction_error']:.3f}")
        print(f"     Configuration: {exp['configuration']}")
    
    # Compare experiments
    print("\n3. Comparing experiments:")
    if len(experiments) >= 2:
        # Compare first two experiments
        comparison = pynlm.compare_brain_states(
            experiments[0].get('brain_states', []),
            experiments[1].get('brain_states', [])
        )
        print(f"   Experimental similarity: {comparison['statistical_similarity']:.3f}")
        
        # Find best performing experiment
        best_exp = max(experiments, key=lambda e: e['final_curiosity'] + e['total_reward'])
        print(f"   Best performing experiment: {best_exp['experiment_id']}")
        print(f"     Score: {best_exp['final_curiosity'] + best_exp['total_reward']:.3f}")
    
    # Print aggregated statistics
    print("\n4. Aggregated statistics:")
    total_steps = sum(exp['steps'] for exp in experiments)
    avg_curiosity = sum(exp['final_curiosity'] for exp in experiments) / len(experiments)
    avg_novelty = sum(exp['final_novelty'] for exp in experiments) / len(experiments)
    avg_reward = sum(exp['total_reward'] for exp in experiments) / len(experiments)
    
    print(f"   Total steps: {total_steps}")
    print(f"   Average final curiosity: {avg_curiosity:.3f}")
    print(f"   Average final novelty: {avg_novelty:.3f}")
    print(f"   Average total reward: {avg_reward:.2f}")
    
    return experiments

if __name__ == "__main__":
    try:
        results = main()
        print(f"\n=== Example completed successfully ===")
    except Exception as e:
        print(f"\n=== Example failed with error: {e} ===")

# Example 5: Error Handling and Validation
# File: error_handling_example.py
"""
Error Handling and Validation Example

This example demonstrates how to handle errors gracefully, validate inputs,
and debug issues in NLM simulations. It shows the importance of proper
error checking and provides strategies for troubleshooting.
"""

import pynlm

def main():
    """Demonstrate error handling and validation."""
    print("=== Error Handling and Validation Example ===")
    
    # Test valid setup
    print("\n1. Testing valid setup...")
    try:
        brain, world, agent = pynlm.create_complete_agent()
        validation = pynlm.validate_agent_setup(brain, world, agent)
        
        if validation['is_valid']:
            print("   ✓ Setup is valid")
        else:
            print(f"   ✗ Setup has issues: {validation['issues']}")
        
        if validation['warnings']:
            print(f"   ⚠ Warnings: {validation['warnings']}")
            
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # Test invalid configurations
    print("\n2. Testing invalid configurations...")
    
    # Test with invalid brain parameters
    print("\n   a) Testing with invalid neuron count...")
    try:
        config = pynlm.create_identity_config()
        config.setFloat("brain.neuron_count", -100)  # Invalid negative count
        print("   ✗ Should have thrown exception for negative neuron count")
    except Exception as e:
        print(f"   ✓ Correctly caught exception: {type(e).__name__}")
    
    # Test with invalid world parameters
    print("\n   b) Testing with invalid world dimensions...")
    try:
        world = pynlm.create_simulation_environment(width=-10, height=-10)
        print("   ✗ Should have thrown exception for negative dimensions")
    except Exception as e:
        print(f"   ✓ Correctly caught exception: {type(e).__name__}")
    
    # Test brain simulation error handling
    print("\n   c) Testing simulation error handling...")
    try:
        brain = pynlm.create_brain_with_default_config()
        brain.initialize()
        
        # Run simulation
        for i in range(50):
            brain.step(i)
        
        print("   ✓ Simulation completed successfully")
        
        # Try to run again without proper reset
        print("\n   d) Testing brain state management...")
        try:
            brain.step(100)  # Should work
            print("   ✓ Step completed (brain may not have reset properly)")
        except Exception as e:
            print(f"   ✗ Step failed: {type(e).__name__}: {e}")
            
    except Exception as e:
        print(f"   ✗ Unexpected error in simulation: {e}")
    
    # Demonstrate validation function
    print("\n3. Using validation function...")
    brain, world, agent = pynlm.create_complete_agent()
    validation = pynlm.validate_agent_setup(brain, world, agent)
    
    print(f"   Validation result: {'PASS' if validation['is_valid'] else 'FAIL'}")
    print(f"   Issues: {validation['issues']}")
    print(f"   Warnings: {validation['warnings']}")
    print(f"   Neuron count: {validation['brain_neuron_count']}")
    print(f"   Synapse count: {validation['brain_synapse_count']}")
    
    return validation

if __name__ == "__main__":
    try:
        result = main()
        print(f"\n=== Example completed successfully ===")
    except Exception as e:
        print(f"\n=== Example failed with error: {e} ===")

# Example 6: Performance Benchmarking
# File: benchmark_example.py
"""
Performance Benchmarking Example

This example demonstrates how to benchmark NLM performance, measure execution
speed, and compare performance across different configurations. It's useful for
evaluating system performance and identifying bottlenecks.
"""

import pynlm
import time

def main():
    """Benchmark NLM performance."""
    print("=== Performance Benchmarking Example ===")
    
    # Test different brain sizes
    brain_sizes = [100, 500, 1000, 2000]
    
    print("\n1. Benchmarking different brain sizes...")
    results = []
    
    for size in brain_sizes:
        print(f"\n   Testing brain size: {size} neurons")
        
        # Create brain
        start_time = time.time()
        brain = pynlm.create_brain_with_default_config(size)
        brain.initialize()
        init_time = time.time() - start_time
        
        # Run simulation
        steps = 500
        start_time = time.time()
        for i in range(steps):
            brain.step(i)
        sim_time = time.time() - start_time
        
        # Calculate statistics
        avg_steps_per_second = steps / sim_time
        neurons_per_second = size / sim_time
        
        result = {
            'neurons': size,
            'initialization_time': init_time,
            'simulation_time': sim_time,
            'steps': steps,
            'avg_steps_per_second': avg_steps_per_second,
            'neurons_per_second': neurons_per_second,
            'avg_firing_rate': brain.getAverageFiringRate()
        }
        
        results.append(result)
        
        print(f"     Initialization: {init_time:.4f}s")
        print(f"     Simulation: {sim_time:.4f}s")
        print(f"     Steps/sec: {avg_steps_per_second:.1f}")
        print(f"     Neurons/sec: {neurons_per_second:.1f}")
        print(f"     Avg firing rate: {result['avg_firing_rate']:.2f} Hz")
    
    # Find best performing configuration
    print("\n2. Performance analysis:")
    best_perf = max(results, key=lambda r: r['avg_steps_per_second'])
    largest_perf = max(results, key=lambda r: r['neurons_per_second'])
    
    print(f"   Best throughput: {best_perf['neurons']} neurons ({best_perf['avg_steps_per_second']:.1f} steps/sec)")
    print(f"   Best neuron throughput: {largest_perf['neurons']} neurons ({largest_perf['neurons_per_second']:.1f} neurons/sec)")
    
    # Print performance table
    print("\n3. Performance summary:")
    print("   Neurons | Init (s) | Sim (s) | Steps/sec | Neurons/sec | Firing Rate (Hz)")
    print("   --------|----------|---------|-----------|-------------|-----------------")
    for r in results:
        print(f"   {r['neurons']:7d} | {r['initialization_time']:8.4f} | {r['simulation_time']:6.4f} | "
              f"{r['avg_steps_per_second']:9.1f} | {r['neurons_per_second']:13.1f} | {r['avg_firing_rate']:17.2f}")
    
    # Memory efficiency analysis
    print("\n4. Memory efficiency analysis:")
    total_memory = sum(r['neurons'] * 0.01 for r in results)  # Estimate (100 bytes per neuron)
    total_time = sum(r['simulation_time'] for r in results)
    efficiency = total_memory / total_time if total_time > 0 else 0
    
    print(f"   Total neurons processed: {sum(r['neurons'] for r in results)}")
    print(f"   Total simulation time: {total_time:.4f}s")
    print(f"   Estimated memory efficiency: {efficiency:.1f} neurons/sec")
    
    return results

if __name__ == "__main__":
    try:
        results = main()
        print(f"\n=== Example completed successfully ===")
    except Exception as e:
        print(f"\n=== Example failed with error: {e} ===")

# Example 7: Custom Configuration Management
# File: custom_config_example.py
"""
Custom Configuration Management Example

This example demonstrates how to create and manage custom NLM configurations,
test different parameter combinations, and find optimal settings for specific
use cases.
"""

import pynlm

def main():
    """Demonstrate custom configuration management."""
    print("=== Custom Configuration Management Example ===")
    
    # Create base configuration
    print("\n1. Creating base configuration...")
    base_config = pynlm.create_identity_config()
    print(f"   Base neurons: {base_config.getFloat('brain.neuron_count'):.0f}")
    print(f"   Base synapse density: {base_config.getFloat('brain.synapse_density'):.3f}")
    print(f"   Base threshold: {base_config.getFloat('brain.v_thresh'):.1f}")
    
    # Test different parameter combinations
    print("\n2. Testing parameter combinations...")
    
    parameter_sets = [
        {"name": "Small, Sparse", "neurons": 500, "density": 0.01, "threshold": -55},
        {"name": "Medium, Standard", "neurons": 1000, "density": 0.1, "threshold": -50},
        {"name": "Large, Dense", "neurons": 2000, "density": 0.2, "threshold": -45},
        {"name": "High Threshold", "neurons": 1500, "density": 0.05, "threshold": -40},
        {"name": "Low Threshold", "neurons": 800, "density": 0.15, "threshold": -60}
    ]
    
    results = []
    
    for param_set in parameter_sets:
        print(f"\n   Testing {param_set['name']} configuration...")
        
        # Create configuration
        config = pynlm.create_identity_config()
        config.setFloat("brain.neuron_count", param_set['neurons'])
        config.setFloat("brain.synapse_density", param_set['density'])
        config.setFloat("brain.v_thresh", param_set['threshold'])
        
        # Create and test brain
        brain = pynlm.create_brain_from_config(config)
        brain.initialize()
        
        # Run quick simulation
        for i in range(100):
            brain.step(i)
        
        # Collect statistics
        result = {
            'name': param_set['name'],
            'neurons': param_set['neurons'],
            'density': param_set['density'],
            'threshold': param_set['threshold'],
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        
        results.append(result)
        
        print(f"     Total spikes: {result['total_spikes']}")
        print(f"     Firing neurons: {result['firing_neurons']}")
        print(f"     Avg firing rate: {result['avg_firing_rate']:.2f} Hz")
        print(f"     E/I ratio: {result['e_i_ratio']:.3f}")
        print(f"     Dev stage: {result['developmental_stage']}")
    
    # Find optimal configurations
    print("\n3. Finding optimal configurations...")
    
    # Best for high firing rate
    best_firing = max(results, key=lambda r: r['avg_firing_rate'])
    print(f"   Best firing rate: {best_firing['name']}")
    print(f"     Rate: {best_firing['avg_firing_rate']:.2f} Hz")
    
    # Best for low variance (stable)
    # Calculate coefficient of variation for firing rate
    firing_rates = [r['avg_firing_rate'] for r in results]
    avg_firing = sum(firing_rates) / len(firing_rates)
    firing_variance = sum((rate - avg_firing) ** 2 for rate in firing_rates) / len(firing_rates)
    cv = (firing_variance ** 0.5) / avg_firing if avg_firing > 0 else 0
    
    # Find most stable configuration (lowest CV)
    stable_configs = sorted(results, key=lambda r: abs(r['avg_firing_rate'] - avg_firing))
    most_stable = stable_configs[0]
    print(f"   Most stable: {most_stable['name']}")
    print(f"     Firing rate: {most_stable['avg_firing_rate']:.2f} Hz (CV: {cv:.3f})")
    
    # Best for high total spikes (activity)
    best_activity = max(results, key=lambda r: r['total_spikes'])
    print(f"   Best activity: {best_activity['name']}")
    print(f"     Total spikes: {best_activity['total_spikes']}")
    
    # Print configuration summary
    print("\n4. Configuration summary:")
    print("   Name | Neurons | Density | Threshold | Spikes | Firing Rate | E/I Ratio")
    print("   ------|---------|---------|-----------|--------|-------------|----------")
    for r in results:
        print(f"   {r['name']:6s} | {r['neurons']:7d} | {r['density']:7.3f} | {r['threshold']:9.1f} | "
              f"{r['total_spikes']:5d} | {r['avg_firing_rate']:11.2f} | {r['e_i_ratio']:8.3f}")
    
    return results

if __name__ == "__main__":
    try:
        results = main()
        print(f"\n=== Example completed successfully ===")
    except Exception as e:
        print(f"\n=== Example failed with error: {e} ===")

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Project Authors"

__all__ = ['main']