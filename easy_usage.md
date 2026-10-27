# NLM - Enhanced Usage Guide for Beginners

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

### Example 2: Enhanced Brain with Analysis (NEW)

```python
import pynlm

# Create enhanced configuration
config = pynlm.createPhase6Config()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with analysis
for step in range(100):
    brain.step(step)

# Get comprehensive statistics
stats = pynlm.analyzeBrain(brain)
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Total spikes: {stats['total_spikes']}")
```

---

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

### Key Improvements:
1. **Thread Safety** - Brain operations can run concurrently without crashes
2. **Performance Optimizations** - Faster spike processing with circular buffers
3. **Error Handling** - Better validation and safe fallback values
4. **Advanced Python API** - More features, better documentation, type safety
5. **Memory Management** - Fixed potential memory leaks and crashes

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (now with thread safety!)
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world (now with advanced features!)

---

## Common Patterns

### Pattern 1: Run a Simulation with Analysis

```python
def run_simulation_with_analysis(brain, world, agent, num_steps):
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
    
    # Get analysis
    stats = pynlm.analyzeBrain(brain)
    return stats
```

### Pattern 2: Get Comprehensive Brain Stats (ENHANCED)

```python
# Use the new analyzeBrain function:
stats = pynlm.analyzeBrain(brain)

print("Neurons firing:", stats['firing_neurons'])
print("Total spikes:", stats['total_spikes'])
print("Avg firing rate:", stats['avg_firing_rate'])
print("E/I ratio:", stats['e_i_ratio'])
```

### Pattern 3: Enable Advanced Brain Features (NEW)

```python
# These are all optional and now more powerful:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses. **NEW: Now includes thread safety and validation!**

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. **ENHANCED: Now thread-safe!**

### `pynlm.createPhase6Config()` (NEW)
Creates an optimized configuration for full integration.

### `pynlm.analyzeBrain(brain)` (NEW)
Returns comprehensive brain statistics.

---

## Mini Projects

### Project 1: Silent Brain

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

# Setup with enhanced configuration
brain = pynlm.createBrain(pynlm.createPhase6Config())
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

# Get analysis
stats = pynlm.analyzeBrain(brain)
print(f"Firing rate: {stats['avg_firing_rate']:.2f} Hz")
```

### Project 3: Complete Agent with Learning

```python
import pynlm

# Setup with high plasticity
config = pynlm.createHighPlasticityConfig()
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

# Get final analysis
stats = pynlm.analyzeBrain(brain)
print(f"Learning complete! Final firing rate: {stats['avg_firing_rate']:.2f} Hz")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create enhanced config | `pynlm.createPhase6Config()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `pynlm.analyzeBrain(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Use `pynlm.createPhase6Config()` for better performance
- Use `pynlm.analyzeBrain(brain)` to check brain state

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Try enabling curiosity with `agent.enableCuriosity(True)`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Use `pynlm.createPhase6Config()` for better initial conditions

**"Configuration errors"**
- Use `pynlm.createPhase6Config()` for tested configurations

---

## Next Steps

When you're comfortable:
1. Read `docs/ARCHITECTURE.md` to understand how it all works
2. Read `docs/PHASE6_FINAL_AUDIT.md` to see Phase 6 integration details
3. Use the new convenience functions for different use cases
4. Use `pynlm.analyzeBrain(brain)` for comprehensive brain analysis
5. Create your own simulation loops with `pynlm.createSimulation()`

## New Convenience Functions

| Function | Purpose |
|----------|---------|
| `pynlm.createPhase6Config()` | Create optimized Phase 6 configuration |
| `pynlm.createMinimalConfig()` | Create lightweight test configuration |
| `pynlm.createHighPlasticityConfig()` | Create configuration for rapid learning |
| `pynlm.createQuickTestConfig()` | Create fast test configuration |
| `pynlm.analyzeBrain(brain)` | Get comprehensive brain statistics |
| `pynlm.createSimulation()` | Run complete simulation loop |

---

## Enhancement Summary

This version includes significant improvements:

- ✅ Thread-safe concurrent operations
- ✅ Performance optimizations with circular buffers
- ✅ Memory management fixes
- ✅ Comprehensive error handling and validation
- ✅ Advanced Python API with type safety
- ✅ Enhanced documentation and examples
- ✅ Configuration templates

The same basic functionality is preserved, but now with significant improvements in reliability, performance, and features!

That's it! You're now ready to use NLM with all the new improvements and features.