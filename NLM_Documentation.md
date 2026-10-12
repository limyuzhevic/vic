# NLM - Unified Documentation Hub

## Overview

**NLM (Neural Learning Machine)** is an experimental artificial developmental brain simulator that creates neural systems beginning in primitive states and acquiring complex abilities through environmental interaction. NLM is a brain-inspired spiking neural network simulator written in C++20 with Python bindings.

## What NLM IS NOT
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

## What NLM IS
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION** - All previous phases completed and integrated into a coherent artificial brain system.

## Quick Start (15-second Overview)

```python
import pynlm

# Create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

## Complete Agent Example

```python
import pynlm

def run_agent_simulation(world_size=20, num_steps=500):
    """Run a complete agent simulation with learning and development."""
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                   visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Run simulation with progress tracking
    pynlm.run_with_progress(brain, world, agent, num_steps)
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation()
```

## Installation

### Prerequisites
- Python 3.8+
- C++ compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16+

### Development Installation
```bash
pip install -e .
```

### Regular Installation
```bash
pip install pynlm
```

### Build from Source
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..
pip install .
```

## Quick Reference Guide

### Core Classes
| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Brain` | Main neural simulation | `step()`, `initialize()`, `reset()`, `save()`, `load()` |
| `AgentBrain` | Agent-brain interface | `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()` |
| `SimpleWorld` | 2D environment simulation | `update()`, `getSensoryPercept()`, `applyMotorCommand()` |
| `Config` | Configuration management | `loadFromFile()`, `saveToFile()`, `set()`, `getKeys()` |

### Common Patterns

#### Pattern 1: Simple Brain Simulation
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Pattern 2: Complete Agent Simulation
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
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Pattern 3: Simulation with Statistics
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Dev stage: {brain.getDevelopmentalStage()}")
```

## Complete Documentation Sections

### 1. Getting Started
#### What is NLM?
NLM is a brain simulator for computers that lets you create virtual brains that can learn, remember things, and make decisions just like your brain!

#### The 3 Things You Need to Know
1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

### 2. Installation and Setup
#### Building from Source
```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Installing Python Package
```bash
# Install with scikit-build-core (automatic with pip)
pip install .

# Install in development mode
pip install -e .

# Install from built wheel
pip install dist/*.whl
```

### 3. Core Concepts
#### Brain Simulation Loop
```
WORLD → SENSORY INPUT → NEURAL PROCESSING → ACTION → WORLD
```

#### Key Components
- **Neurons**: Computational units that process and transmit information
- **Synapses**: Connections between neurons that transmit signals
- **Learning**: Process of strengthening or weakening connections
- **Memory**: Storage of patterns and experiences

### 4. Advanced Features
#### Learning Subsystems
```python
# Enable learning components
agent.enableCuriosity(True)              # Explore new things
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
```

#### Configuration Options
```python
config = pynlm.createDefaultConfig()

# Customize parameters
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.0)

# Save/load configurations
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")
```

### 5. Development and Debugging
#### Progress Tracking
```python
# Use built-in progress tracking
pynlm.run_with_progress(brain, world, agent, num_steps=1000)
```

#### Validation
```python
# Validate simulation setup
pynlm.validate_simulation_setup(brain, world, agent)
```

#### Checkpointing
```python
# Save simulation state
pynlm.save_checkpoint(brain, "checkpoint_step_100.bin", "Simulation at step 100")
```

### 6. Performance and Monitoring
#### Benchmarking
```python
# Benchmark simulation performance
stats = pynlm.benchmark_simulation(brain, world, agent, num_steps=1000)
print(f"Average time: {stats['average_time']:.3f}s")
print(f"Steps per second: {stats['steps_per_second']:.1f}")
```

### 7. Examples Directory
The examples/ directory contains various demonstration scripts:

- `basic.py`: Basic usage examples
- `agent_simulation.py`: Complete agent example
- `advanced_features.py`: Advanced feature demonstrations
- `performance_tests.py`: Performance benchmarking

### 8. API Documentation
#### Enumerations
| Enum | Values |
|------|--------|
| `NeuronType` | `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal` |
| `DevelopmentalStage` | `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging` |
| `ActionType` | `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, `Look`, `Interact`, etc. |
| `WorldObjectType` | `Empty`, `Resource`, `Hazard`, `Wall`, `Marker` |

#### Error Classes
| Exception | Description |
|-----------|-------------|
| `NLMError` | Base exception for all NLM errors |
| `ConfigurationError` | Configuration-related errors |
| `SimulationError` | Simulation-related errors |
| `ValidationError` | Validation errors |

## Architecture Overview

NLM operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Phase 6 Integration Achievements

### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

## Testing

### Running Tests
```bash
# Install development dependencies
pip install -e .[dev]

# Run unit tests
pytest

# Run tests with coverage
pytest --cov=pynlm --cov-report=html
```

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

#### Build Issues
```bash
# CMake cannot find Python
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

#### Runtime Issues
```bash
# ImportError: No module named 'pynlm'
pip install --force-reinstall .
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance metrics and analysis
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report

## Additional Resources

### Performance Documentation
- See `docs/PERFORMANCE.md` for detailed performance metrics
- Check `docs/SCALING.md` for scaling analysis

### Scientific Background
- Review `docs/SCIENCE.md` for theoretical foundations
- Explore `docs/RESEARCH_METHODS.md` for experimental approaches

### Development Guidelines
- Refer to `docs/ROADMAP.md` for future development plans
- Check `docs/LIMITATIONS.md` for current limitations
- Read `docs/FINAL_LIMITATIONS.md` for Phase 6 specific constraints

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.
