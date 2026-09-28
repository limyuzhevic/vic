## Configuration Deep Dive

### Common Configuration Parameters

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `neuron_count` | int64 | 1000 | Number of neurons in the brain | 100-100,000 |
| `region_count` | int64 | 1 | Number of brain regions | 1-100 |
| `connection_probability` | float | 0.1 | Probability of synapse formation | 0.05-0.3 |
| `simulation_timestep` | double | 0.001 | Time step duration in seconds | 0.0001-0.01 |

### STDP Parameters

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `stdp_ltp_weight` | float | 0.01 | Long-term potentiation strength | 0.005-0.05 |
| `stdp_ltd_weight` | float | 0.012 | Long-term depression strength | 0.005-0.03 |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) | 10.0-50.0 |

### Structural Plasticity Parameters

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|------------------|
| `synaptogenesis_rate` | float | 0.0001 | Rate of new connection formation | 0.00001-0.001 |
| `pruning_rate` | float | 0.00001 | Rate of connection removal | 0.000001-0.00005 |

## Performance Optimization Tips

### For Fast Testing
```python
# Create small, fast brain for testing
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Small brain
config.set("simulation_timestep", 0.01)  # Larger time step
brain = pynlm.createBrain(config)
```

### For Realistic Simulation
```python
# Create larger, more realistic brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 10000)  # More neurons
config.set("simulation_timestep", 0.001)  # More precise timing
config.set("region_count", 4)  # Multiple regions
brain = pynlm.createBrain(config)
```

### Memory Management
```python
# Clear memory periodically to prevent accumulation
if step % 1000 == 0:
    # Reset brain to save memory (for long simulations)
    brain.reset()
    # Or save brain state and reload when needed
    brain.save("brain_checkpoint.npy")
```

## Troubleshooting Guide

### "My brain isn't doing anything"

**Check:**
- Did you call `brain.initialize()`?
- Are you calling `brain.step()`?

**Fix:**
```python
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this first!

for step in range(100):
    brain.step(step)  # Make it process
```

### "The agent isn't moving"

**Check:**
- Did you call `world.update()`?
- Is the agent decoding motor commands?

**Fix:**
```python
world.update(0.1)  # Update world state first
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

### "Everything is 0"

**Check:**
- Brains need time to "warm up"
- Some neurons need input to fire

**Fix:**
```python
# Run more steps for warm-up
for step in range(1000):  # More steps
    brain.step(step)
    
# Provide sensory input to stimulate neurons
agent.processSensoryInput(some_percept)
```

### "Learning is not working"

**Check:**
- Are reward modulation and curiosity enabled?
- Are the brain's learning parameters set correctly?

**Fix:**
```python
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Or use more specific parameters
brain = pynlm.createBrain(config)
# Ensure config has reasonable STDP parameters
```

## Advanced Features

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("simulation_timestep", 0.0005)
config.set("stdp_ltp_weight", 0.05)
config.set("stdp_ltd_weight", 0.04)
config.set("connection_probability", 0.15)

brain = pynlm.createBrain(config)
```

### Multiple Brains

```python
import pynlm

# Create multiple brains with different configurations
config1 = pynlm.createDefaultConfig()
config1.set("neuron_count", 1000)
config1.set("random_seed", 42)

config2 = pynlm.createDefaultConfig()
config2.set("neuron_count", 2000)
config2.set("random_seed", 123)

brain1 = pynlm.createBrain(config1)
brain2 = pynlm.createBrain(config2)

# Use them in different contexts or experiments
```

### Brain State Management

```python
import pynlm

# Create and save brain state
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate some learning
for step in range(500):
    brain.step(step)

# Save the trained brain
brain.save("trained_brain.npy")

# Later, load the brain for continued learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.load("trained_brain.npy")

# Continue training
for step in range(500):
    brain.step(step)
```

## Quick Reference

### Core Functions

| Function | Description | Example |
|----------|-------------|---------|
| `pynlm.createBrain(config)` | Create a virtual brain | `brain = pynlm.createBrain(config)` |
| `brain.initialize()` | Initialize the brain | `brain.initialize()` |
| `brain.step(step)` | Process one simulation step | `brain.step(step)` |
| `pynlm.createSimpleWorld()` | Create a simple world | `world = pynlm.createSimpleWorld()` |
| `pynlm.createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |

### Brain Statistics

| Function | Returns | When to Use |
|----------|---------|-------------|
| `brain.getFiringNeuronCount()` | Number of firing neurons | Check brain activity |
| `brain.getTotalSpikeCount()` | Total spikes in simulation | Monitor learning |
| `brain.getAverageFiringRate()` | Average firing rate | Evaluate performance |
| `brain.getDevelopmentalStage()` | Current development stage | Track maturation |

### Agent Features

| Function | Description | When to Enable |
|----------|-------------|----------------|
| `enableRewardModulation()` | Learn from rewards | For learning tasks |
| `enableStructuralPlasticity()` | Grow/ prune connections | For development |
| `enableDevelopment()` | Brain maturation | For long-term simulations |
| `enableCuriosity()` | Exploration motivation | For adaptive behavior |

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md`** for more advanced usage patterns
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!**
4. **Join the NLM community** to share your projects and get help
5. **Contribute to the project** - help improve NLM for everyone

That's it! You're now ready to use NLM. Happy experimenting! 🎉

---

*For technical documentation and API reference, see `docs/API.md`*
*For architecture and design, see `docs/ARCHITECTURE.md`*
*For advanced usage patterns, see `docs/ADVANCED.md`*