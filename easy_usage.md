## Advanced Usage with Neuromodulation

### Example 3: Learning Agent with Full Neuromodulation

```python
import pynlm

# Create configuration with enhanced settings
config = pynlm.createDefaultConfig()
config.set("neuron_count", 3000)  # Larger brain
config.set("region_count", 3)     # More regions
config.set("connection_probability", 0.15)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable comprehensive neuromodulation
agent.enableRewardModulation(True)      # Dopamine-based learning
agent.enableCuriosity(True)             # Exploration motivation
agent.enableDevelopment(True)           # Brain maturation

# Enable advanced neuromodulators
agent.enableAcetylcholine(True)          # Attention and memory
agent.enableNorepinephrine(True)        # Arousal and vigilance
agent.enableSerotonin(True)             # Mood and social behavior

# Run sophisticated learning simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and learn
    reward = world.computeReward(agent)
    agent.applyRewardModulation(reward, 0.0f)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Signal novelty for curiosity
    if step % 50 == 0:
        novelty_level = agent.getNoveltyLevel()
        if novelty_level > 0.3:
            agent.signalNovelty(novelty_level)
    
    if step % 100 == 0:
        stats = agent.getBrainStats()
        print(f"Step {step}: Neurons={stats['total_neurons']}, "
              f"Spikes={stats['total_spikes']}, "
              f"Dopamine={stats['dopamine']:.2f}, "
              f"Curiosity={stats['curiosity']:.2f}, "
              f"ACh={stats['acetylcholine']:.2f}")

print("Advanced learning simulation complete!")
print("Final brain stats:", agent.getBrainStats())
```

### Example 4: Curiosity-Driven Exploration

```python
import pynlm

# Create world with resources to explore
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Add some resources
for i in range(5):
    world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 10.0))
    world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 10.0))

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable curiosity-driven exploration
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.signalSocialInteraction()  # Boost serotonin for social learning

print("Running curiosity-driven exploration...")
print("Agent will seek out novel resources and learn from exploration")

for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.computeReward(agent)
    agent.applyRewardModulation(reward, 0.0f)
    
    # Check if agent is being curious
    curiosity = agent.getCuriosityLevel()
    if curiosity > 0.4:
        # High curiosity means exploration
        print(f"Step {step}: Agent is exploring (curiosity={curiosity:.2f})")
        agent.signalExplorationDrive()
    
    if step % 50 == 0:
        print(f"Step {step}: Reward={reward:.2f}, Curiosity={curiosity:.2f}")

print("Curiosity exploration complete!")
```

### Example 5: Attention and Focus

```python
import pynlm

# Create brain optimized for attention tasks
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("stdp_ltp_weight", 0.025)  # Strong learning

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable attention-focused neuromodulation
agent.enableAcetylcholine(True)  # Enhanced attention
agent.enableRewardModulation(True)

# Set up a challenging task: find and collect resources
print("Running attention-focused task...")
print("Agent must focus on resource location and avoid hazards")

collected_resources = 0
for step in range(400):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.computeReward(agent)
    agent.applyRewardModulation(reward, 0.0f)
    
    if reward > 5.0:
        collected_resources += 1
        print(f"Step {step}: Resource collected! Total: {collected_resources}")
        agent.signalAttentionBoost()  # Focus better after success
    
    if step % 100 == 0:
        stats = agent.getBrainStats()
        attention_boost = agent.getAcetylcholineLevel()
        print(f"Step {step}: Resources={collected_resources}, "
              f"Attention={attention_boost:.2f}, "
              f"Learning rate={stats['dopamine']:.2f}")

print(f"Attention task complete! Resources collected: {collected_resources}")
```

### Example 6: Development Simulation

```python
import pynlm

# Create brain for development study
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=12, height=12, visionWidth=5, visionHeight=5)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable comprehensive development with all neuromodulators
agent.enableDevelopment(True)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableAcetylcholine(True)
agent.enableNorepinephrine(True)
agent.enableSerotonin(True)

print("Running developmental simulation...")
print("Brain will mature over time with changing neuromodulation")

for step in range(600):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.computeReward(agent)
    agent.applyRewardModulation(reward, 0.0f)
    
    agent.updateDevelopment(0.1)
    
    # Record developmental metrics
    if step % 100 == 0:
        stats = agent.getBrainStats()
        stage = agent.getDevelopmentalStage()
        print(f"Step {step}: Development Stage={stage}, "
              f"Plasticity={stats['dopamine']:.2f}, "
              f"Curiosity={stats['curiosity']:.2f}, "
              f"ACh={stats['acetylcholine']:.2f}, "
              f"NE={stats['norepinephrine']:.2f}, "
              f"5-HT={stats['serotonin']:.2f}")

print("Developmental simulation complete!")
```

