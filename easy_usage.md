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

### Example 3: Using the New Convenience Functions (Improved API)

```python
import pynlm

# NEW: Create brain with better defaults
config = pynlm.createConfigFromTemplate("explorer")
brain = pynlm.createValidatedBrain(config)
brain.initialize()

# NEW: Create world with preset
world = pynlm.worldPreset("maze")

# NEW: Create agent system with world
agent = pynlm.createAgentSystem(config, world)

# NEW: Run simulation easily
actions = pynlm.runSimulation(brain, world, 100)

print("Simulation complete! Actions taken:", len(actions))
```

### Example 4: Batch Operations for Advanced Users

```python
import pynlm

# Create multiple agents for simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 500)

world = pynlm.createSimpleWorld()
world.configure(50, 50, 8, 8)

# Create agent system
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Get sensor data for multiple time steps
all_percepts = []
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    all_percepts.append(percept)

# Process all percepts at once (NEW functionality)
processed_actions = pynlm.processAllSensoryInput(agent, all_percepts)

print("Processed", len(processed_actions), "actions in batch")
```

### Example 5: Configuration from Dictionary (NEW Feature)

```python
import pynlm

# NEW: Create configuration from Python dictionary
config_dict = {
    "brain.neuronCount": 2000,
    "brain.regionCount": 5,
    "world.width": 100.0,
    "world.height": 100.0,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "memory.enableWorkingMemory": True
}

config = pynlm.createConfigFromDict(config_dict)
brain = pynlm.createValidatedBrain(config)

print("Configuration created with", brain.getTotalNeuronCount(), "neurons")
```

### Example 6: Error Handling and Validation (NEW Features)

```python
import pynlm

# NEW: Create configuration with automatic validation
config = pynlm.createConfigFromTemplate("advanced")

# NEW: Validate before use
errors = config.validate()
if errors:
    print("Configuration errors:", errors)
    # Fix errors or use defaults
    config.set("brain.neuronCount", 1000)

brain = pynlm.createValidatedBrain(config)
brain.initialize()

print("Brain created successfully with validation")
```

### Example 7: Performance Monitoring (NEW Features)

```python
import pynlm

# Create brain and world
config = pynlm.createConfigFromTemplate("complex")
brain = pynlm.createValidatedBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# NEW: Run simulation and profile performance
agent = pynlm.createAgentSystem(config, world)
profile = pynlm.profileSimulation(brain, world, 1000)

print("Performance profile:")
for key, value in profile.items():
    print(f"  {key}: {value}")
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

### Pattern 3: Enable Brain Features (NEW Enhanced API)

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

### Pattern 4: Multi-Agent System (NEW Feature)

```python
def run_multi_agent_simulation(agent_configs, world, num_steps):
    # Create multiple agents
    agents = pynlm.createMultiAgentSystem(agent_configs, world)
    
    # Run simulation steps
    for step in range(num_steps):
        # Get percepts for all agents
        percepts = [world.getSensoryPercept() for _ in agents]
        
        # Process input for all agents (NEW batch operation)
        actions = pynlm.batchUpdateAgents(agents, percepts)
        
        # Apply actions to world
        for i, action in enumerate(actions):
            if action:  # Check if action is valid
                world.applyMotorCommand(action, world.getSimulationTime())
    
    return agents
