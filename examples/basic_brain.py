#!/usr/bin/env python3
"""
Example script 1: Basic Brain Simulation

This is the simplest possible example that demonstrates how to create and use
a basic neural brain simulation with NLM.
"""

import sys
import time

def main():
    """Run basic brain simulation example"""
    try:
        import pynlm
        
        print("=" * 60)
        print("NLM Example 1: Basic Brain Simulation")
        print("=" * 60)
        
        # Step 1: Create a default configuration
        print("\n1. Creating default configuration...")
        config = pynlm.createDefaultConfig()
        
        # Step 2: Create the brain
        print("2. Creating neural brain...")
        brain = pynlm.createBrain(config)
        
        # Step 3: Initialize the brain
        print("3. Initializing brain...")
        if not brain.initialize():
            print("ERROR: Brain initialization failed!")
            return 1
        
        # Step 4: Run a simple simulation
        print("4. Running basic simulation (50 steps)...")
        for step in range(50):
            brain.step(step)
            
            # Log progress every 10 steps
            if step % 10 == 0:
                firing = brain.getFiringNeuronCount()
                total_spikes = brain.getTotalSpikeCount()
                avg_rate = brain.getAverageFiringRate()
                print(f"   Step {step:2d}: {firing:4d} firing neurons, {total_spikes:6d} total spikes, {avg_rate:6.2f} Hz avg rate")
        
        # Step 5: Display final statistics
        print("\n5. Final statistics:")
        print(f"   Total neurons: {brain.getTotalNeuronCount()}")
        print(f"   Total synapses: {brain.getTotalSynapseCount()}")
        print(f"   Active neurons: {brain.getActiveNeuronCount()}")
        print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"   Total spikes: {brain.getTotalSpikeCount()}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"   Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
        
        # Step 6: Save brain state (optional)
        print("\n6. Saving brain state...")
        brain.save("basic_brain.bin")
        print("   Brain state saved to 'basic_brain.bin'")
        
        print("\n" + "=" * 60)
        print("Basic brain simulation completed successfully!")
        print("=" * 60)
        
        return 0
        
    except ImportError as e:
        print(f"ERROR: Failed to import pynlm: {e}")
        print("Please ensure NLM is properly installed with Python bindings.")
        return 1
    except Exception as e:
        print(f"ERROR: An unexpected error occurred: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())