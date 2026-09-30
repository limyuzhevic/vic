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

## Advanced Usage: Configuration Management

### Configure Your Brain

```python
import pynlm

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Set various parameters
config.set("brain.neuron_count", 2000)  # More neurons
config.set("brain.synapse_density", 0.15)  # Higher connectivity
config.set("plasticity.stdp.learning_rate", 0.005)  # Slower learning
config.set("neuromod.dopamine.scale", 1.5)  # Stronger dopamine

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Use Dictionary Configuration

```python
import pynlm

# Configure using Python dictionary
brain_config = {
    "brain.neuron_count": 3000,
    "brain.synapse_density": 0.12,
    "plasticity.stdp.learning_rate": 0.003,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True
}

brain = pynlm.createBrain(pynlm.createDefaultConfig())
for key, value in brain_config.items():
    # Try to set as int, float, or string
    try:
        if isinstance(value, bool):
            brain.getConfig().set(key, value)
        elif isinstance(value, int):
            brain.getConfig().set(key, value)
        elif isinstance(value, float):
            brain.getConfig().set(key, value)
        else:
            brain.getConfig().set(key, value)
    except:
        pass  # Skip if key doesn't exist

brain.initialize()
```

### Validate Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Check if configuration is valid
errors = pynlm.validate_config(config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid!")
```

---

## Configuration Examples

### Basic Configuration

```python
import pynlm

# Start with default
config = pynlm.createDefaultConfig()

# Modify specific settings
config.set("brain.v_thresh", -45.0)  # Higher threshold
config.set("brain.v_rest", -65.0)    # More negative resting potential
config.set("brain.tau_mem", 25.0)    # Slower membrane time constant

print(config.summary())
```

### Population Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Configure different neuron populations
pynlm.configure_neuron_population(config, "sensory_layer", 500)
pynlm.configure_neuron_population(config, "motor_layer", 300)
pynlm.configure_neuron_population(config, "interneuron_layer", 200)

# Configure plasticity
pynlm.configure_plasticity(config, 0.001)

# Configure development
pynlm.configure_development(config, "critical_period")

print("Configured for critical period development")
print(config.summary())
```

### Export Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Export for experiment
export_config = pynlm.export_config_for_experiment(config, "neural_plasticity_test")
print("Exported config for experiment:")
for key, value in export_config.items():
    print(f"  {key}: {value}")
```
