# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Status

Phase 6: Final Integration

### Major Achievements

#### Core Architecture
- **Real Spiking Neural Computation**: LIF neuron dynamics with event-driven spike propagation
- **Integrated Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Cognition Systems**: Neural planner, concept formation, attentional selection
- **Development System**: Multi-stage development with structural plasticity

#### Key Features
- **Structural Plasticity**: Synaptogenesis and pruning
- **Learning Mechanisms**: STDP, Hebbian plasticity, developmental learning
- **Experience Integration**: Memory consolidation and replay
- **Adaptive Behavior**: Action selection based on neural activity

## Quick Start Guide

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
 cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

### Running Demos

```bash
# Phase 2: Core neural computation demo
./nlm

# Phase 3: World interaction demo  
./nlm_phase3_demo

# Phase 4: Cognition and planning demo
./nlm_phase4_demo

# Phase 5: Performance and optimization demo
./nlm_phase5_demo

# Phase 6: Full integration demo
./nlm_phase6_demo

# Benchmark neural performance
./nlm_benchmark
```

### Python Bindings

```bash
# Install Python bindings
pip install -e .

# Example Python usage
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation steps
for step in range(1000):
    brain.step(step, step * 0.001)
```

## Configuration

### Configuration File Formats

NLM supports multiple configuration file formats:

#### Simple Key-Value Format (`.cfg`)
```
# Basic neural network configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### JSON Format (`.json`)
```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.15,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001
}
```

### Command Line Arguments

```bash
nlm [OPTIONS] [CONFIG_FILE]

Options:
  --help, -h              Show this help message
  --version               Show version information
  --config=FILE           Load configuration from file
  --key=value             Set configuration key=value (string)
  -key value              Set configuration key=value (string)

Configuration Keys:
  random_seed             Random seed (int64_t)
  simulation_timestep     Simulation timestep in seconds (double)
  neuron_count            Number of neurons (int64_t)
  region_count            Number of brain regions (int64_t)
  connection_probability  Connection probability between neurons (float)
  stdp_ltp_weight         STDP long-term potentiation weight (float)
  stdp_ltd_weight         STDP long-term depression weight (float)
  stdp_tau                STDP time constant (float)
  synaptogenesis_rate     Structural plasticity synaptogenesis rate (float)
  pruning_rate            Structural plasticity pruning rate (float)

Examples:
  nlm --config=myconfig.json
  nlm --neuron_count=1000 --simulation_timestep=0.0001
  nlm -random_seed 12345
```

### Advanced Configuration

NLM supports advanced configuration options for research and experimentation:

#### Large Scale Networks
```
# For research with larger networks
neuron_count = 5000
region_count = 5
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.01
stdp_tau = 50.0
```

#### Fast Learning
```
# For rapid learning experiments
simulation_timestep = 0.01
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.03
stdp_tau = 10.0
synaptogenesis_rate = 0.001
```

#### Stable Networks
```
# For stable, well-balanced networks
simulation_timestep = 0.0001
connection_probability = 0.2
stdp_ltp_weight = 0.005
stdp_ltd_weight = 0.005
stdp_tau = 100.0
synaptogenesis_rate = 0.00001
pruning_rate = 0.000001
```

## Running Experiments

### Basic Experiment

```bash
# Run a standard experiment with default settings
./nlm

# Run with custom configuration
./nlm --config=experiments/config1.json

# Run with modified parameters
./nlm --neuron_count=1000 --simulation_timestep=0.0001 --random_seed=12345
```

### Research Experiments

The project includes several demo executables for different research scenarios:

#### Phase 3: World Interaction
- Studies basic sensory-motor loops
- Demonstrates reward prediction error
- Explores developmental stages

#### Phase 4: Cognitive Development  
- Studies concept formation
- Explores neural planning
- Investigates attention mechanisms

#### Phase 5: Performance Optimization
- Studies computational efficiency
- Explores memory management
- Investigates parallel processing

#### Phase 6: Full Integration
- Demonstrates all integrated systems
- Studies memory consolidation
- Explores replay mechanisms

## Python API

The NLM framework provides Python bindings for easier experimentation and integration:

### Basic Usage

```python
import pynlm

