# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Quick Start (Copy & Paste)

### Simplest Example

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

### Agent in World Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
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

## Getting Started

### Build from Source

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Install Python Bindings

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

## Core Components

### Brain
- **Central neural processing unit** with spiking neurons and synapses
- Implements Leaky Integrate-and-Fire (LIF) neuron dynamics
- Supports multiple neural populations and regions
- **Key Methods:**
  - `brain.initialize()` - Initialize with configuration
  - `brain.step(step_number)` - Run one simulation step
  - `brain.receiveSensoryInput(input)` - Inject sensory data
  - `brain.produceAction()` - Get motor output

### World
- **Environment simulation** for agent interaction
- Provides sensory input and motor output interfaces
- **Key Methods:**
  - `world.configure(width, height, visionWidth, visionHeight)` - Set dimensions
  - `world.update(timestep)` - Update world state
  - `world.applyMotorCommand(cmd, time)` - Apply agent action
  - `world.getSensoryPercept()` - Get agent's sensory input

### AgentBrain
- **Bridge between brain and world** handling transduction and decoding
- Manages neuromodulation, development, and learning systems
- **Key Methods:**
  - `agent.processSensoryInput(percept)` - Process sensory input
  - `agent.decodeMotorCommand()` - Decode brain activity to action
  - `agent.applyRewardModulation(reward, predicted)` - Learn from rewards

## Configuration

NLM uses a flexible configuration system with nested key-value pairs:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Configure parameters
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.learning_rate", 0.001)

# Create brain with configuration
brain = pynlm.createBrain(config)
```

**Common Configuration Parameters:**

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Connection probability |
| `plasticity.stdp.enable` | bool | true | Enable spike-timing dependent plasticity |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Simulation Loop

The NLM brain operates in a continuous closed-loop simulation:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE → MEMORY/PREDICTION
    ↓                                        ↓
MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE
    ↓                                        ↓
PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

**Complete Simulation Template:**

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
```

## Memory Systems

NLM features multiple integrated memory systems:

- **Working Memory**: Transient active information, limited capacity
- **Episodic Memory**: Experience storage and replay
- **Semantic Memory**: Pattern associations and concepts
- **Procedural Memory**: Skills and habits
- **Associative Memory**: Hebbian connections between patterns

## Neuromodulation

Biological neuromodulators implemented:

- **Dopamine**: Reward prediction error, reinforcement learning
- **Curiosity**: Novelty detection, exploration motivation
- **Novelty**: Change detection, attention focusing
- **Prediction Error**: Learning signal for prediction systems

## Development

The brain develops through stages:

- **Initial**: High plasticity, rapid learning
- **Critical Period**: Sensitive window for sensory development
- **Maturation**: Stabilization of connections
- **Adult**: Maintained functionality, limited plasticity

## Performance Features

- **Event-driven spike propagation** for efficiency
- **Memory pools and event queues** for real-time processing
- **SIMD vectorization** for parallel computation
- **Parallel processing** for complex simulations
- **Checkpoint system** for saving/loading state

## Phase Integration

The NLM project evolved through 6 phases:

- **Phase 1**: Core utilities, configuration, neural interfaces
- **Phase 2**: Real neuron dynamics, event-driven spikes, plasticity
- **Phase 3**: World interaction, basic learning
- **Phase 4**: Complex cognition, prediction, planning
- **Phase 5**: Performance optimizations
- **Phase 6**: Final integration of all systems

## Scientific Limitations

NLM is a research prototype, not a biologically accurate replica:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- Limited to simulation with available resources

## Technical Specifications

- **Language**: C++20
- **Architecture**: Spiking neural network
- **Learning**: Multiple plasticity rules (STDP, Hebbian)
- **Memory**: Hierarchical memory systems
- **Output**: Python bindings via pybind11

## Performance Profiling

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

start = time.time()
for i in range(10000):
    brain.step(i)
end = time.time()

print(f"10,000 steps in {end-start:.2f} seconds")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Troubleshooting

### ImportError: No module named 'pynlm'

```bash
pip install --force-reinstall .
```

### Segmentation faults

- Ensure `initialize()` is called before `step()`
- Check for null pointers before use
- Verify world and agent are properly initialized

### Performance issues

- Increase timestep for coarser simulation
- Reduce neuron count for testing
- Disable unused subsystems

## Further Reading

- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed usage guide
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## License

MIT

## Authors

Research project — See docs for scientific background.

## Support

For issues, questions, or feature requests:
1. Check documentation in this directory
2. Review examples in the source code
3. Try the minimal examples provided
4. Consult the Phase documentation for system details
