"""
Simple Simulation - Basic Brain Dynamics
=========================================

This example demonstrates basic brain simulation functionality.
It shows how to run a simple simulation and observe neural activity.

Key Concepts Demonstrated:
- Brain creation with default configuration
- Brain initialization
- Step-by-step neural simulation
- Spike count monitoring

Expected Output:
---------------
Starting simple simulation...
Brain initialized with 500 neurons
Running simulation for 100 steps...
Simulation complete: 847 spikes
Average firing rate: 8.47 Hz
"""

import pynlm

def main():
    print("Starting simple simulation...")
    
    # Create brain using the enhanced API
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    initial_neurons = brain.getTotalNeuronCount()
    print(f"Brain initialized with {initial_neurons} neurons")
    
    # Run simulation
    num_steps = 100
    print(f"Running simulation for {num_steps} steps...")
    
    total_spikes = 0
    for step in range(num_steps):
        brain.step(step)
        total_spikes += brain.getTotalSpikeCount()
        
        # Print progress every 20 steps
        if step % 20 == 0 and step > 0:
            current_spikes = brain.getTotalSpikeCount() - total_spikes + brain.getTotalSpikeCount()
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"{brain.getAverageFiringRate():.2f} Hz avg rate")
    
    # Get final statistics
    final_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    
    print(f"Simulation complete: {final_spikes} spikes")
    print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")

if __name__ == "__main__":
    main()