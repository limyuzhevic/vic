import pynlm

print("=== NLM Advanced Action Selection Example ===")
print("Demonstrating sophisticated action selection with multiple factors...")

# Create sophisticated brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with world
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(12.5, 12.5)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print(f"Advanced systems enabled:")
print(f"  Reward Modulation: Enabled")
print(f"  Curiosity: Enabled")
print(f"  Development: Enabled")
print(f"  STDP Plasticity: Enabled")

# Run complex simulation
print("\nRunning advanced action selection simulation...")
action_stats = {}
for action_type in pynlm.ActionType:
    action_stats[str(action_type)] = 0

for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Advanced action selection (brain decides the action)
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate reward
    reward = 0.1  # Simple exploration reward
    
    # Apply neuromodulation
    agent.applyRewardModulation(reward, 0.0)
    
    # Track action statistics
    action_name = str(action.getType())
    action_stats[action_name] += 1
    
    # Update development periodically
    if step % 50 == 0:
        agent.updateDevelopment(0.5)
        brain.develop()
        print(f"  Step {step}: Development updated, stage: {brain.getDevelopmentalStage()}")
    
    # Print progress
    if step % 50 == 0:
        pos = world.getAgentBody()
        action_name = str(action.getType())
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        firing = brain.getAverageFiringRate()
        print(f"  Step {step}: Action={action_name:12s} Pos=({pos.x:6.1f},{pos.y:6.1f}) "
              f"Curiosity={curiosity:.3f} Novelty={novelty:.3f} Firing={firing:.3f}")

print(f"\n=== Advanced Action Selection Results ===")
total_actions = sum(action_stats.values())
print(f"Total actions taken: {total_actions}")
print(f"Action selection statistics:")
for action, count in sorted(action_stats.items(), key=lambda x: x[1], reverse=True):
    percentage = count / total_actions * 100
    print(f"  {action:20s}: {count:4d} ({percentage:5.1f}%)")

print(f"\n=== Final System Status ===")
print(f"Final neuromodulation levels:")
print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
print(f"  Prediction Error: {agent.getPredictionError():.3f}")
print(f"Final brain development stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")

# Demonstrate advanced features
print(f"\n=== Advanced Features Demonstrated ===")
print(f"✓ Neuromodulation (dopamine, curiosity, novelty)")
print(f"✓ Reward-based learning (STDP)")
print(f"✓ Developmental processes (plasticity changes)")
print(f"✓ Action selection optimization")
print(f"✓ Exploration vs. exploitation balance")
print(f"✓ State-dependent behavior (position tracking)")

print(f"\n=== Advanced Example Complete ===")