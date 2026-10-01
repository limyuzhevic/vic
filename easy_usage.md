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

### Example 1: Simplest Brain with Error Handling

```python
import pynlm

def create_and_run_simple_brain():
    """Create a simple brain with complete error handling."""
    try:
        # Step 1: Make a brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Step 2: Turn it on
        brain.initialize()
        
        # Step 3: Make it think for 100 steps
        for i in range(100):
            brain.step(i)  # One step of thinking
            
        print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
        return True
        
    except Exception as e:
        print(f"Error running brain: {e}")
        import traceback
        traceback.print_exc()
        return False

# Run with error handling
if create_and_run_simple_brain():
    print("Simulation completed successfully!")
else:
    print("Simulation failed!")
```

### Example 2: Brain in a Simple World with Error Handling

```python
import pynlm

def run_complete_simulation():
    """Run a complete brain-world simulation with error handling."""
    try:
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
        return True
        
    except Exception as e:
        print(f"Error during simulation: {e}")
        import traceback
        traceback.print_exc()
        return False

# Run with error handling
if run_complete_simulation():
    print("Agent simulation completed successfully!")
else:
    print("Agent simulation failed!")
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

### Pattern 1: Run a Simulation with Error Handling

```python
def run_simulation_with_error_handling(brain, world, agent, num_steps):
    """Run a simulation with comprehensive error handling."""
    try:
        for step in range(num_steps):
            try:
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
                
                # Log statistics periodically
                if step % 100 == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
                    
            except Exception as e:
                print(f"Error at step {step}: {e}")
                print("Continuing to next step...")
                continue
                
        print(f"✓ Simulation completed {num_steps} steps successfully")
        return True
        
    except Exception as e:
        print(f"❌ Simulation failed: {e}")
        import traceback
        traceback.print_exc()
        return False

# Use it like this:
run_simulation_with_error_handling(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats with Error Handling

```python
def get_brain_stats_safely(brain, step_number):
    """Get brain statistics with error handling."""
    try:
        # Just add these inside your loop:
        print(f"Step {step_number}: Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"Step {step_number}: Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Step {step_number}: Avg firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"Step {step_number}: Development stage: {brain.getDevelopmentalStage()}")
        
        return True
        
    except Exception as e:
        print(f"Error getting brain stats: {e}")
        return False

# Use it like this:
for step in range(1000):
    brain.step(step)
    
    # Just add these inside your loop:
    if not get_brain_stats_safely(brain, step):
        print("Failed to get brain stats - continuing...")
```

### Pattern 3: Enable Brain Features with Error Handling

```python
def enable_brain_features_safely(agent):
    """Enable brain features with error handling."""
    features_enabled = []
    features_failed = []
    
    # These are all optional:
    feature_configs = [
        ("Reward Modulation", lambda: agent.enableRewardModulation(True)),
        ("Structural Plasticity", lambda: agent.enableStructuralPlasticity(True)),
        ("Development", lambda: agent.enableDevelopment(True)),
        ("Curiosity", lambda: agent.enableCuriosity(True)),
    ]
    
    for feature_name, enable_func in feature_configs:
        try:
            enable_func()
            features_enabled.append(feature_name)
            print(f"✓ {feature_name} enabled successfully")
        except Exception as e:
            features_failed.append((feature_name, e))
            print(f"❌ Failed to enable {feature_name}: {e}")
    
    print(f"\nFeatures enabled: {len(features_enabled)}")
    for feature in features_enabled:
        print(f"  ✓ {feature}")
        
    if features_failed:
        print(f"\nFeatures failed: {len(features_failed)}")
        for feature, error in features_failed:
            print(f"  ✗ {feature}: {error}")
    
    return len(features_failed) == 0

# Use it like this:
if enable_brain_features_safely(agent):
    print("All features enabled successfully!")
else:
    print("Some features failed to enable - check logs above")
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

### Command Line Usage

When running the NLM executable from the command line, you can customize the brain configuration:

```bash
# Run with default configuration
./nlm

# Load custom configuration file
./nlm --config=my_config.cfg

# Override configuration parameters
./nlm --neuron_count=2000 --simulation_timestep=0.0001

# Show help information
./nlm --help
```

The command line supports:

| Option | Description |
|--------|-------------|
| `--config=file.cfg` or `--config file.cfg` | Load custom configuration file |
| `--key=value` | Override configuration parameter |
| `-h, --help` | Show help message |

Examples:

```bash
# Quick test with minimal configuration
./nlm --config=test_config.cfg --neuron_count=100

# Run a larger brain with custom settings
./nlm --config=experiment.cfg --neuron_count=5000 --max_simulation_steps=10000
```

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!


That's it! You're now ready to use NLM.
