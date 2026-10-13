# nlm - Neural Learning Machine

The NLM (Neural Learning Machine) Python package provides a comprehensive interface
to the high-performance C++ neural simulation framework for experimental
artificial developmental brain research.

## Overview

NLM is an experimental computational brain project designed to create neural
systems that develop and learn through interaction with their environment. The
framework implements real spiking neural computation with integrated memory,
prediction, cognition, and neuromodulation systems.

## Quick Start

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)
    
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Installation

```bash
pip install pynlm
```

## Core Components

### 1. Brain Module (`nlm/brain/`)

The brain module contains the main neural simulation engine:

- **Brain**: Core neural computation with LIF neurons
- **Config**: Configuration system for brain parameters
- **NeuralRegion**: Brain regions with neuron populations
- **NeuralPopulation**: Groups of neurons with same type and connectivity

### 2. World Module (`nlm/world/`)

The world module provides the simulated environment:

- **World**: 2D continuous environment with objects and physics
- **WorldObject**: Entities in the world (resources, hazards, markers)
- **SensoryPercept**: Agent sensory input (vision, touch, etc.)
- **ActionResult**: Results of actions in the world

### 3. Agent Module (`nlm/agent/`)

The agent module provides brain-environment interaction:

- **AgentBrain**: Interface connecting brain to world perception and action
- **AgentBody**: Physical agent state and properties
- **MotorCommand**: Low-level motor commands

### 4. Core Utilities (`nlm/`)

Supporting modules for various functionalities:

- **config**: Configuration management and validation
- **exceptions**: Error handling and recovery
- **types**: Type definitions and enumerations
- **utils**: Debug logging, performance monitoring, mathematical utilities

## Key Features

### Neural Computation
- **LIF Neurons**: Leaky Integrate-and-Fire neuron dynamics
- **Event-Driven Processing**: Efficient spike propagation with delays
- **Multiple Plasticity Rules**: STDP, Hebbian, structural plasticity

### Memory Systems
- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience recording and replay
- **Associative Memory**: Pattern association and recall

### Cognitive Architecture
- **Prediction System**: Forward model and error computation
- **Neural Planner**: Action sequence evaluation
- **Concept Formation**: Pattern discovery
- **Attention**: Competitive selection mechanisms

### Neuromodulation
- **Dopamine**: Reward prediction error processing
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Forward model error computation

### Development
- **Developmental Stages**: Age-dependent plasticity control
- **Maturation**: Cognitive function development
- **Critical Periods**: Sensitive learning periods

## Usage Examples

### Basic Brain Simulation

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
# Monitor performance
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Agent-World Integration

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, vision_width=20, vision_height=20)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Main interaction loop
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning Experiment

```python
import pynlm

# Configure for learning experiment
config = pynlm.createDefaultConfig()
config.neuron_count = 1000
config.stdp_ltp_weight = 0.02
config.stdp_ltd_weight = 0.012
config.synaptogenesis_rate = 0.0001

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, vision_width=15, vision_height=15)
agent.initialize(world)

agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Add resources and hazards
world.addObject(pynlm.WorldObject(25, 25, pynlm.WorldObjectType.Resource, 10.0, 2.0))
world.addObject(pynlm.WorldObject(75, 75, pynlm.WorldObjectType.Hazard, 5.0, 2.0))

# Run learning experiment
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, agent.getPredictionError())
```

### Configuration Management

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.neuron_count = 2000
config.region_count = 2
config.connection_probability = 0.1
config.simulation_timestep = 0.001

# Set plasticity parameters
config.stdp_ltp_weight = 0.05
config.stdp_ltd_weight = 0.04
config.stdp_tau = 30.0
config.hebbian_weight = 0.03

# Set development parameters
config.developmental_stage = "Adult"
config.critical_period_end = 100.0

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

## Error Handling

NLM provides comprehensive error handling:

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Performance Considerations

The NLM framework includes performance optimizations:

- **SIMD Vectorization**: Parallel processing of neural operations
- **Memory Pools**: Efficient allocation/deallocation
- **Event-Driven Architecture**: Efficient spike propagation
- **Checkpoint System**: Persistent state management

## Research Applications

NLM supports various research applications:

1. **Artificial Developmental Systems**: Studying emergence of complex behaviors
2. **Memory Research**: Investigating memory formation and consolidation
3. **Plasticity Studies**: Testing different learning rules
4. **Cognitive Architecture**: Exploring perception-action loops
5. **Neuromodulation**: Studying reward and exploration
6. **Development Psychology**: Investigating critical periods

## Testing

Run the test suite:

```bash
python -m pytest tests/ -v
```

Or run specific test modules:

```bash
python -m pytest tests/test_brain.py -v
python -m pytest tests/test_agent.py -v
python -m pytest tests/test_world.py -v
```

## Building from Source

```bash
# Build C++ extensions and Python package
cmake -B build
cmake --build build -j4
pip install -e .
```

## Command Line Interface

The package includes a command-line interface:

```bash
# Show version information
nlm-cli --version

# Show system information
nlm-cli --info

# Run all examples
nlm-cli --examples

# Run demo
nlm-cli demo --steps 100

# Run tests
nlm-cli test
```

## Documentation

- **README.md**: This file - quick start and overview
- **HOW_TO_USE.md**: Detailed usage instructions
- **docs/ARCHITECTURE.md**: System architecture and design
- **docs/SCIENCE.md**: Scientific background and principles

## Contributing

Contributions are welcome! Please see the `CONTRIBUTING.md` file for
development guidelines and contribution process.

## License

NLM is licensed under the MIT License. See the LICENSE file for details.

## Contact

- Website: https://nlm.org
- Documentation: https://docs.nlm.org
- GitHub: https://github.com/nlm-project/nlm
- Email: contact@nlm.org

The NLM project represents a step toward creating artificial systems that
develop and learn in a manner inspired by biological brains.
