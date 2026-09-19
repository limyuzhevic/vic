import pynlm

# Example 1: Simple Brain Creation
print("=== Example 1: Simple Brain Creation ===")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.4f}")
for i in range(10):
    brain.step(i)
print(f"After 10 steps, firing rate: {brain.getAverageFiringRate():.4f}")
print()

# Example 2: Brain with Memory Systems
print("=== Example 2: Advanced Brain with Memory ===")
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
brain2 = pynlm.createBrain(config)
brain2.initialize()
agent2 = pynlm.createAgentBrain(brain2)

# Enable all learning systems
agent2.enableRewardModulation(True)
agent2.enableStructuralPlasticity(True)
agent2.enableDevelopment(True)
agent2.enableCuriosity(True)

print(f"Created advanced brain with {brain2.getTotalNeuronCount()} neurons")
print(f"Brain regions: {brain2.getRegionCount()}")
print(f"Initial development stage: {brain2.getDevelopmentalStage().name}")
print()

# Example 3: Agent in World Simulation
print("=== Example 3: Complete Agent Simulation ===")
brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
brain3.initialize()
agent3 = pynlm.createAgentBrain(brain3)
world3 = pynlm.createSimpleWorld()
world3.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world3.setAgentStart(10.0, 10.0)
agent3.initialize(world3)

# Run simulation
for step in range(100):
    world3.update(0.1)
    percept = world3.getSensoryPercept()
    agent3.processSensoryInput(percept)
    brain3.step(step)
    action = agent3.decodeMotorCommand()
    world3.applyMotorCommand(action, world3.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain3.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent3.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent3.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent3.getPredictionError():.3f}")
        print(f"  Development stage: {brain3.getDevelopmentalStage().name}")
        print(f"  Action: {action.name}")

print()

# Example 4: Configuration Management
print("=== Example 4: Configuration Management ===")
config4 = pynlm.createDefaultConfig()
print(f"Default config keys: {len(config4.getKeys())}")
config4.set("brain.neuron_count", 5000)
config4.set("brain.synapse_density", 0.2)
config4.set("plasticity.stdp.learning_rate", 0.005)

# Check values
print(f"Neuron count: {config4.getOr('brain.neuron_count', 1000)}")
print(f"Synapse density: {config4.getOr('brain.synapse_density', 0.1)}")
print(f"STDP learning rate: {config4.getOr('plasticity.stdp.learning_rate', 0.001)}")

# Save configuration
config4.saveToFile("test_config.json")
print("Configuration saved to test_config.json")

# Load configuration
config4.loadFromFile("test_config.json")
print(f"Loaded neuron count: {config4.getOr('brain.neuron_count', 0)}")
print()

# Example 5: Learning Experiment
print("=== Example 5: Learning Experiment ===")
config5 = pynlm.createDefaultConfig()
brain5 = pynlm.createBrain(config5)
brain5.initialize()
agent5 = pynlm.createAgentBrain(brain5)
world5 = pynlm.createSimpleWorld()
world5.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world5.setAgentStart(7.5, 7.5)
agent5.initialize(world5)
agent5.enableRewardModulation(True)
agent5.enableCuriosity(True)

# Run learning episode
for step in range(200):
    world5.update(0.1)
    percept = world5.getSensoryPercept()
    agent5.processSensoryInput(percept)
    brain5.step(step)
    action = agent5.decodeMotorCommand()
    world5.applyMotorCommand(action, world5.getSimulationTime())
    
    # Simulate reward based on action success
    reward = 0.1
    if action.name in ["MoveForward", "TurnLeft", "TurnRight"]:
        reward = 0.5
    elif action.name == "Interact":
        reward = 1.0
    
    agent5.applyRewardModulation(reward, 0.0)

print(f"Learning episode completed")
print(f"Final curiosity level: {agent5.getCuriosityLevel():.3f}")
print(f"Final prediction error: {agent5.getPredictionError():.3f}")
print(f"Total spikes: {brain5.getTotalSpikeCount()}")
print()

# Example 6: Brain State Management
print("=== Example 6: Brain State Management ===")
config6 = pynlm.createDefaultConfig()
brain6 = pynlm.createBrain(config6)
brain6.initialize()

# Run some steps
for i in range(50):
    brain6.step(i)

print(f"Brain state before save: {brain6.getTotalSpikeCount()} spikes")

# Save and load brain state
brain6.save("test_brain_state.bin")
print("Brain state saved to test_brain_state.bin")

# Reset and reload
brain6.reset()
print(f"Brain state after reset: {brain6.getTotalSpikeCount()} spikes")

brain6.load("test_brain_state.bin")
print(f"Brain state after load: {brain6.getTotalSpikeCount()} spikes")
print()

# Example 7: Advanced Convenience Methods
print("=== Example 7: Advanced Usage Patterns ===")

# Create a brain with optimized settings
def create_optimized_brain(neurons=3000, plasticity=True):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    if plasticity:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
    return pynlm.createBrain(config)

# Use convenience function
optimized_brain = create_optimized_brain(neurons=1000, plasticity=True)
optimized_brain.initialize()
optimized_brain.logStatus()

# Batch simulation function
def run_simulation(brain, steps, world=None, agent=None):
    """Run a complete simulation with optional world and agent."""
    print(f"Running {steps} simulation steps...")
    for step in range(steps):
        brain.step(step)
        if world and agent:
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate()
    }

# Run batch simulation
results = run_simulation(optimized_brain, 100)
print(f"Simulation results: {results}")
print()

print("=== All Examples Completed Successfully ===")