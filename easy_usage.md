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

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Advanced Examples

When you're ready to use more advanced NLM features:

### Example 4: Complete Agent with Learning and Development

```python
import pynlm

# Create custom configuration for advanced features
config = pynlm.createDefaultConfig()

# Enable advanced learning features
config.set("enable_reward_modulation", True)
config.set("enable_structural_plasticity", True)
config.set("enable_development", True)
config.set("enable_curiosity", True)

# Create brain with custom parameters
brain = pynlm.createBrain(config)
brain.initialize()

# Create world with visual and audio sensors
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8, 
               audioChannels=4, audioRadius=5.0)
world.reset()

# Create agent with advanced capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)   # Grow new connections
agent.enableDevelopment(True)            # Brain matures over time
agent.enableCuriosity(True)              # Explore new things

# Run long-term learning simulation
for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes and learns
    brain.step(step)
    
    # Agent makes decisions
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    if step % 50 == 0:
        agent.applyRewardModulation(1.0, 0.5)  # Give reward
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getTotalSpikeCount()} total spikes, "
              f"curiosity={agent.getCuriosityLevel():.2f}")

print("Advanced simulation complete!")
print(f"Final brain developed to stage {brain.getDevelopmentalStage()}")
print(f"Total connections created: {brain.getTotalSynapseCount()}")
```

### Example 5: Brain Checkpointing and Persistence

```python
import pynlm
import os

# Create and run a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)
    
    # Save checkpoint every 25 steps
    if i % 25 == 0 and i > 0:
        checkpoint_file = f"checkpoint_step_{i}.bin"
        if brain.save(checkpoint_file):
            print(f"Checkpoint saved: {checkpoint_file}")
        else:
            print(f"Failed to save checkpoint: {checkpoint_file}")

# Test loading from checkpoint
print("\n--- Testing checkpoint loading ---")
if os.path.exists("checkpoint_step_50.bin"):
    # Create new brain
    loadedBrain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Load from checkpoint
    if loadedBrain.load("checkpoint_step_50.bin"):
        print("Successfully loaded brain from checkpoint")
        print(f"Loaded brain has {loadedBrain.getTotalNeuronCount()} neurons and "
              f"{loadedBrain.getTotalSynapseCount()} synapses")
        print(f"Last spike time: {loadedBrain.getTotalSpikeCount()} total spikes")
    else:
        print("Failed to load brain from checkpoint")
else:
    print("Checkpoint file not found")

# Cleanup
for i in [0, 25, 50, 75]:
    checkpoint_file = f"checkpoint_step_{i}.bin"
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"Cleaned up: {checkpoint_file}")
```

### Example 6: Multi-Region Brain Architecture

```python
import pynlm

# Create a multi-region brain
config = pynlm.createDefaultConfig()
config.set("region_count", 3)

brain = pynlm.createBrain(config)
brain.initialize()

# Create three regions with different characteristics
region0 = brain.addRegion("Sensory")  # Primary sensory processing
region1 = brain.addRegion("Motor")    # Motor control
region2 = brain.addRegion("Association")  # Higher-level associations

print("Multi-region brain created:")
print(f"Total regions: {brain.getRegionCount()}")
print(f"Region 0 (Sensory): {region0->getTotalNeuronCount()} neurons")
print(f"Region 1 (Motor): {region1->getTotalNeuronCount()} neurons")
print(f"Region 2 (Association): {region2->getTotalNeuronCount()} neurons")

# Create inter-region connections
brain.addInterRegionConnection(0, 1, weight=0.5)   # Sensory -> Motor
brain.addInterRegionConnection(1, 2, weight=0.3)   # Motor -> Association
brain.addInterRegionConnection(2, 0, weight=0.2)   # Association -> Sensory

print(f"Inter-region connections: {brain.getTotalSynapseCount()}")

# Run simulation with inter-region communication
for step in range(50):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}:")
        for i in range(brain.getRegionCount()):
            region = brain.getRegion(i);
            print(f"  Region {i}: {region->getTotalNeuronCount()} neurons, "
                  f"{region->getFiringNeuronCount()} firing")
```

### Example 7: Performance Testing and Benchmarking

