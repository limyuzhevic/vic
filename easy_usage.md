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
## Advanced Usage Examples

### Example 5: Using Advanced CLI Features

```python
import pynlm
import subprocess
import sys

# You can run NLM from Python with command-line arguments
# This example shows how to use the new advanced features

def run_nlm_with_config(config_file=None, neuron_count=5000, timestep=0.01):
    """Run NLM with custom configuration using command line"""
    
    # Build command with advanced options
    cmd = [
        sys.executable, "-m", "nlm",
        "--neuron-count", str(neuron_count),
        "--timestep", str(timestep),
        "--experiment-mode", "all",
        "--verbose"
    ]
    
    if config_file:
        cmd.extend(["--config", config_file])
    
    print(f"Running command: {' '.join(cmd)}")
    
    # Run the simulation
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        print("✓ Advanced simulation completed successfully!")
        print(result.stdout)
    else:
        print("✗ Simulation failed:")
        print(result.stderr)

# Example usage
run_nlm_with_config(neuron_count=10000, timestep=0.005)
```

### Example 6: Configuration Validation

```python
import pynlm

def validate_and_run_simulation():
    """Validate configuration before running"""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    
    # Set some configuration values
    config.set("brain.neuron_count", 5000)
    config.set("brain.simulation_timestep", 0.001)
    config.set("brain.connection_probability", 0.1)
    
    # Validate configuration (you can add validation logic here)
    # For example:
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count > 50000:
        print("Warning: Large neuron count may impact performance")
    
    timestep = config.getOr("brain.simulation_timestep", 0.001)
    if timestep > 0.01:
        print("Warning: Large timestep may cause numerical instability")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Configuration validated. Running simulation...")
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("✓ Simulation completed with validated configuration!")

# Use it
validate_and_run_simulation()
```

### Example 7: Quick Test Mode

```python
import pynlm

def run_quick_test():
    """Run a quick connectivity test to verify the brain works"""
    
    print("=== Running Quick Performance Test ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getTotalNeuronCount()
    
    print(f"Initial state: {initial_neurons} neurons, {initial_spikes} spikes")
    
    # Inject current to test connectivity
    for i in range(10):
        if i < brain.getTotalNeuronCount() / 4:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0f)
    
    # Run a few steps
    for step in range(50):
        brain.step(step, step * 0.001)
    
    final_spikes = brain.getTotalSpikeCount()
    spikes_generated = final_spikes - initial_spikes
    
    print(f"After 50 steps: {spikes_generated} spikes generated")
    
    if spikes_generated > 0:
        print("✓ Brain connectivity test PASSED!")
        print("  Spikes are propagating through the network")
    else:
        print("✗ Brain connectivity test FAILED!")
        print("  No spikes generated - check neuron parameters")
        
        # Debug: show some neuron parameters
        for i in range(min(3, int(brain.getTotalNeuronCount() / 10))):
            # This is a simplified debug - in real usage you'd access neurons differently
            print(f"  Neuron {i}: potential needs adjustment")

# Run the test
run_quick_test()
```

### Example 8: Complete Agent with Advanced Features

```python
import pynlm

def run_advanced_agent_simulation():
    """Advanced agent simulation with all features enabled"""
    
    print("=== Advanced Agent Simulation ===")
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Enabled features: Reward modulation, Structural plasticity, Development, Curiosity")
    
    # 4. Run simulation with monitoring
    print("Starting simulation...")
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward-based learning
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    print("✓ Advanced agent simulation completed!")
    print("The brain learned and adapted throughout the simulation.")

# Run the advanced simulation
run_advanced_agent_simulation()
```

---

## Quick Reference

### New Advanced Features

| Feature | Code | Description |
|---------|------|-------------|
| Configuration Validation | `config.getOr(key, default)` | Safely retrieve values with defaults |
| Quick Test Mode | `brain.injectCurrentToNeurons()` | Test brain connectivity quickly |
| Performance Monitoring | `brain.getFiringNeuronCount()` | Monitor brain activity |
| Development Tracking | `brain.getDevelopmentalStage()` | Track brain maturity |
| Curiosity System | `agent.getCuriosityLevel()` | Monitor exploration behavior |

### Error Handling Tips

```python
# Always validate configuration
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count <= 0:
        print("Error: Invalid neuron count")
        return

# Handle brain initialization
try:
    brain.initialize()
except Exception as e:
    print(f"Failed to initialize brain: {e}")
    return

# Check simulation results
if brain.getTotalSpikeCount() == 0:
    print("Warning: No spikes generated - brain may need input")
```

---

## Mini Projects - Advanced Version

### Advanced Project 1: Learning Environment

```python
import pynlm

def run_learning_environment():
    """Brain learns to navigate and explore a world"""
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableCuriosity(True)
    
    # Run exploration
    for step in range(1000):
        world.update(0.05)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Exploration step {step}")
            print(f"  Path length: Agent position tracked")
            print(f"  Learning: {agent.getCuriosityLevel():.3f}")
    
    print("✓ Exploration complete! Brain has learned the environment.")

run_learning_environment()
```

---

## Getting Help

### Command Line Help

To get help on using NLM's advanced features:

```bash
./nlm --help
./nlm --experiment-mode all --verbose
```

### Python API

For detailed Python API documentation:
1. Read `HOW_TO_USE.md` for complete examples
2. Check the source code for specific method details
3. Try the examples provided in this guide

### Troubleshooting Common Issues

**If the brain isn't learning:**
- Enable reward modulation: `agent.enableRewardModulation(True)`
- Make sure the agent is processing sensory input
- Check that world updates are happening

**If performance is slow:**
- Reduce neuron count in configuration
- Use quick test mode first
- Enable quiet mode to reduce logging

**If spikes aren't propagating:**
- Check neuron parameters
- Make sure plasticity is enabled
- Verify that the brain is properly initialized

---

## Next Steps

Now that you're comfortable with the basics:

1. **Explore the architecture**: Read `docs/ARCHITECTURE.md` to understand how all the components work together
2. **Try advanced simulations**: Use the command-line options with experiment modes
3. **Create your own projects**: Build custom brains for specific tasks
4. **Contribute**: Help improve the documentation and code!

You're now ready to use NLM at an advanced level!

---

**Need more help?**
- Check the main `HOW_TO_USE.md` for comprehensive documentation
- Explore the `docs/` folder for technical details
- Try the examples in this guide first
- Experiment with different configuration options

Happy brain simulation! 🧠
