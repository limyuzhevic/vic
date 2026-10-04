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

## Command-Line Examples (New Section)

### Running the C++ Demo

```bash
# Run Phase 3 demo (basic world interaction)
./nlm_phase3_demo 5 200

# Run Phase 4 demo (cognitive experiments)
./nlm_phase4_demo 100

# Run Phase 6 demo (full integration test)
./nlm_phase6_demo
```

### Command-Line Brain Configuration

```bash
# Create brain with custom neuron count
./nlm --neuron_count=5000 --plasticity.stdp.learning_rate=0.001

# Export brain state
./nlm --export-brain=my_brain.gz --compression=2

# Import brain state with validation
./nlm --import-brain=saved_brain.gz --validate
```

### Command-Line Performance Monitoring

```bash
# Run with performance monitoring
./nlm --performance.optimization_level=3 --simulation.max_steps=10000

# Compare brain states
./nlm --compare-brain=state1.gz,state2.gz --similarity

# Create configuration profile
./nlm --create-profile=research --export-config=research_config.json
```

---

## Performance Monitoring Basics (New Section)

### Monitoring Brain Performance

```python
import pynlm
import time

# Create brain with monitoring enabled
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor performance over time
performance_data = []
start_time = time.time()

for step in range(1000):
    step_start = time.time()
    
    brain.step(step)
    
    # Collect metrics
    firing_rate = brain.getAverageFiringRate()
    step_time = time.time() - step_start
    
    performance_data.append({
        'step': step,
        'firing_rate': firing_rate,
        'step_time': step_time,
        'memory_usage': brain.getWorkingMemory().size() if brain.getWorkingMemory() else 0
    })
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {firing_rate:.2f} Hz firing rate, {step_time:.4f}s per step")

end_time = time.time()

# Analyze performance
total_time = end_time - start_time
avg_step_time = sum(d['step_time'] for d in performance_data) / len(performance_data)
max_step_time = max(d['step_time'] for d in performance_data)
avg_firing_rate = sum(d['firing_rate'] for d in performance_data) / len(performance_data)

print(f"\nPerformance Summary:")
print(f"Total simulation time: {total_time:.2f} seconds")
print(f"Average step time: {avg_step_time:.4f} seconds")
print(f"Maximum step time: {max_step_time:.4f} seconds")
print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
print(f"Peak memory usage: {max(d['memory_usage'] for d in performance_data)}")
```

### Experiment Monitoring

```python
import pynlm

# Setup monitoring for an experiment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable monitoring features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Monitor experiment progress
experiment_metrics = {
    'steps_completed': 0,
    'total_reward': 0.0,
    'max_curiosity': 0.0,
    'memory_growth': [],
    'performance_trend': []
}

for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Collect metrics
    experiment_metrics['steps_completed'] = step + 1
    experiment_metrics['total_reward'] += reward
    experiment_metrics['max_curiosity'] = max(experiment_metrics['max_curiosity'], agent.getCuriosityLevel())
    experiment_metrics['memory_growth'].append(
        brain.getWorkingMemory().size() if brain.getWorkingMemory() else 0
    )
    
    # Track performance trend
    if step % 50 == 0:
        firing_rate = brain.getAverageFiringRate()
        experiment_metrics['performance_trend'].append({
            'step': step,
            'firing_rate': firing_rate,
            'curiosity': agent.getCuriosityLevel()
        })
        
        print(f"Step {step}: Firing rate {firing_rate:.2f} Hz, "
              f"Curiosity {agent.getCuriosityLevel():.3f}, "
              f"Reward {reward:.3f}")

# Calculate experiment statistics
total_steps = experiment_metrics['steps_completed']
avg_reward_per_step = experiment_metrics['total_reward'] / total_steps
memory_growth = experiment_metrics['memory_growth'][-1] - experiment_metrics['memory_growth'][0] if len(experiment_metrics['memory_growth']) > 1 else 0

print(f"\nExperiment Results:")
print(f"Total steps completed: {total_steps}")
print(f"Average reward per step: {avg_reward_per_step:.4f}")
print(f"Peak curiosity level: {experiment_metrics['max_curiosity']:.3f}")
print(f"Memory growth: {memory_growth} units")
print(f"Performance trend: {'Improving' if len(experiment_metrics['performance_trend']) > 1 and experiment_metrics['performance_trend'][-1]['firing_rate'] > experiment_metrics['performance_trend'][0]['firing_rate'] else 'Stable'}")
```

---

## Experiment Management Basics (New Section)

### Creating Simple Experiments

```python
import pynlm

# Experiment 1: Simple Learning Task
print("=== Experiment 1: Simple Learning Task ===")

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run learning episode
for step in range(300):
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Track learning progress
    if step % 50 == 0:
        print(f"Step {step}: "
              f"Firing {brain.getFiringNeuronCount()} neurons, "
              f"Curiosity {agent.getCuriosityLevel():.3f}, "
              f"Reward {reward:.3f}")

print("Experiment 1 complete!")

# Experiment 2: Brain Development Study
print("\n=== Experiment 2: Brain Development Study ===")

# Create brain with developmental stages
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=8, height=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Monitor development across the experiment
developmental_stages = []
firing_rates = []
curiosity_levels = []

for step in range(200):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    # Record development metrics
    developmental_stages.append(brain.getDevelopmentalStage())
    firing_rates.append(brain.getAverageFiringRate())
    curiosity_levels.append(agent.getCuriosityLevel())
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    agent.applyRewardModulation(0.0, 0.0)  # No reward for simplicity
    agent.updateDevelopment(0.1)
    
    # Print development summary every 50 steps
    if step % 50 == 0:
        stage = brain.getDevelopmentalStage()
        firing_rate = brain.getAverageFiringRate()
        curiosity = agent.getCuriosityLevel()
        
        print(f"Step {step}: Stage {stage}, "
              f"Firing {firing_rate:.2f} Hz, "
              f"Curiosity {curiosity:.3f}")

print("Experiment 2 complete!")

# Summary of developmental progression
print(f"\nDevelopment Summary:")
print(f"Final stage: {developmental_stages[-1]}")
print(f"Stage progression: {developmental_stages}")
print(f"Firing rate trend: {firing_rates}")
print(f"Curiosity trend: {curiosity_levels}")
```

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

That's it! You're now ready to use NLM.
