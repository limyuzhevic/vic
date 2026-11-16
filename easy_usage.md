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

### Project 3: Complete Agent with Enhanced Features

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced features."""
    
    # 1. Create configuration with JSON support
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    config.set("simulation.timestep", 0.001)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Get brain information using enhanced API
    print(brain.getBrainInfo())
    
    # 4. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 5. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # 6. Initialize agent with world
    agent.initialize(world)
    
    # 7. Enable enhanced learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 8. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.1 if world.getAgentBody().health > 0.5 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # 9. Show final agent information
    print("\n=== Final Agent Statistics ===")
    print(agent.getAgentInfo())
    print(brain.getBrainInfo())
    
    print("Agent simulation complete!")

# Run the simulation
run_agent_simulation(1000)
```

### Project 4: Configuration Management with JSON

```python
import pynlm
import json

def configure_brain_with_json():
    """Demonstrate JSON configuration management."""
    
    # 1. Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # 2. Export current configuration as JSON
    config_json = brain.getConfig().toJSON()
    print("Current configuration (JSON format):")
    print(config_json)
    
    # 3. Modify configuration using JSON patch
    json_patch = json.dumps({
        "updates": [
            {"key": "brain.neuron_count", "value": 2000},
            {"key": "brain.region_count", "value": 3}
        ]
    })
    
    brain.getConfig().applyJSONPatch(json_patch)
    
    # 4. Verify changes by exporting again
    updated_json = brain.getConfig().toJSON()
    print("\nUpdated configuration (JSON format):")
    print(updated_json)
    
    # 5. Save configuration to JSON file
    brain.getConfig().saveToJSON("brain_config.json")
    
    # 6. Load configuration back from JSON file
    loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    loaded_config = loaded_brain.getConfig()
    loaded_from_file = loaded_config.loadFromModern("brain_config.json")
    
    print(f"\nConfiguration loaded successfully. Total entries: {len(loaded_config.getKeys())}")
    
    return brain

# Run configuration management
brain = configure_brain_with_json()
```

### Project 5: Advanced Brain Monitoring and Debugging

```python
def monitor_brain_diagnostics(brain, agent, world, steps=500):
    """Monitor brain diagnostics and provide detailed reports."""
    
    # Initialize monitoring
    print("=== Brain Monitoring Started ===")
    print(f"Initial Brain Info:\n{brain.getBrainInfo()}")
    print(f"\nInitial Agent Info:\n{agent.getAgentInfo()}")
    
    # Monitor brain development
    development_stages = []
    firing_rates = []
    spike_counts = []
    neuromodulation_levels = []
    
    for step in range(steps):
        # Update world and brain
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
        
        # Collect diagnostics
        development_stages.append(int(brain.getDevelopmentalStage()))
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
        neuromodulation_levels.append(agent.getNeuromodulationLevel())
        
        # Generate report every 100 steps
        if step % 100 == 0:
            print(f"\n--- Step {step} Diagnostics ---")
            print(f"Brain Info:\n{brain.getBrainInfo()}")
            print(f"Agent Info:\n{agent.getAgentInfo()}")
    
    # Generate final report
    print("\n=== Final Diagnostics Report ===")
    
    # Brain performance analysis
    avg_firing_rate = sum(firing_rates) / len(firing_rates) if firing_rates else 0
    total_spikes = spike_counts[-1] if spike_counts else 0
    max_firing_rate = max(firing_rates) if firing_rates else 0
    
    print(f"Brain Performance Summary:")
    print(f"  - Average Firing Rate: {avg_firing_rate:.3f}")
    print(f"  - Maximum Firing Rate: {max_firing_rate:.3f}")
    print(f"  - Total Spikes Generated: {total_spikes}")
    print(f"  - Development Stage Consistency: {len(set(development_stages))} unique stages")
    
    # Neuromodulation analysis
    avg_neuromodulation = sum(neuromodulation_levels) / len(neuromodulation_levels) if neuromodulation_levels else 0
    curiosity_peaks = sum(1 for level in neuromodulation_levels if level > 0.7)
    
    print(f"\nNeuromodulation Analysis:")
    print(f"  - Average Neuromodulation Level: {avg_neuromodulation:.3f}")
    print(f"  - Curiosity Activation Events: {curiosity_peaks}")
    
    # Agent performance analysis
    agent_perf = agent.getAgentInfo()
    print(f"\nAgent Performance:\n{agent_perf}")
    
    # Network health assessment
    health_score = calculate_network_health(brain, agent)
    print(f"\nNetwork Health Score: {health_score:.1f}/10.0")
    
    return {
        "brain_diagnostics": {
            "firing_rates": firing_rates,
            "spike_counts": spike_counts,
            "development_stages": development_stages,
            "avg_firing_rate": avg_firing_rate,
            "total_spikes": total_spikes
        },
        "agent_diagnostics": {
            "neuromodulation_levels": neuromodulation_levels,
            "curiosity_events": curiosity_peaks
        },
        "overall_health": health_score
    }

def calculate_network_health(brain, agent):
    """Calculate overall network health score."""
    # Simple health calculation based on multiple factors
    firing_rate_score = min(brain.getAverageFiringRate() / 50.0, 1.0) * 30.0
    spike_activity_score = min(brain.getTotalSpikeCount() / 1000.0, 1.0) * 20.0
    neuromodulation_score = agent.getNeuromodulationLevel() * 25.0
    development_score = float(agent.getDevelopmentalStage()) / 5.0 * 15.0
    
    return firing_rate_score + spike_activity_score + neuromodulation_score + development_score

# Run brain monitoring
results = monitor_brain_diagnostics(brain, agent, world, 500)
print(f"\n=== Monitoring Complete ===")
print(f"Final health score: {results['overall_health']:.1f}/10.0")
```

---

## Quick Reference (Updated)

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
| Get brain info | `brain.getBrainInfo()` |
| Get agent info | `agent.getAgentInfo()` |
| Load JSON config | `config.loadFromJSON(filepath)` |
| Save JSON config | `config.saveToJSON(filepath)` |
| Export to JSON | `config.toJSON()` |
| Apply JSON patch | `config.applyJSONPatch(json)` |

---

## Troubleshooting (Enhanced)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if `getBrainInfo()` is returning expected results

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify `agent.decodeMotorCommand()` is returning valid actions

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check `brain.getBrainInfo()` and `agent.getAgentInfo()` for detailed status

**"JSON configuration issues"**
- Make sure the JSON file is properly formatted
- Check that `loadFromModern()` is being called (tries JSON first)
- Use `config.getConfigSchema()` to verify JSON structure

---

## Next Steps

When you're comfortable:
1. **Read `HOW_TO_USE.md` for more details** (Phase 4: emerging cognition)
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**
4. **Try the enhanced examples in this updated documentation**
5. **Learn to use JSON configuration for persistent settings**
6. **Master brain and agent diagnostics for debugging**

That's it! You're now ready to use **enhanced NLM** with **modern JSON configuration** and **comprehensive diagnostics**!
