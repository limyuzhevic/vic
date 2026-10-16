# Example 1: Simple Brain Demo

import pynlm

print("=== Simple Brain Demo ===")

# Create a brain
brain = pynlm.createBrain()
brain.initialize()

# Run a simple simulation
print("Running simulation...")
for step in range(10):
    brain.step(step)
    if step % 5 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Show results
print(f"\nResults:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("\n=== Demo Complete ===")

# Save and load example
brain.save("simple_brain.bin")
print(f"Brain saved to 'simple_brain.bin'")

# Load the brain
new_brain = pynlm.createBrain()
new_brain.load("simple_brain.bin")
print(f"Loaded brain has {new_brain.getTotalNeuronCount()} neurons")

print("\n=== Load/Store Demo Complete ===")