```

### Pattern 5: Checkpoint and Recovery (NEW Features)

```python
def run_with_checkpoint(agent, world, checkpoint_name, interval=100):
    # Run simulation with periodic checkpoints
    for step in range(10000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        agent.getBrain().step(step)
        
        if step % interval == 0:
            # Save checkpoint (NEW functionality)
            pynlm.createCheckpoint(agent.getBrain(), checkpoint_name, f"checkpoints/{step}.nlm")
            
            # OPTIONAL: Restore from latest checkpoint if error occurred
            if should_reset:  # Your error checking logic
                latest = pynlm.listCheckpoints()[0] if pynlm.listCheckpoints() else None
                if latest:
                    pynlm.restoreCheckpoint(agent.getBrain(), latest)
    
    return agent
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

### Project 3: Complete Agent (NEW Enhanced Version)

```python
import pynlm

# Setup
config = pynlm.createConfigFromTemplate("explorer")
brain = pynlm.createValidatedBrain(config)
world = pynlm.worldPreset("open")
agent = pynlm.createAgentSystem(config, world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

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

### Project 4: Configuration Builder (NEW Feature)

```python
import pynlm

# Build complex configuration step by step
config = pynlm.createConfig()

# Set brain parameters
config.set("brain.neuronCount", 3000)
config.set("brain.regionCount", 8)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Set world parameters
config.set("world.width", 200.0)
config.set("world.height", 200.0)

# Set learning parameters
config.set("neuromodulation.enableDopamine", True)
config.set("neuromodulation.enableCuriosity", True)
config.set("neuromodulation.enableNovelty", True)

# Set memory parameters
config.set("memory.enableWorkingMemory", True)
config.set("memory.enableEpisodicMemory", True)
config.set("memory.maxMemoryCapacity", 10000)

# Validate and create brain
brain = pynlm.createValidatedBrain(config)

print("Complex brain created!")
```

### Project 5: Multi-Agent Race (NEW Advanced Feature)

```python
import pynlm

def create_explorer_agent(world):
    config = pynlm.createConfigFromTemplate("explorer")
    config.set("brain.neuronCount", 1000)
    return pynlm.createAgentSystem(config, world)

def create_planner_agent(world):
    config = pynlm.createConfigFromTemplate("planner")
    config.set("brain.neuronCount", 2000)
    return pynlm.createAgentSystem(config, world)

def race_agents(world, num_steps):
    # Create different types of agents
    explorer = create_explorer_agent(world)
    planner = create_planner_agent(world)
    
    agents = [explorer, planner]
    
    # Run race
    for step in range(num_steps):
        world.update(0.1)
        
        # Get sensory input for all agents
        percepts = [world.getSensoryPercept() for _ in agents]
        
        # Process and get actions (NEW batch operation)
        actions = pynlm.batchUpdateAgents(agents, percepts)
        
        # Apply actions
        for i, action in enumerate(actions):
            if action:
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check if any agent reached goal
        for i, agent in enumerate(agents):
            if agent.getBrain().getTotalSpikeCount() > 1000:
                print(f"Agent {i} reached goal!")
                return i  # Return winning agent index
    
    return None  # No winner

# Run race
world = pynlm.createSimpleWorld()
world.configure(100, 100, 8, 8)

winner = race_agents(world, 500)
print(f"Race finished! Winner: {winner}")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create brain with validation | `pynlm.createValidatedBrain(config)` |
| Create brain from template | `pynlm.createConfigFromTemplate("explorer")` |
| Create world | `pynlm.createSimpleWorld()` |
| Create world with preset | `pynlm.worldPreset("maze")` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Create agent system | `pynlm.createAgentSystem(config, world)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Run simulation | `pynlm.runSimulation(brain, world, steps)` |
| Batch operations | `pynlm.batchUpdateAgents(agents, percepts)` |
| Profile simulation | `pynlm.profileSimulation(brain, world, steps)` |
| Checkpoint brain | `pynlm.createCheckpoint(brain, "name")` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Use `pynlm.createValidatedBrain(config)` for better error messages

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify agent has enabled motor capabilities

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration values with `config.summary()`

**"Configuration validation failed"**
- Use `config.validate()` to see specific errors
- Check `pynlm.getConfigSchema()` for valid keys
- Use template configurations: `createConfigFromTemplate("default")`

**"Performance is slow"**
- Use `pynlm.profileSimulation()` to identify bottlenecks
- Reduce brain size with `config.set("brain.neuronCount", 500)`
- Use batch operations instead of individual calls

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details about the advanced features
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configuration templates and presets!
4. Try the multi-agent examples to see the power of batch operations
5. Use checkpoint and recovery for long-running simulations

---

## New Features for Advanced Users

The enhanced API provides these additional features:

1. **Template Configurations**: `createConfigFromTemplate("explorer")`, `createConfigFromTemplate("planner")`
2. **Validation**: `config.validate()` for error checking
3. **Batch Operations**: `batchUpdateAgents()`, `batchInjectCurrent()`
4. **Performance Monitoring**: `profileSimulation()`
5. **Checkpointing**: `createCheckpoint()`, `restoreCheckpoint()`
6. **Multi-Agent Systems**: `createMultiAgentSystem()`
7. **Configuration from Dictionary**: `createConfigFromDict(config_dict)`
8. **Error Handling**: `withErrorHandling()`, `safeExecute()`
9. **World Presets**: `worldPreset("maze")`, `worldPreset("open")`
10. **Brain Types**: `brainTemplate("simple")`, `brainTemplate("complex")`

That's it! You're now ready to use NLM with both the basic and advanced features. Happy simulating!