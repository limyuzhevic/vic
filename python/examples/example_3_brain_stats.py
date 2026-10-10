"""
Example 3: Brain Features and Statistics

This example demonstrates how to monitor and analyze brain activity.
This matches the "Get Brain Stats" example from the documentation.
"""

import pynlm as nlm

def main():
    print("=== Example 3: Brain Features and Statistics ===\n")
    
    # Create and initialize brain
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Add some monitoring within the simulation loop
    print("Running simulation with monitoring...")
    for step in range(100):
        brain.step(step)
        
        # Just add these inside your loop (from documentation):
        print(f"Step {step:3d}: Neurons firing: {brain.getFiringNeuronCount():4d}, "
              f"Total spikes: {brain.getTotalSpikeCount():5d}, "
              f"Avg firing rate: {brain.getAverageFiringRate():5.2f}, "
              f"Dev stage: {brain.getDevelopmentalStage()}")
    
    print("\n=== Example 3 Complete ===\n")

def advanced_monitoring_example():
    """Advanced example with comprehensive monitoring."""
    print("=== Advanced Brain Monitoring Example ===\n")
    
    # Create brain
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Monitoring data collection
    stats = {
        'steps': [],
        'firing_neuron_counts': [],
        'total_spike_counts': [],
        'firing_rates': [],
        'development_stages': []
    }
    
    print("Running simulation with detailed monitoring...")
    for step in range(200):
        brain.step(step)
        
        # Collect statistics
        stats['steps'].append(step)
        stats['firing_neuron_counts'].append(brain.getFiringNeuronCount())
        stats['total_spike_counts'].append(brain.getTotalSpikeCount())
        stats['firing_rates'].append(brain.getAverageFiringRate())
        stats['development_stages'].append(brain.getDevelopmentalStage())
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step:3d}: "
                  f"Neurons={stats['firing_neuron_counts'][-1]:4d}, "
                  f"Spikes={stats['total_spike_counts'][-1]:6d}, "
                  f"Rate={stats['firing_rates'][-1]:5.2f}, "
                  f"Dev={stats['development_stages'][-1]}")
    
    # Analysis
    print("\n=== Analysis ===")
    print(f"Simulation completed: {len(stats['steps'])} steps")
    print(f"Final firing rate: {stats['firing_rates'][-1]:.2f}")
    print(f"Peak firing rate: {max(stats['firing_rates']):.2f}")
    print(f"Average firing rate: {sum(stats['firing_rates'])/len(stats['firing_rates']):.2f}")
    print(f"Final development stage: {stats['development_stages'][-1]}")
    
    print("\n=== Advanced Monitoring Complete ===\n")

if __name__ == "__main__":
    main()
    
    # Uncomment to run advanced version
    # advanced_monitoring_example()
