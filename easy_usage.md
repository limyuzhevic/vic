# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Quick Warning About Current Issues

**Known Issues & Limitations:**
- The current Python bindings use deprecated patterns like `std::shared_ptr` return policies that can cause crashes
- Some classes have incomplete implementations (e.g., `AgentBrain` has unused methods like `getSensoryInputSize()` and `getMotorOutputSize()`)
- Error handling is minimal - `nullptr` checks are scattered throughout the code
- The `AgentBrain::initialize()` method requires a `SimpleWorld` object even though the constructor doesn't enforce this

**Workaround for Beginners:**
- Use the easy patterns in this guide - they work with the current version
- For advanced usage, create a `SimpleWorld` object before the agent
- Check return values from methods when possible
- Consider using the complete `HOW_TO_USE.md` guide for more robust examples

## Advanced Usage Note

Once you understand the basics, consider reading `HOW_TO_USE.md` for production-ready code, or `docs/ARCHITECTURE.md` to understand the full system architecture.

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

## Common Advanced Patterns

### Pattern 4: Save and Load Brain States

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Save brain state (checkpoint)
brain.save("my_brain_checkpoint.bin")

# Later: Load saved brain
brain2 = pynlm.createBrain(config)
brain2.load("my_brain_checkpoint.bin")
```

### Pattern 5: Region Management

```python
import pynlm

# Create brain and add regions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add specific regions for different functions
region_cortex = brain.addRegion("cortex")
region_motor = brain.addRegion("motor")
region_sensory = brain.addRegion("sensory")

print(f"Total regions: {brain.getRegionCount()}")
```

### Pattern 6: Complete Error-Handling Version

```python
import pynlm
import traceback

def safe_simulation(brain, world, agent, steps):
    """Simulation with error handling"""
    try:
        # Validate inputs
        if not brain or not world or not agent:
            raise ValueError("Brain, world, and agent are required")
        
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check if simulation should stop
            if brain.getTotalSpikeCount() > 100000:  # Safety limit
                print(f"Simulation stopped at step {step} (spike limit reached)")
                break
                
    except Exception as e:
        print(f"Simulation error: {e}")
        traceback.print_exc()
        return False
    
    return True

# Usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

success = safe_simulation(brain, world, agent, 1000)
```

### Pattern 7: Advanced Configuration

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()

# Modify configuration parameters
config.set("brain.neuron_count", 2000)  # More neurons
config.set("brain.synapse_density", 0.2)  # More connections
config.set("plasticity.stdp.enable", True)  # Enable STDP
config.set("plasticity.hebbian.enable", True)  # Enable Hebbian
config.set("neuromod.dopamine.scale", 2.0)  # Stronger dopamine
config.set("neuromod.curiosity.enable", True)  # Enable curiosity
config.set("neuromod.novelty.enable", True)  # Enable novelty detection

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize agent with world
world = pynlm.createSimpleWorld()
world.configure(30, 30, 16, 16)  # Larger world
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

## Common Advanced Patterns

### Pattern 8: Multi-Agent Simulation

```python
import pynlm

# Create multiple agents sharing brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # One brain per agent

agents = []
for agent_id in range(3):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Each agent can have different parameters
    agent.enableRewardModulation(True)
    
    agents.append({
        'id': agent_id,
        'brain': brain,
        'world': world,
        'agent': agent
    })

# Run simulation with all agents
for step in range(500):
    for agent_data in agents:
        agent_data['world'].update(0.1)
        percept = agent_data['world'].getSensoryPercept()
        agent_data['agent'].processSensoryInput(percept)
        agent_data['brain'].step(step)
        action = agent_data['agent'].decodeMotorCommand()
        agent_data['world'].applyMotorCommand(action, agent_data['world'].getSimulationTime())
```

### Pattern 9: Brain Evolution Experiment

```python
import pynlm
import random

# Create parent brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
parent_brain = pynlm.createBrain(config)
parent_brain.initialize()

# Run parent to learn
for i in range(500):
    parent_brain.step(i)

# Save parent checkpoint
parent_brain.save("parent_brain.bin")

