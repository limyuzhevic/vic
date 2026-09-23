# NLM (Neural Learning Machine) - Getting Started Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. It implements a brain-inspired spiking neural network simulator that learns from experience through interaction with an environment.

### What NLM Is NOT
- A transformer or large language model
- A chatbot or pretrained AI system
- A deep learning framework
- A pre-built intelligence system

### What NLM IS
- A brain-inspired neural system with neurons and synapses
- A simulator that learns from experience
- An artificial developmental brain that adapts over time
- A research platform for studying neural computation

## Key Concepts

### Neural Architecture
NLM simulates brains using **Leaky Integrate-and-Fire (LIF) neurons** with event-driven spike propagation and multiple plasticity mechanisms:

1. **Neurons**: Basic computational units that integrate inputs and spike when threshold is reached
2. **Synapses**: Connections between neurons that transmit signals and learn from experience
3. **Regions**: Groups of neurons with shared properties (like brain regions)
4. **Populations**: Groups of neurons within regions for specialized function

### Plasticity Mechanisms
NLM implements multiple learning rules:

1. **STDP (Spike-Timing-Dependent Plasticity)**: Learning based on precise spike timing
2. **Hebbian Learning**: Strengthening connections for co-activation
3. **Structural Plasticity**: Formation and pruning of synaptic connections
4. **Reward-Modulated Learning**: Learning modulated by reward signals

### The Brain-Environment Loop
The core architecture follows this loop:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → MOTIVATION → ACTION → WORLD
```

1. The agent senses its environment
2. Sensory input is processed by neural networks
3. Neuromodulation signals guide behavior (curiosity, reward)
4. Action selection produces motor commands
5. Actions affect the world, creating feedback
6. Learning updates synaptic connections

## Project Phases

NLM has evolved through 6 phases of development:

### Phase 1: Foundation
- Core types and configuration system
- Neural interfaces and basic structures
- Simulation framework

### Phase 2: Neural Computation
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

### Phase 3: World Interaction
- World simulation system
- Sensory input and motor output
- Reward prediction error
- Neuromodulation integration
- Developmental stages

### Phase 4: Emerging Cognition
- Neural prediction system
- Working memory
- Episodic memory
- Concept formation
- Neural attention (NOT Transformer QKV)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5: Performance
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration (Current)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Complete integration verified

## Installation

### Prerequisites

- C++ compiler with C++20 support (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)

### Build from Source

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Install Python Package

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Quick Start Examples

### Example 1: Simplest Brain (No World)

```python
import pynlm

# Create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make the brain think for 100 steps
for i in range(100):
    brain.step(i)  # One step of neural computation

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Example 2: Complete Agent in World

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create world environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Example 3: Neural Learning Experiment

```python
import pynlm

# Create brain with plasticity enabled
config = pynlm.createDefaultConfig()
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable plasticity on synapses
if brain.getRegion(0):  # First region
    for synapse in brain.getRegion(0).getSynapses():
        synapse.enablePlasticity(True, True, True)  # STDP, Hebbian, structural

# Record initial state
initial_spikes = brain.getTotalSpikeCount()
initial_weights = []
if brain.getRegion(0):
    for synapse in brain.getRegion(0).getSynapses():
        initial_weights.append(synapse.getWeight())

# Run learning experiment: inject patterns
print("Running learning experiment...")
for step in range(500):
    # Create input patterns: inject current into sensory neurons
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
    
    # Run simulation step
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Check learning occurred
final_spikes = brain.getTotalSpikeCount()
final_weights = []
if brain.getRegion(0):
    for synapse in brain.getRegion(0).getSynapses():
        final_weights.append(synapse.getWeight())

print(f"\nLearning Results:")
print(f"Spikes generated: {final_spikes - initial_spikes}")
print(f"Initial weight mean: {sum(initial_weights)/len(initial_weights) if initial_weights else 0:.3f}")
print(f"Final weight mean: {sum(final_weights)/len(final_weights) if final_weights else 0:.3f}")
print(f"Learning detected: {'Yes' if final_weights else 'No'}")
```

## Key Files and Directories

- `src/`: C++ source code directory
- `python/`: Python bindings implementation
- `configs/`: Configuration files
- `docs/`: Documentation
- `tests/`: Unit tests
- `build/`: Compiled binaries (after building)

## Common Patterns

### Pattern 1: Running a Simulation Loop
```python
def run_simulation(world_steps=100):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(world_steps):
        brain.step(step)
    
    return brain.getTotalSpikeCount()
```

### Pattern 2: Experiment with Learning
```python
# Enable plasticity
agent.enablePlasticity(True)

# Record initial state
initial_state = record_brain_state(brain)

# Run learning episode
for step in range(episode_steps):
    # Let environment affect brain
    world.update(time_step)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), time_step)

# Compare final state to initial
final_state = record_brain_state(brain)
learning_detected = compare_states(initial_state, final_state)
```

## Next Steps

### For Beginners
1. Read the [Easy Usage Guide](easy_usage.md) for quick start examples
2. Read the [HOW_TO_USE.md](HOW_TO_USE.md) for comprehensive tutorials
3. Try the examples in this guide

### For Developers
1. Read the [Python API Documentation](docs/python-api.md) for complete API reference
2. Read the [C++ Developer Guide](docs/cpp-developer.md) for working with source code
3. Read the [Configuration Guide](docs/configuration.md) for configuration options
4. Read the [Testing Guide](docs/testing.md) for running tests and contributing

### For Researchers
1. Read the [Architecture Guide](docs/ARCHITECTURE.md) for technical details
2. Read the [Science.md](docs/SCIENCE.md) for scientific background
3. Read the [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md) for integration results
4. Explore the [EXPERIMENTS.md](docs/EXPERIMENTS.md) for experiment descriptions

## Getting Help

### Documentation
- [README.md](README.md): Project overview and quick summary
- [HOW_TO_USE.md](HOW_TO_USE.md): Comprehensive Python usage guide
- [easy_usage.md](easy_usage.md): Beginner-friendly quick start
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md): System architecture
- [docs/SCIENCE.md](docs/SCIENCE.md): Scientific background

### Support
This is a research project. For support with technical questions, please refer to the documentation. For bugs or issues, please check the repository issues.

### Community
- Explore the documentation in the `docs/` directory
- Experiment with different configurations
- Try creating your own experiments
- Contribute to the documentation if you find it helpful

NLM is designed to be approachable for beginners while offering deep capabilities for advanced researchers and developers. The key is starting simple and gradually building up complexity.