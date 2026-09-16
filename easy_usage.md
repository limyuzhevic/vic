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

### Example 2: Brain in a Simple World (Advanced)

```python
import pynlm

# Use the new Phase 6 demo helper for complete setup
config, brain, world, agent = pynlm.createPhase6Demo()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation with monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")

print("✓ Advanced simulation completed!")
```

### Example 3: Advanced Features - Complete Workflow

```python
import pynlm

# Create development configuration for advanced learning
config = pynlm.createDevelopmentConfig()

# Initialize brain with advanced features
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure for exploration
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)

# Initialize agent
agent.initialize(world)

# Enable all advanced features for exploration
pynlm.enableAllFeatures(agent)

# Run exploration simulation
print("Starting exploration simulation...")
for step in range(200):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes (includes learning)
    brain.step(step)
    
    # Agent decides action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning
    reward = agent.getNeuromodulationLevel()
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development system
    agent.updateDevelopment(0.1)

print("✓ Exploration simulation complete!")
print(f"  Final development stage: {brain.getDevelopmentalStage()}")
print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
```

---

## Quick Reference for Advanced Users

| What you want | Code |
|--------------|------|
| Create development brain | `config = pynlm.createDevelopmentConfig()` |
| Create exploration brain | `config = pynlm.createExplorationConfig()` |
| Create stable brain | `config = pynlm.createStableConfig()` |
| Use Phase 6 demo setup | `config, brain, world, agent = pynlm.createPhase6Demo()` |
| Run simulation loop | `spikes, firing, energy = pynlm.runSimulationLoop()` |
| Enable all features | `pynlm.enableAllFeatures(agent)` |
| Get system info | `version = pynlm.getVersionInfo()` |

---

## New Features for Advanced Users

### 1. Configuration Presets

```python
# Development configuration (high plasticity, good for learning)
dev_config = pynlm.createDevelopmentConfig()

# Exploration configuration (curiosity-driven, novelty detection)
expl_config = pynlm.createExplorationConfig()

# Stable configuration (consolidated, low plasticity)
stable_config = pynlm.createStableConfig()
```

### 2. Complete Simulation Helper

```python
# The new runSimulationLoop helper
spikes, firing, energy = pynlm.runSimulationLoop(brain, world, agent, 1000)
```

### 3. Feature Validation

```python
# Check that all features are working
assert agent.isRewardModulationEnabled()
assert agent.isStructuralPlasticityEnabled()
assert agent.isDevelopmentEnabled()
assert agent.isCuriosityEnabled()
```

### 4. Development Monitoring

```python
# Monitor development progress
stage = brain.getDevelopmentalStage()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
neuromod = agent.getNeuromodulationLevel()
```

---

## Common Patterns (Enhanced)

### Pattern 1: Complete Agent with Development

```python
def run_development_simulation(config_type="development", steps=500):
    # Choose configuration
    if config_type == "development":
        config = pynlm.createDevelopmentConfig()
    elif config_type == "exploration":
        config = pynlm.createExplorationConfig()
    else:
        config = pynlm.createStableConfig()
    
    # Initialize components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    pynlm.enableAllFeatures(agent)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain, world, agent
```

### Pattern 2: Multi-Environment Testing

```python
def run_multi_environment_tests():
    environments = ["development", "exploration", "stable"]
    results = {}
    
    for env in environments:
        brain, world, agent = run_development_simulation(env, steps=100)
        
        results[env] = {
            'final_spikes': brain.getTotalSpikeCount(),
            'final_stage': brain.getDevelopmentalStage(),
            'final_curiosity': agent.getCuriosityLevel()
        }
    
    return results
```

---

## Troubleshooting (Enhanced)

### "My brain isn't learning properly"
- Check that `enableAllFeatures(agent)` is called
- Try different configurations (`createDevelopmentConfig()` for high learning)
- Monitor development stage progress

### "My simulation runs too slowly"
- Use smaller worlds: `world.configure(width=10, height=10)`
- Use `createStableConfig()` for faster performance
- Run fewer steps initially

### "Development is not progressing"
- Check that development is enabled: `agent.isDevelopmentEnabled()`
- Use developmental configuration for better learning
- Monitor curiosity and novelty levels

---

## Next Steps

When you're comfortable:

1. **Read Phase 6 integration documentation** - `HOW_TO_USE.md` has advanced examples
2. **Explore `createPhase6Demo()`** - Complete Phase 6 brain setup in one line
3. **Try different configuration presets** - Each designed for specific use cases
4. **Implement your own agents** - Use the advanced Python API for custom behavior

**You now have access to advanced features including:**
- Configuration presets for different use cases
- Complete Phase 6 demo setup helpers
- Comprehensive simulation loop wrapper
- Development monitoring tools
- Feature validation utilities

That's it! You're now ready to use advanced NLM features for research, learning, and exploration!

