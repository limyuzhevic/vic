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

---
## Enhanced Command-Line Options

NLM now supports a comprehensive command-line interface with many options for power users:

### Basic Options:
- `--help` - Show help message
- `--version` - Show version information
- `--dry_run` - Validate configuration only, don't run simulation

### Configuration Options:
- `--config <file>` - Specify configuration file (default: configs/default.cfg)
- `--steps <n>` - Number of simulation steps (default: 100)
- `--mode <mode>` - Operation mode: demo, test, experiment, benchmark (default: demo)

### Advanced Configuration:
- `--neuron_count <n>` - Total number of neurons (default: 500)
- `--region_count <n>` - Number of brain regions (default: 1)
- `--connection_probability <p>` - Connection probability (default: 0.15)
- `--simulation_timestep <t>` - Simulation timestep in seconds (default: 0.001)
- `--random_seed <seed>` - Random seed for reproducible results (default: 42)

### Plasticity Control:
- `--enable_stdp` - Enable Spike-Timing-Dependent Plasticity (default: enabled)
- `--disable_stdp` - Disable STDP
- `--enable_hebbian` - Enable Hebbian plasticity (default: enabled)
- `--disable_hebbian` - Disable Hebbian
- `--enable_structural` - Enable structural plasticity (default: enabled)
- `--disable_structural` - Disable structural plasticity

### Experiment Control:
- `--run_connectivity_test` - Run neural connectivity test (default: disabled)
- `--run_plasticity_test` - Run plasticity learning experiment (default: disabled)
- `--run_stdp_test` - Run STDP verification test (default: disabled)
- `--record_experiment_data` - Record experiment data to file (default: disabled)

### Output and Logging:
- `--log_level <level>` - Log level: DEBUG, INFO, WARNING, ERROR (default: INFO)
- `--log_file <file>` - Log output to file
- `--verbose` - Enable verbose output (default: false)
- `--quiet` - Suppress informational messages (default: false)

### Advanced Options:
- `--checkpoint <file>` - Save brain state to checkpoint file
- `--load_checkpoint <file>` - Load brain state from checkpoint
- `--profile` - Enable performance profiling
- `--description <desc>` - Set experiment description

### Examples:

#### Using Advanced CLI Options

```bash
# Run with custom configuration and multiple tests
nlm --config my_config.cfg --mode demo --steps 1000 \
    --enable_stdp --enable_hebbian --enable_structural \
    --run_connectivity_test --run_plasticity_test --run_stdp_test \
    --verbose --log_level DEBUG

# Run with checkpoint save/load for reproducible experiments
nlm --load_checkpoint brain_state.bin --enable_hebbian --enable_structural \
    --neuron_count 2000 --connection_probability 0.2 --random_seed 12345

# Run experiment with recording
nlm --mode experiment --steps 5000 --record_experiment_data \
    --description "spatial_learning_task" --log_file experiment.log

# Dry run to validate configuration
nlm --dry_run --verbose --config debug_config.cfg
```

## Advanced Features for Power Users

### 1. Configuration File Examples

Create your own configuration file:

```ini
# my_config.cfg
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.15
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
enable_curiosity = true
enable_reward_modulation = true
enable_structural_plasticity = true
```

### 2. Running Multiple Experiments

```python
import pynlm
import time
import os

def run_experiment(config_file, experiment_name, steps=1000):
    """Run a single experiment with logging."""
    start_time = time.time()
    
    # Create brain with specific configuration
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable subsystems based on experiment needs
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Starting {experiment_name} (steps: {steps})")
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: Firing neurons {brain.getFiringNeuronCount()}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}")
    
    end_time = time.time()
    print(f"Experiment {experiment_name} completed in {end_time - start_time:.2f} seconds")
    
    # Save brain state
    os.makedirs("checkpoints", exist_ok=True)
    brain.save(f"checkpoints/{experiment_name}_final.bin")
    
    return brain

# Run multiple experiments
experiments = [
    ("configs/base_config.cfg", "base_experiment"),
    ("configs/learning_config.cfg", "learning_experiment"),
    ("configs/exploration_config.cfg", "exploration_experiment"),
]

for config_file, exp_name in experiments:
    brain = run_experiment(config_file, exp_name, 2000)
```

### 3. Performance Profiling

```python
import pynlm
import time

def profile_simulation(steps=1000):
    """Profile simulation performance."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm-up
    for i in range(10):
        brain.step(i)
    
    # Profile main simulation
    start_time = time.time()
    for i in range(steps):
        brain.step(i)
    end_time = time.time()
    
    total_time = end_time - start_time
    avg_time_per_step = total_time / steps
    
    print(f"Simulation performance:")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Average time per step: {avg_time_per_step * 1000:.2f} ms")
    print(f"  Steps per second: {steps / total_time:.0f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

# Run profiling
brain = profile_simulation(1000)
```

