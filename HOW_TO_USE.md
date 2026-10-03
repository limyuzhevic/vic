# NLM - How To Use

**Complete User Guide for Neural Learning Machine (NLM)**

---

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. It creates virtual brains that can learn, remember things, and make decisions through interaction with an environment.

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on experience
- **Memory** = The brain remembering patterns and experiences

NLM simulates all of this and has reached **Phase 6: Final Integration** where all brain systems work together as a coherent artificial brain.

---

## Quick Start

### Prerequisites

1. **Build the project:**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Install Python bindings:**
   ```bash
   cd /path/to/nlm
   pip install -e .
   ```

3. **Run the Phase 6 integration demo:**
   ```bash
   cd build
   ./nlm_phase6_demo
   ```

### Python Usage

The easiest way to start is with the Python CLI:

```bash
# Show help
./nlm_cli.py --help

# Create and initialize a simple brain
./nlm_cli.py create-brain

# Create complete setup (brain + world + agent)
./nlm_cli.py create-setup

# Run simulation for 1000 steps
./nlm_cli.py simulate 1000

# Show current status
./nlm_cli.py show-status
```

---

## The Three Components

NLM works with three main components that interact:

### 1. Brain (The virtual brain that thinks)
- Contains neurons and synapses
- Processes sensory input
- Makes decisions (motor output)
- Learns from experience

### 2. World (The environment the brain lives in)
- 2D grid world with boundaries
- Contains resources (positive reward) and hazards (negative reward)
- Provides sensory input to the agent
- Executes the brain's actions

### 3. Agent (The bridge connecting brain to world)
- Translates world observations into brain input
- Translates brain activity into world actions
- Applies reward modulation based on experience
- Manages development and curiosity

---

## Basic Operations

### Create a Brain

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain (required before use)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Create Complete Setup

```python
import pynlm

# Setup configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)          # Explore new things
agent.enableDevelopment(True)        # Brain matures over time
```

### Run a Simulation

```python
import pynlm
import time

# Setup (see above)
# ...

# Run simulation
start_time = time.time()
for step in range(1000):
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
    
    # 7. Learn from result
    action_result = world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(action_result.reward, 0.0)

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Brain learned from {brain.getTotalSpikeCount()} spikes")
```

### Monitor Brain Status

```python
import pynlm

# After creating brain and agent
print("=== Brain Status ===")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

if agent:
    print("\n=== Agent Status ===")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Prediction error: {agent.getPredictionError():.3f}")
```

---

## Command Line Interface

The NLM comes with a comprehensive Python CLI (`nlm_cli.py`) for easy command-line usage.

### Available Commands

#### `create-brain [config_file]`
Create and initialize a brain.

```bash
# Create brain with default config
./nlm_cli.py create-brain

# Create brain with custom config
./nlm_cli.py create-brain my_config.cfg
```

#### `create-setup`
Create complete brain+world+agent setup with learning features enabled.

```bash
# Basic setup
./nlm_cli.py create-setup

# Custom setup with 500 neurons
./nlm_cli.py create-setup --neurons 500
```

#### `simulate STEPS [options]`
Run simulation for specified number of steps.

```bash
# Basic simulation
./nlm_cli.py simulate 1000

# Save checkpoints every 500 steps
./nlm_cli.py simulate 5000 --checkpoint simulation.ckpt
```

#### `test TYPE [STEPS]`
Run specific brain tests.

```bash
# Test basic neural connectivity
./nlm_cli.py test connectivity 1000

# Test synaptic plasticity (learning)
./nlm_cli.py test plasticity 2000

# Test STDP (Spike-Timing Dependent Plasticity)
./nlm_cli.py test stdp 1000
```

#### `show-status`
Display current brain, world, and agent status.

```bash
./nlm_cli.py show-status
```

#### `save-config FILEPATH`
Save current configuration to JSON file.

```bash
./nlm_cli.py save-config brain_config.json
```

---

## Working with the Environment

### Understanding World Objects

The world contains two types of objects:

- **Resources** (green): Give positive reward when collected
- **Hazards** (red): Give negative reward when hit

### Common Actions

The brain can perform these actions:

| Action | Description | Reward |
|--------|-------------|---------|
| MoveForward | Move forward in current direction | -0.01 |
| TurnLeft | Turn 45° left | -0.005 |
| TurnRight | Turn 45° right | -0.005 |
| Interact | Try to interact with nearby object | 0.0 |
| Wait | Do nothing, decay velocities | 0.0 |

### Sensory Input

The agent receives four types of sensory input:

1. **Vision** (256 values): 16x16 grid showing visible objects
2. **Touch** (8 values): Proximity to obstacles in 8 directions
3. **Internal** (4 values): Energy, health, movement, turning
4. **Proprioception** (6 values): Position, orientation, velocity

---

## Learning and Development

### Neuromodulation

