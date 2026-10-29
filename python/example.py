#!/usr/bin/env python3
"""
Example script demonstrating the complete NLM Python API.

This script shows how to use the NLM Python bindings to create,
configure, and run neural simulations.
"""

import time
from nlm import (
    Config,
    Brain,
    NeuralNetwork,
    get_version,
    set_log_level,
)
def main_example():
    """Main example demonstrating NLM Python API."""
    print("NLM Python API Example")
    print("=" * 50)
    
    # Check version
    print(f"NLM Version: {get_version()}")
    print()
    
    # Create configuration
    print("1. Creating configuration...")
    config = Config()
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.15)
    config.set("simulation.timestep", 0.001)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    
    print("   Configuration created with:")
    print(f"   - {config.get('brain.neuron_count')} neurons")
    print(f"   - Connection probability: {config.get('brain.connection_probability')}")
    print(f"   - Time step: {config.get('simulation.timestep')}")
    print()
    
    # Create brain
    print("2. Creating brain...")
    brain = Brain(config)
    brain.initialize()
    
    print(f"   Brain initialized successfully!")
    print(f"   Total neurons: {brain.get_total_neuron_count()}")
    print(f"   Total synapses: {brain.get_total_synapse_count()}")
    print()
    
    # Run simulation
    print("3. Running simulation...")
    print("   Step | Spikes | Firing | Avg Rate | Learning")
    print("   ------|--------|--------|----------|---------")
    
    for step in range(100):
        # Record initial state
        initial_spikes = brain.get_total_spike_count()
        initial_firing = brain.get_firing_neuron_count()
        initial_time = time.time()
        
        # Run one step
        brain.step(step, step * 0.001)
        
        # Record final state
        final_spikes = brain.get_total_spike_count()
        final_firing = brain.get_firing_neuron_count()
        step_time = time.time() - initial_time
        
        # Calculate metrics
        spikes_generated = final_spikes - initial_spikes
        avg_rate = spikes_generated / 0.001  # spikes per second
        learning_detected = (abs(final_firing - initial_firing) > 1) or (spikes_generated > 0)
        
        # Print progress (every 10 steps)
        if step % 10 == 0:
            print(f"   {step:5d} | {spikes_generated:6d} | {final_firing:7d} | "
                  f"{avg_rate:8.1f} | {learning_detected}")
    
    print("   -----|--------|--------|----------|---------")
    
    # Get final status
    print("\n4. Final brain status:")
    status = brain.get_status()
    
    print(f"   Step count: {status['step_count']}")
    print(f"   Total neurons: {status['total_neurons']}")
    print(f"   Total spikes: {status['total_spikes']}")
    print(f"   Firing neurons: {status['firing_neurons']}")
    print(f"   Total synapses: {status['total_synapses']}")
    print(f"   Average firing rate: {status['average_firing_rate']:.2f} Hz")
    print(f"   Excitation/Inhibition ratio: {status['excitation_inhibition_ratio']:.2f}")
    print()
    
    # Run with neural network interface
    print("5. Testing NeuralNetwork interface...")
    neural_net = NeuralNetwork(brain)
    
    # Run an experiment
    results = neural_net.run_experiment("plasticity_test", {
        'steps': 500,
        'initial_weights': 0.0,
        'final_weights': 1.0,
    })
    
    print(f"   Experiment results:")
    print(f"   - Type: {results['experiment_type']}")
    print(f"   - Steps: {results['steps_completed']}")
    print(f"   - Initial weight: {results['initial_weights']}")
    print(f"   - Final weight: {results['final_weights']}")
    print(f"   - Weight change: {results['weight_change']}")
    print(f"   - Learning detected: {results['learning_detected']}")
    
    # Analyze results
    analysis = neural_net.analyze_results(results)
    print(f"   - Performance: {analysis['performance_rating']}")
    print(f"   - Key metrics: {analysis['key_metrics']}")
    print(f"   - Recommendations: {analysis['recommendations']}")
    
    # Save/load demonstration
    print("\n6. Testing checkpoint functionality...")
    checkpoint_file = "nlm_checkpoint.bin"
    
    # Save checkpoint
    brain.save(checkpoint_file)
    print(f"   Checkpoint saved to {checkpoint_file}")
    
    # Reset brain
    brain.reset()
    brain.initialize()
    print("   Brain reset and re-initialized")
    
    # Load checkpoint
    brain.load(checkpoint_file)
    print("   Checkpoint loaded successfully")
    
    print("\nExample completed successfully!")
    print("\nThe NLM Python API provides:")
    print("  ✓ Easy configuration and brain creation")
    print("  ✓ High-level neural network interface")
    print("  ✓ Experiment running and analysis tools")
    print("  ✓ Checkpoint save/load functionality")
    print("  ✓ Comprehensive statistics and monitoring")
if __name__ == "__main__":
    main_example()
