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

## NEW: Advanced Patterns and Convenience Functions

### Pattern 4: Easy Simulation Setup with createAgentSimulation

```python
import pynlm

# One function call creates everything!
brain, agent_brain, world = pynlm.createAgentSimulation(
    config=pynlm.createDefaultConfig(),
    world=pynlm.createSimpleWorld(),
    steps=100,
    dt=0.1
)

# World is automatically configured with agent at (0, 0)
print(f"Setup complete: {brain} | {agent_brain} | {world}")
```

### Pattern 5: Run Complete Episode with runEpisode

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
brain = pynlm.createBrain(config)
brain.initialize()
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run episode
brain, world, agent_brain = pynlm.runEpisode(
    config, world, brain, agent_brain, 
    steps=50, dt=0.1
)

print("Episode completed!")
```

### Pattern 6: Get Comprehensive Brain Statistics

```python
import pynlm

# Setup and run brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get detailed statistics
stats = pynlm.getBrainStats(brain)
print(stats)

# Get memory usage
mem_stats = pynlm.getMemoryStats(brain)
print(mem_stats)
```

### Pattern 7: Use Simulation Presets

```python
import pynlm

# Get different pre-configured brains
configs = {}
for preset in ["learning", "fast_learning", "default"]:
    configs[preset] = pynlm.createSimulationPreset(preset)
    print(f"{preset}: {len(configs[preset].getKeys())} configuration options")

# Use learning preset for complex tasks
learning_config = pynlm.createSimulationPreset("learning")
```

### Pattern 8: Simple Agent Creation with createQuickAgent

```python
import pynlm

# One function creates a ready-to-use agent with all features enabled
def createQuickAgent(preset="learning"):
    config = pynlm.createSimulationPreset(preset)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable advanced features
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableDevelopment(True)
    
    return brain, agent_brain, world

# Use it
brain, agent_brain, world = createQuickAgent("fast_learning")
```

### Pattern 9: Batch Processing with batchProcessEpisodes

```python
import pynlm

# Create multiple simulations for comparison
simulations = []
for i in range(3):
    config = pynlm.createSimulationPreset("learning")
    world = pynlm.createSimpleWorld()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    simulations.append((config, world, brain, agent_brain))

# Run all episodes in batch
results = pynlm.batchProcessEpisodes(simulations, stepsPerEpisode=20)
print(f"Processed {len(results)} episodes efficiently")
```

### Pattern 10: JSON Serialization for Data Export

```python
import pynlm

# Create and configure objects
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world_obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, 5.0, 0.5)
agent_body = pynlm.AgentBody()
agent_body.x = 1.0
agent_body.y = 2.0
agent_body.orientation = 0.5
agent_body.energy = 0.8
agent_body.health = 0.9
agent_body.age = 10

# Export to dictionaries (JSON-serializable)
export_data = {
    "config": config.to_dict(),
    "world_object": world_obj.to_dict(),
    "agent_body": agent_body.to_dict(),
    "brain_stats": pynlm.getBrainStats(brain),
    "memory_stats": pynlm.getMemoryStats(brain),
}

# Export to file or send to database
import json
with open("brain_data.json", "w") as f:
    json.dump(export_data, f, indent=2)

print("Data exported successfully to JSON file")
```

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

## Practical Examples with New Functions

### Example 1: One-Line Agent Setup (Beginner)

```python
import pynlm

# OLD WAY: 6 lines of setup
# config = pynlm.createDefaultConfig()
# brain = pynlm.createBrain(config)
# brain.initialize()
# world = pynlm.createSimpleWorld()
# world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
# agent_brain = pynlm.createAgentBrain(brain)
# agent_brain.initialize(world)

# NEW WAY: 1 line with createAgentSimulation
brain, agent_brain, world = pynlm.createAgentSimulation(
    config=pynlm.createDefaultConfig(),
    world=pynlm.createSimpleWorld(),
    steps=100,
    dt=0.1
)

print(f"Setup complete! Brain: {brain}, Agent: {agent_brain}, World: {world}")
```

### Example 2: One-Week Episode (Intermediate)

```python
import pynlm

# Setup with custom configuration
brain, agent_brain, world = pynlm.createAgentSimulation(
    config=pynlm.createSimulationPreset("learning"),
    world=pynlm.createSimpleWorld(),
    steps=50,
    dt=0.1
)

# Run the episode
brain, agent_brain, world = pynlm.runEpisode(
    config=pynlm.createSimulationPreset("learning"),
    world=pynlm.createSimpleWorld(),
    brain=brain,
    agentBrain=agent_brain,
    steps=50,
    dt=0.1
)

print(f"Episode complete! Brain stats: {pynlm.getBrainStats(brain)[:200]}...")
```

### Example 3: Data Collection and Export (Advanced)

```python
import pynlm

# Create agent with learning preset
brain, agent_brain, world = pynlm.createAgentSimulation(
    config=pynlm.createSimulationPreset("fast_learning"),
    world=pynlm.createSimpleWorld(),
    steps=100,
    dt=0.1
)

# Run simulation and collect data
for i in range(100):
    world.update(0.1)
    agent_brain.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Export everything for analysis
export_data = {
    "config": pynlm.createSimulationPreset("fast_learning").to_dict(),
    "brain_stats": pynlm.getBrainStats(brain),
    "memory_stats": pynlm.getMemoryStats(brain),
    "simulation_results": {
        "steps_run": 100,
        "neurons_firing": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "final_development_stage": brain.getDevelopmentalStage()
    }
}

import json
with open("simulation_data.json", "w") as f:
    json.dump(export_data, f, indent=2)

print("Simulation data exported to simulation_data.json")
```

### Example 4: Comparing Configurations (Performance Testing)

```python
import pynlm

# Test multiple configurations
presets = ["learning", "fast_learning", "default"]
results = {}

for preset in presets:
    print(f"\nTesting {preset} preset...")
    
    # Create and run simulation
    brain, agent_brain, world = pynlm.createAgentSimulation(
        config=pynlm.createSimulationPreset(preset),
        world=pynlm.createSimpleWorld(),
        steps=30,
        dt=0.1
    )
    
    brain, agent_brain, world = pynlm.runEpisode(
        config=pynlm.createSimulationPreset(preset),
        world=pynlm.createSimpleWorld(),
        brain=brain,
        agentBrain=agent_brain,
        steps=30,
        dt=0.1
    )
    
    # Store results
    results[preset] = {
        "brain_stats": pynlm.getBrainStats(brain),
        "memory_stats": pynlm.getMemoryStats(brain),
        "neurons_created": brain.getTotalNeuronCount(),
        "spikes_produced": brain.getTotalSpikeCount()
    }
    
    print(f"  ✓ {preset} completed with {brain.getTotalNeuronCount()} neurons")

print(f"\nComparison complete for {len(results)} presets!")
```

### Example 5: Batch Processing for Testing

```python
import pynlm

# Create multiple simulations for stress testing
simulations = []
for i in range(5):
    print(f"Creating simulation {i+1}/5...")
    
    config = pynlm.createSimulationPreset("learning")
    world = pynlm.createSimpleWorld()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    simulations.append((config, world, brain, agent_brain))

print("\nRunning batch simulation...")
# Note: The batchProcessEpisodes function requires the C++ implementation
# to be compiled. The setup above demonstrates how to prepare the data.

# Once compiled:
# try:
#     results = pynlm.batchProcessEpisodes(simulations, stepsPerEpisode=20)
#     print(f"✓ Batch processing complete! Processed {len(results)} episodes")
# except Exception as e:
#     print(f"! Batch processing requires compilation: {e}")
```
