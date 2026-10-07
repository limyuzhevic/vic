"""NLM Hello World - The absolute simplest example"""
import pynlm

# Create and initialize a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
brain.step(0)

print(f"Your brain has {brain.neuron_count} neurons!")
print(f"It has fired {brain.total_spike_count} times.")