### 4. Batch Processing

```python
import pynlm
import os

def process_batch(config_files, output_dir="results"):
    """Process multiple configuration files in batch."""
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    results = []
    
    for i, config_file in enumerate(config_files):
        print(f"Processing configuration {i+1}/{len(config_files)}: {config_file}")
        
        # Load configuration
        config = pynlm.createDefaultConfig()
        if config.loadFromFile(config_file):
            print(f"  Loaded configuration with {len(config.getKeys())} settings")
        else:
            print(f"  Failed to load configuration, using defaults")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Collect results
        result = {
            'config_file': config_file,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage(),
        }
        
        results.append(result)
        
        # Save brain state
        output_file = os.path.join(output_dir, f"brain_{i:03d}.bin")
        brain.save(output_file)
        print(f"  Saved brain state to {output_file}")
    
    # Save results summary
    summary_file = os.path.join(output_dir, "results_summary.txt")
    with open(summary_file, 'w') as f:
        f.write("NLM Batch Processing Results\n")
        f.write("=" * 40 + "\n\n")
        
        for i, result in enumerate(results):
            f.write(f"Experiment {i+1}: {result['config_file']}\n")
            f.write(f"  Total Neurons: {result['total_neurons']}\n")
            f.write(f"  Total Spikes: {result['total_spikes']}\n")
            f.write(f"  Firing Neurons: {result['firing_neurons']}\n")
            f.write(f"  Average Firing Rate: {result['avg_firing_rate']:.2f} Hz\n")
            f.write(f"  Excitation/Inhibition Ratio: {result['exc_inh_ratio']:.3f}\n")
            f.write(f"  Developmental Stage: {result['developmental_stage']}\n\n")
    
    print(f"Batch processing complete. Results saved to {summary_file}")
    return results

# Run batch processing
config_files = [
    "configs/config1.cfg",
    "configs/config2.cfg",
    "configs/config3.cfg",
]

results = process_batch(config_files)
```

## Command Line Quick Reference

| Command | Description |
|---------|-------------|
| `--help` | Show help message |
| `--version` | Show version information |
| `--config <file>` | Specify configuration file |
| `--steps <n>` | Number of simulation steps |
| `--mode <mode>` | Operation mode (demo, test, experiment, benchmark) |
| `--enable_stdp` | Enable STDP plasticity |
| `--disable_stdp` | Disable STDP |
| `--enable_hebbian` | Enable Hebbian plasticity |
| `--disable_hebbian` | Disable Hebbian |
| `--enable_structural` | Enable structural plasticity |
| `--disable_structural` | Disable structural plasticity |
| `--run_connectivity_test` | Run neural connectivity tests |
| `--run_plasticity_test` | Run plasticity experiments |
| `--run_stdp_test` | Run STDP verification tests |
| `--log_level <level>` | Set log level |
| `--log_file <file>` | Log to file |
| `--verbose` | Enable verbose output |
| `--quiet` | Suppress informational messages |
| `--checkpoint <file>` | Save brain state |
| `--load_checkpoint <file>` | Load brain state |
| `--profile` | Enable performance profiling |

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
| Enable reward learning | `agent.enableRewardModulation(True)` |
| Enable curiosity | `agent.enableCuriosity(True)` |
| Enable structural growth | `agent.enableStructuralPlasticity(True)` |
| Enable development | `agent.enableDevelopment(True)` |

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - Comprehensive usage guide with examples
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into the system architecture
3. **Experiment with different configurations!** - Try various settings to see how they affect brain behavior
4. **Try the advanced CLI options** - Use command-line parameters for more control
5. **Create your own experiments** - Design and run custom simulation experiments
6. **Use the new batch processing features** - Automate multiple experiment runs
7. **Profile your simulations** - Optimize performance for large-scale experiments

That's it! You're now ready to use NLM with both basic and advanced features, including the comprehensive command-line interface!

## Enhanced Command-Line Interface

The NLM command-line interface has been completely redesigned to support:

- **Extensive configuration options** for fine-tuning brain parameters
- **Advanced plasticity control** for experimenting with learning mechanisms
- **Multiple experiment modes** for different types of simulations
- **Comprehensive logging and output** for debugging and analysis
- **Checkpoint management** for reproducible research
- **Performance profiling** for optimization

All these features make NLM much more powerful and user-friendly for both beginners and advanced users!

The enhanced CLI interface provides unprecedented control over NLM simulations, making it suitable for:
- Educational demonstrations
- Research experiments
- Performance testing
- Automated workflows
- Custom application development

These improvements transform NLM from a simple brain simulator into a comprehensive neural simulation platform!

