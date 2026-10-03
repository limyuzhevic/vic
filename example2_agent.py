import pynlm

print("=== NLM Agent System Example ===")
print("Creating a complete agent with sensory input and motor output...")

# Setup brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Create world and setup
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)

agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print(f"Agent initialized at position ({world.getAgentBody().x}, {world.getAgentBody().y})")
print(f"World size: {world.getWidth()}x{world.getHeight()}")

# Run simulation
print("\nRunning agent simulation...")
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = 0.1  # Simple reward for trying
    agent.applyRewardModulation(reward, 0.0)
    
    # Print progress
    if step % 20 == 0:
        pos = world.getAgentBody()
        action_name = str(action.getType())
        curiosity = agent.getCuriosityLevel()
        firing = brain.getAverageFiringRate()
        print(f"Step {step:3d}: Action={action_name:12s} Pos=({pos.x:5.1f},{pos.y:5.1f}) "
              f"Curiosity={curiosity:.3f} Firing={firing:.3f}")

# Print final statistics
print("\n=== Agent Simulation Complete ===")
print(f"Final position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Final prediction error: {agent.getPredictionError():.3f}")
print(f"Final development stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

print("\n=== Agent Example Complete ===")