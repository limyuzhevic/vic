/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d36b4516-9598-423b-a111-045888d2ef5f/examples/basic_usage.py
Name: Basic usage example
Summary: Basic example of NLM Python interface usage

"""
Basic usage example for NLM Python interface.

This example demonstrates the fundamental operations of the NLM brain system.
"""

import pynlm
import time

# Initialize NLM system
print("Initializing NLM system...")
interface = pynlm.initialize_nlm()

# Create and configure brain
print("Creating brain...")
brain = interface.create_brain()
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Brain contains {brain.getTotalSynapseCount()} synapses")

# Create world
print("Creating world...")
world = interface.create_world(width=100, height=100, vision_width=32, vision_height=32)

# Run simulation
print("Running simulation...")
time_steps = 100
for i in range(time_steps):
    result = interface.run_step(dt=0.1)
    print(f"Step {i+1}/{time_steps}: {result['status']}")

# Inject stimuli
print("Injecting stimuli...")
interface.inject_stimulus("current", {"neuron_id": 0, "current": 1.0})
interface.inject_stimulus("type", {"neuron_type": pynlm.NeuronType.Excitatory, "current": 0.5})

# Save brain state
print("Saving brain state...")
interface.save_brain_state("brain_state_basic.json")

# Load brain state
print("Loading brain state...")
interface.load_brain_state("brain_state_basic.json")

# Display final state
print("\nFinal brain state:")
state = interface.get_state()
for key, value in state.items():
    print(f"  {key}: {value}")

# Reset system
print("\nResetting system...")
interface.reset_system()

print("Basic usage example completed successfully!")
