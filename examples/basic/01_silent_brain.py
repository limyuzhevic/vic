#!/usr/bin/env python3
"""
Example 1: Silent Brain

The simplest possible NLM example - creates a brain that thinks without any input.
This demonstrates the basic brain creation and stepping process.
"""

import pynlm

def main():
    print("NLM Example 1: Silent Brain")
    print("=" * 40)
    
    # Step 1: Create a default brain
    print("Creating brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Step 2: Initialize the brain (required before use)
    print("Initializing brain...")
    brain.initialize()
    
    # Step 3: Make the brain think for several steps
    print("Making brain think (silent operation)...")
    for i in range(10):
        brain.step(i)
    
    # Step 4: Report results
    total_spikes = brain.getTotalSpikeCount()
    print(f"\nResults:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    print(f"\nExample completed successfully!")
    print("Note: This brain operated silently without any external input.")

if __name__ == "__main__":
    main()