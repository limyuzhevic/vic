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

### New: Example 3: Advanced Brain Configuration

```python
import pynlm

# Create a standard configuration (new convenience function)
config = pynlm.createStandardConfig()

# Get brain statistics before initialization
print("Config validation:", pynlm.validateConfig(config))

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Use advanced convenience function for brain stats
stats = pynlm.brainStats(brain)
print(stats)

# Test with minimal configuration for faster startup
min_config = pynlm.createMinimalConfig()
brain2 = pynlm.createBrain(min_config)
brain2.initialize()
```

### New: Example 4: Batch Processing

```python
import pynlm

# Create multiple brains for batch processing (new convenience function)
brains = pynlm.createMultipleBrains(5)

# Initialize and run each brain
for i, brain in enumerate(brains):
    brain.initialize()
    for step in range(50):
        brain.step(step)
    print(f"Brain {i+1} fired {brain.getTotalSpikeCount()} times")

# Save all brains with timestamps
for i, brain in enumerate(brains):
    saved = pynlm.saveBrainWithTimestamp(brain, f"brain_{i+1}")
    print(f"Saved brain {i+1}: {saved}")
```

### New: Example 5: Complete Simulation with Error Handling

```python
import pynlm
import sys

def run_safe_simulation(steps=100):
    """Run a simulation with proper error handling"""
    try:
        # Create configuration
        config = pynlm.createStandardConfig()
        
        # Validate configuration first
        if not pynlm.validateConfig(config):
            errors = pynlm.getConfigErrors(config)
            print("Configuration errors:", errors)
            return False
        
        # Create all components
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 50 == 0:
                print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.3f}")
        
        print("Simulation completed successfully!")
        return True
        
    except pynlm.NLMException as e:
        print(f"NLM Error: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False

# Run the safe simulation
if __name__ == "__main__":
    success = run_safe_simulation(200)
    sys.exit(0 if success else 1)
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

### New: Project 2: Configuration Comparison

```python
import pynlm

# Compare different configurations
configs = [
    ("Minimal", pynlm.createMinimalConfig()),
    ("Standard", pynlm.createStandardConfig()),
    ("High Performance", pynlm.createHighPerformanceConfig())
]

for name, config in configs:
    print(f"\n{name} configuration:")
    print(f"  - Validation: {pynlm.validateConfig(config)}")
    print(f"  - Error count: {len(pynlm.getConfigErrors(config))}")
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
| Get brain stats | `pynlm.brainStats(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Validate config | `pynlm.validateConfig(config)` |
| Get config errors | `pynlm.getConfigErrors(config)` |
| Create multiple brains | `pynlm.createMultipleBrains(count)` |
| Save brain with timestamp | `pynlm.saveBrainWithTimestamp(brain, path)` |
| Create standard config | `pynlm.createStandardConfig()` |
| Create minimal config | `pynlm.createMinimalConfig()` |
| Create high performance config | `pynlm.createHighPerformanceConfig()` |

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

**"Configuration errors"**
- Check validation with `pynlm.validateConfig(config)`
- Use `pynlm.getConfigErrors(config)` for detailed error messages

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM with the enhanced Python bindings.