### Example 7: Checkpoint and Persistence

```python
import pynlm
import os

# Create and run a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)

# Run for a while
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

# Save brain state
save_path = "./nlm_brain_checkpoint.nlm"
print(f"Saving brain state to {save_path}")
brain.save(save_path)

print("Brain state saved successfully!")
print("You can reload this brain later to continue learning.")

# Later, reload the brain
print("\nReloading brain state...")
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load(save_path)

# Continue training
for step in range(201, 400):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    loaded_brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print("Brain reloaded and continued learning!")

# Clean up
if os.path.exists(save_path):
    os.remove(save_path)
    print("Checkpoint cleaned up.")
```

---

## Comprehensive API Reference

### Core Classes

#### Brain
- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize brain
- `step(current_step)` - Perform simulation step
- `step(current_step, current_time)` - Step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

#### AgentBrain
- `__init__(brain)` - Create agent with brain
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected input size
- `getMotorOutputSize()` - Get expected output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted)` - Apply reward
- `updateDevelopment(timestep)` - Update development
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent

#### World and Environment
- `SimpleWorld()` - Create simple world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state

### Neuromodulation Access

#### Advanced AgentBrain Methods
- `getDopamineLevel()` - Get dopamine level
- `getAcetylcholineLevel()` - Get acetylcholine level
- `getNorepinephrineLevel()` - Get norepinephrine level
- `getSerotoninLevel()` - Get serotonin level
- `getPredictionErrorLevel()` - Get prediction error level
- `enableAcetylcholine(enable)` - Enable/disable ACh
- `enableNorepinephrine(enable)` - Enable/disable NE
- `enableSerotonin(enable)` - Enable/disable 5-HT
- `signalNovelty(novelty)` - Signal novelty detection
- `signalSocialInteraction()` - Signal social interaction
- `triggerBurstMode()` - Trigger NE burst mode
- `signalAttentionBoost()` - Signal attention boost

### Factory Functions

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with config
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain
- `createDefaultExperimentConfig()` - Create advanced experiment config
- `run_simple_simulation(brain, world, agent, steps, learning)` - Run simple simulation
- `run_development_simulation(...)` - Run development simulation
- `quick_start_example()` - Print quick start guide

---

## Configuration Options

### Common Settings
- `neuron_count` - Number of neurons (default: 1000)
- `region_count` - Number of brain regions (default: 1)
- `simulation_timestep` - Simulation timestep (default: 0.001)
- `connection_probability` - Random connection probability (default: 0.1)

### Neuromodulation Settings
- `stdp_ltp_weight` - STDP LTP weight (default: 0.01)
- `stdp_ltd_weight` - STDP LTD weight (default: 0.012)
- `stdp_tau` - STDP time constant (default: 20.0)
- `synaptogenesis_rate` - Development synaptogenesis rate (default: 0.0001)
- `pruning_rate` - Development pruning rate (default: 0.00001)
- `dopamine_baseline` - Dopamine baseline level (default: 0.1)

### Experiment Settings
- `replay_interval` - Memory replay interval (default: 100)
- `consolidation_interval` - Memory consolidation interval (default: 1000)
- `visualization_enabled` - Enable visualization (default: false)
- `checkpoint_dir` - Checkpoint directory (default: "./checkpoints")

---

## Quick Reference

### Simple Brain
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

### Learning Agent
```python
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### Full Neuromodulation
```python
agent.enableAcetylcholine(True)      # Attention
agent.enableNorepinephrine(True)    # Arousal
agent.enableSerotonin(True)         # Mood/social
agent.signalNovelty(0.5)           # Exploration
```

### Get Statistics
```python
stats = agent.getBrainStats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Learning rate: {stats['dopamine']}")
print(f"Curiosity: {stats['curiosity']}")
```

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more detailed usage instructions
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try advanced experiments using `createDefaultExperimentConfig()`
5. Use `run_development_simulation()` for comprehensive testing
6. Implement your own custom experiments

That's it! You're now ready to use NLM with full neuromodulation support.

For help, visit the NLM documentation or consult the source code examples.