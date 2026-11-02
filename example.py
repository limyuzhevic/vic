import pynlm

# Create and initialize a brain with standard configuration
brain = pynlm.createDefaultBrain()
brain.initialize()

# Create a simple world
world = pynlm.createSimpleDemoWorld()
world.reset()

# Create an agent
agent = pynlm.createDefaultAgent()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run an episode
for step in range(100):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process it
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Episode complete! Final stats: {agent.getSystemStatus()}")