# Quick Start Guide

This guide will get you up and running with NLM in 5 minutes or less.

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain simulator. It implements a brain-inspired neural system that can learn, remember, and make decisions through interaction with environments.

## Installation

NLM is a C++ project with Python bindings. If you have the full source, you can build it with:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running the Demo

### Option 1: Run the Phase 6 Integration Demo

The easiest way to see NLM in action is to run the Phase 6 demo:

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test that verifies all brain systems work together.

### Option 2: Run Phase 2 Basic Neural Computation

```bash
./nlm
```

This runs a simpler demonstration of the core neural computation capabilities.

## Python Usage

NLM provides Python bindings that make it easy to use the system programmatically.

### Installation

The Python bindings are automatically built with the C++ library. You can import NLM as:

```python
import pynlm
```

### Creating a Simple Brain

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 simulation steps
for step in range(100):
    brain.step(step)

print(f"Brain thought {brain.getTotalSpikeCount()} times")
```

### Complete Agent Example

```python
import pynlm

# Setup the complete system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run the agent for 500 steps
for step in range(500):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What agent sees
    agent.processSensoryInput(percept)  # Brain processes
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print(f"Simulation complete! Final dopamine level: {agent.getNeuromodulationLevel():.3f}")
```

## Command Line Interface

NLM includes a command-line interface for running simulations and managing brains.

### Basic Commands

```bash
# Create and save a brain
nlm create --neuron-count 2000 --save brain.bin

# Run a simulation
nlm simulate 1000 --with-world --with-agent

# Analyze a saved brain
nlm analyze brain.bin -o analysis.txt
```

### Command Line Options

- `--neuron-count`: Number of neurons (default: 1000)
- `--region-count`: Number of brain regions (default: 2)
- `--with-world`: Run with world simulation
- `--with-agent`: Run with agent brain
- `--save`: Save brain state to file
- `--load`: Load brain state from file

## Configuration Files

NLM uses configuration files to control brain parameters.

### Default Configuration

Create a file called `configs/default.cfg`:

```
# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability
connection_probability = 0.1

# Simulation timestep
simulation_timestep = 0.001

# Log level
log_level = INFO
```

### Loading Configurations

Load a configuration file:

```cpp
auto config = std::make_shared<Config>();
config->loadFromFile("configs/default.cfg");

auto brain = std::make_shared<Brain>(config);
```

## Troubleshooting

### Common Issues

**"Brain isn't doing anything"**

If your brain isn't firing any neurons, try:

1. Check that `brain.initialize()` was called
2. Inject current: `brain.injectCurrentToNeurons(NeuronType::Sensory, 10.0f)`
3. Verify configuration: `neuron_count = 500` (not too high)
4. Check neuron thresholds in the code

**"Agent isn't moving"**

If your agent isn't producing actions:

1. Check motor neuron groups
2. Verify `agent.decodeMotorCommand()` is being called
3. Check neural activity with `brain.getFiringNeuronCount()`
4. Ensure world integration is set up properly

### Getting Help

1. Run `./nlm_phase6_demo` for a working example
2. Check the docs/ folder for detailed documentation
3. Review the Python examples above
4. Check the source code comments for implementation details

## Advanced Usage

### Using Different Neuron Types

```python
# The system automatically distributes neurons by type
# You can access them through the agent or by type:
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 2.0)
brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 1.0)
```

### Learning Features

Enable different learning systems:

```python
# Reward-based learning (dopamine)
agent.enableRewardModulation(True)

# Structural plasticity (growing new connections)
agent.enableStructuralPlasticity(True)

# Developmental changes over time
agent.enableDevelopment(True)

# Curiosity-driven exploration
agent.enableCuriosity(True)
```

### Monitoring and Logging

Track brain activity during simulation:

```python
# Get current brain state
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

## Examples Directory

Look for example code in the `examples/` directory (if it exists) or check the demo applications in the `src/experiments/` directory.

## Learning More

- **Phase 6 Demo**: `./nlm_phase6_demo` - Shows all integrated systems working
- **Phase 2 Demo**: `./nlm` - Shows basic neural computation
- **Documentation**: See docs/ for detailed technical documentation
- **Architecture**: See docs/ARCHITECTURE.md for system design
- **Research**: See docs/SCIENCE.md for scientific background

## Conclusion

NLM provides a complete brain simulation system that combines:

- Real spiking neural computation (LIF neurons)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Neural development and structural plasticity
- Prediction systems and cognitive planning

Start with the Phase 6 demo to see all systems working together, then explore the Python API for custom applications!
