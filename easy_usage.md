# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

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

### Example 3: Complete Agent with Learning

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
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

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
print("Excitement/Inhibition ratio:", brain.getExcitationInhibitionRatio())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)              # Explore new things
```

### Pattern 4: Advanced Configuration

```python
import pynlm

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.01)
config.set("random_seed", 12345)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Pattern 5: Performance Monitoring

```python
def benchmark_brain(brain, steps):
    import time
    start_time = time.time()
    for i in range(steps):
        brain.step(i)
    end_time = time.time()
    
    elapsed_ms = (end_time - start_time) * 1000
    steps_per_second = steps / (elapsed_ms / 1000)
    
    print(f"Benchmark: {steps} steps in {elapsed_ms:.2f} ms")
    print(f"Performance: {steps_per_second:.0f} steps/second")
    
    return elapsed_ms, steps_per_second

# Use it:
benchmark_brain(brain, 1000)
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

### `world.applyMotorCommand(action, time)`
Executes an action in the world.

### `agent.applyRewardModulation(reward, predicted)`
Teaches the brain from the results of actions.

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
print("Neurons:", brain.getTotalNeuronCount())
print("Spikes:", brain.getTotalSpikeCount())
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

### Project 3: Complete Learning Agent

```python
import pynlm

# Setup with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableAllFeatures()

# Create a simple environment with food and obstacles
world.addObject(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 10))
world.addObject(pynlm.WorldObject(8, 8, pynlm.WorldObjectType.Resource, 10))
world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Hazard, 5))

# Run agent for many steps
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"Agent position: ({agent.getBrain().getRegion(1)->getNeurons()[0]->getPosition().x}, "
              f"{agent.getBrain().getRegion(1)->getNeurons()[0]->getPosition().y})")

print("Learning simulation complete!")
print("Final statistics:", brain.getStatistics())
```

### Project 4: Developmental Brain

```python
import pynlm

# Create brain that develops over time
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Track development stages
stages = []
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    current_stage = brain.getDevelopmentalStage()
    if current_stage != stages[-1] if stages else False:
        stages.append(current_stage)
        print(f"Stage {len(stages)} reached: {current_stage}")

print(f"Development completed through {len(stages)} stages")
print("Final stage:", brain.getDevelopmentalStage())
```

### Project 5: Comparison Study

```python
import pynlm

def compare_configurations(config_list, steps):
    """Compare different brain configurations"""
    results = {}
    
    for i, config in enumerate(config_list):
        brain = pynlm.createBrain(config)
        brain.initialize()
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
        
        results[f"config_{i}"] = {
            "neurons": brain.getTotalNeuronCount(),
            "spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "e_i_ratio": brain.getExcitationInhibitionRatio(),
            "stage": brain.getDevelopmentalStage()
        }
    
    return results

# Compare different configurations
configs = [
    pynlm.createDefaultConfig(),  # Default
]

# Add custom configs
c1 = pynlm.createDefaultConfig()
c1.set("neuron_count", 500)
c1.set("connection_probability", 0.05)
configs.append(c1)

c2 = pynlm.createDefaultConfig()
c2.set("neuron_count", 3000)
c2.set("connection_probability", 0.2)
configs.append(c2)

results = compare_configurations(configs, 100)

# Print comparison
for name, stats in results.items():
    print(f"\n{name}:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
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

**"Learning isn't working"**
- Did you enable the learning features? `agent.enableRewardModulation(True)`
- Check that the agent is getting sensory input
- Verify the world is providing meaningful feedback

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Covers advanced usage patterns
   - Explains configuration options
   - Documents debugging techniques

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - Deep dive into the neural architecture
   - Learn about memory systems
   - Understand prediction and cognition

3. **Experiment with different configurations!**
   - Try varying neuron counts
   - Experiment with connection probabilities
   - Test different developmental stages

4. **Create your own projects**
   - Combine NLM with other libraries
   - Integrate with real-world simulations
   - Build custom learning environments

That's it! You're now ready to use NLM.

## Pro Tips for Getting Started

1. **Start simple**: Use `createDefaultConfig()` and keep the default settings
2. **Visualize**: Print brain statistics regularly to understand what's happening
3. **Experiment**: Try changing the neuron count and see how it affects behavior
4. **Document**: Keep notes on what works and what doesn't
5. **Iterate**: Start with simple examples and gradually add complexity

Remember: The brain needs time to "warm up" and learn. Don't expect perfect behavior immediately!

---

## Common Questions

**Q: Do I need to understand neural networks to use NLM?**
A: No! NLM provides a high-level interface. You can create and experiment with virtual brains without understanding the underlying neural mathematics.

**Q: What's the difference between Brain, AgentBrain, and the world?**
A: The Brain does the thinking, the AgentBrain connects the brain to the world, and the World provides the environment and sensory input.

**Q: Can I save and load brain states?**
A: Yes! Use `brain.save("filename.json")` and `brain.load("filename.json")` to persist brain states.

**Q: How long should I run simulations?**
A: Start with 100-200 steps, then gradually increase. More steps allow the brain to learn and develop.

**Q: What's the best way to debug?**
A: Print brain statistics regularly (`brain.getFiringNeuronCount()`, `brain.getTotalSpikeCount()`), and enable the logging features.

Enjoy exploring the world of artificial developmental brains with NLM! 🚀