# Create a new brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
for i in range(1000):
    brain.step(i, i * 0.001)

# Log brain status
brain.logStatus()
```

### Advanced Usage

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Create a simple world for agent interaction
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=20, visionHeight=20)

# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run interaction loop
for episode in range(100):
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Produce action
    action = agent_brain.decodeMotorCommand()
    
    # Apply action to world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    agent_brain.applyRewardModulation(result.reward, result.reward)
    
    # Update development
    agent_brain.updateDevelopment(world.getSimulationTime())
```

### Key Python Classes

#### Config
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)
config.loadFromArgs(["--config", "myconfig.json"])
config.saveToFile("output_config.cfg")
```

#### Brain
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(100, 0.1)  # Step with time
brain.reset()          # Reset brain state
brain.save("checkpoint.brain")  # Save brain state
brain.load("checkpoint.brain")  # Load brain state
```

#### SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)
world.reset()
world.update(0.001)
```

#### AgentBrain
```python
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)
action = agent_brain.decodeMotorCommand()
```

## Development

### Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities (Config, Logger, Random, etc.)
│   ├── brain/          # Neural components (Neuron, Synapse, etc.)
│   ├── dynamics/       # Neural dynamics and spike system
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian, etc.)
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators and motivation
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework
│   └── visualization/ # Visualization
│   └── agent/          # Agent-brain interface
│   └── world/          # World simulation
├── python/              # Python bindings
├── configs/             # Default configuration files
├── tests/               # Unit tests
└── docs/                # Documentation
```

### Building for Development

```bash
# Create debug build
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run tests
ctest
```

### Adding New Features

The NLM framework is designed for extensibility. To add new features:

1. **Add new neural components** in the `brain/` directory
2. **Add new plasticity rules** in the `plasticity/` directory  
3. **Add new memory systems** in the `memory/` directory
4. **Add new cognitive mechanisms** in the `cognition/` directory
5. **Add new neuromodulators** in the `neuromodulation/` directory
6. **Add new sensory modalities** in the `sensory/` directory
7. **Add new environments** in the `environment/` directory

### Testing

Run the unit tests to ensure your changes don't break existing functionality:

```bash
# Build and run tests
mkdir build_test
cd build_test
cmake ..
make
ctest -V  # Run with verbose output
```

Individual test suites:
- `test_config.cpp` - Configuration system tests
- `test_brain.cpp` - Brain system tests
- `test_neuron.cpp` - Neuron dynamics tests
- `test_stdp.cpp` - STDP plasticity tests
- `test_synapse.cpp` - Synapse dynamics tests
- `test_clock.cpp` - Simulation clock tests
- `test_random.cpp` - Random number generation tests
- `test_types.cpp` - Core type tests

## Research Applications

### Memory Research
NLM can be used to study:
- Working memory capacity and dynamics
- Episodic memory formation and retrieval
- Associative memory and pattern completion
- Memory consolidation during sleep/rest

### Learning Research
NLM supports investigation of:
- Supervised learning through reward signals
- Unsupervised learning through statistical regularities
- Reinforcement learning through action outcomes
- Developmental learning through experience

### Neuroscience Research
NLM provides a platform for studying:
- Spiking neural dynamics
- Population coding
- Network oscillations and synchronization
- Structural plasticity and rewiring

### Artificial Intelligence Research
NLM enables exploration of:
- Neuromorphic computing
- Self-organizing systems
- Autonomous agents
- Emergent cognition

## Limitations

NLM is a research project with the following limitations:

### Scientific Limitations
- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition

### Computational Limitations
- Limited to available computing resources
- Simplified world models
- Basic sensory and motor systems
- Simplified reward mechanisms

## License

MIT

## Authors

Research project — See docs for scientific background.

## Acknowledgments

This project builds on several research contributions and is part of the broader NLM (Neural Learning Machine) research initiative.

## Support

For technical support, questions, or collaboration opportunities, please refer to the project documentation and issue tracker.
