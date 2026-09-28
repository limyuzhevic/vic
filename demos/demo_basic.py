import pynlm
import time
import json

"""
Demo 1: Basic Brain Operations

This demo showcases the fundamental brain operations using the NLM Python API.
"""

def demo_basic():
    print("=== NLM Basic Operations Demo ===")
    
    # Create brain
    print("\n1. Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   and {brain.getTotalSynapseCount()} synapses")
    
    # Initialize brain
    print("\n2. Initializing brain...")
    brain.initialize()
    print("   Brain initialized successfully")
    
    # Run simulation
    print("\n3. Running simulation (100 steps)...")
    start_time = time.time()
    
    for i in range(100):
        brain.step(i)
    
    end_time = time.time()
    simulation_time = end_time - start_time
    
    print(f"   Simulation completed in {simulation_time:.3f} seconds")
    print(f"   Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"   Currently firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"   Development stage: {brain.getDevelopmentalStage()}")
    
    # Save brain state
    print("\n4. Saving brain state...")
    brain.save("demo_basic_brain.bin")
    print(f"   Brain saved to 'demo_basic_brain.bin'")
    
    # Load brain state
    print("\n5. Loading brain state...")
    brain.load("demo_basic_brain.bin")
    print("   Brain loaded successfully")
    
    print("\n=== Demo Complete ===")
    return brain

if __name__ == "__main__":
    demo_basic()