The brain uses neuromodulators to control learning:

- **Dopamine**: Based on prediction error (reward surprise)
- **Curiosity**: Based on novelty and prediction error  
- **Novelty**: Based on changes in sensory input

### Developmental Stages

The brain goes through these developmental stages:

1. **Initial** (0-60s): High plasticity, rapid learning
2. **Critical Period** (60-300s): Moderate plasticity, fine-tuning
3. **Maturation** (300-900s): Low plasticity, stable
4. **Adult** (>900s): Minimal plasticity, mostly stable
5. **Aging** (>1800s): Declining performance

### Curiosity-Driven Exploration

When curiosity is enabled, the agent occasionally takes random actions to explore new experiences, even if they don't seem rewarding immediately.

---

## Configuration Files

### Default Configuration

The default configuration (`configs/default.cfg`) includes:

```ini
# Core settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Plasticity
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Development
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation
dopamine_baseline = 0.1

# Environment
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

### Custom Configuration

Create your own configuration file:

```ini
# My custom brain configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.2

# Strong learning
plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.03

# Environment settings
environment_width = 20
environment_height = 20
environment_name = ComplexWorld

# Logging
log_level = DEBUG
log_to_file = true
log_filename = my_brain.log
```

Load it with:

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.getConfig().loadFromFile("my_config.cfg")
```

---

## Examples

### Example 1: Silent Brain

Just neurons doing their thing:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Brain Watching a World

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

# Watch the world
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print(f"Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
```

### Example 3: Complete Learning Agent

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

### Example 4: Stress Test Performance

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)  # Large brain

brain = pynlm.createBrain(config)
brain.initialize()

print("Running performance stress test...")
start_time = time.time()

# Run many steps
for step in range(10000):
    # Minimal input to test processing speed
    brain.step(step)

end_time = time.time()
elapsed = end_time - start_time

print(f"Processed 10000 steps in {elapsed:.2f} seconds")
print(f"Performance: {10000 / elapsed:.0f} steps/second")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
```

---

## Troubleshooting

### "My brain isn't doing anything"

- Did you call `brain.initialize()`?
- Try running more steps (brains need time to "warm up")
- Make sure sensory input is connected to the brain

### "The agent isn't moving"

- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure the brain is producing motor commands

### "Everything is 0"

- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that `agent.processSensoryInput()` is being called

### "Learning isn't happening"

- Make sure `agent.enableRewardModulation(True)` is called
- Check that `agent.applyRewardModulation()` is being called with rewards
- Verify that `agent.enableDevelopment(True)` is set for developmental learning

### "Performance is slow"

- Reduce the number of neurons for testing
- Use smaller simulation timesteps
- Check for infinite loops in custom code

---

## Advanced Features

### Checkpointing and Saving

```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("my_brain.bin")
```

### Visualization

The NLM includes visualization tools for viewing brain activity and world state. Run the visualization demo:

```bash
# Build with visualization support
cmake .. -DVISUALIZATION_ENABLED=ON
make -j4

# Run visualization
./visualization_demo
```

### Experiment Management

Use the Phase 6 experiment runner for comprehensive testing:

```python
from src.experiments.Phase6IntegratedExperiment import Phase6IntegratedExperiment, Phase6Config

experiment = Phase6IntegratedExperiment()
config = Phase6Config()
config.neuronCount = 1000
config.maxSteps = 5000
config.enableCheckpointing = True

result = experiment.run(config)
print(f"Integration result: {result.totalReward} total reward")
```

---

## Getting Help

### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **Repository**: https://github.com/nlm-project/nlm
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Local Documentation

- **README.md**: Project overview and building instructions
- **docs/ARCHITECTURE.md**: System architecture
- **docs/SCIENCE.md**: Scientific background
- **docs/ROADMAP.md**: Development roadmap

### Examples and Tutorials

- **Easy Usage Guide**: `easy_usage.md` - Beginner-friendly examples
- **HOW_TO_USE.md**: Complete user guide (this file)
- **Phase-specific demos**: Run `./nlm_phaseX_demo` for each phase

---

## Contributing

### Report Issues

- File issues on GitHub with clear reproduction steps
- Include system information (OS, NLM version, configuration)
- Provide minimal reproduction code

### Contribute Code

1. Fork the repository
2. Create a feature branch
3. Follow the existing code style and conventions
4. Add tests for new functionality
5. Update documentation

### Development Guidelines

- Use meaningful variable names and clear comments
- Follow existing C++ and Python coding styles
- Add unit tests for new functionality
- Keep changes minimal and focused
- Write clear commit messages

---

## License

MIT License. See `LICENSE` file for details.

---

## Contact

For questions, support, or collaboration:

- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Project Website: https://nlm.readthedocs.io
- Email: contact@nlm-project.org

---

*Last updated: October 2026*
*NLM Phase 6: Final Integration Complete*