```python
import pynlm
import time

def benchmark_brain_performance(neuron_counts, steps_per_test):
    """Benchmark brain performance with different sizes"""
    results = []
    
    for neuron_count in neuron_counts:
        print(f"\nBenchmarking brain with {neuron_count} neurons...")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        start_spikes = brain.getTotalSpikeCount()
        
        for step in range(steps_per_test):
            brain.step(step)
        
        end_time = time.time()
        end_spikes = brain.getTotalSpikeCount()
        
        elapsed = end_time - start_time
        spikes_generated = end_spikes - start_spikes
        spikes_per_second = spikes_generated / elapsed if elapsed > 0 else 0
        
        result = {
            "neuron_count": neuron_count,
            "steps": steps_per_test,
            "elapsed_time": elapsed,
            "spikes_generated": spikes_generated,
            "spikes_per_second": spikes_per_second,
            "efficiency": spikes_generated / neuron_count
        }
        
        results.append(result)
        
        print(f"  Time: {elapsed:.3f}s")
        print(f"  Spikes generated: {spikes_generated}")
        print(f"  Performance: {spikes_per_second:.1f} spikes/sec")
        print(f"  Efficiency: {result['efficiency']:.3f} spikes/neuron")
    
    return results

# Run benchmarks
neuron_counts = [100, 500, 1000, 2000]
steps_per_test = 100

print("NLM Brain Performance Benchmark")
print("=" * 40)

results = benchmark_brain_performance(neuron_counts, steps_per_test)

print("\nBenchmark Summary:")
print("-" * 40)
print(f"{'Neurons':>10} {'Steps':>8} {'Time (s)':>10} {'Spikes/s':>12} {'Efficiency':>12}")
print("-" * 40)

for r in results:
    print(f"{r['neuron_count']:>10} {r['steps']:>8} {r['elapsed_time']:>10.3f} "
          f"{r['spikes_per_second']:>12.1f} {r['efficiency']:>12.3f}")
```

### Example 8: Advanced Configuration Management

```python
import pynlm

# Create brain with JSON configuration
config_content = '''{
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15,
    "enable_reward_modulation": true,
    "enable_structural_plasticity": true,
    "enable_development": true,
    "enable_curiosity": true,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "learning_rate": 0.01
}'''

# Save to file
with open("advanced_config.json", "w") as f:
    f.write(config_content)

# Load configuration from file
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Load from JSON file
if brain.load("advanced_config.json"):
    print("Successfully loaded configuration from JSON file")
    print(f"Configuration entries: {brain.getTotalSynapseCount()} synapses configured")
else:
    print("Failed to load configuration from JSON file")

# Override with command line arguments
brain.loadFromArgs(["--neuron_count=1000", "--connection_probability=0.2"])
print(f"After command line override: {brain.getTotalSynapseCount()} synapses configured")

# Save updated configuration
if brain.save("updated_config.json"):
    print("Successfully saved updated configuration")

# Clean up
import os
if os.path.exists("advanced_config.json"):
    os.remove("advanced_config.json")
if os.path.exists("updated_config.json"):
    os.remove("updated_config.json")
```

## Quick Reference for Advanced Features

| Feature | Method | Description |
|---------|--------|-------------|
| **Reward Learning** | `enableRewardModulation(True)` | Enable reward-based learning |
| **Structural Growth** | `enableStructuralPlasticity(True)` | Allow creation/removal of synapses |
| **Brain Development** | `enableDevelopment(True)` | Brain matures over time |
| **Curiosity** | `enableCuriosity(True)` | Drives exploration behavior |
| **Checkpointing** | `brain.save("file.bin")` | Save brain state to file |
| **Loading** | `brain.load("file.bin")` | Load brain state from file |
| **Multi-Region** | `brain.addRegion("name")` | Create brain region |
| **Inter-Region** | `brain.addInterRegionConnection(src, dst, weight)` | Connect regions |
| **Performance** | `brain.getTotalSynapseCount()` | Get total synapses |
| **Activity** | `brain.getTotalSpikeCount()` | Get total spikes |

## Troubleshooting Advanced Usage

**"My brain isn't learning anything"**
- Check that `enableRewardModulation(True)` is called
- Verify rewards are being applied with `applyRewardModulation()`
- Ensure `enableCuriosity(True)` is enabled for exploration

**"Brain memory usage is too high"**
- Reduce `neuron_count` in configuration
- Disable `enableDevelopment(True)` if not needed
- Use smaller world dimensions

**"Checkpoint files are not being saved"**
- Check file permissions in current directory
- Ensure sufficient disk space
- Verify brain has actually processed some steps before saving

**"Multi-region brain isn't working"**
- Make sure regions are properly initialized
- Check inter-region connection setup
- Verify region connectivity is working

That's it! You now have the advanced usage guide with examples for:
1. Complete learning agents with development
2. Brain checkpointing and persistence
3. Multi-region brain architecture
4. Performance testing and benchmarking
5. Advanced configuration management

You're ready to build sophisticated neural systems with NLM! 🚀
