# Example 2: Agent in World Demo

import pynlm

print("=== Agent in World Demo ===")

# Step 1: Create brain and initialize
print("\n1. Creating brain and initializing...")
brain = pynlm.createBrain()
brain.initialize()

# Step 2: Create and configure world
print("\n2. Creating and configuring world...")
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
world.setAgentStart(10.0, 10.0)

print(f"  World size: {world.getWidth()} x {world.getHeight()}")
print(f"  Agent start: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")

# Step 3: Create agent
print("\n3. Creating agent...")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 4: Enable learning subsystems
print("\n4. Enabling learning subsystems...")
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("  Enabled: Reward Modulation, Curiosity, Development, Structural Plasticity")

# Step 5: Run agent episode
print("\n5. Running agent episode...")
for step in range(30):
    # Update world
    world.update(0.1)
    
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 10 == 0:
        print(f"  Step {step}: "
              f"Action: {action.name()}, "
              f"Reward: {reward:.3f}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}, "
              f"Dev Stage: {agent.getDevelopmentalStage()}")

# Step 6: Analyze results
print("\n6. Episode analysis...")
print(f"  Total reward received: {agent.getNeuromodulationLevel():.3f}")
print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
print(f"  Final development stage: {agent.getDevelopmentalStage()}")
print(f"  Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")

# Step 7: Save agent state
print("\n7. Saving agent state...")
brain.save("agent_brain.bin")
world.save("agent_world.bin")
agent.save("agent_state.bin")
print("  Saved brain, world, and agent states")

print("\n=== Agent Demo Complete ===")

# Step 8: Load and continue
print("\n8. Loading saved state...")
saved_brain = pynlm.createBrain()
saved_brain.load("agent_brain.bin")
saved_world = pynlm.createSimpleWorld()
saved_world.load("agent_world.bin")
saved_agent = pynlm.createAgentBrain(saved_brain)
saved_agent.load("agent_state.bin")

print(f"  Loaded brain: {saved_brain.getTotalNeuronCount()} neurons")
print(f"  Loaded agent: {saved_agent.getNeuromodulationLevel():.3f} neuromodulation level")

print("\n=== Load Demo Complete ===")
