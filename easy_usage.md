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

### Example 3: Advanced Usage with Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Configure using command-line style options
config.set("neuron_count", 2000)           # More neurons
config.set("region_count", 3)              # More regions  
config.set("random_seed", 123)             # Reproducible results
config.set("simulation_timestep", 0.0005)  # Higher resolution
config.set("connection_probability", 0.15) # More connections

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print("Advanced brain created:", brain.getTotalNeuronCount(), "neurons")
```

---

## Command Line Interface (C++)

For advanced users, NLM provides command-line tools to configure and run experiments without modifying code:

### Phase 4 Demo Command Line

```bash
# Basic usage - run with 100 trials
./nlm_phase4_demo 100

# Advanced usage with configuration options
./nlm_phase4_demo \
    --trials 200 \
    --neurons 1500 \
    --regions 4 \
    --seed 123 \
    --verbose \
    --config my_settings.cfg
```

### Available Options

| Option | Description | Default |
|--------|-------------|---------|
| `--trials N` | Number of trials for experiments | 50 |
| `--neurons N` | Number of neurons in the brain | 1000 |
| `--regions N` | Number of brain regions | 2 |
| `--seed N` | Random seed for reproducibility | 42 |
| `--verbose` | Enable verbose logging | false |
| `--help` | Print help message | - |
| `--version` | Print version information | - |

### Configuration File

Create a `settings.cfg` file for custom configurations:

```
# NLM Configuration File
neuron_count = 2000
region_count = 3
random_seed = 123
simulation_timestep = 0.001
connection_probability = 0.15
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Enable/disable features
enable_reward_modulation = true
enable_structural_plasticity = true
enable_development = true
enable_curiosity = true
```

Then run with configuration:

```bash
./nlm_phase4_demo --config settings.cfg
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

### Project 4: Experiment Runner (Command Line)

Save this as `experiment_runner.py` and run with:

```bash
python experiment_runner.py --trials 50 --neurons 1500 --regions 3 --seed 123 --verbose
```

```python
#!/usr/bin/env python3
import sys
import pynlm
import argparse

def parse_args():
    parser = argparse.ArgumentParser(description="Run NLM Phase 4 experiments")
    parser.add_argument("--trials", type=int, default=50, help="Number of trials")
    parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser.add_argument("--regions", type=int, default=2, help="Number of brain regions")
    parser.add_argument("--seed", type=int, default=42, help="Random seed")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose logging")
    return parser.parse_args()

def main():
    args = parse_args()
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", args.neurons)
    config.set("region_count", args.regions)
    config.set("random_seed", args.seed)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run experiments (simplified)
    print(f"Running NLM Phase 4 with {args.trials} trials...")
    print(f"Brain: {args.neurons} neurons, {args.regions} regions")
    print(f"Seed: {args.seed}, Verbose: {args.verbose}")
    
    # Run a simple brain loop
    for step in range(args.trials):
        brain.step(step)
        if args.verbose and step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Experiment complete! Total spikes: {brain.getTotalSpikeCount()}")

if __name__ == "__main__":
    main()
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

1. **Explore C++ Command Line Interface**: Use `./nlm_phase4_demo --help` for options
2. **Read `HOW_TO_USE.md`** for more details
3. **Read `docs/ARCHITECTURE.md`** to understand how it all works
4. **Experiment with different configurations** - try changing neuron counts, connection probabilities, etc.
5. **Run the Phase 6 integration demo**: `./nlm_phase6_demo` to see all systems working together

That's it! You're now ready to use NLM in both Python and C++!

(End of file - total 487 lines)