# Create child brain (mutated from parent)
def create_mutated_brain(parent_path):
    # Load parent
    child_brain = pynlm.createBrain(config)
    child_brain.load(parent_path)
    
    # Mutate some parameters (simple example)
    if random.random() < 0.3:  # 30% chance to mutate
        # Randomly change some parameters
        params = ["brain.neuron_count", "brain.synapse_density"]
        for param in params:
            if random.random() < 0.5:
                if param == "brain.neuron_count":
                    new_value = max(100, min(5000, child_brain.getConfig().getOr[int64_t](param, 1000) + random.randint(-200, 200)))
                    child_brain.getConfig().set(param, new_value)
                elif param == "brain.synapse_density":
                    new_value = max(0.01, min(0.5, child_brain.getConfig().getOr[float](param, 0.1) + random.uniform(-0.05, 0.05)))
                    child_brain.getConfig().set(param, new_value)
    
    return child_brain

# Create and train child
c_child = create_mutated_brain("parent_brain.bin")
c_child.initialize()

# Train child
for i in range(300):
    c_child.step(i)

print(f"Parent performance: {parent_brain.getTotalSpikeCount()} spikes")
print(f"Child performance: {c_child.getTotalSpikeCount()} spikes")
```

### Pattern 10: Real-time Monitoring

```python
import pynlm
import time
import threading
import queue

def monitor_brain(brain, event_queue, stop_event):
    """Background thread to monitor brain metrics"""
    while not stop_event.is_set():
        try:
            # Collect metrics
            metrics = {
                'step': brain.getTotalSpikeCount(),
                'firing': brain.getFiringNeuronCount(),
                'rate': brain.getAverageFiringRate(),
                'development': brain.getDevelopmentalStage(),
                'time': time.time()
            }
            
            event_queue.put(metrics)
            time.sleep(0.1)  # Monitor every 100ms
        except:
            break

# Create and start monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Start monitoring
stop_monitor = threading.Event()
metric_queue = queue.Queue()
monitor_thread = threading.Thread(target=monitor_brain, args=(brain, metric_queue, stop_monitor))
monitor_thread.daemon = True
monitor_thread.start()

# Run simulation while monitoring
for step in range(100):
    brain.step(step)
    
    # Check for new metrics
    while not metric_queue.empty():
        metrics = metric_queue.get_nowait()
        if metrics['rate'] > 50:  # Alert if firing rate too high
            print(f"HIGH FIRE RATE: {metrics['rate']:.2f} Hz at step {metrics['step']}")

# Stop monitoring
stop_monitor.set()
monitor_thread.join(timeout=1.0)
```

## Troubleshooting Advanced Issues

### Issue: Simulation doesn't progress
```python
# Solution: Check for common mistakes
brain.initialize()  # Always call initialize!
for i in range(100):
    brain.step(i)  # Missing step() causes no activity
    
# OR
world.update(0.1)  # Missing world update
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)  # Missing this causes no learning
```

### Issue: No movement detected
```python
# Solution: Check motor neurons are connected
if brain.getTotalNeuronCount() < 100:
    print("Too few neurons - increase brain.neuron_count")
    
if agent.getMotorOutputSize() == 0:
    print("Motor system not properly initialized")
```

### Issue: Excessive memory usage
```python
# Solution: Configure more efficiently
config.set("brain.neuron_count", 500)  # Start smaller
config.set("brain.synapse_density", 0.05)  # Fewer connections

# Or use specialized brain types
g_config = pynlm.createDefaultConfig()
g_config.set("brain.neuron_count", 100)
g_brain = pynlm.createBrain(g_config)  # Small 'starter' brain
```

### Issue: Model not learning
```python
# Solution: Enable learning mechanisms
agent.enableRewardModulation(True)      # Reward-based learning
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableCuriosity(True)            # Exploration rewards
agent.enableDevelopment(True)           # Age-based plasticity
```

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Check `docs/PHASE6_FINAL_AUDIT.md` for latest integration results

That's it! You're now ready to use NLM, even for more complex scenarios.
