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

## Advanced Examples

### Example 3: Learning and Memory

```python
import pynlm
import time

# Setup with advanced features
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # More neurons for complex learning
config.set("enable_reward_modulation", True)
config.set("enable_structural_plasticity", True)
config.set("enable_development", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.setAgentStart(10, 10)  # Start in center
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
print("Starting learning simulation...")
start_time = time.time()

for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Let brain process
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Agent decides action
    action = agent.decodeMotorCommand()
    
    # Apply action and get reward
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward based on agent's performance
    if world.getAgentBody().health > 50:
        reward = 1.0
    else:
        reward = -0.5
    
    # Apply reward modulation for learning
    predicted_reward = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Print progress
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
        print()

print(f"Learning simulation complete in {time.time() - start_time:.1f} seconds")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Final development stage: {brain.getDevelopmentalStage()}")
```

### Example 4: Brain with Memory and Replay

```python
import pynlm
import time

# Create brain with memory features
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("enable_episodic_memory", True)
config.set("enable_working_memory", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Set developmental stage to enable advanced memory
brain.setDevelopmentalStage(nlm.DevelopmentalStage.Maturation)

# Create world with resources
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)

# Add resources for the brain to learn about
world.addObject(5, 5, nlm.WorldObjectType.Resource, 10.0)
world.addObject(25, 25, nlm.WorldObjectType.Resource, 10.0)
world.setAgentStart(15, 15)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Memory and Replay Simulation")
print("===========================")

for episode in range(5):  # 5 episodes
    print(f"\n--- Episode {episode + 1} ---")
    
    # Track episode stats
    episode_spikes = 0
    episode_reward = 0.0
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Apply action and check for rewards
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world
        if action.getType() == nlm.ActionType.Eat and world.isResourceAt(world.getAgentBody().x, world.getAgentBody().y):
            reward = 5.0  # Found resource
            world.removeObject(world.getAgentBody().x, world.getAgentBody().y)  # Consume resource
        else:
            reward = -0.1  # Cost of doing nothing
        
        episode_reward += reward
        episode_spikes += brain.getTotalSpikeCount()
        
        # Print episode progress
        if step % 50 == 0:
            print(f"  Step {step}: Reward {reward:.2f}, Total {episode_reward:.2f}")
    
    # Save brain state (checkpoint)
    brain.save(f"checkpoint_episode_{episode}.bin")
    
    # Trigger memory replay (simulated with development update)
    if episode < 4:  # Don't replay after last episode
        agent.updateDevelopment(0.1)
    
    print(f"  Episode summary: {episode_spikes} spikes, {episode_reward:.2f} total reward")

print("\nSimulation complete!")
print(f"Brain saved to checkpoint files for replay")
```

### Example 5: Multiple Brain Regions and Integration

```python
import pynlm

# Create brain with multiple regions
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 3)  # Multiple regions

brain = pynlm.createBrain(config)
brain.initialize()

# Add different regions for different functions
region1_id = brain.addRegion("Sensory")
region2_id = brain.addRegion("Motor")
region3_id = brain.addRegion("WorkingMemory")

print("Multi-Region Brain Simulation")
print("=============================")

# Test each region
print(f"Region count: {brain.getRegionCount()}")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run integration test
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"\nStep {step}:")
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            neurons = region.getAllNeurons()
            firing = region.getFiringCount()
            print(f"  {region.getName()}: {neurons.size()} neurons, {firing} firing")

print("\nIntegration test complete!")
```

### Example 6: Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Override basic settings
config.set("neuron_count", 800)
config.set("simulation_timestep", 0.002)  # Slower simulation
config.set("random_seed", 12345)

# Configure STDP parameters
config.set("stdp_ltp_weight", 0.03)
config.set("stdp_ltd_weight", 0.02)
config.set("stdp_tau", 25.0)

# Configure structural plasticity
config.set("synaptogenesis_rate", 0.0002)
config.set("pruning_rate", 0.00005)

# Save custom configuration
config.saveToFile("my_custom_config.cfg")

print("Custom configuration created and saved to 'my_custom_config.cfg'")
print("Configuration summary:")
print(config.summary())

# Load the custom configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_custom_config.cfg")

brain = pynlm.createBrain(config2)
brain.initialize()
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
```

### Example 7: Performance Benchmarking

```python
import pynlm
import time

# Test different brain sizes
def benchmark_brain_size(neuron_counts, steps=100):
    results = {}
    
    for neuron_count in neuron_counts:
        print(f"Testing brain with {neuron_count} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        duration = end_time - start_time
        spikes_per_second = brain.getTotalSpikeCount() / duration
        
        results[neuron_count] = {
            'duration': duration,
            'spikes_per_second': spikes_per_second,
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        print(f"  Duration: {duration:.3f}s")
        print(f"  Spikes per second: {spikes_per_second:.1f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return results

# Run benchmark
neuron_counts = [100, 500, 1000, 2000]
print("Performance Benchmark")
print("======================")
benchmark_brain_size(neuron_counts, steps=50)
```

### Example 8: Advanced Error Handling

```python
import pynlm

def safe_simulation(num_steps=100, config_file=None):
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Load custom config if provided
        if config_file:
            if not config.loadFromFile(config_file):
                print(f"Warning: Could not load config file '{config_file}', using defaults")
        
        # Validate configuration
        if config.get("neuron_count", 500) <= 0:
            raise ValueError("Neuron count must be positive")
        
        if config.get("simulation_timestep", 0.001) <= 0:
            raise ValueError("Simulation timestep must be positive")
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        # Initialize with error checking
        if not brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        print("Simulation started successfully")
        
        # Run simulation
        for step in range(num_steps):
            try:
                brain.step(step)
                
                # Print progress every 10 steps
                if step % 10 == 0:
                    print(f"Step {step}/{num_steps}: {brain.getFiringNeuronCount()} neurons firing")
                    
            except Exception as e:
                print(f"Error at step {step}: {e}")
                raise
        
        print("Simulation completed successfully")
        return brain
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        return None

# Example usage
brain = safe_simulation(50, "my_config.cfg")
if brain:
    print(f"Simulation finished with {brain.getTotalSpikeCount()} total spikes")
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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM at beginner and advanced levels.
