Better Initialize

# Example for the user:
config = nlm.createDefaultConfig()
config.set("neuron_count", 2000)
brain = nlm.createBrain(config)

# Using method chaining - clean and Pythonic!
world = nlm.createSimpleWorld()
world.configure(width=20, height=20)

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems cleanly
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: Firing {brain.getFiringNeuronCount()} neurons")

print("\nBrain Stats:")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development: {brain.getDevelopmentalStage()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Save brain state
brain.save("my_brain.bin")
print(f"Brain saved to 'my_brain.bin'")

# Check curiosity
print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"Novelty level: {agent.getNoveltyLevel():.2f}")
