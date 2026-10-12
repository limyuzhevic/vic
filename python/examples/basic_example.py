from pynlm import *

# Example: Basic Brain Simulation
print("NLM Python Bindings Example: Basic Brain Simulation")
print("=" * 60)

# Create a default configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Create and configure a simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

print("Created world with size 20x20")

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Created agent and initialized with world")

# Run a simulation cycle
time_step = 0
for i in range(5):
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    if percept:
        agent.processSensoryInput(percept)
    
    # Step brain
    brain.step(time_step)
    
    # Get action
    action = agent.decodeMotorCommand()
    if action:
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Time step {i}: {brain.getFiringNeuronCount()} firing neurons")
    time_step += 1

print("\nExample completed successfully!")
print("NLM Python bindings are working correctly.")