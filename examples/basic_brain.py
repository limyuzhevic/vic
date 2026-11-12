import pynlm

# Basic brain creation and initialization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
