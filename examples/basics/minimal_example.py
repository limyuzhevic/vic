"""
Minimal working example for quick testing.
"""
import pynlm

# Create and run a minimal simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"✓ Minimal simulation complete. Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
