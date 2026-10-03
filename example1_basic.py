import pynlm

# Basic NLM example - Hello World

print("=== NLM Basic Example ===")
print("Creating a simple brain and running a simulation...")

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Brain configuration: {brain.getConfig().summary()}")

# Run simulation for 100 steps
print("\nRunning simulation...")
for step in range(100):
    brain.step(step)
    
    # Print progress every 20 steps
    if step % 20 == 0:
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"Step {step:3d}: {firing:4d} neurons firing, rate: {rate:.4f}")

# Print final statistics
print("\n=== Simulation Complete ===")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Demonstrate memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

print(f"\n=== Memory Systems ===")
print(f"Working memory capacity: {working_memory.getCapacity()}")
print(f"Working memory traces: {working_memory.getActiveTraces()}")
print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")

print("\n=== Basic Example Complete ===")