"""Quick start examples for NLM Python API"""

import pynlm

print("=== NLM Quick Start Examples ===")

# Example 1: Simplest Brain
print("\n1. Simplest Brain:")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"   Total spikes: {brain.getTotalSpikeCount()}")

# Example 2: Brain with World Interaction
print("\n2. Brain + World Interaction:")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a few steps
for step in range(5):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"   Simulation completed with {brain.getTotalSpikeCount()} spikes")
print(f"   Agent curiosity level: {agent.getCuriosityLevel():.3f}")

# Example 3: Configuration
print("\n3. Configuration Management:")
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)

brain2 = pynlm.createBrain(config)
brain2.initialize()
print(f"   Custom brain with {brain2.getTotalNeuronCount()} neurons")

# Example 4: Checkpoint system
print("\n4. Checkpoint System:")
checkpoint_file = "/tmp/nlm_quick_test.bin"
brain.save(checkpoint_file)
print(f"   Saved brain to {checkpoint_file}")

brain3 = pynlm.createBrain(config)
brain3.initialize()
brain3.load(checkpoint_file)
print(f"   Loaded brain from checkpoint")

print("\n=== All quick start examples completed successfully! ===")
