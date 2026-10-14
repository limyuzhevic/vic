"""
NLM (Neural Learning Machine) - Basic Usage Example

This example demonstrates the basic usage of NLM for creating and running a simple neural simulation.

Author: NLM Research Team
Version: 6.0.0
"""

import pynlm
import time

def main():
    print("NLM Basic Usage Example")
    print("=" * 50)
    
    # 1. Create configuration
    print("\n1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    print(f"   Configuration created")
    
    # 2. Create brain
    print("\n2. Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"   Regions: {brain.getRegionCount()}")
    
    # 3. Run simulation
    print("\n3. Running simulation...")
    num_steps = 100
    for step in range(num_steps):
        brain.step(step)
        
        # Print statistics every 25 steps
        if step % 25 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # 4. Display results
    print("\n4. Simulation Results:")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"   E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\n✅ Basic example completed successfully!")

if __name__ == "__main__":
    main()
