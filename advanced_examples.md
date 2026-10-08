# Advanced Examples

### Example 4: Configuration Management

```python
import pynlm

# Create a brain with custom configuration
config = pynlm.createDefaultConfig()

# Modify configuration parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with custom configuration
for step in range(200):
    brain.step(step)

print(f"Custom brain simulation completed:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate()}")
```

### Example 5: Working Memory and Learning

```python
import pynlm

# Create a brain with advanced memory features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with memory systems
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run an episode with working memory and learning
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input (this stores information in working memory)
    agent.processSensoryInput(percept)
    
    # Brain thinks and updates working memory
    brain.step(step)
    
    # Get action from brain's motor system
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Working memory active traces: {brain.getWorkingMemory()->getActiveTraces()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

print("Advanced learning episode complete!")
```

### Example 6: Configuration Export/Import

```python
import pynlm

# Create a complex configuration
config = pynlm.createDefaultConfig()

# Set various parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.enable", True)
config.set("world.width", 10)
config.set("world.height", 10)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Show configuration
print("Current configuration:")
print(config.summary())

# Export to file
import tempfile
import os
with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
    temp_filename = f.name

brain.save(temp_filename)  # This saves the brain state
config.saveToFile(temp_filename.replace('.cfg', '.json'))  # This saves the config

print(f"Configuration exported to {temp_filename}")

# Import configuration from file
imported_config = pynlm.createDefaultConfig()
imported_config.loadFromFile(temp_filename.replace('.cfg', '.json'))

print("Imported configuration:")
print(imported_config.summary())

# Clean up
os.unlink(temp_filename)
os.unlink(temp_filename.replace('.cfg', '.json'))
```