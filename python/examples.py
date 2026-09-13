import pynlm
import sys

print("NLM Python Examples")
print("==================")
print()

# Example 1: Simplest Brain
print("Example 1: Simplest Brain")
print("-" * 30)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print()

# Example 2: Brain in a Simple World
print("Example 2: Brain in a Simple World")
print("-" * 35)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print()

# Example 3: Complete Agent with Learning
print("Example 3: Complete Agent with Learning")
print("-" * 40)

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward from world (using internal signals as a simple proxy)
    percept = world.getSensoryPercept()
    if percept.getInternal():
        reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)

print("Complete agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print()

# Example 4: Configuration Management
print("Example 4: Configuration Management")
print("-" * 35)
config = pynlm.createDefaultConfig()

# Set some configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.enable", False)
config.set("neuromod.dopamine.scale", 1.5)

print(f"Configuration summary: {config.summary()}")
print(f"Has brain.neuron_count: {config.has('brain.neuron_count')}")
print(f"Neuron count: {config.getOr('brain.neuron_count', 1000)}")
print()

# Example 5: Agent State Management
print("Example 5: Agent State Management")
print("-" * 30)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

print(f"Sensory input size: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")
print(f"Initial developmental stage: {agent.getDevelopmentalStage()}")
print(f"Initial neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print()

# Example 6: World Interaction
print("Example 6: World Interaction")
print("-" * 25)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Add some objects to the world
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=20.0))

print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Agent at position ({world.getAgentBody().x}, {world.getAgentBody().y})")
print(f"Position valid: {world.isValidPosition(10.0, 10.0)}")
print()

# Example 7: Simulation Statistics
print("Example 7: Simulation Statistics")
print("-" * 30)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

stats = {
    "Total Neurons": brain.getTotalNeuronCount(),
    "Total Synapses": brain.getTotalSynapseCount(),
    "Active Neurons": brain.getActiveNeuronCount(),
    "Firing Neurons": brain.getFiringNeuronCount(),
    "Total Spikes": brain.getTotalSpikeCount(),
    "Average Firing Rate": brain.getAverageFiringRate(),
    "Excitatory/Inhibition Ratio": brain.getExcitationInhibitionRatio(),
    "Developmental Stage": brain.getDevelopmentalStage()
}

for key, value in stats.items():
    print(f"{key}: {value}")
print()

print("All examples completed successfully!")
