#!/usr/bin/env python3
"""
NLM Python Demo - Enhanced Python bindings demonstration

This script demonstrates the enhanced features of the NLM Python bindings,
including convenience methods, batch operations, error handling, and
the new context manager functionality.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src/python'))

import pynlm as nlm
import numpy as np
import time

def demo_basic_functionality():
    """Demonstrate basic NLM brain functionality"""
    print("=== Basic NLM Functionality Demo ===")
    
    # Create a default brain
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    print(f"Total synapses: {brain.get_total_synapse_count()}")
    
    # Demonstrate convenience methods
    potentials = brain.get_neuron_potentials()
    weights = brain.get_synapse_weights()
    firing_rates = brain.get_firing_rates()
    
    print(f"Neuron potentials range: {min(potentials):.3f} to {max(potentials):.3f}")
    print(f"Synapse weights range: {min(weights):.3f} to {max(weights):.3f}")
    print(f"Firing rates range: {min(firing_rates):.3f} to {max(firing_rates):.3f}")
    
    # Demonstrate batch operations
    print("\n=== Batch Operations Demo ===")
    batch_currents = [(nlm.NeuronId(i), 10.0) for i in range(10)] if brain.get_total_neuron_count() >= 10 else []
    brain.inject_current_batch(batch_currents)
    print(f"Injected batch of {len(batch_currents)} currents")
    
    # Demonstrate context manager
    print("\n=== Context Manager Demo ===")
    with brain.create_simulator() as simulator:
        print("Brain simulation context manager entered")
        brain.step(1)
        print(f"Step completed, total spikes: {brain.get_total_spike_count()}")
    
    print("Context manager exited")
    brain.reset()
    brain.initialize()
    
    return brain

def demo_advanced_features():
    """Demonstrate advanced NLM features"""
    print("\n=== Advanced Features Demo ===")
    
    # Create brain with specific configuration
    config = nlm.create_default_config()
    config.set_int("neuron_count", 100)
    config.set_int("region_count", 2)
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
    print(f"Created brain with custom config: {brain.get_total_neuron_count()} neurons, {brain.get_region_count()} regions")
    
    # Demonstrate error handling
    print("\n=== Error Handling Demo ===")
    try:
        # This should work - getting existing region
        region = brain.get_region_by_name("region_1")
        print(f"Found region: {region.get_name()}")
    except nlm.BrainError as e:
        print(f"Expected error caught: {e}")
    
    # Demonstrate system status
    print("\n=== System Status Demo ===")
    brain = nlm.create_brain(nlm.create_default_config())
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(nlm.create_simple_world())
    
    # Enable all adaptive systems
    agent_brain.enable_all_systems()
    print(f"Enabled systems: {agent_brain.get_active_systems()}")
    
    # Get system status
    neuromod, curiosity, novelty, pred_err = agent_brain.get_system_status()
    print(f"Neuromodulation: {neuromod:.3f}, Curiosity: {curiosity:.3f}, Novelty: {novelty:.3f}, Prediction Error: {pred_err:.3f}")
    
    return brain

def demo_pattern_matching():
    """Demonstrate pattern matching and similarity computation"""
    print("\n=== Pattern Matching Demo ===")
    
    # Create patterns for comparison
    pattern1 = np.array([0.1, 0.5, 1.0, 0.8, 0.3, 0.2], dtype=np.float32)
    pattern2 = np.array([0.15, 0.6, 0.95, 0.75, 0.25, 0.1], dtype=np.float32)  # Similar to pattern1
    pattern3 = np.array([0.0, 1.0, 0.0, 0.0, 1.0, 0.0], dtype=np.float32)  # Different from pattern1
    
    # Compute similarity
    similarity_12 = nlm.compute_pattern_similarity(pattern1.tolist(), pattern2.tolist())
    similarity_13 = nlm.compute_pattern_similarity(pattern1.tolist(), pattern3.tolist())
    
    print(f"Pattern 1 vs Pattern 2 similarity: {np.mean(similarity_12):.3f}")
    print(f"Pattern 1 vs Pattern 3 similarity: {np.mean(similarity_13):.3f}")
    
    # Demonstrate validation
    print("\n=== Configuration Validation Demo ===")
    config = nlm.create_default_config()
    try:
        is_valid = config.validate()
        print(f"Configuration validation: {is_valid}")
    except Exception as e:
        print(f"Validation error: {e}")
    
    return pattern1, pattern2, pattern3

def demo_world_simulation():
    """Demonstrate world simulation with agent brain"""
    print("\n=== World Simulation Demo ===")
    
    # Create world and agent brain
    world = nlm.create_simple_world()
    config = nlm.create_default_config()
    config.set_int("neuron_count", 50)
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(world)
    
    # Set up a simple world with objects
    agent_body = world.get_agent_body()
    world.set_agent_start(agent_body.x, agent_body.y)
    
    print(f"World configured: {world.get_width()}x{world.get_height()} pixels")
    print(f"Agent body initialized at ({agent_body.x}, {agent_body.y})")
    
    # Run a few simulation steps
    for step in range(5):
        world.update(0.001)
        print(f"Step {step + 1}: Agent at ({agent_body.x:.1f}, {agent_body.y:.1f}), Speed: {agent_body.speed():.2f}")
    
    return world

def demo_performance_monitoring():
    """Demonstrate performance monitoring capabilities"""
    print("\n=== Performance Monitoring Demo ===")
    
    brain = nlm.create_brain(nlm.create_default_config())
    brain.initialize()
    
    # Simulate some work to measure performance
    start_time = time.time()
    steps_to_run = 100
    
    for step in range(steps_to_run):
        brain.step(step)
        
        # Simulate work every 10 steps
        if step % 10 == 0:
            time.sleep(0.001)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"Completed {steps_to_run} steps in {elapsed:.2f} seconds")
    print(f"Average step time: {elapsed/steps_to_run*1000:.2f} ms")
    print(f"Performance: {steps_to_run/elapsed:.0f} Hz")
    
    return elapsed

def main():
    """Main demo function"""
    print("NLM Python Bindings Enhanced Demo")
    print("=" * 50)
    
    try:
        # Run all demos
        demo_basic_functionality()
        demo_advanced_features()
        demo_pattern_matching()
        demo_world_simulation()
        demo_performance_monitoring()
        
        print("\n" + "=" * 50)
        print("All demos completed successfully!")
        print("\nKey features demonstrated:")
        print("- Enhanced brain API with convenience methods")
        print("- Batch operations for improved performance")
        print("- Context managers for automatic resource management")
        print("- Advanced error handling with custom exceptions")
        print("- Pattern matching and similarity computation")
        print("- Configuration validation")
        print("- World simulation with agent brains")
        print("- Performance monitoring")
        
    except Exception as e:
        print(f"\nDemo failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())