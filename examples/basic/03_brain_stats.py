#!/usr/bin/env python3
"""
Example 3: Brain Statistics

Demonstrates how to collect and use brain metrics during simulation.
Shows real-time monitoring and data collection.
"""

import pynlm

def main():
    print("NLM Example 3: Brain Statistics")
    print("=" * 40)
    
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    print("\nRunning simulation with statistics collection...")
    print("-" * 50)
    
    # Run simulation and collect statistics
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 20 == 0:
            print(f"Step {step:3d}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount():4d}")
            print(f"  Total spikes:   {brain.getTotalSpikeCount():6d}")
            print(f"  Avg firing rate: {brain.getAverageFiringRate():6.2f} Hz")
            print(f"  E/I ratio:      {brain.getExcitationInhibitionRatio():6.2f}")
            print()
    
    # Final statistics
    print("=" * 50)
    print("Final Statistics:")
    print(f"  Total steps: {100}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  Neural activity: {'High' if brain.getAverageFiringRate() > 1.0 else 'Low'}")
    
    print(f"\nExample completed successfully!")

if __name__ == "__main__":
    main()