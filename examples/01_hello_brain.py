"""Example 01: Hello Brain - Simple Brain Simulation

This example demonstrates the most basic usage of NLM: creating a brain and
running a simple simulation.
"""

import pynlm

print("=== Example 1: Hello Brain ===")
print()

# Create a brain with default configuration
print("Creating brain with default configuration...")
config = pynlm.createDefaultConfig()
config.neuron_count = 100  # Smaller for faster demo
brain = pynlm.createBrain(config)

# Initialize the brain
print("Initializing brain...")
brain.initialize()

# Run a simple simulation
print("Running simulation...")
for step in range(50):
    brain.step(step)
    
# Display results
print()
print("=== Simulation Results ===")
print(f"Total steps: {brain.get_step_count() if hasattr(brain, 'get_step_count') else 'N/A'}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")

print()
print("✓ Example completed successfully!")

# Additional exploration
print("\n=== Additional Brain Statistics ===")
brain.log_status()
