import pynlm

print("=== NLM Neuromodulation and Learning Example ===")
print("Demonstrating dopamine, curiosity, and novelty systems...")

# Create brain with neuromodulation systems
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 600)
config.set("plasticity.stdp.learning_rate", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
world = pynlm.createSimpleWorld()
world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable neuromodulation systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print(f"Neuromodulation systems enabled:")
print(f"  Reward Modulation: Enabled")
print(f"  Curiosity: Enabled")
print(f"  Novelty: {'Enabled' if agent.getNoveltyLevel() > 0 else 'Disabled'}")
print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")

# Run simulation with various experiences
print("\nRunning neuromodulation simulation...")
for episode in range(3):
    print(f"\n--- Episode {episode + 1} ---")
    
    for step in range(80):
        # Choose action based on curiosity level
        if agent.getCuriosityLevel() > 0.7:
            # High curiosity - prefer exploration
            from pynlm import Action
            action = Action(pynlm.ActionType.Look)
            print(f"  Step {step}: Exploration action (Curiosity: {agent.getCuriosityLevel():.3f})")
        else:
            # Normal - random action
            from pynlm import Action
            action = Action(pynlm.ActionType.MoveForward)
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Decode action from brain
        brain_action = agent.decodeMotorCommand()
        
        # Calculate reward based on action
        reward = 0.0
        if brain_action.getType() == pynlm.ActionType.Eat:
            reward = 1.0
        elif brain_action.getType() == pynlm.ActionType.Interact:
            reward = 0.5
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print neuromodulation levels
        if step % 20 == 0:
            print(f"    Neuromod: Dopamine={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")

# Print final neuromodulation statistics
print(f"\n=== Neuromodulation Demo Complete ===")
print(f"Final neuromodulation levels:")
print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
print(f"  Prediction Error: {agent.getPredictionError():.3f}")
print(f"Final brain development stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Demonstrate learning effects
print(f"\n=== Learning Effects Demo ===")
print(f"Learning plasticity enabled: {config.has('plasticity.stdp.enable')}")
if config.has('plasticity.stdp.learning_rate'):
    learning_rate = config.get('plasticity.stdp.learning_rate')
    print(f"STDP learning rate: {learning_rate:.4f}")

# Show example of neuromodulation effects
print(f"\nNeuromodulation effects demonstration:")
print(f"  High dopamine -> increased neural excitability")
print(f"  High curiosity -> increased exploration behavior")
print(f"  High novelty -> preference for novel stimuli")
print(f"  Reward prediction error -> learning modulation")

print("\n=== Neuromodulation Example Complete ===")