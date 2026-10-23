"""
Example 1: Basic Brain Simulation
Creates a simple brain and runs a basic simulation with sensory input.
"""

import sys
sys.path.insert(0, 'python')

import pynlm
import time

def basic_brain_simulation():
    """Basic example demonstrating brain creation and simulation"""
    print("=== NLM Basic Brain Simulation Example ===\n")
    
    # Create default configuration
    config = pynlm.create_default_config()
    
    # Customize configuration for this experiment
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    
    print(f"Configuration:")
    print(f"  Neurons: {config.getOr('neuron_count', 1000)}")
    print(f"  Regions: {config.getOr('region_count', 1)}")
    print(f"  Connection probability: {config.getOr('connection_probability', 0.1)}")
    print()
    
    # Create brain
    print("Creating brain...")
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print(f"Brain initialized successfully")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print()
    
    # Run simulation
    print("Running simulation for 1000 steps...")
    start_time = time.time()
    
    for step in range(1000):
        # In this example, we're just running the brain without external input
        brain.step(step)
        
        # Log progress every 200 steps
        if step % 200 == 0:
            firing_rate = brain.getAverageFiringRate()
            spike_count = brain.getFiringNeuronCount()
            print(f"  Step {step:4d}: Firing rate={firing_rate:6.3f}, "
                  f"Firing neurons={spike_count:4d}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nSimulation complete in {duration:.2f} seconds")
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print()
    
    return brain

if __name__ == "__main__":
    brain = basic_brain_simulation()
    print("Example completed successfully!")
