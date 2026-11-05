# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
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
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

#### Advanced Usage

### Advanced Configuration

#### Creating Advanced Brain Configuration

```python
import pynlm

# Create advanced brain configuration
config = pynlm.createBrainConfig(
    neuronCount=2000,
    connectionProbability=0.15,
    enablePlasticity=True,
    enableNeuromodulation=True
)

# Initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Enable all advanced systems
agent.enableAllSystems()

# Run advanced simulation with monitoring
def monitor_simulation(brain, world, agent, step):
    firing_rate = brain.getAverageFiringRate()
    e_i_ratio = brain.getExcitationInhibitionRatio()
    print(f"Step {step}: FR={firing_rate:.2f}, E/I={e_i_ratio:.2f}")

pynlm.runAdvancedSimulation(brain, world, agent, 5000, monitor_simulation)
```

### Performance Monitoring and Benchmarking

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark brain performance
results = pynlm.benchmarkBrain(brain, 2000)
print(f"Benchmark Results:")
print(f"  Steps/second: {results['steps_per_second']:.1f}")
print(f"  Total spikes: {results['total_spikes']}")
print(f"  Average firing rate: {results['avg_firing_rate']:.3f}")
```

### Advanced World Configuration

```python
import pynlm

# Create advanced world with custom settings
world = pynlm.createAdvancedWorld(
    width=30,
    height=30,
    visionWidth=16,
    visionHeight=16,
    maxEnergy=200.0,
    energyDecayRate=0.005
)
world.reset()

# Set agent start position
world.setAgentStart(15.0, 15.0)
```

### Brain Statistics and Monitoring

```python
import pynlm

# Get comprehensive brain statistics
stats = pynlm.getBrainStats(brain)
print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Example output:
#   total_neurons: 1000
#   total_synapses: 50000
#   active_neurons: 120
#   firing_neurons: 85
#   avg_firing_rate: 0.085
#   total_spikes: 42000
#   e_i_ratio: 1.23
```

### Checkpoint Management

```python
import pynlm

# Save brain with metadata
brain.save("checkpoints/episode_1.bin", "Episode 1: Random exploration")

# Load brain with metadata
loaded_metadata = pynlm.loadBrainWithMetadata(brain, "checkpoints/episode_1.bin")
print(f"Loaded from: {loaded_metadata}")
```

### Complete Advanced Agent Example

```python
import pynlm

def runAdvancedAgentSimulation(config, world_config, steps=3000):
    """Run complete advanced agent simulation with monitoring."""
    
    # Create configuration
    config = pynlm.createAdvancedBrainConfig(
        neuronCount=config.get("neuron_count", 1000),
        connectionProbability=config.get("connection_probability", 0.1),
        enablePlasticity=True,
        enableNeuromodulation=True
    )
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createAdvancedWorld(**world_config)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all systems
    agent.enableAllSystems()
    
    # Run simulation with monitoring
    def monitor_callback(brain, world, agent, step):
        if step % 100 == 0:
            stats = pynlm.getBrainStats(brain)
            print(f"Step {step}: FR={stats['avg_firing_rate']:.3f}, "
                  f"E/I={stats['e_i_ratio']:.2f}, "
                  f"Spikes={stats['total_spikes']}")
    
    pynlm.runAdvancedSimulation(brain, world, agent, steps, monitor_callback)
    
    return brain, world, agent

# Example usage
if __name__ == "__main__":
    config = {
        "neuron_count": 2000,
        "connection_probability": 0.12
    }
    
    world_config = {
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8,
        "maxEnergy": 150.0,
        "energyDecayRate": 0.02
    }
    
    brain, world, agent = runAdvancedAgentSimulation(config, world_config, 2000)
    print("Advanced simulation complete!")
```

---

## Advanced Features Summary

| Feature | Python Function | Description |
|---------|----------------|-------------|
| Advanced Brain Config | `createAdvancedBrainConfig()` | Create brain with advanced settings |
| Advanced World | `createAdvancedWorld()` | Create world with custom parameters |
| All Systems Enable | `enableAllSystems()` | Enable reward, plasticity, development, curiosity |
| Advanced Simulation | `runAdvancedSimulation()` | Run simulation with monitoring callback |
| Benchmarking | `benchmarkBrain()` | Measure performance metrics |
| Brain Stats | `getBrainStats()` | Get comprehensive statistics |
| Checkpoint Save/Load | `saveBrainWithMetadata()` / `loadBrainWithMetadata()` | Save/load with metadata |
| World Monitoring | `monitorSimulation()` | Monitor simulation progress |

---

## Best Practices

### For Advanced Users

1. **Use Callbacks for Monitoring**: Always use callbacks in advanced simulations to track metrics without affecting performance.

2. **Benchmark Before Production**: Use `benchmarkBrain()` to ensure your configuration performs well.

3. **Monitor E/I Balance**: Keep an eye on `e_i_ratio` to ensure proper neural balance.

4. **Use Checkpoints Wisely**: Save checkpoints at key intervals, not every step.

5. **Profile Memory Usage**: Large simulations benefit from monitoring memory usage.

### Common Issues and Solutions

**Problem**: Brain doesn't learn enough.
**Solution**: Ensure `enableAllSystems()` is called and check curiosity/novelty levels.

**Problem**: Simulation too slow.
**Solution**: Benchmark first, then optimize configuration or reduce steps.

**Problem**: Memory usage high.
**Solution**: Use smaller brain configurations or save/load checkpoints.

That's it! You're now ready for advanced NLM usage with all the new powerful features.
