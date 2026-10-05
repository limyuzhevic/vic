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

## The Enhanced Example with All Features

```python
import pynlm

# Create a complete demo system with brain, world, and agent
print("Creating demo system...")
system = pynlm.createDemoSystem()
brain = system["brain"]
world = system["world"]
agent = system["agent"]

# Configure the brain with custom settings
print("Configuring brain...")
pynlm.configure_brain(brain, neuronCount=200, regionCount=2)

# Initialize everything
brain.initialize()
agent.initialize(world)

# Enable all learning features for the agent
pynlm.enable_all_features(agent)

# Run a complete simulation
print("Running simulation with learning...")
pynlm.run_simulation(brain, world, agent, 500)

# Get comprehensive network statistics
print("\nNetwork Statistics:")
stats = pynlm.get_network_stats(brain)
for key, value in stats.items():
    print(f"  {key}: {value}")

# Test brain functionality
print("\nTesting brain functionality:")
test_results = pynlm.test_brain_functionality(brain)
for key, value in test_results.items():
    print(f"  {key}: {value}")

print("\nEnhanced demo complete!")
```

---

## Common Patterns

### Pattern 4: Run a Complete Enhanced Simulation

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run the simulation
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
        
        # 7. Monitor progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"Avg rate: {brain.getAverageFiringRate():.2f}")

# Use it like this:
run_enhanced_simulation(brain, world, agent, 1000)
```

### Pattern 5: Save and Load Brain States

```python
def save_and_restore(brain, filepath):
    # Save brain state (checkpoint)
    print("Saving brain state...")
    if brain.save(filepath):
        print(f"Brain saved to {filepath}")
        
        # Test with a new brain
        new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        new_brain.initialize()
        
        # Load saved state
        print("Loading brain state...")
        if new_brain.load(filepath):
            print("Brain loaded successfully!")
            
            # Verify structure is intact
            print(f"Original brain neurons: {brain.getTotalNeuronCount()}")
            print(f"Loaded brain neurons: {new_brain.getTotalNeuronCount()}")
        else:
            print("Failed to load brain")
    else:
        print("Failed to save brain")

# Use it like this:
save_and_restore(brain, "./my_brain_checkpoint.bin")
```

### Pattern 6: Get Comprehensive Brain Analysis

```python
def analyze_brain(brain):
    # Get comprehensive statistics
    stats = pynlm.get_network_stats(brain)
    
    print("=== Brain Analysis ===")
    for key, value in stats.items():
        if isinstance(value, (int, float)):
            print(f"{key}: {value:.4f}")
        else:
            print(f"{key}: {value}")
    
    # Test basic functionality
    test_results = pynlm.test_brain_functionality(brain)
    
    print("\n=== Functionality Test ===")
    for key, value in test_results.items():
        print(f"{key}: {value}")
    
    return stats, test_results

# Use it like this:
stats, test_results = analyze_brain(brain)
```

### Pattern 7: Experiment with Different Configurations

```python
def experiment_with_configurations():
    configurations = [
        {"neuronCount": 100, "regionCount": 1},
        {"neuronCount": 200, "regionCount": 2},
        {"neuronCount": 500, "regionCount": 3},
    ]
    
    results = []
    
    for i, config in enumerate(configurations):
        print(f"\n=== Running Configuration {i+1} ===")
        
        # Create brain with specific config
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        pynlm.configure_brain(brain, 
                            neuronCount=config["neuronCount"],
                            regionCount=config["regionCount"])
        
        # Initialize and test
        brain.initialize()
        
        # Get network stats
        stats = pynlm.get_network_stats(brain)
        results.append({
            "config": config,
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
            "firing_rate": stats["average_firing_rate"]
        })
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    return results

# Use it like this:
experiment_results = experiment_with_configurations()
```

### Pattern 8: Quick Testing and Validation

```python
def quick_test(brain):
    """Quick test to verify brain functionality"""
    # Initialize if not already done
    if brain.getTotalNeuronCount() == 0:
        brain.initialize()
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    # Check basic functionality
    stats = pynlm.get_network_stats(brain)
    
    # Simple validation
    checks = {
        "has_neurons": brain.getTotalNeuronCount() > 0,
        "has_synapses": brain.getTotalSynapseCount() >= 0,
        "can_produce_action": brain.produceAction() is not None,
        "has_active_spiking": stats["total_spikes"] > 0,
        "network_balanced": 0.1 < stats["excitatory_inhibition_ratio"] < 10.0
    }
    
    all_passed = all(checks.values())
    
    print("Quick Test Results:")
    for check, passed in checks.items():
        status = "✓" if passed else "✗"
        print(f"  {status} {check}")
    
    print(f"\nOverall: {'PASSED' if all_passed else 'FAILED'}")
    return all_passed

# Use it like this:
quick_test(brain)
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

That's it! You're now ready to use NLM.
