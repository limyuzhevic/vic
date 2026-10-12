# NLM - Easy Usage Guide for Beginners (Enhanced)

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

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

### Example 2: Brain in a Simple World (NEW!)

```python
import pynlm

# Setup with new convenience functions
config = pynlm.createDemoConfig()  # NEW: Create demo configuration
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createQuickAgent(brain, world)  # NEW: Quick agent setup

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Research Setup (NEW!)

```python
import pynlm

# Create a research configuration with all features enabled
config = pynlm.createResearchConfig()  # NEW: Research configuration
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createQuickAgent(brain, world)  # NEW: Quick agent setup

# Enable learning and development
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent with new helper function
brain, world, agent = pynlm.runQuickSimulation(brain, world, agent, 100)  # NEW: Helper function

print("Agent simulation complete!")
```

### Example 4: Quick Test (NEW!)

```python
import pynlm

# Quick test to verify NLM installation and basic functionality
neurons, spikes, rate = pynlm.quickTest()  # NEW: Quick test
print(f"Installation verified: {neurons} neurons, {spikes} spikes, rate: {rate}")
```

---

## Advanced Usage (NEW!)

### Working with JSON/YAML Configurations

```python
import pynlm

# Create brain from JSON configuration file
brain = pynlm.createBrainFromJSON("my_config.json")  # NEW

# Create brain from YAML configuration file
brain = pynlm.createBrainFromYAML("my_config.yaml")  # NEW

# Create brain from Python dictionary
config_dict = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "curiosity_enabled": True,
    "reward_modulation_enabled": True
}
brain = pynlm.createBrainFromConfigDict(config_dict)  # NEW
```

### Performance Testing (NEW!)

```python
import pynlm

# Benchmark brain performance
neurons, synapses, spikes, duration, rate = pynlm.benchmarkBrain(brain, 1000)  # NEW
print(f"Benchmark: {neurons} neurons, {duration:.2f}s duration, {rate:.2f} firing rate")
```

### Complete Environment Setup (NEW!)

```python
import pynlm

# Create complete environment setup with pre-configured subsystems
brain, world, agent = pynlm.createEnvironmentSimulation()  # NEW

# Now you can run your simulation immediately
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

---

## New Convenience Functions

| Function | Description | Example Usage |
|----------|-------------|---------------|
| `createDemoConfig()` | Create demo configuration for quick testing | `config = pynlm.createDemoConfig()` |
| `createResearchConfig()` | Create research configuration with all features | `config = pynlm.createResearchConfig()` |
| `createQuickAgent(brain, world)` | Create and initialize agent with common subsystems | `agent = pynlm.createQuickAgent(brain, world)` |
| `createSimpleSimulation(brain, world, agent)` | Create simulation function | `sim = pynlm.createSimpleSimulation(brain, world, agent)` |
| `runQuickSimulation(brain, world, agent, steps)` | Run complete simulation with validation | `pynlm.runQuickSimulation(brain, world, agent, 100)` |
| `benchmarkBrain(brain, steps)` | Performance testing | `neurons, synapses, spikes, duration, rate = pynlm.benchmarkBrain(brain, 1000)` |
| `createEnvironmentSimulation()` | Complete environment setup | `brain, world, agent = pynlm.createEnvironmentSimulation()` |
| `quickTest()` | Quick installation and functionality test | `neurons, spikes, rate = pynlm.quickTest()` |

---

## Enhanced Error Handling (NEW!)

The new NLM Python API includes better error handling:

```python
import pynlm

# Invalid inputs now provide clear error messages
try:
    brain = pynlm.createBrain(None)  # Will raise ValueError
except ValueError as e:
    print(f"Error: {e}")

# Invalid steps
brain, world, agent = pynlm.runQuickSimulation(brain, world, agent, -1)  # Will raise ValueError
```

---

## Configuration File Examples

### JSON Configuration

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "curiosity_enabled": true,
    "reward_modulation_enabled": true,
    "structural_plasticity_enabled": false,
    "development_enabled": true,
    "checkpointing_enabled": true,
    "replay_enabled": true,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012
}
```

### YAML Configuration

```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.1
curiosity_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: false
development_enabled: true
checkpointing_enabled: true
replay_enabled: true
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create demo config | `pynlm.createDemoConfig()` |
| Create research config | `pynlm.createResearchConfig()` |
| Create quick agent | `pynlm.createQuickAgent(brain, world)` |
| Create brain from JSON | `pynlm.createBrainFromJSON("file.json")` |
| Create brain from YAML | `pynlm.createBrainFromYAML("file.yaml")` |
| Create brain from dict | `pynlm.createBrainFromConfigDict(dict)` |
| Run quick simulation | `pynlm.runQuickSimulation(brain, world, agent, steps)` |
| Benchmark brain | `pynlm.benchmarkBrain(brain, steps)` |
| Quick test | `pynlm.quickTest()` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting (Enhanced)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if the configuration is valid

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent is properly initialized

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if the configuration has reasonable parameters

**"Configuration file error"**
- Check if the file path is correct
- Verify the file format (JSON/YAML)
- Check for syntax errors in the configuration file

**"Import error: No module named 'pynlm'"**
- Run: `pip install pynlm`
- Make sure the NLM C++ library is built and installed

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - Includes advanced C++ configuration options
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into NLM architecture
3. **Experiment with different configurations!** - Try different brain sizes, connection probabilities, etc.
4. **Test the new functionality** - Run `test_new_features.py` to verify all improvements
5. **Explore the documentation examples** - See how to use JSON/YAML configurations

---

## Testing the New Features

To test the new functionality, run:

```bash
python test_new_features.py
```

This will test all the new convenience functions and error handling.

---

That's it! You're now ready to use the enhanced NLM with all the new convenience features and improved error handling.
