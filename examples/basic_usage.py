#!/usr/bin/env python3
"""Basic brain creation and usage example.

This script demonstrates the fundamental building blocks of NLM:
creating a brain, initializing it, and running basic simulations.
"""

import pynlm
import sys

def main():
    print("=== NLM Basic Usage Example ===\n")
    
    # 1. Create and initialize a brain
    print("Step 1: Creating and initializing brain")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ {brain.getTotalSynapseCount()} synapses initially")
    print()
    
    # 2. Run a simple simulation
    print("Step 2: Running simple simulation")
    initial_spikes = brain.getTotalSpikeCount()
    
    for i in range(50):
        brain.step(i)
        
        if i % 10 == 0:
            firing = brain.getFiringNeuronCount()
            print(f"  Step {i}: {firing} neurons firing")
    
    final_spikes = brain.getTotalSpikeCount()
    print(f"✓ Simulation complete. Total spikes: {final_spikes - initial_spikes}")
    print()
    
    # 3. Show brain statistics
    print("Step 3: Brain statistics")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Currently firing: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print()
    
    # 4. Demonstrate plasticity (STDP)
    print("Step 4: Testing STDP plasticity")
    stdp = brain.getSTDP()
    
    print(f"  Initial LTP weight: {stdp.getLTPWeight():.3f}")
    print(f"  Initial LTD weight: {stdp.getLTDWeight():.3f}")
    print(f"  Initial time constant: {stdp.getTimeConstant():.1f} ms")
    
    # Configure STDP
    stdp.configure(0.05, 0.06, 25.0)
    print(f"  After configuration:")
    print(f"    LTP weight: {stdp.getLTPWeight():.3f}")
    print(f"    LTD weight: {stdp.getLTDWeight():.3f}")
    print(f"    Time constant: {stdp.getTimeConstant():.1f} ms")
    print()
    
    # 5. Test save/load functionality
    print("Step 5: Testing state persistence")
    save_file = "brain_checkpoint.bin"
    
    try:
        brain.save(save_file)
        print(f"✓ Saved brain state to {save_file}")
        
        # Create new brain and load
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.load(save_file)
        print(f"✓ Loaded brain state from {save_file}")
        print(f"  Loaded brain has {brain2.getTotalNeuronCount()} neurons")
        
    except Exception as e:
        print(f"✗ State persistence test failed: {e}")
    
    print()
    print("=== Basic Usage Example Complete ===")

if __name__ == "__main__":
    main()
