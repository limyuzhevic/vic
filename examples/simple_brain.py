#!/usr/bin/env python3
"""
Example 1: Simple Brain Creation and Simulation
===================================================

This is the most basic example of using NLM (Neural Learning Machine).
It demonstrates how to create a virtual brain, initialize it, and run a simple simulation.

Learning Objectives:
- Create a brain using NLM's Python API
- Initialize the brain before use
- Run a simulation loop
- Monitor brain activity statistics
- Understand basic brain metrics

Expected Time: ~30 minutes
Difficulty: Beginner
"""

import pynlm
import time
import sys

def create_simple_brain(neuron_count=1000):
    """
    Create a simple brain with specified neuron count.
    
    Args:
        neuron_count: Number of neurons in the brain (default: 1000)
    
    Returns:
        Initialized brain object
    """
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Configure brain parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    # Create brain with configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def run_basic_simulation(brain, num_steps=100):
    """
    Run a basic simulation with a brain.
    
    Args:
        brain: The brain object to simulate
        num_steps: Number of simulation steps to run
    
    Returns:
        Dictionary with simulation statistics
    """
    print(f"Running simulation for {num_steps} steps...")
    
    # Track simulation statistics
    stats = {
        'steps_completed': 0,
        'max_firing_neurons': 0,
        'total_spikes': 0,
        'avg_firing_rate': 0.0,
        'start_time': time.time(),
        'step_times': []
    }
    
    # Run simulation
    for step in range(num_steps):
        step_start = time.time()
        
        try:
            # Run brain step
            brain.step(step)
            
            # Collect statistics
            firing_count = brain.getFiringNeuronCount()
            spike_count = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            
            # Update statistics
            stats['steps_completed'] += 1
            stats['max_firing_neurons'] = max(stats['max_firing_neurons'], firing_count)
            stats['total_spikes'] += spike_count
            stats['step_times'].append(time.time() - step_start)
            
            # Print progress every 10 steps
            if step % 10 == 0:
                print(f"  Step {step:4d}: "
                      f"{firing_count:4d} firing neurons, "
                      f"{spike_count:6d} total spikes, "
                      f"{firing_rate:6.2f} firing rate")
                
        except Exception as e:
            print(f"Error at step {step}: {e}")
            break
    
    # Calculate final statistics
    end_time = time.time()
    total_time = end_time - stats['start_time']
    
    if stats['steps_completed'] > 0:
        stats['avg_firing_rate'] = stats['total_spikes'] / stats['steps_completed']
        stats['avg_step_time'] = sum(stats['step_times']) / len(stats['step_times'])
        stats['steps_per_second'] = stats['steps_completed'] / total_time
    
    return stats

def analyze_brain_results(brain, stats):
    """
    Analyze and print brain simulation results.
    
    Args:
        brain: The brain object
        stats: Statistics from simulation run
    """
    print("\n" + "="*60)
    print("SIMULATION RESULTS ANALYSIS")
    print("="*60)
    
    # Basic brain information
    print(f"\nBrain Configuration:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    
    # Developmental stage
    dev_stage = brain.getDevelopmentalStage()
    print(f"  Developmental stage: {dev_stage}")
    
    # Simulation statistics
    print(f"\nSimulation Performance:")
    print(f"  Steps completed: {stats['steps_completed']}")
    print(f"  Total time: {stats['steps_completed'] / stats['steps_per_second']:.2f} seconds")
    print(f"  Steps per second: {stats['steps_per_second']:.2f}")
    print(f"  Average step time: {stats['avg_step_time']*1000:.2f} ms")
    
    # Activity statistics
    print(f"\nNeural Activity:")
    print(f"  Maximum firing neurons: {stats['max_firing_neurons']}")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.4f} spikes/step")
    print(f"  Average neurons firing: {brain.getAverageFiringRate():.2f}")
    
    # Connectivity analysis
    excitation_inhibition = brain.getExcitationInhibitionRatio()
    print(f"\nNetwork Properties:")
    print(f"  Excitation/Inhibition ratio: {excitation_inhibition:.4f}")
    
    if excitation_inhibition > 2.0:
        print("  Network status: Strong excitation (potentially unstable)")
    elif excitation_inhibition < 0.5:
        print("  Network status: Strong inhibition (potentially underactive)")
    else:
        print("  Network status: Balanced excitation/inhibition")
    
    # Performance recommendations
    print(f"\nPerformance Recommendations:")
    if stats['steps_per_second'] < 100:
        print("  - Simulation speed is slow. Consider increasing timestep.")
    if stats['max_firing_neurons'] < brain.getTotalNeuronCount() * 0.01:
        print("  - Neural activity is low. Increase input current or adjust thresholds.")
    if excitation_inhibition > 3.0 or excitation_inhibition < 0.33:
        print("  - Network balance is poor. Adjust plasticity parameters.")

def main():
    """
    Main function demonstrating basic NLM usage.
    """
    print("NLM Simple Brain Example")
    print("="*50)
    print("This example demonstrates basic brain creation and simulation.")
    print()
    
    try:
        # Create simple brain
        print("Creating brain...")
        brain = create_simple_brain(neuron_count=500)
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation
        print("\nRunning simulation...")
        stats = run_basic_simulation(brain, num_steps=50)
        
        # Analyze results
        analyze_brain_results(brain, stats)
        
        print("\n" + "="*60)
        print("EXAMPLE COMPLETED SUCCESSFULLY")
        print("="*60)
        
    except Exception as e:
        print(f"\n❌ ERROR: {e}")
        print("Please check your NLM installation and try again.")
        sys.exit(1)

if __name__ == "__main__":
    main()
